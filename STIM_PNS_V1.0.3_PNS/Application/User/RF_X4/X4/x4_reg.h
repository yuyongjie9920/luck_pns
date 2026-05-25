#ifndef __X4_REG_H__
#define __X4_REG_H__

/*==============================================================================
 * 寄存器地址宏定义 - 与寄存器联合体对应
 *============================================================================*/

/**
 * @brief LMS0820X4 寄存器地址枚举
 * @note 每个枚举值对应寄存器的物理地址，可直接用于寄存器读写操作
 */

#if 0
typedef enum {
    /* ==================== 刺激波形控制寄存器组 (0x00-0x09) ==================== */
    LMS0820X4_REG_TPHASE1        = 0x00,  /**< 正脉冲宽度寄存器 */
    LMS0820X4_REG_TPHASE2        = 0x01,  /**< 负脉冲宽度寄存器 */
    LMS0820X4_REG_TINTERPHASE    = 0x02,  /**< 正负脉冲间隔寄存器 */
    LMS0820X4_REG_TPHASE3        = 0x03,  /**< 电荷平衡阶段宽度寄存器 */
    LMS0820X4_REG_TINTERPULSE    = 0x04,  /**< 脉冲间延迟寄存器 */
    LMS0820X4_REG_TDEFINE        = 0x05,  /**< 刺激结束延迟寄存器 */
    LMS0820X4_REG_STIM_MODE      = 0x06,  /**< 刺激模式控制寄存器 */
    LMS0820X4_REG_PWM_SETTING    = 0x07,  /**< PWM供能设置寄存器 */
    LMS0820X4_REG_TINTERBURST    = 0x08,  /**< 突发刺激间隔寄存器 */
    LMS0820X4_REG_PHASE_CONFIG   = 0x09,  /**< 相位配置寄存器 */

    /* ==================== 电极选择寄存器 (0x0A-0x0B) ==================== */
    LMS0820X4_REG_STIM_ON0       = 0x0A,  /**< 正相电极选择寄存器 */
    LMS0820X4_REG_STIM_ON1       = 0x0B,  /**< 负相电极选择寄存器 */

    /* ==================== ADC/RFID配置寄存器 (0x0C-0x0F) ==================== */
    LMS0820X4_REG_ADC_DIFF_CFG   = 0x0C,  /**< ADC差分配置寄存器 */
    LMS0820X4_REG_RFID_CFG1      = 0x0D,  /**< RFID Tari配置寄存器 */
    LMS0820X4_REG_RFID_CFG2      = 0x0E,  /**< RFID Rt/Delta配置寄存器 */
    LMS0820X4_REG_RFID_CFG3      = 0x0F,  /**< RFID发送周期配置寄存器 */

    /* ==================== 正相DAC幅度寄存器 (0x10-0x19) ==================== */
    LMS0820X4_REG_AMP_DAC_P_0    = 0x10,  /**< 正相DAC值0寄存器(A组) */
    LMS0820X4_REG_AMP_DAC_P_1    = 0x11,  /**< 正相DAC值1寄存器(B组脉冲计数) */
    LMS0820X4_REG_AMP_DAC_P_2    = 0x12,  /**< 正相DAC值2寄存器(B组Tinterburst) */
    LMS0820X4_REG_AMP_DAC_P_3    = 0x13,  /**< 正相DAC值3寄存器(B组正电极信息) */
    LMS0820X4_REG_AMP_DAC_P_4    = 0x14,  /**< 正相DAC值4寄存器(B组负电极信息) */
    LMS0820X4_REG_AMP_DAC_P_5    = 0x15,  /**< 正相DAC值5寄存器(B组Tphase1) */
    LMS0820X4_REG_AMP_DAC_P_6    = 0x16,  /**< 正相DAC值6寄存器(B组Tphase2) */
    LMS0820X4_REG_AMP_DAC_P_7    = 0x17,  /**< 正相DAC值7寄存器(Tinterphase_B) */
    LMS0820X4_REG_AMP_DAC_P_8    = 0x18,  /**< 正相DAC值8寄存器(Tphase3_B) */
    LMS0820X4_REG_AMP_DAC_P_9    = 0x19,  /**< 正相DAC值9寄存器(Tinterpulse_B) */

    /* ==================== 负相DAC幅度寄存器 (0x1A-0x23) ==================== */
    LMS0820X4_REG_AMP_DAC_N_0    = 0x1A,  /**< 负相DAC值0寄存器(A组) */
    LMS0820X4_REG_AMP_DAC_N_1    = 0x1B,  /**< 负相DAC值1寄存器(B组) */
    LMS0820X4_REG_AMP_DAC_N_2    = 0x1C,  /**< 负相DAC值2寄存器(交替切换时间) */
    LMS0820X4_REG_AMP_DAC_N_3    = 0x1D,  /**< 负相DAC值3寄存器(交替切换时间B) */
    LMS0820X4_REG_AMP_DAC_N_4    = 0x1E,  /**< 负相DAC值4寄存器 */
    LMS0820X4_REG_AMP_DAC_N_5    = 0x1F,  /**< 负相DAC值5寄存器 */
    LMS0820X4_REG_AMP_DAC_N_6    = 0x20,  /**< 负相DAC值6寄存器 */
    LMS0820X4_REG_AMP_DAC_N_7    = 0x21,  /**< 负相DAC值7寄存器 */
    LMS0820X4_REG_AMP_DAC_N_8    = 0x22,  /**< 负相DAC值8寄存器 */
    LMS0820X4_REG_AMP_DAC_N_9    = 0x23,  /**< 负相DAC值9寄存器 */

    /* ==================== eFuse控制寄存器 (0x24-0x28) ==================== */
    LMS0820X4_REG_EFUSE_W_DATA0  = 0x24,  /**< eFuse写入数据低16位寄存器 */
    LMS0820X4_REG_EFUSE_W_DATA1  = 0x25,  /**< eFuse写入数据高8位和控制寄存器 */
    LMS0820X4_REG_EFUSE_R_DATA0  = 0x26,  /**< eFuse读取数据低16位寄存器(只读) */
    LMS0820X4_REG_EFUSE_R_DATA1  = 0x27,  /**< eFuse读取数据高8位和状态寄存器(只读) */
    LMS0820X4_REG_EFUSE_TBURN    = 0x28,  /**< eFuse烧写时间控制寄存器 */

    /* ==================== 修调数据寄存器 (0x60-0x67) ==================== */
    LMS0820X4_REG_TRIMDATA_CTRL0 = 0x60,  /**< 修调控制0寄存器 */
    LMS0820X4_REG_TRIMDATA_CTRL1 = 0x61,  /**< 修调控制1寄存器 */
    LMS0820X4_REG_TRIMDATA_CTRL2 = 0x62,  /**< 修调控制2寄存器 */
    LMS0820X4_REG_TRIMDATA_CTRL3 = 0x63,  /**< 修调控制3寄存器 */
    LMS0820X4_REG_LDO_TRIM       = 0x64,  /**< LDO修调寄存器 */
    LMS0820X4_REG_VOLT_DELAY_SAMPLE = 0x65, /**< 电压延迟采样寄存器 */
    LMS0820X4_REG_UID_H          = 0x66,  /**< 唯一ID高16位寄存器(只读) */
    LMS0820X4_REG_OUT_FAB        = 0x67,  /**< 工厂测试输出寄存器(只读) */

    /* ==================== 控制寄存器组 (0x68-0x76) ==================== */
    LMS0820X4_REG_CTRLREG0       = 0x68,  /**< 控制寄存器0 */
    LMS0820X4_REG_CTRLREG1       = 0x69,  /**< 控制寄存器1(只读) */
    LMS0820X4_REG_IMP_DATA1      = 0x6A,  /**< 电压基准值寄存器 */
    LMS0820X4_REG_IMP_DATA2      = 0x6B,  /**< 阻抗测量数据寄存器2 */
    LMS0820X4_REG_IMP_DATA3      = 0x6C,  /**< 阻抗测量数据寄存器3 */
    LMS0820X4_REG_IMP_DATA4      = 0x6D,  /**< 阻抗测量数据寄存器4 */
    LMS0820X4_REG_IMP_DATA5      = 0x6E,  /**< 阻抗测量数据寄存器5 */
    LMS0820X4_REG_IMP_DATA6      = 0x6F,  /**< 阻抗测量数据寄存器6 */
    LMS0820X4_REG_IMP_DATA7      = 0x70,  /**< 阻抗测量数据寄存器7 */
    LMS0820X4_REG_IMP_DATA8      = 0x71,  /**< 阻抗测量数据寄存器8 */
    LMS0820X4_REG_VDD_LOW        = 0x72,  /**< 低电压测量数据寄存器 */
    LMS0820X4_REG_VDD_HIGH       = 0x73,  /**< 高电压测量数据寄存器 */
    LMS0820X4_REG_TEMP_DATA      = 0x74,  /**< 温度测量数据寄存器 */
    LMS0820X4_REG_CURRENT_DATA_NEG = 0x75, /**< 负电流测量数据寄存器 */
    LMS0820X4_REG_CURRENT_DATA_PLUS = 0x76, /**< 正电流测量数据寄存器 */

    /* ==================== 寄存器地址边界 ==================== */
    LMS0820X4_REG_MAX            = 0x77   /**< 寄存器地址最大值(无效地址) */
} lms0820x4_reg_addr_e;
#endif
/*==============================================================================
 * 寄存器组结构体定义
 *============================================================================*/
