#include <app_verify_ble.h>
#include <string.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/display/Display.h>
#include <ti/drivers/utils/List.h>
#include <ti_drivers_config.h>
#include <icall.h>
#include "util.h"
#include "onboard.h"
#include <bcomdef.h>
#include <icall_ble_api.h>
#include <devinfoservice.h>
#include <simple_gatt_profile.h>
#include "ti_ble_config.h"
#include "simple_peripheral.h"
#include "ble_data_gatt.h"
#include "app_log.h"
#include "app_user.h"


// BLE调试宏定义：启用调试时使用logInfo，否则打印为空
#define BLE_DEBUG         1
#if BLE_DEBUG
#define ble_log(fmt, ...) logInfo(fmt, ##__VA_ARGS__)
#else
#define ble_log(fmt) ((void)0)
#endif


/* 包含必要的头文件 */
#include <ti/sysbios/gates/GateMutexPri.h>
#include <ti/sysbios/knl/Task.h>

/* 声明互斥锁句柄 */
extern GateMutexPri_Handle myGate;
extern GateMutexPri_Params myGateParams;

/* 定义互斥锁变量（放在全局区）*/
GateMutexPri_Handle myGate;
GateMutexPri_Params myGateParams;
IArg key_lock;


// 8字节对齐属性，确保内存对齐满足硬件要求
__attribute__ ((aligned (8)))

// 全局使用的实体ID，用于检查消息的源和/或目标
static ICall_EntityID selfEntity;
// 全局使用的事件，用于发布本地事件并等待系统和本地事件
static ICall_SyncHandle syncEvent;
// 用于应用程序消息的队列对象
static Queue_Struct appMsgQueue;          // 应用程序消息队列结构体
static Queue_Handle appMsgQueueHandle;    // 应用程序消息队列句柄
// 用于内部周期性事件的时钟实例。由于GattServApp会处理通知所有连接的GATT客户端，因此只需要一个
static Clock_Struct clkPeriodic;          // 周期性时钟结构体
// 用于RPA读取事件的时钟实例
static Clock_Struct clkRpaRead;           // RPA读取时钟结构体
// 用于向时钟处理程序传递周期性事件ID的内存
spClockEventData_t argPeriodic =
{ .event = SP_PERIODIC_EVT };             // 周期性事件参数
// 用于向时钟处理程序传递RPA读取事件ID的内存
spClockEventData_t argRpaRead =
{ .event = SP_READ_RPA_EVT };             // RPA读取事件参数
// 每个连接的句柄信息记录
static spConnRec_t connList[MAX_NUM_BLE_CONNS];  // 连接记录列表，最大连接数
// 通过菜单选择的当前连接句柄
static uint16_t menuConnHandle = LINKDB_CONNHANDLE_INVALID;  // 初始化为无效连接句柄
// 用于存储设置PHY命令状态的连接句柄列表
static List_List setPhyCommStatList;      // PHY命令状态列表
// 用于存储排队参数更新的连接句柄列表
static List_List paramUpdateList;         // 参数更新列表
// 用于存储OAD完成后待处理消息数量的变量
// 应用程序在所有待处理消息发送完成前不能重启
static uint8_t numPendingMsgs = 0;        // 待处理消息数量

// 存储在NV中的标志，用于跟踪是否需要在下次启动时发送服务更改指示
static uint32_t  sendSvcChngdOnNextBoot = FALSE;  // 下次启动发送服务更改标志
// 广播句柄
static uint8 advHandleLegacy;             // 传统广播句柄
// 地址模式
static GAP_Addr_Modes_t addrMode = DEFAULT_ADDRESS_MODE;  // 默认地址模式
// 当前随机私有地址
static uint8 rpa[B_ADDR_LEN] = {0};       // 随机私有地址数组，初始化为0


/*********************************************************************
 * @fn      BLE_processGATTMsg
 *
 * @brief   处理GATT消息和事件
 *
 * @param   pMsg - 要处理的消息
 *
 * @return  如果安全释放传入消息返回TRUE，否则返回FALSE
 */
static uint8_t BLE_processGATTMsg(gattMsgEvent_t *pMsg)
{
  // 处理ATT流控制违规事件
  if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT)
  {
    // ATT请求-响应或指示-确认流控制被违反
    // 所有后续的ATT请求或指示将被丢弃
    // 通知应用程序，以便它可以决定是否断开连接

    // 导致违规的消息的操作码
    // Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "FC Violated: %d", pMsg->msg.flowCtrlEvt.opcode);
  }
  // 处理MTU更新事件
  else if (pMsg->method == ATT_MTU_UPDATED_EVENT)
  {
    // MTU大小已更新

    // Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "MTU Size: %d", pMsg->msg.mtuEvt.MTU);
  }

  // 释放消息负载。仅ATT协议消息需要
  GATT_bm_free(&pMsg->msg, pMsg->method);

  // 可以安全释放传入消息
  return (TRUE);
}

/*********************************************************************
 * @fn      BLE_enqueueMsg
 *
 * @brief   创建消息并将消息放入RTOS队列
 *
 * @param   event - 消息事件
 * @param   pData - 消息数据指针
 *
 * @return  成功返回SUCCESS，失败返回FAILURE或bleMemAllocError
 */
static status_t BLE_enqueueMsg(uint8_t event, void *pData)
{
  uint8_t success;
  // 动态分配消息内存
  spEvt_t *pMsg = ICall_malloc(sizeof(spEvt_t));

  // 创建动态消息指针
  if(pMsg)
  {
    pMsg->event = event;
    pMsg->pData =  pData;

    // 将消息加入队列
    success = Util_enqueueMsg(appMsgQueueHandle, syncEvent, (uint8_t *)pMsg);
    return (success) ? SUCCESS : FAILURE;
  }
  return(bleMemAllocError);  // 内存分配错误
}

/*********************************************************************
 * @fn      BLE_pairStateCb
 *
 * @brief   配对状态回调函数
 *
 * @param   connHandle - 连接句柄
 * @param   state - 配对状态
 * @param   status - 状态代码
 *
 * @return  无
 */
static void BLE_pairStateCb(uint16_t connHandle, uint8_t state,
                                         uint8_t status)
{
  // 分配内存存储配对状态数据
  spPairStateData_t *pData = ICall_malloc(sizeof(spPairStateData_t));

  // 为事件数据分配空间
  if (pData)
  {
    pData->state = state;
    pData->connHandle = connHandle;
    pData->status = status;

    // 将配对状态事件加入队列
    if(BLE_enqueueMsg(SP_PAIR_STATE_EVT, pData) != SUCCESS)
    {
      ICall_free(pData);  // 入队失败则释放内存
    }
  }
}

void BLE_send_notify(uint8_t *data, uint16_t len)
{
    bt_ev_data *pData = ICall_malloc(sizeof(bt_ev_data));



    if (pData) {
            pData->data = data;
            pData->len = len;
//            logInfo("##############ble send len = %d", len);
//            put_buf(pData->data, pData->len);
            // 将配对状态事件加入队列
            if(BLE_enqueueMsg(GATT_PROP_NOTIFY, pData) != SUCCESS)
            {
              ICall_free(pData);  // 入队失败则释放内存
            }
    }
}

/*********************************************************************
 * @fn      BLE_passcodeCb
 *
 * @brief   密码回调函数
 *
 * @param   pDeviceAddr - 设备地址指针
 * @param   connHandle - 连接句柄
 * @param   uiInputs - 用户输入
 * @param   uiOutputs - 用户输出
 * @param   numComparison - 比较次数
 *
 * @return  无
 */
