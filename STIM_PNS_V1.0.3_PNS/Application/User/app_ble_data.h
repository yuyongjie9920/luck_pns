/*
 * app_ble_data.h
 *
 *  Created on: 2026年4月16日
 *      Author: BLE Protocol v1.0.0
 *
 *  Description: BLE通信协议头文件
 *               定义程控仪与APP之间的通信数据结构
 *               协议版本: v1.0.0
 *               参考文档: 《体外刺激器版程控仪与APP通信接口协议》
 */

#ifndef __APPLICATION_USER_APP_BLE_DATA_H__
#define __APPLICATION_USER_APP_BLE_DATA_H__

#include <stdint.h>
#include "stdlib.h"
#include "main_user.h"

/*==============================================================================
 * 功能开关宏定义
 *============================================================================*/
#define BLE_TEST_MODE  1

#define MAX_PROGRAM_GROUP_COUNT 8           /**< 最大程序组数量 */
#define MAX_PROGRAM_PER_GROUP   4           /**< 每组最大程序数量 */
#define MAX_AREA_PER_PROGRAM    4          /**< 每程序最大区域数量 */

#define SAVE_PACK_MUB           "pack"      /**< 包信息存储键名 */
#define SAVE_DICTIONARIES_DIR   "dic"       /**< 字典数据存储目录名 */
#define SAVE_PATIENT_DIR        "pat"       /**< 患者数据存储目录名 */
#define SAVE_CONFIG_DIR         "cfg"       /**< 配置数据存储目录名 */

#define MIN_VRECP               0           /**< 最小Vrecp电压(mV) */
#define MAX_VRECP               22500       /**< 最大Vrecp电压(mV) */

#define DEFAULT_TEMP_LIMIT      50          /**< 默认温度上限(℃) */
#define MIN_TEMP_LIMIT          30          /**< 最小温度上限(℃) */
#define MAX_TEMP_LIMIT          60          /**< 最大温度上限(℃) */

/**
 * @brief 分包存储信息结构体
 * @note 用于管理大数据分包传输
 */
typedef struct {
    uint16_t pack_count;            ///< 包的数量
} pack_info_t;

/**
 * @brief 记录信息结构体
 */
typedef struct {
    uint16_t old_index;             ///< 记录旧的包号
    uint16_t old_cmd_index;         ///< 记录旧包的cmd
    uint8_t  new_cmd_flag;          ///< 标记新包
} record_t;

/**
 * @brief 存储信息结构体
 */
typedef struct {
    pack_info_t dictionaries;       ///< 字典包数
    pack_info_t patient;            ///< 患者包数
    pack_info_t config;             ///< 配置包数
} save_info_t;

/**
 * @brief BLE数据管理结构体
 */
typedef struct {
    save_info_t info;               ///< 存储信息
    record_t    record;             ///< 记录信息
} user_ble_data_t;


/*==============================================================================
 * 错误码定义 (第5章)
 *============================================================================*/
/**
 * @brief 命令执行错误码 (5.3节)
 * @note 命令执行失败时，响应帧中携带这些错误码
 */
typedef enum {
    ERR_GENERAL             = 0x21,  ///< 一般错误
    ERR_PDU_LENGTH          = 0x22,  ///< 可选数据段字节长度错误
    ERR_PARAM_RANGE         = 0x23,  ///< 参数不在正常范围内
    ERR_TIMEOUT             = 0x24,  ///< 操作超时
    ERR_NO_MEMORY           = 0x25,  ///< 内存不足
    ERR_BUSY                = 0x26,  ///< 系统忙
    ERR_IO                  = 0x27,  ///< I/O错误
    ERR_IO_FULL             = 0x28,  ///< I/O资源满
    ERR_RESOURCE_EMPTY      = 0x29,  ///< 请求资源空
    ERR_STIM_NOT_CONNECTED  = 0x2A,  ///< 刺激器未连接
    ERR_STIM_COMM_FAIL      = 0x2B,  ///< 与刺激器通信失败(超时)
    ERR_STIM_ADDR_INVALID   = 0x2C,  ///< 刺激器地址不合法(未配置DevID)
    ERR_CONTROLLER_OVERTEMP = 0x2D,  ///< 程控仪温度过高
    ERR_STIM_PARAM          = 0x2E,  ///< 发给刺激器的参数错误
    ERR_CHARGING_STIM       = 0x2F,  ///< 充电过程中不能刺激
    ERR_CHARGER_SHUTDOWN    = 0x31,  ///< 插入充电器导致设备关机(通知断开原因)
    ERR_ANTENNA_DISCONNECT  = 0x32,  ///< 天线断连/损坏
    ERR_GROUP_NOT_EXIST     = 0x41,  ///< 本地程序组不存在
    ERR_PROG_NOT_EXIST      = 0x42,  ///< 本地程序不存在
    ERR_GROUP_NOT_OPEN      = 0x43,  ///< 本地程序组未打开
    ERR_PROG_NOT_OPEN       = 0x44,  ///< 本地程序未打开
} exec_error_code_t;


/*==============================================================================
 * 枚举类型定义
 *============================================================================*/

/**
 * @brief 刺激器运行状态枚举
 */
typedef enum {
    STIM_STATE_INIT           = 0,  ///< 初始状态
    STIM_STATE_RUNNING        = 1,  ///< 正在刺激
    STIM_STATE_PAUSED         = 2,  ///< 暂停刺激
    STIM_STATE_ABNORMAL_PAUSE = 3,  ///< 异常暂停刺激
    STIM_STATE_STOPPED        = 4,  ///< 停止刺激
    STIM_STATE_ABNORMAL_STOP  = 5,  ///< 异常停止刺激
} stim_run_state_t;

/**
 * @brief 刺激器连接状态枚举
 */
typedef enum {
    STIM_CONN_DISCONNECTED  = 0,  ///< 未连接
    STIM_CONN_CONNECTED     = 1,  ///< 已连接
} stim_connect_status_t;

/**
 * @brief 程序组模式枚举
 */