#if 0
/**
 * @brief LMS0820X4 完整寄存器映射结构体
 * @warning 此结构体用于直接内存映射访问，需确保编译器无填充字节
 */
typedef struct __attribute__((packed)) {
    /* 刺激波形控制寄存器组 (0x00-0x09) */
    lms0820x4_tphase1_t        tphase1;           /* 0x00 */
    lms0820x4_tphase2_t        tphase2;           /* 0x01 */
    lms0820x4_tinterphase_t    tinterphase;       /* 0x02 */
    lms0820x4_tphase3_t        tphase3;           /* 0x03 */
    lms0820x4_tinterpulse_t    tinterpulse;       /* 0x04 */
    lms0820x4_tdefine_t        tdefine;           /* 0x05 */
    lms0820x4_stim_mode_t      stim_mode;         /* 0x06 */
    lms0820x4_pwm_setting_t    pwm_setting;       /* 0x07 */
    lms0820x4_tinterburst_t    tinterburst;       /* 0x08 */
    lms0820x4_phase_config_t   phase_config;      /* 0x09 */

    /* 电极选择寄存器 (0x0A-0x0B) */
    lms0820x4_stim_on0_t       stim_on0;          /* 0x0A */
    lms0820x4_stim_on1_t       stim_on1;          /* 0x0B */

    /* ADC/RFID配置寄存器 (0x0C-0x0F) */
    lms0820x4_adc_diff_cfg_t   adc_diff_cfg;      /* 0x0C */
    lms0820x4_rfid_cfg1_t      rfid_cfg1;         /* 0x0D */
    lms0820x4_rfid_cfg2_t      rfid_cfg2;         /* 0x0E */
    lms0820x4_rfid_cfg3_t      rfid_cfg3;         /* 0x0F */

    /* 正相DAC幅度寄存器 (0x10-0x19) */
    lms0820x4_amp_dac_p_0_t    amp_dac_p_0;       /* 0x10 */
    lms0820x4_amp_dac_p_1_t    amp_dac_p_1;       /* 0x11 */
    lms0820x4_amp_dac_p_2_t    amp_dac_p_2;       /* 0x12 */
    lms0820x4_amp_dac_p_3_t    amp_dac_p_3;       /* 0x13 */
    lms0820x4_amp_dac_p_4_t    amp_dac_p_4;       /* 0x14 */
    lms0820x4_amp_dac_p_5_t    amp_dac_p_5;       /* 0x15 */
    lms0820x4_amp_dac_p_6_t    amp_dac_p_6;       /* 0x16 */
    lms0820x4_amp_dac_p_7_t    amp_dac_p_7;       /* 0x17 */
    lms0820x4_amp_dac_p_8_t    amp_dac_p_8;       /* 0x18 */
    lms0820x4_amp_dac_p_9_t    amp_dac_p_9;       /* 0x19 */

    /* 负相DAC幅度寄存器 (0x1A-0x23) */
    lms0820x4_amp_dac_n_0_t    amp_dac_n_0;       /* 0x1A */
    lms0820x4_amp_dac_n_1_t    amp_dac_n_1;       /* 0x1B */
    lms0820x4_amp_dac_n_2_t    amp_dac_n_2;       /* 0x1C */
    lms0820x4_amp_dac_n_3_t    amp_dac_n_3;       /* 0x1D */
    lms0820x4_amp_dac_n_4_t    amp_dac_n_4;       /* 0x1E */
    uint16_t                   reserved_1F[5];    /* 0x1F-0x23 保留 */

    /* eFuse控制寄存器 (0x24-0x28) */
    lms0820x4_efuse_w_data0_t  efuse_w_data0;     /* 0x24 */
    lms0820x4_efuse_w_data1_t  efuse_w_data1;     /* 0x25 */
    lms0820x4_efuse_r_data0_t  efuse_r_data0;     /* 0x26 */
    lms0820x4_efuse_r_data1_t  efuse_r_data1;     /* 0x27 */
    lms0820x4_efuse_tburn_t    efuse_tburn;       /* 0x28 */

    /* 保留区域 (0x29-0x5F) */
    uint16_t                   reserved_29[0x5F-0x29+1]; /* 0x29-0x5F */

    /* 修调数据寄存器 (0x60-0x67) */
    lms0820x4_trimdata_ctrl0_t trimdata_ctrl0;    /* 0x60 */
    uint16_t                   trimdata_ctrl1;    /* 0x61 */
    uint16_t                   trimdata_ctrl2;    /* 0x62 */
    lms0820x4_trimdata_ctrl3_t trimdata_ctrl3;    /* 0x63 */
    lms0820x4_ldo_trim_t       ldo_trim;          /* 0x64 */
    lms0820x4_volt_delay_sample_t volt_delay_sample; /* 0x65 */
    uint16_t                   uid_h;             /* 0x66 */
    uint16_t                   out_fab;           /* 0x67 */

    /* 控制寄存器组 (0x68-0x76) */
    lms0820x4_ctrlreg0_t       ctrlreg0;          /* 0x68 */
    lms0820x4_ctrlreg1_t       ctrlreg1;          /* 0x69 */
    lms0820x4_imp_data1_t      imp_data1;         /* 0x6A */
    lms0820x4_imp_data_t       imp_data[7];       /* 0x6B-0x71 */
    lms0820x4_vdd_data_t       vdd_low;           /* 0x72 */
    lms0820x4_vdd_data_t       vdd_high;          /* 0x73 */
    lms0820x4_temp_data_t      temp_data;         /* 0x74 */
    lms0820x4_current_data_t   current_data_neg;  /* 0x75 */
    lms0820x4_current_data_t   current_data_plus; /* 0x76 */
} lms0820x4_reg_map_t;
#endif

