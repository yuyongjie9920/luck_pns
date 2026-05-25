/**
 * @file x5_reg.h
 * @brief LMS0820X5 数字寄存器驱动头文件
 * @version 2.0
 * @date 2026-03-17
 *
 * 基于芯片测试方案文档和寄存器列表更新
 */

#ifndef __X5_REG_H__
#define __X5_REG_H__

#include <stdint.h>

/*==============================================================================
 * 刺激波形控制寄存器组 (0x00-0x09)
 *============================================================================*/

/**
 * @brief Tphase1寄存器 (地址0x00) - 正脉冲宽度
 */
#define LMS0820X5_REG_00_TPHASE1_0           (0x00)
typedef union {
    uint16_t value;
    struct {
        uint16_t tphase1 : 12;      /**< [11:0] 正脉冲时钟周期数 */
        uint16_t reserved : 1;       /**< [12] 保留位 */
        uint16_t stim_di : 3;        /**< [15:13] 给模拟或射频使用 */
    } bits;
} lms0820x5_tphase1_0_t;

/**
 * @brief Tphase2寄存器 (地址0x01) - 负脉冲宽度
 */
#define LMS0820X5_REG_01_TPHASE2_0           (0x01)
typedef union {
    uint16_t value;
    struct {
        uint16_t tphase2 : 12;      /**< [11:0] 负脉冲时钟周期数 */
        uint16_t reserved : 4;       /**< [15:12] 保留位 */
    } bits;
} lms0820x5_tphase2_0_t;

/**
 * @brief Tinterphase寄存器 (地址0x02) - 正负脉冲间隔
 */
#define LMS0820X5_REG_02_TINTERPHASE0       (0x02)
typedef union {
    uint16_t value;
    struct {
        uint16_t tinterphase1 : 8;   /**< [7:0] Tinterphase1 时钟周期数 */
        uint16_t tinterphase2 : 8;   /**< [15:8] Tinterphase2 时钟周期数 */
    } bits;
} lms0820x5_tinterphase0_t;

/**
 * @brief Tphase3寄存器 (地址0x03) - 电荷平衡阶段宽度
 */
#define LMS0820X5_REG_03_TPHASE3_0           (0x03)
typedef union {
    uint16_t value;
    struct {
        uint16_t tphase3 : 12;      /**< [11:0] 电荷平衡阶段时钟周期数 */
        uint16_t reserved : 4;       /**< [15:12] 保留位 */
    } bits;
} lms0820x5_tphase3_0_t;

/**
 * @brief Tinterpulse寄存器 (地址0x04) - 脉冲间延迟
 */
#define LMS0820X5_REG_04_TINTERPULSE0       (0x04)
typedef union {
    uint16_t value;
    struct {
        uint16_t tinterpulse : 16;  /**< [15:0] Tphase3与下一周期之间的延迟 */
    } bits;
} lms0820x5_tinterpulse0_t;

/**
 * @brief Tdefine寄存器 (地址0x05) - 刺激结束延迟
 */
#define LMS0820X5_REG_05_TDEFINE           (0x05)
typedef union {
    uint16_t value;
    struct {
        uint16_t stim_td1 : 8;       /**< [7:0] 刺激电路开启与开始刺激之间的延迟时间 */
        uint16_t stim_td2 : 2;       /**< [9:8] 保留位，固定为1 clock cycle */
        uint16_t stim_td3 : 6;       /**< [15:10] 电荷平衡结束与STIM_EN等关闭之间的延迟 */
    } bits;
} lms0820x5_tdefine_t;

/**
 * @brief STIM_MODE寄存器 (地址0x06) - 刺激模式控制
 */
#define LMS0820X5_REG_06_STIM_MODE         (0x06)
typedef union {
    uint16_t value;
    struct {
        uint16_t stim_start : 1;      /**< [0] 开始刺激信号 */
        uint16_t para_vld : 1;        /**< [1] 参数有效信号 */
        uint16_t stim_stop : 1;       /**< [2] 停止刺激: 1-停止,0-不停止 */
        uint16_t stim_mode : 2;       /**< [4:3] 刺激模式: 00-简单,01-任意,10-突发 */
        uint16_t stim_low_power : 1;  /**< [5] 低功耗模式 */
        uint16_t program_cnt : 2;     /**< [7:6] 刺激程序数量: 0-1个,1-2个,2-3个,3-4个 */
        uint16_t pulse_cnt : 8;       /**< [15:8] 一个burst里面的脉冲数(1-255) */
    } bits;
} lms0820x5_stim_mode_t;