typedef enum {
    GROUP_MODE_SIMPLE       = 0,  ///< 简单模式
    GROUP_MODE_SERIES       = 1,  ///< 串联模式
} group_mode_t;

/**
 * @brief 程序模式枚举 (6.3节)
 */
typedef enum {
    PROG_MODE_LOW_FREQ      = 0,  ///< 传统低频
    PROG_MODE_LOW_FREQ_TDMA = 1,  ///< 传统低频分时复用
    PROG_MODE_HIGH_FREQ     = 2,  ///< 传统高频
    PROG_MODE_BURST         = 3,  ///< 爆发模式
} program_mode_t;

/**
 * @brief 循环模式枚举 (6.3节)
 */
typedef enum {
    CYCLE_MODE_CONTINUOUS   = 0,  ///< 一直刺激
    CYCLE_MODE_TIMED        = 1,  ///< 定时刺激
    CYCLE_MODE_CYCLE        = 2,  ///< 循环刺激
} cycle_mode_t;

/**
 * @brief 设备级开关状态枚举 (6.7节)
 */
typedef enum {
    DEVICE_SWITCH_OFF       = 0,  ///< 关闭
    DEVICE_SWITCH_ON        = 1,  ///< 开启
} device_switch_state_t;

/*==============================================================================
 * 命令码定义 (第4章)
 *============================================================================*/
#define CMD_01_SET_PROGRAM_GROUP        0x01    /* 6.1 设置程序组参数 */
#define CMD_02_GET_PROGRAM_GROUP        0x02    /* 6.2 获取程序组参数 */
#define CMD_03_SET_PROGRAM_PARAM        0x03    /* 6.3 设置程序参数 */
#define CMD_04_GET_PROGRAM_PARAM        0x04    /* 6.4 获取程序参数 */
#define CMD_05_SET_PROGRAM_SWITCH       0x05    /* 6.5 设置程序开关 */
#define CMD_06_GET_PROGRAM_SWITCH       0x06    /* 6.6 获取程序开关 */
#define CMD_07_SET_DEVICE_SWITCH        0x07    /* 6.7 设置开关状态(设备级) */
#define CMD_08_GET_DEVICE_SWITCH        0x08    /* 6.8 获取开关状态 */
#define CMD_09_START_STIM               0x09    /* 6.9 开始刺激 */
#define CMD_0A_STOP_STIM                0x0A    /* 6.10 停止刺激 */
#define CMD_0B_EMERGENCY_STOP           0x0B    /* 6.11 紧急停止 */
#define CMD_0C_GET_DEVICE_STATUS        0x0C    /* 6.12 获取设备状态 */
#define CMD_0D_SET_FW_SIZE              0x0D    /* 6.13 设置升级固件大小 */
#define CMD_0E_SEND_FW_PACKET           0x0E    /* 6.14 发送升级固件包 */
#define CMD_0F_GET_LOG                  0x0F    /* 6.15 获取日志信息 */
#define CMD_10_CLEAR_LOG                0x10    /* 6.16 清除全部日志信息 */
#define CMD_11_GET_LOG_USAGE            0x11    /* 6.17 获取当前日志使用率 */
#define CMD_12_STOP_LOG                 0x12    /* 6.18 停止获取日志信息 */
#define CMD_13_DELETE_STIMULATOR        0x13    /* 6.19 删除某个刺激器的相关配置 */
#define CMD_14_CLEAR_PROGRAM            0x14    /* 6.20 清除指定的程序组/程序/区域 */
#define CMD_15_GET_PROGRAM_COUNT        0x15    /* 6.21 获取程序组和程序ID */
#define CMD_16_ANTENNA_THRESHOLD        0x16    /* 6.22 修改天线位置判定条件 */
#define CMD_17_GET_STIM_VERSION         0x17    /* 6.23 获取刺激器版本 */
#define CMD_18_GET_IMPEDANCE            0x18    /* 6.24 获取刺激器电极一组阻抗值 */
#define CMD_19_GET_CONNECTED_STIM       0x19    /* 6.25 获取当前连接刺激器序列号 */
#define CMD_1A_RESERVED                 0x1A    /* 6.26 预留 */
#define CMD_1B_SEARCH_IMPEDANCE         0x1B    /* 6.27 检索刺激器电极一组阻抗值 */
#define CMD_1C_ANTENNA_DETECT           0x1C    /* 6.28 设置开始和暂停天线检测 */
#define CMD_1D_RESERVED                 0x1D    /* 6.29 预留 */
#define CMD_1E_TEMP_LIMIT               0x1E    /* 6.30 设置或获取刺激器温度上限值 */
#define CMD_1F_CLEAR_ALL_CONFIG         0x1F    /* 6.31 清除所有配置 */
#define CMD_20_FACTORY_RESET            0x20    /* 6.32 恢复出厂设置 */
#define CMD_21_SET_PATIENT_INFO         0x21    /* 6.33 设置患者植入信息 */
#define CMD_22_SET_DICT_CONFIG          0x22    /* 6.34 设置字典配置信息 */
#define CMD_23_GET_PATIENT_INFO         0x23    /* 6.35 获取患者植入信息 */
#define CMD_24_GET_DICT_CONFIG          0x24    /* 6.36 获取字典配置信息 */
#define CMD_25_SET_GENERAL_CONFIG       0x25    /* 6.37 设置配置信息 */
#define CMD_26_GET_GENERAL_CONFIG       0x26    /* 6.38 获取配置信息 */
#define CMD_27_DELETE_STIMULATOR_CONFIG 0x27    /* 6.39 删除某个刺激器的相关配置 */
#define CMD_28_RF_POWER                 0x28    /* 6.40 设置和获取射频功率值 */
#define CMD_29_GET_VRECP                0x29    /* 6.41 获取刺激器Vrecp电压 */

#pragma pack(1)  /* 设置1字节对齐，避免结构体填充 */

/*==============================================================================
 * 章节 6.1: 命令码0x01 - 设置程序组参数
 *============================================================================*/