/**
 * @brief Tphase1寄存器 (地址0x00) - 正脉冲宽度
 */
#define LMS0820X4_REG_00_TPHASE1           (0x00)
typedef union {
    uint16_t value;  /**< 16位寄存器值 */
    struct {
        uint16_t tphase1 : 12;  /**< [11:0] 正脉冲时钟周期数 */
        uint16_t reserved : 4;  /**< [15:12] 保留位 */
    } bits;
} lms0820x4_tphase1_t;

/**
 * @brief Tphase2寄存器 (地址0x01) - 负脉冲宽度
 */
#define LMS0820X4_REG_01_TPHASE2           (0x01)
typedef union {
    uint16_t value;
    struct {
        uint16_t tphase2 : 12;  /**< [11:0] Tphase2: 数值表示负脉冲多少个时钟周期，此数值的设定与phase_config存在一定关系，即必须是当前phase中使用多少组DAC值的倍数。 */
        uint16_t reserved : 4;  /**< [15:12] 保留位 */
    } bits;
} lms0820x4_tphase2_t;

/**
 * @brief Tinterphase寄存器 (地址0x02) - 正负脉冲间隔
 */
#define LMS0820X4_REG_02_TINTERPHASE       (0x02)
typedef union {
    uint16_t value;
    struct {
        uint16_t tinterphase1 : 8;  /**< [7:0] Tinterphase1 时钟周期数 */
        uint16_t tinterphase2 : 8;  /**< [15:8] Tinterphase2 时钟周期数 */
    } bits;
} lms0820x4_tinterphase_t;

/**
 * @brief Tphase3寄存器 (地址0x03) - 电荷平衡阶段宽度
 */
#define LMS0820X4_REG_03_TPHASE3           (0x03)
typedef union {
    uint16_t value;
    struct {
        uint16_t tphase3 : 12;  /**< [11:0] 电荷平衡阶段时钟周期数 */
        uint16_t reserved : 4;  /**< [15:12] 保留位 */
    } bits;
} lms0820x4_tphase3_t;

/**
 * @brief Tinterpulse寄存器 (地址0x04) - 脉冲间延迟
 */
#define  LMS0820X4_REG_04_TINTERPULSE          (0x04)
typedef union {
    uint16_t value;
    struct {
        uint16_t tinterpulse : 14;  /**< [13:0] Tphase3与下一周期延迟 */
        uint16_t reserved : 2;      /**< [15:14] 保留位 */
    } bits;
} lms0820x4_tinterpulse_t;

/**
 * @brief Tdefine寄存器 (地址0x05) - 刺激结束延迟
 */
#define LMS0820X4_REG_05_TDEFINE           (0x05)
typedef union {
    uint16_t value;
    struct {
        uint16_t stim_td1 : 8;      /**< [7:0] stim_td1: 刺激电路开启与开始刺激之间的延迟时间, 典型值128us */
        uint16_t stim_td2 : 2;      /**< [9:8] stim_td2, stim_td2修改为固定的1 clock cycle的延时 */
        uint16_t stim_td3 : 6;      /**< [15:10] stim_td3: 电荷平衡结束与STIM_EN\STIM_ON\DAC_ENI关闭之间的延迟时间, 典型值32us */
    } bits;
} lms0820x4_tdefine_t;

/**
 * @brief STIM_MODE寄存器 (地址0x06) - 刺激模式控制
 */
#define LMS0820X4_REG_06_STIM_MODE         (0x06)

