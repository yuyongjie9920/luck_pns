/**
 * @file app_verify_ble.h
 * @brief 蓝牙连接验证与数据通信协议头文件
 * @details 实现程控仪(刺激器控制器)与手机APP之间的BLE安全通信协议
 * @version 1.0
 * @date 2026-04-28
 */

#ifndef __APP_VERIFY_BLE_H__
#define __APP_VERIFY_BLE_H__

#include "main_user.h"
#include "app_ble_data.h"

/*==============================================================================
 * 1. 加密与安全配置
 *============================================================================*/

/**
 * @def APP_PRIVATE_KEY
 * @brief SM2算法使用的APP端私钥（十六进制字符串格式，长度64字节）
 * @note 用于解密APP发送的签名数据，验证APP身份合法性
 */
#define APP_PRIVATE_KEY "a698bc604083199dc7709f070452bece634def374095ddf3280da5c3fc3066c6"  /**< key1生成的盐值  重要勿动  需要修改的话 app也要改才行*/
#define KEY1_SALT                      "lead920s-"   /**< key1生成的盐值  重要勿动 需要修改的话 app也要改才行*/
/*==============================================================================
 * 2. 版本信息定义
 *============================================================================*/
#define APP_API_VERSION                "V3.0.0"      /**< APP API协议版本，用于通信协商 */
#define CONTROLLER_SOFTWARE_VERSION    "V1.0.3"      /**< 程控仪(控制端)软件版本号 */
#define CONTROLLER_HARDWARE_VERSION    "V2.12"       /**< 程控仪(控制端)硬件版本号 */
#define STIMULATOR_SERIAL_NUMBER       "SA0230530001" /**< 刺激器(植入式设备)唯一序列号 */
#define STIMULATOR_SOFTWARE_VERSION    "V1.0.6"      /**< 刺激器(植入式设备)软件版本号 */
#define STIMULATOR_HARDWARE_VERSION    "V1.1"        /**< 刺激器(植入式设备)硬件版本号 */
#define BLE_SOFTWARE_VERSION           "V1.0.0"      /**< 蓝牙模块固件版本号 */

/*==============================================================================
 * 3. 超时与缓冲区配置
 *============================================================================*/
#define VERIFY_TIMEOUT_TIME            5000          /**< 身份验证窗口超时时间(ms) */
#define BLE_MTU                        182           /**< BLE MTU大小(微信小程序182，APP为244) */

/*==============================================================================
 * 4. 协议帧格式定义
 *============================================================================*/
#define PROTOCOL_FRAME_HEADER           0xC3          /**< 帧头标志字节 */
#define PROTOCOL_FRAME_TAIL             0x3C          /**< 帧尾标志字节 */

/*==============================================================================
 * 5. 枚举类型定义
 *============================================================================*/
/**
 * @enum aes_e
 * @brief AES加解密模式选择
 */
typedef enum {
    AES_DECRYPT = 0,  /**< 解密模式 */
    AES_ENCRYPT = 1,  /**< 加密模式 */
} aes_e;

/**
 * @enum protocol_frame_index_t
 * @brief 协议帧中各字段的索引位置(不含帧头帧尾)
 */
typedef enum {
    FRAME_HEADER_INDEX          = 0,    /**< 帧头索引 */
    FRAME_TAIL_INDEX            = 1,    /**< 帧尾倒数索引 */
    FRAME_SERIAL_NUMBER_INDEX   = 0,    /**< 帧序号索引 */
    FRAME_LEN_INDEX             = 2,    /**< 帧长度索引 */
    FRAME_CMD_INDEX             = 3,    /**< 命令码索引 */
    FRAME_DATA_INDEX            = 4,    /**< 可选数据索引 */
    FRAME_ERR_NO_INDEX          = 4,    /**< 错误码索引 */
    FRAME_ERR_CRC16_INDEX       = 5,    /**< 错误帧CRC16索引 */
    FRAME_CRC16_INDEX           = 2,    /**< CRC16倒数索引 */
} protocol_frame_index_t;

/**
 * @enum app_connect_state_t
 * @brief APP与程控仪的连接状态机
 * @details 定义完整的4步握手验证流程状态
 */
typedef enum {
    APP_CONNECT_STATE_INVALID           = 0x00U, /**< 无效状态 */
    APP_CONNECT_STATE_STARTED           = 0x01U, /**< 蓝牙模块上电完成，已生成key1 */
    APP_CONNECT_STATE_CONNECTED         = 0x02U, /**< APP已连接，等待验证步骤0 */
    APP_CONNECT_STATE_VERIFY0           = 0x03U, /**< 验证步骤0通过(SM2签名验证) */
    APP_CONNECT_STATE_VERIFY1           = 0x04U, /**< 验证步骤1通过(MAC地址验证) */
    APP_CONNECT_STATE_VERIFY2           = 0x05U, /**< 验证步骤2通过(完整验证完成) */
    APP_CONNECT_STATE_DISCONNECTED      = 0x06U, /**< APP已断开连接 */
    APP_CONNECT_STATE_CONNECT_TIMEOUT   = 0x07U, /**< 连接超时 */
    APP_CONNECT_STATE_IN_DATA           = 0x08U, /**< 数据交互中 */
} app_connect_state_t;