/**
 * @brief 0x01命令发送帧结构体
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4) + progID(1) +
 *              test_mode(1) + name(15) + group_mode(1) + group_switch(1) +
 *              ID(4) + ver(8) + delete_flag(1)
 *       总长度: 1+12+4+1+1+15+1+1+4+8+1 = 49字节
 */
typedef struct {
    uint8_t  stim_sn[12];           ///< 刺激器SN号(出厂唯一识别号)
    uint32_t stim_efuse;            ///< 刺激器efuse_id(芯片唯一识别码)
    uint8_t  progID;                ///< 程序组ID(高4位:组ID 1-8, 低4位:程序ID 0)
    uint8_t  test_mode;             ///< 测试模式(0:正常模式)
    uint8_t  name[15];              ///< 程序组名称(UTF-8编码)
    uint8_t  group_mode;            ///< 程序组模式: 0=简单模式, 1=串联模式
    uint8_t  group_switch;          ///< 程序组开关: 0=关, 1=开
    uint32_t id;                    ///< 数据库ID
    struct {
        uint32_t doctorTimestamp;   ///< 医生修改时间戳
        uint32_t patientTimestamp;  ///< 患者修改时间戳
    } ver;                          ///< 程序组版本号(8字节)
    uint8_t  delete_flag;           ///< 删除标识: 0=未删除, 1=已删除
} cmd_01_set_group_t;

/*==============================================================================
 * 章节 6.2: 命令码0x02 - 获取程序组参数
 *============================================================================*/

/**
 * @brief 0x02命令发送帧
 * @note 帧格式: cmd(1) + progID(1)
 *       总长度: 2字节
 */
typedef struct {
    uint8_t progID;                 ///< 程序组ID
} cmd_02_get_group_t;

/**
 * @brief 0x02命令正常响应帧
 * @note 帧格式同0x01发送帧(49字节)
 */
typedef cmd_01_set_group_t cmd_02_ack_group_t;

/*==============================================================================
 * 章节 6.3: 命令码0x03 - 设置程序参数
 *============================================================================*/

/**
 * @brief 患者限度结构体 (21字节)
 */
typedef struct {
    uint16_t current_down;          ///< 电流下限(uA, 100-20000)
    uint16_t current_up;            ///< 电流上限(uA, 100-20000)
    uint16_t doc_current;           ///< 医生设置电流(uA, 100-20000)
    uint16_t pulse_width_down;      ///< 脉宽下限(us, 10-1000)
    uint16_t pulse_width_up;        ///< 脉宽上限(us, 10-1000)
    uint16_t doc_pulse_width;       ///< 医生设置脉宽(us, 10-1000)
    uint16_t freq_down;             ///< 频率下限(Hz, 1-10000)
    uint16_t freq_up;               ///< 频率上限(Hz, 1-10000)
    uint16_t doc_freq;              ///< 医生设置频率(Hz, 1-10000)
    uint8_t  current_limit_switch;  ///< 电流限制开关: 0=关, 1=开
    uint8_t  pulse_width_limit_switch; ///< 脉宽限制开关: 0=关, 1=开
    uint8_t  freq_limit_switch;     ///< 频率限制开关: 0=关, 1=开
} patient_limit_t;

/**
 * @brief 开关合集位定义 (2字节)
 * @note 位0-15分别表示不同的开关状态
 */
typedef struct {
    uint16_t prog_switch            : 1;  ///< 位0: 程序开关
    uint16_t test_mode              : 1;  ///< 位1: 测试模式(不保存刺激参数)
    uint16_t cycle_time_switch      : 1;  ///< 位2: 患者循环开启时间可调开关
    uint16_t cycle_interval_switch  : 1;  ///< 位3: 患者循环间歇时间可调开关
    uint16_t run_time_switch        : 1;  ///< 位4: 患者运行时间可调开关
    uint16_t interval_time_switch   : 1;  ///< 位5: 患者间歇时间可调开关
    uint16_t reserved6              : 1;  ///< 保留
    uint16_t reserved7              : 1;  ///< 保留
    uint16_t reserved8              : 1;  ///< 保留
    uint16_t reserved9              : 1;  ///< 保留
    uint16_t reserved10             : 1;  ///< 保留
    uint16_t reserved11             : 1;  ///< 保留
    uint16_t reserved12             : 1;  ///< 保留
    uint16_t reserved13             : 1;  ///< 保留
    uint16_t reserved14             : 1;  ///< 保留
    uint16_t doctor_mode            : 1;  ///< 位15: 医生模式标记(1=医生端设置)
} switch_bits_t;

/**
 * @brief 程序参数结构体 (18字节)
 */
typedef struct {
    uint8_t  program_mode;          ///< 程序模式(见program_mode_t枚举)
    uint8_t  cycle_mode;            ///< 循环模式(见cycle_mode_t枚举)
    uint16_t run_time;              ///< 患者运行时间(min, 0-3600)
    uint16_t run_interval_time;     ///< 患者运行停止时间(min, 0-3600)
    uint16_t cycle_start_time;      ///< 患者循环开始时间(s, 0-3600)
    uint16_t cycle_interval;        ///< 患者循环间歇时间(s, 0-3600)
    uint16_t doc_run_time;          ///< 医生运行时间(min, 0-3600)
    uint16_t doc_run_interval_time; ///< 医生运行停止时间(min, 0-3600)
    uint16_t doc_cycle_start_time;  ///< 医生循环开始时间(s, 0-3600)
    uint16_t doc_cycle_interval;    ///< 医生循环间歇时间(s, 0-3600)
} program_params_t;

/**
 * @brief 区域参数结构体 (51字节)
 */