typedef union {
    uint16_t value; /**< 16位寄存器值 */
    struct {
        uint16_t stim_start : 1;             /**< [0] 给刺激波形模块的开始刺激信号 */
        uint16_t para_vld : 1;                /**< [1] 给刺激波形模块的配置参数有效信号，更新参数使用 */
        uint16_t stim_stop : 1;                /**< [2] 停止刺激，1：停止刺激，0：不停止 */
        uint16_t stim_mode : 2;                 /**< [4:3] 刺激模式
                                               * 00: 简单模式
                                               * 01: 任意模式
                                               * 10: 突发刺激模式
                                               */
        uint16_t stim_low_power : 1;          /**< [5] 低功耗模式 */
        uint16_t inturn_mode : 1;                /**< [6] 轮流刺激模式
                                               * 0: 单一参数刺激波形
                                               * 1: 默认A参数和B参数刺激轮流进行
                                               * 注：只有简单模式和突发刺激模式有轮流刺激模式
                                               */
        uint16_t reserved : 1;                 /**< [7] 保留位 */
        uint16_t pulse_cnt : 8;                  /**< [15:8] 一个burst里面的脉冲数
                                               * 取值范围：1-255
                                               */
    } bits;
} lms0820x4_stim_mode_t;

/**
 * @brief pwm_setting寄存器 (地址0x07) - PWM供能设置
 */
#define LMS0820X4_REG_07_PWM_SETTING       (0x07)
typedef union {
    uint16_t value;
    struct {
        uint16_t pwm_power_dly : 13;  /**< [12:0] 供能检测后延迟 */
        uint16_t pwm_mode : 1;        /**< [13] : 1表示占空比供能模式，0：常供能模式*/
        uint16_t reserved : 2;        /**< [15:14] 保留位 */
    } bits;
} lms0820x4_pwm_setting_t;

/**
 * @brief Tinterburst寄存器 (地址0x08) - 突发刺激间隔
 */
#define LMS0820X4_REG_08_TINTERBURST       (0x08)
typedef union {
    uint16_t value;  /**< [15:0] 突发模式刺激簇间隔时间 */
} lms0820x4_tinterburst_t;

/**
 * @brief phase_config寄存器 (地址0x09) - 相位配置
 */
#define LMS0820X4_REG_09_PHASE_CONFIG      (0x09)
typedef union {
    uint16_t value;
    struct {
        uint16_t phase1_config : 3;   /**< [2:0] Phase1 DAC组数 @ref lms0820x4_phase_dac_t */
        uint16_t phase2_config : 3;   /**< [5:3] Phase2 3'b000: 表示phase1周期内只使用1组值，3’b001表示使用2组值，即2**n个值，最大3'b100, 2**4=16组值 */
        uint16_t reserved1 : 2;       /**< [7:6] 保留位 */
        uint16_t stim_clk_div_value : 6; /**< [13:8] 刺激时钟分频: n+1分频 */
        uint16_t reserved2 : 2;       /**< [15:14] 保留位 */
    } bits;
} lms0820x4_phase_config_t;

/**
 * @brief STIM_ON0寄存器 (地址0x0A) - 正相电极选择
 */
#define LMS0820X4_REG_0A_STIM_ON0          (0x0A)
typedef union {
    uint16_t value;  /**< [15:0] {DCASEL_p, DANSEL_p} */
    struct {
        uint16_t dansel_p : 8;  /**< DANSEL_p */
        uint16_t dcasel_p : 8;  /**< DCASEL_p */
    } bits;
} lms0820x4_stim_on0_t;

/**
 * @brief STIM_ON1寄存器 (地址0x0B) - 负相电极选择
 */
#define LMS0820X4_REG_0B_STIM_ON1          (0x0B)
typedef union {
    uint16_t value;  /**< [15:0] {DCASEL_n, DANSEL_n} */
    struct {
        uint16_t dansel_n : 8;  /**< DANSEL_n */
        uint16_t dcasel_n : 8;  /**< DCASEL_n */
    } bits;
} lms0820x4_stim_on1_t;

/**
 * @brief adc_diff_cfg寄存器 (地址0x0C) - ADC差分配置
 */
#define LMS0820X4_REG_0C_ADC_DIFF_CFG      (0x0C)
typedef union {
    uint16_t value;
    struct {
        uint16_t reserved1 : 3;      /**< [2:0] 保留位 */
        uint16_t reserved2 : 3;      /**< [5:3] 保留位 */
        uint16_t reserved3 : 2;      /**< [7:6] 保留位 */
        uint16_t reserved4 : 3;      /**< [10:8] 保留位 */
        uint16_t sel_demod : 1;      /**< [11] 解调信号选择: 0-原始,1-滤波后 */
        uint16_t pdelt : 4;          /**< [15:12] 数字滤波器宽度 */
    } bits;
} lms0820x4_adc_diff_cfg_t;

/**
 * @brief rfid_cfg1寄存器 (地址0x0D) - RFID Tari配置
 */
#define LMS0820X4_REG_0D_RFID_CFG1         (0x0D)
typedef union {
    uint16_t value;
    struct {
        uint16_t tari_min : 8;  /**< [7:0] Tari最小值 */
        uint16_t tari_max : 8;  /**< [15:8] Tari最大值 */
    } bits;
} lms0820x4_rfid_cfg1_t;

/**
 * @brief rfid_cfg2寄存器 (地址0x0E) - RFID Rt/Delta配置
 */
#define LMS0820X4_REG_0E_RFID_CFG2         (0x0E)
typedef union {
    uint16_t value;
    struct {
        uint16_t delta : 8;    /**< [7:0] Delta值 */
        uint16_t rt_max : 8;   /**< [15:8] Rt最大值 */
    } bits;
} lms0820x4_rfid_cfg2_t;

/**
 * @brief rfid_cfg3寄存器 (地址0x0F) - RFID发送周期配置
 */
#define LMS0820X4_REG_0F_RFID_CFG3         (0x0F)
typedef union {
    uint16_t value;
    struct {
        uint16_t tr_max : 9;       /**< [8:0] TrMax (已弃用) */
        uint16_t reserved : 2;     /**< [10:9] 保留位 */
        uint16_t rfid_tx_data_period : 5; /**< [15:11] 发送数据周期(偶数,最大30) */
    } bits;
} lms0820x4_rfid_cfg3_t;

/**
 * @brief amp_dac_p_0寄存器 (地址0x10) - 正相DAC值 (A组)
 */
#define LMS0820X4_REG_10_AMP_DAC_P_0       (0x10)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_0 : 10;  /**< [9:0] A组正相DAC值0 (简单/突发模式phase1) */
        uint16_t amplitude_dac_p_1 : 6;   /**< [15:10] B组正相DAC值低6位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t amplitude_dac_p_A : 10;  /**< [9:0] A组正相DAC值 */
        uint16_t amplitude_dac_p_B : 6;   /**< [15:10] B组正相DAC值高6位 */
    } ab_bits;
} lms0820x4_amp_dac_p_0_t;

