/**
 * @file LMS0820X4_driver.h
 * @brief LMS0820X4/LMP0820X4 数字寄存器驱动头文件
 * @version 2.0
 * @date 2025-05-08
 *
 * 基于寄存器列表 LMS0820X4_LMP0820X4数字寄存器列表20250508.xlsx 生成
 * 使用枚举、联合体和结构体方式定义
 */

#ifndef __LMS0820X4_DRIVER_H__
#define __LMS0820X4_DRIVER_H__

#include <stdint.h>
#include "x4_reg.h"
#include "leadinno_st25r3916.h"

//对接功率接口
#define SET_RF_POWER(a)   st25r3916_set_rf_power_level(a)

#define MAX_NUM_RANDOM    16                  //随机模式内刺激的最大次数
#define R_CUR_FOR_BAK     40
#define STIM_CLK_DIV_LF   10      //低频刺激时的分频值 同时也是能接受的时间步进精度
#define SYS_MAX_MA        200     //最大支持20ma





#pragma pack(1)  // 设置1字节对齐

typedef enum { //获取VRECP模式
    X4_VRECP_NEW_MODE,
    X4_VRECP_DELAY_MODE,
} x4_get_vrecp_mode_e;


typedef enum {
    X4_MEAS_ENABLE_NONE  = 0x00,  // 二进制 000 - 无测量使能
    X4_MEAS_ENABLE_TEMP  = 0x01,  // 二进制 001 - 温度测量使能 (bit0)
    X4_MEAS_ENABLE_VOLT  = 0x02,  // 二进制 010 - 电压测量使能 (bit1)
    X4_MEAS_ENABLE_IMP   = 0x04,  // 二进制 100 - 阻抗测量使能 (bit2)
} x4_meas_enable_mask_e;

typedef enum {
    X4_OLD_POWER_QUERY,  //以上一次通讯能量进行通讯
    X4_SET_POWER_QUERY,  //设置功率进行通讯
    X4_ALL_POWER_QUERY,  //所以功率点都通讯一次用于测试通讯
    X4_AUTO_POWER_QUERY  //自动找到通讯功率点并query
} x4_query_mode_e;

typedef enum {
    X4_STIM_A = 0,   //刺激A 默认只有一个刺激的时候使用刺激A
    X4_STIM_B,       //刺激B 目前25-9月版本支持到A-B两个交替
    X4_STIM_C,       //刺激C
    X4_STIM_D        //刺激D
} x4_invert_idx_t;

typedef enum
{
    X4_ASIC_1 = 0,   //刺激器1 默认只有一个刺激器的时候使用刺激器1
    X4_ASIC_2,       //刺激器2
    X4_ASIC_SUM      //刺激器数目
} x4_asic_idx_t;

typedef enum {
    X4_ASIC_SINGLE = 0,   //单刺激器 默认单刺激器
    X4_ASIC_DOUBLE,       //双刺激器
    X4_ASIC_OTHER         //其它数目刺激器
} x4_asic_num_t;

/// 随机刺激模式中每个刺激脉宽可以细分的个数/每个都可以单独控制DAC
typedef enum {
    X4_RANDOM_AMP_NUM_1 = 0,   //只有1个 和简单模式一致
    X4_RANDOM_AMP_NUM_2,       //细分为2个
    X4_RANDOM_AMP_NUM_4,       //细分为4个
    X4_RANDOM_AMP_NUM_8,       //细分为8个
    X4_RANDOM_AMP_NUM_16       //细分为16个
} x4_random_wave_amp_t;

//方波 正弦波 三角波 锯齿波 阶梯波 等形状
typedef enum {
    X4_RANDOM_SQUARE_WAVE = 0,    //方波
    X4_RANDOM_SINE_WAVE,          //正弦波
    X4_RANDOM_TRIANGLE_WAVE,      //三角波
    X4_RANDOM_TRAPE_WAVE,         //梯形波
    X4_RANDOM_SAWTOOTH_WAVE1,     //锯齿波1
    X4_RANDOM_SAWTOOTH_WAVE2,     //锯齿波2
    X4_RANDOM_SAWTOOTH_WAVE3,     //锯齿波3
    X4_RANDOM_SAWTOOTH_WAVE4,     //锯齿波4
    X4_RANDOM_LOGO_WAVE,          //LOGO模式
} x4_random_wave_shape_t;