typedef struct {
    uint8_t  area_id;               ///< 区域ID(高4位数量,低4位ID)
    patient_limit_t limit;          ///< 患者限度(21字节)
    uint8_t  area_switch;           ///< 区域开关: 0=关, 1=开
    uint8_t  soft_switch;           ///< 软启动开关: 0=关, 1=开
    uint8_t  soft_start;            ///< 软启动时间(s, 1-10)
    uint16_t stimPolar;             ///< 电极极性(位0-15: 1=源, 0=沉)
    uint16_t stimOn;                ///< 电极开关(位0-15: 1=开, 0=关)
    uint16_t ele_percent[8];        ///< 电极电流百分比(16个电极)
    uint8_t  pulse_width_ratio;     ///< 前后脉宽比(高4位分子,低4位分母)
    uint8_t  interPhase;            ///< 正负脉宽间隔(us, 1-255)
    uint16_t current;               ///< 电流(uA, 0-20000)
    uint16_t pulse_width;           ///< 脉宽(us, 10-1000)
} area_params_t;

/**
 * @brief 刺激参数结构体 (7 + 53*N字节)
 */
typedef struct {
    uint16_t frequency;             ///< 简单刺激频率(Hz, 1-30000)
    uint16_t burst_frequency;       ///< 爆发内频(Hz, 1-30000)
    uint8_t  stimBurstNum;          ///< 爆发个数(1-10)
    area_params_t areas[4];         ///< 区域参数数组(N个)
} stim_params_t;

/**
 * @brief 0x03命令发送帧结构体
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4) + progID(1) +
 *              switch_bits(2) + program_params(18) + stim_params(可变)
 */
typedef struct {
    uint8_t         stim_sn[12];    ///< 刺激器SN号
    uint32_t        stim_efuse;     ///< 刺激器efuse_id
    uint8_t         progID;         ///< 程序组ID(高4位), 程序ID(低4位)
    switch_bits_t   switches;       ///< 开关合集(2字节)
    program_params_t prog_params;   ///< 程序参数(18字节)
    stim_params_t   stim_params;    ///< 刺激参数
} cmd_03_set_program_t;

/*==============================================================================
 * 章节 6.4: 命令码0x04 - 获取程序参数
 *============================================================================*/

/**
 * @brief 0x04命令发送帧
 * @note 帧格式: cmd(1) + progID(1)
 *       总长度: 2字节
 */
typedef struct {
    uint8_t progID;                 ///< 程序ID
} cmd_04_get_program_t;

/**
 * @brief 0x04命令正常响应帧
 * @note 帧格式同0x03发送帧(可变长度)
 */
typedef cmd_03_set_program_t cmd_04_ack_program_t;


/*==============================================================================
 * 章节 6.5: 命令码0x05 - 设置程序开关
 *============================================================================*/

/**
 * @brief 0x05命令发送帧结构体
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4) + progID(2) + prog_switch(1)
 *       总长度: 1+12+4+2+1 = 20字节
 */
typedef struct {
    uint8_t  stim_sn[12];           ///< 刺激器SN号
    uint32_t stim_efuse;            ///< 刺激器efuse_id
    uint16_t progID;                ///< 高8位:程序组ID, 次4位:程序ID, 低4位:区域ID
    uint8_t  prog_switch;           ///< 开关状态: 0=关, 1=开
} cmd_05_set_switch_t;

/*==============================================================================
 * 章节 6.6: 命令码0x06 - 获取程序开关
 *============================================================================*/

/**
 * @brief 0x06命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x06
} cmd_06_get_switch_t;

/**
 * @brief 0x06命令正常响应帧
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4) + progID(2) + prog_switch(1)
 *       总长度: 1+12+4+2+1 = 20字节
 */
typedef struct {
    uint8_t  stim_sn[12];           ///< 刺激器SN号
    uint32_t stim_efuse;            ///< 刺激器efuse_id
    uint16_t progID;                ///< 程序组ID/程序ID/区域ID
    uint8_t  prog_switch;           ///< 开关状态
} cmd_06_ack_switch_t;

/*==============================================================================
 * 章节 6.7: 命令码0x07 - 设置开关状态(设备级)
 *============================================================================*/

/**
 * @brief 设备级开关合集位定义
 * @note 开关状态值: 0=关, 1=开
 */
typedef struct {
    uint8_t rf_disable_switch       : 1;  ///< 位0: 射频空载开关
    uint8_t surgery_mode_switch     : 1;  ///< 位1: 手术模式开关
    uint8_t rf_blind_mode_switch    : 1;  ///< 位2: 盲发模式开关
    uint8_t log_zip_switch          : 1;  ///< 位3: 日志压缩开关
    uint8_t auto_open_stim_switch   : 1;  ///< 位4: 开机自动刺激开关
    uint8_t reserved5               : 1;  ///< 保留
    uint8_t reserved6               : 1;  ///< 保留
    uint8_t reserved7               : 1;  ///< 保留
} device_switch_bits_t;

/**
 * @brief 0x07命令发送帧结构体
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4) + switch_bits(1)
 *       总长度: 1+12+4+1 = 18字节
 */
typedef struct {
    uint8_t  stim_sn[12];           ///< 刺激器SN号
    uint32_t stim_efuse;            ///< 刺激器efuse_id
    uint8_t  switch_bits;           ///< 开关状态位
} cmd_07_set_device_switch_t;

/*==============================================================================
 * 章节 6.8: 命令码0x08 - 获取开关状态
 *============================================================================*/

/**
 * @brief 0x08命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x08
} cmd_08_get_device_switch_t;

/**
 * @brief 0x08命令正常响应帧
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4) + progID(2) + switches(3)
 *       总长度: 1+12+4+2+3 = 22字节
 */
typedef struct {
    uint8_t  stim_sn[12];           ///< 刺激器SN号
    uint32_t stim_efuse;            ///< 刺激器efuse_id
    uint16_t progID;                ///< 程序组ID/程序ID/区域ID
    uint8_t  switches[3];           ///< 开关合集(24位)
} cmd_08_ack_device_switch_t;

/*==============================================================================
 * 章节 6.9: 命令码0x09 - 开始刺激
 *============================================================================*/

/**
 * @brief 0x09命令发送帧
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4) + progID(1)
 *       总长度: 1+12+4+1 = 18字节
 */