/**
 * @brief amp_dac_p_1寄存器 (地址0x11) - 正相DAC值 (B组脉冲计数)
 */
#define LMS0820X4_REG_11_AMP_DAC_P_1       (0x11)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_1_low : 4;  /**< [3:0] 正相DAC值1低4位 */
        uint16_t amplitude_dac_p_2 : 10;     /**< [13:4] 正相DAC值2 */
        uint16_t amplitude_dac_p_3_high : 2; /**< [15:14] 正相DAC值3高2位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t amplitude_dac_p_B_h : 4;   /**< [3:0] B组正相DAC值高4位 */
        uint16_t pulse_cnt_B : 8;              /**< [11:4] B组脉冲计数 */
        uint16_t reserved : 4;                  /**< [15:12] 保留位 */
    } ab_bits;
} lms0820x4_amp_dac_p_1_t;

/**
 * @brief amp_dac_p_2寄存器 (地址0x12) - 正相DAC值 (B组Tinterburst)
 */
#define LMS0820X4_REG_12_AMP_DAC_P_2       (0x12)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_3_low : 2;  /**< [1:0] 正相DAC值3低2位 */
        uint16_t amplitude_dac_p_4 : 8;      /**< [9:2] 正相DAC值4 */
        uint16_t amplitude_dac_p_5_high : 6; /**< [15:10] 正相DAC值5高6位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t tinterburst_B : 16;  /**< [15:0] B组突发刺激间隔 */
    } ab_bits;
} lms0820x4_amp_dac_p_2_t;

/**
 * @brief amp_dac_p_3寄存器 (地址0x13) - 正相DAC值 (B组正电极信息)
 */
#define LMS0820X4_REG_13_AMP_DAC_P_3       (0x13)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_4_high : 2;  /**< [1:0] 正相DAC值4高2位 */
        uint16_t amplitude_dac_p_5_low : 10;  /**< [11:2] 正相DAC值5 */
        uint16_t amplitude_dac_p_6_high : 4;  /**< [15:12] 正相DAC值6高4位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t b_dansel_p : 8;  /**< DANSEL_p */
        uint16_t b_dcasel_p : 8;  /**< DCASEL_p */
    } ab_bits;
} lms0820x4_amp_dac_p_3_t;

/**
 * @brief amp_dac_p_4寄存器 (地址0x14) - 正相DAC值 (B组负电极信息)
 */
#define LMS0820X4_REG_14_AMP_DAC_P_4       (0x14)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_6_low : 4;  /**< [3:0] 正相DAC值6低4位 */
        uint16_t amplitude_dac_p_7 : 10;     /**< [13:4] 正相DAC值7 */
        uint16_t amplitude_dac_p_8_high : 2; /**< [15:14] 正相DAC值8高2位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t b_dansel_n : 8;  /**< DANSEL_n */
        uint16_t b_dcasel_n : 8;  /**< DCASEL_n */
    } ab_bits;
} lms0820x4_amp_dac_p_4_t;

/**
 * @brief amp_dac_p_5寄存器 (地址0x15) - 正相DAC值 (B组Tphase1)
 */
#define LMS0820X4_REG_15_AMP_DAC_P_5       (0x15)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_8_low : 10;  /**< [9:0] 正相DAC值8 */
        uint16_t amplitude_dac_p_9_high : 6;  /**< [15:10] 正相DAC值9高6位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t tphase1_B : 11;    /**< [11:0] B组正脉冲宽度 */
        uint16_t reserved : 4;      /**< [15:11] 保留位 */
    } ab_bits;
} lms0820x4_amp_dac_p_5_t;

/**
 * @brief amp_dac_p_6寄存器 (地址0x16) - 正相DAC值 (B组Tphase2)
 */
#define LMS0820X4_REG_16_AMP_DAC_P_6       (0x16)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_9_low : 4;   /**< [3:0] 正相DAC值9低4位 */
        uint16_t amplitude_dac_p_10 : 10;     /**< [13:4] 正相DAC值10 */
        uint16_t amplitude_dac_p_11_high : 2; /**< [15:14] 正相DAC值11高2位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t tphase2_B : 11;    /**< [11:0] B组负脉冲宽度 */
        uint16_t reserved : 4;      /**< [15:11] 保留位 */
    } ab_bits;
} lms0820x4_amp_dac_p_6_t;

/**
 * @brief amp_dac_p_7寄存器 (地址0x17) - 正相DAC值 (Tinterphase_B)
 */
#define LMS0820X4_REG_17_AMP_DAC_P_7       (0x17)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_11_low : 2;  /**< [1:0] 正相DAC值11低2位 */
        uint16_t amplitude_dac_p_12 : 8;      /**< [9:2] 正相DAC值12 */
        uint16_t amplitude_dac_p_13_high : 6; /**< [15:10] 正相DAC值13高6位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t tinterphase1_B : 8;  /**< [7:0] B组Tinterphase1 */
        uint16_t tinterphase2_B : 8;  /**< [15:8] B组Tinterphase2 */
    } ab_bits;
} lms0820x4_amp_dac_p_7_t;

/**
 * @brief amp_dac_p_8寄存器 (地址0x18) - 正相DAC值 (Tphase3_B)
 */
#define LMS0820X4_REG_18_AMP_DAC_P_8       (0x18)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_12_high : 2;  /**< [1:0] 正相DAC值12高2位 */
        uint16_t amplitude_dac_p_13_low : 10;  /**< [11:2] 正相DAC值13 */
        uint16_t amplitude_dac_p_14_high : 4;  /**< [15:12] 正相DAC值14高4位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t reserved : 4;      /**< [15:12] 保留位 */
        uint16_t tphase3_B : 12;    /**< [11:0] B组电荷平衡阶段宽度 */
    } ab_bits;
} lms0820x4_amp_dac_p_8_t;

/**
 * @brief amp_dac_p_9寄存器 (地址0x19) - 正相DAC值 (Tinterpulse_B)
 */
#define LMS0820X4_REG_19_AMP_DAC_P_9       (0x19)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_p_14_low : 4;  /**< [3:0] 正相DAC值14低4位 */
        uint16_t amplitude_dac_p_15 : 10;     /**< [13:4] 正相DAC值15 */
        uint16_t reserved : 2;                 /**< [15:14] 保留位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t tinterpulse_B : 14;  /**< [13:0] B组脉冲间延迟 */
        uint16_t reserved : 2;         /**< [15:14] 保留位 */
    } ab_bits;
} lms0820x4_amp_dac_p_9_t;

