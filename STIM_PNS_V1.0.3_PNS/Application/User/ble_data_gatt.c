//实现蓝牙跟手机交互的接口定义  纯蓝牙接口
#include <app_verify_ble.h>
#include <string.h>
#include <icall.h>
#include "util.h"
#include "icall_ble_api.h"
#include "ble_data_gatt.h"

#include "main_user.h"

#include "app_log.h"


#define SHELL_DEBUG_BLE_TEST   0
// BLE调试宏定义：启用调试时使用logInfo，否则打印为空
#define BLE_DEBUG         1
#if BLE_DEBUG
#define ble_log(fmt, ...) logInfo(fmt, ##__VA_ARGS__)
#else
#define ble_log(fmt) ((void)0)
#endif

// UART Service 128-bit UUID
CONST uint8 UartProfileServUUID[ATT_UUID_SIZE]  = { 0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E };
// Characteristic UUIDs
CONST uint8 RxUUID[ATT_UUID_SIZE]               = { 0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E };
CONST uint8 NotifyCharUUID[ATT_UUID_SIZE]       = { 0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E };
CONST uint8 RxNotifyCharUUID[ATT_UUID_SIZE]     = { 0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x04, 0x00, 0x40, 0x6E };
// UART Service attribute
static CONST gattAttrType_t UartServiceUUID = { ATT_UUID_SIZE, UartProfileServUUID };


// Characteristic Values
static uint8 RxBuf[RX_TX_BUFF_SIZE];
static uint8 NotifyValue[RX_TX_BUFF_SIZE];
static uint8 RxNotifyValue[RX_TX_BUFF_SIZE];

struct tx_data_pack tx_buf; //发送缓冲区

// Descriptions
static uint8 UartServiceName[] = "UART Service";
static uint8 TxDesp[] = "Tx";
static uint8 RxNotifyDesp[] = "RxNotify";
static uint8 RxDesp[] = "Rx";

// 客户端特征配置描述符句柄
static gattCharCfg_t *UartProfileHdl;
static gattCharCfg_t *UartProfileHdl2;

// Characteristic Properties
static uint8 TxProps       = GATT_PROP_WRITE | GATT_PROP_WRITE_NO_RSP;
static uint8 RxNotifyProps = GATT_PROP_NOTIFY;
static uint8 RxNotifyProps2       = GATT_PROP_NOTIFY  | GATT_PROP_WRITE ;

static gattAttribute_t UuidTbl[] =
{
    // UART Service
    { { ATT_BT_UUID_SIZE, primaryServiceUUID }, GATT_PERMIT_READ,  0, (uint8 *)&UartServiceUUID    },
    // Service Name
    { { ATT_BT_UUID_SIZE, charUserDescUUID },   GATT_PERMIT_READ,  0, UartServiceName              },
    // 接收手机的数据 (手机 -> 设备)
    { { ATT_BT_UUID_SIZE, characterUUID },      GATT_PERMIT_READ,  0, &TxProps                     },
    { { ATT_UUID_SIZE, RxUUID },                GATT_PERMIT_WRITE, 0, RxBuf                        },
    { { ATT_BT_UUID_SIZE, charUserDescUUID },   GATT_PERMIT_READ,  0, TxDesp                       },
    // 发送通知到手机的UUID (设备 -> 手机，通知)
    { { ATT_BT_UUID_SIZE, characterUUID },      GATT_PERMIT_READ,  0, &RxNotifyProps               },
    { { ATT_UUID_SIZE, NotifyCharUUID },        GATT_PERMIT_READ,  0, NotifyValue                  },
    { { ATT_BT_UUID_SIZE, clientCharCfgUUID },  GATT_PERMIT_READ | GATT_PERMIT_WRITE, 0, (uint8 *)&UartProfileHdl },
    { { ATT_BT_UUID_SIZE, charUserDescUUID },   GATT_PERMIT_READ,  0, RxNotifyDesp                 },
    // 发送数据到手机的UUID (设备 -> 手机，订阅 + 手机 -> 设备，写入)   //订阅失败先不处理
    { { ATT_BT_UUID_SIZE, characterUUID },      GATT_PERMIT_READ,  0, &RxNotifyProps2              },
    { { ATT_UUID_SIZE, RxNotifyCharUUID },      GATT_PERMIT_READ  | GATT_PROP_WRITE, 0, RxNotifyValue },
    { { ATT_BT_UUID_SIZE, clientCharCfgUUID },  GATT_PERMIT_READ | GATT_PROP_WRITE , 0, (uint8 *)&UartProfileHdl2 },
    { { ATT_BT_UUID_SIZE, charUserDescUUID },   GATT_PERMIT_READ,  0, RxDesp                       }
};