typedef struct {
    uint8_t  stim_sn[12];           ///< 刺激器SN号
    uint32_t stim_efuse;            ///< 刺激器efuse_id
    uint8_t  progID;                ///< 程序组ID
} cmd_09_start_stim_t;

/*==============================================================================
 * 章节 6.10: 命令码0x0A - 停止刺激
 *============================================================================*/

/**
 * @brief 0x0A命令发送帧
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4)
 *       总长度: 1+12+4 = 17字节
 */
typedef struct {
    uint8_t  stim_sn[12];           ///< 刺激器SN号
    uint32_t stim_efuse;            ///< 刺激器efuse_id
} cmd_0a_stop_stim_t;

/*==============================================================================
 * 章节 6.11: 命令码0x0B - 紧急停止
 *============================================================================*/

/**
 * @brief 0x0B命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x0B
} cmd_0b_emergency_stop_t;

/*==============================================================================
 * 章节 6.12: 命令码0x0C - 获取设备状态
 *============================================================================*/

/**
 * @brief 0x0C命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x0C
} cmd_0c_get_status_t;

/**
 * @brief 0x0C命令正常响应帧
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4) + stimRun(1) + stimCon(1) +
 *              batteryCapacity(1) + batteryTemperature(1) + batteryHealth(1) +
 *              chargingState(1) + progID(1) + left_time(2)
 *       总长度: 1+12+4+1+1+1+1+1+1+1+2 = 26字节
 */
typedef struct {
    uint8_t  stim_sn[12];           ///< 刺激器SN号
    uint32_t stim_efuse;            ///< 刺激器efuse_id
    uint8_t  stimRun;               ///< 刺激器运行状态
    uint8_t  stimCon;               ///< 刺激器连接状态(0=未连接,1=已连接)
    uint8_t  batteryCapacity;       ///< 电池电量百分比(0-100)
    int8_t   batteryTemperature;    ///< 电池温度(℃, -100-100)
    uint8_t  batteryHealth;         ///< 电池健康状况(1=健康)
    uint8_t  chargingState;         ///< 充电状态(0=充电中,1=充电完成/未充电)
    uint8_t  progID;                ///< 程序组ID和程序ID
    uint16_t left_time;             ///< 剩余治疗时间(min)
} cmd_0c_ack_status_t;

/*==============================================================================
 * 章节 6.13: 命令码0x0D - 设置升级固件大小
 *============================================================================*/

/**
 * @brief 0x0D命令发送帧
 * @note 帧格式: cmd(1) + size(4)
 *       总长度: 5字节
 */
typedef struct {
    uint32_t size;                  ///< 升级固件总长度(字节)
} cmd_0d_set_fw_size_t;

/*==============================================================================
 * 章节 6.14: 命令码0x0E - 发送升级固件包
 *============================================================================*/

/**
 * @brief 0x0E命令发送帧
 * @note 帧格式: cmd(1) + offset(4) + Data(128)
 *       总长度: 1+4+128 = 133字节
 */
typedef struct {
    uint32_t offset;                ///< 固件偏移(在整个固件包中的位置)
    uint8_t  data[128];             ///< 固件数据(不足128补0)
} cmd_0e_send_fw_packet_t;

/*==============================================================================
 * 章节 6.15: 命令码0x0F - 获取日志信息
 *============================================================================*/

/**
 * @brief 0x0F命令发送帧
 * @note 帧格式: cmd(1) + offset(4) + size(4)
 *       总长度: 9字节
 */
typedef struct {
    uint32_t offset;                ///< 日志字符偏移量(4字节对齐)
    uint32_t size;                  ///< 请求日志字符数(1-42840, 4字节对齐)
} cmd_0f_get_log_t;

/*==============================================================================
 * 章节 6.16: 命令码0x10 - 清除全部日志信息
 *============================================================================*/

/**
 * @brief 0x10命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x10
} cmd_10_clear_log_t;

/*==============================================================================
 * 章节 6.17: 命令码0x11 - 获取当前日志使用率
 *============================================================================*/

/**
 * @brief 0x11命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x11
} cmd_11_get_log_usage_t;

/**
 * @brief 0x11命令正常响应帧
 * @note 帧格式: cmd(1) + percent(1) + log_size(4)
 *       总长度: 6字节
 */
typedef struct {
    uint8_t  percent;               ///< 当前日志使用率(%)
    uint32_t log_size;              ///< 当前已存储的日志大小(字节)
} cmd_11_ack_log_usage_t;

/*==============================================================================
 * 章节 6.18: 命令码0x12 - 停止获取日志信息
 *============================================================================*/

/**
 * @brief 0x12命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x12
} cmd_12_stop_log_t;

/*==============================================================================
 * 章节 6.19: 命令码0x13 - 删除某个刺激器的相关配置
 *============================================================================*/

/**
 * @brief 0x13命令发送帧
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4)
 *       总长度: 1+12+4 = 17字节
 */
typedef struct {
    uint8_t  stim_sn[12];           ///< 刺激器SN号
    uint32_t stim_efuse;            ///< 刺激器efuse_id
} cmd_13_delete_stimulator_t;

/*==============================================================================
 * 章节 6.20: 命令码0x14 - 清除指定的程序组/程序/区域
 *============================================================================*/

/**
 * @brief 0x14命令发送帧
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4) + progID(1) + area_id(1)
 *       总长度: 1+12+4+1+1 = 19字节
 */
typedef struct {
    uint8_t  stim_sn[12];           ///< 刺激器SN号
    uint32_t stim_efuse;            ///< 刺激器efuse_id
    uint8_t  progID;                ///< 程序组ID
    uint8_t  area_id;               ///< 区域ID
} cmd_14_clear_program_t;

/*==============================================================================
 * 章节 6.21: 命令码0x15 - 获取程序组和程序ID
 *============================================================================*/

/**
 * @brief 0x15命令发送帧
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4)
 *       总长度: 1+12+4 = 17字节
 */
typedef struct {
    uint8_t  stim_sn[12];           ///< 刺激器SN号
    uint32_t stim_efuse;            ///< 刺激器efuse_id
} cmd_15_get_program_count_t;