/**
 * @brief amp_dac_n_0寄存器 (地址0x1A) - 负相DAC值 (A组)
 */
#define LMS0820X4_REG_1A_AMP_DAC_N_0       (0x1A)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_0 : 10;  /**< [9:0] 负相DAC值0 (简单/突发模式phase2) */
        uint16_t amplitude_dac_n_1 : 6;   /**< [15:10] 负相DAC值1高6位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t amplitude_dac_n_A : 10;  /**< [9:0] A组负相DAC值 */
        uint16_t amplitude_dac_n_B : 6;   /**< [15:10] B组负相DAC值高6位 */
    } ab_bits;
} lms0820x4_amp_dac_n_0_t;

/**
 * @brief amp_dac_n_1寄存器 (地址0x1B) - 负相DAC值 (B组)
 */
#define LMS0820X4_REG_1B_AMP_DAC_N_1       (0x1B)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_1_low : 4;  /**< [3:0] 负相DAC值1低4位 */
        uint16_t amplitude_dac_n_2 : 10;     /**< [13:4] 负相DAC值2 */
        uint16_t amplitude_dac_n_3_high : 2; /**< [15:14] 负相DAC值3高2位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t amplitude_dac_n_B_h : 4;  /**< [3:0] B组负相DAC值高4位 */
        uint16_t reserved1 : 8;               /**< [11:4] 保留位 */
        uint16_t reserved2 : 4;               /**< [15:12] 保留位 */
    } ab_bits;
} lms0820x4_amp_dac_n_1_t;

/**
 * @brief amp_dac_n_2寄存器 (地址0x1C) - 负相DAC值 (交替切换时间)
 */
#define LMS0820X4_REG_1C_AMP_DAC_N_2       (0x1C)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_3_low : 2;  /**< [1:0] 负相DAC值3低2位 */
        uint16_t amplitude_dac_n_4 : 8;      /**< [9:2] 负相DAC值4 */
        uint16_t amplitude_dac_n_5_high : 6; /**< [15:10] 负相DAC值5高6位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t invert_cnt : 16;  /**< [15:0] A组切换时间(0=65536) */
    } ab_bits;
} lms0820x4_amp_dac_n_2_t;

/**
 * @brief amp_dac_n_3寄存器 (地址0x1D) - 负相DAC值 (交替切换时间B)
 */
#define LMS0820X4_REG_1D_AMP_DAC_N_3       (0x1D)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_4_high : 2;  /**< [1:0] 负相DAC值4高2位 */
        uint16_t amplitude_dac_n_5_low : 10;  /**< [11:2] 负相DAC值5 */
        uint16_t amplitude_dac_n_6_high : 4;  /**< [15:12] 负相DAC值6高4位 */
    } bits;

    /* 交替刺激模式时的定义 */
    struct {
        uint16_t invert_cnt_b : 16;  /**< [15:0] B组切换时间(0=65536) */
    } ab_bits;
} lms0820x4_amp_dac_n_3_t;

/**
 * @brief amp_dac_n_4寄存器 (地址0x1E) - 负相DAC值
 */
#define LMS0820X4_REG_1E_AMP_DAC_N_4       (0x1E)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_6_low : 4;  /**< [3:0] 负相DAC值6低4位 */
        uint16_t amplitude_dac_n_7 : 10;     /**< [13:4] 负相DAC值7 */
        uint16_t amplitude_dac_n_8_high : 2; /**< [15:14] 负相DAC值8高2位 */
    } bits;
} lms0820x4_amp_dac_n_4_t;

/**
 * @brief amp_dac_n_5寄存器 (地址0x1F)
 */
#define LMS0820X4_REG_1F_AMP_DAC_N_5       (0x1F)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_8_low : 4;  /**< [3:0] 负相DAC值8低4位 */
        uint16_t amplitude_dac_n_9 : 10;     /**< [13:4] 负相DAC值9 */
        uint16_t reserved : 2;                /**< [15:14] 保留位 */
    } bits;
} lms0820x4_amp_dac_n_5_t;

/**
 * @brief amp_dac_n_6寄存器 (地址0x20)
 */
#define LMS0820X4_REG_20_AMP_DAC_N_6       (0x20)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_9_low : 2;  /**< [1:0] 负相DAC值9低2位 */
        uint16_t amplitude_dac_n_10 : 8;     /**< [9:2] 负相DAC值10 */
        uint16_t amplitude_dac_n_11_high : 6; /**< [15:10] 负相DAC值11高6位 */
    } bits;
} lms0820x4_amp_dac_n_6_t;

/**
 * @brief amp_dac_n_7寄存器 (地址0x21)
 */
#define LMS0820X4_REG_21_AMP_DAC_N_7       (0x21)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_11_low : 2;  /**< [1:0] 负相DAC值11低2位 */
        uint16_t amplitude_dac_n_12 : 8;      /**< [9:2] 负相DAC值12 */
        uint16_t amplitude_dac_n_13_high : 6; /**< [15:10] 负相DAC值13高6位 */
    } bits;
} lms0820x4_amp_dac_n_7_t;

/**
 * @brief amp_dac_n_8寄存器 (地址0x22)
 */
#define LMS0820X4_REG_22_AMP_DAC_N_8       (0x22)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_13_low : 2;  /**< [1:0] 负相DAC值13低2位 */
        uint16_t amplitude_dac_n_14 : 8;      /**< [9:2] 负相DAC值14 */
        uint16_t amplitude_dac_n_15_high : 6; /**< [15:10] 负相DAC值15高6位 */
    } bits;
} lms0820x4_amp_dac_n_8_t;

/**
 * @brief amp_dac_n_9寄存器 (地址0x23)
 */
#define LMS0820X4_REG_23_AMP_DAC_N_9       (0x23)
typedef union {
    uint16_t value;
    struct {
        uint16_t amplitude_dac_n_14 : 6;  //未知
        uint16_t amplitude_dac_n_15 : 10;  //未知
    } bits;
} lms0820x4_amp_dac_n_9_t;

/**
 * @brief efuse_w_data0寄存器 (地址0x24) - eFuse写入数据低16位
 */