/**
 * @brief pwm_setting寄存器 (地址0x07) - PWM供能设置
 */
#define LMS0820X5_REG_07_PWM_SETTING       (0x07)
typedef union {
    uint16_t value;
    struct {
        uint16_t pwm_power_dly : 13;  /**< [12:0] 供能检测后延迟的RC时钟周期数 */
        uint16_t pwm_mode : 1;        /**< [13] 1-占空比供能,0-常供能 */
        uint16_t reserved : 2;        /**< [15:14] 保留位 */
    } bits;
} lms0820x5_pwm_setting_t;

/**
 * @brief Tinterburst寄存器 (地址0x08) - 突发刺激间隔
 */
#define LMS0820X5_REG_08_TINTERBURST       (0x08)
typedef union {
    uint16_t value;  /**< [15:0] 突发模式刺激簇间隔时间 */
} lms0820x5_tinterburst_t;

/**
 * @brief phase_config寄存器 (地址0x09) - 相位配置
 */
#define LMS0820X5_REG_09_PHASE_CONFIG      (0x09)
typedef union {
    uint16_t value;
    struct {
        uint16_t phase1_config : 3;    /**< [2:0] Phase1 DAC组数 */
        uint16_t phase2_config : 3;    /**< [5:3] Phase2 DAC组数 */
        uint16_t reserved1 : 2;        /**< [7:6] 保留位 */
        uint16_t stim_clk_div_value : 6; /**< [13:8] 刺激时钟分频: n+1分频 */
        uint16_t reserved2 : 2;        /**< [15:14] 保留位 */
    } bits;
} lms0820x5_phase_config_t;

/*==============================================================================
 * 电极选择寄存器 (0x0A-0x0B)
 *============================================================================*/

/**
 * @brief STIM_ON0寄存器 (地址0x0A) - 正相电极选择
 */
#define LMS0820X5_REG_0A_STIM_ON0          (0x0A)
typedef union {
    uint16_t value;
    struct {
        uint16_t dansel_p : 8;  /**< DANSEL_p */
        uint16_t dcasel_p : 8;  /**< DCASEL_p */
    } bits;
} lms0820x5_stim_on0_0_t;

/**
 * @brief STIM_ON1寄存器 (地址0x0B) - 负相电极选择
 */
#define LMS0820X5_REG_0B_STIM_ON1          (0x0B)
typedef union {
    uint16_t value;
    struct {
        uint16_t dansel_n : 8;  /**< DANSEL_n */
        uint16_t dcasel_n : 8;  /**< DCASEL_n */
    } bits;
} lms0820x5_stim_on0_1_t;

/*==============================================================================
 * ADC/RFID配置寄存器 (0x0C-0x0F)
 *============================================================================*/

/**
 * @brief adc_diff_cfg寄存器 (地址0x0C) - ADC差分配置
 */
#define LMS0820X5_REG_0C_ADC_DIFF_CFG      (0x0C)
typedef union {
    uint16_t value;
    struct {
        uint16_t rfid_resp_delay_cnt : 11; /**< [10:0] RFID响应延迟计数 */
        uint16_t sel_demod : 1;            /**< [11] 解调信号选择: 0-原始,1-滤波后 */
        uint16_t pdelt : 4;                /**< [15:12] 数字滤波器宽度 */
    } bits;
} lms0820x5_adc_diff_cfg_t;

/**
 * @brief rfid_cfg1寄存器 (地址0x0D) - RFID Tari配置
 */
#define LMS0820X5_REG_0D_RFID_CFG1         (0x0D)
typedef union {
    uint16_t value;
    struct {
        uint16_t tari_min : 8;  /**< [7:0] Tari最小值 */
        uint16_t tari_max : 8;  /**< [15:8] Tari最大值 */
    } bits;
} lms0820x5_rfid_cfg1_t;

/**
 * @brief rfid_cfg2寄存器 (地址0x0E) - RFID Rt/Delta配置
 */
#define LMS0820X5_REG_0E_RFID_CFG2         (0x0E)
typedef union {
    uint16_t value;
    struct {
        uint16_t delta : 8;    /**< [7:0] Delta值 */
        uint16_t rt_max : 8;   /**< [15:8] Rt最大值 */
    } bits;
} lms0820x5_rfid_cfg2_t;

/**
 * @brief rfid_cfg3寄存器 (地址0x0F) - RFID发送周期配置
 */
