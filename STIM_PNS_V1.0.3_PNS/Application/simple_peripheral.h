#ifndef SBP_OAD_ONCHIP_H
#define SBP_OAD_ONCHIP_H

#include <string.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/display/Display.h>
#include <ti/drivers/utils/List.h>
#include <icall.h>
#include "util.h"
#include "onboard.h"
#include <bcomdef.h>
#include <icall_ble_api.h>
#include <devinfoservice.h>
#include <simple_gatt_profile.h>
#include <ti_drivers_config.h>
#include <board_key.h>
#include <menu/two_btn_menu.h>
#include "ti_ble_config.h"



/*********************************************************************
 * 常量定义
 */

// 执行周期性事件的间隔时间（毫秒）
#define SP_PERIODIC_EVT_PERIOD               2000

// 扫描响应数据字符串中软件版本信息的存储偏移量
#define OAD_SOFT_VER_OFFSET                   15

// 任务配置
#define SP_TASK_PRIORITY                     1  // 任务优先级

#ifndef SP_TASK_STACK_SIZE
#define SP_TASK_STACK_SIZE                   1408  // 任务堆栈大小（字节）
#endif

// 应用程序事件定义
#define SP_STATE_CHANGE_EVT                  0   // 状态改变事件
#define SP_CHAR_CHANGE_EVT                   1   // 特征值改变事件
#define SP_KEY_CHANGE_EVT                    2   // 密钥改变事件
#define SP_ADV_EVT                           3   // 广播事件
#define SP_PAIR_STATE_EVT                    4   // 配对状态事件
#define SP_PASSCODE_EVT                      5   // 密码事件
#define SP_PERIODIC_EVT                      6   // 周期性事件
#define SP_READ_RPA_EVT                      7   // 读取RPA事件
#define SP_SEND_PARAM_UPDATE_EVT             8   // 发送参数更新事件
#define SP_CONN_EVT                          9   // 连接事件
#define SP_DISCONNECT_EVT                          10   // 断开连接事件
// yyjtest在原有事件定义附近添加
#define SP_SEND_DATA_EVT                  0x0020  // 发送数据事件

// RTOS应用程序的内部事件
#define SP_ICALL_EVT                         ICALL_MSG_EVENT_ID  // ICall消息事件 (Event_Id_31)
#define SP_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // 工具队列事件 (Event_Id_30)

// 所有需要等待的RTOS事件的位或组合                // ICall事件 // 队列事件      // OAD队列事件       // OAD完成事件       // OAD内存不足事件
#define SP_ALL_EVENTS                        (SP_ICALL_EVT | SP_QUEUE_EVT)// | SP_OAD_QUEUE_EVT | SP_OAD_COMPLETE_EVT | SP_OAD_NO_MEM_EVT)

// 字符串转换后的设备地址大小（"0xXXXXXXXXXXXX"格式）
#define SP_ADDR_STR_SIZE     15

// 双按钮菜单的行号定义
#define SP_ROW_SEPARATOR_1   (TBM_ROW_APP + 0)  // 分隔符行1
#define SP_ROW_STATUS_1      (TBM_ROW_APP + 1)  // 状态行1
#define SP_ROW_STATUS_2      (TBM_ROW_APP + 2)  // 状态行2
#define SP_ROW_CONNECTION    (TBM_ROW_APP + 3)  // 连接行
#define SP_ROW_ADVSTATE      (TBM_ROW_APP + 4)  // 广播状态行
#define SP_ROW_RSSI          (TBM_ROW_APP + 5)  // RSSI行
#define SP_ROW_IDA           (TBM_ROW_APP + 6)  // 身份地址行
#define SP_ROW_RPA           (TBM_ROW_APP + 7)  // 随机私有地址行
#define SP_ROW_DEBUG         (TBM_ROW_APP + 8)  // 调试行

// 用于存储活动连接的配置
#define SP_RSSI_TRACK_CHNLS        1            // 最大可能通道数（可为GAP_BONDINGS_MAX）
#define SP_MAX_RSSI_STORE_DEPTH    5            // RSSI存储最大深度
#define SP_INVALID_HANDLE          0xFFFF       // 无效句柄值

// RSSI阈值定义（用于PHY选择）
#define RSSI_2M_THRSHLD           -30           // 2M PHY阈值：-80 dB RSSI
#define RSSI_1M_THRSHLD           -40           // 1M PHY阈值：-90 dB RSSI
#define RSSI_S2_THRSHLD           -50           // S2 PHY阈值：-100 dB RSSI
#define RSSI_S8_THRSHLD           -60           // S8 PHY阈值：-120 dB RSSI

// PHY相关常量
#define SP_PHY_NONE                LL_PHY_NONE  // 无PHY设置
#define AUTO_PHY_UPDATE            0xFF         // 自动PHY更新标志


// 直接使用System_abort
#define BLE_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            logInfo("Assertion failed: %s at %s:%d\n", #condition, __FILE__, __LINE__); \
            while(1);\
        } \
    } while(0)

//// Spin if the expression is not true
//#define BLE_ASSERT(expr) if (!(expr))while(1)

typedef struct
{
  uint16_t len;                // event type
  uint8_t *data;               // pointer to message
} bt_ev_data;

typedef struct
{
  uint16_t event;                // event type
  void    *pData;               // pointer to message
} spEvt_t;

typedef struct
{
  uint8_t state;
  uint16_t connHandle;
  uint8_t status;
} spPairStateData_t;

typedef struct
{
  uint8_t deviceAddr[B_ADDR_LEN];
  uint16_t connHandle;
  uint8_t uiInputs;
  uint8_t uiOutputs;
  uint32_t numComparison;
} spPasscodeData_t;

typedef struct
{
  uint32_t event;
  void *pBuf;
} spGapAdvEventData_t;

typedef struct
{
  uint8_t event;
  uint8_t data[];
} spClockEventData_t;

typedef struct
{
  List_Elem elem;
  uint16_t  connHandle;
} spConnHandleEntry_t;

typedef struct {
  uint16_t         connHandle;          // 连接句柄
  Clock_Struct     *pUpdateClock;       // 用于参数更新的RTOS时钟
  int8_t           rssiArr[5];          // RSSI历史记录（滑动窗口）
  uint8_t          rssiCntr;
  int8_t           rssiAvg;             // RSSI平均值
  bool             phyCngRq;            // PHY切换请求标志
  uint8_t          currPhy;             // 当前PHY模式（HCI_PHY_1_MBPS等）
  uint8_t          rqPhy;               // 请求的PHY模式
  uint8_t          phyRqFailCnt;        // PHY change request count
  bool             isAutoPHYEnable;     // 是否启用自动PHY切换
} spConnRec_t;

void BLE_taskFxn(UArg a0, UArg a1);
extern void BLE_createTask(void);
extern void BLE_disconnEvtCB(void); //发送消息断开蓝牙连接
bStatus_t BLE_disconnectAll(void);
extern void BLE_send_notify(uint8_t *data, uint16_t len);


#endif /* SBP_OAD_ONCHIP_H */