#define LMS0820X4_REG_24_EFUSE_W_DATA0    (0x24)
typedef union {
    uint16_t value;  /**< [15:0] eFuse写入数据[15:0] */
} lms0820x4_efuse_w_data0_t;

/**
 * @brief efuse_w_data1寄存器 (地址0x25) - eFuse写入数据高8位和控制
 */
#define LMS0820X4_REG_25_EFUSE_W_DATA1    (0x25)
typedef union {
    uint16_t value;
    struct {
        uint16_t efuse_w_data_high : 8;    /**< [7:0] eFuse写入数据[23:16] */
        uint16_t efuse_data_enable : 6;    /**< [13:8] 数据有效使能(每bit对应4bit数据) */
        uint16_t reserved : 1;              /**< [14] 保留位 */
        uint16_t efuse_data_valid : 1;      /**< [15] 0->1触发写操作 */
    } bits;
} lms0820x4_efuse_w_data1_t;

/**
 * @brief efuse_r_data0寄存器 (地址0x26) - eFuse读取数据低16位
 */
#define LMS0820X4_REG_26_EFUSE_R_DATA0    (0x26)
typedef union {
    uint16_t value;  /**< [15:0] eFuse读取数据[15:0] (只读) */
} lms0820x4_efuse_r_data0_t;

/**
 * @brief efuse_r_data1寄存器 (地址0x27) - eFuse读取数据高8位和状态
 */
#define LMS0820X4_REG_27_EFUSE_R_DATA1    (0x27)
typedef union {
    uint16_t value;
    struct {
        uint16_t efuse_r_data_high : 8;     /**< [7:0] eFuse读取数据[23:16] (只读) */
        uint16_t reserved : 6;               /**< [13:8] 保留位 */
        uint16_t efuse_auto_read_done : 1;   /**< [14] 上电自动读完成标志(只读) */
        uint16_t efuse_auto_read_done_dup : 1; /**< [15] 上电自动读完成标志(只读) */
    } bits;
} lms0820x4_efuse_r_data1_t;

/**
 * @brief efuse_tburn寄存器 (地址0x28) - eFuse烧写时间控制
 */
#define LMS0820X4_REG_28_EFUSE_TBURN      (0x28)
typedef union {
    uint16_t value;
    struct {
        uint16_t efuse_tburn : 14;      /**< [13:0] 烧写时间(单位us) */
        uint16_t reserved : 1;           /**< [14] 保留位 */
        uint16_t efuse_burn_clk_en : 1;  /**< [15] 时钟门控使能:1-常开,0-自动 */
    } bits;
} lms0820x4_efuse_tburn_t;

/**
 * @brief trimdata_ctrl0寄存器 (地址0x60) - 修调控制0
 */
#define LMS0820X4_REG_60_TRIMDATA_CTRL0   (0x60)
typedef union {
    uint16_t value;
    struct {
        uint16_t trim_ref : 8;           /**< [7:0] 参考电压控制 */
        uint16_t trim_adc : 8;           /**< [15:8] ADC修调 */
    } bits;

    struct {
        uint16_t trim_ref : 8;           /**< [7:0] 参考电压控制 */
        uint16_t pdho_sel : 1;           /**< [8] PDHO选择 */
        uint16_t imp_gain : 2;           /**< [10:9] 阻抗增益 */
        uint16_t isel_opa : 1;           /**< [11] OPA电流选择 */
        uint16_t isel_dac : 2;           /**< [13:12] DAC电流选择 */
        uint16_t isel_adc : 2;           /**< [15:14] ADC电流选择 */
    } trim_bits;
} lms0820x4_trimdata_ctrl0_t;

/**
 * @brief trimdata_ctrl1寄存器 (地址0x61)
 */
#define LMS0820X4_REG_61_TRIMDATA_CTRL1   (0x61)
typedef union {
    uint16_t value;
    struct {
        uint16_t trim_rc_osc : 8;        /**< [7:0] RC振荡器修调 */
        uint16_t reserved : 8;            /**< [15:8] 保留位 */
    } bits;
} lms0820x4_trimdata_ctrl1_t;

/**
 * @brief trimdata_ctrl2寄存器 (地址0x62)
 */
#define LMS0820X4_REG_62_TRIMDATA_CTRL2   (0x62)
typedef union {
    uint16_t value;
    struct {
        uint16_t trim_rc_osc_sel : 8;    /**< [7:0] RC振荡器选择修调 */
        uint16_t reserved : 8;            /**< [15:8] 保留位 */
    } bits;
} lms0820x4_trimdata_ctrl2_t;

/**
 * @brief trimdata_ctrl3寄存器 (地址0x63) - 修调控制3
 */
#define LMS0820X4_REG_63_TRIMDATA_CTRL3   (0x63)
typedef union {
    uint16_t value;
    struct {
        uint16_t stim_trim : 8;          /**< [7:0] 刺激修调 */
        uint16_t trim_demod : 1;          /**< [8] 解调使能: 0-数字控制,1-常开 */
        uint16_t reserved1 : 3;           /**< [11:9] 保留位 */
        uint16_t rd_cfg_bak_addr : 2;     /**< [13:12] 配置备份地址(未使用) */
        uint16_t reserved2 : 2;           /**< [15:14] 保留位 */
    } bits;
} lms0820x4_trimdata_ctrl3_t;

/**
 * @brief ldo_trim寄存器 (地址0x64) - LDO修调
 */
#define LMS0820X4_REG_64_LDO_TRIM         (0x64)
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
} lms0820x4_ldo_trim_t;

/**
 * @brief volt_delay_sample寄存器 (地址0x65) - 电压延迟采样
 */
#define LMS0820X4_REG_65_VOLT_DELAY_SAMPLE (0x65)
typedef union {
    uint16_t value;
    struct {
        uint16_t sample_delay : 8;        /**< [7:0] 延迟刺激周期数 */
        uint16_t sample_enable : 1;       /**< [8] 电压延迟测量使能 */
        uint16_t reserved : 7;             /**< [15:9] 保留位 */
    } bits;
} lms0820x4_volt_delay_sample_t;

/**
 * @brief uid_h寄存器 (地址0x66) - 唯一ID高16位
 */
#define LMS0820X4_REG_66_UID_H            (0x66)
typedef union {
    uint16_t value;  /**< [15:0] 唯一ID高16位 (只读) */
} lms0820x4_uid_h_t;

/**
 * @brief out_fab寄存器 (地址0x67) - 工厂测试输出
 */