/**
 * @brief 0x15命令正常响应帧
 * @note 帧格式: cmd(1) + progID(N)
 *       总长度: 1+N字节
 */
typedef struct {
    uint8_t  progID[32];            ///< 程序组ID数组
} cmd_15_ack_program_count_t;

/*==============================================================================
 * 章节 6.22: 命令码0x16 - 修改天线位置判定条件
 *============================================================================*/

/**
 * @brief 0x16命令发送帧(设置)
 * @note 帧格式: cmd(1) + subCmd(1) + txPower(1) + voltage(1)
 *       总长度: 4字节
 */
typedef struct {
    uint8_t subCmd;                 ///< 子命令: 0x00=设置
    uint8_t txPower;                ///< 判定hmc输出功率衰减值(0-31)
    uint8_t voltage;                ///< 判定电压(V, 3-22)
} cmd_16_set_antenna_threshold_t;

/**
 * @brief 0x16命令正常响应帧(获取)
 * @note 帧格式: cmd(1) + txPower(1) + voltage(1)
 *       总长度: 3字节
 */
typedef struct {
    uint8_t txPower;                ///< 判定hmc输出功率衰减值(0-31)
    uint8_t voltage;                ///< 判定电压(V, 3-22)
} cmd_16_ack_antenna_threshold_t;

/*==============================================================================
 * 章节 6.23: 命令码0x17 - 获取刺激器版本
 *============================================================================*/

/**
 * @brief 0x17命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x17
} cmd_17_get_stim_version_t;

/**
 * @brief 0x17命令正常响应帧
 * @note 帧格式: cmd(1) + sSoftVer(6) + sHardVer(6)
 *       总长度: 13字节
 */
typedef struct {
    uint8_t sSoftVer[6];            ///< 刺激器软件版本(主+次+次次各2字节)
    uint8_t sHardVer[6];            ///< 刺激器硬件版本(主+次+次次各2字节)
} cmd_17_ack_stim_version_t;

/*==============================================================================
 * 章节 6.24: 命令码0x18 - 获取刺激器电极一组阻抗值
 *============================================================================*/

/**
 * @brief 0x18命令发送帧
 * @note 帧格式: cmd(1) + stimID(1) + stimSerialNum(8) + pole(1)
 *       总长度: 1+1+8+1 = 11字节
 */
typedef struct {
    uint8_t  stimID;                ///< 刺激器编号(1-2)
    uint8_t  stimSerialNum[8];      ///< 刺激器efuse_id
    uint8_t  pole;                  ///< 参考电极(1-16)
} cmd_18_get_impedance_t;

/**
 * @brief 0x18命令正常响应帧
 * @note 帧格式: cmd(1) + 16个电阻值(各2字节)
 *       总长度: 1+32 = 33字节
 */
typedef struct {
    uint16_t resistance[16];        ///< 16个电极的电阻值(Ω)
} cmd_18_ack_impedance_t;

/*==============================================================================
 * 章节 6.25: 命令码0x19 - 获取当前连接刺激器序列号
 *============================================================================*/

#if BLE_TEST_MODE
// 获取设备状态响应帧（正常接收）
typedef struct {
    uint8_t cmd;                                // 0x0C
    uint8_t stim_sn[12];                        // 刺激器SN号
    uint8_t stim_efuse[4];                      // 刺激器efuse_id
    uint8_t stimRun;                            // 刺激器运行状态
    uint8_t stimCon;                            // 刺激器连接状态
    uint8_t batteryCapacity;                    // 电池电量百分比(%)
    int8_t batteryTemperature;                  // 电池温度(℃)
    uint8_t batteryHealth;                      // 电池健康状况
    uint8_t chargingState;                      // 电池充电状态
    uint8_t progID;                             // 对应的程序组ID (0-8)
    uint16_t curAmp[8];                         // 4个程序 × 2根刺激器 = 8个幅度值
    // 或者使用: program_cur_amp_t curAmp[4];   // 4个程序的刺激幅度
} device_status_response_t;
#endif
/**
 * @brief 0x19命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x19
} cmd_19_get_connected_stim_t;


/*==============================================================================
 * 章节 6.27: 命令码0x1B - 检索刺激器电极一组阻抗值
 *============================================================================*/

/**
 * @brief 0x1B命令发送帧
 * @note 帧格式: cmd(1) + stimID(1) + stimSN(12) + pole(1) + amp(2)
 *       总长度: 1+1+12+1+2 = 17字节
 */
typedef struct {
    uint8_t  stimID;                ///< 刺激器编号(1-2)
    uint8_t  stimSN[12];            ///< 刺激器序列号
    uint8_t  pole;                  ///< 参考电极(1-16)
    uint16_t amp;                   ///< 刺激器输出幅度(uA, 0-20000)
} cmd_1b_search_impedance_t;

/*==============================================================================
 * 章节 6.28: 命令码0x1C - 设置开始和暂停天线检测
 *============================================================================*/

/**
 * @brief 0x1C命令发送帧
 * @note 帧格式: cmd(1) + state(1)
 *       总长度: 2字节
 */
typedef struct {
    uint8_t state;                  ///< 开关状态: 1=开始检测, 0=停止检测
} cmd_1c_antenna_detect_t;

/**
 * @brief 天线检测数据推送帧 (程控仪主动推送)
 * @note 帧格式: cmd(1) + RSSI(1) + vrecp(2) + state(1)
 *       总长度: 5字节
 */
typedef struct {
    uint8_t  RSSI;                  ///< 信号强度
    uint16_t vrecp;                 ///< 刺激器电压值(mV)
    uint8_t  state;                 ///< 合格状态
} cmd_1c_antenna_data_t;

/*==============================================================================
 * 章节 6.30: 命令码0x1E - 设置或获取刺激器温度上限值
 *============================================================================*/

/**
 * @brief 0x1E命令发送帧(设置)
 * @note 帧格式: cmd(1) + subCmd(1) + temperature(1)
 *       总长度: 3字节
 */
