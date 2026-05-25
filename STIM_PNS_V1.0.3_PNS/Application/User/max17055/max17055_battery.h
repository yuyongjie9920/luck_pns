#ifndef __MAX17055_BATTERY_H__
#define __MAX17055_BATTERY_H__

#include "main_user.h"

/* MAX17055 I2C设备地址定义 (7位地址，不包含读写位) */
#define MAX17055_I2C_DEVICE_ADD    0x36

/* 电压转换宏：将LSB值转换为微伏(μV) */
#define MAX17055_LSB_TO_UVOLTS(lsb)    (lsb * 625 / 8)

/* ==================== 类型定义部分 ==================== */
/**
 * @brief I2C读写函数指针类型定义
 * @param addr 设备地址
 * @param reg 寄存器地址指针
 * @param reglen 寄存器地址长度
 * @param data 数据缓冲区指针
 * @param datalen 数据长度
 * @return 执行结果
 */
typedef int (*max_iic_func)(uint16_t addr, \
                        uint8_t *reg, uint8_t reglen,\
                        uint8_t *data,  uint16_t datalen);

/**
 * @brief I2C初始化结构体
 */
typedef struct {
    max_iic_func p_write;  /* I2C写函数指针 */
    max_iic_func p_read;   /* I2C读函数指针 */
} max_iic_init_s;

/**
 * @brief 电源健康状态枚举
 */
typedef enum{
    POWER_SUPPLY_HEALTH_GOOD,         /* 良好状态 */
    POWER_SUPPLY_HEALTH_OVERVOLTAGE,  /* 过压状态 */
    POWER_SUPPLY_HEALTH_OVERHEAT,     /* 过热状态 */
}max17055_power_e;

/**
 * @brief 电池属性枚举
 * 定义了MAX17055支持的所有电池相关属性
 */
enum max17055_prop_e {
    POWER_SUPPLY_PROP_TEMP = 0,     // 获取当前温度（调用专用函数）
    POWER_SUPPLY_PROP_HEALTH,       // 获取电池健康状态
    POWER_SUPPLY_PROP_VOLTAGE_NOW,  // 读取当前电压寄存器
    POWER_SUPPLY_PROP_CURRENT_AVG,  // 读取平均电流寄存器
    POWER_SUPPLY_PROP_CURRENT_NOW,  // 读取当前电流寄存器
    POWER_SUPPLY_PROP_CAPACITY,     // 剩余容量百分比
    POWER_SUPPLY_PROP_CYCLE_COUNT,  // 读取充电循环次数寄存器
    POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN, //设计容量
    POWER_SUPPLY_PROP_TIME_TO_EMPTY_AVG,  //读取平均放电时间寄存器
    POWER_SUPPLY_PROP_CHARGE_FULL,   //冲满电电池容量衡量电池健康的一个指标
};

/* ==================== 寄存器定义 ==================== */

/**
 * @brief MAX17055寄存器索引枚举
 * 定义了MAX17055的所有寄存器地址
 */
enum max17055_reg_index {
    /* 标准命令 */
    MAX17055_STATUS_REG     = 0x00,  /* 状态寄存器 */
    MAX17055_VALRTTH_REG    = 0x01,  /* 电压警报阈值寄存器 */
    MAX17055_TALRTTH_REG    = 0x02,  /* 温度警报阈值寄存器 */
    MAX17055_SALRTTH_REG    = 0x03,  /* SOC警报阈值寄存器 */
    MAX17055_REPCAP_REG     = 0x05,  /* 报告剩余容量寄存器（mAh） */
    MAX17055_REPSOC_REG     = 0x06,  /* 报告SOC百分比寄存器 */
    MAX17055_AGE_REG        = 0x07,  /* 电池老化百分比寄存器 */
    MAX17055_TEMP_REG       = 0x08,  /* 温度测量寄存器 */
    MAX17055_VCELL_REG      = 0x09,  /* 电池电压测量寄存器 */
    MAX17055_CURRENT_REG    = 0x0A,  /* 电流测量寄存器  IC测量CSP和CSN引脚之间的电压，结果以2的补码值存储在Current寄存器中。寄存器值应除以感测电阻以转换为安培*/
    MAX17055_AVGCURRENT_REG = 0x0B,  /* 平均电流寄存器 */

    /* 扩展寄存器 */
    MAX17055_FULLCAPREP_REG = 0x10,  /* 满容量寄存器 */
    MAX17055_TTE_REG        = 0x11,  /* 预计放空时间寄存器 */
    MAX17055_QRTABLE00_REG  = 0x12,  /* QR表00寄存器 */
    MAX17055_AVGTA_REG      = 0x16,  /* 平均温度寄存器 */
    MAX17055_CYCLES_REG     = 0x17,  /* 充放电循环计数寄存器 */
    MAX17055_DESIGNCAP_REG  = 0x18,  /* 设计容量寄存器 */
    MAX17055_AVGVCELL_REG   = 0x19,  /* 平均电压寄存器 */
    MAX17055_REG_MaxMinTemp = 0x1A,  /* 最大最小温度寄存器 */
    MAX17055_MAXMINVOLT_REG = 0x1B,  /* 最大最小电压寄存器 */
    MAX17055_MaxMinCurr_REG = 0x1C,  /* 最大最小电流寄存器 */
    MAX17055_CONFIG_REG     = 0x1D,  /* 配置寄存器 */
    MAX17055_ICHGTERM_REG   = 0x1E,  /* 充电终止电流寄存器 */