#define LMS0820X4_REG_67_OUT_FAB          (0x67)
typedef union {
    uint16_t value;  /**< [15:0] 工厂测试输出 (只读) */
} lms0820x4_out_fab_t;

/**
 * @brief ctrlreg0寄存器 (地址0x68) - 控制寄存器0
 */
#define LMS0820X4_REG_68_CTRLREG0         (0x68)
typedef union {
    uint16_t value;
    struct {
        uint16_t start_en : 3;             /**< [2:0] 测量使能: bit0-温度,bit1-电压,bit2-阻抗 */
        uint16_t cmd_rst : 1;               /**< [3] 软件复位(写1再写0完成复位) */
        uint16_t test_mode : 1;              /**< [4] 测试模式: 1-测试模式 */
        uint16_t imp_channel_sel : 3;       /**< [7:5] X4C阴极通道选择 */
        uint16_t imp_test_trim_osc : 4;     /**< [11:8] 阻抗测试时钟修调 */
        uint16_t imp_anode_channel_sel : 3; /**< [14:12] 无用 */
        uint16_t current_measure : 1;        /**< [15] 电流测量 */
    } bits;
} lms0820x4_ctrlreg0_t;

/**
 * @brief ctrlreg1寄存器 (地址0x69) - 控制寄存器1 (只读)
 */
#define LMS0820X4_REG_69_CTRLREG1         (0x69)
typedef union {
    uint16_t value;
    struct {
        uint16_t stim_done : 1;          /**< [0] 刺激完成标志 */
        uint16_t temp_done : 1;          /**< [1] 温度测量完成 */
        uint16_t vdd_sense_done : 1;     /**< [2] 电压测量完成 */
        uint16_t imp_done : 1;            /**< [3] 阻抗测量完成 */
        uint16_t duvlo : 1;               /**< [4] 欠压指示: 0-欠压(<5V),1-正常 */
        uint16_t reserved : 11;           /**< [15:5] 保留位 */
    } bits;
} lms0820x4_ctrlreg1_t;

/**
 * @brief imp_data1寄存器 (地址0x6A) - 电压基准值
 */
#define LMS0820X4_REG_6A_IMP_DATA1        (0x6A)
typedef union {
    uint16_t value;
    struct {
        uint16_t voltage_reference : 12; /**< [11:0] 电压基准值 */
        uint16_t reserved : 4;            /**< [15:12] 保留位 */
    } bits;
} lms0820x4_imp_data1_t;

/**
 * @brief imp_data2寄存器 (地址0x6B) - 阻抗测量数据
 */
#define LMS0820X4_REG_6B_IMP_DATA2        (0x6B)
typedef union {
    uint16_t value;
    struct {
        uint16_t imp_data : 12;  /**< [11:0] 阻抗测量数据 */
        uint16_t reserved : 4;          /**< [15:12] 保留位 */
    } bits;
} lms0820x4_imp_data_t;

/**
 * @brief imp_data3寄存器 (地址0x6C) - 阻抗测量数据
 */
#define LMS0820X4_REG_6C_IMP_DATA3        (0x6C)
typedef lms0820x4_imp_data_t lms0820x4_imp_data3_t;

/**
 * @brief imp_data4寄存器 (地址0x6D) - 阻抗测量数据
 */
#define LMS0820X4_REG_6D_IMP_DATA4        (0x6D)
typedef lms0820x4_imp_data_t lms0820x4_imp_data4_t;

/**
 * @brief imp_data5寄存器 (地址0x6E) - 阻抗测量数据
 */
#define LMS0820X4_REG_6E_IMP_DATA5        (0x6E)
typedef lms0820x4_imp_data_t lms0820x4_imp_data5_t;

/**
 * @brief imp_data6寄存器 (地址0x6F) - 阻抗测量数据
 */
#define LMS0820X4_REG_6F_IMP_DATA6        (0x6F)
typedef lms0820x4_imp_data_t lms0820x4_imp_data6_t;

/**
 * @brief imp_data7寄存器 (地址0x70) - 阻抗测量数据
 */
#define LMS0820X4_REG_70_IMP_DATA7        (0x70)
typedef lms0820x4_imp_data_t lms0820x4_imp_data7_t;

/**
 * @brief imp_data8寄存器 (地址0x71) - 阻抗测量数据
 */
#define LMS0820X4_REG_71_IMP_DATA8        (0x71)
typedef lms0820x4_imp_data_t lms0820x4_imp_data8_t;

/**
 * @brief vdd_low寄存器 (地址0x72) - 低电压测量数据
 */
#define LMS0820X4_REG_72_VDD_LOW          (0x72)
typedef union {
    uint16_t value;
    struct {
        uint16_t vdd_data : 12;  /**< [11:0] 电压测量数据 */
        uint16_t reserved : 4;    /**< [15:12] 保留位 */
    } bits;
} lms0820x4_vdd_data_t;

/**
 * @brief vdd_high寄存器 (地址0x73) - 高电压测量数据
 */
#define LMS0820X4_REG_73_VDD_HIGH         (0x73)
typedef lms0820x4_vdd_data_t lms0820x4_vdd_high_t;

/**
 * @brief temp_data寄存器 (地址0x74) - 温度测量数据
 */
#define LMS0820X4_REG_74_TEMP_DATA        (0x74)
typedef union {
    uint16_t value;
    struct {
        uint16_t temp_data : 12;  /**< [11:0] 温度测量值 */
        uint16_t reserved : 4;    /**< [15:12] 保留位 */
    } bits;
} lms0820x4_temp_data_t;

/**
 * @brief current_data_neg寄存器 (地址0x75) - 负电流测量数据
 */
#define LMS0820X4_REG_75_CURRENT_DATA_NEG (0x75)
typedef union {
    uint16_t value;
    struct {
        uint16_t current_data : 12;  /**< [11:0] 电流测量值 */
        uint16_t reserved : 4;        /**< [15:12] 保留位 */
    } bits;
} lms0820x4_current_data_t;

/**
 * @brief current_data_plus寄存器 (地址0x76) - 正电流测量数据
 */
#define LMS0820X4_REG_76_CURRENT_DATA_PLUS (0x76)
typedef lms0820x4_current_data_t lms0820x4_current_data_plus_t;

/*==============================================================================
 * 寄存器地址最大值定义
 *============================================================================*/
#define LMS0820X4_REG_MAX              (0x77)


#endif /* __X4_REG_H__ */