#if SHELL_DEBUG_BLE_TEST
static int shell_ble_send_data(int argc, char *argv[])
{
    uint16_t tx_len;
    uint8_t data[64];

    ble_log("%d parameter(s)", argc);
    for (char i = 1; i < argc; i++)
    {
        ble_log("%s", argv[i]);
        sscanf((char*)argv[i], "%d", (int *)&data[i]);
    }
    data[0] = argc-1; //len
    put_buf(&data[1], data[0]);
    tx_len = (uint8_t)data[0];
    RxNotifyValue[0] = tx_len;
    memcpy(&RxNotifyValue[1], &data[1], tx_len);

    return 0;
}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), shell_ble_send_data, shell_ble_send_data, [frist data is len  ather is data]);


static int shell_ble_send_notify_data(int argc, char *argv[])  //设备订阅后 测试订阅接口 立即发射通知  用于设备发送数据到手机
{
    uint8_t data[64];
    uint16_t tx_len;
//    ble_log("%d parameter(s)", argc);
    for (char i = 1; i < argc; i++) {
//        ble_log("%s", argv[i]);
        sscanf((char*)argv[i], "%d", (int *)&data[i]);
    }
    uint8_t data_array[] = {
        0xc3, 0x34, 0x30, 0x46, 0x41, 0x33, 0x46, 0x36, 0x39, 0x33, 0x41, 0x42, 0x31, 0x30, 0x42, 0x34,
        0x36, 0x43, 0x30, 0x42, 0x45, 0x30, 0x30, 0x39, 0x38, 0x33, 0x32, 0x33, 0x36, 0x32, 0x29, 0x33,
        0x41, 0x3c
    };

    // 数组长度
    uint16_t data_length = sizeof(data_array) / sizeof(data_array[0]);
    BLE_send_notify(data_array, data_length);  //接口
//    BLE_send_notify(&data[1], argc-1);  //接口
    return 0;
}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), shell_ble_send_notify_data, shell_ble_send_notify_data, [frist data is len  ather is data]);


#endif

#include <ti/sysbios/gates/GateMutexPri.h>
#include <ti/sysbios/knl/Task.h>
extern GateMutexPri_Handle myGate;
extern IArg key_lock;

//手机读取设备数据
bStatus_t UartProfile_ReadAttrCB(uint16_t connHandle,gattAttribute_t *pAttr,uint8_t *pValue, uint16_t *pLen,uint16_t offset, uint16_t maxLen,uint8_t method)
{
    bStatus_t status = SUCCESS;
    if (offset > 0)
    {
        return (ATT_ERR_ATTR_NOT_LONG);
    }

    if (pAttr->type.len == ATT_UUID_SIZE) {
        if (memcmp(pAttr->type.uuid, RxNotifyCharUUID, ATT_UUID_SIZE) == 0) {
//            logInfo("RxNotifyCharUUID");
        } else if (memcmp(pAttr->type.uuid, NotifyCharUUID, ATT_UUID_SIZE) == 0) {
//             logInfo("send buf len = %d offset = %d", tx_buf.len, offset);
            *pLen = tx_buf.len;
//            logInfo("tx_buf.len = %d, maxLen = %d, offset = %d", tx_buf.len,maxLen,offset);
//            *pLen = MIN(maxLen, tx_buf.len - offset); // Transmit as much as possible
            memcpy(pValue,  tx_buf.data,  tx_buf.len);
            GateMutexPri_leave(myGate, key_lock); //释放锁
        } else {
            *pLen = 0;
            status = ATT_ERR_ATTR_NOT_FOUND;
        }
    }
    else if (pAttr->type.len == ATT_BT_UUID_SIZE)
    {
//        ble_log("77");
        uint16 uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);
        switch (uuid)
        {
            case GATT_CLIENT_CHAR_CFG_UUID:
                if (maxLen >= sizeof(uint16))
                {
                    *pLen = sizeof(uint16);
                    *((uint16 *)pValue) = *((uint16 *)pAttr->pValue);
                }
                else
                {
                    status = ATT_ERR_INSUFFICIENT_RESOURCES;
                }
                break;

            default:
                *pLen = 0;
                status = ATT_ERR_ATTR_NOT_FOUND;
                break;
        }
    }
    else
    {
        *pLen = 0;
        status = ATT_ERR_INVALID_HANDLE;
    }

    return (status);
}

