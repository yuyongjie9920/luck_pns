///**
// * @file lmp1602x1_reg.h
// * @brief LMP1602X1 寄存器头文件
// * @version 2.0
// * @date 2026-04-07
// *
// * 根据芯片寄存器列表整理
// */
//
//#ifndef __LMP1602X1_REG_H__
//#define __LMP1602X1_REG_H__
//
//#include <stdint.h>
//
///*==============================================================================
// * 刺激波形寄存器 (0x00-0x0F)
// *============================================================================*/
//
///**
// * @brief Tphase1寄存器 (地址0x00)
// * @details 数值表示正脉冲多少个时钟周期，此数值的设定与phase_config存在一定关系，
// *          即必须是当前phase中使用多少组DAC值的倍数。
// *          phase_config==0时，Tphase1最大值为0x400（1024个clk cycle）
// *          phase_config==1时，Tphase1最大值为0x800（2048个clk cycle）
// */
//#define LMP1602X1_REG_00_TPHASE1            (0x00)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase1 : 12;      /**< [11:0] 正脉冲时钟周期数，默认值0xD2 */
//        uint16_t reserved_12 : 1;   /**< [12] 保留位，默认值0x0 */
//        uint16_t reserved_13 : 3;   /**< [15:13] 保留位，默认为0 */
//    } bits;
//} lmp1602x1_tphase1_t;
//
///**
// * @brief Tphase2寄存器 (地址0x01)
// * @details 数值表示负脉冲多少个时钟周期，此数值的设定与phase_config存在一定关系，
// *          即必须是当前phase中使用多少组DAC值的倍数。
// */
//#define LMP1602X1_REG_01_TPHASE2            (0x01)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase2 : 12;      /**< [11:0] 负脉冲时钟周期数，默认值0xD2 */
//        uint16_t reserved_12 : 1;   /**< [12] 保留位，默认值0x0 */
//        uint16_t demod_triml : 3;   /**< [15:13] DEMOD_TRIML[2:0]，给解调器模块做修调用的配置寄存器，默认值3'b000 */
//    } bits;
//} lmp1602x1_tphase2_t;
//
///**
// * @brief Tinterphase寄存器 (地址0x02)
// * @details 数值表示多少个时钟周期
// */
//#define LMP1602X1_REG_02_TINTERPHASE        (0x02)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterphase1 : 8;   /**< [7:0] Tinterphase1，数值表示多少个时钟周期，默认值0xD2 */
//        uint16_t tinterphase2 : 8;   /**< [15:8] Tinterphase2，数值表示多少个时钟周期，默认值0xD2 */
//    } bits;
//} lmp1602x1_tinterphase_t;
//
///**
// * @brief Tphase3寄存器 (地址0x03)
// * @details 数值表示电荷平衡阶段多少个时钟周期
// */
//#define LMP1602X1_REG_03_TPHASE3            (0x03)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase3 : 12;      /**< [11:0] 电荷平衡阶段时钟周期数，默认值0xA0 */
//        uint16_t reserved : 4;      /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_tphase3_t;
//
///**
// * @brief Tinterpulse寄存器 (地址0x04)
// * @details Tphase3与下一周期之间的延迟多少个时钟周期
// */
//#define LMP1602X1_REG_04_TINTERPULSE        (0x04)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterpulse : 16;   /**< [15:0] Tphase3与下一周期之间的延迟时钟周期数，默认值0x0 */
//    } bits;
//} lmp1602x1_tinterpulse_t;
//
///**
// * @brief Tdefine寄存器 (地址0x05)
// * @details 刺激时序定义寄存器
// */
//#define LMP1602X1_REG_05_TDEFINE            (0x05)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t pulse_cnt : 8;      /**< [7:0] pulse_cnt: 刺激波形重复次数，默认值0x0 */
//        uint16_t stim_td2 : 2;       /**< [9:8] stim_td2: 保留位，修改为固定的1 clock cycle的延时，默认值0x0 */
//        uint16_t stim_td3 : 6;       /**< [15:10] stim_td3: 电荷平衡结束与STIM_EN\STIM_ON\DAC_ENI关闭之间的延迟时间，典型值32us，默认值0x1F */
//    } bits;
//} lmp1602x1_tdefine_t;
//
///**
// * @brief STIM_MODE寄存器 (地址0x06)
// * @details 刺激模式控制寄存器
// */
//#define LMP1602X1_REG_06_STIM_MODE          (0x06)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_start : 1;             /**< [0] stim_start: 给刺激波形模块的开始刺激信号，默认值0x0 */
//        uint16_t para_vld : 1;               /**< [1] para_vld: 给刺激波形模块的配置参数有效信号，更新参数使用，默认值0x0 */
//        uint16_t stim_stop : 1;              /**< [2] stim_stop: 1表示停止刺激，0表示不停止，默认值0x0 */
//        uint16_t stim_mode : 2;              /**< [4:3] stim_mode: 2'b00:简单模式，2'b01:任意模式，2'b10:突发刺激模式，默认值0x0 */
//        uint16_t stim_low_power : 1;         /**< [5] stim_low_power，默认值0x0 */
//        uint16_t program_cnt : 2;            /**< [7:6] program_cnt: 刺激程序数量，0:一个刺激程序，1:两个刺激程序，默认值0x0 */
//        uint16_t stim_clk_div_value : 6;     /**< [13:8] stim_clk_div_value: 刺激时钟分频，0不分频，1:2分频，n:n+1分频，最大62，默认值0x0 */
//        uint16_t stim_auto_stop_en : 1;      /**< [14] 刺激自动停止开关，0:关闭，1:开启，默认值0x0 */
//        uint16_t ab_alt_stim_en : 1;            /**< [15] AB交替开关*/
//    } bits;
//} lmp1602x1_stim_mode_t;
//
///**
// * @brief burst_cnt寄存器 (地址0x07)
// * @details 突发计数/PWM模式寄存器
// */
//#define LMP1602X1_REG_07_PWM_MODE         (0x07)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t pwm_power_dly : 13;  /**< [12:0] pwm_power_dly[12:0]，占空比供能时，检测到供能之后多少个RC时钟周期后再进入正向刺激，默认值0x0 */
//        uint16_t pwm_mode : 1;        /**< [13] pwm_mode: 1表示占空比供能模式，0表示常供能模式，默认值0x0 */
//        uint16_t reserved : 2;        /**< [15:14] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_pwm_mode_t;
//
///**
// * @brief Tinterburst寄存器 (地址0x08)
// * @details 突发模式下面刺激簇之间的间隔时间
// */
//#define LMP1602X1_REG_08_TINTERBURST        (0x08)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterburst : 16;    /**< [15:0] Tinterburst[15:0]，突发模式下面刺激簇之间的间隔时间，默认值0x0 */
//    } bits;
//} lmp1602x1_tinterburst_t;
//
///**
// * @brief softstart_cycle寄存器 (地址0x09)
// * @details 软启动周期寄存器
// */
//#define LMP1602X1_REG_09_SOFTSTART_CYCLE    (0x09)  //保留 未使用
//
//
///**
// * @brief stim_auto_stop_cycle寄存器 (地址0x0A)
// * @details 自动停止周期寄存器
// */
//#define LMP1602X1_REG_0A_AUTO_STOP_CYCLE    (0x0A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_auto_stop_cycle : 16; /**< [15:0] 每位数值表示的是4个刺激周期， 例如如果是10KHz的高频刺激（刺激周期100us），此寄存器配置的是最大值65535， 那么表示刺激程序跑完65535*4=26214个刺激，大约26s之后会停止刺激。 */
//    } bits;
//} lmp1602x1_auto_stop_cycle_t;
//
///**
// * @brief 软启动控制寄存器 (地址0x0B)
// * @details 所有程序软启动控制
// */
//#define LMP1602X1_REG_0B_SOFTSTART_CTRL     (0x0B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t softstart_step : 10;       /**< [9:0] softstart_step: 有效值1~1023，表示经过多少个刺激周期后触发一次softstart的increase value，默认值0x0 */
//        uint16_t softstart_inc_value : 5;   /**< [14:10] 有效值1~31, 表示每次增加的DAC value */
//        uint16_t softstart_en : 1;          /**< [15] softstart_en: 所有程序软启动使能，应用于所有程序组，默认值0x0 */
//    } bits;
//} lmp1602x1_softstart_ctrl_t;
//
///**
// * @brief adc_diff_cfg寄存器 (地址0x0C)
// * @details ADC差分配置寄存器
// */
//#define LMP1602X1_REG_0C_ADC_DIFF_CFG      (0x0C)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t rfid_resp_delay_cnt : 11; /**< [10:0] rfid_resp_delay_cnt[10:0]: 控制芯片收到demod解码后多久返回mod给程控仪，默认值0x0 */
//        uint16_t sel_demod : 1;        /**< [11] sel_demod: 0选择原始射频解调信号，1选择数字滤波后的射频解调信号，默认值0x0 */
//        uint16_t pdelt : 4;            /**< [15:12] pdelt: 设置数字滤波器波宽度，正常为2,3，设置其他值为异常，配合bit11为1时滤波射频信号，默认值0x0 */
//    } bits;
//} lmp1602x1_adc_diff_cfg_t;
//
///**
// * @brief rfid_cfg1寄存器 (地址0x0D)
// * @details RFID RX端配置1
// */
//#define LMP1602X1_REG_0D_RFID_CFG1         (0x0D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tari_min : 8;         /**< [7:0] TariMin，默认值0x03 */
//        uint16_t tari_max : 8;         /**< [15:8] TariMax，默认值0x32 */
//    } bits;
//} lmp1602x1_rfid_cfg1_t;
//
///**
// * @brief rfid_cfg2寄存器 (地址0x0E)
// * @details RFID RX端配置2
// */
//#define LMP1602X1_REG_0E_RFID_CFG2         (0x0E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t delta : 8;            /**< [7:0] Delta，默认值0x02 */
//        uint16_t rt_max : 8;           /**< [15:8] RtMax，默认值0xFF */
//    } bits;
//} lmp1602x1_rfid_cfg2_t;
//
///**
// * @brief rfid_cfg3寄存器 (地址0x0F)
// * @details RFID RX端配置3
// */
//#define LMP1602X1_REG_0F_RFID_CFG3         (0x0F)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t demod_pon_cnt : 9;    /**< [8:0] demod_pon_cnt: 占空比功能时demod高电平多久认定为供能，以区别通讯时的Rtcal高电平时间，默认值0x80 */
//        uint16_t reserved_10_9 : 2;    /**< [10:9] 保留位，默认值0x0 */
//        uint16_t rfid_tx_data_period : 5; /**< [15:11] rfid_tx_data_period: 默认值0xA，由于半周期也必须是正整数，因此值只能是偶数，最大值0x1E(30) */
//    } bits;
//} lmp1602x1_rfid_cfg3_t;
//
///*==============================================================================
// * eFuse寄存器 (0x10-0x14)
// *============================================================================*/
//
///**
// * @brief efuse_w_data0寄存器 (地址0x10)
// * @details eFuse写数据[15:0]
// */
//#define LMP1602X1_REG_10_EFUSE_W_DATA0      (0x10)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_w_data : 16;    /**< [15:0] efuse_w_data[15:0]，默认值0x0 */
//    } bits;
//} lmp1602x1_efuse_w_data0_t;
//
///**
// * @brief efuse_w_data1寄存器 (地址0x11)
// * @details eFuse写数据[23:16]及控制
// */
//#define LMP1602X1_REG_11_EFUSE_W_DATA1      (0x11)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_w_data_16_23 : 8; /**< [7:0] efuse_w_data[23:16]，默认值0x0 */
//        uint16_t efuse_data_enable : 6; /**< [13:8] efuse_data_enable[5:0]，每bit为1表示databus的4bit数据有效，需要写入efuse，默认值0x0 */
//        uint16_t reversed : 1;         /**< [14] 保留位，默认值0x0 */
//        uint16_t efuse_data_valid : 1; /**< [15] efuse_data_valid: 0->1变化表示触发一次efuse写操作，默认值0x0 */
//    } bits;
//} lmp1602x1_efuse_w_data1_t;
//
///**
// * @brief efuse_r_data0寄存器 (地址0x12)
// * @details eFuse读数据[15:0]
// */
//#define LMP1602X1_REG_12_EFUSE_R_DATA0      (0x12)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_r_data : 16;    /**< [15:0] efuse_r_data[15:0]，只读，默认值0x0 */
//    } bits;
//} lmp1602x1_efuse_r_data0_t;
//
///**
// * @brief efuse_r_data1寄存器 (地址0x13)
// * @details eFuse读数据[23:16]及状态
// */
//#define LMP1602X1_REG_13_EFUSE_R_DATA1      (0x13)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_r_data_16_23 : 8; /**< [7:0] efuse_r_data[23:16]，只读，默认值0x0 */
//        uint16_t reversed : 7;         /**< [14:8] 保留位，默认值0x0 */
//        uint16_t efuse_auto_read_done : 1; /**< [15] efuse_auto_read_done: 1表示上电后自动读efuse执行完毕，0表示未执行完毕，只有为1时才可以开始efuse烧写操作，只读，默认值0x0 */
//    } bits;
//} lmp1602x1_efuse_r_data1_t;
//
///**
// * @brief efuse_tburn寄存器 (地址0x14)
// * @details eFuse烧写时间配置
// */
//#define LMP1602X1_REG_14_EFUSE_TBURN        (0x14)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t efuse_tburn : 15;     /**< [14:0] efuse_tburn: 1bit的烧写时间，单位us，如15'd1000为1ms，默认值0x0 */
//        uint16_t efuse_burn_clk_en : 1; /**< [15] efuse_burn_clk_en: 寄存器控制efuse gating时钟，为1时常开，为0时内部逻辑控制关时钟，默认值0x0 */
//    } bits;
//} lmp1602x1_efuse_tburn_t;
//
//
///*==============================================================================
// * 程序0 A电流源正向刺激DAC值寄存器 (地址0x15)
// *============================================================================*/
//#define LMP1602X1_REG_15_AMP_DAC_A_P_0        (0x15)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_a_p_0 : 10; /**< [9:0] 电极0 DAC正向值，默认值0x0 */
//        uint16_t reserved : 5;             /**< [14:10] 保留位，默认值0x0 */
//        uint16_t dual_a_en : 1;            /**< [15] A电流源使能信号，0：不使能A电流源（没有0至7电极），1：使能A电流源（使能0至7电极） */
//    } bits;
//} lmp1602x1_amp_dac_a_p_0_t;
//
///*==============================================================================
// * 程序0 A电流源负向刺激DAC值寄存器 (地址0x16)
// *============================================================================*/
//#define LMP1602X1_REG_16_AMP_DAC_A_N_0        (0x16)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_a_n_0 : 10; /**< [9:0] 电极0 DAC负向值，默认值0x0 */
//        uint16_t reserved : 6;             /**< [15:10] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_amp_dac_a_n_0_t;
//
///*==============================================================================
// * 程序0 B电流源正向刺激DAC值寄存器 (地址0x17)
// *============================================================================*/
//#define LMP1602X1_REG_17_AMP_DAC_B_P_0        (0x17)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_b_p_0 : 10; /**< [9:0] 电极8-15 DAC正向值，默认值0x0 */
//        uint16_t reserved : 5;             /**< [14:10] 保留位，默认值0x0 */
//        uint16_t dual_b_en : 1;            /**< [15] B电流源使能信号，0：不使能B电流源（没有8至15电极），1：使能B电流源（使能8至15电极） */
//    } bits;
//} lmp1602x1_amp_dac_b_p_0_t;
//
///*==============================================================================
// * 程序0 B电流源负向刺激DAC值寄存器 (地址0x18)
// *============================================================================*/
//#define LMP1602X1_REG_18_AMP_DAC_B_N_0        (0x18)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_b_n_0 : 10; /**< [9:0] 电极8-15 DAC负向值，默认值0x0 */
//        uint16_t reserved : 6;             /**< [15:10] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_amp_dac_b_n_0_t;
//
///*==============================================================================
// * 程序1 A电流源正向刺激DAC值寄存器 (地址0x19)
// *============================================================================*/
//#define LMP1602X1_REG_19_AMP_DAC_A_P_1        (0x19)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_a_p_1 : 10; /**< [9:0] 程序1电极0-7 DAC正向值，默认值0x0 */
//        uint16_t reserved : 6;             /**< [15:10] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_amp_dac_a_p_1_t;
//
///*==============================================================================
// * 程序1 A电流源负向刺激DAC值寄存器 (地址0x1A)
// *============================================================================*/
//#define LMP1602X1_REG_1A_AMP_DAC_A_N_1        (0x1A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_a_n_1 : 10; /**< [9:0] 程序1电极0-7 DAC负向值，默认值0x0 */
//        uint16_t reserved : 6;             /**< [15:10] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_amp_dac_a_n_1_t;
//
///*==============================================================================
// * 程序1 B电流源正向刺激DAC值寄存器 (地址0x1B)
// *============================================================================*/
//#define LMP1602X1_REG_1B_AMP_DAC_B_P_1        (0x1B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_b_p_1 : 10; /**< [9:0] 程序1电极8-15 DAC正向值，默认值0x0 */
//        uint16_t reserved : 6;             /**< [15:10] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_amp_dac_b_p_1_t;
//
///*==============================================================================
// * 程序1 B电流源负向刺激DAC值寄存器 (地址0x1C)
// *============================================================================*/
//#define LMP1602X1_REG_1C_AMP_DAC_B_N_1        (0x1C)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t amplitude_dac_b_n_1 : 10; /**< [9:0] 程序1电极8-15 DAC负向值，默认值0x0 */
//        uint16_t reserved : 6;             /**< [15:10] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_amp_dac_b_n_1_t;
//
///*==============================================================================
// * STIM_ON寄存器 (地址0x1D-0x20)
// * 说明：{DCASEL, DANSEL} 阳极和阴极开关
// *============================================================================*/
//#define LMP1602X1_REG_1D_STIM_ON_A_0          (0x1D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_on_a_0 : 16;         /**< [15:0] 程序0 A电流源对应的正向刺激周期内阳极和阴极开关，默认值0x0 */
//    } bits;
//} lmp1602x1_stim_on_a_0_t;
//
//#define LMP1602X1_REG_1E_STIM_ON_B_0          (0x1E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_on_b_0 : 16;         /**< [15:0] 程序0 B电流源对应的正向刺激周期内阳极和阴极开关，默认值0x0 */
//    } bits;
//} lmp1602x1_stim_on_b_0_t;
//
//#define LMP1602X1_REG_1F_STIM_ON_A_1          (0x1F)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_on_a_1 : 16;         /**< [15:0] 程序1 A电流源对应的正向刺激周期内阳极和阴极开关，默认值0x0 */
//    } bits;
//} lmp1602x1_stim_on_a_1_t;
//
//#define LMP1602X1_REG_20_STIM_ON_B_1          (0x20)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_on_b_1 : 16;         /**< [15:0] 程序1 B电流源对应的正向刺激周期内阳极和阴极开关，默认值0x0 */
//    } bits;
//} lmp1602x1_stim_on_b_1_t;
//
///*==============================================================================
// * 程序0 B电流源时序寄存器 (地址0x21-0x27)
// * 说明：B电流源支持简单刺激和爆发刺激
// *============================================================================*/
//#define LMP1602X1_REG_21_TPHASE1_B_0          (0x21)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase1 : 12;         /**< [11:0] 程序0 B电流源的正脉冲时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;             /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_tphase1_b_0_t;
//
//#define LMP1602X1_REG_22_TPHASE2_B_0          (0x22)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase2 : 12;         /**< [11:0] 程序0 B电流源Tphase2时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;             /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_tphase2_b_0_t;
//
//#define LMP1602X1_REG_23_TPHASE3_B_0          (0x23)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase3 : 12;         /**< [11:0] 程序0 B电流源Tphase3时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;             /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_tphase3_b_0_t;
//
//#define LMP1602X1_REG_24_TINTERPHASE_B_0      (0x24)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterphase1 : 8;     /**< [7:0] Tinterphase1：相间间隔时钟周期数，默认值0x0 */
//        uint16_t tinterphase2 : 8;     /**< [15:8] Tinterphase2：相间间隔时钟周期数，默认值0x0 */
//    } bits;
//} lmp1602x1_tinterphase_b_0_t;
//
//#define LMP1602X1_REG_25_TINTERPULSE_B_0      (0x25)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterpulse : 16;     /**< [15:0] Tphase3与下一周期之间的延迟时钟周期数，默认值0x0 */
//    } bits;
//} lmp1602x1_tinterpulse_b_0_t;
//
//#define LMP1602X1_REG_26_PULSE_CNT_B_0        (0x26)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t pulse_cnt : 8;        /**< [7:0] B电流源一个burst中的脉冲数（1-255），默认值0x0 */
//        uint16_t reserved : 8;             /**< [15:8] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_pulse_cnt_b_0_t;
//
//#define LMP1602X1_REG_27_TINTERBURST_B_0      (0x27)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterburst : 16;     /**< [15:0] B电流源突发模式刺激簇之间的间隔时间，默认值0x0 */
//    } bits;
//} lmp1602x1_tinterburst_b_0_t;
//
///*==============================================================================
// * 程序1 A电流源时序寄存器 (地址0x28-0x2C)
// *============================================================================*/
//#define LMP1602X1_REG_28_TPHASE1_A_1          (0x28)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase1 : 12;         /**< [11:0] 程序1 A电流源的正脉冲时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;             /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_tphase1_a_1_t;
//
//#define LMP1602X1_REG_29_TPHASE2_A_1          (0x29)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase2 : 12;         /**< [11:0] 程序1 A电流源Tphase2时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;             /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_tphase2_a_1_t;
//
//#define LMP1602X1_REG_2A_TPHASE3_A_1          (0x2A)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase3 : 12;         /**< [11:0] 程序1 A电流源Tphase3时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;             /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_tphase3_a_1_t;
//
//#define LMP1602X1_REG_2B_TINTERPHASE_A_1      (0x2B)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterphase1 : 8;     /**< [7:0] Tinterphase1：相间间隔时钟周期数，默认值0x0 */
//        uint16_t tinterphase2 : 8;     /**< [15:8] Tinterphase2：相间间隔时钟周期数，默认值0x0 */
//    } bits;
//} lmp1602x1_tinterphase_a_1_t;
//
//#define LMP1602X1_REG_2C_TINTERPULSE_A_1      (0x2C)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterpulse : 16;     /**< [15:0] Tphase3与下一周期之间的延迟时钟周期数，默认值0x0 */
//    } bits;
//} lmp1602x1_tinterpulse_a_1_t;
//
///*==============================================================================
// * 程序1 B电流源时序寄存器 (地址0x2D-0x31)
// *============================================================================*/
//#define LMP1602X1_REG_2D_TPHASE1_B_1          (0x2D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase1 : 12;         /**< [11:0] 程序1 B电流源的正脉冲时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;             /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_tphase1_b_1_t;
//
//#define LMP1602X1_REG_2E_TPHASE2_B_1          (0x2E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase2 : 12;         /**< [11:0] 程序1 B电流源Tphase2时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;             /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_tphase2_b_1_t;
//
//#define LMP1602X1_REG_2F_TPHASE3_B_1          (0x2F)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tphase3 : 12;         /**< [11:0] 程序1 B电流源Tphase3时钟周期数，默认值0x0 */
//        uint16_t reserved : 4;             /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_tphase3_b_1_t;
//
//#define LMP1602X1_REG_30_TINTERPHASE_B_1      (0x30)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterphase1 : 8;     /**< [7:0] Tinterphase1：相间间隔时钟周期数，默认值0x0 */
//        uint16_t tinterphase2 : 8;     /**< [15:8] Tinterphase2：相间间隔时钟周期数，默认值0x0 */
//    } bits;
//} lmp1602x1_tinterphase_b_1_t;
//
//#define LMP1602X1_REG_31_TINTERPULSE_B_1      (0x31)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t tinterpulse : 16;     /**< [15:0] Tphase3与下一周期之间的延迟时钟周期数，默认值0x0 */
//    } bits;
//} lmp1602x1_tinterpulse_b_1_t;
//
///*==============================================================================
// * 保留寄存器区域 (0x31-0x5F)
// *============================================================================*/
///* 地址0x2E-0x5F均为保留寄存器，未使用 */
//
///*==============================================================================
// * 修调数据寄存器 (0x60-0x67)
// *============================================================================*/
//
///**
// * @brief trimdata_ctrl0寄存器 (地址0x60)
// * @details 修调数据控制0
// */
//#define LMP1602X1_REG_60_TRIMDATA_CTRL0     (0x60)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t trim_ref : 1;           /**< [0] trim_ref[0]: 1表示电压恒正常，0表示电压受模拟电路控制，默认值0x0 */
//        uint16_t trim_duvlo : 1;         /**< [1] trim_duvlo: 0表示Vrecp低于标定电压停止刺激，1表示复位芯片，默认值0x0 */
//        uint16_t cmd_rst : 1;            /**< [2] cmd_rst: 数字电路寄存器软复位信号，写1写0完成复位，默认值0x0 */
//        uint16_t test_mode : 1;          /**< [3] test_mode: 1为测试模式PO<0>与VRECP连接，0为正常工作，默认值0x0 */
//        uint16_t vrecp_trim : 4;         /**< [7:4] Vrecp_trim: 设定和Vrecp的比较电压生成DUVLO信号，0x0:1V,0x1:1.6V,0x2:2.2V,0x3:2.8V,0x4:3.4V...0xF:10V，默认值0xC */
//        uint16_t trim_adc : 8;           /**< [15:8] trim_adc -> {ISEL_ADC[1:0], ISEL_DAC[1:0], ISEL_OPA, IMP_GAIN[1:0], PDHO_SEL}，默认值0x0 */
//    } bits;
//} lmp1602x1_trimdata_ctrl0_t;
//
///**
// * @brief digital version寄存器 (地址0x61)
// * @details 数字版本号，只读
// */
//#define LMP1602X1_REG_61_DIGITAL_VERSION   (0x61)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t digital_version : 16;   /**< [15:0] 数字版本号，只读，默认值0x2601 */
//    } bits;
//} lmp1602x1_digital_version_t;
//
///**
// * @brief stim_para_crc寄存器 (地址0x62)
// * @details 刺激参数CRC值，只读
// */
//#define LMP1602X1_REG_62_STIM_PARA_CRC     (0x62)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_para_crc : 16;     /**< [15:0] 刺激参数CRC值，只读，默认值0x0 */
//    } bits;
//} lmp1602x1_stim_para_crc_t;
//
///**
// * @brief trimdata_ctrl3寄存器 (地址0x63)
// * @details 修调数据控制3（保留）
// */
//#define LMP1602X1_REG_63_TRIMDATA_CTRL3     (0x63)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t reserved0 : 8;              /**< [7:0] 保留位，默认值0x0 */
//        uint16_t rf_rx_en_always_on : 1;     /**< [8] X5删除trim_demod，射频接收使能控制，0：射频接收使能打开受数字电路控制，1：使能常开（模拟电路tie high），默认值0x0 */
//        uint16_t reserved1 : 3;              /**< [11:9] 保留位，默认值0x0 */
//        uint16_t rd_cfg_bak_addr : 2;        /**< [13:12] 读配置备份地址，默认值0x0 */
//        uint16_t reserved2 : 2;              /**< [15:14] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_trimdata_ctrl3_t;
//
///**
// * @brief ldo_trim寄存器 (地址0x64)
// * @details LDO修调寄存器
// */
//#define LMP1602X1_REG_64_LDO_TRIM          (0x64)
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
//} lmp1602x1_ldo_trim_t;
//
///**
// * @brief volt_delay_sample寄存器 (地址0x65)
// * @details 电压测量延迟周期寄存器
// */
//#define LMP1602X1_REG_65_VOLT_DELAY_SAMPLE (0x65)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t volt_delay_sample : 16; /**< [15:0] 延迟多少个刺激周期才开启电压测量，0无延时，最大65535，默认值0x0 */
//    } bits;
//} lmp1602x1_volt_delay_sample_t;
//
//
//#define LMP1602X1_REG_66_RESERVED          (0x66)  //未使用
//#define LMP1602X1_REG_67_RESERVED          (0x67) //未使用
//
//
///*==============================================================================
// * 控制和状态寄存器 (0x68-0x7E)
// *============================================================================*/
//
///**
// * @brief ctrlreg0寄存器 (地址0x68)
// * @details 控制寄存器0
// */
//#define LMP1602X1_REG_68_CTRLREG0              (0x68)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t start_en : 3;               /**< [2:0] 测量使能开关，[0]：温度测量开关，[1]：电压测量开关，[2]：阻抗测量开关。注：2024.9月之后版本必须单个下发，不支持同时下发，默认值0x0 */
//        uint16_t program_sel : 2;            /**< [4:3] 延迟测量程序选择，0-3表示相应的刺激程序。bit3选择电流源（0：A电流源，1：B电流源），bit4选择程序组，默认值0x0 */
//        uint16_t imp_channel_sel : 4;        /**< [8:5] 阻抗测量通道选择，选择测量哪个通道的阻抗，默认值0x0 */
//        uint16_t imp_test_trim_osc : 4;      /**< [12:9] 阻抗测量时使用的时钟trim_osc，start_en[2]为1时使用此trim_osc，否则使用寄存器0x64的trim_osc，默认值0x0 */
//        uint16_t current_measure : 2;        /**< [14:13]  电流测量选择，2'b01：测量A电流源电流，2'b10：测量B电流源电流，2'b00：不测量，不能设置为2'b11（不能同时测量AB电流源） */
//        uint16_t reserved : 1;               /**< [15]保留位 ，默认值0x0  */
//    } bits;
//} lmp1602x1_ctrlreg0_t;
//
///**
// * @brief ctrlreg1寄存器 (地址0x69)
// * @details 控制寄存器1，只读
// */
//#define LMP1602X1_REG_69_CTRLREG1          (0x69)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t stim_done : 1;          /**< [0] stim_done，刺激完成标志，只读，默认值0x1 */
//        uint16_t temp_done : 1;          /**< [1] temp_done，温度测量完成，只读，默认值0x0 */
//        uint16_t vdd_sense_done : 1;     /**< [2] vdd_sense_done，电压测量完成，只读，默认值0x0 */
//        uint16_t imp_done : 1;           /**< [3] imp_done，阻抗测量完成，只读，默认值0x0 */
//        uint16_t duvlo : 1;              /**< [4] DUVLO: 0欠压(电压小于5V)，1正常，只读，默认值0x0 */
//        uint16_t reserved : 11;          /**< [15:5] 保留位，只读，默认值0x0 */
//    } bits;
//} lmp1602x1_ctrlreg1_t;
//
///*==============================================================================
// * 测量数据寄存器 (0x6A-0x7E)
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
//} lmp1602x1_meas_data_t;
//
///**
// * @brief impedance_data1寄存器 (地址0x6A)
// * @details 阻抗数据1，只读
// */
//#define LMP1602X1_REG_6A_IMPEDANCE_DATA1   (0x6A)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data1_t;
//
///**
// * @brief impedance_data2寄存器 (地址0x6B)
// * @details 阻抗数据2，只读
// */
//#define LMP1602X1_REG_6B_IMPEDANCE_DATA2   (0x6B)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data2_t;
//
///**
// * @brief impedance_data3寄存器 (地址0x6C)
// * @details 阻抗数据3，只读
// */
//#define LMP1602X1_REG_6C_IMPEDANCE_DATA3   (0x6C)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data3_t;
//
///**
// * @brief impedance_data4寄存器 (地址0x6D)
// * @details 阻抗数据4，只读
// */
//#define LMP1602X1_REG_6D_IMPEDANCE_DATA4   (0x6D)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data4_t;
//
///**
// * @brief impedance_data5寄存器 (地址0x6E)
// * @details 阻抗数据5，只读
// */
//#define LMP1602X1_REG_6E_IMPEDANCE_DATA5   (0x6E)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data5_t;
//
///**
// * @brief impedance_data6寄存器 (地址0x6F)
// * @details 阻抗数据6，只读
// */
//#define LMP1602X1_REG_6F_IMPEDANCE_DATA6   (0x6F)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data6_t;
//
///**
// * @brief impedance_data7寄存器 (地址0x70)
// * @details 阻抗数据7，只读
// */
//#define LMP1602X1_REG_70_IMPEDANCE_DATA7   (0x70)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data7_t;
//
///**
// * @brief impedance_data8寄存器 (地址0x71)
// * @details 阻抗数据8，只读
// */
//#define LMP1602X1_REG_71_IMPEDANCE_DATA8   (0x71)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data8_t;
//
///*==============================================================================
// * VDD测量寄存器 (地址0x72-0x73)
// *============================================================================*/
//
///**
// * @brief VDD低压测量值寄存器 (地址0x72)
// * @details VDD刺激最低测量值，若无刺激vdd_low等于vdd_high
// */
//#define LMP1602X1_REG_72_VDD_LOW               (0x72)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t vdd_low : 12;               /**< [11:0] VDD刺激最低测量值，默认值0x0 */
//        uint16_t reserved : 4;               /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_vdd_low_t;
//
///**
// * @brief VDD高压测量值寄存器 (地址0x73)
// * @details VDD刺激最高测量值
// */
//#define LMP1602X1_REG_73_VDD_HIGH              (0x73)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t vdd_high : 12;              /**< [11:0] VDD刺激最高测量值，默认值0x0 */
//        uint16_t reserved : 4;               /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_vdd_high_t;
//
///*==============================================================================
// * 温度测量寄存器 (地址0x74)
// *============================================================================*/
//
///**
// * @brief 温度测量数据寄存器 (地址0x74)
// */
//#define LMP1602X1_REG_74_TEMP_DATA             (0x74)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t temp_sync_data : 12;        /**< [11:0] 温度测量值，默认值0x0 */
//        uint16_t reserved : 4;               /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_temp_data_t;
//
///**
// * @brief impedance_data9寄存器 (地址0x75)
// * @details 阻抗数据9，只读
// */
//#define LMP1602X1_REG_75_IMPEDANCE_DATA9   (0x75)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data9_t;
//
///**
// * @brief impedance_data10寄存器 (地址0x76)
// * @details 阻抗数据10，只读
// */
//#define LMP1602X1_REG_76_IMPEDANCE_DATA10  (0x76)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data10_t;
//
///**
// * @brief impedance_data11寄存器 (地址0x77)
// * @details 阻抗数据11，只读
// */
//#define LMP1602X1_REG_77_IMPEDANCE_DATA11  (0x77)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data11_t;
//
///**
// * @brief impedance_data12寄存器 (地址0x78)
// * @details 阻抗数据12，只读
// */
//#define LMP1602X1_REG_78_IMPEDANCE_DATA12  (0x78)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data12_t;
//
///**
// * @brief impedance_data13寄存器 (地址0x79)
// * @details 阻抗数据13，只读
// */
//#define LMP1602X1_REG_79_IMPEDANCE_DATA13  (0x79)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data13_t;
//
///**
// * @brief impedance_data14寄存器 (地址0x7A)
// * @details 阻抗数据14，只读
// */
//#define LMP1602X1_REG_7A_IMPEDANCE_DATA14  (0x7A)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data14_t;
//
///**
// * @brief impedance_data15寄存器 (地址0x7B)
// * @details 阻抗数据15，只读
// */
//#define LMP1602X1_REG_7B_IMPEDANCE_DATA15  (0x7B)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data15_t;
//
///**
// * @brief impedance_data16寄存器 (地址0x7C)
// * @details 阻抗数据16，只读
// */
//#define LMP1602X1_REG_7C_IMPEDANCE_DATA16  (0x7C)
//typedef lmp1602x1_meas_data_t lmp1602x1_impedance_data16_t;
//
///**
// * @brief 负向刺激电流测量数据寄存器 (地址0x7D)
// * @details 负向刺激电流中间脉宽时刻的数值
// */
//#define LMP1602X1_REG_7D_CURRENT_DATA_NEG     (0x7D)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t current_data_neg : 12;      /**< [11:0] 负向刺激电流中间脉宽时刻的数值，默认值0x0 */
//        uint16_t reserved : 4;               /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_current_data_neg_t;
//
///**
// * @brief 正向刺激电流测量数据寄存器 (地址0x7E)
// * @details 正向刺激电流中间脉宽时刻的数值
// */
//#define LMP1602X1_REG_7E_CURRENT_DATA_PLUS    (0x7E)
//typedef union {
//    uint16_t value;
//    struct {
//        uint16_t current_data_plus : 12;     /**< [11:0] 正向刺激电流中间脉宽时刻的数值，默认值0x0 */
//        uint16_t reserved : 4;               /**< [15:12] 保留位，默认值0x0 */
//    } bits;
//} lmp1602x1_current_data_plus_t;
//
//#endif /* __LMP1602X1_REG_H__ */
