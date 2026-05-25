#ifndef APP_USER_H
#define APP_USER_H

#include "main_user.h"
#include "app_ble_data.h"

#pragma pack(push, 8)  // 保存当前对齐方式，设置为8字节对齐


typedef enum {
    POWER_OFF = 0,
    POWER_ON
} power_status;

typedef enum {
    STIM_OFF = 0,
    STIM_ON
} stim_status;


// 经皮刺激器业务逻辑消息类型枚举
typedef enum {
    // 刺激控制命令
    MSG_STIM_START = 0,          // 开始刺激
    MSG_STIM_THRESHOLD_TEST,     // 阈值测试
    MSG_STIM_STOP,               // 刺激停止

    MSG_SYS_RESET,               // 系统复位
    MSG_FAC_RESET,               // 恢复出厂设置
    MSG_POWER_ON,                // 系统上电
    MSG_POWER_OFF,               // 系统掉电 用户主动关机
    // 用户交互命令
    MSG_KEY_INCREASE,            // 按键加
    MSG_KEY_DECREASE,            // 按键减

    // 电源管理
    MSG_POWER_MANAGEMENT,        // 电量处理
    MSG_POWER_LOW,               // 低电量警告
    MSG_POWER_CHARGING,          // 充电状态

    // 蓝牙连接状态
    MSG_BLUETOOTH_CONNECTED,     // 蓝牙正在连接
    MSG_BLUETOOTH_CONNECTED_SUCCEED, // 蓝牙连接成功
    MSG_BLUETOOTH_DISCONNECTED,  // 蓝牙断开
    MSG_BLUETOOTH_DATA_RECEIVED, // 蓝牙数据接收

    // 电极片管理
    MSG_ELECTRODE_PROCESSING,    // 电极片处理
    MSG_ELECTRODE_DETECT,        // 电极片检测
    MSG_ELECTRODE_ERROR,         // 电极片错误
    MSG_ELECTRODE_BURN_MODE,     // 电极片烧录模式

    MSG_SHELL_TEST_STIM,         //测试

    MSG_START_STIM,              //开始刺激
    MSG_STOP_STIM,               //停止刺激
    MSG_CHECK_STIM,              //检查刺激状态

    MSG_TYPE_COUNT,               // 消息类型总数，用于边界检查
    MSG_TYPE_NULL,
} stim_message_type_t;

// 消息结构体
typedef struct {
    stim_message_type_t type;
    uint8_t data[128];
    uint32_t data_len;
} app_user_message_t;



//#define SOFT_VERSION   "V1.0.3"
//#define HARD_VERSION   "V1.1"
//#define DEV_SN         "TTNS12345678"
//#define PRD_TIME       "251110101801"
//#define BAT_INFO       "330"
////设备信息
//typedef struct {
//    u8 softver[7]; //软件版本号
//    u8 hardver[7]; //硬件版本号
//    u8 dev_id[15]; //刺激器SN码 字符串格式
//    u8 timer[14];
//    u8 bat_info[5]; //电池信息
//} dev_info;




typedef struct {
//    device_status_t dev_status;
    Mailbox_Struct *mailbox;
    bool power_status;
    bool sys_power_on_flag;
//    StimProgramFull_t ble_set_stim; //接收蓝牙传入的刺激参数
    uint8_t stim_stata;  //刺激器刺激状态
    uint8_t stim_start_flag;  //刺激器开始刺激标志

} app_user_data_t;
extern app_user_data_t user;

void app_user_task(UArg arg0, UArg arg1);
int send_mailbox_to_app_user(stim_message_type_t msg_type, uint8_t *data, uint16_t len);


#pragma pack(pop)       // 恢复之前的对齐方式

#endif /* APP_USER_H */
