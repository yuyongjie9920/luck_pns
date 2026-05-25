/**
 * @file x5.h
 * @brief LMS0820X5 驱动头文件
 * @version 2.0
 * @date 2026-03-17
 */

#ifndef __X5_H__
#define __X5_H__

#include <stdint.h>
#include <stdbool.h>
#include "x5_reg.h"
#include "leadinno_st25r3916.h"

/*==============================================================================
 * 宏定义
 *============================================================================*/

#define SET_RF_POWER(a)   st25r3916_set_rf_power_level(a)


#define RF_OK             0
#define RF_FAIL           -1

#define MAX_PROGRAM_NUM   4    /**< 最大刺激程序数量 */

/*==============================================================================
 * Query返回状态位定义 - 根据测试方案文档表4.3
 *============================================================================*/

/**
 * @brief 计算STIM_ON0寄存器值
 * @param dir 方向位
 * @param on 使能位
 * @return stim_on0寄存器值
 */
#define LMS0820X5_CALC_STIM_ON0(dir, on) \
    ((((~(dir)) & (on)) << 8) | ((dir) << 0))

/**
 * @brief 计算STIM_ON1寄存器值
 * @param dir 方向位
 * @param on 使能位
 * @return stim_on1寄存器值
 */
#define LMS0820X5_CALC_STIM_ON1(dir, on) \
    ((((~(dir)) & (on)) << 0) | ((dir) << 8))


#pragma pack(1)  // 设置1字节对齐

typedef union {
    uint16_t value;
    struct {
        /* 低8位: efuse ID */
        uint16_t efuse_id : 8;      /**< [7:0] efuse ID的低8位，用来区分不同刺激器 */
        /* 状态位 */
        uint16_t reserved : 1;       /**< [8] 保留位 */
        uint16_t imp_done : 1;       /**< [9] 1:阻抗测量完成,0:未完成 */
        uint16_t volt_done : 1;      /**< [10] 1:电压或电流测量完成,0:未完成 */
        uint16_t temp_done : 1;      /**< [11] 1:温度测量完成,0:未完成 */
        uint16_t stim_complete : 1;  /**< [12] 1:刺激完成,0:未完成(程控仪主动停止后) */
        uint16_t overlap : 1;        /**< [13] 1:异频刺激有重叠,0:无重叠 */
        uint16_t prog0_active : 1;   /**< [14] 1:程序0刺激正在进行,0:停止 */
        uint16_t undervolt : 1;      /**< [15] 1:欠压发生过,0:未发生 */
    } bits;
} x5_query_status_t;

/*==============================================================================
 * 枚举类型定义
 *============================================================================*/

typedef enum {
    X5_MEAS_ENABLE_NONE  = 0x00,  /**< 无测量使能 */
    X5_MEAS_ENABLE_TEMP  = 0x01,  /**< 温度测量使能 */
    X5_MEAS_ENABLE_VOLT  = 0x02,  /**< 电压测量使能 */
    X5_MEAS_ENABLE_IMP   = 0x04,  /**< 阻抗测量使能 */
} x5_meas_enable_mask_e;

typedef enum {
    X5_QUERY_OLD_POWER,   /**< 以上一次通讯功率通讯 */
    X5_QUERY_SET_POWER,   /**< 设置功率通讯 */
    X5_QUERY_ALL_POWER,   /**< 所有功率点通讯测试 */
    X5_QUERY_AUTO         /**< 自动寻找功率点 */
} x5_query_mode_e;

typedef enum {
    X5_STIM_MODE_SIMPLE = 0,  /**< 简单模式 */
    X5_STIM_MODE_RANDOM = 1,  /**< 任意模式 */
    X5_STIM_MODE_BURST = 2,   /**< 突发模式 */
} x5_stim_wave_mode_e;

typedef enum {
    X5_PROGRAM_0 = 0,  /**< 刺激程序0 */
    X5_PROGRAM_1,      /**< 刺激程序1 */
    X5_PROGRAM_2,      /**< 刺激程序2 */
    X5_PROGRAM_3       /**< 刺激程序3 */
} x5_program_idx_t;