/*==============================================================================
 * 6. 数据结构定义
 *============================================================================*/

/**
 * @struct mailbox_msg
 * @brief 邮箱消息结构体，用于任务间通信
 */
struct mailbox_msg {
    uint8_t *ptr;      /**< 消息数据指针 */
    uint32_t size;     /**< 消息数据大小 */
};

/**
 * @struct app_ble_client
 * @brief BLE客户端主控制结构体
 */
struct app_ble_client {
    app_connect_state_t connect_state;      /**< 当前连接状态 */
    uint8_t key1[16];                       /**< 第一阶段会话密钥(HMAC-MD5生成) */
    uint8_t key2[16];                       /**< 第二阶段会话密钥 */
    uint8_t macaddr_byte[B_ADDR_LEN];       /**< MAC地址(字节数组) */
    char macaddr_str[19];                   /**< 蓝牙mac地址字符串 */
    uint8_t in_data[BLE_MTU];               /**< 接收数据缓冲区(防止被篡改) */
    Mailbox_Struct *mailbox;                /**< 任务通信邮箱 */
    Clock_Struct *verify_timer;             /**< 验证超时定时器 */
};

/*==============================================================================
 * 7. 验证协议结构体(4步握手)
 *============================================================================*/

#pragma pack(1)

/**
 * @struct auth_step1_t
 * @brief 步骤1: APP发送设备ID和SM2签名
 * @details APP使用自己的私钥对设备ID进行签名，程控仪用公钥验证
 */
typedef struct {
    uint8_t fh;                     /**< 帧头: 0xC3 */
    uint8_t app_dev_id[32];         /**< APP安装设备的设备ID(32字节) */
    uint8_t *ciphertext;            /**< 签名数据(可变长度) */
    uint8_t decode_dev_id[32];      /**< 解密后的设备ID(验证用) */
    size_t decode_dev_id_len;       /**< 解密数据长度 */
    size_t ciphertext_len;          /**< 签名数据长度 */
    uint8_t ft;                     /**< 帧尾: 0x3C */
} auth_step1_t;

/**
 * @struct auth_step2_t
 * @brief 步骤2: 程控仪验证APP合法性响应
 */
typedef struct {
    uint8_t fh;                     /**< 帧头: 0xC3 */
    uint8_t app_dev_id[32];         /**< 返回APP的设备ID(验证通过) */
    uint8_t ft;                     /**< 帧尾: 0x3C */
} auth_step2_t;

/**
 * @struct auth_step3_t
 * @brief 步骤3: APP发送用key1加密的MAC地址数据
 */
typedef struct {
    uint8_t fh;                      /**< 帧头: 0xC3 */
    union {
        struct {
            uint8_t random_data[31]; /**< 31字节随机填充数据 */
            uint8_t cmac[17];        /**< 程控仪蓝牙MAC地址字符串(17字节) */
        };
        uint8_t encrypted_data[48];  /**< 合并后的48字节待解密数据 */
    };
    uint8_t ft;                      /**< 帧尾: 0x3C */
} auth_step3_t;

/**
 * @struct auth_step3_plain_t
 * @brief 步骤3解密后的48字节随机数据
 */
typedef struct {
    uint8_t random_bytes[48];       /**< 48字节随机数(内含8字节用于生成key2) */
} auth_step3_plain_t;

/**
 * @struct auth_step4_t
 * @brief 步骤4: APP发送用key1加密的code2和时间戳
 */
typedef struct {
    uint8_t fh;                     /**< 帧头: 0xC3 */
    uint8_t encrypted_data[48];     /**< 加密数据(42字节随机数据+6字节时间) */
    uint8_t ft;                     /**< 帧尾: 0x3C */
} auth_step4_t;

/**
 * @struct auth_step4_plain_t
 * @brief 步骤4解密后的明文数据结构
 */
typedef struct {
    uint8_t random_data[42];        /**< 42字节随机数据(内含8字节用于key2后半部分) */
    union {
        struct {
            uint8_t year;           /**< 年份(后2位: 24-99) */
            uint8_t month;          /**< 月份(1-12) */
            uint8_t day;            /**< 日期(1-31) */
            uint8_t hour;           /**< 小时(0-23) */
            uint8_t minute;         /**< 分钟(0-59) */
            uint8_t second;         /**< 秒钟(0-59) */
        };
        uint8_t time[6];            /**< 时间字节数组 */
    };
} auth_step4_plain_t;

/**
 * @struct dev_info_t
 * @brief 设备信息结构体(步骤6，使用key2加密传输)
 */