#define LMS0820X5_REG_0F_RFID_CFG3         (0x0F)
typedef union {
    uint16_t value;
    struct {
        uint16_t demod_pon_cnt : 9;      /**< [8:0] 占空比功能时demod高电平时间 */
        uint16_t reserved : 2;           /**< [10:9] 保留位 */
        uint16_t rfid_tx_data_period : 5; /**< [15:11] 发送数据周期(偶数,最大30) */
    } bits;
} lms0820x5_rfid_cfg3_t;

/*==============================================================================
 * 正相DAC幅度寄存器 (0x10-0x19) - 根据测试方案文档修正
 *============================================================================*/

/**
 * @brief amp_dac_p_0寄存器 (地址0x10) - 程序0正相DAC值
 */
#define LMS0820X5_REG_10_AMP_DAC_P_0       (0x10)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_0 : 10;  /**< [9:0] 9-0 程序0正相DAC值 */
        uint16_t amplitude_dac_n_0 : 6;   /**< [15:10] 5-0程序1正相DAC值 */
    } bits;
} lms0820x5_amp_dac_p_0_t;

/**
 * @brief amp_dac_p_1寄存器 (地址0x11) - 程序1正相DAC值
 */
#define LMS0820X5_REG_11_AMP_DAC_P_1       (0x11)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_0 : 4;  /**< [3:0] [9-6]程序0负相DAC值 */
        uint16_t amplitude_dac_p_1 : 10; /**< [13:4] [9-0] 程序1正相DAC值 */
        uint16_t amplitude_dac_n_1 : 2;  /**< [15:14] [1-0] 程序1负相DAC值 */
    } bits;
} lms0820x5_amp_dac_p_1_t;

/**
 * @brief amp_dac_p_2寄存器 (地址0x12) - 程序2正相DAC值
 */
#define LMS0820X5_REG_12_AMP_DAC_P_2       (0x12)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_1 : 8;    /**< [15:8] [9-2] 程序1负相DAC值 */
        uint16_t amplitude_dac_p_2 : 8;    /**< [7:0] [7-0] 程序2正相DAC值*/
    } bits;
} lms0820x5_amp_dac_p_2_t;


/**
 * @brief amp_dac_p_2寄存器 (地址0x12) - 程序2正相DAC值
 */
#define LMS0820X5_REG_13_AMP_DAC_P_3       (0x13)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_2 : 2;    /**< [1:0] 9-8 程序2正向刺激 */
        uint16_t amplitude_dac_n_2 : 10;   /**< [11:2] 9-0 程序2负相DAC值 */
        uint16_t amplitude_dac_p_3 : 4;    /**< [15:12] 3-0 程序3正相DAC值 */
    } bits;
} lms0820x5_amp_dac_p_3_t;


/**
 * @brief amp_dac_p_2寄存器 (地址0x12) - 程序2正相DAC值
 */
#define LMS0820X5_REG_14_AMP_DAC_P_4       (0x14)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_3 : 6;    /**< [5:0] 9-4 程序3正负相刺激DAC值 */
        uint16_t amplitude_dac_n_3 : 10;    /**< [15:6] 9-0 */
    } bits;
} lms0820x5_amp_dac_p_4_t;


/**
 * @brief amp_dac_p_5寄存器 (地址0x15) - 程序1的Tphase1和Tphase2
 */
#define LMS0820X5_REG_15_TPHASE1_1       (0x15)
typedef union {
    uint16_t value;
    struct {
        uint16_t tphase1 : 12;       /**< [11:0] [11-0]程序1的正脉冲时钟周期数 */
        uint16_t tphase2 : 4;       /**< [15:12] [3-0] 程序2的正脉冲时钟周期数的低4位 */
    } bits;
} lms0820x5_tphase1_1_t;

/**
 * @brief amp_dac_p_6寄存器 (地址0x16) - 程序1的Tphase2和Tphase3
 */
#define LMS0820X5_REG_16_TPHASE2_1       (0x16)
typedef union {
    uint16_t value;
    struct {
        uint16_t tphase2 : 8;       /**< [11:4] 程序1的负脉冲低8位 */
        uint16_t tphase3 : 8;         /**< [7:0] 程序1的电荷平衡时间 */
    } bits;
} lms0820x5_tphase2_1_t;

/**
 * @brief amp_dac_p_7寄存器 (地址0x17) - 程序1的Tphase3和程序2的Tphase1
 */
#define LMS0820X5_REG_17_TPHASE3_1       (0x17)
typedef union {
    uint16_t value;
    struct {
        uint16_t tphase3_1 : 4;       /**< [11-8] 程序1的电荷平衡时间 */
        uint16_t tphase1_2 : 12;      /**< [0-11] 程序2的正脉冲时钟周期数 */
    } bits;
} lms0820x5_tphase3_1_t;

