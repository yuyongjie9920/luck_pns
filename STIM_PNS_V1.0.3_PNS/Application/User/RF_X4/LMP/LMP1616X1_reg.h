///**
// * @file lmp1616x1_reg.h
// * @brief LMP1616X1 芯片寄存器定义头文件
// * @version 1.0
// * @date 2026-04-09
// *
// * 基于LMP1616X1寄存器定义，适配16电极版本（电极0-15）
// * 寄存器地址范围: 0x00-0x7E
// * 支持4个刺激程序组（程序0-3）
// */
//
//#ifndef __LMP1616X1_REG_H__
//#define __LMP1616X1_REG_H__
//
//#include <stdint.h>
//
///*==============================================================================
// * 刺激波形时序寄存器 (0x00-0x0B)
// *============================================================================*/
//
///**
// * @brief Tphase1寄存器 (地址0x00)
// * @details 数值表示正脉冲多少个时钟周期，此数值的设定与phase_config存在一定关系，
// *          即必须是当前phase中使用多少组DAC值的倍数。
// *          phase_config==0时，Tphase1最大值为0x400（1024个clk cycle）
// *          phase_config==1时，Tphase1最大值为0x800（2048个clk cycle）
// */
//#define LMP1616X1_REG_00_TPHASE1               (0x00)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase1 : 12;           /**< [11:0] 正脉冲时钟周期数，默认值0xD2 */
//        uint16_t reserved0 : 1;          /**< [12] 保留位，默认值0x0 */
//        uint16_t sel_rvt : 2;            /**< [14:13] SEL_RVT[4:3]控制端，默认为0 */
//        uint16_t reserved1 : 1;          /**< [15] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_tphase1_t;
//
///**
// * @brief Tphase2寄存器 (地址0x01)
// * @details 数值表示负脉冲多少个时钟周期，此数值的设定与phase_config存在一定关系，
// *          即必须是当前phase中使用多少组DAC值的倍数。
// */
//#define LMP1616X1_REG_01_TPHASE2               (0x01)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase2 : 12;           /**< [11:0] 负脉冲时钟周期数，默认值0xD2 */
//        uint16_t reserved : 1;           /**< [12] 保留位，默认值0x0 */
//        uint16_t demod_triml : 3;        /**< [15:13] DEMOD_TRIML[2:0]，给解调器模块做修调用，默认值0x0 */
//    } bits;
//} lmp1616x1_tphase2_t;
//
///**
// * @brief Tinterphase寄存器 (地址0x02)
// * @details 相间间隔时间配置
// */
//#define LMP1616X1_REG_02_TINTERPHASE           (0x02)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterphase1 : 8;       /**< [7:0] Tinterphase1：数值表示多少个时钟周期，默认值0xD2 */
//        uint16_t tinterphase2 : 8;       /**< [15:8] Tinterphase2：数值表示多少个时钟周期，默认值0xD2 */
//    } bits;
//} lmp1616x1_tinterphase_t;
//
///**
// * @brief Tphase3寄存器 (地址0x03)
// * @details 数值表示电荷平衡阶段多少个时钟周期
// */
//#define LMP1616X1_REG_03_TPHASE3               (0x03)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase3 : 12;           /**< [11:0] 电荷平衡阶段时钟周期数，默认值0xA0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_tphase3_t;
//
///**
// * @brief Tinterpulse寄存器 (地址0x04)
// * @details Tphase3与下一周期之间的延迟多少个时钟周期
// */
//#define LMP1616X1_REG_04_TINTERPULSE           (0x04)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterpulse : 16;       /**< [15:0] Tphase3与下一周期之间的延迟时钟周期数，默认值0x0 */
//    } bits;
//} lmp1616x1_tinterpulse_t;
//
///**
// * @brief Tdefine寄存器 (地址0x05)
// * @details 刺激时序定义寄存器
// */
//#define LMP1616X1_REG_05_TDEFINE               (0x05)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t pulse_cnt : 8;          /**< [7:0] pulse_cnt：刺激脉冲计数，默认值0x7F */
//        uint16_t reserved : 2;           /**< [9:8] 保留位，stim_td2修改为固定的1 clock cycle的延时，默认值0x0 */
//        uint16_t stim_td3 : 6;           /**< [15:10] 电荷平衡结束与STIM_EN\STIM_ON\DAC_EN关闭之间的延迟时间，典型值32us，默认值0x1F */
//    } bits;
//} lmp1616x1_tdefine_t;
//
///**
// * @brief STIM_MODE寄存器 (地址0x06)
// * @details 刺激模式控制寄存器
// */
//#define LMP1616X1_REG_06_STIM_MODE             (0x06)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_start : 1;          /**< [0] 给刺激波形模块的开始刺激信号，默认值0x0 */
//        uint16_t para_vld : 1;            /**< [1] 给刺激波形模块的配置参数有效信号，更新参数使用，默认值0x0 */
//        uint16_t stim_stop : 1;           /**< [2] 停止刺激，1：停止刺激，0：不停止，默认值0x0 */
//        uint16_t stim_mode : 2;           /**< [4:3] 刺激模式，2'b00:简单模式，2'b01:任意模式，2'b10：突发刺激模式，默认值0x0 */
//        uint16_t stim_low_power : 1;      /**< [5] 低功耗模式，默认值0x0 */
//        uint16_t program_cnt : 2;         /**< [7:6] 刺激程序数量，0：一个刺激程序，1：两个刺激程序，默认值0x0 */
//        uint16_t stim_clk_div_value : 6;  /**< [13:8] 刺激时钟分频，0不分频，1:2分频，n:n+1分频，最大62，默认值0x0 */
//        uint16_t stim_auto_stop : 1;      /**< [14] 刺激自动停止开关，0：关闭，1：开启，默认值0x0 */
//        uint16_t reserved : 1;            /**< [15] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_stim_mode_t;
//
///**
// * @brief burst_cnt寄存器 (地址0x07)
// * @details 突发计数/PWM模式寄存器
// */
//#define LMP1616X1_REG_07_BURST_CNT             (0x07)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t pwm_power_dly : 13;     /**< [12:0] 占空比供能时，检测到供能之后多少个RC时钟周期后再进入正向刺激，默认值0x0 */
//        uint16_t pwm_mode : 1;           /**< [13] 1表示占空比供能模式，0表示常供能模式，默认值0x0 */
//        uint16_t reserved : 2;           /**< [15:14] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_burst_cnt_t;
//
///**
// * @brief Tinterburst寄存器 (地址0x08)
// * @details 突发模式下面刺激簇之间的间隔时间
// */
//#define LMP1616X1_REG_08_TINTERBURST           (0x08)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterburst : 16;       /**< [15:0] 突发模式下面刺激簇之间的间隔时间，默认值0x0 */
//    } bits;
//} lmp1616x1_tinterburst_t;
//
///**
// * @brief softstart_cycle寄存器 (地址0x09)
// * @details 软启动周期寄存器
// */
//#define LMP1616X1_REG_09_SOFTSTART_CYCLE       (0x09)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t softstart_finish_cycle : 10; /**< [9:0] 表示有多少个增加softstart_inc_value的cycle，默认值0x0 */
//        uint16_t reserved : 6;               /**< [15:10] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_softstart_cycle_t;
//
///**
// * @brief stim_auto_stop_cycle寄存器 (地址0x0A)
// * @details 自动停止周期寄存器
// */
//#define LMP1616X1_REG_0A_STIM_AUTO_STOP_CYCLE  (0x0A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_auto_stop_cycle : 16; /**< [15:0] 每位数值表示的是4个刺激周期，默认值0x0 */
//    } bits;
//} lmp1616x1_stim_auto_stop_cycle_t;
//
///**
// * @brief 软启动控制寄存器 (地址0x0B)
// * @details 所有程序软启动控制
// */
//#define LMP1616X1_REG_0B_SOFTSTART_CTRL        (0x0B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t softstart_step : 10;    /**< [9:0] 有效值1~1023，表示经过多少个刺激周期后触发一次softstart的increase value，默认值0x0 */
//        uint16_t reserved : 5;           /**< [14:10] 保留位，默认值0x0 */
//        uint16_t softstart_en : 1;       /**< [15] 所有程序软启动使能，应用于所有程序组，默认值0x0 */
//    } bits;
//} lmp1616x1_softstart_ctrl_t;
//
///*==============================================================================
// * ADC/RFID配置寄存器 (0x0C-0x0F)
// *============================================================================*/
//
///**
// * @brief adc_diff_cfg寄存器 (地址0x0C)
// * @details ADC差分配置寄存器
// */
//#define LMP1616X1_REG_0C_ADC_DIFF_CFG          (0x0C)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t rfid_resp_delay_cnt : 11; /**< [10:0] 控制芯片收到demod解码后多久返回mod给程控仪，默认值0x0 */
//        uint16_t sel_demod : 1;            /**< [11] 0选择原始射频解调信号，1选择数字滤波后的射频解调信号，默认值0x0 */
//        uint16_t pdelt : 4;                /**< [15:12] 设置数字滤波器波宽度，正常为2,3，默认值0x0 */
//    } bits;
//} lmp1616x1_adc_diff_cfg_t;
//
///**
// * @brief rfid_cfg1寄存器 (地址0x0D)
// * @details RFID RX端配置1
// */
//#define LMP1616X1_REG_0D_RFID_CFG1             (0x0D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tari_min : 8;           /**< [7:0] TariMin，默认值0x03 */
//        uint16_t tari_max : 8;           /**< [15:8] TariMax，默认值0x32 */
//    } bits;
//} lmp1616x1_rfid_cfg1_t;
//
///**
// * @brief rfid_cfg2寄存器 (地址0x0E)
// * @details RFID RX端配置2
// */
//#define LMP1616X1_REG_0E_RFID_CFG2             (0x0E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t delta : 8;              /**< [7:0] Delta，默认值0x02 */
//        uint16_t rt_max : 8;             /**< [15:8] RtMax，默认值0xFF */
//    } bits;
//} lmp1616x1_rfid_cfg2_t;
//
///**
// * @brief rfid_cfg3寄存器 (地址0x0F)
// * @details RFID RX端配置3
// */
//#define LMP1616X1_REG_0F_RFID_CFG3             (0x0F)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t demod_pon_cnt : 9;      /**< [8:0] 占空比功能时demod高电平多久认定为供能，以区别通讯时的Rtcal高电平时间，默认值0x80 */
//        uint16_t reserved : 2;           /**< [10:9] 保留位，默认值0x0 */
//        uint16_t rfid_tx_data_period : 5; /**< [15:11] 默认值0xA，由于半周期也必须是正整数，因此值只能是偶数，最大值0x1E(30)，默认值0xA */
//    } bits;
//} lmp1616x1_rfid_cfg3_t;
//
///*==============================================================================
// * eFuse相关寄存器 (0x10-0x14)
// *============================================================================*/
//
///**
// * @brief efuse_w_data0寄存器 (地址0x10)
// * @details eFuse写数据[15:0]
// */
//#define LMP1616X1_REG_10_EFUSE_W_DATA0         (0x10)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_w_data : 16;      /**< [15:0] efuse_w_data[15:0]，默认值0x0 */
//    } bits;
//} lmp1616x1_efuse_w_data0_t;
//
///**
// * @brief efuse_w_data1寄存器 (地址0x11)
// * @details eFuse写数据[23:16]及控制
// */
//#define LMP1616X1_REG_11_EFUSE_W_DATA1         (0x11)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_w_data_high : 8;  /**< [7:0] efuse_w_data[23:16]，默认值0x0 */
//        uint16_t efuse_data_enable : 6;  /**< [13:8] 每bit为1表示databus的4bit数据有效，需要写入efuse，默认值0x0 */
//        uint16_t reversed : 1;           /**< [14] 保留位，默认值0x0 */
//        uint16_t efuse_data_valid : 1;   /**< [15] 0->1变化表示触发一次efuse写操作，默认值0x0 */
//    } bits;
//} lmp1616x1_efuse_w_data1_t;
//
///**
// * @brief efuse_r_data0寄存器 (地址0x12)
// * @details eFuse读数据[15:0]
// */
//#define LMP1616X1_REG_12_EFUSE_R_DATA0         (0x12)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_r_data : 16;      /**< [15:0] efuse_r_data[15:0]，只读，默认值0x0 */
//    } bits;
//} lmp1616x1_efuse_r_data0_t;
//
///**
// * @brief efuse_r_data1寄存器 (地址0x13)
// * @details eFuse读数据[23:16]及状态
// */
//#define LMP1616X1_REG_13_EFUSE_R_DATA1         (0x13)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_r_data_high : 8;  /**< [7:0] efuse_r_data[23:16]，只读，默认值0x0 */
//        uint16_t reserved : 7;           /**< [14:8] 保留位，默认值0x0 */
//        uint16_t efuse_auto_read_done : 1; /**< [15] 1表示上电后自动读efuse执行完毕，0表示未执行完毕，只读，默认值0x0 */
//    } bits;
//} lmp1616x1_efuse_r_data1_t;
//
///**
// * @brief efuse_tburn寄存器 (地址0x14)
// * @details eFuse烧写时间配置
// */
//#define LMP1616X1_REG_14_EFUSE_TBURN           (0x14)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_tburn : 15;       /**< [14:0] 1bit的烧写时间，单位us，默认值0x0 */
//        uint16_t efuse_burn_clk_en : 1;  /**< [15] 寄存器控制efuse gating时钟，为1时常开，为0时内部逻辑控制关时钟，默认值0x0 */
//    } bits;
//} lmp1616x1_efuse_tburn_t;
//
///*==============================================================================
// * DAC值寄存器 - 程序0 (0x15-0x1E)
// * 说明：电极0-15的正向刺激DAC值
// *============================================================================*/
//
///**
// * @brief 程序0电极01正向刺激DAC值寄存器 (地址0x15)
// */
//#define LMP1616X1_REG_15_PROG0_ELEC01_DAC_P    (0x15)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_0 : 10; /**< [9:0] 电极0 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_1_low : 6; /**< [15:10] 电极1 DAC低6位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog0_elec01_dac_p_t;
//
///**
// * @brief 程序0电极123正向刺激DAC值寄存器 (地址0x16)
// */
//#define LMP1616X1_REG_16_PROG0_ELEC123_DAC_P   (0x16)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_1_high : 4; /**< [3:0] 电极1 DAC高4位，默认值0x0 */
//        uint16_t amplitude_dac_p_2 : 10;    /**< [13:4] 电极2 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_3_low : 2; /**< [15:14] 电极3 DAC低2位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog0_elec123_dac_p_t;
//
///**
// * @brief 程序0电极34正向刺激DAC值寄存器 (地址0x17)
// */
//#define LMP1616X1_REG_17_PROG0_ELEC34_DAC_P    (0x17)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_3_high : 8; /**< [7:0] 电极3 DAC高8位，默认值0x0 */
//        uint16_t amplitude_dac_p_4_low : 8;  /**< [15:8] 电极4 DAC低8位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog0_elec34_dac_p_t;
//
///**
// * @brief 程序0电极456正向刺激DAC值寄存器 (地址0x18)
// */
//#define LMP1616X1_REG_18_PROG0_ELEC456_DAC_P   (0x18)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_4_high : 2; /**< [1:0] 电极4 DAC高2位，默认值0x0 */
//        uint16_t amplitude_dac_p_5 : 10;    /**< [11:2] 电极5 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_6_low : 4; /**< [15:12] 电极6 DAC低4位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog0_elec456_dac_p_t;
//
///**
// * @brief 程序0电极67正向刺激DAC值寄存器 (地址0x19)
// */
//#define LMP1616X1_REG_19_PROG0_ELEC67_DAC_P    (0x19)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_6_high : 6; /**< [5:0] 电极6 DAC高6位，默认值0x0 */
//        uint16_t amplitude_dac_p_7 : 10;    /**< [15:6] 电极7 DAC值，默认值0x0 */
//    } bits;
//} lmp1616x1_prog0_elec67_dac_p_t;
//
///**
// * @brief 程序0电极89正向刺激DAC值寄存器 (地址0x1A)
// */
//#define LMP1616X1_REG_1A_PROG0_ELEC89_DAC_P    (0x1A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_8 : 10;    /**< [9:0] 电极8 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_9_low : 6; /**< [15:10] 电极9 DAC低6位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog0_elec89_dac_p_t;
//
///**
// * @brief 程序0电极9 10 11正向刺激DAC值寄存器 (地址0x1B)
// */
//#define LMP1616X1_REG_1B_PROG0_ELEC9_10_11_DAC_P (0x1B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_9_high : 4; /**< [3:0] 电极9 DAC高4位，默认值0x0 */
//        uint16_t amplitude_dac_p_10 : 10;   /**< [13:4] 电极10 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_11_low : 2; /**< [15:14] 电极11 DAC低2位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog0_elec9_10_11_dac_p_t;
//
///**
// * @brief 程序0电极11 12正向刺激DAC值寄存器 (地址0x1C)
// */
//#define LMP1616X1_REG_1C_PROG0_ELEC11_12_DAC_P (0x1C)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_11_high : 8; /**< [7:0] 电极11 DAC高8位，默认值0xC */
//        uint16_t amplitude_dac_p_12_low : 8;  /**< [15:8] 电极12 DAC低8位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog0_elec11_12_dac_p_t;
//
///**
// * @brief 程序0电极12 13 14正向刺激DAC值寄存器 (地址0x1D)
// */
//#define LMP1616X1_REG_1D_PROG0_ELEC12_13_14_DAC_P (0x1D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_12_high : 2; /**< [1:0] 电极12 DAC高2位，默认值0x0 */
//        uint16_t amplitude_dac_p_13 : 10;    /**< [11:2] 电极13 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_14_low : 4; /**< [15:12] 电极14 DAC低4位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog0_elec12_13_14_dac_p_t;
//
///**
// * @brief 程序0电极14 15正向刺激DAC值寄存器 (地址0x1E)
// */
//#define LMP1616X1_REG_1E_PROG0_ELEC14_15_DAC_P (0x1E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_14_high : 6; /**< [5:0] 电极14 DAC高6位，默认值0x0 */
//        uint16_t amplitude_dac_p_15 : 10;    /**< [15:6] 电极15 DAC值，默认值0x0 */
//    } bits;
//} lmp1616x1_prog0_elec14_15_dac_p_t;
//
///*==============================================================================
// * DAC值寄存器 - 程序1 (0x1F-0x28)
// *============================================================================*/
//
///**
// * @brief 程序1电极01正向刺激DAC值寄存器 (地址0x1F)
// */
//#define LMP1616X1_REG_1F_PROG1_ELEC01_DAC_P    (0x1F)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_0 : 10; /**< [9:0] 电极0 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_1_low : 6; /**< [15:10] 电极1 DAC低6位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_elec01_dac_p_t;
//
///**
// * @brief 程序1电极123正向刺激DAC值寄存器 (地址0x20)
// */
//#define LMP1616X1_REG_20_PROG1_ELEC123_DAC_P   (0x20)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_1_high : 4; /**< [3:0] 电极1 DAC高4位，默认值0x0 */
//        uint16_t amplitude_dac_p_2 : 10;    /**< [13:4] 电极2 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_3_low : 2; /**< [15:14] 电极3 DAC低2位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_elec123_dac_p_t;
//
///**
// * @brief 程序1电极34正向刺激DAC值寄存器 (地址0x21)
// */
//#define LMP1616X1_REG_21_PROG1_ELEC34_DAC_P    (0x21)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_3_high : 8; /**< [7:0] 电极3 DAC高8位，默认值0x0 */
//        uint16_t amplitude_dac_p_4_low : 8;  /**< [15:8] 电极4 DAC低8位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_elec34_dac_p_t;
//
///**
// * @brief 程序1电极456正向刺激DAC值寄存器 (地址0x22)
// */
//#define LMP1616X1_REG_22_PROG1_ELEC456_DAC_P   (0x22)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_4_high : 2; /**< [1:0] 电极4 DAC高2位，默认值0x0 */
//        uint16_t amplitude_dac_p_5 : 10;    /**< [11:2] 电极5 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_6_low : 4; /**< [15:12] 电极6 DAC低4位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_elec456_dac_p_t;
//
///**
// * @brief 程序1电极67正向刺激DAC值寄存器 (地址0x23)
// */
//#define LMP1616X1_REG_23_PROG1_ELEC67_DAC_P    (0x23)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_6_high : 6; /**< [5:0] 电极6 DAC高6位，默认值0x0 */
//        uint16_t amplitude_dac_p_7 : 10;    /**< [15:6] 电极7 DAC值，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_elec67_dac_p_t;
//
///**
// * @brief 程序1电极89正向刺激DAC值寄存器 (地址0x24)
// */
//#define LMP1616X1_REG_24_PROG1_ELEC89_DAC_P    (0x24)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_8 : 10;    /**< [9:0] 电极8 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_9_low : 6; /**< [15:10] 电极9 DAC低6位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_elec89_dac_p_t;
//
///**
// * @brief 程序1电极9 10 11正向刺激DAC值寄存器 (地址0x25)
// */
//#define LMP1616X1_REG_25_PROG1_ELEC9_10_11_DAC_P (0x25)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_9_high : 4; /**< [3:0] 电极9 DAC高4位，默认值0x0 */
//        uint16_t amplitude_dac_p_10 : 10;   /**< [13:4] 电极10 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_11_low : 2; /**< [15:14] 电极11 DAC低2位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_elec9_10_11_dac_p_t;
//
///**
// * @brief 程序1电极11 12正向刺激DAC值寄存器 (地址0x26)
// */
//#define LMP1616X1_REG_26_PROG1_ELEC11_12_DAC_P (0x26)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_11_high : 8; /**< [7:0] 电极11 DAC高8位，默认值0x0 */
//        uint16_t amplitude_dac_p_12_low : 8;  /**< [15:8] 电极12 DAC低8位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_elec11_12_dac_p_t;
//
///**
// * @brief 程序1电极12 13 14正向刺激DAC值寄存器 (地址0x27)
// */
//#define LMP1616X1_REG_27_PROG1_ELEC12_13_14_DAC_P (0x27)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_12_high : 2; /**< [1:0] 电极12 DAC高2位，默认值0x0 */
//        uint16_t amplitude_dac_p_13 : 10;    /**< [11:2] 电极13 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_14_low : 4; /**< [15:12] 电极14 DAC低4位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_elec12_13_14_dac_p_t;
//
///**
// * @brief 程序1电极14 15正向刺激DAC值寄存器 (地址0x28)
// */
//#define LMP1616X1_REG_28_PROG1_ELEC14_15_DAC_P (0x28)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_14_high : 6; /**< [5:0] 电极14 DAC高6位，默认值0x0 */
//        uint16_t amplitude_dac_p_15 : 10;    /**< [15:6] 电极15 DAC值，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_elec14_15_dac_p_t;
//
///*==============================================================================
// * DAC值寄存器 - 程序2 (0x29-0x32)
// *============================================================================*/
//
///**
// * @brief 程序2电极01正向刺激DAC值寄存器 (地址0x29)
// */
//#define LMP1616X1_REG_29_PROG2_ELEC01_DAC_P    (0x29)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_0 : 10; /**< [9:0] 电极0 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_1_low : 6; /**< [15:10] 电极1 DAC低6位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_elec01_dac_p_t;
//
///**
// * @brief 程序2电极123正向刺激DAC值寄存器 (地址0x2A)
// */
//#define LMP1616X1_REG_2A_PROG2_ELEC123_DAC_P   (0x2A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_1_high : 4; /**< [3:0] 电极1 DAC高4位，默认值0x0 */
//        uint16_t amplitude_dac_p_2 : 10;    /**< [13:4] 电极2 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_3_low : 2; /**< [15:14] 电极3 DAC低2位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_elec123_dac_p_t;
//
///**
// * @brief 程序2电极34正向刺激DAC值寄存器 (地址0x2B)
// */
//#define LMP1616X1_REG_2B_PROG2_ELEC34_DAC_P    (0x2B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_3_high : 8; /**< [7:0] 电极3 DAC高8位，默认值0x0 */
//        uint16_t amplitude_dac_p_4_low : 8;  /**< [15:8] 电极4 DAC低8位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_elec34_dac_p_t;
//
///**
// * @brief 程序2电极456正向刺激DAC值寄存器 (地址0x2C)
// */
//#define LMP1616X1_REG_2C_PROG2_ELEC456_DAC_P   (0x2C)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_4_high : 2; /**< [1:0] 电极4 DAC高2位，默认值0x0 */
//        uint16_t amplitude_dac_p_5 : 10;    /**< [11:2] 电极5 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_6_low : 4; /**< [15:12] 电极6 DAC低4位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_elec456_dac_p_t;
//
///**
// * @brief 程序2电极67正向刺激DAC值寄存器 (地址0x2D)
// */
//#define LMP1616X1_REG_2D_PROG2_ELEC67_DAC_P    (0x2D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_6_high : 6; /**< [5:0] 电极6 DAC高6位，默认值0x0 */
//        uint16_t amplitude_dac_p_7 : 10;    /**< [15:6] 电极7 DAC值，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_elec67_dac_p_t;
//
///**
// * @brief 程序2电极89正向刺激DAC值寄存器 (地址0x2E)
// */
//#define LMP1616X1_REG_2E_PROG2_ELEC89_DAC_P    (0x2E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_8 : 10;    /**< [9:0] 电极8 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_9_low : 6; /**< [15:10] 电极9 DAC低6位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_elec89_dac_p_t;
//
///**
// * @brief 程序2电极9 10 11正向刺激DAC值寄存器 (地址0x2F)
// */
//#define LMP1616X1_REG_2F_PROG2_ELEC9_10_11_DAC_P (0x2F)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_9_high : 4; /**< [3:0] 电极9 DAC高4位，默认值0x0 */
//        uint16_t amplitude_dac_p_10 : 10;   /**< [13:4] 电极10 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_11_low : 2; /**< [15:14] 电极11 DAC低2位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_elec9_10_11_dac_p_t;
//
///**
// * @brief 程序2电极11 12正向刺激DAC值寄存器 (地址0x30)
// */
//#define LMP1616X1_REG_30_PROG2_ELEC11_12_DAC_P (0x30)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_11_high : 8; /**< [7:0] 电极11 DAC高8位，默认值0x0 */
//        uint16_t amplitude_dac_p_12_low : 8;  /**< [15:8] 电极12 DAC低8位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_elec11_12_dac_p_t;
//
///**
// * @brief 程序2电极12 13 14正向刺激DAC值寄存器 (地址0x31)
// */
//#define LMP1616X1_REG_31_PROG2_ELEC12_13_14_DAC_P (0x31)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_12_high : 2; /**< [1:0] 电极12 DAC高2位，默认值0x0 */
//        uint16_t amplitude_dac_p_13 : 10;    /**< [11:2] 电极13 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_14_low : 4; /**< [15:12] 电极14 DAC低4位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_elec12_13_14_dac_p_t;
//
///**
// * @brief 程序2电极14 15正向刺激DAC值寄存器 (地址0x32)
// */
//#define LMP1616X1_REG_32_PROG2_ELEC14_15_DAC_P (0x32)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_14_high : 6; /**< [5:0] 电极14 DAC高6位，默认值0x0 */
//        uint16_t amplitude_dac_p_15 : 10;    /**< [15:6] 电极15 DAC值，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_elec14_15_dac_p_t;
//
///*==============================================================================
// * DAC值寄存器 - 程序3 (0x33-0x3C)
// *============================================================================*/
//
///**
// * @brief 程序3电极01正向刺激DAC值寄存器 (地址0x33)
// */
//#define LMP1616X1_REG_33_PROG3_ELEC01_DAC_P    (0x33)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_0 : 10; /**< [9:0] 电极0 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_1_low : 6; /**< [15:10] 电极1 DAC低6位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_elec01_dac_p_t;
//
///**
// * @brief 程序3电极123正向刺激DAC值寄存器 (地址0x34)
// */
//#define LMP1616X1_REG_34_PROG3_ELEC123_DAC_P   (0x34)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_1_high : 4; /**< [3:0] 电极1 DAC高4位，默认值0x0 */
//        uint16_t amplitude_dac_p_2 : 10;    /**< [13:4] 电极2 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_3_low : 2; /**< [15:14] 电极3 DAC低2位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_elec123_dac_p_t;
//
///**
// * @brief 程序3电极34正向刺激DAC值寄存器 (地址0x35)
// */
//#define LMP1616X1_REG_35_PROG3_ELEC34_DAC_P    (0x35)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_3_high : 8; /**< [7:0] 电极3 DAC高8位，默认值0x0 */
//        uint16_t amplitude_dac_p_4_low : 8;  /**< [15:8] 电极4 DAC低8位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_elec34_dac_p_t;
//
///**
// * @brief 程序3电极456正向刺激DAC值寄存器 (地址0x36)
// */
//#define LMP1616X1_REG_36_PROG3_ELEC456_DAC_P   (0x36)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_4_high : 2; /**< [1:0] 电极4 DAC高2位，默认值0x0 */
//        uint16_t amplitude_dac_p_5 : 10;    /**< [11:2] 电极5 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_6_low : 4; /**< [15:12] 电极6 DAC低4位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_elec456_dac_p_t;
//
///**
// * @brief 程序3电极67正向刺激DAC值寄存器 (地址0x37)
// */
//#define LMP1616X1_REG_37_PROG3_ELEC67_DAC_P    (0x37)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_6_high : 6; /**< [5:0] 电极6 DAC高6位，默认值0x0 */
//        uint16_t amplitude_dac_p_7 : 10;    /**< [15:6] 电极7 DAC值，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_elec67_dac_p_t;
//
///**
// * @brief 程序3电极89正向刺激DAC值寄存器 (地址0x38)
// */
//#define LMP1616X1_REG_38_PROG3_ELEC89_DAC_P    (0x38)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_8 : 10;    /**< [9:0] 电极8 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_9_low : 6; /**< [15:10] 电极9 DAC低6位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_elec89_dac_p_t;
//
///**
// * @brief 程序3电极9 10 11正向刺激DAC值寄存器 (地址0x39)
// */
//#define LMP1616X1_REG_39_PROG3_ELEC9_10_11_DAC_P (0x39)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_9_high : 4; /**< [3:0] 电极9 DAC高4位，默认值0x0 */
//        uint16_t amplitude_dac_p_10 : 10;   /**< [13:4] 电极10 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_11_low : 2; /**< [15:14] 电极11 DAC低2位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_elec9_10_11_dac_p_t;
//
///**
// * @brief 程序3电极11 12正向刺激DAC值寄存器 (地址0x3A)
// */
//#define LMP1616X1_REG_3A_PROG3_ELEC11_12_DAC_P (0x3A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_11_high : 8; /**< [7:0] 电极11 DAC高8位，默认值0x0 */
//        uint16_t amplitude_dac_p_12_low : 8;  /**< [15:8] 电极12 DAC低8位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_elec11_12_dac_p_t;
//
///**
// * @brief 程序3电极12 13 14正向刺激DAC值寄存器 (地址0x3B)
// */
//#define LMP1616X1_REG_3B_PROG3_ELEC12_13_14_DAC_P (0x3B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_12_high : 2; /**< [1:0] 电极12 DAC高2位，默认值0x0 */
//        uint16_t amplitude_dac_p_13 : 10;    /**< [11:2] 电极13 DAC值，默认值0x0 */
//        uint16_t amplitude_dac_p_14_low : 4; /**< [15:12] 电极14 DAC低4位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_elec12_13_14_dac_p_t;
//
///**
// * @brief 程序3电极14 15正向刺激DAC值寄存器 (地址0x3C)
// */
//#define LMP1616X1_REG_3C_PROG3_ELEC14_15_DAC_P (0x3C)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_p_14_high : 6; /**< [5:0] 电极14 DAC高6位，默认值0x0 */
//        uint16_t amplitude_dac_p_15 : 10;    /**< [15:6] 电极15 DAC值，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_elec14_15_dac_p_t;
//
///*==============================================================================
// * 电极开关控制寄存器 (0x3D-0x44)
// *============================================================================*/
//
///**
// * @brief 程序0正向刺激时阳极开关寄存器 (地址0x3D)
// */
//#define LMP1616X1_REG_3D_PROG0_STIM_ON0        (0x3D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_on0_0 : 16;          /**< [15:0] DANSEL_p[15:0]，程序0正向刺激时阳极开关，默认值0x0 */
//    } bits;
//} lmp1616x1_prog0_stim_on0_t;
//
///**
// * @brief 程序0正向刺激时阴极开关寄存器 (地址0x3E)
// */
//#define LMP1616X1_REG_3E_PROG0_STIM_ON1        (0x3E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_on1_0 : 16;          /**< [15:0] DCASEL_p[15:0]，程序0正向刺激时阴极开关，默认值0x0 */
//    } bits;
//} lmp1616x1_prog0_stim_on1_t;
//
///**
// * @brief 程序1正向刺激时阳极开关寄存器 (地址0x3F)
// */
//#define LMP1616X1_REG_3F_PROG1_STIM_ON0        (0x3F)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_on0_1 : 16;          /**< [15:0] DANSEL_n[15:0]，程序1正向刺激时阳极开关，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_stim_on0_t;
//
///**
// * @brief 程序1正向刺激时阴极开关寄存器 (地址0x40)
// */
//#define LMP1616X1_REG_40_PROG1_STIM_ON1        (0x40)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_on1_1 : 16;          /**< [15:0] DCASEL_n[15:0]，程序1正向刺激时阴极开关，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_stim_on1_t;
//
///**
// * @brief 程序2正向刺激时阳极开关寄存器 (地址0x41)
// */
//#define LMP1616X1_REG_41_PROG2_STIM_ON0        (0x41)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_on0_2 : 16;          /**< [15:0] DANSEL_p[15:0]，程序2正向刺激时阳极开关，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_stim_on0_t;
//
///**
// * @brief 程序2正向刺激时阴极开关寄存器 (地址0x42)
// */
//#define LMP1616X1_REG_42_PROG2_STIM_ON1        (0x42)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_on1_2 : 16;          /**< [15:0] DCASEL_p[15:0]，程序2正向刺激时阴极开关，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_stim_on1_t;
//
///**
// * @brief 程序3正向刺激时阳极开关寄存器 (地址0x43)
// */
//#define LMP1616X1_REG_43_PROG3_STIM_ON0        (0x43)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_on0_3 : 16;          /**< [15:0] DANSEL_p[15:0]，程序3正向刺激时阳极开关，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_stim_on0_t;
//
///**
// * @brief 程序3正向刺激时阴极开关寄存器 (地址0x44)
// */
//#define LMP1616X1_REG_44_PROG3_STIM_ON1        (0x44)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_on1_3 : 16;          /**< [15:0] DCASEL_n[15:0]，程序3正向刺激时阴极开关，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_stim_on1_t;
//
///*==============================================================================
// * IDAC设置寄存器 (0x45-0x46)
// *============================================================================*/
//
///**
// * @brief IDAC_SET0寄存器 (地址0x45)
// * @details 给模拟的档位信号低16位
// */
//#define LMP1616X1_REG_45_IDAC_SET0             (0x45)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t idac_set_low : 16;      /**< [15:0] 档位信号低16位，默认值0x0 */
//    } bits;
//} lmp1616x1_idac_set0_t;
//
///**
// * @brief IDAC_SET1寄存器 (地址0x46)
// * @details 给模拟的档位信号高16位
// */
//#define LMP1616X1_REG_46_IDAC_SET1             (0x46)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t idac_set_high : 16;     /**< [15:0] 档位信号高16位，默认值0x0 */
//    } bits;
//} lmp1616x1_idac_set1_t;
//
///*==============================================================================
// * 程序1-3时序寄存器 (0x47-0x55)
// *============================================================================*/
//
///**
// * @brief 程序1的Tphase1寄存器 (地址0x47)
// */
//#define LMP1616X1_REG_47_PROG1_TPHASE1         (0x47)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase1_1 : 12;         /**< [11:0] 程序1的正脉冲时钟周期数，默认值0x0 */
//        uint16_t offset_trim_low : 4;    /**< [15:12] offset_trim[3:0]，给模拟用的trim信号，默认只有trim[1]为1，默认值0x2 */
//    } bits;
//} lmp1616x1_prog1_tphase1_t;
//
///**
// * @brief 程序1的Tphase2寄存器 (地址0x48)
// */
//#define LMP1616X1_REG_48_PROG1_TPHASE2         (0x48)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase2_1 : 12;         /**< [11:0] 程序1的Tphase2时钟周期数，默认值0x0 */
//        uint16_t offset_trim_high : 4;   /**< [15:12] offset_trim[7:4]，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_tphase2_t;
//
///**
// * @brief 程序1的Tphase3寄存器 (地址0x49)
// */
//#define LMP1616X1_REG_49_PROG1_TPHASE3         (0x49)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase3_1 : 12;         /**< [11:0] 程序1的Tphase3时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_tphase3_t;
//
///**
// * @brief 程序1的Tinterphase寄存器 (地址0x4A)
// */
//#define LMP1616X1_REG_4A_PROG1_TINTERPHASE     (0x4A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterphase1_1 : 8;     /**< [7:0] Tinterphase1_1，数值表示多少个时钟周期，默认值0x0 */
//        uint16_t tinterphase2_1 : 8;     /**< [15:8] Tinterphase2_1，数值表示多少个时钟周期，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_tinterphase_t;
//
///**
// * @brief 程序1的Tinterpulse寄存器 (地址0x4B)
// */
//#define LMP1616X1_REG_4B_PROG1_TINTERPULSE     (0x4B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterpulse_1 : 16;     /**< [15:0] Tphase3与下一周期之间的延迟时钟周期数，默认值0x0 */
//    } bits;
//} lmp1616x1_prog1_tinterpulse_t;
//
///**
// * @brief 程序2的Tphase1寄存器 (地址0x4C)
// */
//#define LMP1616X1_REG_4C_PROG2_TPHASE1         (0x4C)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase1_2 : 12;         /**< [11:0] 程序2的正脉冲时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_tphase1_t;
//
///**
// * @brief 程序2的Tphase2寄存器 (地址0x4D)
// */
//#define LMP1616X1_REG_4D_PROG2_TPHASE2         (0x4D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase2_2 : 12;         /**< [11:0] 程序2的Tphase2时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_tphase2_t;
//
///**
// * @brief 程序2的Tphase3寄存器 (地址0x4E)
// */
//#define LMP1616X1_REG_4E_PROG2_TPHASE3         (0x4E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase3_2 : 12;         /**< [11:0] 程序2的Tphase3时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_tphase3_t;
//
///**
// * @brief 程序2的Tinterphase寄存器 (地址0x4F)
// */
//#define LMP1616X1_REG_4F_PROG2_TINTERPHASE     (0x4F)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterphase1_2 : 8;     /**< [7:0] Tinterphase1_2，数值表示多少个时钟周期，默认值0x0 */
//        uint16_t tinterphase2_2 : 8;     /**< [15:8] Tinterphase2_2，数值表示多少个时钟周期，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_tinterphase_t;
//
///**
// * @brief 程序2的Tinterpulse寄存器 (地址0x50)
// */
//#define LMP1616X1_REG_50_PROG2_TINTERPULSE     (0x50)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterpulse_2 : 16;     /**< [15:0] Tphase3与下一周期之间的延迟时钟周期数，默认值0x0 */
//    } bits;
//} lmp1616x1_prog2_tinterpulse_t;
//
///**
// * @brief 程序3的Tphase1寄存器 (地址0x51)
// */
//#define LMP1616X1_REG_51_PROG3_TPHASE1         (0x51)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase1_3 : 12;         /**< [11:0] 程序3的正脉冲时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_tphase1_t;
//
///**
// * @brief 程序3的Tphase2寄存器 (地址0x52)
// */
//#define LMP1616X1_REG_52_PROG3_TPHASE2         (0x52)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase2_3 : 12;         /**< [11:0] 程序3的Tphase2时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_tphase2_t;
//
///**
// * @brief 程序3的Tphase3寄存器 (地址0x53)
// */
//#define LMP1616X1_REG_53_PROG3_TPHASE3         (0x53)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase3_3 : 12;         /**< [11:0] 程序3的Tphase3时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_tphase3_t;
//
///**
// * @brief 程序3的Tinterphase寄存器 (地址0x54)
// */
//#define LMP1616X1_REG_54_PROG3_TINTERPHASE     (0x54)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterphase1_3 : 8;     /**< [7:0] Tinterphase1_3，数值表示多少个时钟周期，默认值0x0 */
//        uint16_t tinterphase2_3 : 8;     /**< [15:8] Tinterphase2_3，数值表示多少个时钟周期，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_tinterphase_t;
//
///**
// * @brief 程序3的Tinterpulse寄存器 (地址0x55)
// */
//#define LMP1616X1_REG_55_PROG3_TINTERPULSE     (0x55)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterpulse_3 : 16;     /**< [15:0] Tphase3与下一周期之间的延迟时钟周期数，默认值0x0 */
//    } bits;
//} lmp1616x1_prog3_tinterpulse_t;
//
///*==============================================================================
// * 软启动增量DAC寄存器 (0x56-0x59)
// *============================================================================*/
//
///**
// * @brief softstart_inc_dac_0寄存器 (地址0x56)
// * @details 第1-4个DAC软启动增量值
// */
//#define LMP1616X1_REG_56_SOFTSTART_INC_DAC_0   (0x56)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t softstart_inc_dac0 : 4; /**< [3:0] 第一个DAC软启动增量值，默认值0x0 */
//        uint16_t softstart_inc_dac1 : 4; /**< [7:4] 第二个DAC软启动增量值，默认值0x0 */
//        uint16_t softstart_inc_dac2 : 4; /**< [11:8] 第三个DAC软启动增量值，默认值0x0 */
//        uint16_t softstart_inc_dac3 : 4; /**< [15:12] 第四个DAC软启动增量值，默认值0x0 */
//    } bits;
//} lmp1616x1_softstart_inc_dac_0_t;
//
///**
// * @brief softstart_inc_dac_1寄存器 (地址0x57)
// * @details 第5-8个DAC软启动增量值
// */
//#define LMP1616X1_REG_57_SOFTSTART_INC_DAC_1   (0x57)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t softstart_inc_dac4 : 4; /**< [3:0] 第五个DAC软启动增量值，默认值0x0 */
//        uint16_t softstart_inc_dac5 : 4; /**< [7:4] 第六个DAC软启动增量值，默认值0x0 */
//        uint16_t softstart_inc_dac6 : 4; /**< [11:8] 第七个DAC软启动增量值，默认值0x0 */
//        uint16_t softstart_inc_dac7 : 4; /**< [15:12] 第八个DAC软启动增量值，默认值0x0 */
//    } bits;
//} lmp1616x1_softstart_inc_dac_1_t;
//
///**
// * @brief softstart_inc_dac_2寄存器 (地址0x58)
// * @details 第9-12个DAC软启动增量值
// */
//#define LMP1616X1_REG_58_SOFTSTART_INC_DAC_2   (0x58)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t softstart_inc_dac8 : 4;  /**< [3:0] 第九个DAC软启动增量值，默认值0x0 */
//        uint16_t softstart_inc_dac9 : 4;  /**< [7:4] 第十个DAC软启动增量值，默认值0x0 */
//        uint16_t softstart_inc_dac10 : 4; /**< [11:8] 第十一个DAC软启动增量值，默认值0x0 */
//        uint16_t softstart_inc_dac11 : 4; /**< [15:12] 第十二个DAC软启动增量值，默认值0x0 */
//    } bits;
//} lmp1616x1_softstart_inc_dac_2_t;
//
///**
// * @brief softstart_inc_dac_3寄存器 (地址0x59)
// * @details 第13-16个DAC软启动增量值
// */
//#define LMP1616X1_REG_59_SOFTSTART_INC_DAC_3   (0x59)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t softstart_inc_dac12 : 4; /**< [3:0] 第十三个DAC软启动增量值，默认值0x0 */
//        uint16_t softstart_inc_dac13 : 4; /**< [7:4] 第十四个DAC软启动增量值，默认值0x0 */
//        uint16_t softstart_inc_dac14 : 4; /**< [11:8] 第十五个DAC软启动增量值，默认值0x0 */
//        uint16_t softstart_inc_dac15 : 4; /**< [15:12] 第十六个DAC软启动增量值，默认值0x0 */
//    } bits;
//} lmp1616x1_softstart_inc_dac_3_t;
//
///*==============================================================================
// * 修调数据和控制寄存器 (0x60-0x67)
// *============================================================================*/
//
///**
// * @brief trimdata_ctrl0寄存器 (地址0x60)
// * @details 修调数据控制0
// */
//#define LMP1616X1_REG_60_TRIMDATA_CTRL0         (0x60)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t trim_ref : 1;           /**< [0] 1：电压恒正常，0：电压受模拟电路控制，默认值0x0 */
//        uint16_t trim_duvlo : 1;         /**< [1] 0：Vrecp低于标定电压停止刺激，1：Vrecp低于标定电压复位芯片，默认值0x0 */
//        uint16_t cmd_rst : 1;            /**< [2] 数字电路寄存器软复位信号，写1写0完成复位，默认值0x0 */
//        uint16_t test_mode : 1;          /**< [3] 1为测试模式PO<0>与VRECP连接，0为正常工作，默认值0x0 */
//        uint16_t vrecp_trim : 4;         /**< [7:4] Vrecp_trim，设定和Vrecp的比较电压生成DUVLO信号，0x0:1V,0x1:1.6V,0x2:2.2V,0x3:2.8V,0x4:3.4V...0xF:10V，默认值0xC */
//        uint16_t trim_adc : 8;           /**< [15:8] trim_adc -> {ISEL_ADC[1:0], ISEL_DAC[1:0], ISEL_OPA, IMP_GAIN[1:0], PDHO_SEL}，默认值0x0 */
//    } bits;
//} lmp1616x1_trimdata_ctrl0_t;
//
///**
// * @brief digital version寄存器 (地址0x61)
// * @details 数字版本号，只读
// */
//#define LMP1616X1_REG_61_DIGITAL_VERSION      (0x61)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t digital_version : 16;   /**< [15:0] 数字版本号，只读，默认值0x2601 */
//    } bits;
//} lmp1616x1_digital_version_t;
//
///**
// * @brief stim_para_crc寄存器 (地址0x62)
// * @details 刺激参数CRC值，只读
// */
//#define LMP1616X1_REG_62_STIM_PARA_CRC         (0x62)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_para_crc : 16;     /**< [15:0] 刺激参数CRC值，只读，默认值0x0 */
//    } bits;
//} lmp1616x1_stim_para_crc_t;
//
///**
// * @brief trimdata_ctrl3寄存器 (地址0x63)
// * @details 保留寄存器
// */
//#define LMP1616X1_REG_63_TRIMDATA_CTRL3        (0x63)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t reserved : 16;          /**< [15:0] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_trimdata_ctrl3_t;
//
///**
// * @brief ldo_trim寄存器 (地址0x64)
// * @details LDO修调寄存器
// */
//#define LMP1616X1_REG_64_LDO_TRIM              (0x64)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t trim_osc : 4;           /**< [3:0] TRIM_OSC，默认值0x0 */
//        uint16_t temp_trim_ptati : 2;    /**< [5:4] TEMP_TRIM_PTATI，默认值0x0 */
//        uint16_t temp_trim_ibi : 1;      /**< [6] TEMP_TRIM_IBI，默认值0x0 */
//        uint16_t temp_trim_ctati : 1;    /**< [7] TEMP_TRIM_CTATI，默认值0x0 */
//        uint16_t limit_en : 1;           /**< [8] LIMIT_EN，默认值0x0 */
//        uint16_t stim_si : 2;            /**< [10:9] STIM_SI，默认值0x0 */
//        uint16_t adc_trim_osc : 4;       /**< [14:11] ADC_TRIM_OSC，默认值0x0 */
//        uint16_t adc_testmode : 1;       /**< [15] ADC_TESTMODE，数字模块输出，默认值0x0 */
//    } bits;
//} lmp1616x1_ldo_trim_t;
//
///**
// * @brief volt_delay_sample寄存器 (地址0x65)
// * @details 电压测量延迟周期寄存器
// */
//#define LMP1616X1_REG_65_VOLT_DELAY_SAMPLE     (0x65)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t volt_delay_sample : 16; /**< [15:0] 延迟多少个刺激周期才开启电压测量，0无延时，最大65535，默认值0x0 */
//    } bits;
//} lmp1616x1_volt_delay_sample_t;
//
///**
// * @brief reserved寄存器 (地址0x66)
// * @details 保留寄存器
// */
//#define LMP1616X1_REG_66_RESERVED              (0x66)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t reserved : 16;          /**< [15:0] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_reserved_66_t;
//
///**
// * @brief reserved寄存器 (地址0x67)
// * @details 保留寄存器
// */
//#define LMP1616X1_REG_67_RESERVED              (0x67)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t reserved : 16;          /**< [15:0] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_reserved_67_t;
//
///*==============================================================================
// * 控制和状态寄存器 (0x68-0x69)
// *============================================================================*/
//
///**
// * @brief ctrlreg0寄存器 (地址0x68)
// * @details 控制寄存器0
// */
//#define LMP1616X1_REG_68_CTRLREG0              (0x68)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t start_en : 3;           /**< [2:0] 测量使能开关，[0]：温度测量开关，[1]：电压测量开关，[2]：阻抗测量开关，默认值0x0 */
//        uint16_t program_sel : 2;        /**< [4:3] 延迟测量程序选择，0,1,2,3表示相应的刺激程序，默认值0x0 */
//        uint16_t imp_channel_sel : 4;    /**< [8:5] 阻抗测量通道选择，选择测量哪个通道的阻抗，默认值0x0 */
//        uint16_t imp_test_trim_osc : 4;  /**< [12:9] 阻抗测量时使用的时钟trim_osc，默认值0x0 */
//        uint16_t current_measure : 2;    /**< [14:13] b01 表示测量A电流源电流， 2’b10表示测量B电流源电流。2‘b00表示不测量， 不能同时测量AB电流源电流，即不能设置为2’b11 */
//        uint16_t reserved : 1;           /**< [15:15] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_ctrlreg0_t;
//
///**
// * @brief ctrlreg1寄存器 (地址0x69)
// * @details 控制寄存器1，只读
// */
//#define LMP1616X1_REG_69_CTRLREG1              (0x69)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_done : 1;          /**< [0] 刺激完成标志，只读，默认值0x0 */
//        uint16_t temp_done : 1;          /**< [1] 温度测量完成，只读，默认值0x0 */
//        uint16_t vdd_sense_done : 1;     /**< [2] 电压测量完成，只读，默认值0x0 */
//        uint16_t imp_done : 1;           /**< [3] 阻抗测量完成，只读，默认值0x0 */
//        uint16_t duvlo : 1;              /**< [4] DUVLO：0欠压（电压小于5V），1正常，只读，默认值0x0 */
//        uint16_t reserved : 11;          /**< [15:5] 保留位，只读，默认值0x0 */
//    } bits;
//} lmp1616x1_ctrlreg1_t;
//
///*==============================================================================
// * 阻抗测量数据寄存器 (0x6A-0x71, 0x75-0x7C)
// *============================================================================*/
//
///**
// * @brief 通用测量数据类型
// */
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t data : 12;              /**< [11:0] 测量数据 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位 */
//    } bits;
//} lmp1616x1_meas_data_t;
//
///**
// * @brief impedance_data1寄存器 (地址0x6A)
// */
//#define LMP1616X1_REG_6A_IMPEDANCE_DATA1       (0x6A)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data1_t;
//
///**
// * @brief impedance_data2寄存器 (地址0x6B)
// */
//#define LMP1616X1_REG_6B_IMPEDANCE_DATA2       (0x6B)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data2_t;
//
///**
// * @brief impedance_data3寄存器 (地址0x6C)
// */
//#define LMP1616X1_REG_6C_IMPEDANCE_DATA3       (0x6C)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data3_t;
//
///**
// * @brief impedance_data4寄存器 (地址0x6D)
// */
//#define LMP1616X1_REG_6D_IMPEDANCE_DATA4       (0x6D)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data4_t;
//
///**
// * @brief impedance_data5寄存器 (地址0x6E)
// */
//#define LMP1616X1_REG_6E_IMPEDANCE_DATA5       (0x6E)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data5_t;
//
///**
// * @brief impedance_data6寄存器 (地址0x6F)
// */
//#define LMP1616X1_REG_6F_IMPEDANCE_DATA6       (0x6F)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data6_t;
//
///**
// * @brief impedance_data7寄存器 (地址0x70)
// */
//#define LMP1616X1_REG_70_IMPEDANCE_DATA7       (0x70)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data7_t;
//
///**
// * @brief impedance_data8寄存器 (地址0x71)
// */
//#define LMP1616X1_REG_71_IMPEDANCE_DATA8       (0x71)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data8_t;
//
///*==============================================================================
// * VDD测量和温度测量寄存器 (0x72-0x74)
// *============================================================================*/
//
///**
// * @brief vdd_low寄存器 (地址0x72)
// * @details VDD刺激最低测量值
// */
//#define LMP1616X1_REG_72_VDD_LOW               (0x72)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t vdd_low : 12;           /**< [11:0] VDD刺激最低测量值，若无刺激vdd_low等于vdd_high，默认值0x0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_vdd_low_t;
//
///**
// * @brief vdd_high寄存器 (地址0x73)
// * @details VDD刺激最高测量值
// */
//#define LMP1616X1_REG_73_VDD_HIGH              (0x73)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t vdd_high : 12;          /**< [11:0] VDD刺激最高测量值，默认值0x0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_vdd_high_t;
//
///**
// * @brief temp_data寄存器 (地址0x74)
// * @details 温度测量数据寄存器
// */
//#define LMP1616X1_REG_74_TEMP_DATA             (0x74)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t temp_sync_data : 12;    /**< [11:0] 温度测量值，默认值0x0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_temp_data_t;
//
///*==============================================================================
// * 阻抗测量数据寄存器 (0x75-0x7C)
// *============================================================================*/
//
///**
// * @brief impedance_data9寄存器 (地址0x75)
// */
//#define LMP1616X1_REG_75_IMPEDANCE_DATA9       (0x75)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data9_t;
//
///**
// * @brief impedance_data10寄存器 (地址0x76)
// */
//#define LMP1616X1_REG_76_IMPEDANCE_DATA10      (0x76)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data10_t;
//
///**
// * @brief impedance_data11寄存器 (地址0x77)
// */
//#define LMP1616X1_REG_77_IMPEDANCE_DATA11      (0x77)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data11_t;
//
///**
// * @brief impedance_data12寄存器 (地址0x78)
// */
//#define LMP1616X1_REG_78_IMPEDANCE_DATA12      (0x78)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data12_t;
//
///**
// * @brief impedance_data13寄存器 (地址0x79)
// */
//#define LMP1616X1_REG_79_IMPEDANCE_DATA13      (0x79)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data13_t;
//
///**
// * @brief impedance_data14寄存器 (地址0x7A)
// */
//#define LMP1616X1_REG_7A_IMPEDANCE_DATA14      (0x7A)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data14_t;
//
///**
// * @brief impedance_data15寄存器 (地址0x7B)
// */
//#define LMP1616X1_REG_7B_IMPEDANCE_DATA15      (0x7B)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data15_t;
//
///**
// * @brief impedance_data16寄存器 (地址0x7C)
// */
//#define LMP1616X1_REG_7C_IMPEDANCE_DATA16      (0x7C)
//typedef lmp1616x1_meas_data_t lmp1616x1_impedance_data16_t;
//
///*==============================================================================
// * 电流测量数据寄存器 (0x7D-0x7E)
// *============================================================================*/
//
///**
// * @brief current_data_neg寄存器 (地址0x7D)
// * @details 负向刺激电流测量数据
// */
//#define LMP1616X1_REG_7D_CURRENT_DATA_NEG      (0x7D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t current_data_neg : 12;  /**< [11:0] 负向刺激电流中间脉宽时刻的数值，默认值0x0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_current_data_neg_t;
//
///**
// * @brief current_data_plus寄存器 (地址0x7E)
// * @details 正向刺激电流测量数据
// */
//#define LMP1616X1_REG_7E_CURRENT_DATA_PLUS     (0x7E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t current_data_plus : 12; /**< [11:0] 正向刺激电流中间脉宽时刻的数值，默认值0x0 */
//        uint16_t reserved : 4;           /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1616x1_current_data_plus_t;
//
///*==============================================================================
// * 寄存器地址范围
// *============================================================================*/
//#define LMP1616X1_REG_MAX                  (0x7E)
//
//#endif /* __LMP1616X1_REG_H__ */