typedef enum {
    X4_PWR_DONW_RST = 0,    //掉电复位
    X4_NO_PWR_DONW_RST      //不掉电复位
} x4_asic_rst_way_t;

typedef enum {
    X4_STIM_CFG = 0,      //刺激配置
    X4_TRIM_CFG           //微调配置 一般是初始化用
} x4_asic_block_state_t;

/// 刺激模式的枚举值
typedef enum {
    X4_SIMPLE_MODE = 0,  //简单模式
    X4_RANDOM_MODE,      //任意模式
    X4_BURST_MODE,       //爆发模式
    X4_INVERT_MODE,      //交替模式
} x4_stim_wave_mode_t;

/// 电极状态的枚举值
typedef enum {
    X4_RES_OPEN = 0,  //断路
    X4_RES_500, //500欧姆(目前是有电阻就归到这一项)
    X4_RES_SHORT, //短路
    X4_RES_OTHER //其它情况
} x4_polar_state_t;

typedef struct {
    uint32_t amplitude_forward;  ///< 前向波形幅度 单位ua
    uint32_t amplitude_backward; ///< 后向波形幅度 单位ua
} x4_range_t;

typedef struct {
    uint16_t dir;    ///< 电极电流方向(源极位置)  [7:0]生效，[0]=A组 [1]=B组
    uint16_t on;     ///< 电极开关(电极开的位置)  [7:0]生效，[0]=A组 [1]=B组
} x4_polar_config;

/// 简单模式波形特征参数
typedef struct {
    bool low_power_mode;   ///< 低功耗模式 0：不开启 1：开启 默认0，[0]=A组 [1]=B组
    uint32_t pulse_forward;      ///< 前向波形脉宽 us
    uint16_t time_interphase1;   ///< 前向波形与后向波形的间隔  us 1-255us
    uint32_t pulse_backward;     ///< 后向波形脉宽 us
    uint16_t time_interphase2;   ///< 后向波形与time_phase3的间隔  us 1-255us
    uint32_t time_phase3;        ///< 被动平衡延时 us 1-16383us
    uint32_t time_interpulse;    ///< 被动平衡之后的延时 us  1-65535us
    uint32_t amplitude_forward;  ///< 前向波形幅度 单位ua
    uint32_t amplitude_backward; ///< 后向波形幅度 单位ua
    uint8_t stim_strategy;       ///< 刺激策略 默认主动平衡
    uint16_t base_freq;          ///< 刺激频率
    x4_polar_config polar;       //电极方向
    x4_range_t range;            //刺激幅度
    x4_stim_wave_mode_t pulse_mode; //刺激模式

    uint16_t pwm_duty;          /**< PWM占空比 */
    uint8_t pwm_sync_mode;      /**< PWM_同步模式 */
    uint16_t pwm_delay_count;   /**< PWM_同步位移模式 */

    uint8_t stim_clk_div;
} x4_simple_params;

/// 爆发模式波形特有特征参数
typedef struct {
    uint32_t period;             ///< 刺激簇周期 单位us
    uint16_t pulse_count;        ///< 大周期内刺激的小周期数目
    uint32_t inter_burst;        ///< 爆发模式下不同簇之间的间隔
    uint16_t invert_div;         ///< 交替模式时此参数是最大频率的分频数 （X4版本以前默认交替都使用爆发模式）
    uint32_t invert_ori_period;  ///< 交替模式时刺激簇原始周期 单位us 由于爆发需要变形使用，period已经发生改变
} x4_burst_params;

