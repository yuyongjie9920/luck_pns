#ifndef __LEADINNO_APP_H__
#define __LEADINNO_APP_H__

#define ENABLE_LEADNNINO_X4  //使用X4驱动芯片
#define ENABLE_LEADNNINO_X5  //使用X4驱动芯片

#include "leadinno_rf_port.h"
#include "leadinno_st25r3916.h"

#ifdef ENABLE_LEADNNINO_X4
#include "x4.h"
#endif

#ifdef ENABLE_LEADNNINO_X5
#include "x5.h"
#endif

enum {
    DEV_X4,   //设备
    DEV_X5,   //设备
    DEV_NO_FIND //没有发现设备
}leadinno_dev_name;

//函数控制接口
typedef enum {
    LEAD_SEND_QEURY,        //发送query
    LEAD_GET_VRECP,         //获取电压
    LEAD_GET_TEMP,          //获取温度
    LEAD_WRITE_EFUSE,       //烧录efuse  写入数据 lead.write_efuse
    LEAD_READ_EFUSE,        //读取efuse结果
    LEAD_RESET,             //复位
    LEAD_GET_RES,           //测量各个电极直接的阻抗
    LEAD_OUT_PULSE_SIMPLE,  //简单刺激
    LEAD_OUT_PULSE_BURST,   //输出爆发模式
    LEAD_OUT_PULSE_ANY,     //输出任意波形
    LEAD_OUT_PULSE_TWA,     //输出交替刺激波形
    LEAD_STOP_PULSE,        //停止波形输出
    LEAD_GET_I_STIM,        //获取刺激电流
    LEAD_MORO_SIMPLE,       //多次程序刺激
    LEAD_WRITE_REG,         //写寄存器值
    LEAD_READ_REG,          //读寄存器
}leadinno_ctl_func_e;


typedef struct {
    uint8_t dev_name;       //设备名

} leadinno_user_app;

typedef struct {
    uint8_t reg;
    uint16_t data;
} leadinno_reg_t;

//控制接口返回或填充的数据
typedef struct {
    uint8_t ctl;              //函数控制
    float vrecp;              //获取到的电压值
    float temp;               //获取到的温度值
    float get_i_stim;         //获取到的电流值
    float get_i_p_stim;       //获取到的正向刺激电流
    float get_i_n_stim;       //获取到的负向刺激电流
    uint32_t write_efuse;     //设置的efuse值
    uint8_t set_query_power;  //通讯时候的功率
    uint8_t stim_pulse_power; //出刺激的供能功率
    uint16_t get_res[16];      //存储阻抗测量结果   //第一个数据存放 基准电极-0-7 电极
    uint8_t set_res_ch;       //设置阻抗测量的基准通道
    uint32_t get_efuse;       //获取efuse值
#ifdef ENABLE_LEADNNINO_X4
    x4_simple_params simple;
    x4_burst_params burst;
    x4_TWA_params TWA;
#endif
#ifdef ENABLE_LEADNNINO_X5
    x5_query_status_t x5_query;
    x5_simple_params_t x5_pulse; //x5 芯片配置
#endif
    leadinno_user_app this;
    leadinno_reg_t reg;
} leadinno_data;
extern leadinno_data lead;