/**
 * @brief amp_dac_p_8寄存器 (地址0x18) - 程序2的Tphase2和Tphase3
 */
#define LMS0820X5_REG_18_TPHASE2_2       (0x18)
typedef union {
    uint16_t value;
    struct {
         uint16_t tphase2_2 : 12;    /**< [11:0] 11-0 程序2负向脉宽 */
         uint16_t tphase3_2 : 4;     /**< [15:12] 3-0 程序2的电荷平衡 */
    } bits;
} lms0820x5_tphase2_2_t;

/**
 * @brief amp_dac_p_9寄存器 (地址0x19) - 程序2的Tphase3和程序3的Tphase1
 */
#define LMS0820X5_REG_19_TPHASE3_2       (0x19)
typedef union {
    uint16_t value;
    struct {
        uint16_t tphase3_2 : 8;       /**< [7:0] 11-4 程序2的电荷平衡时间 */
        uint16_t tphase1_3 : 8;       /**< [15:8] 7-0 程序3的正脉宽 */
    } bits;
} lms0820x5_tphase3_2_t;

/*==============================================================================
 * 负相DAC幅度寄存器 (0x1A-0x23)
 *============================================================================*/

/**
 * @brief amp_dac_n_0寄存器 (地址0x1A) - 程序0负相DAC值
 */
/**
 * @brief amp_dac_n_0寄存器 (地址0x1A)
 *
 * 作为AMP_DAC_N_0时：
 *   [9:0]   - amplitude_dac_n_0: 程序0负相DAC值
 *   [15:10] - amplitude_dac_n_1: 程序1负相DAC值高6位
 *
 * 作为程序3的Tphase寄存器时：
 *   [3:0]   - Tphase1_3[11:8]: 程序3正脉冲高4位
 *   [15:4]  - Tphase2_3[11:0]: 程序3负脉冲
 */
#define LMS0820X5_REG_1A_TPHASE1_3       (0x1A)
typedef union {
    uint16_t value;

    /* 作为 amp_dac_n_0 时的定义 */
    struct {
        uint16_t amplitude_dac_n_0 : 10;  /**< [9:0] 程序0负相DAC值 */
        uint16_t amplitude_dac_n_1 : 6;   /**< [15:10] 程序1负相DAC值高6位 */
    } dac_bits;

    /* 作为程序3的Tphase寄存器时的定义 */
    struct {
        uint16_t tphase1_3 : 4;           /**< [3:0] 11-8 程序3正脉宽度 */
        uint16_t tphase2_3 : 12;          /**< [15:4] 11-0 程序3负脉冲 */
    } bits;
} lms0820x5_tphase1_3_t;
/**
 * @brief amp_dac_n_1寄存器 (地址0x1B) - 程序1负相DAC值
 */
#define LMS0820X5_REG_1B_TPHASE3_3       (0x1B)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_1_low : 4;  /**< [3:0] 程序1负相DAC值低4位 */
        uint16_t amplitude_dac_n_2 : 10;     /**< [13:4] 程序2负相DAC值 */
        uint16_t amplitude_dac_n_3_high : 2; /**< [15:14] 程序3负相DAC值高2位 */
    } bits1;
    struct {
        uint16_t tphase3 : 12;           /**< [3:0] 11-8 程序3正脉冲高4位 */
        uint16_t reversed : 4;
    } bits;

} lms0820x5_tphase3_3_t;

/**
 * @brief amp_dac_n_2寄存器 (地址0x1C) - 程序1的Tinterphase
 */
#define LMS0820X5_REG_1C_TINTERPHASE1       (0x1C)
typedef union {
    uint16_t value;
    struct {
        uint16_t tinterphase1 : 8;    /**< [7:0] 程序1的Tinterphase1 */
        uint16_t tinterphase2 : 8;    /**< [15:8] 程序1的Tinterphase2 */
    } bits;
} lms0820x5_tinterphase1_t;

/**
 * @brief amp_dac_n_3寄存器 (地址0x1D) - 程序2的Tinterphase
 */
#define LMS0820X5_REG_1D_TINTERPHASE2       (0x1D)
typedef union {
    uint16_t value;
    struct {
        uint16_t tinterphase1 : 8;    /**< [7:0] 程序2的Tinterphase1 */
        uint16_t tinterphase2 : 8;    /**< [15:8] 程序2的Tinterphase2 */
    } bits;
} lms0820x5_tinterphase2_t;

/**
 * @brief amp_dac_n_4寄存器 (地址0x1E) - 程序3的Tinterphase
 */