typedef struct {
    uint8_t subCmd;                 ///< 子命令: 0x00=设置
    uint8_t temperature;            ///< 温度上限(℃, 30-60)
} cmd_1e_set_temp_t;

/**
 * @brief 0x1E命令正常响应帧
 * @note 帧格式: cmd(1) + temperature(1)
 *       总长度: 2字节
 */
typedef struct {
    uint8_t temperature;            ///< 温度上限值(℃)
} cmd_1e_ack_temp_t;

/*==============================================================================
 * 章节 6.31: 命令码0x1F - 清除所有配置
 *============================================================================*/

/**
 * @brief 0x1F命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x1F
} cmd_1f_clear_all_t;

/*==============================================================================
 * 章节 6.32: 命令码0x20 - 恢复出厂设置
 *============================================================================*/

/**
 * @brief 0x20命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x20
} cmd_20_factory_reset_t;

/*==============================================================================
 * 章节 6.33: 命令码0x21 - 设置患者植入信息
 *============================================================================*/

/**
 * @brief 0x21命令发送帧(支持分包)
 * @note 帧格式: cmd(1) + info(N)
 */
typedef struct {
    uint8_t  *info;                 ///< 加密的植入信息数据(可变长度)
} cmd_21_set_patient_info_t;

/*==============================================================================
 * 章节 6.34: 命令码0x22 - 设置字典配置信息
 *============================================================================*/

/**
 * @brief 0x22命令发送帧(支持分包)
 * @note 帧格式: cmd(1) + info(N)
 */
typedef struct {
    uint8_t  *info;                 ///< 加密的字典配置信息(可变长度)
} cmd_22_set_dict_config_t;

/*==============================================================================
 * 章节 6.35: 命令码0x23 - 获取患者植入信息
 *============================================================================*/

/**
 * @brief 0x23命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x23
} cmd_23_get_patient_info_t;

/*==============================================================================
 * 章节 6.36: 命令码0x24 - 获取字典配置信息
 *============================================================================*/

/**
 * @brief 0x24命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x24
} cmd_24_get_dict_config_t;

/*==============================================================================
 * 章节 6.37: 命令码0x25 - 设置配置信息
 *============================================================================*/

/**
 * @brief 0x25命令发送帧(支持分包)
 * @note 帧格式: cmd(1) + info(N)
 *       没有info字段表示删除配置信息
 */
typedef struct {
    uint8_t  *info;                 ///< 加密的配置信息(可变长度)
} cmd_25_set_general_config_t;

/*==============================================================================
 * 章节 6.38: 命令码0x26 - 获取配置信息
 *============================================================================*/

/**
 * @brief 0x26命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t *info;                   ///< 命令码: 0x26
} cmd_26_get_general_config_t;

/*==============================================================================
 * 章节 6.39: 命令码0x27 - 删除某个刺激器的相关配置
 *============================================================================*/

/**
 * @brief 0x27命令发送帧
 * @note 帧格式: cmd(1) + stim_sn(12) + stim_efuse(4)
 *       总长度: 1+12+4 = 17字节
 */
typedef struct {
    uint8_t  stim_sn[12];           ///< 刺激器SN号
    uint32_t stim_efuse;            ///< 刺激器efuse_id
} cmd_27_delete_stimulator_t;

/*==============================================================================
 * 章节 6.40: 命令码0x28 - 设置和获取射频功率值
 *============================================================================*/

/**
 * @brief 功率档位配置结构体 (8字节)
 */
typedef struct {
    uint16_t freq;                  ///< 刺激频率(Hz)
    uint16_t pw;                    ///< 刺激脉宽(us)
    uint16_t amp;                   ///< 刺激幅度(uA)
    uint8_t  pwmDuty;               ///< 输出功率占空比(%, 0-100)
    uint8_t  dutyTxPower;           ///< 刺激器供能时HMC输出功率衰减值(0-31)
} power_group_t;

/**
 * @brief 0x28命令发送帧(设置)
 * @note 帧格式: cmd(1) + subCmd(1) + txPower(1) + pwmFreq(2) + powerGroup(5*8)
 *       总长度: 1+1+1+2+40 = 45字节
 */
typedef struct {
    uint8_t      subCmd;            ///< 子命令: 0=临时值, 1=永久值, 2=恢复默认
    uint8_t      txPower;           ///< HMC输出功率衰减值(0-31)
    uint16_t     pwmFreq;           ///< 输出功率频率(Hz, 1-10000)
    power_group_t powerGroup[5];    ///< 5个功率档位组
} cmd_28_set_rf_power_t;

/**
 * @brief 0x28命令发送帧(获取)
 * @note 帧格式: cmd(1) + subCmd(1)
 *       总长度: 2字节
 */
typedef struct {
    uint8_t      subCmd;            ///< 子命令: 0=临时值, 1=永久值
} cmd_28_get_rf_power_t;

/**
 * @brief 0x28命令正常响应帧(获取)
 * @note 帧格式: cmd(1) + txPower(1) + pwmFreq(2) + powerGroup(5*8)
 *       总长度: 1+1+2+40 = 44字节
 */
typedef struct {
    uint8_t      txPower;           ///< HMC输出功率衰减值(0-31)
    uint16_t     pwmFreq;           ///< 输出功率频率(Hz, 1-10000)
    power_group_t powerGroup[5];    ///< 5个功率档位组
} cmd_28_ack_rf_power_t;

/*==============================================================================
 * 章节 6.41: 命令码0x29 - 获取刺激器Vrecp电压
 *============================================================================*/

/**
 * @brief 0x29命令发送帧
 * @note 帧格式: cmd(1)
 */
typedef struct {
    uint8_t null;                   ///< 命令码: 0x29
} cmd_29_get_vrecp_t;

/**
 * @brief 0x29命令正常响应帧
 * @note 帧格式: cmd(1) + Vrecp(2)
 *       总长度: 3字节
 */
typedef struct {
    uint16_t Vrecp;                 ///< Vrecp电压(mV, 0-22500)
} cmd_29_ack_vrecp_t;