typedef struct {
    uint8_t  padding[11];           /**< 11字节随机填充 */
    uint32_t patient_id;            /**< 患者ID(全0表示未绑定) */
    char     api_version[6];        /**< API协议版本 */
    char     controller_sn[11];     /**< 程控仪序列号 */
    char     controller_sw[6];      /**< 程控仪软件版本 */
    char     controller_hw[6];      /**< 程控仪硬件版本 */
    char     stimulator_sn[12];     /**< 刺激器序列号 */
    char     stimulator_sw[6];      /**< 刺激器软件版本 */
    char     stimulator_hw[6];      /**< 刺激器硬件版本 */
    char     ble_sw[6];             /**< 蓝牙模块软件版本 */
    uint8_t  stimulator_mac[6];     /**< 刺激器MAC地址 */
} dev_info_t;

/*==============================================================================
 * 8. 通信帧结构体
 *============================================================================*/

/**
 * @struct ble_packet_t
 * @brief BLE数据包完整结构体
 */
typedef struct {
    uint8_t  header;                /**< 帧头: 0xC3 */
    uint16_t pack_indx;             /**< 包序号(用于多包传输排序) */
    uint8_t  length;                /**< 帧长度(命令码+可选数据) */
    uint8_t  cmd;                   /**< 命令码(0x01-0x7F) */
    uint8_t  data[BLE_MTU];         /**< 可选数据(最大255字节) */
    uint16_t crc;                   /**< CRC16校验值 */
    uint8_t  tail;                  /**< 帧尾: 0x3C */
} ble_packet_t;

/**
 * @struct ble_error_packet_t
 * @brief 错误响应帧结构体
 */
typedef struct {
    uint8_t  header;                /**< 帧头: 0xC3 */
    uint16_t pack_indx;             /**< 包序号(用于多包传输排序) */
    uint8_t  length;                /**< 帧长度(固定为2) */
    uint8_t  cmd;                   /**< 命令码(最高位置1表示错误响应) */
    uint8_t  error_code;            /**< 错误码 */
    uint16_t crc;                   /**< CRC16校验值 */
    uint8_t  tail;                  /**< 帧尾: 0x3C */
} ble_error_packet_t;

/**
 * @struct ble_recv_frame_t
 * @brief 接收帧结构体(不含帧头帧尾，用于解析)
 */
typedef struct {
    uint8_t seq_low;                /**< 帧序号低字节 */
    uint8_t seq_high;               /**< 帧序号高字节 */
    uint8_t length;                 /**< 帧长度 */
    uint8_t cmd;                    /**< 命令码 */
    uint8_t data[];                 /**< 可变长度数据(柔性数组) */
} ble_recv_frame_t;

#pragma pack()

/*==============================================================================
 * 9. 外部变量声明
 *============================================================================*/

extern struct app_ble_client ble_client;  /**< BLE客户端全局实例 */

/*==============================================================================
 * 10. 外部函数声明
 *============================================================================*/

/**
 * @brief BLE任务主函数
 * @param arg0 任务参数0(未使用)
 * @param arg1 任务参数1(未使用)
 */
void app_ble_task(UArg arg0, UArg arg1);

/**
 * @brief 将手机发来的BLE数据发送到任务线程处理
 * @param data 接收到的数据指针
 * @param len 数据长度
 * @return 0:成功, -1:失败
 */
int user_get_ble_data_send_mailbox(uint8_t *data, uint16_t len);

/**
 * @brief 发送BLE连接状态变化通知
 * @param ble_state 新的连接状态值
 * @return 0:成功, -1:失败
 */
int user_send_mailbox_ble_state(uint8_t ble_state);

/**
 * @brief 保存BLE MAC地址(同时保存字节数组和字符串格式)
 * @param addr MAC地址字节数组(6字节)
 */
void user_save_ble_mac_addr(uint8_t *addr);

/**
 * @brief AES-128-CBC加解密函数
 * @param input 输入数据(长度必须是16的倍数)
 * @param size 输入数据大小
 * @param key 16字节密钥
 * @param mode AES_ENCRYPT或AES_DECRYPT
 * @return 加解密后的数据指针(需调用free释放)
 */
uint8_t *aes128_crypt_cbc(uint8_t *input, uint8_t size, uint8_t *key, uint8_t mode);

/**
 * @brief 向APP发送正常响应帧
 * @param cmd 命令码
 * @param frame_index 帧序号
 * @param data 可选数据指针(NULL表示无)
 * @param size 可选数据大小
 * @return 执行结果
 */
uint8_t send_normal_frame_to_app(uint8_t cmd, uint16_t frame_index,
                                               uint8_t *data, size_t size);

/**
 * @brief 向APP发送错误响应帧
 * @param cmd 命令码
 * @param err 错误类型
 */
void send_err_frame_to_app(uint8_t cmd, uint8_t err);

__attribute__((weak)) int process_command(uint8_t cmd, uint16_t indx, uint8_t *data, uint16_t len);

#endif /* __APP_VERIFY_BLE_H__ */