typedef enum {
    X5_PHASE_DAC_1 = 0,   /**< 每周期1组DAC值 */
    X5_PHASE_DAC_2 = 1,   /**< 每周期2组DAC值 */
    X5_PHASE_DAC_4 = 2,   /**< 每周期4组DAC值 */
    X5_PHASE_DAC_8 = 3,   /**< 每周期8组DAC值 */
    X5_PHASE_DAC_16 = 4   /**< 每周期16组DAC值 */
} x5_phase_dac_num_t;

/// 刺激模式的枚举值
typedef enum {
    X5_SIMPLE_MODE = 0,  //简单模式
    X5_RANDOM_MODE,      //任意模式
    X5_BURST_MODE,       //爆发模式
    X5_INVERT_MODE,      //交替模式
} x5_stim_wave_mode_t;

/*==============================================================================
 * 结构体类型定义
 *============================================================================*/



typedef struct {
    uint16_t dir;    ///< 电极电流方向(源极位置)
    uint16_t on;     ///< 电极开关(电极开的位置)
} x5_polar_config;

/**
 * @brief 刺激程序参数结构体
 */
typedef struct {
    uint32_t tphase1;           /**< 正脉冲宽度(us) */
    uint32_t tphase2;           /**< 负脉冲宽度(us) */
    uint32_t tphase3;           /**< 电荷平衡时间(us) */
    uint16_t tinterphase1;      /**< 相位间隔1(us) */
    uint16_t tinterphase2;      /**< 相位间隔2(us) */
    uint32_t tinterpulse;       /**< 脉冲间延迟(us) */
    uint16_t amplitude_p;       /**< 正相幅度(DAC值) 传入数据ua */
    uint16_t amplitude_n;       /**< 负相幅度(DAC值) 传入数据ua*/
    uint16_t multiple;         /**< 刺激频率是基准CLK的多少倍*/
    x5_polar_config polar;      /**< 电极选择*/
} x5_program_params_t;

/**
 * @brief 分配设置
 */
typedef struct {
    x5_phase_dac_num_t phase1_config;  /**< Phase1 DAC组数配置 */
    x5_phase_dac_num_t phase2_config;  /**< Phase2 DAC组数配置 */
    uint8_t stim_clk_div;              /**< 刺激时钟分频(0-62, 实际分频n+1) */
} x5_phase_config_t;

/**
 * @brief 简单模式波形参数
 */
typedef struct {
    bool low_power_mode;        /**< 低功耗模式 */
    uint8_t program_cnt;        /**< 刺激程序数量(1-4) */
    uint16_t base_freq;         /**< 多程序组的基准频率 */
    uint16_t sotf_time;         /**< 软启动时间 单位ms*/
    x5_stim_wave_mode_t pulse_mode; /**< 刺激波形模式 */
    x5_program_params_t program[MAX_PROGRAM_NUM]; /**< 各程序参数 */
    x5_phase_config_t phase;
    uint16_t burst_pulse_cnt;         /**< 爆发模式每个burst的脉冲数(1-255) */
    uint16_t burst_tinter;       /**< 爆发模式间间隔(us) */
} x5_simple_params_t;

/**
 * @brief 随机模式波形参数 - 根据测试方案文档
 */
typedef struct {
    uint8_t random_dac_mode;    /**< [1:0] 随机模式DAC控制模式:
                                      00: 正负都在初始值上减少
                                      01: 正减负增
                                      10: 正增负减
                                      11: 正负都在初始值上增加 */
    uint8_t random_time_step;   /**< [3:0] 每个刺激电流持续时间步进 */
    uint8_t dac_ctr_step;       /**< [14:8] 刺激电流变化幅度 */
    uint16_t dac_ctr_num;       /**< [9:0] 正/负刺激周期内刺激电流个数 */
} x5_random_params_t;


/**
 * @brief 软启动参数
 */
typedef struct {
    bool enable;                /**< 软启动使能 */
    uint8_t inc_value;          /**< 每次增加的DAC值(1-31) */
    uint16_t step;              /**< 刺激周期数(1-1023) */
} x5_softstart_params_t;