static void BLE_passcodeCb(uint8_t *pDeviceAddr,
                                        uint16_t connHandle,
                                        uint8_t uiInputs,
                                        uint8_t uiOutputs,
                                        uint32_t numComparison)
{
  // 分配内存存储密码数据
  spPasscodeData_t *pData = ICall_malloc(sizeof(spPasscodeData_t));

  // 为密码事件分配空间
  if (pData )
  {
    pData->connHandle = connHandle;
    memcpy(pData->deviceAddr, pDeviceAddr, B_ADDR_LEN);  // 复制设备地址
    pData->uiInputs = uiInputs;
    pData->uiOutputs = uiOutputs;
    pData->numComparison = numComparison;

    // 将密码事件加入队列
    if(BLE_enqueueMsg(SP_PASSCODE_EVT, pData) != SUCCESS)
    {
      ICall_free(pData);  // 入队失败则释放内存
    }
  }
}

/*********************************************************************
 * @fn      BLE_clockHandler
 *
 * @brief   时钟超时处理函数
 *
 * @param   arg - 事件类型参数
 *
 * @return  无
 */
static void BLE_clockHandler(UArg arg)
{
  spClockEventData_t *pData = (spClockEventData_t *)arg;

  // 处理周期性事件
  if (pData->event == SP_PERIODIC_EVT)
  {
//      logInfo("SP_PERIODIC_EVT");
    // 启动下一个周期
    Util_startClock(&clkPeriodic);

    // 发布事件唤醒应用程序
    BLE_enqueueMsg(SP_PERIODIC_EVT, NULL);
  }
  // 处理RPA读取事件
  else if (pData->event == SP_READ_RPA_EVT)
  {
    // 启动下一个周期
    Util_startClock(&clkRpaRead);

    // 发布事件读取当前RPA
    BLE_enqueueMsg(SP_READ_RPA_EVT, NULL);
  }
  // 处理参数更新事件
  else if (pData->event == SP_SEND_PARAM_UPDATE_EVT)
  {
    // 发送消息到应用程序
    BLE_enqueueMsg(SP_SEND_PARAM_UPDATE_EVT, pData);
  }
}

/*********************************************************************
 * @fn      BLE_initPHYRSSIArray
 *
 * @brief   初始化用于存储基于RSSI的自动PHY更改相关数据的结构体数组
 *
 * @return  无
 */
static void BLE_initPHYRSSIArray(void)
{
  // 初始化存储连接句柄和RSSI值的数组
  memset(connList, 0, sizeof(connList));
  for (uint8_t index = 0; index < MAX_NUM_BLE_CONNS; index++)
  {
    connList[index].connHandle = SP_INVALID_HANDLE;  // 初始化为无效句柄
  }
}

/*********************************************************************
 * @fn      BLE_getConnIndex
 *
 * @brief   通过连接句柄在已连接设备列表中查找索引
 *
 * @param   connHandle - 连接句柄
 *
 * @return  找到返回索引，未找到返回MAX_NUM_BLE_CONNS
 */
static uint8_t BLE_getConnIndex(uint16_t connHandle)
{
  uint8_t i;

  // 遍历连接列表查找匹配的句柄
  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == connHandle)
    {
      return i;  // 找到返回索引
    }
  }

  return(MAX_NUM_BLE_CONNS);  // 未找到返回最大值
}

/*********************************************************************
 * @fn      BLE_clearConnListEntry
 *
 * @brief   清除连接列表条目
 *
 * @param   connHandle - 要清除的连接句柄（LINKDB_CONNHANDLE_ALL表示清除所有）
 *
 * @return  成功返回SUCCESS，无效范围返回bleInvalidRange
 */
static uint8_t BLE_clearConnListEntry(uint16_t connHandle)
{
  uint8_t i;
  // 初始设置为无效连接索引
  uint8_t connIndex = MAX_NUM_BLE_CONNS;

  // 如果不是清除所有连接
  if(connHandle != LINKDB_CONNHANDLE_ALL)
  {
    // 从句柄获取连接索引
    connIndex = BLE_getConnIndex(connHandle);
    if(connIndex >= MAX_NUM_BLE_CONNS)
    {
      return(bleInvalidRange);  // 无效范围
    }
  }

  // 清除特定句柄或所有句柄
  for(i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if((connIndex == i) || (connHandle == LINKDB_CONNHANDLE_ALL))
    {
      // 重置连接信息
      connList[i].connHandle = LINKDB_CONNHANDLE_INVALID;
      connList[i].currPhy = 0;
      connList[i].phyCngRq = 0;
      connList[i].phyRqFailCnt = 0;
      connList[i].rqPhy = 0;
      memset(connList[i].rssiArr, 0, SP_MAX_RSSI_STORE_DEPTH);
      connList[i].rssiAvg = 0;
      connList[i].rssiCntr = 0;
      connList[i].isAutoPHYEnable = FALSE;
    }
  }

  return(SUCCESS);  // 操作成功
}



/*********************************************************************
 * @fn      BLE_init
 *
 * @brief   初始化函数，在初始化期间调用，包含应用程序特定的初始化
 *
 * @return  无
 */
static void BLE_init(void)
{
    // GAP绑定管理器回调
    static gapBondCBs_t BLE_BondMgrCBs = {
      BLE_passcodeCb,       // 密码回调
      BLE_pairStateCb       // 配对/绑定状态回调
    };


  // ******************************************************************
  // 在调用ICall_registerApp之前不能发生任何STACK API调用
  // ******************************************************************
  // 注册当前线程作为ICall调度应用程序，以便应用程序可以发送和接收消息
  ICall_registerApp(&selfEntity, &syncEvent);

  // 创建RTOS队列用于从配置文件发送到应用程序的消息
  appMsgQueueHandle = Util_constructQueue(&appMsgQueue);

  // 创建内部周期性事件的一次性时钟
  Util_constructClock(&clkPeriodic, BLE_clockHandler,
                      SP_PERIODIC_EVT_PERIOD, 0, false, (UArg)&argPeriodic);



  // 在GAP GATT服务中设置设备名称特征
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, attDeviceName);

  // 配置GAP
  {
    uint16_t paramUpdateDecision = DEFAULT_PARAM_UPDATE_REQ_DECISION;

    // 将所有参数更新请求传递给应用程序决定
    GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, paramUpdateDecision);
  }

  // 设置GAP绑定管理器
  setBondManagerParameters();

  // 初始化GATT属性
  GGS_AddService(GAP_SERVICE);                 // GAP GATT服务
  GATTServApp_AddService(GATT_ALL_SERVICES);   // GATT服务

  Ble_GATT_User_Init();

  // 注册简单GATT配置文件回调
//  SimpleProfile_RegisterAppCBs(&BLE_simpleProfileCBs);
  // 启动绑定管理器并注册回调
  VOID GAPBondMgr_Register(&BLE_BondMgrCBs);
  // 注册GAP以接收HCI/主机消息
  GAP_RegisterForMsgs(selfEntity);
  // 注册GATT本地事件和待传输的ATT响应
  GATT_RegisterForMsgs(selfEntity);
  // 设置数据长度扩展的默认值
  {
    // 设置初始值为最大值，RX默认设置为最大值(251字节，2120us)
    #define APP_SUGGESTED_PDU_SIZE 251 // 默认27字节(TX)
    #define APP_SUGGESTED_TX_TIME 2120 // 默认328us(TX)

    // 发送设置数据长度命令
    HCI_LE_WriteSuggestedDefaultDataLenCmd(APP_SUGGESTED_PDU_SIZE, APP_SUGGESTED_TX_TIME);
  }
  // 初始化GATT客户端
  GATT_InitClient();
  // 初始化连接列表
  BLE_clearConnListEntry(LINKDB_CONNHANDLE_ALL);
  // 初始化外设角色的GAP层并注册接收GAP事件
  GAP_DeviceInit(GAP_PROFILE_PERIPHERAL, selfEntity, addrMode, &pRandomAddress);
  // 初始化存储连接句柄和RSSI值的数组
  BLE_initPHYRSSIArray();
}

/*********************************************************************
 * @fn      BLE_stopAutoPhyChange
 *
 * @brief   取消链路上的周期性RSSI读取
 *
 * @param   connHandle - 链接的连接句柄
 *
 * @return  成功返回SUCCESS，无链接返回bleIncorrectMode
 */
