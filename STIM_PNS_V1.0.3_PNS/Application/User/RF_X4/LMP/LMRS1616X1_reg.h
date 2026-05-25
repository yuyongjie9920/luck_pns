///**
// * @file lmp1616x1_reg.h
// * @brief LMP1616X1 芯片寄存器定义头文件
// * @version 1.0
// * @date 2026-04-07
// *
// * 基于LMP1616X1寄存器定义，适配15电极版本
// * 寄存器地址范围: 0x00-0x80
// */
//
//#ifndef __LMP1616X1_REG_H__
//#define __LMP1616X1_REG_H__
//
//#include <stdint.h>
//
///*==============================================================================
// * 刺激波形时序寄存器 (0x00-0x09)
// *============================================================================*/
//
///**
// * @brief Tphase1寄存器 (地址0x00)
// */
//#define LMP1616X1_REG_00_TPHASE1_0           (0x00)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase1 : 12;      /**< [11:0] 正脉冲时间，数值表示正脉冲多少个时钟周期 */
//        uint16_t reserved0 : 1;     /**< [12] reserved */
//        uint16_t sel_rvt : 2;       /**< [14:13] SEL_RVT[4:3]控制端，默认为0 */
//        uint16_t reserved1 : 1;     /**< [15] reserved */
//    } bits;
//} lmp1616x1_tphase1_0_t;
//
///**
// * @brief Tphase2寄存器 (地址0x01)
// */
//#define LMP1616X1_REG_01_TPHASE2_0           (0x01)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase2 : 12;      /**< [11:0] 负脉冲时间，数值表示负脉冲多少个时钟周期 */
//        uint16_t reserved : 1;      /**< [12] reserved */
//        uint16_t demod_triml : 3;   /**< [15:13] DEMOD_TRIML[2:0], 给解调器模块做修调用 */
//    } bits;
//} lmp1616x1_tphase2_0_t;
//
///**
// * @brief Tinterphase寄存器 (地址0x02)
// */
//#define LMP1616X1_REG_02_TINTERPHASE0       (0x02)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterphase1 : 8;   /**< [7:0] Tinterphase1：数值表示多少个时钟周期 */
//        uint16_t tinterphase2 : 8;   /**< [15:8] Tinterphase2：数值表示多少个时钟周期 */
//    } bits;
//} lmp1616x1_tinterphase0_t;
//
///**
// * @brief Tphase3寄存器 (地址0x03)
// */
//#define LMP1616X1_REG_03_TPHASE3_0           (0x03)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase3 : 12;      /**< [11:0] 电荷平衡阶段多少个时钟周期 */
//        uint16_t reserved : 4;      /**< [15:12] reserved */
//    } bits;
//} lmp1616x1_tphase3_0_t;
//
///**
// * @brief Tinterpulse寄存器 (地址0x04)
// */
//#define LMP1616X1_REG_04_TINTERPULSE0       (0x04)
//typedef union {
//    uint16_t value;  /**< [15:0] Tphase3与下一周期之间的延迟多少个时钟周期 */
//} lmp1616x1_tinterpulse0_t;
//
///**
// * @brief Tdefine寄存器 (地址0x05)
// */
//#define LMP1616X1_REG_05_TDEFINE           (0x05)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_td1 : 8;       /**< [7:0] 刺激电路开启与开始刺激之间的延迟时间, 典型值128us，固定为128clk */
//        uint16_t stim_td2 : 2;       /**< [9:8] reserved, stim_td2修改为固定的1 clock cycle的延时 */
//        uint16_t stim_td3 : 6;       /**< [15:10] 电荷平衡结束与STIM_EN\STIM_ON\DAC_ENI关闭之间的延迟时间, 典型值32us */
//    } bits;
//} lmp1616x1_tdefine_t;
//
///**
// * @brief STIM_MODE寄存器 (地址0x06)
// */
//#define LMP1616X1_REG_06_STIM_MODE         (0x06)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_start : 1;      /**< [0] 给刺激波形模块的开始刺激信号 */
//        uint16_t para_vld : 1;        /**< [1] 给刺激波形模块的配置参数有效信号，更新参数使用 */
//        uint16_t stim_stop : 1;       /**< [2] 停止刺激：1停止刺激，0不停止 */
//        uint16_t stim_mode : 2;       /**< [4:3] 刺激模式：00简单模式，01任意模式，10突发刺激模式 */
//        uint16_t stim_low_power : 1;  /**< [5] stim_low_power */
//        uint16_t program_cnt : 2;     /**< [7:6] 刺激程序数量：0一个刺激程序，1两个刺激程序 */
//        uint16_t stim_clk_div_value : 6; /**< [13:8] 刺激时钟分频：0不分频，n：n+1分频，最大62 */
//        uint16_t stim_auto_stop : 1;  /**< [14] 刺激自动停止开关，0关闭，1开启 */
//        uint16_t reserved : 1;        /**< [15] reserved */
//    } bits;
//} lmp1616x1_stim_mode_t;
//
///**
// * @brief burst_cnt/PWM_SETTING寄存器 (地址0x07)
// */
//#define LMP1616X1_REG_07_PWM_SETTING       (0x07)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t pwm_power_dly : 13;  /**< [12:0] 占空比供能时，检测到供能之后多少个RC时钟周期后再进入正向刺激 */
//        uint16_t pwm_mode : 1;        /**< [13] 1表示占空比供能模式，0常供能模式 */
//        uint16_t reserved : 2;        /**< [15:14] reserved */
//    } bits;
//} lmp1616x1_pwm_setting_t;
//
///**
// * @brief TINTERBURST寄存器 (地址0x08)
// */
//#define LMP1616X1_REG_08_TINTERBURST       (0x08)
//typedef union {
//    uint16_t value;  /**< [15:0] 突发模式下面刺激簇之间的间隔时间 */
//} lmp1616x1_tinterburst_t;
//
///**
// * @brief SOFTSTART_CYCLE寄存器 (地址0x09)
// */
//#define LMP1616X1_REG_09_SOFTSTART_CYCLE   (0x09)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t softstart_finish_cycle : 10;  /**< [9:0] 表示有多少个增加softstart_inc_value的cycle */
//        uint16_t reserved : 6;                 /**< [15:10] reserved */
//    } bits;
//} lmp1616x1_softstart_cycle_t;
//
///*==============================================================================
// * 自动停止周期寄存器 (0x0A)
// *============================================================================*/
//
///**
// * @brief STIM_AUTO_STOP_CYCLE寄存器 (地址0x0A)
// */
//#define LMP1616X1_REG_0A_STIM_AUTO_STOP_CYCLE (0x0A)
//typedef union {
//    uint16_t value;  /**< [15:0] 每位数值表示的是4个刺激周期 */
//} lmp1616x1_stim_auto_stop_cycle_t;
//
///*==============================================================================
// * 软启动控制寄存器 (0x0B)
// *============================================================================*/
//
///**
// * @brief SOFTSTART_CTRL寄存器 (地址0x0B)
// */
//#define LMP1616X1_REG_0B_SOFTSTART_CTRL    (0x0B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t softstart_step : 10;      /**< [9:0] 有效值1~1023，表示经过多少个刺激周期后触发一次softstart的increase value */
//        uint16_t softstart_inc_value : 5;  /**< [14:10] 有效值1~31, 表示每次增加的DAC value */
//        uint16_t softstart_en : 1;         /**< [15] 所有程序软启动使能，应用于所有程序组 */
//    } bits;
//} lmp1616x1_softstart_ctrl_t;
//
///*==============================================================================
// * ADC/RFID配置寄存器 (0x0C-0x0F)
// *============================================================================*/
//
///**
// * @brief ADC_DIFF_CFG寄存器 (地址0x0C)
// */
//#define LMP1616X1_REG_0C_ADC_DIFF_CFG      (0x0C)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t rfid_resp_delay_cnt : 11; /**< [10:0] 控制芯片收到demod解码后多久返回mod给程控仪 */
//        uint16_t sel_demod : 1;            /**< [11] 0选择原始的射频解调信号，1选择数字滤波后的射频解调信号 */
//        uint16_t pdelt : 4;                /**< [15:12] 设置数字滤波器波宽度，正常为2,3 */
//    } bits;
//} lmp1616x1_adc_diff_cfg_t;
//
///**
// * @brief RFID_CFG1寄存器 (地址0x0D)
// */
//#define LMP1616X1_REG_0D_RFID_CFG1         (0x0D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tari_min : 8;  /**< [7:0] TariMin */
//        uint16_t tari_max : 8;  /**< [15:8] TariMax */
//    } bits;
//} lmp1616x1_rfid_cfg1_t;
//
///**
// * @brief RFID_CFG2寄存器 (地址0x0E)
// */
//#define LMP1616X1_REG_0E_RFID_CFG2         (0x0E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t delta : 8;    /**< [7:0] Delta */
//        uint16_t rt_max : 8;   /**< [15:8] RtMax */
//    } bits;
//} lmp1616x1_rfid_cfg2_t;
//
///**
// * @brief RFID_CFG3寄存器 (地址0x0F)
// */
//#define LMP1616X1_REG_0F_RFID_CFG3         (0x0F)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t demod_pon_cnt : 9;      /**< [8:0] 占空比功能时demod高电平多久认定为供能，以区别通讯时的Rtcal高电平时间 */
//        uint16_t reserved : 2;           /**< [10:9] reserved */
//        uint16_t rfid_tx_data_period : 5; /**< [15:11] 默认值是0xA，只能是偶数，最大值0x1E(30) */
//    } bits;
//} lmp1616x1_rfid_cfg3_t;
//
///*==============================================================================
// * eFuse相关寄存器 (0x10-0x14)
// *============================================================================*/
//
///**
// * @brief EFUSE_W_DATA0寄存器 (地址0x10)
// */
//#define LMP1616X1_REG_10_EFUSE_W_DATA0    (0x10)
//typedef union {
//    uint16_t value;  /**< [15:0] efuse_w_data[15:0] */
//} lmp1616x1_efuse_w_data0_t;
//
///**
// * @brief EFUSE_W_DATA1寄存器 (地址0x11)
// */
//#define LMP1616X1_REG_11_EFUSE_W_DATA1    (0x11)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_w_data_high : 8;    /**< [7:0] efuse_w_data[23:16] */
//        uint16_t efuse_data_enable : 6;    /**< [13:8] efuse_data_enable[5:0]，每bit为1表示databus的4bit数据有效 */
//        uint16_t reversed : 1;             /**< [14] reversed */
//        uint16_t efuse_data_valid : 1;     /**< [15] 0->1变化表示触发一次efuse写操作 */
//    } bits;
//} lmp1616x1_efuse_w_data1_t;
//
///**
// * @brief EFUSE_R_DATA0寄存器 (地址0x12)
// */
//#define LMP1616X1_REG_12_EFUSE_R_DATA0    (0x12)
//typedef union {
//    uint16_t value;  /**< [15:0] efuse_r_data[15:0] */
//} lmp1616x1_efuse_r_data0_t;
//
///**
// * @brief EFUSE_R_DATA1寄存器 (地址0x13)
// */
//#define LMP1616X1_REG_13_EFUSE_R_DATA1    (0x13)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_r_data_high : 8;    /**< [7:0] efuse_r_data[23:16] */
//        uint16_t reserved : 6;             /**< [14:8] reversed */
//        uint16_t efuse_auto_read_done : 1; /**< [15] 1表示上电后自动读efuse执行完毕，0表示未执行完毕 */
//    } bits;
//} lmp1616x1_efuse_r_data1_t;
//
///**
// * @brief EFUSE_TBURN寄存器 (地址0x14)
// */
//#define LMP1616X1_REG_14_EFUSE_TBURN      (0x14)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_tburn : 15;      /**< [14:0] 1bit的烧写时间，单位是us */
//        uint16_t efuse_burn_clk_en : 1; /**< [15] 寄存器控制efuse gating时钟，为1时gating时钟常开 */
//    } bits;
//} lmp1616x1_efuse_tburn_t;
//
///*==============================================================================
// * 程序0正向刺激DAC值寄存器 (0x15-0x1E)
// *============================================================================*/
//
///**
// * @brief 程序0电极01正向刺激DAC值 (地址0x15)
// */
//#define LMP1616X1_REG_15_PROG0_ELEC01_DAC_P   (0x15)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_0 : 10; /**< [9:0] amplitude_dac_p_0[9:0] */
//        uint16_t amplitude_dac_p_1 : 6;  /**< [15:10] amplitude_dac_p_1[5:0] */
//    } bits;
//} lmp1616x1_prog0_elec01_dac_p_t;
//
///**
// * @brief 程序0电极123正向刺激DAC值 (地址0x16)
// */
//#define LMP1616X1_REG_16_PROG0_ELEC123_DAC_P  (0x16)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_1_high : 4; /**< [3:0] amplitude_dac_p_1[9:6] */
//        uint16_t amplitude_dac_p_2 : 10;     /**< [13:4] amplitude_dac_p_2[9:0] */
//        uint16_t amplitude_dac_p_3 : 2;      /**< [15:14] amplitude_dac_p_3[1:0] */
//    } bits;
//} lmp1616x1_prog0_elec123_dac_p_t;
//
///**
// * @brief 程序0电极34正向刺激DAC值 (地址0x17)
// */
//#define LMP1616X1_REG_17_PROG0_ELEC34_DAC_P   (0x17)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_3_high : 6; /**< [5:0] amplitude_dac_p_3[7:2] */
//        uint16_t amplitude_dac_p_3_low : 2;  /**< [7:6] 实际与bits[5:0]组成8位，此处与描述对齐 */
//        uint16_t amplitude_dac_p_4 : 8;      /**< [15:8] amplitude_dac_p_4[7:0] */
//    } bits;
//} lmp1616x1_prog0_elec34_dac_p_t;
//
///**
// * @brief 程序0电极456正向刺激DAC值 (地址0x18)
// */
//#define LMP1616X1_REG_18_PROG0_ELEC456_DAC_P  (0x18)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_4_high : 2; /**< [1:0] amplitude_dac_p_4[9:8] */
//        uint16_t amplitude_dac_p_5 : 10;     /**< [11:2] amplitude_dac_p_5[9:0] */
//        uint16_t amplitude_dac_p_6 : 4;      /**< [15:12] amplitude_dac_p_6[3:0] */
//    } bits;
//} lmp1616x1_prog0_elec456_dac_p_t;
//
///**
// * @brief 程序0电极67正向刺激DAC值 (地址0x19)
// */
//#define LMP1616X1_REG_19_PROG0_ELEC67_DAC_P   (0x19)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_6_high : 4; /**< [3:0] amplitude_dac_p_6[7:4] */
//        uint16_t amplitude_dac_p_6_low : 2;  /**< [5:4] 实际与bits[3:0]组成6位 */
//        uint16_t amplitude_dac_p_7 : 10;     /**< [15:6] amplitude_dac_p_7[9:0] */
//    } bits;
//} lmp1616x1_prog0_elec67_dac_p_t;
//
///**
// * @brief 程序0电极89正向刺激DAC值 (地址0x1A)
// */
//#define LMP1616X1_REG_1A_PROG0_ELEC89_DAC_P   (0x1A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_8 : 10; /**< [9:0] amplitude_dac_p_8[9:0] */
//        uint16_t amplitude_dac_p_9 : 6;  /**< [15:10] amplitude_dac_p_9[5:0] */
//    } bits;
//} lmp1616x1_prog0_elec89_dac_p_t;
//
///**
// * @brief 程序0电极9 10 11正向刺激DAC值 (地址0x1B)
// */
//#define LMP1616X1_REG_1B_PROG0_ELEC9_10_11_DAC_P (0x1B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_9_high : 4; /**< [3:0] amplitude_dac_p_9[9:6] */
//        uint16_t amplitude_dac_p_10 : 10;    /**< [13:4] amplitude_dac_p_10[9:0] */
//        uint16_t amplitude_dac_p_11 : 2;     /**< [15:14] amplitude_dac_p_11[1:0] */
//    } bits;
//} lmp1616x1_prog0_elec9_10_11_dac_p_t;
//
///**
// * @brief 程序0电极11 12正向刺激DAC值 (地址0x1C)
// */
//#define LMP1616X1_REG_1C_PROG0_ELEC11_12_DAC_P (0x1C)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_11_high : 6; /**< [5:0] amplitude_dac_p_11[7:2] */
//        uint16_t amplitude_dac_p_11_low : 2;  /**< [7:6] 实际与bits[5:0]组成8位 */
//        uint16_t amplitude_dac_p_12 : 8;      /**< [15:8] amplitude_dac_p_12[7:0] */
//    } bits;
//} lmp1616x1_prog0_elec11_12_dac_p_t;
//
///**
// * @brief 程序0电极12 13 14正向刺激DAC值 (地址0x1D)
// */
//#define LMP1616X1_REG_1D_PROG0_ELEC12_13_14_DAC_P (0x1D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_12_high : 2; /**< [1:0] amplitude_dac_p_12[9:8] */
//        uint16_t amplitude_dac_p_13 : 10;     /**< [11:2] amplitude_dac_p_13[9:0] */
//        uint16_t amplitude_dac_p_14 : 4;      /**< [15:12] amplitude_dac_p_14[3:0] */
//    } bits;
//} lmp1616x1_prog0_elec12_13_14_dac_p_t;
//
///**
// * @brief 程序0电极14 15正向刺激DAC值 (地址0x1E)
// */
//#define LMP1616X1_REG_1E_PROG0_ELEC14_15_DAC_P (0x1E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_14_high : 4; /**< [3:0] amplitude_dac_p_14[7:4] */
//        uint16_t amplitude_dac_p_14_low : 2;  /**< [5:4] 实际与bits[3:0]组成6位 */
//        uint16_t amplitude_dac_p_15 : 10;     /**< [15:6] amplitude_dac_p_15[9:0] */
//    } bits;
//} lmp1616x1_prog0_elec14_15_dac_p_t;
//
///*==============================================================================
// * 程序1正向刺激DAC值寄存器 (0x1F-0x28)
// *============================================================================*/
//
//#define LMP1616X1_REG_1F_PROG1_ELEC01_DAC_P   (0x1F)
//#define LMP1616X1_REG_20_PROG1_ELEC123_DAC_P  (0x20)
//#define LMP1616X1_REG_21_PROG1_ELEC34_DAC_P   (0x21)
//#define LMP1616X1_REG_22_PROG1_ELEC456_DAC_P  (0x22)
//#define LMP1616X1_REG_23_PROG1_ELEC67_DAC_P   (0x23)
//#define LMP1616X1_REG_24_PROG1_ELEC89_DAC_P   (0x24)
//#define LMP1616X1_REG_25_PROG1_ELEC9_10_11_DAC_P (0x25)
//#define LMP1616X1_REG_26_PROG1_ELEC11_12_DAC_P (0x26)
//#define LMP1616X1_REG_27_PROG1_ELEC12_13_14_DAC_P (0x27)
//#define LMP1616X1_REG_28_PROG1_ELEC14_15_DAC_P (0x28)
//
///*==============================================================================
// * 程序2正向刺激DAC值寄存器 (0x29-0x32)
// *============================================================================*/
//
//#define LMP1616X1_REG_29_PROG2_ELEC01_DAC_P   (0x29)
//#define LMP1616X1_REG_2A_PROG2_ELEC123_DAC_P  (0x2A)
//#define LMP1616X1_REG_2B_PROG2_ELEC34_DAC_P   (0x2B)
//#define LMP1616X1_REG_2C_PROG2_ELEC456_DAC_P  (0x2C)
//#define LMP1616X1_REG_2D_PROG2_ELEC67_DAC_P   (0x2D)
//#define LMP1616X1_REG_2E_PROG2_ELEC89_DAC_P   (0x2E)
//#define LMP1616X1_REG_2F_PROG2_ELEC9_10_11_DAC_P (0x2F)
//#define LMP1616X1_REG_30_PROG2_ELEC11_12_DAC_P (0x30)
//#define LMP1616X1_REG_31_PROG2_ELEC12_13_14_DAC_P (0x31)
//#define LMP1616X1_REG_32_PROG2_ELEC14_15_DAC_P (0x32)
//
///*==============================================================================
// * 程序3正向刺激DAC值寄存器 (0x33-0x3C)
// *============================================================================*/
//
//#define LMP1616X1_REG_33_PROG3_ELEC01_DAC_P   (0x33)
//#define LMP1616X1_REG_34_PROG3_ELEC123_DAC_P  (0x34)
//#define LMP1616X1_REG_35_PROG3_ELEC34_DAC_P   (0x35)
//#define LMP1616X1_REG_36_PROG3_ELEC456_DAC_P  (0x36)
//#define LMP1616X1_REG_37_PROG3_ELEC67_DAC_P   (0x37)
//#define LMP1616X1_REG_38_PROG3_ELEC89_DAC_P   (0x38)
//#define LMP1616X1_REG_39_PROG3_ELEC9_10_11_DAC_P (0x39)
//#define LMP1616X1_REG_3A_PROG3_ELEC11_12_DAC_P (0x3A)
//#define LMP1616X1_REG_3B_PROG3_ELEC12_13_14_DAC_P (0x3B)
//#define LMP1616X1_REG_3C_PROG3_ELEC14_15_DAC_P (0x3C)
//
///*==============================================================================
// * 电极开关控制寄存器 (0x3D-0x44)
// *============================================================================*/
//
///**
// * @brief 程序0正向刺激时阳极开关 (地址0x3D)
// */
//#define LMP1616X1_REG_3D_PROG0_ANSEL_P     (0x3D)
//typedef union {
//    uint16_t value;  /**< [15:0] DANSEL_p[15:0] 程序0正向刺激时阳极开关 */
//} lmp1616x1_prog0_ansel_p_t;
//
///**
// * @brief 程序0正向刺激时阴极开关 (地址0x3E)
// */
//#define LMP1616X1_REG_3E_PROG0_CASEL_P     (0x3E)
//typedef union {
//    uint16_t value;  /**< [15:0] DCASEL_p[15:0] 程序0正向刺激时阴极开关 */
//} lmp1616x1_prog0_casel_p_t;
//
///**
// * @brief 程序1正向刺激时阳极开关 (地址0x3F)
// */
//#define LMP1616X1_REG_3F_PROG1_ANSEL_N     (0x3F)
//typedef union {
//    uint16_t value;  /**< [15:0] DANSEL_n[15:0] 程序1正向刺激时阳极开关 */
//} lmp1616x1_prog1_ansel_n_t;
//
///**
// * @brief 程序1正向刺激时阴极开关 (地址0x40)
// */
//#define LMP1616X1_REG_40_PROG1_CASEL_N     (0x40)
//typedef union {
//    uint16_t value;  /**< [15:0] DCASEL_n[15:0] 程序1正向刺激时阴极开关 */
//} lmp1616x1_prog1_casel_n_t;
//
///**
// * @brief 程序2正向刺激时阳极开关 (地址0x41)
// */
//#define LMP1616X1_REG_41_PROG2_ANSEL_P     (0x41)
//typedef union {
//    uint16_t value;  /**< [15:0] DANSEL_p[15:0] 程序2正向刺激时阳极开关 */
//} lmp1616x1_prog2_ansel_p_t;
//
///**
// * @brief 程序2正向刺激时阴极开关 (地址0x42)
// */
//#define LMP1616X1_REG_42_PROG2_CASEL_P     (0x42)
//typedef union {
//    uint16_t value;  /**< [15:0] DCASEL_p[15:0] 程序2正向刺激时阴极开关 */
//} lmp1616x1_prog2_casel_p_t;
//
///**
// * @brief 程序3正向刺激时阳极开关 (地址0x43)
// */
//#define LMP1616X1_REG_43_PROG3_ANSEL_N     (0x43)
//typedef union {
//    uint16_t value;  /**< [15:0] DANSEL_n[15:0] 程序3正向刺激时阳极开关 */
//} lmp1616x1_prog3_ansel_n_t;
//
///**
// * @brief 程序3正向刺激时阴极开关 (地址0x44)
// */
//#define LMP1616X1_REG_44_PROG3_CASEL_N     (0x44)
//typedef union {
//    uint16_t value;  /**< [15:0] DCASEL_n[15:0] 程序3正向刺激时阴极开关 */
//} lmp1616x1_prog3_casel_n_t;
//
///*==============================================================================
// * IDAC设置寄存器 (0x45-0x46)
// *============================================================================*/
//
///**
// * @brief IDAC_SET0寄存器 (地址0x45)
// */
//#define LMP1616X1_REG_45_IDAC_SET0         (0x45)
//typedef union {
//    uint16_t value;  /**< [15:0] 给模拟的档位信号低16位 */
//} lmp1616x1_idac_set0_t;
//
///**
// * @brief IDAC_SET1寄存器 (地址0x46)
// */
//#define LMP1616X1_REG_46_IDAC_SET1         (0x46)
//typedef union {
//    uint16_t value;  /**< [15:0] 给模拟的档位信号高16位 */
//} lmp1616x1_idac_set1_t;
//
///*==============================================================================
// * 程序1时序寄存器 (0x47-0x4B)
// *============================================================================*/
//
///**
// * @brief 程序1的Tphase1寄存器 (地址0x47)
// */
//#define LMP1616X1_REG_47_PROG1_TPHASE1     (0x47)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase1_1 : 12;   /**< [11:0] 程序1的正脉冲多少个时钟周期 */
//        uint16_t offset_trim : 4;   /**< [15:12] offset_trim[3:0] 给模拟用的trim信号，默认只有trim[1]为1 */
//    } bits;
//} lmp1616x1_prog1_tphase1_t;
//
///**
// * @brief 程序1的Tphase2寄存器 (地址0x48)
// */
//#define LMP1616X1_REG_48_PROG1_TPHASE2     (0x48)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase2_1 : 12;   /**< [11:0] 程序1的负脉冲多少个时钟周期 */
//        uint16_t offset_trim : 4;   /**< [15:12] offset_trim[7:4] */
//    } bits;
//} lmp1616x1_prog1_tphase2_t;
//
///**
// * @brief 程序1的Tphase3寄存器 (地址0x49)
// */
//#define LMP1616X1_REG_49_PROG1_TPHASE3     (0x49)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase3_1 : 12;   /**< [11:0] 程序1的电荷平衡阶段多少个时钟周期 */
//        uint16_t reversed : 4;     /**< [15:12] reversed */
//    } bits;
//} lmp1616x1_prog1_tphase3_t;
//
///**
// * @brief 程序1的Tinterphase寄存器 (地址0x4A)
// */
//#define LMP1616X1_REG_4A_PROG1_TINTERPHASE (0x4A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterphase1_1 : 8;   /**< [7:0] Tinterphase1_1：数值表示多少个时钟周期 */
//        uint16_t tinterphase2_1 : 8;   /**< [15:8] Tinterphase2_1：数值表示多少个时钟周期 */
//    } bits;
//} lmp1616x1_prog1_tinterphase_t;
//
///**
// * @brief 程序1的Tinterpulse寄存器 (地址0x4B)
// */
//#define LMP1616X1_REG_4B_PROG1_TINTERPULSE (0x4B)
//typedef union {
//    uint16_t value;  /**< [15:0] Tphase3与下一周期之间的延迟多少个时钟周期 */
//} lmp1616x1_prog1_tinterpulse_t;
//
///*==============================================================================
// * 程序2时序寄存器 (0x4C-0x50)
// *============================================================================*/
//
//#define LMP1616X1_REG_4C_PROG2_TPHASE1     (0x4C)
//#define LMP1616X1_REG_4D_PROG2_TPHASE2     (0x4D)
//#define LMP1616X1_REG_4E_PROG2_TPHASE3     (0x4E)
//#define LMP1616X1_REG_4F_PROG2_TINTERPHASE (0x4F)
//#define LMP1616X1_REG_50_PROG2_TINTERPULSE (0x50)
//
///*==============================================================================
// * 程序3时序寄存器 (0x51-0x55)
// *============================================================================*/
//
//#define LMP1616X1_REG_51_PROG3_TPHASE1     (0x51)
//#define LMP1616X1_REG_52_PROG3_TPHASE2     (0x52)
//#define LMP1616X1_REG_53_PROG3_TPHASE3     (0x53)
//#define LMP1616X1_REG_54_PROG3_TINTERPHASE (0x54)
//#define LMP1616X1_REG_55_PROG3_TINTERPULSE (0x55)
//
///*==============================================================================
// * 软启动增量DAC值寄存器 (0x56-0x59)
// *============================================================================*/
//
//#define LMP1616X1_REG_56_SOFTSTART_INC_DAC_0 (0x56)
//#define LMP1616X1_REG_57_SOFTSTART_INC_DAC_1 (0x57)
//#define LMP1616X1_REG_58_SOFTSTART_INC_DAC_2 (0x58)
//#define LMP1616X1_REG_59_SOFTSTART_INC_DAC_3 (0x59)
//
///*==============================================================================
// * 修调数据和控制寄存器 (0x60-0x67)
// *============================================================================*/
//
///**
// * @brief TRIMDATA_CTRL0寄存器 (地址0x60)
// */
//#define LMP1616X1_REG_60_TRIMDATA_CTRL0   (0x60)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t trim_ref : 1;           /**< [0] 1：电压恒正常，0：电压受模拟电路控制 */
//        uint16_t trim_duvlo : 1;         /**< [1] 0: Vrecp低于标定电压停止刺激，1：Vrecp低于标定电压复位芯片 */
//        uint16_t cmd_rst : 1;            /**< [2] 数字电路寄存器软复位信号，写1，写0完成复位 */
//        uint16_t test_mode : 1;          /**< [3] 设置为1时为测试模式，0为正常工作模式 */
//        uint16_t vrecp_trim : 4;         /**< [7:4] 设定和Vrecp的比较电压，来生成DUVLO信号给数字 */
//        uint16_t trim_adc : 8;           /**< [15:8] trim_adc -> {ISEL_ADC[1:0], ISEL_DAC[1:0], ISEL_OPA, IMP_GAIN[1:0], PDHO_SEL} */
//    } bits;
//} lmp1616x1_trimdata_ctrl0_t;
//
///**
// * @brief DIGITAL_VERSION寄存器 (地址0x61)
// */
//#define LMP1616X1_REG_61_DIGITAL_VERSION  (0x61)
//typedef union {
//    uint16_t value;  /**< [15:0] 数字版本号 */
//} lmp1616x1_digital_version_t;
//
///**
// * @brief STIM_PARA_CRC寄存器 (地址0x62)
// */
//#define LMP1616X1_REG_62_STIM_PARA_CRC    (0x62)
//typedef union {
//    uint16_t value;  /**< [15:0] 刺激参数CRC值 */
//} lmp1616x1_stim_para_crc_t;
//
///**
// * @brief TRIMDATA_CTRL3寄存器 (地址0x63)
// */
//#define LMP1616X1_REG_63_TRIMDATA_CTRL3   (0x63)
//typedef union {
//    uint16_t value;  /**< [15:0] reserved */
//} lmp1616x1_trimdata_ctrl3_t;
//
///**
// * @brief LDO_TRIM寄存器 (地址0x64)
// */
//#define LMP1616X1_REG_64_LDO_TRIM         (0x64)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t trim_osc : 4;            /**< [3:0] TRIM_OSC */
//        uint16_t temp_trim_ptati : 2;     /**< [5:4] TEMP_TRIM_PTATI */
//        uint16_t temp_trim_ibi : 1;       /**< [6] TEMP_TRIM_IBI */
//        uint16_t temp_trim_ctati : 1;     /**< [7] TEMP_TRIM_CTATI */
//        uint16_t limit_en : 1;            /**< [8] LIMIT_EN */
//        uint16_t stim_si : 2;             /**< [10:9] STIM_SI */
//        uint16_t adc_trim_osc : 4;        /**< [14:11] ADC_TRIM_OSC */
//        uint16_t adc_testmode : 1;        /**< [15] ADC_TESTMODE，此项为数字模块输出 */
//    } bits;
//} lmp1616x1_ldo_trim_t;
//
///**
// * @brief VOLT_DELAY_SAMPLE寄存器 (地址0x65)
// */
//#define LMP1616X1_REG_65_VOLT_DELAY_SAMPLE (0x65)
//typedef union {
//    uint16_t value;  /**< [15:0] 值代表延迟多少个刺激周期才开启电压测量，0表示无延时，最大值65535 */
//} lmp1616x1_volt_delay_sample_t;
//
///* 地址0x66-0x67 reserved */
//#define LMP1616X1_REG_66_RESERVED         (0x66)
//#define LMP1616X1_REG_67_RESERVED         (0x67)
//
///*==============================================================================
// * 控制寄存器 (0x68-0x6B)
// *============================================================================*/
//
///**
// * @brief CTRLREG0寄存器 (地址0x68)
// */
//#define LMP1616X1_REG_68_CTRLREG0         (0x68)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t start_en : 3;               /**< [2:0] [0]温度测量开关，[1]电压测量开关，[2]阻抗测量开关 */
//        uint16_t cmd_rst : 1;                /**< [3] 数字电路寄存器软复位信号，写1，写0完成复位 */
//        uint16_t test_mode : 1;              /**< [4] 设置为1时为测试模式，0为正常工作模式 */
//        uint16_t program_sel : 2;            /**< [6:5] 延迟测量程序选择，0,1,2,3表示相应的刺激程序 */
//        uint16_t imp_channel_sel : 4;        /**< [10:7] 选择测量那个通道的阻抗 */
//        uint16_t imp_test_trim_osc : 4;      /**< [14:11] 阻抗测量时使用的时钟trim_osc */
//        uint16_t current_measure : 1;        /**< [15] 16电流源电流测量功能不使能 */
//    } bits;
//} lmp1616x1_ctrlreg0_t;
//
///**
// * @brief CTRLREG1寄存器 (地址0x69)
// */
//#define LMP1616X1_REG_69_CTRLREG1         (0x69)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_done : 1;          /**< [0] 刺激完成标志 */
//        uint16_t temp_done : 1;          /**< [1] 温度测量完成 */
//        uint16_t vdd_sense_done : 1;     /**< [2] 电压测量完成 */
//        uint16_t imp_done : 1;           /**< [3] 阻抗测量完成 */
//        uint16_t duvlo : 1;              /**< [4] 0欠压(电压小于5V)，1正常 */
//        uint16_t reserved : 11;          /**< [15:5] reserved */
//    } bits;
//} lmp1616x1_ctrlreg1_t;
//
///**
// * @brief 阻抗数据寄存器1 (地址0x6A)
// */
//#define LMP1616X1_REG_6A_IMPEDANCE_DATA1  (0x6A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t impedance_data1 : 12;   /**< [11:0] impedance_data1[11:0] */
//        uint16_t reserved : 4;           /**< [15:12] reserved */
//    } bits;
//} lmp1616x1_impedance_data1_t;
//
///**
// * @brief 阻抗数据寄存器2 (地址0x6B)
// */
//#define LMP1616X1_REG_6B_IMPEDANCE_DATA2  (0x6B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t impedance_data2 : 12;   /**< [11:0] impedance_data2[11:0] */
//        uint16_t reserved : 4;           /**< [15:12] reserved */
//    } bits;
//} lmp1616x1_impedance_data2_t;
//
///*==============================================================================
// * 阻抗数据寄存器 (0x6C-0x7B)
// *============================================================================*/
//#define LMP1616X1_REG_6C_IMPEDANCE_DATA3  (0x6C)
//#define LMP1616X1_REG_6D_IMPEDANCE_DATA4  (0x6D)
//#define LMP1616X1_REG_6E_IMPEDANCE_DATA5  (0x6E)
//#define LMP1616X1_REG_6F_IMPEDANCE_DATA6  (0x6F)
//#define LMP1616X1_REG_70_IMPEDANCE_DATA7  (0x70)
//#define LMP1616X1_REG_71_IMPEDANCE_DATA8  (0x71)
//#define LMP1616X1_REG_72_VDD_LOW          (0x72)
//#define LMP1616X1_REG_73_VDD_HIGH         (0x73)
//#define LMP1616X1_REG_74_TEMP_DATA        (0x74)
//#define LMP1616X1_REG_75_IMPEDANCE_DATA9  (0x75)
//#define LMP1616X1_REG_76_IMPEDANCE_DATA10 (0x76)
//#define LMP1616X1_REG_77_IMPEDANCE_DATA11 (0x77)
//#define LMP1616X1_REG_78_IMPEDANCE_DATA12 (0x78)
//#define LMP1616X1_REG_79_IMPEDANCE_DATA13 (0x79)
//#define LMP1616X1_REG_7A_IMPEDANCE_DATA14 (0x7A)
//#define LMP1616X1_REG_7B_IMPEDANCE_DATA15 (0x7B)
//#define LMP1616X1_REG_7C_IMPEDANCE_DATA16 (0x7C)
//
///*==============================================================================
// * 电流测量数据寄存器 (0x7D-0x7E)
// *============================================================================*/
//
///**
// * @brief CURRENT_DATA_NEG寄存器 (地址0x7D)
// * @details 负向刺激电流中间脉宽时刻的数值
// */
//#define LMP1616X1_REG_7D_CURRENT_DATA_NEG (0x7D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t current_data_neg : 12;  /**< [11:0] 负向刺激电流中间脉宽时刻的数值 */
//        uint16_t reserved : 4;           /**< [15:12] reserved */
//    } bits;
//} lmp1616x1_current_data_neg_t;
//
///**
// * @brief CURRENT_DATA_PLUS寄存器 (地址0x7E)
// * @details 正向刺激电流中间脉宽时刻的数值
// */
//#define LMP1616X1_REG_7E_CURRENT_DATA_PLUS (0x7E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t current_data_plus : 12;  /**< [11:0] 正向刺激电流中间脉宽时刻的数值 */
//        uint16_t reserved : 4;            /**< [15:12] reserved */
//    } bits;
//} lmp1616x1_current_data_plus_t;
//
///*==============================================================================
// * 负向刺激DAC值寄存器 (0x80-0xA7)
// *============================================================================*/
//
///*==============================================================================
// * 程序0正向刺激DAC值寄存器 (0x80-0x84)
// *============================================================================*/
//
///**
// * @brief 程序0电极01正向刺激DAC值 (地址0x80)
// */
//#define LMP1616X1_REG_80_PROG0_ELEC01_DAC_P   (0x80)
////typedef union {
////    uint16_t value;
////    struct {
////        uint16_t amplitude_dac_p_0 : 10;  /**< [9:0] amplitude_dac_p_0[9:0] */
////        uint16_t amplitude_dac_p_1 : 6;   /**< [15:10] amplitude_dac_p_1[5:0] */
////    } bits;
////} lmp1616x1_prog0_elec01_dac_p_t;
//
///**
// * @brief 程序0电极123正向刺激DAC值 (地址0x81)
// */
//#define LMP1616X1_REG_81_PROG0_ELEC123_DAC_P  (0x81)
////typedef union {
////    uint16_t value;
////    struct {
////        uint16_t amplitude_dac_p_1_high : 4;  /**< [3:0] amplitude_dac_p_1[9:6] */
////        uint16_t amplitude_dac_p_2 : 10;      /**< [13:4] amplitude_dac_p_2[9:0] */
////        uint16_t amplitude_dac_p_3 : 2;       /**< [15:14] amplitude_dac_p_3[1:0] */
////    } bits;
////} lmp1616x1_prog0_elec123_dac_p_t;
//
///**
// * @brief 程序0电极34正向刺激DAC值 (地址0x82)
// */
//#define LMP1616X1_REG_82_PROG0_ELEC34_DAC_P   (0x82)
////typedef union {
////    uint16_t value;
////    struct {
////        uint16_t amplitude_dac_p_3_low : 8;   /**< [7:0] amplitude_dac_p_3[9:2] */
////        uint16_t amplitude_dac_p_4 : 8;       /**< [15:8] amplitude_dac_p_4[7:0] */
////    } bits;
////} lmp1616x1_prog0_elec34_dac_p_t;
//
///**
// * @brief 程序0电极456正向刺激DAC值 (地址0x83)
// */
//#define LMP1616X1_REG_83_PROG0_ELEC456_DAC_P  (0x83)
////typedef union {
////    uint16_t value;
////    struct {
////        uint16_t amplitude_dac_p_4_high : 2;  /**< [1:0] amplitude_dac_p_4[9:8] */
////        uint16_t amplitude_dac_p_5 : 10;      /**< [11:2] amplitude_dac_p_5[9:0] */
////        uint16_t amplitude_dac_p_6 : 4;       /**< [15:12] amplitude_dac_p_6[3:0] */
////    } bits;
////} lmp1616x1_prog0_elec456_dac_p_t;
//
///**
// * @brief 程序0电极67正向刺激DAC值 (地址0x84)
// */
//#define LMP1616X1_REG_84_PROG0_ELEC67_DAC_P   (0x84)
////typedef union {
////    uint16_t value;
////    struct {
////        uint16_t amplitude_dac_p_6_high : 6;  /**< [5:0] amplitude_dac_p_6[9:4] */
////        uint16_t amplitude_dac_p_7 : 10;      /**< [15:6] amplitude_dac_p_7[9:0] */
////    } bits;
////} lmp1616x1_prog0_elec67_dac_p_t;
//
///*==============================================================================
// * 程序0负向刺激DAC值寄存器 (0x85-0x89)
// *============================================================================*/
//
///**
// * @brief 程序0电极01负向刺激DAC值 (地址0x85)
// */
//#define LMP1616X1_REG_85_PROG0_ELEC01_DAC_N   (0x85)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_0 : 10;  /**< [9:0] amplitude_dac_n_0[9:0] */
//        uint16_t amplitude_dac_n_1 : 6;   /**< [15:10] amplitude_dac_n_1[5:0] */
//    } bits;
//} lmp1616x1_prog0_elec01_dac_n_t;
//
///**
// * @brief 程序0电极123负向刺激DAC值 (地址0x86)
// */
//#define LMP1616X1_REG_86_PROG0_ELEC123_DAC_N  (0x86)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_1_high : 4;  /**< [3:0] amplitude_dac_n_1[9:6] */
//        uint16_t amplitude_dac_n_2 : 10;      /**< [13:4] amplitude_dac_n_2[9:0] */
//        uint16_t amplitude_dac_n_3 : 2;       /**< [15:14] amplitude_dac_n_3[1:0] */
//    } bits;
//} lmp1616x1_prog0_elec123_dac_n_t;
//
///**
// * @brief 程序0电极34负向刺激DAC值 (地址0x87)
// */
//#define LMP1616X1_REG_87_PROG0_ELEC34_DAC_N   (0x87)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_3_low : 8;   /**< [7:0] amplitude_dac_n_3[9:2] */
//        uint16_t amplitude_dac_n_4 : 8;       /**< [15:8] amplitude_dac_n_4[7:0] */
//    } bits;
//} lmp1616x1_prog0_elec34_dac_n_t;
//
///**
// * @brief 程序0电极456负向刺激DAC值 (地址0x88)
// */
//#define LMP1616X1_REG_88_PROG0_ELEC456_DAC_N  (0x88)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_4_high : 2;  /**< [1:0] amplitude_dac_n_4[9:8] */
//        uint16_t amplitude_dac_n_5 : 10;      /**< [11:2] amplitude_dac_n_5[9:0] */
//        uint16_t amplitude_dac_n_6 : 4;       /**< [15:12] amplitude_dac_n_6[3:0] */
//    } bits;
//} lmp1616x1_prog0_elec456_dac_n_t;
//
///**
// * @brief 程序0电极67负向刺激DAC值 (地址0x89)
// */
//#define LMP1616X1_REG_89_PROG0_ELEC67_DAC_N   (0x89)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_6_high : 6;  /**< [5:0] amplitude_dac_n_6[9:4] */
//        uint16_t amplitude_dac_n_7 : 10;      /**< [15:6] amplitude_dac_n_7[9:0] */
//    } bits;
//} lmp1616x1_prog0_elec67_dac_n_t;
//
///*==============================================================================
// * 程序1正向刺激DAC值寄存器 (0x8A-0x8E)
// *============================================================================*/
//
///**
// * @brief 程序1电极01正向刺激DAC值 (地址0x8A)
// */
//#define LMP1616X1_REG_8A_PROG1_ELEC01_DAC_P   (0x8A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_0 : 10;  /**< [9:0] amplitude_dac_p_0[9:0], 默认值0x00C */
//        uint16_t amplitude_dac_p_1 : 6;   /**< [15:10] amplitude_dac_p_1[5:0] */
//    } bits;
//} lmp1616x1_prog1_elec01_dac_p_t;
//
///**
// * @brief 程序1电极123正向刺激DAC值 (地址0x8B)
// */
//#define LMP1616X1_REG_8B_PROG1_ELEC123_DAC_P  (0x8B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_1_high : 4;  /**< [3:0] amplitude_dac_p_1[9:6] */
//        uint16_t amplitude_dac_p_2 : 10;      /**< [13:4] amplitude_dac_p_2[9:0] */
//        uint16_t amplitude_dac_p_3 : 2;       /**< [15:14] amplitude_dac_p_3[1:0] */
//    } bits;
//} lmp1616x1_prog1_elec123_dac_p_t;
//
///**
// * @brief 程序1电极34正向刺激DAC值 (地址0x8C)
// */
//#define LMP1616X1_REG_8C_PROG1_ELEC34_DAC_P   (0x8C)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_3_low : 8;   /**< [7:0] amplitude_dac_p_3[9:2] */
//        uint16_t amplitude_dac_p_4 : 8;       /**< [15:8] amplitude_dac_p_4[7:0] */
//    } bits;
//} lmp1616x1_prog1_elec34_dac_p_t;
//
///**
// * @brief 程序1电极456正向刺激DAC值 (地址0x8D)
// */
//#define LMP1616X1_REG_8D_PROG1_ELEC456_DAC_P  (0x8D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_4_high : 2;  /**< [1:0] amplitude_dac_p_4[9:8] */
//        uint16_t amplitude_dac_p_5 : 10;      /**< [11:2] amplitude_dac_p_5[9:0] */
//        uint16_t amplitude_dac_p_6 : 4;       /**< [15:12] amplitude_dac_p_6[3:0] */
//    } bits;
//} lmp1616x1_prog1_elec456_dac_p_t;
//
///**
// * @brief 程序1电极67正向刺激DAC值 (地址0x8E)
// */
//#define LMP1616X1_REG_8E_PROG1_ELEC67_DAC_P   (0x8E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_6_high : 6;  /**< [5:0] amplitude_dac_p_6[9:4] */
//        uint16_t amplitude_dac_p_7 : 10;      /**< [15:6] amplitude_dac_p_7[9:0] */
//    } bits;
//} lmp1616x1_prog1_elec67_dac_p_t;
//
///*==============================================================================
// * 程序1负向刺激DAC值寄存器 (0x8F-0x93)
// *============================================================================*/
//
///**
// * @brief 程序1电极01负向刺激DAC值 (地址0x8F)
// */
//#define LMP1616X1_REG_8F_PROG1_ELEC01_DAC_N   (0x8F)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_0 : 10;  /**< [9:0] amplitude_dac_n_0[9:0] */
//        uint16_t amplitude_dac_n_1 : 6;   /**< [15:10] amplitude_dac_n_1[5:0] */
//    } bits;
//} lmp1616x1_prog1_elec01_dac_n_t;
//
///**
// * @brief 程序1电极123负向刺激DAC值 (地址0x90)
// */
//#define LMP1616X1_REG_90_PROG1_ELEC123_DAC_N  (0x90)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_1_high : 4;  /**< [3:0] amplitude_dac_n_1[9:6] */
//        uint16_t amplitude_dac_n_2 : 10;      /**< [13:4] amplitude_dac_n_2[9:0] */
//        uint16_t amplitude_dac_n_3 : 2;       /**< [15:14] amplitude_dac_n_3[1:0] */
//    } bits;
//} lmp1616x1_prog1_elec123_dac_n_t;
//
///**
// * @brief 程序1电极34负向刺激DAC值 (地址0x91)
// */
//#define LMP1616X1_REG_91_PROG1_ELEC34_DAC_N   (0x91)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_3_low : 8;   /**< [7:0] amplitude_dac_n_3[9:2] */
//        uint16_t amplitude_dac_n_4 : 8;       /**< [15:8] amplitude_dac_n_4[7:0] */
//    } bits;
//} lmp1616x1_prog1_elec34_dac_n_t;
//
///**
// * @brief 程序1电极456负向刺激DAC值 (地址0x92)
// */
//#define LMP1616X1_REG_92_PROG1_ELEC456_DAC_N  (0x92)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_4_high : 2;  /**< [1:0] amplitude_dac_n_4[9:8] */
//        uint16_t amplitude_dac_n_5 : 10;      /**< [11:2] amplitude_dac_n_5[9:0] */
//        uint16_t amplitude_dac_n_6 : 4;       /**< [15:12] amplitude_dac_n_6[3:0] */
//    } bits;
//} lmp1616x1_prog1_elec456_dac_n_t;
//
///**
// * @brief 程序1电极67负向刺激DAC值 (地址0x93)
// */
//#define LMP1616X1_REG_93_PROG1_ELEC67_DAC_N   (0x93)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_6_high : 6;  /**< [5:0] amplitude_dac_n_6[9:4] */
//        uint16_t amplitude_dac_n_7 : 10;      /**< [15:6] amplitude_dac_n_7[9:0] */
//    } bits;
//} lmp1616x1_prog1_elec67_dac_n_t;
//
///*==============================================================================
// * 程序2正向刺激DAC值寄存器 (0x94-0x98)
// *============================================================================*/
//
///**
// * @brief 程序2电极01正向刺激DAC值 (地址0x94)
// */
//#define LMP1616X1_REG_94_PROG2_ELEC01_DAC_P   (0x94)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_0 : 10;  /**< [9:0] amplitude_dac_p_0[9:0] */
//        uint16_t amplitude_dac_p_1 : 6;   /**< [15:10] amplitude_dac_p_1[5:0] */
//    } bits;
//} lmp1616x1_prog2_elec01_dac_p_t;
//
///**
// * @brief 程序2电极123正向刺激DAC值 (地址0x95)
// */
//#define LMP1616X1_REG_95_PROG2_ELEC123_DAC_P  (0x95)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_1_high : 4;  /**< [3:0] amplitude_dac_p_1[9:6] */
//        uint16_t amplitude_dac_p_2 : 10;      /**< [13:4] amplitude_dac_p_2[9:0] */
//        uint16_t amplitude_dac_p_3 : 2;       /**< [15:14] amplitude_dac_p_3[1:0] */
//    } bits;
//} lmp1616x1_prog2_elec123_dac_p_t;
//
///**
// * @brief 程序2电极34正向刺激DAC值 (地址0x96)
// */
//#define LMP1616X1_REG_96_PROG2_ELEC34_DAC_P   (0x96)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_3_low : 8;   /**< [7:0] amplitude_dac_p_3[9:2] */
//        uint16_t amplitude_dac_p_4 : 8;       /**< [15:8] amplitude_dac_p_4[7:0] */
//    } bits;
//} lmp1616x1_prog2_elec34_dac_p_t;
//
///**
// * @brief 程序2电极456正向刺激DAC值 (地址0x97)
// */
//#define LMP1616X1_REG_97_PROG2_ELEC456_DAC_P  (0x97)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_4_high : 2;  /**< [1:0] amplitude_dac_p_4[9:8] */
//        uint16_t amplitude_dac_p_5 : 10;      /**< [11:2] amplitude_dac_p_5[9:0] */
//        uint16_t amplitude_dac_p_6 : 4;       /**< [15:12] amplitude_dac_p_6[3:0] */
//    } bits;
//} lmp1616x1_prog2_elec456_dac_p_t;
//
///**
// * @brief 程序2电极67正向刺激DAC值 (地址0x98)
// */
//#define LMP1616X1_REG_98_PROG2_ELEC67_DAC_P   (0x98)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_6_high : 6;  /**< [5:0] amplitude_dac_p_6[9:4] */
//        uint16_t amplitude_dac_p_7 : 10;      /**< [15:6] amplitude_dac_p_7[9:0] */
//    } bits;
//} lmp1616x1_prog2_elec67_dac_p_t;
//
///*==============================================================================
// * 程序2负向刺激DAC值寄存器 (0x99-0x9D)
// *============================================================================*/
//
///**
// * @brief 程序2电极01负向刺激DAC值 (地址0x99)
// */
//#define LMP1616X1_REG_99_PROG2_ELEC01_DAC_N   (0x99)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_0 : 10;  /**< [9:0] amplitude_dac_n_0[9:0] */
//        uint16_t amplitude_dac_n_1 : 6;   /**< [15:10] amplitude_dac_n_1[5:0] */
//    } bits;
//} lmp1616x1_prog2_elec01_dac_n_t;
//
///**
// * @brief 程序2电极123负向刺激DAC值 (地址0x9A)
// */
//#define LMP1616X1_REG_9A_PROG2_ELEC123_DAC_N  (0x9A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_1_high : 4;  /**< [3:0] amplitude_dac_n_1[9:6] */
//        uint16_t amplitude_dac_n_2 : 10;      /**< [13:4] amplitude_dac_n_2[9:0] */
//        uint16_t amplitude_dac_n_3 : 2;       /**< [15:14] amplitude_dac_n_3[1:0] */
//    } bits;
//} lmp1616x1_prog2_elec123_dac_n_t;
//
///**
// * @brief 程序2电极34负向刺激DAC值 (地址0x9B)
// */
//#define LMP1616X1_REG_9B_PROG2_ELEC34_DAC_N   (0x9B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_3_low : 8;   /**< [7:0] amplitude_dac_n_3[9:2] */
//        uint16_t amplitude_dac_n_4 : 8;       /**< [15:8] amplitude_dac_n_4[7:0] */
//    } bits;
//} lmp1616x1_prog2_elec34_dac_n_t;
//
///**
// * @brief 程序2电极456负向刺激DAC值 (地址0x9C)
// */
//#define LMP1616X1_REG_9C_PROG2_ELEC456_DAC_N  (0x9C)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_4_high : 2;  /**< [1:0] amplitude_dac_n_4[9:8] */
//        uint16_t amplitude_dac_n_5 : 10;      /**< [11:2] amplitude_dac_n_5[9:0] */
//        uint16_t amplitude_dac_n_6 : 4;       /**< [15:12] amplitude_dac_n_6[3:0] */
//    } bits;
//} lmp1616x1_prog2_elec456_dac_n_t;
//
///**
// * @brief 程序2电极67负向刺激DAC值 (地址0x9D)
// */
//#define LMP1616X1_REG_9D_PROG2_ELEC67_DAC_N   (0x9D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_6_high : 6;  /**< [5:0] amplitude_dac_n_6[9:4] */
//        uint16_t amplitude_dac_n_7 : 10;      /**< [15:6] amplitude_dac_n_7[9:0] */
//    } bits;
//} lmp1616x1_prog2_elec67_dac_n_t;
//
///*==============================================================================
// * 程序3正向刺激DAC值寄存器 (0x9E-0xA2)
// *============================================================================*/
//
///**
// * @brief 程序3电极01正向刺激DAC值 (地址0x9E)
// */
//#define LMP1616X1_REG_9E_PROG3_ELEC01_DAC_P   (0x9E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_0 : 10;  /**< [9:0] amplitude_dac_p_0[9:0], 默认值0x00C */
//        uint16_t amplitude_dac_p_1 : 6;   /**< [15:10] amplitude_dac_p_1[5:0] */
//    } bits;
//} lmp1616x1_prog3_elec01_dac_p_t;
//
///**
// * @brief 程序3电极123正向刺激DAC值 (地址0x9F)
// */
//#define LMP1616X1_REG_9F_PROG3_ELEC123_DAC_P  (0x9F)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_1_high : 4;  /**< [3:0] amplitude_dac_p_1[9:6] */
//        uint16_t amplitude_dac_p_2 : 10;      /**< [13:4] amplitude_dac_p_2[9:0] */
//        uint16_t amplitude_dac_p_3 : 2;       /**< [15:14] amplitude_dac_p_3[1:0] */
//    } bits;
//} lmp1616x1_prog3_elec123_dac_p_t;
//
///**
// * @brief 程序3电极34正向刺激DAC值 (地址0xA0)
// */
//#define LMP1616X1_REG_A0_PROG3_ELEC34_DAC_P   (0xA0)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_3_low : 8;   /**< [7:0] amplitude_dac_p_3[9:2] */
//        uint16_t amplitude_dac_p_4 : 8;       /**< [15:8] amplitude_dac_p_4[7:0] */
//    } bits;
//} lmp1616x1_prog3_elec34_dac_p_t;
//
///**
// * @brief 程序3电极456正向刺激DAC值 (地址0xA1)
// */
//#define LMP1616X1_REG_A1_PROG3_ELEC456_DAC_P  (0xA1)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_4_high : 2;  /**< [1:0] amplitude_dac_p_4[9:8] */
//        uint16_t amplitude_dac_p_5 : 10;      /**< [11:2] amplitude_dac_p_5[9:0] */
//        uint16_t amplitude_dac_p_6 : 4;       /**< [15:12] amplitude_dac_p_6[3:0] */
//    } bits;
//} lmp1616x1_prog3_elec456_dac_p_t;
//
///**
// * @brief 程序3电极67正向刺激DAC值 (地址0xA2)
// */
//#define LMP1616X1_REG_A2_PROG3_ELEC67_DAC_P   (0xA2)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_6_high : 6;  /**< [5:0] amplitude_dac_p_6[9:4] */
//        uint16_t amplitude_dac_p_7 : 10;      /**< [15:6] amplitude_dac_p_7[9:0] */
//    } bits;
//} lmp1616x1_prog3_elec67_dac_p_t;
//
///*==============================================================================
// * 程序3负向刺激DAC值寄存器 (0xA3-0xA7)
// *============================================================================*/
//
///**
// * @brief 程序3电极01负向刺激DAC值 (地址0xA3)
// */
//#define LMP1616X1_REG_A3_PROG3_ELEC01_DAC_N   (0xA3)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_0 : 10;  /**< [9:0] amplitude_dac_n_0[9:0] */
//        uint16_t amplitude_dac_n_1 : 6;   /**< [15:10] amplitude_dac_n_1[5:0] */
//    } bits;
//} lmp1616x1_prog3_elec01_dac_n_t;
//
///**
// * @brief 程序3电极123负向刺激DAC值 (地址0xA4)
// */
//#define LMP1616X1_REG_A4_PROG3_ELEC123_DAC_N  (0xA4)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_1_high : 4;  /**< [3:0] amplitude_dac_n_1[9:6] */
//        uint16_t amplitude_dac_n_2 : 10;      /**< [13:4] amplitude_dac_n_2[9:0] */
//        uint16_t amplitude_dac_n_3 : 2;       /**< [15:14] amplitude_dac_n_3[1:0] */
//    } bits;
//} lmp1616x1_prog3_elec123_dac_n_t;
//
///**
// * @brief 程序3电极34负向刺激DAC值 (地址0xA5)
// */
//#define LMP1616X1_REG_A5_PROG3_ELEC34_DAC_N   (0xA5)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_3_low : 8;   /**< [7:0] amplitude_dac_n_3[9:2] */
//        uint16_t amplitude_dac_n_4 : 8;       /**< [15:8] amplitude_dac_n_4[7:0] */
//    } bits;
//} lmp1616x1_prog3_elec34_dac_n_t;
//
///**
// * @brief 程序3电极456负向刺激DAC值 (地址0xA6)
// */
//#define LMP1616X1_REG_A6_PROG3_ELEC456_DAC_N  (0xA6)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_4_high : 2;  /**< [1:0] amplitude_dac_n_4[9:8] */
//        uint16_t amplitude_dac_n_5 : 10;      /**< [11:2] amplitude_dac_n_5[9:0] */
//        uint16_t amplitude_dac_n_6 : 4;       /**< [15:12] amplitude_dac_n_6[3:0] */
//    } bits;
//} lmp1616x1_prog3_elec456_dac_n_t;
//
///**
// * @brief 程序3电极67负向刺激DAC值 (地址0xA7)
// */
//#define LMP1616X1_REG_A7_PROG3_ELEC67_DAC_N   (0xA7)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_n_6_high : 6;  /**< [5:0] amplitude_dac_n_6[9:4] */
//        uint16_t amplitude_dac_n_7 : 10;      /**< [15:6] amplitude_dac_n_7[9:0] */
//    } bits;
//} lmp1616x1_prog3_elec67_dac_n_t;
//
///*==============================================================================
// * 寄存器最大值
// *============================================================================*/
//#define LMP1616X1_REG_MAX              (0xA7)
//
//#endif /* __LMP1616X1_REG_H__ */
