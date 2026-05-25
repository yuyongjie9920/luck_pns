///**
// * @file lmp1616x1.h
// * @brief LMP1616X1 芯片驱动头文件
// * @version 1.0
// * @date 2026-04-07
// *
// * @note 基于LMP1616X1寄存器定义，适配15电极版本
// *       电极编号: 0-14 (共15个电极)
// */
//
//#ifndef __LMP1616X1_H__
//#define __LMP1616X1_H__
//
//#include <stdint.h>
//#include <stdbool.h>
//#include "LMP1616X1_reg.h"
//
///*==============================================================================
// * 常量定义
// *============================================================================*/
//
//#define SET_RF_POWER(a)   st25r3916_set_rf_power_level(a)  /**< 设置RF功率宏 */
//
//#define RF_OK             0    /**< 操作成功 */
//#define RF_FAIL           -1   /**< 操作失败 */
//
//#define MAX_PROGRAM_NUM   4    /**< 最大程序数量 */
//#define MAX_ELECTRODE_NUM 15   /**< 最大电极数量 (0-14) */
//
///*==============================================================================
// * Query状态位定义
// *============================================================================*/
//
//#pragma pack(1)  // 1字节对齐
//
///**
// * @brief Query响应状态联合体
// */
//typedef union {
//    uint16_t value;
//    struct {
//        /* 状态位 */
//        uint16_t efuse_id_low : 8;      /**< [7:0] efuse ID低8位 */
//        uint16_t reserved : 1;          /**< [8] 保留位 */
//        uint16_t imp_done : 1;          /**< [9] 阻抗测量完成: 1=完成, 0=未完成 */
//        uint16_t volt_done : 1;         /**< [10] 电压测量完成: 1=完成, 0=未完成 */
//        uint16_t temp_done : 1;         /**< [11] 温度测量完成: 1=完成, 0=未完成 */
//        uint16_t stim_complete : 1;     /**< [12] 刺激完成: 1=完成, 0=未完成(仅停止刺激) */
//        uint16_t overlap : 1;           /**< [13] 电极复用冲突: 1=冲突, 0=无冲突 */
//        uint16_t prog0_active : 1;      /**< [14] 程序0激活: 1=激活中, 0=停止或未激活 */
//        uint16_t undervolt : 1;         /**< [15] 欠压保护: 1=欠压发生, 0=正常 */
//    } bits;
//} lmp1616x1_query_status_t;
//
///*==============================================================================
// * 枚举定义
// *============================================================================*/
//
///**
// * @brief 测量使能掩码枚举
// */
//typedef enum {
//    LMP1616X1_MEAS_ENABLE_NONE  = 0x00,  /**< 无测量使能 */
//    LMP1616X1_MEAS_ENABLE_TEMP  = 0x01,  /**< 温度测量使能 */
//    LMP1616X1_MEAS_ENABLE_VOLT  = 0x02,  /**< 电压测量使能 */
//    LMP1616X1_MEAS_ENABLE_IMP   = 0x04,  /**< 阻抗测量使能 */
//} lmp1616x1_meas_enable_mask_e;
//
///**
// * @brief Query模式枚举
// */
//typedef enum {
//    LMP1616X1_QUERY_OLD_POWER,   /**< 使用上一次通信成功的功率 */
//    LMP1616X1_QUERY_SET_POWER,   /**< 使用指定功率通信 */
//    LMP1616X1_QUERY_ALL_POWER,   /**< 所有功率点尝试通信 */
//    LMP1616X1_QUERY_AUTO         /**< 自动寻找合适功率 */
//} lmp1616x1_query_mode_e;
//
///**
// * @brief 刺激波形模式枚举
// */
//typedef enum {
//    LMP1616X1_STIM_MODE_SIMPLE = 0,  /**< 简单模式 */
//    LMP1616X1_STIM_MODE_RANDOM = 1,  /**< 任意模式 */
//    LMP1616X1_STIM_MODE_BURST  = 2,  /**< 突发模式 */
//} lmp1616x1_stim_wave_mode_e;
//
///**
// * @brief 程序索引枚举
// */
//typedef enum {
//    LMP1616X1_PROGRAM_0 = 0,  /**< 刺激程序0 */
//    LMP1616X1_PROGRAM_1,      /**< 刺激程序1 */
//    LMP1616X1_PROGRAM_2,      /**< 刺激程序2 */
//    LMP1616X1_PROGRAM_3       /**< 刺激程序3 */
//} lmp1616x1_program_idx_t;
//
///**
// * @brief 每相位DAC数量枚举
// */
//typedef enum {
//    LMP1616X1_PHASE_DAC_1  = 0,  /**< 每相位1个DAC值 */
//    LMP1616X1_PHASE_DAC_2  = 1,  /**< 每相位2个DAC值 */
//    LMP1616X1_PHASE_DAC_4  = 2,  /**< 每相位4个DAC值 */
//    LMP1616X1_PHASE_DAC_8  = 3,  /**< 每相位8个DAC值 */
//    LMP1616X1_PHASE_DAC_16 = 4,  /**< 每相位16个DAC值 */
//} lmp1616x1_phase_dac_num_t;
//
///*==============================================================================
// * 结构体定义
// *============================================================================*/
//
///**
// * @brief 电极极性配置结构体
// */
//typedef struct {
//    uint16_t dir;    /**< 电极方向(电流源选择) - 16位，每位对应一个电极 */
//    uint16_t on;     /**< 电极使能(电流宿选择) - 16位，每位对应一个电极 */
//} lmp1616x1_polar_config_t;
//
///**
// * @brief 单个刺激程序参数结构体
// */
//typedef struct {
//    uint32_t tphase1;           /**< 正脉冲宽度(us) */
//    uint32_t tphase2;           /**< 负脉冲宽度(us) */
//    uint32_t tphase3;           /**< 电荷平衡时间(us) */
//    uint16_t tinterphase1;      /**< 正负脉冲间隔1(us) */
//    uint16_t tinterphase2;      /**< 正负脉冲间隔2(us) */
//    uint32_t tinterpulse;       /**< 脉冲间隔(us) */
//    uint16_t amplitude_p[15];   /**< 正向刺激幅度(DAC值) - 15个电极 */
//    uint16_t amplitude_n[15];   /**< 负向刺激幅度(DAC值) - 15个电极 */
//    uint16_t multiple;          /**< 刺激频率倍率(相对于基准CLK的倍数) */
//    lmp1616x1_polar_config_t polar;  /**< 电极极性选择 */
//} lmp1616x1_program_params_t;
//
///**
// * @brief 相位配置结构体
// */
//typedef struct {
//    lmp1616x1_phase_dac_num_t phase1_config;  /**< Phase1 DAC配置 */
//    lmp1616x1_phase_dac_num_t phase2_config;  /**< Phase2 DAC配置 */
//    uint8_t stim_clk_div;                     /**< 刺激时钟分频(0-62, 实际分频n+1) */
//} lmp1616x1_phase_config_t;
//
///**
// * @brief 简单模式参数结构体
// */
//typedef struct {
//    bool low_power_mode;                      /**< 低功耗模式 */
//    uint8_t program_cnt;                      /**< 刺激程序数量(0-3对应1-4个程序) */
//    uint16_t base_freq;                       /**< 基准频率(Hz) */
//    lmp1616x1_stim_wave_mode_e pulse_mode;    /**< 刺激波形模式 */
//    lmp1616x1_program_params_t program[MAX_PROGRAM_NUM]; /**< 刺激程序参数数组 */
//    lmp1616x1_phase_config_t phase;           /**< 相位配置 */
//    uint16_t burst_pulse_cnt;                 /**< 突发模式每簇脉冲数(1-255) */
//    uint16_t burst_tinter;                    /**< 突发模式簇间隔(us) */
//} lmp1616x1_simple_params_t;
//
///**
// * @brief 软启动参数结构体
// */
//typedef struct {
//    bool enable;                /**< 使能 */
//    uint8_t inc_value[16];      /**< 每次增加的DAC值(1-31) - 16个DAC，索引0-15 */
//    uint16_t step;              /**< 刺激周期数(1-1023) */
//} lmp1616x1_softstart_params_t;
//
///*==============================================================================
// * RF通信帧结构
// *============================================================================*/
//
///* Query命令格式 (Reader to Tag) */
//typedef struct {
//    uint8_t cmd : 4;            /**< 命令: 0x8 */
//    uint16_t reader_fill : 16;  /**< Reader填充: 8位reserved=0, 8位efuse_id */
//    uint16_t crc : 16;          /**< CRC校验 */
//} lmp1616x1_rf_query_cmd_t;
//
///* 写命令发送数据结构 */
//typedef struct {
//    uint8_t cmd;
//    uint8_t reg;
//    uint16_t reg_data;
//    uint16_t uid;
//    uint8_t crc_h;
//    uint8_t crc_l;
//} lmp1616x1_rf_write_send_data_t;
//
///* 读命令发送数据结构 */
//typedef struct {
//    uint8_t cmd;
//    uint8_t reg;
//    uint16_t uid;
//    uint8_t crc_h;
//    uint8_t crc_l;
//} lmp1616x1_rf_read_send_data_t;
//
///*==============================================================================
// * 全局数据结构
// *============================================================================*/
//
///**
// * @brief LMP1616X1全局数据结构
// */
//struct lmp1616x1_data {
//    uint8_t efuse_id;           /**< 芯片efuse ID低8位 */
//    lmp1616x1_query_status_t last_query_status; /**< 最后一次查询状态 */
//};
//extern struct lmp1616x1_data lmp1616x1;
//
//#pragma pack()   // 恢复默认对齐
//
///*==============================================================================
// * 函数声明
// *============================================================================*/
//
///* RF通信函数 */
//int lmp1616x1_rf_send_query(void);
//int lmp1616x1_rf_send_query_with_power(uint8_t power);
//int lmp1616x1_rf_send_query_all_power(void);
//int __lmp1616x1_rf_send_query(void);
//
///* 寄存器读写函数 */
//int lmp1616x1_cmd_write(uint8_t reg, uint16_t reg_data);
//int lmp1616x1_cmd_read(uint8_t reg, uint16_t *reg_data);
//
///* 测量函数 */
//int lmp1616x1_rf_get_vrecp(float *vrecp, uint16_t delay_count);
//int lmp1616x1_rf_get_temp(float *temp);
//int lmp1616x1_rf_get_res(uint8_t channel_sel, uint16_t *res, uint8_t osc);
//int lmp1616x1_rf_get_current(float *current_p, float *current_n);
//
///* eFuse操作函数 */
//int lmp1616x1_rf_write_efuse(uint32_t efuse);
//int lmp1616x1_rf_read_efuse(uint32_t *efuse);
//
///* 刺激控制函数 */
//int lmp1616x1_rf_stop_pulse(void);
//int lmp1616x1_rf_reset(void);
//
///* 刺激配置函数 */
//int lmp1616x1_rf_config_simple(lmp1616x1_simple_params_t *params);
//int lmp1616x1_rf_config_more_simple(lmp1616x1_simple_params_t *params);
//int lmp1616x1_rf_config_burst(lmp1616x1_simple_params_t *params);
//int lmp1616x1_rf_config_phase(lmp1616x1_phase_config_t *config);
//int lmp1616x1_rf_config_softstart(lmp1616x1_program_idx_t prog_idx, lmp1616x1_softstart_params_t *params);
//int lmp1616x1_rf_config_softstop(uint8_t enable_mask);
//
//
///* 工具函数 */
//uint32_t lmp1616x1_us_to_clk(uint32_t us, uint8_t clk_div);
//uint32_t lmp1616x1_clk_to_us(uint32_t clk, uint8_t clk_div);
//uint16_t lmp1616x1_ma_to_dac(uint32_t ma_uA);
//uint32_t lmp1616x1_dac_to_ma(uint16_t dac);
//uint16_t lmp1616x1_ua_to_dac(uint32_t target_ua);
//
//#endif /* __LMP1616X1_H__ */