static status_t BLE_stopAutoPhyChange(uint16_t connHandle)
{
  // 从句柄获取连接索引
  uint8_t connIndex = BLE_getConnIndex(connHandle);
  BLE_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

  // 停止连接事件通知
  Gap_RegisterConnEventCb(NULL, GAP_CB_UNREGISTER, GAP_CB_CONN_EVENT_ALL, connHandle);

  // 更新活动RSSI跟踪连接的PHY更改请求状态
  connList[connIndex].phyCngRq = FALSE;
  connList[connIndex].isAutoPHYEnable = FALSE;

  return SUCCESS;
}

/*********************************************************************
 * @fn      BLE_removeConn
 *
 * @brief   从已连接设备列表中移除设备
 *
 * @param   connHandle - 要移除的连接句柄
 *
 * @return  被移除连接信息的索引，未找到返回MAX_NUM_BLE_CONNS
 */
static uint8_t BLE_removeConn(uint16_t connHandle)
{
  uint8_t connIndex = BLE_getConnIndex(connHandle);

  if(connIndex != MAX_NUM_BLE_CONNS)
  {
    Clock_Struct* pUpdateClock = connList[connIndex].pUpdateClock;

    // 处理参数更新时钟
    if (pUpdateClock != NULL)
    {
      // 如果时钟仍处于活动状态，停止并销毁RTOS时钟
      if (Util_isActive(pUpdateClock))
      {
        Util_stopClock(pUpdateClock);
      }

      // 销毁时钟对象
      Clock_destruct(pUpdateClock);
      // 释放时钟结构内存
      ICall_free(pUpdateClock);
    }

    // 停止自动PHY更改
    BLE_stopAutoPhyChange(connHandle);
    // 清除连接列表条目
    BLE_clearConnListEntry(connHandle);
  }

  return connIndex;
}

/*********************************************************************
 * @fn      BLE_connEvtCB
 *
 * @brief   连接事件回调函数
 *
 * @param   pReport - 指向连接事件报告的指针
 *
 * @return  无
 */
static void BLE_connEvtCB(Gap_ConnEventRpt_t *pReport)
{
  // 将事件加入队列以便在应用程序上下文中处理
  if(BLE_enqueueMsg(SP_CONN_EVT, pReport) != SUCCESS)
  {
    ICall_free(pReport);  // 入队失败则释放内存
  }
}
void BLE_disconnEvtCB(void)
{
    // 动态分配消息内存
    spEvt_t *pMsg = ICall_malloc(sizeof(spEvt_t));

    if(BLE_enqueueMsg(SP_DISCONNECT_EVT, (uint8_t *)pMsg) != SUCCESS)
      {
        ICall_free(pMsg);  // 入队失败则释放内存
      }
}
/*********************************************************************
 * @fn      BLE_startAutoPhyChange
 *
 * @brief   在链路上启动周期性RSSI读取
 *
 * @param   connHandle - 链接的连接句柄
 *
 * @return  成功返回SUCCESS，无链接返回bleIncorrectMode，无资源返回bleNoResources
 */
static status_t BLE_startAutoPhyChange(uint16_t connHandle)
{
  status_t status = FAILURE;

  // 从句柄获取连接索引
  uint8_t connIndex = BLE_getConnIndex(connHandle);
  BLE_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

  // 启动连接事件通知以进行RSSI计算
  Gap_RegisterConnEventCb(BLE_connEvtCB, GAP_CB_REGISTER, GAP_CB_CONN_EVENT_ALL, connHandle);

  // 如果成功，在连接信息中设置标志
  if (status == SUCCESS)
  {
    connList[connIndex].isAutoPHYEnable = TRUE;
  }

  return status;
}

/*********************************************************************
 * @fn      BLE_setPhy
 *
 * @brief   调用HCI设置PHY API并将句柄添加到列表中以匹配传入的命令状态事件
 *
 * @param   connHandle - 连接句柄
 * @param   allPhys - 所有PHY
 * @param   txPhy - 发送PHY
 * @param   rxPhy - 接收PHY
 * @param   phyOpts - PHY选项
 *
 * @return  成功返回SUCCESS
 */
static status_t BLE_setPhy(uint16_t connHandle, uint8_t allPhys,
                                        uint8_t txPhy, uint8_t rxPhy,
                                        uint16_t phyOpts)
{
  // 分配列表条目以存储命令状态的句柄
  spConnHandleEntry_t *connHandleEntry = ICall_malloc(sizeof(spConnHandleEntry_t));

  if (connHandleEntry)
  {
    connHandleEntry->connHandle = connHandle;

    // 将条目添加到PHY命令状态列表
    List_put(&setPhyCommStatList, (List_Elem *)connHandleEntry);

    // 发送PHY更新命令
    HCI_LE_SetPhyCmd(connHandle, allPhys, txPhy, rxPhy, phyOpts);
  }

  return SUCCESS;
}

/*********************************************************************
 * @fn      BLE_doSetConnPhy
 *
 * @brief   设置PHY偏好
 *
 * @param   index - PHY选项索引:
 *                  0: 1M PHY
 *                  1: 2M PHY
 *                  2: 1M + 2M PHY
 *                  3: CODED PHY (长距离)
 *                  4: 1M + 2M + CODED PHY
 *
 * @return  总是返回true
 */
bool BLE_doSetConnPhy(uint8 index)
{
  bool status = TRUE;

  // PHY选项数组
  static uint8_t phy[] = {
    HCI_PHY_1_MBPS, HCI_PHY_2_MBPS, HCI_PHY_1_MBPS | HCI_PHY_2_MBPS,
    HCI_PHY_CODED, HCI_PHY_1_MBPS | HCI_PHY_2_MBPS | HCI_PHY_CODED,
    AUTO_PHY_UPDATE
  };

  // 获取当前连接的索引
  uint8_t connIndex = BLE_getConnIndex(menuConnHandle);
  BLE_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

  // 在当前连接上设置PHY偏好。对RX和TX应用相同的值
  // 如果未选择自动PHY更新且启用了自动PHY更新，则停止自动PHY更新
  if(phy[index] != AUTO_PHY_UPDATE)
  {
    // 取消RSSI读取和自动PHY更改
    BLE_stopAutoPhyChange(connList[connIndex].connHandle);

    // 设置PHY
    BLE_setPhy(menuConnHandle, 0, phy[index], phy[index], 0);

    // Display_printf(dispHandle, SP_ROW_STATUS_1, 0, "PHY preference: %s",
    //                TBM_GET_ACTION_DESC(&spMenuConnPhy, index));
  }
  else
  {
    // 启动RSSI读取以进行自动PHY更新（如果已禁用）
    BLE_startAutoPhyChange(menuConnHandle);
  }

  return status;
}

/*********************************************************************
 * @fn      BLE_advCallback
 *
 * @brief   GapAdv模块回调函数
 *
 * @param   event - 事件类型
 * @param   pBuf - 数据缓冲区
 * @param   arg - 参数
 *
 * @return  无
 */
static void BLE_advCallback(uint32_t event, void *pBuf, uintptr_t arg)
{
  // 分配内存存储广播事件数据
  spGapAdvEventData_t *pData = ICall_malloc(sizeof(spGapAdvEventData_t));

  if (pData)
  {
    pData->event = event;
    pData->pBuf = pBuf;

    // 将广播事件加入队列
    if(BLE_enqueueMsg(SP_ADV_EVT, pData) != SUCCESS)
    {
      ICall_free(pData);  // 入队失败则释放内存
    }
  }
}

/*********************************************************************
 * @fn      BLE_processAdvEvent
 *
 * @brief   在应用程序上下文中处理广播事件
 *
 * @param   pEventData - 事件数据指针
 *
 * @return  无
 */