/// 随机模式波形特征参数
typedef struct {
    x4_random_wave_shape_t shape;         ///< 随机模式下的细分形状
    uint16_t amplitude_forward;        ///< 随机模式下正刺激的幅度值
    uint32_t amplitude_time;           ///< 随机模式下每个刺激电流的持续时间，单位us
    uint16_t amplitude_nums;           ///< 正或负刺激周期刺激电流个数
} x4_random_wave_feature_params;

//交替刺激
typedef struct {
    x4_simple_params puse_a;
    x4_polar_config polar_a;
    x4_range_t range_a;
    x4_simple_params puse_b;
    x4_polar_config polar_b;
    x4_range_t range_b;
    bool low_power_mode;   ///< 低功耗模式 0：不开启 1：开启 默认0，[0]=A组 [1]=B组

} x4_TWA_params;


//#define INVERT_NUM   2
//typedef struct //刺激相关配置（含上述所有模式）
//{
//    uint8_t  stim_wave_mode; //刺激波形模式
//
//    bool low_power_mode[INVERT_NUM]; //低功耗模式 0：不开启 1：开启 默认0
//    uint32_t phase1[INVERT_NUM]; //阶段1刺激脉宽
//    uint32_t inter_phase1[INVERT_NUM]; //阶段1后插入的静默脉宽
//    uint32_t phase2[INVERT_NUM]; //阶段2刺激脉宽
//    uint32_t inter_phase2[INVERT_NUM]; //阶段2后插入的静默脉宽
//
//    uint32_t phase3[INVERT_NUM]; //阶段3被动平衡脉宽
//    uint32_t inter_pulse[INVERT_NUM]; //阶段3后插入的静默脉宽
//
//    uint32_t low_power_td1[INVERT_NUM]; //低功耗模式下的td1 刺激电路开启与开始刺激之间的延迟时间 典型值32us 必须设置为1，否则刺激会有500ms延时和开不开启低功耗模式无关
//    uint32_t low_power_td3[INVERT_NUM]; //低功耗模式下的td3 电荷平衡结束与STIM_EN\STIM_ON\DAC_ENI关闭之间的延迟时间 典型值128us
//
//    uint32_t pulse_count_burst; //爆发模式下 持续的的刺激簇个数 执行这些次数后会停止 暂时没用
//    uint8_t pulse_count[INVERT_NUM]; //爆发模式下 一簇刺激中的刺激个数
//    uint32_t inter_burst[INVERT_NUM]; //爆发模式下不同簇之间的间隔
//
//    uint16_t dac_pos[INVERT_NUM]; //正极幅度
//    uint16_t dac_neg[INVERT_NUM];  //负极幅度
//
//    uint16_t p_dca_dan_sel[INVERT_NUM]; //设置正刺激时，阳阴极开关状态，8bit的数值，每一位代表一个开关，该位为1时开关打开，为0时开关关闭
//    uint16_t n_dca_dan_sel[INVERT_NUM]; //设置负刺激时，阳阴极开关状态，8bit的数值，每一位代表一个开关，该位为1时开关打开，为0时开关关闭
//
//    uint16_t random_dac_pos[MAX_NUM_RANDOM]; //仅任意刺激模式使用，设置每个刺激电流正幅度
//    uint16_t random_dac_neg[MAX_NUM_RANDOM]; //仅任意刺激模式使用，设置每个刺激电流正幅度
//    uint8_t random_phase_config1;  //仅任意刺激模式使用，设置任意刺激模式下正刺激电流的个数
//    uint8_t random_phase_config2;  //仅任意刺激模式使用，设置任意刺激模式下负刺激电流的个数
//
//    uint8_t invert_id; //交替刺激的id
//    uint16_t invert_cnt[INVERT_NUM]; //交替刺激A/B的切换计数时间 单位是刺激时钟周期 CLK
//    // bool invert_diff_freq_mode;  //异频模式  FALSE：同频 TRUE：异频 默认FALSE
//    // uint32_t invert_period_gcd; //交替刺激周期的公约数 单位us
//    // uint32_t invert_fre_lcm;   //交替刺激的频率公倍数 单位hz
//    uint32_t invert_AB_NUM;       //交替时隔几个出来一个（X4版本以前只支持这种方式）
//    // uint8_t stim_clk_div[MAX_NUM_RANDOM]; //分频系数1-63
//} x4_stim_config;