/// 错误码的枚举值
typedef enum
{
    ASIC_OK = 0,               ///< ok
    ASIC_ERR = 0xFF,           ///< 错误
    ASIC_PULSE_ERR = 0xFE,     ///< 脉宽错误
    ASIC_AMP_ERR = 0xFD,       ///< 幅度错误
    ASIC_PER_ERR = 0xFC,       ///< 周期错误
    ASIC_POLAR_ERR = 0xFB,     ///< 电极错误
    ASIC_STY_ERR = 0xFA,       ///< 策略错误
    ASIC_TIMEOUT = 0xF9,       ///< 超时
    ASIC_BUSY = 0xF8,          ///< 繁忙
    ASIC_REPEAT = 0xF7,        ///< 重复
    ASIC_ANALY_ERR = 0xF6,     ///< 识别错误
    ASIC_DATA_OVERFLOW = 0xF5, ///< 获取数据溢出
    ASIC_DATA_NULL = 0xF4,     ///< 设置数据
    ASIC_CHARGE_IMB = 0xF3,    ///< 电荷不平衡
    ASIC_CLK_ERR = 0xF2,       ///< 时钟错误

    ASIC_MODE_ERR = 0xF1,        ///< 模式/形状错误
    ASIC_RANDOM_STEP_ERR = 0xF0, ///< 随机模式步数错误
    ASIC_BURST_MODE_ERR = 0xEF,  ///< 爆发模式刺激数目错误
    ASIC_FEEDBACK_ERR = 0xEE,    ///< 芯片返回错误 设置参数无效

    ASIC_WRITE_ERR          = 0xED,    ///< 写错误
    ASIC_READ_ERR           = 0xEC,    ///< 读错误
    ASIC_RW_SIM_ERR         = 0xEB,    ///< 读写不一致错误
    FUN_ERR                 = 0xEA,    ///< 函数错误 没有对应的case
    ASIC_INVERT_PULSE_ERR   = 0xE9,    ///< 交替模式脉宽错误  低频刺激的脉宽占高频周期的百分比过大/误差过大
    ASIC_INVERT_PER_ERR     = 0xE8,    ///< 交替模式周期错误  刺激所有时长大于周期的最大公约数

    ASIC_RF_PWM_ERR         = 0xDF,    ///< 芯片在切换射频供能为pwm方式的时候出现错误

    ASIC_ANT_LOC_OK         = 0xDE,    ///< 芯片天线位置对齐可以用
    ASIC_ANT_LOC_HARDLY     = 0xDD,    ///< 芯片天线位置没对齐但能用
    ASIC_ANT_LOC_ERR        = 0xDC,    ///< 芯片天线位置没对齐且不能用
    ASIC_ANT_LOST           = 0xDB,    ///< 无法和芯片通信/芯片天线完全找不到

    ASIC_HB_DISC_ERR        = 0xDA,    ///< 和刺激器进行心跳交互时，芯片完全找不到/掉线
    ASIC_HB_LOC_ERR         = 0xD9,    ///< 和刺激器进行心跳交互时，芯片没复位但是天线位置异常，整流电压不够
    ASIC_HB_RST_LOC_ERR     = 0xD8,    ///< 和刺激器进行心跳交互时，芯片复位且天线位置异常，整流电压不够
    ASIC_HB_RST_ERR         = 0xD7,    ///< 和刺激器进行心跳交互时，芯片在两次心跳间隙只是复位但是天线正常

    ASIC_STIM_PWR_ERR       = 0xD6,    ///< 芯片刺激参数和射频能量不匹配，即参数耗能太多，程控仪在合理温升区间不能提供足够能量，射频能量已经给到最大也不够
    ASIC_STIM_RST           = 0xD5,    ///< 由于没有query导致的刺激失败 可能原因是忘记query或者query到刺激之间芯片发生复位
    ASIC_CHECK_ERR          = 0xD4,    ///< 没有轮询到合适的通信功率在3次及以上
    ASIC_VRECP_ZERO         = 0xD3,    ///< 和刺激器通信成功但是获取的vrecp电压是0  原因是第一次读或者间隔太短
    ASIC_STIM_PWR_SMALL     = 0xD2,    ///< 通过电压闭环检测到射频能量不够，需要增大射频能量
    ASIC_STIM_PWR_BIG       = 0xD1,    ///< 通过电压闭环检测到射频能量冗余，需要降低射频能量
    ASIC_STIM_PWR_SPEC      = 0xD0,    ///< 出现调射频能量，芯片整流电压突变或其它认为已经调节到位的情况，此时不再调节射频能量

    ASIC_DOUBLE_NUM_ERR     =  0xAF,   ///< 双刺激器的个数错误 或者
    ASIC_DOUBLE_UID_ERR     =  0xAE,   ///< 双刺激器的UID错误 正式版的两个UID一样或者有为0的
    ASIC_DOUBLE_SINGLE_ERR  =  0xAD    ///< 当前单/双刺激器模式错误 某些函数只可在单/双刺激器模式下配置
} asic_err_code;

int leadinno_x4_crtl(leadinno_ctl_func_e func);
int leadinno_x5_crtl(leadinno_ctl_func_e func);
int leadinno_crtl(leadinno_ctl_func_e func);


#endif