static void BLE_processAdvEvent(spGapAdvEventData_t *pEventData)
{
  switch (pEventData->event)
  {
    case GAP_EVT_ADV_START_AFTER_ENABLE:
        ble_log("Adv Set %d Enabled", *(uint8_t *)(pEventData->pBuf));
      break;

    case GAP_EVT_ADV_END_AFTER_DISABLE:
        ble_log("Adv Set %d Disabled", *(uint8_t *)(pEventData->pBuf));
      break;

    case GAP_EVT_ADV_START:
      break;

    case GAP_EVT_ADV_END:
      break;

    case GAP_EVT_ADV_SET_TERMINATED:
    {
      #ifndef Display_DISABLE_ALL
      GapAdv_setTerm_t *advSetTerm = (GapAdv_setTerm_t *)(pEventData->pBuf);
      ble_log("Adv Set %d disabled after conn %d", advSetTerm->handle, advSetTerm->connHandle );
      #endif
    }
    break;

    case GAP_EVT_SCAN_REQ_RECEIVED:
      break;

    case GAP_EVT_INSUFFICIENT_MEMORY:
      break;

    default:
      break;
  }

  // 除了内存不足事件外，所有事件都有相关的内存需要释放
  if (pEventData->event != GAP_EVT_INSUFFICIENT_MEMORY)
  {
    ICall_free(pEventData->pBuf);
  }
}

/*********************************************************************
 * @fn      BLE_doSelectConn
 *
 * @brief   选择要通信的连接
 *
 * @param   index - 菜单中的项目索引
 *
 * @return  总是返回true
 */
bool BLE_doSelectConn(uint8_t index)
{
  // 设置当前菜单选择的连接句柄
  menuConnHandle = connList[index].connHandle;
  return (true);
}

void log_hex_data_no_newline(const char* prefix, const uint8_t* data, uint16_t length)
{
    char buffer[256]; // 增大缓冲区，因为每个字节需要更多字符
    int offset = snprintf(buffer, sizeof(buffer), "%s", prefix);

    for (int i = 0; i < length; i++) {
        offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                          "%s[%d]0x%02X", (i > 0 ? ", " : ""), i, data[i]);
    }

    ble_log("%s", buffer);
}
/*********************************************************************
 * @fn      BLE_processCharValueChangeEvt
 *
 * @brief   处理待处理的简单配置文件特征值更改事件
 *
 * @param   paramId - 被更改的参数ID
 *
 * @return  无
 */