    /* 版本和配置寄存器 */
    MAX17055_VERSION_REG    = 0x21,  /* 版本寄存器 */
    MAX17055_QRTABLE10_REG  = 0x22,  /* QR表10寄存器 */
    MAX17055_LEARNCFG_REG   = 0x28,  /* 学习配置寄存器 */
    MAX17055_RELAXCFG_REG   = 0x2A,  /* 松弛状态检测配置寄存器 */
    MAX17055_TGAIN_REG      = 0x2C,  /* 温度增益寄存器 */
    MAX17055_TOFF_REG       = 0x2D,  /* 温度偏移寄存器 */

    /* 高级配置寄存器 */
    MAX17055_QRTABLE20_REG  = 0x32,  /* QR表20寄存器 */
    MAX17055_DieTemp_REG    = 0x34,  /* 芯片温度寄存器 */
    MAX17055_RCOMP0_REG     = 0x38,  /* 初始内阻补偿寄存器 */
    MAX17055_TEMPCO_REG     = 0x39,  /* 温度补偿系数寄存器 */
    MAX17055_VEMPTY_REG     = 0x3A,  /* 空载电压阈值寄存器 */
    MAX17055_FSTAT_REG      = 0x3D,  /* 算法状态寄存器 */

    /* 模型相关寄存器 */
    MAX17055_QRTABLE30_REG  = 0x42,  /* QR表30寄存器 */
    MAX17055_DQACC_REG      = 0x45,  /* 容量变化精度寄存器 */
    MAX17055_DPACC_REG      = 0x46,  /* 功率变化精度寄存器 */

    /* 唤醒寄存器 */
    MAX17055_SoftWakeup_REG = 0x60,  /* 软件唤醒寄存器 */

    /* 电源相关寄存器 */
    MAX17055_POWER_REG      = 0xB1,  /* 瞬时功率寄存器 */
    MAX17055_AVGPOWER_REG   = 0xB3,  /* 平均功率寄存器 */
    MAX17055_ALRTTH_REG     = 0xB4,  /* 电流警报阈值寄存器 */
    MAX17055_HIBCFG_REG     = 0xBA,  /* 休眠模式配置寄存器 */
    MAX17055_CONFIG2_REG    = 0xBB,  /* 配置寄存器2 */

    /* 模型配置寄存器 */
    MAX17055_MODELCFG_REG   = 0xDB,  /* 模型配置寄存器 */

    /* OCV和SOC寄存器 */
    MAX17055_OCV_REG        = 0xFB,  /* OCV寄存器 */
    MAX17055_VFSOC_REG      = 0xFF,  /* VFSOC寄存器 */

    MAX17055_REG_MAX,  /* 哨兵值，表示寄存器数量 */
};

/* ==================== 结构体定义 ==================== */
/**
 * @brief 寄存器缓存结构体
 * 缓存MAX17055的关键寄存器值，避免频繁读取
 */
struct max17055_reg_cache {
    /* 基本测量数据 */
    int temperature;        /* 温度（℃） */
    int current;            /* 当前电流（mA） */
    int average_current;    /* 平均电流（mA） */
    int voltage;            /* 当前电压（mV） */
    int capacity;           /* 当前电量百分比（0-100） */
    int designcap;          /* 设计容量（mAh） */
    int left_time;          /* 设备剩余使用时间（min） */
    int charge_full;        /* 设备充满电容量（min） */
    int cycle_count;        /* 电池循环次数（min） */
    int design_full;        /* 电池出厂容量（ma） */
    /* 配置参数 */
    uint16_t ichgterm;      /* 充电终止电流阈值（mA） */
    uint16_t vempty;        /* 空载电压阈值（mV） */
    int vcharge;            /* 充电电压（mV） */
    /* 传感器参数 */
    unsigned int rsense;    /* 电流检测电阻值（mΩ） */
    /* 保护限制 */
    int volt_min;           /* 最小电压限制（mV） */
    int volt_max;           /* 最大电压限制（mV） */
    int temp_min;           /* 最低温度限制（℃） */
    int temp_max;           /* 最高温度限制（℃） */
    int health;             /* 电池健康状态（%） */
};

/**
 * @brief 设备信息结构体
 * 包含MAX17055设备的所有状态信息和配置
 */
struct max17055_device_info {
    struct max17055_reg_cache cache;    /* 寄存器缓存 包含关机电量百分比*/
    int (*read)(uint8_t reg, uint16_t *val);  /* 读寄存器函数指针 */
    int8_t (*write)(uint8_t reg, uint16_t val); /* 写寄存器函数指针 */
};

/**
 * @brief 设备信息结构体
 * 用于用户获取设备信息
 */
extern struct max17055_device_info max17055_dev;

/* ==================== 函数声明 ==================== */

/**
 * @brief MAX17055电池管理初始化函数
 * 初始化MAX17055设备，配置寄存器并启动监测
 */
void max17055_battery_init(void);

/**
 * @brief MAX17055定时循环处理函数
 * 定时读取电池状态并更新缓存，处理警报等事件
 */
void max17055_battery_update(void);


#endif /* __MAX17055_BATTERY_H__ */