#define LMS0820X5_REG_1E_TINTERPHASE3       (0x1E)
typedef union {
    uint16_t value;
    struct {
        uint16_t tinterphase1 : 8;    /**< [7:0] 程序3的Tinterphase1 */
        uint16_t tinterphase2 : 8;    /**< [15:8] 程序3的Tinterphase2 */
    } bits;
} lms0820x5_tinterphase3_t;

/**
 * @brief amp_dac_n_5寄存器 (地址0x1F) - 程序1的Tinterpulse
 */
#define LMS0820X5_REG_1F_TINTERPULSE1       (0x1F)
typedef union {
    uint16_t value;
    struct {
        uint16_t tinterpulse : 16;    /**< [15:0] 程序1的脉冲间延迟 */
    } bits;
} lms0820x5_tinterpulse1_t;

/**
 * @brief amp_dac_n_6寄存器 (地址0x20) - 程序2的Tinterpulse
 */
#define LMS0820X5_REG_20_TINTERPULSE2       (0x20)
typedef union {
    uint16_t value;
    struct {
        uint16_t tinterpulse : 16;    /**< [15:0] 程序2的脉冲间延迟 */
    } bits;
} lms0820x5_tinterpulse2_t;

/**
 * @brief amp_dac_n_7寄存器 (地址0x21) - 程序3的Tinterpulse
 */
#define LMS0820X5_REG_21_TINTERPULSE3       (0x21)
typedef union {
    uint16_t value;
    struct {
        uint16_t tinterpulse3 : 16;    /**< [15:0] 程序3的电荷平衡 */
    } bits;
} lms0820x5_tinterpulse3_t;

/**
 * @brief amp_dac_n_8寄存器 (地址0x22) - 程序1的正向刺激电极
 */
#define LMS0820X5_REG_22_STIM_ON1_0       (0x22)
typedef union {
    uint16_t value;
    struct {
        uint16_t stim_on0_1 : 16;       /**< [15:0] 程序1的正向刺激电极 */
    } bits;
} lms0820x5_stim_on1_0_t;

/**
 * @brief amp_dac_n_9寄存器 (地址0x23) - 程序1的负向刺激电极
 */
#define LMS0820X5_REG_23_STIM_ON1_1       (0x23)
typedef union {
    uint16_t value;
    struct {
        uint16_t stim_on1_1 : 16;       /**< [15:0] 程序1的负向刺激电极 */
    } bits;
} lms0820x5_stim_on1_1_t;

/*==============================================================================
 * eFuse控制寄存器 (0x24-0x28)
 *============================================================================*/

/**
 * @brief efuse_w_data0寄存器 (地址0x24) - eFuse写入数据低16位
 */
#define LMS0820X5_REG_24_EFUSE_W_DATA0    (0x24)
typedef union {
    uint16_t value;  /**< [15:0] eFuse写入数据[15:0] */
} lms0820x5_efuse_w_data0_t;

/**
 * @brief efuse_w_data1寄存器 (地址0x25) - eFuse写入数据高8位和控制
 */
#define LMS0820X5_REG_25_EFUSE_W_DATA1    (0x25)
typedef union {
    uint16_t value;
    struct {
        uint16_t efuse_w_data_high : 8;    /**< [7:0] eFuse写入数据[23:16] */
        uint16_t efuse_data_enable : 6;    /**< [13:8] 数据有效使能 */
        uint16_t reversed : 1;              /**< [14] 保留位 */
        uint16_t efuse_data_valid : 1;      /**< [15] 0->1触发写操作 */
    } bits;
} lms0820x5_efuse_w_data1_t;

/**
 * @brief efuse_r_data0寄存器 (地址0x26) - eFuse读取数据低16位
 */
#define LMS0820X5_REG_26_EFUSE_R_DATA0    (0x26)
typedef union {
    uint16_t value;  /**< [15:0] eFuse读取数据[15:0] */
} lms0820x5_efuse_r_data0_t;

/**
 * @brief efuse_r_data1寄存器 (地址0x27) - eFuse读取数据高8位和状态
 */
#define LMS0820X5_REG_27_EFUSE_R_DATA1    (0x27)
typedef union {
    uint16_t value;
    struct {
        uint16_t efuse_r_data_high : 8;    /**< [7:0] eFuse读取数据[23:16] */
        uint16_t reserved : 6;              /**< [13:8] 保留位 */
        uint16_t efuse_highest_bit_written : 1; /**< [14] 最高数据位写过指示位 */
        uint16_t efuse_auto_read_done : 1;      /**< [15] 上电自动读完成标志 */
    } bits;
} lms0820x5_efuse_r_data1_t;