/// 各模式波形特征参数(三种模式公共和特有参数)
//typedef struct
//{
//    uint16_t soft_start;               ///< 软起动 单位ms
//    uint16_t polar_dir[INVERT_NUM];    ///< 电极电流方向(源极位置)  [7:0]生效，[0]=A组 [1]=B组
//    uint16_t polar_on[INVERT_NUM];     ///< 电极开关(电极开的位置)  [7:0]生效，[0]=A组 [1]=B组
//    bool low_power_mode[INVERT_NUM];   ///< 低功耗模式 0：不开启 1：开启 默认0，[0]=A组 [1]=B组
//    x4_invert_idx_t stim_id;              ///< 刺激ID
//    x4_stim_wave_mode_t wave_mode;        ///< 刺激的几种模式
//    x4_simple_params simple[INVERT_NUM]; ///< 简单模式波形参数 [0]=A组 [1]=B组
//    x4_burst_params burst[INVERT_NUM];   ///< 爆发模式波形参数 [0]=A组 [1]=B组
//    x4_random_wave_feature_params random[INVERT_NUM]; ///< 随机模式波形参数 [0]=A组 [1]=B组
//} x4_gro_wave_feature_params;
//


//query返回的数据
typedef union {
    uint16_t rest_uid;
    struct __attribute__((packed)) {
        uint16_t rest_flag : 1;
        uint16_t uid : 15;
    } data;
} x4_rest_add_uid_t;

//RF接收
typedef struct {
    uint8_t cmd;
    uint8_t reg;
    uint16_t uid;
    uint8_t crc_h;
    uint8_t crc_l;
} x4_rf_read_send_data_t;
typedef struct {//接收数据 header 1bit  reg_data 16bit   rn 16bit  crc 16bit
    uint8_t header;
    uint16_t reg_data;
    uint16_t rn;
    uint16_t crc;
} x4_rf_rx_dec_data_t;

//RF发送
typedef struct {
    uint8_t cmd;
    uint8_t reg;
    uint16_t reg_data;
    uint16_t uid;
    uint8_t crc_h;
    uint8_t crc_l;
} x4_rf_write_send_data_t;
typedef struct {//发送数据中 header 1bit  reg_data 16bit   rn 16bit  crc 16bit
    uint8_t header;
    uint16_t rn;
    uint16_t crc;
} x4_rf_write_dec_data_t;

struct x4_data{
    x4_rest_add_uid_t filler;
};


#pragma pack()   // 恢复默认对齐

#define RF_OK       0
#define RF_FIAL    -1


int x4_cmd_write(uint8_t reg, uint16_t reg_data);
int x4_cmd_read(uint8_t reg, uint16_t *get_reg_data);
int __x4_rf_send_query(void);
int x4_rf_send_query(void);
int x4_rf_get_leadinno_vrecp(float *vrecp, uint16_t delay_count);
int x4_rf_get_leadinno_temp(float *temp);
int x4_rf_set_leadinno_efuse(uint32_t efuse);
int x4_rf_get_leadinno_res(uint8_t channel_sel, uint16_t *res);
int x4_rf_get_leadinno_efuse(uint32_t *efuse);
int x4_rf_get_leadinno_i_stim(float *currentm, bool delay_sample_mode);

int x4_rf_out_pulse_simple(x4_simple_params *params);
int x4_rf_out_pulse_burst(x4_simple_params *params, x4_burst_params *params2);
int x4_rf_pluse_TWA(x4_TWA_params *params);

int x4_rf_stop_pulse(void); //停止刺激
int x4_rf_run_pulse(void); //运行刺激

#endif /* __LMS0820X4_DRIVER_H__ */