//接收手机数据
bStatus_t UartProfile_WriteAttrCB(uint16_t connHandle,gattAttribute_t *pAttr,uint8_t *pValue, uint16_t len,uint16_t offset, uint8_t method)
{
    bStatus_t status = SUCCESS;
    uint8 notifyApp = 0xFF;

    // Check if this is a 128-bit UUID characteristic value
    if (pAttr->type.len == ATT_UUID_SIZE) {
        //接收到外部设备的BLE设备
        if (memcmp(pAttr->type.uuid, RxUUID, ATT_UUID_SIZE) == 0) {
//            logInfo("get mobile phone data len = %d",  len);
//            put_buf(pValue, len);
            user_get_ble_data_send_mailbox(pValue , len);  //接收到手机的数据发送到线程
        } else if (memcmp(pAttr->type.uuid, RxNotifyCharUUID, ATT_UUID_SIZE) == 0) {
            // 处理对 RxNotifyCharUUID 特征的写入
//            logInfo("get mobile phone data to RxNotifyCharUUID");
//            put_buf(pValue, len);
            // 这里可以添加对 RxNotifyCharUUID 写入数据的处理逻辑
        } else {
            status = ATT_ERR_ATTR_NOT_FOUND;
        }
    } else if (pAttr->type.len == ATT_BT_UUID_SIZE)  {//订阅通知设置
        uint16 uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);
        switch (uuid) {
            case GATT_CLIENT_CHAR_CFG_UUID:
//                logInfo("set subscription");
                // 根据不同的特征使用不同的处理
                if (pAttr->pValue == (uint8 *)&UartProfileHdl) {
                    status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len, offset, GATT_CLIENT_CFG_NOTIFY);
                } else if (pAttr->pValue == (uint8 *)&UartProfileHdl2) {
                    status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len, offset, GATT_CLIENT_CFG_NOTIFY);
                }
                break;
            default:
                status = ATT_ERR_ATTR_NOT_FOUND;
                break;
        }
    } else {
        status = ATT_ERR_INVALID_HANDLE;
    }
    return (status);
}

//发送通知 在 BLE_performPeriodicTask 进行周期性调用
bStatus_t UartProfile_NotifySenddata(bt_ev_data *in_data)
{
    if(in_data->data != NULL && in_data->len != 0) {
//        logInfo("len = %d",  in_data->len);
//        put_buf(in_data->data, in_data->len);
        if(in_data->len <512) {
            tx_buf.len = in_data->len;
            memcpy(tx_buf.data, in_data->data, in_data->len);
        } else {
            logInfo("!!!!!!!!!!!!!!!!!tx len max >512");
            return -1;
        }
    }

    GATTServApp_ProcessCharCfg(UartProfileHdl, NotifyValue, FALSE,
                              UuidTbl, GATT_NUM_ATTRS(UuidTbl),
                              INVALID_TASK_ID, UartProfile_ReadAttrCB);
    return 0;
}

// 添加第二个特征的发送通知函数
bStatus_t UartProfile_NotifySenddata2(void)
{
    GATTServApp_ProcessCharCfg(UartProfileHdl2, RxNotifyValue, FALSE,
                              UuidTbl, GATT_NUM_ATTRS(UuidTbl),
                              INVALID_TASK_ID, UartProfile_ReadAttrCB);
    return 0;
}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), UartProfile_NotifySenddata, UartProfile_NotifySenddata, NULL);
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), UartProfile_NotifySenddata2, UartProfile_NotifySenddata2, NULL);

bStatus_t Ble_GATT_User_Init(void)
{
    uint8 status;

    static CONST gattServiceCBs_t GATTProfileCBs = {
        UartProfile_ReadAttrCB,
        UartProfile_WriteAttrCB,
        NULL
    };

    UartProfileHdl = (gattCharCfg_t *)ICall_malloc(sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS);
    if (UartProfileHdl == NULL) {
        return (bleMemAllocError);
    }

    // 初始化第二个客户端特征配置描述符
    UartProfileHdl2 = (gattCharCfg_t *)ICall_malloc(sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS);
    if (UartProfileHdl2 == NULL) {
        ICall_free(UartProfileHdl);
        return (bleMemAllocError);
    }

    GATTServApp_InitCharCfg(LINKDB_CONNHANDLE_INVALID, UartProfileHdl);
    GATTServApp_InitCharCfg(LINKDB_CONNHANDLE_INVALID, UartProfileHdl2);

    status = GATTServApp_RegisterService(UuidTbl, GATT_NUM_ATTRS(UuidTbl), GATT_MAX_ENCRYPT_KEY_SIZE, &GATTProfileCBs);
    return (status);
}