/**
 * @brief efuse_tburn寄存器 (地址0x28) - eFuse烧写时间控制
 */
#define LMS0820X5_REG_28_EFUSE_TBURN      (0x28)
typedef union {
    uint16_t value;
    struct {
        uint16_t efuse_tburn : 15;      /**< [14:0] 烧写时间(单位us) */
        uint16_t efuse_burn_clk_en : 1;  /**< [15] 时钟门控使能:1-常开,0-自动 */
    } bits;
} lms0820x5_efuse_tburn_t;

/*==============================================================================
 * 程序2/3电极选择寄存器 (0x29-0x2C)
 *============================================================================*/

/**
 * @brief stim_on0_2寄存器 (地址0x29) - 程序2的正向刺激电极
 */
#define LMS0820X5_REG_29_STIM_ON2_0       (0x29)
typedef union {
    uint16_t value;  /**< [15:0] 程序2的正向刺激电极 */
} lms0820x5_stim_on2_0_t;

/**
 * @brief stim_on1_2寄存器 (地址0x2A) - 程序2的负向刺激电极
 */
#define LMS0820X5_REG_2A_STIM_ON2_1       (0x2A)
typedef union {
    uint16_t value;  /**< [15:0] 程序2的负向刺激电极 */
} lms0820x5_stim_on2_1_t;

/**
 * @brief stim_on0_3寄存器 (地址0x2B) - 程序3的正向刺激电极
 */
#define LMS0820X5_REG_2B_STIM_ON3_0       (0x2B)
typedef union {
    uint16_t value;  /**< [15:0] 程序3的正向刺激电极 */
} lms0820x5_stim_on3_0_t;

/**
 * @brief stim_on1_3寄存器 (地址0x2C) - 程序3的负向刺激电极
 */
#define LMS0820X5_REG_2C_STIM_ON3_1       (0x2C)
typedef union {
    uint16_t value;  /**< [15:0] 程序3的负向刺激电极 */
} lms0820x5_stim_on3_1_t;

/*==============================================================================
 * 软启动寄存器 (0x2D-0x30)
 *============================================================================*/

/**
 * @brief softstart_0寄存器 (地址0x2D) - 程序0软启动
 */
#define LMS0820X5_REG_2D_SOFTSTART_0      (0x2D)
typedef union {
    uint16_t value;
    struct {
        uint16_t softstart_step : 10;      /**< [9:0] 刺激周期数(1-1023) */
        uint16_t softstart_inc_value : 5;  /**< [14:10] 每次增加的DAC值(1-31) */
        uint16_t softstart_en : 1;          /**< [15] 软启动使能 */
    } bits;
} lms0820x5_softstart_0_t;

/**
 * @brief softstart_1寄存器 (地址0x2E) - 程序1软启动
 */
#define LMS0820X5_REG_2E_SOFTSTART_1      (0x2E)
typedef union {
    uint16_t value;
    struct {
        uint16_t softstart_step : 10;      /**< [9:0] 刺激周期数(1-1023) */
        uint16_t softstart_inc_value : 5;  /**< [14:10] 每次增加的DAC值(1-31) */
        uint16_t softstart_en : 1;          /**< [15] 软启动使能 */
    } bits;
} lms0820x5_softstart_1_t;

/**
 * @brief softstart_2寄存器 (地址0x2F) - 程序2软启动
 */
#define LMS0820X5_REG_2F_SOFTSTART_2      (0x2F)
typedef union {
    uint16_t value;
    struct {
        uint16_t softstart_step : 10;      /**< [9:0] 刺激周期数(1-1023) */
        uint16_t softstart_inc_value : 5;  /**< [14:10] 每次增加的DAC值(1-31) */
        uint16_t softstart_en : 1;          /**< [15] 软启动使能 */
    } bits;
} lms0820x5_softstart_2_t;

/**
 * @brief softstart_3寄存器 (地址0x30) - 程序3软启动
 */
#define LMS0820X5_REG_30_SOFTSTART_3      (0x30)
typedef union {
    uint16_t value;
    struct {
        uint16_t softstart_step : 10;      /**< [9:0] 刺激周期数(1-1023) */
        uint16_t softstart_inc_value : 5;  /**< [14:10] 每次增加的DAC值(1-31) */
        uint16_t softstart_en : 1;          /**< [15] 软启动使能 */
    } bits;
} lms0820x5_softstart_3_t;

/**
 * @brief softstop_en寄存器 (地址0x31) - 软停止使能
 */