/**
 * @brief query返回的数据 - 根据测试方案文档
 */
typedef struct {
    uint8_t status[2];          /**< 状态字节(高8位状态,低8位efuse_id) */
    uint16_t crc;               /**< CRC校验和 */
} x5_query_response_t;

/*==============================================================================
 * RF通信数据结构 - 根据测试方案文档修正
 *============================================================================*/

/* Query命令格式 (Reader to Tag) */
typedef struct {
    uint8_t cmd : 4;            /**< 命令字: 0x8 */
    uint16_t reader_fill : 16;  /**< Reader填充: 高8位reserved=0,低8位0=广播或efuse_id=点对点 */
    uint16_t crc : 16;          /**< CRC校验和 */
} x5_rf_query_cmd_t;


//RF接收
typedef struct {
    uint8_t cmd;
    uint8_t reg;
    uint16_t uid;
    uint8_t crc_h;
    uint8_t crc_l;
} x5_rf_read_send_data_t;
typedef struct {//接收数据 header 1bit  reg_data 16bit   rn 16bit  crc 16bit
    uint8_t header;
    uint16_t reg_data;
    uint16_t rn;
    uint16_t crc;
} x5_rf_rx_dec_data_t;

//RF发送
typedef struct {
    uint8_t cmd;
    uint8_t reg;
    uint16_t reg_data;
    uint16_t uid;
    uint8_t crc_h;
    uint8_t crc_l;
} x5_rf_write_send_data_t;
typedef struct {//发送数据中 header 1bit  reg_data 16bit   rn 16bit  crc 16bit
    uint8_t header;
    uint16_t rn;
    uint16_t crc;
} x5_rf_write_dec_data_t;


/*==============================================================================
 * 全局数据结构
 *============================================================================*/
struct x5_data {
    uint8_t efuse_id;           /**< 芯片efuse ID低8位 */
};
extern struct x5_data x5;

#pragma pack()   // 恢复默认对齐

/*==============================================================================
 * 函数声明
 *============================================================================*/
/* 基本RF通信函数 - 根据测试方案文档修正 */
int x5_rf_send_query(void);
int x5_rf_send_query_with_power(uint8_t power);
int x5_rf_send_query_all_power(void);

/* 寄存器读写函数 - 根据测试方案文档修正 */
int x5_cmd_write(uint8_t reg, uint16_t reg_data);
int x5_cmd_read(uint8_t reg, uint16_t *reg_data);

/* 测量函数 */
int x5_rf_get_vrecp(float *vrecp, uint16_t delay_count);
int x5_rf_get_temp(float *temp);
int x5_rf_get_res(uint8_t channel_sel, uint16_t *res, uint8_t osc);
int x5_rf_get_current(float *current_p, float *current_n);

/* eFuse操作 - 根据测试方案文档 */
int x5_rf_write_efuse(uint32_t efuse);
int x5_rf_read_efuse(uint32_t *efuse);

/* 刺激控制函数 */
int x5_rf_stop_pulse(void);
int x5_rf_run_pulse(void);

int __x5_rf_reset(void);
/* 刺激配置函数 - 根据测试方案文档修正 */
int x5_rf_config_simple(x5_simple_params_t *params);
int x5_rf_config_more_simple(x5_simple_params_t *params);
int x5_rf_config_burst(x5_simple_params_t *params);
int x5_rf_config_random(x5_simple_params_t *simple, x5_random_params_t *random);
int x5_rf_config_phase(x5_phase_config_t *config);
int x5_rf_config_softstart(x5_program_idx_t prog_idx, x5_softstart_params_t *params);
int x5_rf_config_softstop(uint8_t enable_mask);

/* 工具函数 */
uint32_t x5_us_to_clk(uint32_t us, uint8_t clk_div);
uint32_t x5_clk_to_us(uint32_t clk, uint8_t clk_div);
uint16_t x5_ma_to_dac(uint32_t ma_uA);  /* 输入uA */
uint32_t x5_dac_to_ma(uint16_t dac);

#endif /* __X5_H__ */
