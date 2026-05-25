///**
// * @file lmrs1616.h
// * @brief LMRS1616X1A 驱动头文件
// * @version 2.0
// * @date 2026-04-07
// */
//
//#ifndef __LMRS1616_H__
//#define __LMRS1616_H__
//
//#include <stdint.h>
//#include <stdbool.h>
//#include "LMRS1616X1_reg.h"
//#include "leadinno_st25r3916.h"
//
///*==============================================================================
// * 宏定义
// *============================================================================*/
//
//#define SET_RF_POWER(a)   st25r3916_set_rf_power_level(a)
//
//#define RF_OK             0
//#define RF_FAIL           -1
//
//#define MAX_PROGRAM_NUM   4        /**< 最大程序数量 */
//#define MAX_CHANNEL_NUM   16       /**< 最大通道数量 */
//
///*==============================================================================
// * STIM_ON寄存器计算宏
// *============================================================================*/
//
///**
// * @brief STIM_ON0寄存器值计算
// * @param dir 方向位
// * @param on 使能位
// * @return stim_on0寄存器值
// */
//#define LMRS1616X1A_CALC_STIM_ON0(dir, on) \
//    ((((~(dir)) & (on)) << 8) | ((dir) << 0))
//
///**
// * @brief STIM_ON1寄存器值计算
// * @param dir 方向位
// * @param on 使能位
// * @return stim_on1寄存器值
// */
//#define LMRS1616X1A_CALC_STIM_ON1(dir, on) \
//    ((((~(dir)) & (on)) << 0) | ((dir) << 8))
//
//#pragma pack(1)  // 1字节对齐
//
//
///*==============================================================================
// * LMRS1616 查询响应状态位定义
// *============================================================================*/
//typedef union {
//    uint16_t value;
//    struct {
//        /* 8位: efuse ID */
//        uint16_t efuse_id : 8;      /**< [7:0] efuse ID的低8位 */
//        /* 状态位 */
//        uint16_t reserved : 1;       /**< [8] 保留位 */
//        uint16_t imp_done : 1;       /**< [9] 1:阻抗测量完成,0:未完成 */
//        uint16_t volt_done : 1;      /**< [10] 1:电压测量完成,0:未完成 */
//        uint16_t temp_done : 1;      /**< [11] 1:温度测量完成,0:未完成 */
//        uint16_t stim_complete : 1;  /**< [12] 1:刺激完成,0:未完成(刺激未停止) */
//        uint16_t overlap : 1;        /**< [13] 1:高频刺激重叠,0:未重叠 */
//        uint16_t prog0_active : 1;   /**< [14] 1:程序0刺激进行中,0:刺激停止 */
//        uint16_t undervolt : 1;      /**< [15] 1:发生过欠压,0:未发生 */
//    } bits;
//} lmrs1616_query_status_t;
//
///*==============================================================================
// * 枚举定义
// *============================================================================*/
//
//typedef enum {
//    LMRS1616_MEAS_ENABLE_NONE  = 0x00,  /**< 无测量使能 */
//    LMRS1616_MEAS_ENABLE_TEMP  = 0x01,  /**< 温度测量使能 */
//    LMRS1616_MEAS_ENABLE_VOLT  = 0x02,  /**< 电压测量使能 */
//    LMRS1616_MEAS_ENABLE_IMP   = 0x04,  /**< 阻抗测量使能 */
//} lmrs1616_meas_enable_mask_e;
//
//typedef enum {
//    LMRS1616_QUERY_OLD_POWER,   /**< 上一次通信功率通信 */
//    LMRS1616_QUERY_SET_POWER,   /**< 指定功率通信 */
//    LMRS1616_QUERY_ALL_POWER,   /**< 所有功率点通信 */
//    LMRS1616_QUERY_AUTO         /**< 自动寻找功率点 */
//} lmrs1616_query_mode_e;
//
//typedef enum {
//    LMRS1616_STIM_MODE_SIMPLE = 0,  /**< 简单模式 */
//    LMRS1616_STIM_MODE_RANDOM = 1,  /**< 任意模式 */
//    LMRS1616_STIM_MODE_BURST = 2,   /**< 突发模式 */
//} lmrs1616_stim_wave_mode_e;
//
//typedef enum {
//    LMRS1616_PROGRAM_0 = 0,  /**< 程序0 */
//    LMRS1616_PROGRAM_1,      /**< 程序1 */
//    LMRS1616_PROGRAM_2,      /**< 程序2 */
//    LMRS1616_PROGRAM_3       /**< 程序3 */
//} lmrs1616_program_idx_t;
//
//typedef enum {
//    LMRS1616_PHASE_DAC_1 = 0,   /**< 每1个DAC值 */
//    LMRS1616_PHASE_DAC_2 = 1,   /**< 每2个DAC值 */
//    LMRS1616_PHASE_DAC_4 = 2,   /**< 每4个DAC值 */
//    LMRS1616_PHASE_DAC_8 = 3,   /**< 每8个DAC值 */
//    LMRS1616_PHASE_DAC_16 = 4   /**< 每16个DAC值 */
//} lmrs1616_phase_dac_num_t;
//
//typedef enum {
//    LMRS1616_SIMPLE_MODE = 0,  // 简单模式
//    LMRS1616_RANDOM_MODE,      // 任意模式
//    LMRS1616_BURST_MODE,       // 突发模式
//} lmrs1616_stim_wave_mode_t;
//
///*==============================================================================
// * 结构体定义
// *============================================================================*/
//
//typedef struct {
//    uint16_t dir;    /**< 电极方向(阳极位置) */
//    uint16_t on;     /**< 电极使能(阴极位置) */
//} lmrs1616_polar_config;
//
///**
// * @brief 刺激程序参数结构体
// */
//typedef struct {
//    uint32_t tphase1;           /**< 正脉宽(us) */
//    uint32_t tphase2;           /**< 负脉宽(us) */
//    uint32_t tphase3;           /**< 电荷平衡时间(us) */
//    uint16_t tinterphase1;      /**< 正负间隔1(us) */
//    uint16_t tinterphase2;      /**< 正负间隔2(us) */
//    uint32_t tinterpulse;       /**< 脉冲间隔(us) */
//    uint16_t amplitude_p;       /**< 正向刺激幅度(μA) */
//    uint16_t amplitude_n;       /**< 负向刺激幅度(μA) */
//    uint16_t multiple;          /**< 程序频率是基准CLK的多少倍 */
//    lmrs1616_polar_config polar; /**< 电极选择 */
//} lmrs1616_program_params_t;
//
///**
// * @brief 相位配置结构体
// */
//typedef struct {
//    lmrs1616_phase_dac_num_t phase1_config;  /**< Phase1 DAC配置 */
//    lmrs1616_phase_dac_num_t phase2_config;  /**< Phase2 DAC配置 */
//    uint8_t stim_clk_div;                    /**< 刺激时钟分频值(0-62, 实际分频n+1) */
//} lmrs1616_phase_config_t;
//
///**
// * @brief 简单模式参数结构体
// */
//typedef struct {
//    bool low_power_mode;                     /**< 低功耗模式 */
//    uint8_t program_cnt;                     /**< 程序数量(1-4) */
//    uint16_t base_freq;                      /**< 基准频率(Hz) */
//    uint16_t soft_time;                      /**< 软启动时间(ms) */
//    lmrs1616_stim_wave_mode_t pulse_mode;    /**< 刺激模式 */
//    lmrs1616_program_params_t program[MAX_PROGRAM_NUM]; /**< 程序参数 */
//    lmrs1616_phase_config_t phase;           /**< 相位配置 */
//    uint16_t burst_pulse_cnt;                /**< 突发模式每burst脉冲数(1-255) */
//    uint16_t burst_tinter;                   /**< 突发模式间隔(us) */
//} lmrs1616_simple_params_t;
//
///**
// * @brief 任意模式参数结构体
// */
//typedef struct {
//    uint8_t random_dac_mode;    /**< [1:0] 任意模式DAC模式 */
//    uint8_t random_time_step;   /**< [3:0] 每步刺激时间步进 */
//    uint8_t dac_ctr_step;       /**< [14:8] 刺激变化步进 */
//    uint16_t dac_ctr_num;       /**< [9:0] 正向/负向刺激次数 */
//} lmrs1616_random_params_t;
//
///**
// * @brief 软启动参数结构体
// */
//typedef struct {
//    bool enable;                /**< 使能 */
//    uint8_t inc_value;          /**< 每次增加的DAC值(1-31) */
//    uint16_t step;              /**< 刺激周期数(1-1023) */
//} lmrs1616_softstart_params_t;
//
///**
// * @brief query响应结构体
// */
//typedef struct {
//    uint8_t status[2];          /**< 状态字节(8位状态,8位efuse_id) */
//    uint16_t crc;               /**< CRC校验 */
//} lmrs1616_query_response_t;
//
///*==============================================================================
// * RF通信数据结构体
// *============================================================================*/
//
//// 读命令发送数据结构
//typedef struct {
//    uint8_t cmd;
//    uint8_t reg;
//    uint16_t uid;
//    uint8_t crc_h;
//    uint8_t crc_l;
//} lmrs1616_rf_read_send_data_t;
//
//// 读命令接收解码结构
//typedef struct {
//    uint8_t header;
//    uint16_t reg_data;
//    uint16_t rn;
//    uint16_t crc;
//} lmrs1616_rf_rx_dec_data_t;
//
//// 写命令发送数据结构
//typedef struct {
//    uint8_t cmd;
//    uint8_t reg;
//    uint16_t reg_data;
//    uint16_t uid;
//    uint8_t crc_h;
//    uint8_t crc_l;
//} lmrs1616_rf_write_send_data_t;
//
//// 写命令接收解码结构
//typedef struct {
//    uint8_t header;
//    uint16_t rn;
//    uint16_t crc;
//} lmrs1616_rf_write_dec_data_t;
//
///*==============================================================================
// * 全局数据结构体
// *============================================================================*/
//struct lmrs1616_data {
//    uint8_t efuse_id;           /**< 目标芯片efuse ID 8位 */
//};
//extern struct lmrs1616_data lmrs1616;
//
//#pragma pack()   // 恢复默认对齐
//
///*==============================================================================
// * 函数声明
// *============================================================================*/
//
///* RF通信函数 */
//int lmrs1616_rf_send_query(void);
//int lmrs1616_rf_send_query_with_power(uint8_t power);
//int lmrs1616_rf_send_query_all_power(void);
//
///* 寄存器读写函数 */
//int lmrs1616_cmd_write(uint8_t reg, uint16_t reg_data);
//int lmrs1616_cmd_read(uint8_t reg, uint16_t *reg_data);
//
///* 测量功能函数 */
//int lmrs1616_rf_get_vrecp(float *vrecp, uint16_t delay_count);
//int lmrs1616_rf_get_temp(float *temp);
//int lmrs1616_rf_get_res(uint8_t channel_sel, uint16_t *res, uint8_t osc);
//int lmrs1616_rf_get_current(float *current_p, float *current_n);
//
///* eFuse操作函数 */
//int lmrs1616_rf_write_efuse(uint32_t efuse);
//int lmrs1616_rf_read_efuse(uint32_t *efuse);
//
///* 刺激控制函数 */
//int lmrs1616_rf_stop_pulse(void);
//int __lmrs1616_rf_reset(void);
//
///* 刺激配置函数 */
//int lmrs1616_rf_config_simple(lmrs1616_simple_params_t *params);
//int lmrs1616_rf_config_more_simple(lmrs1616_simple_params_t *params);
//int lmrs1616_rf_config_burst(lmrs1616_simple_params_t *params);
//int lmrs1616_rf_config_random(lmrs1616_simple_params_t *simple, lmrs1616_random_params_t *random);
//int lmrs1616_rf_config_phase(lmrs1616_phase_config_t *config);
//int lmrs1616_rf_config_softstart(lmrs1616_program_idx_t prog_idx, lmrs1616_softstart_params_t *params);
//int lmrs1616_rf_config_softstop(uint8_t enable_mask);
//
///* 转换函数 */
//uint32_t lmrs1616_us_to_clk(uint32_t us, uint8_t clk_div);
//uint32_t lmrs1616_clk_to_us(uint32_t clk, uint8_t clk_div);
//uint16_t lmrs1616_ma_to_dac(uint32_t ma_uA);
//uint32_t lmrs1616_dac_to_ma(uint16_t dac);
//
//#endif /* __LMRS1616_H__ */