#define LMS0820X5_REG_31_SOFTSTOP_EN      (0x31)
typedef union {
    uint16_t value;
    struct {
        uint16_t softstop_en : 4;       /**< [3:0] 程序3~0的软停止使能 */
        uint16_t reversed : 12;          /**< [15:4] 保留位 */
    } bits;
} lms0820x5_softstop_en_t;

/*==============================================================================
 * 修调数据寄存器 (0x60-0x67)
 *============================================================================*/

/**
 * @brief trimdata_ctrl0寄存器 (地址0x60) - 修调控制0
 */
#define LMS0820X5_REG_60_TRIMDATA_CTRL0   (0x60)
typedef union {
    uint16_t value;
    struct {
        uint16_t trim_ref : 1;           /**< [0] 参考电压控制 */
        uint16_t trim_duvlo : 1;         /**< [1] 欠压控制: 0-停止刺激,1-复位芯片 */
        uint16_t cmd_rst : 1;            /**< [2] 软件复位 */
        uint16_t test_mode : 1;          /**< [3] 测试模式 */
        uint16_t vrecp_trim : 4;         /**< [7:4] VRECP比较电压修调 */
        uint16_t trim_adc : 8;           /**< [15:8] ADC修调 */
    } bits;
} lms0820x5_trimdata_ctrl0_t;

/**
 * @brief digital_version寄存器 (地址0x61) - 数字版本号
 */
#define LMS0820X5_REG_61_DIGITAL_VERSION  (0x61)
typedef union {
    uint16_t value;  /**< [15:0] 数字版本号，X5版本为0x2510? */
} lms0820x5_digital_version_t;

/**
 * @brief stim_para_crc寄存器 (地址0x62) - 刺激参数CRC值
 */
#define LMS0820X5_REG_62_STIM_PARA_CRC    (0x62)
typedef union {
    uint16_t value;  /**< [15:0] 刺激参数CRC值 */
} lms0820x5_stim_para_crc_t;

/**
 * @brief trimdata_ctrl3寄存器 (地址0x63) - 修调控制3
 */
#define LMS0820X5_REG_63_TRIMDATA_CTRL3   (0x63)
typedef union {
    uint16_t value;
    struct {
        uint16_t reserved1 : 4;          /**< [3:0] 保留位 */
        uint16_t i_trim : 2;             /**< [5:4] 电流修调(暂不实现) */
        uint16_t reserved2 : 2;          /**< [7:6] 保留位 */
        uint16_t trim_demod : 1;          /**< [8] 解调使能: 0-数字控制,1-常开(X5删除) */
        uint16_t reserved3 : 3;           /**< [11:9] 保留位 */
        uint16_t reserved4 : 2;           /**< [13:12] 保留位 */
        uint16_t reserved5 : 2;           /**< [15:14] 保留位 */
    } bits;
} lms0820x5_trimdata_ctrl3_t;

/**
 * @brief ldo_trim寄存器 (地址0x64) - LDO修调
 */
#define LMS0820X5_REG_64_LDO_TRIM         (0x64)
typedef union {
    uint16_t value;
    struct {
        uint16_t trim_osc : 4;            /**< [3:0] 振荡器修调 */
        uint16_t temp_trim_ptati : 2;     /**< [5:4] PTAT电流修调 */
        uint16_t temp_trim_ibi : 1;       /**< [6] IBI修调 */
        uint16_t temp_trim_ctati : 1;     /**< [7] CTAT电流修调 */
        uint16_t limit_en : 1;            /**< [8] 限幅使能 */
        uint16_t stim_si : 2;              /**< [10:9] 刺激SI */
        uint16_t adc_trim_osc : 4;         /**< [14:11] ADC振荡器修调 */
        uint16_t adc_testmode : 1;         /**< [15] ADC测试模式 */
    } bits;
} lms0820x5_ldo_trim_t;

/**
 * @brief volt_delay_sample寄存器 (地址0x65) - 电压延迟采样
 */
#define LMS0820X5_REG_65_VOLT_DELAY_SAMPLE (0x65)
typedef union {
    uint16_t value;
    struct {
        uint16_t sample_delay : 16;      /**< [15:0] 延迟刺激周期数 */
    } bits;
} lms0820x5_volt_delay_sample_t;

/*==============================================================================
 * 控制寄存器组 (0x68-0x76)
 *============================================================================*/

/**
 * @brief ctrlreg0寄存器 (地址0x68) - 控制寄存器0
 */