static void BLE_processCharValueChangeEvt(uint8_t paramId, uint16_t len)
{
  uint8_t newValue[128];

  switch(paramId)
  {
//    case SIMPLEPROFILE_CHAR1:
////      SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR1, newValue, len);
//      log_hex_data_no_newline("get data = ", newValue, len);
//      break;
//
//    case SIMPLEPROFILE_CHAR3:
////      SimpleProfile_GetParameter(SIMPLEPROFILE_CHAR3, newValue, len);
//      ble_log("Char 3: %d", newValue[0]);
//      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      BLE_performPeriodicTask
 *
 * @brief   执行周期性应用程序任务。此函数每五秒调用一次。
 *          在此示例中，从SimpleGATTProfile服务检索第三个特征的值，
 *          然后复制到第四个特征的值中。
 *
 * @return  无
 */
static void BLE_performPeriodicTask(void)
{
    uint8_t valueToCopy;
//   UartProfile_NotifySenddata(NULL, 0);  //定时发射订阅数据  数据在TxNotifyValue
}

/*********************************************************************
 * @fn      BLE_updateRPA
 *
 * @brief   更新随机私有地址
 *
 * @return  无
 */
static void BLE_updateRPA(void)
{
  // 读取当前RPA。
  // 调用HCI_LE_ReadLocalResolvableAddressCmd的参数不需要精确即可检索本地可解析地址。
  // 第一个参数可以是ADDRMODE_PUBLIC和ADDRMODE_RANDOM中的任何一个。
  // 第二个参数只需要不为NULL。
  // 结果将随HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS完成事件一起到来。
  HCI_LE_ReadLocalResolvableAddressCmd(0, rpa);
}

/*********************************************************************
 * @fn      BLE_processPairState
 *
 * @brief   处理新的配对状态
 *
 * @param   pPairData - 配对状态数据指针
 *
 * @return  无
 */
static void BLE_processPairState(spPairStateData_t *pPairData)
{
  uint8_t state = pPairData->state;
  uint8_t status = pPairData->status;

  switch (state)
  {
    case GAPBOND_PAIRING_STATE_STARTED:
        ble_log("Pairing started");
      break;

    case GAPBOND_PAIRING_STATE_COMPLETE:
      if (status == SUCCESS) {
          ble_log("Pairing success");
      } else {
          ble_log("Pairing fail: %d", status);
      }
      break;

    case GAPBOND_PAIRING_STATE_ENCRYPTED:
      if (status == SUCCESS) {
          ble_log("Encryption success");
      } else {
          ble_log("Encryption failed: %d", status);
      }
      break;

    case GAPBOND_PAIRING_STATE_BOND_SAVED:
      if (status == SUCCESS) {
          ble_log("Bond save success");
      } else {
          ble_log("Bond save failed: %d", status);
      }
      break;

    default:
      break;
  }
}

/*********************************************************************
 * @fn      BLE_processPasscode
 *
 * @brief   处理密码请求
 *
 * @param   pPasscodeData - 密码数据指针
 *
 * @return  无
 */
static void BLE_processPasscode(spPasscodeData_t *pPasscodeData)
{
  // 向用户显示密码
  if (pPasscodeData->uiOutputs != 0) {
      ble_log("Passcode: %d", B_APP_DEFAULT_PASSCODE);
  }

  // 发送密码响应
  GAPBondMgr_PasscodeRsp(pPasscodeData->connHandle , SUCCESS, B_APP_DEFAULT_PASSCODE);
}

/*********************************************************************
 * @fn      BLE_addConn
 *
 * @brief   向已连接设备列表添加设备
 *
 * @param   connHandle - 连接句柄
 *
 * @return  成功返回SUCCESS，无空间返回bleNoResources
 */
static uint8_t BLE_addConn(uint16_t connHandle)
{
  uint8_t i;
  uint8_t status = bleNoResources;

  // 尝试找到可用条目
  for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
  {
    if (connList[i].connHandle == LINKDB_CONNHANDLE_INVALID)
    {
      #ifdef DEFAULT_SEND_PARAM_UPDATE_REQ
      spClockEventData_t *paramUpdateEventData;

      // 分配通过时钟处理程序发送的数据
      paramUpdateEventData = ICall_malloc(sizeof(spClockEventData_t) + sizeof (uint16_t));
      if(paramUpdateEventData)
      {
        paramUpdateEventData->event = SP_SEND_PARAM_UPDATE_EVT;
        *((uint16_t *)paramUpdateEventData->data) = connHandle;

        // 创建时钟对象并启动
        connList[i].pUpdateClock = (Clock_Struct*) ICall_malloc(sizeof(Clock_Struct));

        if (connList[i].pUpdateClock)
        {
          Util_constructClock(connList[i].pUpdateClock, BLE_clockHandler,
                              SEND_PARAM_UPDATE_DELAY, 0, true, (UArg) paramUpdateEventData);
        }
      }
      else
      {
        status = bleMemAllocError;
      }
      #endif

      // 找到可用条目放置新连接信息
      connList[i].connHandle = connHandle;

      // 设置默认PHY为1M
      connList[i].currPhy = HCI_PHY_1_MBPS;

      break;
    }
  }

  return status;
}

/*********************************************************************
 * @fn      SimpleCentral_processCmdCompleteEvt
 *
 * @brief   处理传入的OSAL HCI命令完成事件
 *
 * @param   pMsg - 要处理的消息
 *
 * @return  无
 */
static void BLE_processCmdCompleteEvt(hciEvt_CmdComplete_t *pMsg)
{
  uint8_t status = pMsg->pReturnParam[0];

  // 查找此命令完成是针对哪个命令的
  switch (pMsg->cmdOpcode)
  {
    case HCI_READ_RSSI:  // RSSI读取命令
    {
      // 显示RSSI值，如果RSSI高于阈值，切换到更快的PHY
      if (status == SUCCESS)
      {
        uint16_t handle = BUILD_UINT16(pMsg->pReturnParam[1], pMsg->pReturnParam[2]);

        uint8_t index = BLE_getConnIndex(handle);
        BLE_ASSERT(index < MAX_NUM_BLE_CONNS);

        // 存储RSSI值到数组
        connList[index].rssiArr[connList[index].rssiCntr++] = (int8_t)pMsg->pReturnParam[3];
        connList[index].rssiCntr %= SP_MAX_RSSI_STORE_DEPTH;

        // 计算平均RSSI
        int16_t sum_rssi = 0;
        for(uint8_t cnt=0; cnt<SP_MAX_RSSI_STORE_DEPTH; cnt++)
        {
          sum_rssi += connList[index].rssiArr[cnt];
        }
        connList[index].rssiAvg = (uint32_t)(sum_rssi/SP_MAX_RSSI_STORE_DEPTH);

        ble_log ("RSSI:-%d, AVG RSSI:-%d",(uint32_t)(-(int8_t)pMsg->pReturnParam[3]), (uint32_t)(-sum_rssi/SP_MAX_RSSI_STORE_DEPTH));

        uint8_t phyRq = SP_PHY_NONE;
        uint8_t phyRqS = SP_PHY_NONE;
        uint8_t phyOpt = LL_PHY_OPT_NONE;

        // 根据RSSI值决定PHY切换策略
        if(connList[index].phyCngRq == FALSE)
        {
          if((connList[index].rssiAvg >= RSSI_2M_THRSHLD) &&
             (connList[index].currPhy != HCI_PHY_2_MBPS) &&
             (connList[index].currPhy != SP_PHY_NONE))
          {
            // 尝试切换到更高数据速率
            phyRqS = phyRq = HCI_PHY_2_MBPS;
          }
          else if((connList[index].rssiAvg < RSSI_2M_THRSHLD) &&
                  (connList[index].rssiAvg >= RSSI_1M_THRSHLD) &&
                  (connList[index].currPhy != HCI_PHY_1_MBPS) &&
                  (connList[index].currPhy != SP_PHY_NONE))
          {
            // 尝试切换到传统常规数据速率
            phyRqS = phyRq = HCI_PHY_1_MBPS;
          }
          else if((connList[index].rssiAvg >= RSSI_S2_THRSHLD) &&
                  (connList[index].rssiAvg < RSSI_1M_THRSHLD) &&
                  (connList[index].currPhy != SP_PHY_NONE))
          {
            // 尝试切换到较低数据速率 S=2(500kb/s)
            phyRqS = HCI_PHY_CODED;
            phyOpt = LL_PHY_OPT_S2;
            phyRq = BLE5_CODED_S2_PHY;
          }
          else if(connList[index].rssiAvg < RSSI_S2_THRSHLD )
          {
            // 尝试切换到最低数据速率 S=8(125kb/s)
            phyRqS = HCI_PHY_CODED;
            phyOpt = LL_PHY_OPT_S8;
            phyRq = BLE5_CODED_S8_PHY;
          }

          // 发起基于RSSI的PHY更改
          if((phyRq != SP_PHY_NONE) &&
             (((connList[index].rqPhy == phyRq) && (connList[index].phyRqFailCnt < 2)) ||
              (connList[index].rqPhy != phyRq)))
          {
            BLE_setPhy(connList[index].connHandle, 0, phyRqS, phyRqS, phyOpt);
            connList[index].phyCngRq = TRUE;

            // 如果是与失败请求不同的PHY请求，重置计数
            if(connList[index].rqPhy != phyRq)
            {
              connList[index].phyRqFailCnt = 0;
            }

            // 记录请求的PHY
            if(phyOpt == LL_PHY_OPT_NONE)
            {
              connList[index].rqPhy = phyRq;
            }
            else if(phyOpt == LL_PHY_OPT_S2)
            {
              connList[index].rqPhy = BLE5_CODED_S2_PHY;
            }
            else
            {
              connList[index].rqPhy = BLE5_CODED_S8_PHY;
            }
          }
        }
      }
      break;
    }

    case HCI_LE_READ_PHY:  // 读取PHY命令
    {
      if (status == SUCCESS)
      {
        // Display_printf(dispHandle, SP_ROW_RSSI + 2, 0, "RXPh: %d, TXPh: %d",
        //                pMsg->pReturnParam[3], pMsg->pReturnParam[4]);
      }
      break;
    }

    case HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS:  // 读取本地可解析地址命令
    {
      uint8_t* pRpaNew = &(pMsg->pReturnParam[1]);

      // 如果RPA已更改，更新显示
      if (memcmp(pRpaNew, rpa, B_ADDR_LEN))
      {
        // Display_printf(dispHandle, SP_ROW_RPA, 0, "RP Addr: %s",
        //                Util_convertBdAddr2Str(pRpaNew));
        memcpy(rpa, pRpaNew, B_ADDR_LEN);  // 更新RPA
      }
      break;
    }

    default:
      break;
  }
}

/*********************************************************************
 * @fn      BLE_processParamUpdate
 *
 * @brief   处理参数更新
 *
 * @param   connHandle - 连接句柄
 *
 * @return  无
 */
static void BLE_processParamUpdate(uint16_t connHandle)
{
  gapUpdateLinkParamReq_t req;
  uint8_t connIndex;

  req.connectionHandle = connHandle;
  #ifdef DEFAULT_SEND_PARAM_UPDATE_REQ
  req.connLatency = DEFAULT_DESIRED_PERIPHERAL_LATENCY;
  req.connTimeout = DEFAULT_DESIRED_CONN_TIMEOUT;
  req.intervalMin = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
  req.intervalMax = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
  #endif

  connIndex = BLE_getConnIndex(connHandle);
  BLE_ASSERT(connIndex < MAX_NUM_BLE_CONNS);

  // 销毁时钟对象
  Clock_destruct(connList[connIndex].pUpdateClock);
  // 释放时钟结构内存
  ICall_free(connList[connIndex].pUpdateClock);
  connList[connIndex].pUpdateClock = NULL;

  // 发送参数更新请求
  bStatus_t status = GAP_UpdateLinkParamReq(&req);

  // 如果有正在进行的更新，将此更新排队等待更新完成
  if (status == bleAlreadyInRequestedMode)
  {
    spConnHandleEntry_t *connHandleEntry = ICall_malloc(sizeof(spConnHandleEntry_t));
    if (connHandleEntry)
    {
      connHandleEntry->connHandle = connHandle;
      List_put(&paramUpdateList, (List_Elem *)&connHandleEntry);
    }
  }
}

/*********************************************************************
 * @fn      BLE_processConnEvt
 *
 * @brief   处理连接事件
 *
 * @param   pReport - 连接事件报告指针
 *
 * @return  无
 */
static void BLE_processConnEvt(Gap_ConnEventRpt_t *pReport)
{

  {
    // 从句柄获取索引
    uint8_t connIndex = BLE_getConnIndex(pReport->handle);

    // 如果启用了自动PHY更改
    if (connList[connIndex].isAutoPHYEnable == TRUE)
    {
      // 读取RSSI
      HCI_ReadRssiCmd(pReport->handle);
    }
  }
}


/*********************************************************************
 * @fn      BLE_processAppMsg
 *
 * @brief   处理来自配置文件的传入回调
 *
 * @param   pMsg - 要处理的消息
 *
 * @return  无
 */
static void BLE_processAppMsg(spEvt_t *pMsg)
{
     bt_ev_data *in_data;
  bool dealloc = TRUE;  // 默认安全释放内存
//  ble_log("BLE_processAppMsg");
  switch (pMsg->event)
  {
    case SP_CHAR_CHANGE_EVT:  // 特征值改变事件
//      BLE_processCharValueChangeEvt(*(uint8_t*)(pMsg->pData), pMsg->pData_len);
      break;

    case SP_KEY_CHANGE_EVT:  // 按键改变事件
      // BLE_handleKeys(*(uint8_t*)(pMsg->pData));
      break;

    case SP_ADV_EVT:  // 广播事件
      BLE_processAdvEvent((spGapAdvEventData_t*)(pMsg->pData));
      break;

    case SP_PAIR_STATE_EVT:  // 配对状态事件
      BLE_processPairState((spPairStateData_t*)(pMsg->pData));
      break;

    case SP_PASSCODE_EVT:  // 密码事件
      BLE_processPasscode((spPasscodeData_t*)(pMsg->pData));
      break;

    case SP_PERIODIC_EVT:  // 周期性事件
      BLE_performPeriodicTask();
      break;

    case SP_READ_RPA_EVT:  // 读取RPA事件
      BLE_updateRPA();
      break;

    case GATT_PROP_NOTIFY:  // 发送数据事件
       /* 离开临界区，释放互斥锁 */

       UartProfile_NotifySenddata( (bt_ev_data *)pMsg->pData);

      break;
    case SP_SEND_PARAM_UPDATE_EVT:  // 发送参数更新事件
    {
      // 从数据中提取连接句柄
      uint16_t connHandle = *(uint16_t *)(((spClockEventData_t *)pMsg->pData)->data);
      BLE_processParamUpdate(connHandle);
      dealloc = FALSE;  // 此数据不是动态分配的
      break;
    }

    case SP_CONN_EVT:  // 连接事件
      BLE_processConnEvt((Gap_ConnEventRpt_t *)(pMsg->pData));
      break;
    case SP_DISCONNECT_EVT:  // 断开连接事件
      BLE_disconnectAll();
      break;

    default:
      // 什么都不做
      break;
  }

  // 如果存在消息数据且需要释放，则释放内存
  if ((dealloc == TRUE) && (pMsg->pData != NULL))
  {
    ICall_free(pMsg->pData);
  }
}

/*********************************************************************
 * @fn      BLE_disconnectAll
 *
 * @brief   断开所有蓝牙连接
 *
 * @return  状态码
 */
bStatus_t BLE_disconnectAll(void)
{
    bStatus_t status = SUCCESS;
    uint8_t numActive = linkDB_NumActive();

    if (numActive == 0) {
        ble_log("No active connections to disconnect");
        return SUCCESS;
    }

    ble_log("Disconnecting %d active connections", numActive);

    // 遍历所有连接并断开
    for (uint8_t i = 0; i < MAX_NUM_BLE_CONNS; i++) {
        if (connList[i].connHandle != LINKDB_CONNHANDLE_INVALID) {
            bStatus_t tempStatus = GAP_TerminateLinkReq(connList[i].connHandle,
                                                       HCI_DISCONNECT_REMOTE_USER_TERM);
            if (tempStatus != SUCCESS) {
                status = tempStatus; // 记录最后一个错误状态
                ble_log("Failed to disconnect handle 0x%04X: 0x%02X",
                       connList[i].connHandle, tempStatus);
            }
        }
    }

    return status;
}

/*********************************************************************
 * @fn      BLE_processGapMessage
 *
 * @brief   处理传入的GAP事件
 *
 * @param   pMsg - 要处理的消息
 *
 * @return  无
 */
uint8_t newAdvData[31]={0};
uint8_t user_scan_data[31] = {0};

uint8_t user_scan_rsp_data[31] = {
    0x06, 0xff, 0x52, 0x46, 0x0c, 0xc8, 0x07, 0x0c,  // 前8字节：广播包头和其他数据
    0x09, 0x52, 0x46, 0x73, 0x74, 0x61, 0x72, 0x5F,  // 设备名："RFstar_"
    0x37, 0x36, 0x44, 0x41,                          // 设备名："76DA"
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // 填充数据
    0x00, 0x00, 0x00
};
static void BLE_processGapMessage(gapEventHdr_t *pMsg)
{
    ble_log("BLE_processGapMessage");
  switch(pMsg->opcode)
  {
    case GAP_DEVICE_INIT_DONE_EVENT:  // 设备初始化完成事件
    {
      gapDeviceInitDoneEvent_t *pPkt = (gapDeviceInitDoneEvent_t *)pMsg;

      if(pPkt->hdr.status == SUCCESS)
      {
        // 存储系统ID
        uint8_t systemId[8];

        // 使用设备地址的6个字节作为系统ID值的8个字节
        systemId[0] = pPkt->devAddr[0];
        systemId[1] = pPkt->devAddr[1];
        systemId[2] = pPkt->devAddr[2];
        systemId[3] = 0x00;
        systemId[4] = 0x00;              // 中间字节设置为零
        systemId[5] = pPkt->devAddr[3];
        systemId[6] = pPkt->devAddr[4];
        systemId[7] = pPkt->devAddr[5];  // 向上移动三个字节

        ble_log("start GapAdv");

        // 设置并启动广播
        bStatus_t status = FAILURE;

        // 创建广播集#1并分配句柄
        status = GapAdv_create(&BLE_advCallback, &advParams1, &advHandleLegacy);
        BLE_ASSERT(status == SUCCESS);

        //保存蓝牙地址用于应用层使用
        user_save_ble_mac_addr(pPkt->devAddr);

        uint8_t advData_len = sizeof(advData1);
        advData1[advData_len-1] = pPkt->devAddr[0];
        advData1[advData_len-2] = pPkt->devAddr[1];
        advData1[advData_len-3] = pPkt->devAddr[2];
        advData1[advData_len-4] = pPkt->devAddr[3];
        advData1[advData_len-5] = pPkt->devAddr[4];
        advData1[advData_len-6] = pPkt->devAddr[5];



        status = GapAdv_loadByHandle(advHandleLegacy, GAP_ADV_DATA_TYPE_ADV,sizeof(advData1), advData1);  //创建广播
        status = GapAdv_loadByHandle(advHandleLegacy, GAP_ADV_DATA_TYPE_SCAN_RSP, sizeof(scanResData1), scanResData1); //创建广播应答

        BLE_ASSERT(status == SUCCESS);

        // 为集#1设置事件掩码
        status = GapAdv_setEventMask(advHandleLegacy,
                                     GAP_ADV_EVT_MASK_START_AFTER_ENABLE |
                                     GAP_ADV_EVT_MASK_END_AFTER_DISABLE |
                                     GAP_ADV_EVT_MASK_SET_TERMINATED);

        // 启用集#1的传统广播
        status = GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
        BLE_ASSERT(status == SUCCESS);
        // 显示设备地址
        ble_log("%s Addr: %s",(addrMode <= ADDRMODE_RANDOM) ? "Dev" : "ID", Util_convertBdAddr2Str(pPkt->devAddr));

        // 如果使用解析地址模式，启动RPA检查
        if (addrMode > ADDRMODE_RANDOM)
        {
          BLE_updateRPA();

          // 创建RPA检查事件的一次性时钟
          Util_constructClock(&clkRpaRead, BLE_clockHandler,
                              READ_RPA_PERIOD, 0, true, (UArg) &argRpaRead);
        }
        // 蓝牙模块连接状态初始化
        user_send_mailbox_ble_state(APP_CONNECT_STATE_STARTED);

      }
      break;
    }

    case GAP_LINK_ESTABLISHED_EVENT:  // 链接建立事件
    {
      gapEstLinkReqEvent_t *pPkt = (gapEstLinkReqEvent_t *)pMsg;

      // 显示当前连接数量
      uint8_t numActive = linkDB_NumActive();
      ble_log("Num Conns: %d", (uint16_t)numActive);

      if (pPkt->hdr.status == SUCCESS) {
        // 将连接添加到列表并启动RSSI
        BLE_addConn(pPkt->connectionHandle);
        // 显示此连接的地址
        ble_log("Connected to %s", Util_convertBdAddr2Str(pPkt->devAddr));
        // 启动周期性时钟
        Util_startClock(&clkPeriodic);
        // 备注 安卓端需要手动修改MTU 数据包长度
        attExchangeMTUReq_t pReq;
        pReq.clientRxMTU = MAX_PDU_SIZE-L2CAP_HDR_SIZE;
        GATT_ExchangeMTU(pPkt->connectionHandle, &pReq, selfEntity);
      }
      send_mailbox_to_app_user(MSG_BLUETOOTH_CONNECTED, NULL, 0);
      ble_client.connect_state = APP_CONNECT_STATE_CONNECTED;

      // 根据连接数量决定是否继续广播
      if (numActive < MAX_NUM_BLE_CONNS) { // 启动广播，因为有更多连接的空间
        GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
      } else { // 停止广播，因为没有更多连接的空间
        GapAdv_disable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
      }
      break;
    }

    case GAP_LINK_TERMINATED_EVENT:  // 链接终止事件
    {
      gapTerminateLinkEvent_t *pPkt = (gapTerminateLinkEvent_t *)pMsg;

      // 显示当前连接数量
      uint8_t numActive = linkDB_NumActive();
      ble_log("Device Disconnected!");
      ble_log("Num Conns: %d", (uint16_t)numActive);

      // 从列表中移除连接并在需要时禁用RSSI
      BLE_removeConn(pPkt->connectionHandle);

      // 如果没有活动连接
      if (numActive == 0) {// 停止周期性时钟
        Util_stopClock(&clkPeriodic);
      }

      // 启动广播，因为有更多连接的空间
      GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
      user_send_mailbox_ble_state(APP_CONNECT_STATE_DISCONNECTED);
      break;
    }

    case GAP_UPDATE_LINK_PARAM_REQ_EVENT:  // 更新链接参数请求事件
    {
      gapUpdateLinkParamReqReply_t rsp;
      gapUpdateLinkParamReqEvent_t *pReq = (gapUpdateLinkParamReqEvent_t *)pMsg;

      rsp.connectionHandle = pReq->req.connectionHandle;
      rsp.signalIdentifier = pReq->req.signalIdentifier;

      // 仅接受外设延迟为0的连接间隔
      if(pReq->req.connLatency == 0)
      {
        rsp.intervalMin = pReq->req.intervalMin;
        rsp.intervalMax = pReq->req.intervalMax;
        rsp.connLatency = pReq->req.connLatency;
        rsp.connTimeout = pReq->req.connTimeout;
        rsp.accepted = TRUE;
      }
      else
      {
        rsp.accepted = FALSE;
      }

      // 发送回复
      VOID GAP_UpdateLinkParamReqReply(&rsp);
      break;
    }

    case GAP_LINK_PARAM_UPDATE_EVENT:  // 链接参数更新事件
    {
      gapLinkUpdateEvent_t *pPkt = (gapLinkUpdateEvent_t *)pMsg;

      // 从句柄获取地址
      linkDBInfo_t linkInfo;
      linkDB_GetInfo(pPkt->connectionHandle, &linkInfo);

      if(pPkt->status == SUCCESS) { // 显示连接更新的地址
          ble_log("Link Param Updated: %s", Util_convertBdAddr2Str(linkInfo.addr));
      } else { // 显示连接更新失败的地址
         ble_log("Link Param Update Failed 0x%x: %s", pPkt->opcode, Util_convertBdAddr2Str(linkInfo.addr));
      }

      // 检查是否有任何排队的参数更新
      spConnHandleEntry_t *connHandleEntry = (spConnHandleEntry_t *)List_get(&paramUpdateList);
      if (connHandleEntry != NULL) {
        // 现在尝试发送排队的更新
        BLE_processParamUpdate(connHandleEntry->connHandle);
        ICall_free(connHandleEntry);  // 释放列表元素
      }
      break;
    }

    #if defined ( NOTIFY_PARAM_UPDATE_RJCT )
    case GAP_LINK_PARAM_UPDATE_REJECT_EVENT:  // 链接参数更新拒绝事件
    {
      linkDBInfo_t linkInfo;
      gapLinkUpdateEvent_t *pPkt = (gapLinkUpdateEvent_t *)pMsg;

      // 从句柄获取地址
      linkDB_GetInfo(pPkt->connectionHandle, &linkInfo);

      // 显示连接更新失败的地址
      // Display_printf(dispHandle, SP_ROW_STATUS_2, 0,
      //                "Peer Device's Update Request Rejected 0x%x: %s", pPkt->opcode,
      //                Util_convertBdAddr2Str(linkInfo.addr));
      break;
    }
    #endif

    default:
      break;
  }
}

/*********************************************************************
 * @fn      BLE_updatePHYStat
 *
 * @brief   更新自动PHY更新状态机
 *
 * @param   eventCode - 事件代码
 * @param   pMsg - 消息指针
 *
 * @return  无
 */
static void BLE_updatePHYStat(uint16_t eventCode, uint8_t *pMsg)
{
  uint8_t connIndex;

  switch (eventCode)
  {
    case HCI_LE_SET_PHY:  // 设置PHY命令状态
    {
      // 从列表获取连接句柄
      spConnHandleEntry_t *connHandleEntry = (spConnHandleEntry_t *)List_get(&setPhyCommStatList);

      if (connHandleEntry)
      {
        // 从句柄获取索引
        connIndex = BLE_getConnIndex(connHandleEntry->connHandle);
        ICall_free(connHandleEntry);

        // 此连接是否仍然有效？
        if (connIndex < MAX_NUM_BLE_CONNS)
        {
          hciEvt_CommandStatus_t *pMyMsg = (hciEvt_CommandStatus_t *)pMsg;

          if (pMyMsg->cmdStatus == HCI_ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE)
          {
            // 更新活动RSSI跟踪连接的PHY更改请求状态
            connList[connIndex].phyCngRq = FALSE;
            connList[connIndex].phyRqFailCnt++;
          }
        }
      }
      break;
    }

    case HCI_BLE_PHY_UPDATE_COMPLETE_EVENT:  // PHY更新完成事件
    {
      hciEvt_BLEPhyUpdateComplete_t *pPUC = (hciEvt_BLEPhyUpdateComplete_t*) pMsg;

      if(pPUC)
      {
        // 从句柄获取索引
        connIndex = BLE_getConnIndex(pPUC->connHandle);

        // 此连接是否仍然有效？
        if (connIndex < MAX_NUM_BLE_CONNS)
        {
          // 更新活动RSSI跟踪连接的PHY更改请求状态
          connList[connIndex].phyCngRq = FALSE;

          if (pPUC->status == SUCCESS)
          {
            connList[connIndex].currPhy = pPUC->rxPhy;  // 更新当前PHY
          }

          // 检查请求的PHY是否与实际PHY匹配
          if(pPUC->rxPhy != connList[connIndex].rqPhy)
          {
            connList[connIndex].phyRqFailCnt++;
          }
          else
          {
            // 重置请求PHY计数器和请求的PHY
            connList[connIndex].phyRqFailCnt = 0;
            connList[connIndex].rqPhy = 0;
          }
        }
      }
      break;
    }

    default:
      break;
  }
}

/*********************************************************************
 * @fn      BLE_processL2CAPMsg
 *
 * @brief   处理L2CAP消息和事件
 *
 * @param   pMsg - 要处理的消息
 *
 * @return  如果可以安全释放传入消息返回TRUE，否则返回FALSE
 */
static uint8_t BLE_processL2CAPMsg(l2capSignalEvent_t *pMsg)
{
  uint8_t safeToDealloc = TRUE;
  static bool firstRun = TRUE;

  switch (pMsg->opcode)
  {
    case L2CAP_NUM_CTRL_DATA_PKT_EVT:  // 控制数据包数量事件
    {
      /*
      * 我们不能在收到启用命令后立即重启设备，
      * 必须给协议栈足够的时间来处理和响应
      * OAD_EXT_CTRL_ENABLE_IMG命令。此命令将确定
      * LE控制器当前排队的包数量。
      * BIM变量已通过OadPersistApp_processOadWriteCB设置
      */
      if(firstRun)
      {
        firstRun = false;

        // 我们只想设置一次numPendingMsgs
        numPendingMsgs = MAX_NUM_PDU - pMsg->cmd.numCtrlDataPktEvt.numDataPkt;


        /* 设置标志，以便连接事件回调将在
         * 待处理OAD重启的上下文中处理
         */

      }
      break;
    }

    default:
      // 什么都不做
      break;
  }

  return (safeToDealloc);
}

/*********************************************************************
 * @fn      BLE_processStackMsg
 *
 * @brief   处理传入的协议栈消息
 *
 * @param   pMsg - 要处理的消息
 *
 * @return  如果可以安全释放传入消息返回TRUE，否则返回FALSE
 */
static uint8_t BLE_processStackMsg(ICall_Hdr *pMsg)
{
  // 除非另行设置，否则始终释放pMsg
  uint8_t safeToDealloc = TRUE;

  switch (pMsg->event)
  {
    case GAP_MSG_EVENT:  // GAP消息事件
      BLE_processGapMessage((gapEventHdr_t*) pMsg);
      break;

    case GATT_MSG_EVENT:  // GATT消息事件
      // 处理GATT消息
      safeToDealloc = BLE_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    case HCI_GAP_EVENT_EVENT:  // HCI GAP事件
    {
      // 处理HCI消息
      switch(pMsg->status)
      {
        case HCI_COMMAND_COMPLETE_EVENT_CODE:  // 命令完成事件
          // 在此处处理HCI命令完成事件
          BLE_processCmdCompleteEvt((hciEvt_CmdComplete_t *) pMsg);
          break;

        case HCI_BLE_HARDWARE_ERROR_EVENT_CODE:  // BLE硬件错误事件
          // 添加硬件错误日志
          // heed add log yyj
          break;

        case HCI_COMMAND_STATUS_EVENT_CODE:  // 命令状态事件
        {
          hciEvt_CommandStatus_t *pMyMsg = (hciEvt_CommandStatus_t *)pMsg;
          switch ( pMyMsg->cmdOpcode )
          {
            case HCI_LE_SET_PHY:  // 设置PHY命令状态
            {
              if (pMyMsg->cmdStatus == HCI_ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE) {
                  ble_log("PHY Change failure, peer does not support this");
              }
              else
              {
                  ble_log("PHY Update Status Event: 0x%x", pMyMsg->cmdStatus);
              }

              BLE_updatePHYStat(HCI_LE_SET_PHY, (uint8_t *)pMsg);
              break;
            }

            default:
              break;
          }
          break;
        }

        case HCI_LE_EVENT_CODE:  // LE事件代码
        {
          hciEvt_BLEPhyUpdateComplete_t *pPUC = (hciEvt_BLEPhyUpdateComplete_t*) pMsg;

          // PHY更新已完成或失败
          if (pPUC->BLEEventCode == HCI_BLE_PHY_UPDATE_COMPLETE_EVENT)
          {
            if (pPUC->status != SUCCESS)
            {
                ble_log("PHY Change failure");
            }
            else
            {
              // 仅支持对称PHY。rxPhy应该等于txPhy。
                ble_log("PHY Updated to %s",
                              (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_1M) ? "1M" :
                              (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_2M) ? "2M" :
                              (pPUC->rxPhy == PHY_UPDATE_COMPLETE_EVENT_CODED) ? "CODED" : "Unexpected PHY Value");
            }

            BLE_updatePHYStat(HCI_BLE_PHY_UPDATE_COMPLETE_EVENT, (uint8_t *)pMsg);
          }
          break;
        }

        default:
          break;
      }
      break;
    }

    case L2CAP_SIGNAL_EVENT:  // L2CAP信号事件
      // 处理L2CAP信号
      safeToDealloc = BLE_processL2CAPMsg((l2capSignalEvent_t *)pMsg);
      break;

    default:
      // 什么都不做
      break;
  }

  return (safeToDealloc);
}



//BLE主循环
void BLE_taskFxn(UArg a0, UArg a1)
{
  delay_ms(1000);
  ble_log("BLE_init###########");

  // 初始化应用程序
  BLE_init();
  // 应用程序主循环
  ble_client.connect_state = APP_CONNECT_STATE_INVALID;

  GateMutexPri_Params_init(&myGateParams);
  /* 创建互斥锁实例 */
  myGate = GateMutexPri_create(&myGateParams, NULL);

  while(1)
  {
    uint32_t events;
    // 等待事件发生



    events = Event_pend(syncEvent, Event_Id_NONE, SP_ALL_EVENTS, ICALL_TIMEOUT_FOREVER);

    #if 0
    Task_Handle taskhandle = 0;
    Task_Stat statbuf;  /* declare buffer */

    taskhandle = Task_self();
    Task_stat(taskhandle, &statbuf);

    logWarning("Task 0x%08X BLE_taskFxn usage: %d/%d bytes (%.1f%%)",
                      taskhandle,
                      statbuf.used,
                      statbuf.stackSize,
                      (statbuf.used * 100.0f) / statbuf.stackSize);
    #endif

    // 如果有事件发生
    if (events) {
      ICall_EntityID dest;        // 目标实体ID
      ICall_ServiceEnum src;      // 源服务枚举
      ICall_HciExtEvt *pMsg = NULL; // 消息指针

      // 获取可能从协议栈发送的任何可用消息
      if (ICall_fetchServiceMsg(&src, &dest, (void **)&pMsg) == ICALL_ERRNO_SUCCESS) {
        uint8 safeToDealloc = TRUE; // 安全释放标志，默认为TRUE

        // 检查消息是否来自BLE协议栈服务且目标是当前实体
        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntity)) {
          ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;
          // 检查BLE协议栈事件签名（确保是有效事件）
          if (pEvt->signature != 0xffff) {
            // 处理任务间消息，并获取是否可以安全释放消息的指示
            safeToDealloc = BLE_processStackMsg((ICall_Hdr *)pMsg);
          }
        }
        // 如果消息存在且可以安全释放，则释放消息内存
        if (pMsg && safeToDealloc) {
          ICall_freeMsg(pMsg);
        }
      }

      // 如果RTOS队列事件触发且队列不为空，处理应用程序消息
      if (events & SP_QUEUE_EVT) {
        // 循环处理队列中的所有消息
        while (!Queue_empty(appMsgQueueHandle)) {
          // 从队列中取出消息
          spEvt_t *pMsg = (spEvt_t *)Util_dequeueMsg(appMsgQueueHandle);
          if (pMsg) {
            // 处理应用程序消息
            BLE_processAppMsg(pMsg);

            // 释放消息占用的内存空间
            ICall_free(pMsg);
          }
//          logInfo("while");
        }
      }
    }
  }
} // 结束BLE_taskFxn函数
//
////创建线程
//void BLE_createTask(void)
//{
//  static Task_Struct spTask;  // 任务结构体
//  static uint8_t spTaskStack[SP_TASK_STACK_SIZE];  // 任务堆栈
//  Task_Params taskParams;  // 任务参数
//  // 初始化任务参数
//  Task_Params_init(&taskParams);
//  taskParams.stack = spTaskStack;
//  taskParams.stackSize = SP_TASK_STACK_SIZE;
//  taskParams.priority = SP_TASK_PRIORITY;
//  // 构造任务
//  Task_construct(&spTask, BLE_taskFxn, &taskParams, NULL);
//
//  ble_client.connect_state = APP_CONNECT_STATE_INVALID;
//}