/*==============================================================================
 * 兼容旧版绑定相关结构体
 *============================================================================*/

/**
 * @brief 绑定刺激器命令发送帧(兼容旧版)
 */
typedef struct {
    uint8_t  stimModel[8];          ///< 刺激器型号
    uint8_t  stimSN[12];            ///< 刺激器序列号
    uint32_t stimSerialNum;         ///< 刺激器内置唯一识别码
} cmd_bind_stimulator_t;

/*==============================================================================
 * 外部变量声明
 *============================================================================*/
extern user_ble_data_t ble;

#pragma pack()  /* 恢复默认对齐 */

/*==============================================================================
 * 函数声明
 *============================================================================*/
int handle_set_program_group_01(uint8_t cmd, cmd_01_set_group_t *param, uint16_t len);
int handle_get_program_group_02(uint8_t cmd, cmd_02_get_group_t *param, uint16_t len);
int handle_set_program_param_03(uint8_t cmd, cmd_03_set_program_t *param, uint16_t len);
int handle_get_program_param_04(uint8_t cmd, cmd_04_get_program_t *param, uint16_t len);
int handle_set_program_switch_05(uint8_t cmd, cmd_05_set_switch_t *param, uint16_t len);
int handle_get_program_switch_06(uint8_t cmd, cmd_06_get_switch_t *param, uint16_t len);
int handle_set_device_switch_07(uint8_t cmd, cmd_07_set_device_switch_t *param, uint16_t len);
int handle_get_device_switch_08(uint8_t cmd, cmd_08_get_device_switch_t *param, uint16_t len);
int handle_start_stim_09(uint8_t cmd, cmd_09_start_stim_t *param, uint16_t len);
int handle_stop_stim_0a(uint8_t cmd, cmd_0a_stop_stim_t *param, uint16_t len);
int handle_emergency_stop_0b(uint8_t cmd, cmd_0b_emergency_stop_t *param, uint16_t len);
int handle_get_device_status_0c(uint8_t cmd, cmd_0c_get_status_t *param, uint16_t len);
int handle_set_fw_size_0d(uint8_t cmd, cmd_0d_set_fw_size_t *param, uint16_t len);
int handle_send_fw_packet_0e(uint8_t cmd, cmd_0e_send_fw_packet_t *param, uint16_t len);
int handle_get_log_0f(uint8_t cmd, cmd_0f_get_log_t *param, uint16_t len);
int handle_clear_log_10(uint8_t cmd, cmd_10_clear_log_t *param, uint16_t len);
int handle_get_log_usage_11(uint8_t cmd, cmd_11_get_log_usage_t *param, uint16_t len);
int handle_stop_log_12(uint8_t cmd, cmd_12_stop_log_t *param, uint16_t len);
int handle_delete_stimulator_13(uint8_t cmd, cmd_13_delete_stimulator_t *param, uint16_t len);
int handle_clear_program_14(uint8_t cmd, cmd_14_clear_program_t *param, uint16_t len);
int handle_get_program_count_15(uint8_t cmd, cmd_15_get_program_count_t *param, uint16_t len);
int handle_antenna_threshold_16(uint8_t cmd, uint8_t *data, uint16_t len);
int handle_get_stim_version_17(uint8_t cmd, cmd_17_get_stim_version_t *param, uint16_t len);
int handle_get_impedance_18(uint8_t cmd, cmd_18_get_impedance_t *param, uint16_t len);
int handle_get_connected_stim_19(uint8_t cmd, cmd_19_get_connected_stim_t *param, uint16_t len);
int handle_search_impedance_1b(uint8_t cmd, cmd_1b_search_impedance_t *param, uint16_t len);
int handle_antenna_detect_1c(uint8_t cmd, cmd_1c_antenna_detect_t *param, uint16_t len);
int handle_temperature_limit_1e(uint8_t cmd, uint8_t *data, uint16_t len);
int handle_clear_all_config_1f(uint8_t cmd, cmd_1f_clear_all_t *param, uint16_t len);
int handle_factory_reset_20(uint8_t cmd, cmd_20_factory_reset_t *param, uint16_t len);
int handle_set_patient_info_21(uint8_t cmd, cmd_21_set_patient_info_t *param, uint16_t index, uint16_t len);
int handle_set_dict_config_22(uint8_t cmd, cmd_22_set_dict_config_t *param, uint16_t index, uint16_t len);
int handle_get_patient_info_23(uint8_t cmd, cmd_23_get_patient_info_t *param, uint16_t len);
int handle_get_dict_config_24(uint8_t cmd, cmd_24_get_dict_config_t *param, uint16_t len);
int handle_set_general_config_25(uint8_t cmd, cmd_25_set_general_config_t *param, uint16_t index, uint16_t len);
int handle_get_general_config_26(uint8_t cmd, cmd_26_get_general_config_t *param, uint16_t len);
int handle_delete_stimulator_27(uint8_t cmd, cmd_27_delete_stimulator_t *param, uint16_t len);
int handle_rf_power_28(uint8_t cmd, uint8_t *data, uint16_t len);
int handle_get_vrecp_29(uint8_t cmd, cmd_29_get_vrecp_t *param, uint16_t len);

/* 兼容旧版函数 */
int handle_bind_stimulator(uint8_t cmd, cmd_bind_stimulator_t *param, uint16_t len);
int handle_get_bound_stimulator(uint8_t cmd, uint8_t *data, uint16_t len);

/**
 * @brief 初始化BLE数据模块
 */
void process_init(void);

/**
 * @brief 发送刺激器状态
 * @param status 刺激器状态(见stim_run_state_t)
 */
void send_stim_status(uint8_t status);

/**
 * @brief 主命令分发函数
 * @param cmd 命令码
 * @param index 分包序号
 * @param data 数据指针
 * @param len 数据长度
 * @return 0:成功, 非0:失败
 */
int process_command(uint8_t cmd, uint16_t index, uint8_t *data, uint16_t len);

#endif /* __APPLICATION_USER_APP_BLE_DATA_H__ */