#define LMS0820X5_REG_68_CTRLREG0         (0x68)
typedef union {
    uint16_t value;
    struct {
        uint16_t start_en : 3;               /**< [2:0] 测量使能: bit0-温度,bit1-电压,bit2-阻抗 */
        uint16_t program_sel : 2;            /**< [3:4] 延迟测量程序选择 */
        uint16_t imp_channel_sel : 3;        /**< [5:7] 阴极通道选择 */
        uint16_t imp_test_trim_osc : 4;      /**< [8:11] 阻抗测试时钟修调 */
        uint16_t reserved : 3;               /**< [12:14] 阻抗测试时钟修调 */
        uint16_t current_measure : 1;        /**< [15] 电流测量(可能占用15位，与阳极通道冲突) */
    } bits;
} lms0820x5_ctrlreg0_t;

/**
 * @brief ctrlreg1寄存器 (地址0x69) - 控制寄存器1 (只读)
 */
#define LMS0820X5_REG_69_CTRLREG1         (0x69)
typedef union {
    uint16_t value;
    struct {
        uint16_t stim_done : 1;          /**< [0] 刺激完成标志 */
        uint16_t temp_done : 1;          /**< [1] 温度测量完成 */
        uint16_t vdd_sense_done : 1;     /**< [2] 电压测量完成 */
        uint16_t imp_done : 1;            /**< [3] 阻抗测量完成 */
        uint16_t duvlo : 1;               /**< [4] 欠压指示: 0-欠压,1-正常 */
        uint16_t reserved : 11;           /**< [15:5] 保留位 */
    } bits;
} lms0820x5_ctrlreg1_t;

/**
 * @brief imp_data寄存器 (地址0x6A-0x71) - 阻抗测量数据
 */
#define LMS0820X5_REG_6A_IMP_DATA1        (0x6A)
#define LMS0820X5_REG_6B_IMP_DATA2        (0x6B)
#define LMS0820X5_REG_6C_IMP_DATA3        (0x6C)
#define LMS0820X5_REG_6D_IMP_DATA4        (0x6D)
#define LMS0820X5_REG_6E_IMP_DATA5        (0x6E)
#define LMS0820X5_REG_6F_IMP_DATA6        (0x6F)
#define LMS0820X5_REG_70_IMP_DATA7        (0x70)
#define LMS0820X5_REG_71_IMP_DATA8        (0x71)

typedef union {
    uint16_t value;
    struct {
        uint16_t imp_data : 12;  /**< [11:0] 阻抗测量数据 */
        uint16_t reserved : 4;    /**< [15:12] 保留位 */
    } bits;
} lms0820x5_imp_data_t;

/**
 * @brief vdd_low寄存器 (地址0x72) - 低电压测量数据
 */
#define LMS0820X5_REG_72_VDD_LOW          (0x72)
typedef union {
    uint16_t value;
    struct {
        uint16_t vdd_data : 12;  /**< [11:0] 电压测量数据 */
        uint16_t reserved : 4;    /**< [15:12] 保留位 */
    } bits;
} lms0820x5_vdd_data_t;

/**
 * @brief vdd_high寄存器 (地址0x73) - 高电压测量数据
 */
#define LMS0820X5_REG_73_VDD_HIGH         (0x73)
typedef lms0820x5_vdd_data_t lms0820x5_vdd_high_t;

/**
 * @brief temp_data寄存器 (地址0x74) - 温度测量数据
 */
#define LMS0820X5_REG_74_TEMP_DATA        (0x74)
typedef union {
    uint16_t value;
    struct {
        uint16_t temp_data : 12;  /**< [11:0] 温度测量值 */
        uint16_t reserved : 4;    /**< [15:12] 保留位 */
    } bits;
} lms0820x5_temp_data_t;

/**
 * @brief current_data_neg寄存器 (地址0x75) - 负电流测量数据
 */
#define LMS0820X5_REG_75_CURRENT_DATA_NEG (0x75)
typedef union {
    uint16_t value;
    struct {
        uint16_t current_data : 12;  /**< [11:0] 电流测量值 */
        uint16_t reserved : 4;        /**< [15:12] 保留位 */
    } bits;
} lms0820x5_current_data_t;

/**
 * @brief current_data_plus寄存器 (地址0x76) - 正电流测量数据
 */
#define LMS0820X5_REG_76_CURRENT_DATA_PLUS (0x76)
typedef lms0820x5_current_data_t lms0820x5_current_data_plus_t;

/*==============================================================================
 * 寄存器地址最大值定义
 *============================================================================*/
#define LMS0820X5_REG_MAX              (0x77)

#endif /* __X5_REG_H__ */
