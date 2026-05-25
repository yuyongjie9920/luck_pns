///**
// * @file lmp1602x1.h
// * @brief LMP1602X1 驱动头文件
// * @version 2.0
// * @date 2026-04-07
// */
//
//#ifndef __LMP1602X1_H__
//#define __LMP1602X1_H__
//
//#include <stdint.h>
//#include <stdbool.h>
//#include "LMP1602X1_reg.h"
//#include "leadinno_st25r3916.h"
//
///*==============================================================================
// * 宏定义
// *============================================================================*/
//
//#define SET_RF_POWER(a)   st25r3916_set_rf_power_level(a)
//#define RF_OK             0
//#define RF_FAIL           -1
//
///*==============================================================================
// * STIM_ON寄存器计算宏
// *============================================================================*/
///**
// * @brief 计算STIM_ON0寄存器值
// * @param dir 方向位
// * @param on 使能位
// * @return stim_on0寄存器值
// */
//#define LMP1602X1_STIM_ON0(dir, on) \
//    ((((~(dir)) & (on)) << 8) | ((dir) << 0))
//
///**
// * @brief 计算STIM_ON1寄存器值
// * @param dir 方向位
// * @param on 使能位
// * @return stim_on1寄存器值
// */
//#define LMP1602X1_STIM_ON1(dir, on) \
//    ((((~(dir)) & (on)) << 0) | ((dir) << 8))
//
//
//#pragma pack(1)  // 1字节对齐
//
///*==============================================================================
// * Query状态位 - 根据测试方案文档
// *============================================================================*/
//typedef union {
//    uint16_t value;
//    struct {
//        /* 8位: efuse ID */
//        uint16_t efuse_id : 8;      /**< [7:0] efuse ID的低8位 */
//        /* 状态位 */
//        uint16_t reserved : 1;       /**< [8] 保留位 */
//        uint16_t imp_done : 1;       /**< [9] 1:阻抗完成,0:未完成 */
//        uint16_t volt_done : 1;      /**< [10] 1:电压完成,0:未完成 */
//        uint16_t temp_done : 1;      /**< [11] 1:温度完成,0:未完成 */
//        uint16_t stim_complete : 1;  /**< [12] 1:刺激完成,0:未完成(停止或暂停) */
//        uint16_t overlap : 1;        /**< [13] 1:两路刺激重叠,0:无重叠 */
//        uint16_t prog0_active : 1;   /**< [14] 1:程序0刺激进行中,0:停止或暂停 */
//        uint16_t undervolt : 1;      /**< [15] 1:是否欠压,0:未欠压 */
//    } bits;
//} lmp1602x1_query_status_t;
//
///*==============================================================================
// * 枚举定义
// *============================================================================*/
//
//typedef enum {
//    LMP_MEAS_ENABLE_NONE  = 0x00,  /**< 无测量使能 */
//    LMP_MEAS_ENABLE_TEMP  = 0x01,  /**< 温度测量使能 */
//    LMP_MEAS_ENABLE_VOLT  = 0x02,  /**< 电压测量使能 */
//    LMP_MEAS_ENABLE_IMP   = 0x04,  /**< 阻抗测量使能 */
//} lmp_meas_enable_mask_e;
//
//typedef enum {
//    LMP_SIMPLE_MODE = 0,  /**< 简单模式 */
//    LMP_RANDOM_MODE,      /**< 随机模式 */
//    LMP_BURST_MODE,       /**< 突发模式 */
//} lmp_stim_wave_mode_t;
//
//typedef enum {
//    LMP_CURRENT_SOURCE_A = 0,  /**< A电流源 */
//    LMP_CURRENT_SOURCE_B = 1,  /**< B电流源 */
//} lmp_current_source_e;
//
//typedef enum {
//    LMP_PROGRAM_0 = 0,  /**< 程序0 */
//    LMP_PROGRAM_1,      /**< 程序1 */
//} lmp_program_idx_t;
//
///*==============================================================================
// * 结构体定义
// *============================================================================*/
//
///**
// * @brief 极性配置
// */
//typedef struct {
//    uint16_t dir;    /**< 电极方向(阴极位置) */
//    uint16_t on;     /**< 电极使能(电极位置) */
//} lmp_polar_config_t;
//
///**
// * @brief 程序参数结构体
// */
//typedef struct {
//    uint32_t tphase1;           /**< 正脉宽(us) */
//    uint32_t tphase2;           /**< 负脉宽(us) */
//    uint32_t tphase3;           /**< 电荷平衡时间(us) */
//    uint16_t tinterphase1;      /**< 正脉宽后延迟(us) */
//    uint16_t tinterphase2;      /**< 负脉宽后延迟(us) */
//    uint32_t tinterpulse;       /**< 脉冲间延迟(us) */
//    uint16_t amplitude_p;       /**< 正向幅度(DAC值) ua */
//    uint16_t amplitude_n;       /**< 负向幅度(DAC值) ua*/
//    uint16_t multiple;          /**< 程序频率是基准CLK的多少倍 */
//    lmp_polar_config_t polar;   /**< 电极选择 */
//} lmp_program_params_t;
//
///**
// * @brief 相位配置
// */
//typedef struct {
//    uint8_t phase1_config;      /**< Phase1 DAC 数量 */
//    uint8_t phase2_config;      /**< Phase2 DAC 数量 */
//    uint8_t stim_clk_div;       /**< 刺激时钟分频(0-62) */
//} lmp_phase_config_t;
//
//typedef struct {
//    uint8_t en_0;
//    uint8_t en_1;
//    uint8_t en_2;
//    uint8_t en_3;
//} lmp_program_en_t;
///**
// * @brief 简单模式参数
// */
//typedef struct {
//    bool low_power_mode;        /**< 低功耗模式 */
//    uint8_t program_cnt;        /**< 程序数量(1-2) */
//    uint16_t base_freq;         /**< 基础频率(Hz) */
//    lmp_stim_wave_mode_t pulse_mode; /**< 刺激波形模式 */
//    lmp_program_params_t program[2][2]; /**< 程序数组 */
//    lmp_program_en_t p_en;
//    lmp_phase_config_t phase;   /**< 相位配置 */
//    uint16_t burst_pulse_cnt;   /**< 突发模式下每burst脉冲数(1-255) */
//    uint16_t burst_tinter;      /**< 突发模式脉冲间隔(us) */
//    uint16_t auto_stop_en;      /**< 自动停止使能 */
//    uint16_t pwm_duty;          /**< PWM占空比 */
//    uint8_t pwm_sync_mode;     /**< PWM_同步模式 */
//    uint16_t pwm_delay_count;   /**< PWM_同步位移模式 */
//} lmp_simple_params_t;
///**
//
// * @brief 软启动参数
// */
//typedef struct {
//    bool enable;                /**< 使能 */
//    uint8_t inc_value;          /**< 每次增加的DAC值(1-31) */
//    uint16_t step;              /**< 刺激次数(1-1023) */
//} lmp_softstart_params_t;
//
///*==============================================================================
// * RF通信数据结构 - 根据测试方案文档
// *============================================================================*/
//
///* Query命令格式 (Reader to Tag) */
//typedef struct {
//    uint8_t cmd : 4;            /**< 命令: 0x8 */
//    uint16_t reader_fill : 16;  /**< Reader填充: 8位reserved=0,8位0=广播efuse_id=指定 */
//    uint16_t crc : 16;          /**< CRC校验 */
//} lmp_rf_query_cmd_t;
//
//// RF读数据结构
//typedef struct {
//    uint8_t cmd;
//    uint8_t reg;
//    uint16_t uid;
//    uint8_t crc_h;
//    uint8_t crc_l;
//} lmp_rf_read_send_data_t;
//
//typedef struct {
//    uint8_t header;
//    uint16_t reg_data;
//    uint16_t rn;
//    uint16_t crc;
//} lmp_rf_rx_dec_data_t;
//
//// RF写数据结构
//typedef struct {
//    uint8_t cmd;
//    uint8_t reg;
//    uint16_t reg_data;
//    uint16_t uid;
//    uint8_t crc_h;
//    uint8_t crc_l;
//} lmp_rf_write_send_data_t;
//
//typedef struct {
//    uint8_t header;
//    uint16_t rn;
//    uint16_t crc;
//} lmp_rf_write_dec_data_t;
//
///*==============================================================================
// * 全局数据结构
// *============================================================================*/
//struct lmp1602x1_data {
//    uint8_t efuse_id;           /**< 目标芯片efuse ID低8位 */
//    uint8_t sel_pwm_AB;         /**< 选择PWM模式以谁对齐 */
//    lmp1602x1_query_status_t last_status; /**< 最后状态 */
//};
//extern struct lmp1602x1_data lmp;
//
//#pragma pack()   // 恢复默认对齐
//
///*==============================================================================
// * 函数声明
// *============================================================================*/
//
///* RF通信相关 */
//int lmp_rf_send_query(void);
//int lmp_rf_send_query_with_power(uint8_t power);
//int lmp_rf_send_query_all_power(void);
//
///* 寄存器读写 */
//int lmp_cmd_write(uint8_t reg, uint16_t reg_data);
//int lmp_cmd_read(uint8_t reg, uint16_t *reg_data);
//
///* 测量功能 */
//int lmp_rf_get_vrecp(float *vrecp, uint16_t delay_count);
//int lmp_rf_get_temp(float *temp);
//int lmp_rf_get_res(uint8_t channel_sel, uint16_t *res, uint8_t osc);
//int lmp_rf_get_current(float *current_p, float *current_n, lmp_current_source_e source);
///* eFuse操作 */
//int lmp_rf_write_efuse(uint32_t efuse);
//int lmp_rf_read_efuse(uint32_t *efuse);
//
///* 刺激控制 */
//int lmp_rf_stop_pulse(void);
//int lmp_rf_stop_pulse_current_source(void);
//int __lmp_rf_reset(void);
//int lmp_auot_stop(uint16_t stop_clk_count);
//int lmp_set_pwm_mode(lmp_simple_params_t *params, uint16_t pwm_mode_en, uint8_t *pwm_duty, uint16_t frq);
//
///* 刺激配置 */
//int lmp_rf_config_simple(lmp_simple_params_t *params);
//int lmp_rf_config_burst(lmp_simple_params_t *params);
//int lmp_rf_config_phase(lmp_phase_config_t *config);
//int lmp_rf_config_softstart(lmp_program_idx_t prog_idx, lmp_softstart_params_t *params);
//int lmp_rf_config_softstop(uint8_t enable_mask);
//
///* 辅助函数 */
//uint32_t lmp_us_to_clk(uint32_t us, uint8_t clk_div);
//uint16_t lmp_ua_to_dac(uint32_t target_ua);
//uint32_t lmp_dac_to_ua(uint16_t dac);
//
//#endif /* __LMP1602X1_H__ */
