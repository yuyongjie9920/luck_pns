/**
 * @file x5_test.c
 * @brief LMS0820X5 测试命令 - 基于测试方案文档
 * @version 2.0
 * @date 2026-03-17
 */

#include "leadinno_app.h"
#include "x5.h"
#include "main_user.h"
#include "string.h"

#define ENABLE_TEST_X5
/*==============================================================================
 * 调试日志宏
 *============================================================================*/

#ifdef  ENABLE_TEST_X5
#if 1
    #define log(...) logInfo(__VA_ARGS__)
#else
    #define log(...) do {} while(0)
#endif

static uint8_t test_pwm_puty, test_pwm_hz;
/*==============================================================================
 * 功率控制命令
 *============================================================================*/

/**
 * @brief 设置射频功率
 * @param power 功率值 0-15
 */
static int test_x5_set_power(uint8_t power)
{
    if (power > 15) power = 15;
    log("Set RF power: %d", power);
    st25r3916_set_rf_power_level(power);
    st25r3916_open_rf(OPEN);
    lead.stim_pulse_power = power;
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5PWR, test_x5_set_power, "X5: Set RF power (0-15)");

/**
 * @brief 射频开关
 * @param onoff 1-开启, 0-关闭
 */
static int test_x5_rf_onoff(uint8_t onoff)
{
    if (onoff) {
        log("RF ON");
        st25r3916_open_rf(OPEN);
    } else {
        log("RF OFF");
        st25r3916_open_rf(CLOSE);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5RF, test_x5_rf_onoff, "X5: RF ON/OFF (1-on, 0-off)");

/*==============================================================================
 * Query测试命令 - 根据测试方案文档表4.3
 *============================================================================*/

/**
 * @brief 广播Query - 获取所有响应
 */
static int test_x5_query_broadcast(void)
{
    int err = x5_rf_send_query();
    if (err == 0) {
        log("===== Query Response =====");
        log("Status Value : 0x%04x", lead.x5_query.value);
        log("EFUSE ID     : 0x%02x", lead.x5_query.bits.efuse_id);
        log("");
        log("Bit15 - UnderVolt occurred     : %d", lead.x5_query.bits.undervolt);
        log("Bit14 - Program0 active        : %d", lead.x5_query.bits.prog0_active);
        log("Bit13 - Overlap occurred       : %d", lead.x5_query.bits.overlap);
        log("Bit12 - Stimulation complete   : %d", lead.x5_query.bits.stim_complete);
        log("Bit11 - Temperature done       : %d", lead.x5_query.bits.temp_done);
        log("Bit10 - Voltage/Current done   : %d", lead.x5_query.bits.volt_done);
        log("Bit9  - Impedance done         : %d", lead.x5_query.bits.imp_done);
        log("Bit8  - Reserved               : %d", lead.x5_query.bits.reserved);
        log("Bit7-0- EFUSE ID low 8 bits    : 0x%02x", lead.x5_query.bits.efuse_id);
        log("==========================");
    }
    return err;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5QRYB, test_x5_query_broadcast, "X5: Broadcast query");

/**
 * @brief 指定功率Query
 * @param power 功率值 0-15
 * @param mode 0-广播, 1-点对点(使用当前efuse_id)
 */
static int test_x5_query_with_power(uint8_t power)
{
    int err = x5_rf_send_query_with_power(power);
    if (err == 0) {
        log("Query with power is %d OK", power);
    }
    return err;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5QRYPWR, test_x5_query_with_power, "X5: Query with power [power] [0-bcast,1-p2p]");

/**
 * @brief 所有功率点Query测试
 */
static int test_x5_query_all_power(void)
{
    x5_rf_send_query_all_power();
    test_x5_set_power(0);
    rf_pwm_chang(test_pwm_puty, test_pwm_hz);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5QRYA, test_x5_query_all_power, "X5: Query all power levels");


/**
 * @brief RFID接口读写测试
 * @param reg 寄存器地址
 * @param data 要写入的数据
 */
static int test_x5_rfid_rw(uint8_t reg, uint16_t data)
{
    uint16_t readback = 0;

//    /* 先确保有efuse_id */
    if (x5.efuse_id == 0) {
        x5_rf_send_query();
    }

    int err = x5_cmd_write(reg, data);

    if (err == 0) {
        delay_ms(10);
        err = x5_cmd_read(reg, &readback);
        if (err == 0) {
            log("RFID Readback: reg 0x%02x -> 0x%04x %s",
                    reg, readback, (readback == data) ? "MATCH" : "MISMATCH");
        }
    }

    return err;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5RFID, test_x5_rfid_rw, "X5: RFID test [reg] [data]");

static uint8_t power = 0;

extern int __x5_rf_send_cmd_write(uint8_t reg, uint16_t reg_data);

static void test_func(void)
{
    logInfo("x5_rf_send_query_with_power");
    x5_rf_send_query_with_power(power);
    logInfo("x5_cmd_write(0x60, 0xc8);");
    __x5_rf_send_cmd_write(0x60, 0xc8);
    delay_ms(500);
    logInfo("x5_rf_send_query_with_power(power)111111111111111;");
    x5_rf_send_query_with_power(power);
    logInfo("x5_cmd_write(0x60, 0xc0);@@@@@@@@@@@@  power = 0x%x", power);
    __x5_rf_send_cmd_write(0x60, 0xc0);
    power++;
    power = power % 12;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 test_func, test_func, "test");

static void w_x5_reg(uint8_t reg, uint8_t reg_data)
{
    x5_cmd_write(reg, reg_data);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 w_x5_reg, w_x5_reg, "test");

static void r_x5_reg(uint8_t reg)
{
    uint16_t get_data;
    x5_cmd_read(reg, &get_data);
    logInfo("get_data = 0x%x",  x5_cmd_read);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 r_x5_reg, r_x5_reg, "test");

/*==============================================================================
 * 测量命令 - 根据测试方案文档3.5.2-3.5.5
 *============================================================================*/

/**
 * @brief 获取整流电压 - 测试方案文档3.5.3
 */
static int test_x5_get_vrecp(void)
{
    float vrecp[2] = {0};
    if (x5_rf_get_vrecp(vrecp, 0) == 0) {
        log("VRECP Voltage: %.2fV", vrecp[0]);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5VRECP, test_x5_get_vrecp, "X5: Get VRECP voltage");

/**
 * @brief 延迟电压测量 - 测试方案文档3.5.9
 * @param delay 延迟的刺激周期数
 */
static int test_x5_get_vrecp_delay(uint16_t delay)
{
    float vrecp[2] = {0};
    if (x5_rf_get_vrecp(vrecp, delay) == 0) {
        log("VRECP Delayed Measurement:");
        log("  Min (before stim): %.2fV", vrecp[0]);
        log("  Max (during stim) : %.2fV", vrecp[1]);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5VRD, test_x5_get_vrecp_delay, "X5: Get VRECP with delay [cycles]");

/**
 * @brief 获取温度 - 测试方案文档3.5.4
 * 公式: OUT_RTV = 1.48716 - 0.021598 * T
 */
static int test_x5_get_temp(void)
{
    float temp = 0;
    if (x5_rf_get_temp(&temp) == 0) {
        log("Temperature: %.1f", temp);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5TEMP, test_x5_get_temp, "X5: Get temperature");

/**
 * @brief 获取阻抗 - 测试方案文档3.5.2
 * @param base_ch 基准通道(0-7)
 * 公式: OUT_RTV = VOUT_DAC/50 * R * 4/30 + 0.305
 */
static int test_x5_get_res(uint8_t base_ch, uint8_t osc)
{
    uint16_t res[8] = {0};
    x5_rf_get_res(base_ch, res, osc);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5RES, test_x5_get_res, "X5: Get impedance [base_ch(0-7)]");

/**
 * @brief 获取刺激电流 - 测试方案文档3.5.10
 */
static int test_x5_get_current(void)
{
    float p = 0, n = 0;
    if (x5_rf_get_current(&p, &n) == 0) {
        log("Stimulation Current:");
        log("  Positive: %.2f mA", p);
        log("  Negative: %.2f mA", n);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5CUR, test_x5_get_current, "X5: Get stimulation current");

/**
 * @brief 写eFuse
 * @param efuse 24位efuse值(十六进制)
 */
static int test_x5_write_efuse(uint32_t efuse)
{
    log("Writing EFUSE: 0x%06x", efuse & 0xFFFFFF);
    log("Note: This will reset the chip and change its ID");
    return x5_rf_write_efuse(efuse);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5EFW, test_x5_write_efuse, "X5: Write EFUSE [24-bit hex]");

/**
 * @brief 读eFuse
 */
static int test_x5_read_efuse(void)
{
    uint32_t efuse = 0;
    if (x5_rf_read_efuse(&efuse) == 0) {
        log("EFUSE Value: 0x%06x", efuse);
        log("  Low 16 bits: 0x%04x", efuse & 0xFFFF);
        log("  High 8 bits : 0x%02x", (efuse >> 16) & 0xFF);
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5EFR, test_x5_read_efuse, "X5: Read EFUSE");

/**
 * @brief 停止刺激
 */
static int test_x5_stop_pulse(void)
{
    log("Stop stimulation");
    return x5_rf_stop_pulse();
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5STOP, test_x5_stop_pulse, "X5: Stop stimulation");

/**
 * @brief 复位芯片
 */
static int test_x5_reset(void)
{
    log("Reset chip");
    rf_reset_chip();
    delay_ms(100);
    /* 复位后重新查询 */
    x5_rf_send_query();
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5RST, test_x5_reset, "X5: Reset chip");


#define SET_STIM_ON0(dir, on) (((dir) << 8) | ((~(dir)) & (on)))
#define SET_STIM_ON1(dir, on) ((((~(dir)) & (on)) << 8) | (dir))

/**
 * @brief 简单刺激测试 - 程序0
 * @param pwm_duty PWM占空比(%)
 * @param width 脉宽(ms)
 * @param freq 频率(Hz)
 * @param ma 电流(0.1mA单位, 如20=2.0mA)
 * @param pin_dir 电极方向(8位掩码)
 * @param pin_on 电极开启(8位掩码)
 */
static void test_x5_simple(uint8_t pwm_duty, uint16_t width, uint16_t freq,
                          uint16_t ua, uint8_t stim_on, uint8_t power)
{
    x5_simple_params_t params = {0};

    lead.stim_pulse_power = power;  //设置刺激功率

    /* 先查询获取efuse_id */
    if (x5.efuse_id == 0) {
        x5_rf_send_query();
    }

    lead.stim_pulse_power = power;

    logInfo("stim_on = %d", stim_on);
    uint8_t pin_dir;
    uint8_t pin_on;
    pin_dir = (1 << (stim_on /10)) | (0 << (stim_on%10));
    logInfo("set key = 0x%x", pin_dir);
    pin_on = (1 << (stim_on/10)) | (1 << (stim_on%10));
    logInfo("set_on = 0x%x", pin_on);


    params.pulse_mode = X5_SIMPLE_MODE; //刺激波形爆发模式
    params.program_cnt = 0;   //刺激波形程序个数
    params.low_power_mode = 0;

    params.base_freq = freq;  //设置频率
    /* 程序0参数 */
    params.program[0].tphase1 = width;      /* 正脉冲 */
    params.program[0].tphase2 = width;      /* 负脉冲 */
    params.program[0].tphase3 = 10;            /* 电荷平衡时间 */
    params.program[0].tinterphase1 = 10;       /* 间隔10us */
    params.program[0].tinterphase2 = 10;       /* 间隔10us */
    params.program[0].amplitude_p = ua;        /* 刺激幅度ua*/
    params.program[0].amplitude_n = ua;        /* 刺激幅度ua */
    params.program[0].polar.dir = pin_dir;     /* 电极配置: 正相和负相 */
    params.program[0].polar.on = pin_on;
    params.program[0].multiple = 1;            /* 设置频率倍数关系 */

    /* 打印配置 */
    log("Input Parameters:");
    log("  PWM Duty: %d%%", pwm_duty);
    log("  Pulse Width: %d ms (%d us)", width, width);
    log("  Frequency: %d Hz", freq);
    log("  Current: (%d uA)", ua);
    log("  Electrode Direction: 0x%02x", pin_dir);
    log("  Electrode Enable: 0x%02x", pin_on);
    log("Configured Parameters (us):");
    log("  Tphase1 (positive): %d us", params.program[0].tphase1);
    log("  Tinterphase1: %d us", params.program[0].tinterphase1);
    log("  Tphase2 (negative): %d us", params.program[0].tphase2);
    log("  Tinterphase2: %d us", params.program[0].tinterphase2);
    log("  Tphase3 (balance): %d us", params.program[0].tphase3);
    log("  Tinterpulse: %d us", params.program[0].tinterpulse);
    log("  DAC Positive: 0x%03x (%d)", params.program[0].amplitude_p, params.program[0].amplitude_p);
    log("  DAC Negative: 0x%03x (%d)", params.program[0].amplitude_n, params.program[0].amplitude_n);
    log("  STIM_ON0 dir: 0x%04x", params.program[0].polar.dir);
    log("  STIM_ON1 on: 0x%04x", params.program[0].polar.on);

    /* 配置刺激 */
    int err = x5_rf_config_simple(&params);
    if (err == 0) {
        if (pwm_duty > 0) {
            test_pwm_puty = pwm_duty;
            test_pwm_hz = freq;
            rf_pwm_chang(pwm_duty, freq);  /* 启动PWM供能 */
            log("PWM enabled: duty=%d%%, freq=%dHz", pwm_duty, freq);
        }
        log("Simple stimulation started - OK!!!");
    } else {
        log("Stimulation configuration FAILED!");
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5SIMPLE, test_x5_simple,
                 "X5: Simple stim [pwm%%] [w(ms)] [freq] [ma*10] [dir] [on]");


static void test_x5_soft_simple(uint16_t width, uint16_t freq,
                          uint16_t ua, uint8_t stim_on, uint8_t inc_value, uint8_t step)  //软启动
{

    __x5_rf_reset();

    x5_softstart_params_t soft;
    soft.enable = 1;
    soft.inc_value = inc_value; //步进ADC值
    soft.step = step;   //步进刺激数
    x5_rf_config_softstart(X5_PROGRAM_0, &soft);
    x5_rf_config_softstop(1);

    test_x5_simple(100,  width,  freq, ua,  stim_on , 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5SOFT, test_x5_soft_simple,
                 "X5: Simple stim [pwm%%] [w(ms)] [freq] [ma*10] [dir] [on]");

static void test_x5_soft_stop(void)
{
    x5_rf_stop_pulse();
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5SOFT_STOP, test_x5_soft_stop,
                 "X5: Simple stim [pwm%%] [w(ms)] [freq] [ma*10] [dir] [on]");
/*==============================================================================
 * 多程序刺激测试 - 根据测试方案文档3.5.6 (4个程序)
 *============================================================================*/

/**
 * @brief 多程序刺激测试 (4个程序)
 * @param pwm_duty PWM占空比(%)
 * @param freq 频率(Hz)
 * multiple0 程序0 是freq的多少倍
 * multiple1 程序1 是freq的多少倍
 */
static void test_x5_multi_prog(uint8_t stim_on, uint8_t pwm_duty, uint16_t freq, uint8_t program,
                               uint16_t multiple0, uint16_t multiple1, uint16_t multiple2, uint16_t multiple3)
{
    x5_simple_params_t params = {0};
    x5_phase_config_t phase = {0};

    uint8_t pin_dir;
    uint8_t pin_on;
    pin_dir = (1 << (stim_on /10)) | (0 << (stim_on%10));
    logInfo("set key = 0x%x", pin_dir);
    pin_on = (1 << (stim_on/10)) | (1 << (stim_on%10));
    logInfo("set_on = 0x%x", pin_on);



    if (x5.efuse_id == 0) {
        x5_rf_send_query();
    }

    uint8_t w = 60;
    log("===== X5 Multi-Program Test (4 programs) =====");
    log("Chip EFUSE ID: 0x%02x");

//    /* 相位配置 - 需要根据寄存器列表约束设置 */
//    phase.phase1_config = X5_PHASE_DAC_1;
//    phase.phase2_config = X5_PHASE_DAC_1;
//    phase.stim_clk_div = 0;
//    x5_rf_config_phase(&phase);

    uint32_t period_us = 1000000 / freq;

    params.pulse_mode = X5_SIMPLE_MODE; //刺激波形爆发模式
    /* 配置4个程序 */
    params.program_cnt = program;
    params.low_power_mode = 0;

    params.base_freq = freq; //保存基准频率

    if(program >= 0) {
        /* 程序0: 电极1, 200us, 1mA */
        params.program[0].tphase1 = w;
        params.program[0].tphase2 = w;
        params.program[0].tinterphase1 = w;
        params.program[0].tinterphase2 = w;
        params.program[0].multiple = multiple0;
        params.program[0].tphase3 = 10;
        params.program[0].amplitude_p = 1000;  /* 1mA */
        params.program[0].amplitude_n = 1000;
        params.program[0].polar.dir = pin_dir;  /* 电极1 */
        params.program[0].polar.on = pin_on;
        log("Program 0: Electrode1, 200us, 1.0mA");
        log("Configured Parameters (us):");
        log("  Tphase1 (positive): %d us", params.program[0].tphase1);
        log("  Tinterphase1: %d us", params.program[0].tinterphase1);
        log("  Tphase2 (negative): %d us", params.program[0].tphase2);
        log("  Tinterphase2: %d us", params.program[0].tinterphase2);
        log("  Tphase3 (balance): %d us", params.program[0].tphase3);
        log("  Tinterpulse: %d us", params.program[0].tinterpulse);
        log("  DAC Positive: 0x%03x (%d)", params.program[0].amplitude_p, params.program[0].amplitude_p);
        log("  DAC Negative: 0x%03x (%d)", params.program[0].amplitude_n, params.program[0].amplitude_n);
        log("  STIM_ON0 dir: 0x%04x", params.program[0].polar.dir);
        log("  STIM_ON1 on: 0x%04x", params.program[0].polar.on);
    }
    if(program >= 1) {
        /* 程序1: 电极2, 300us, 2mA */
        params.program[1].tphase1 = w;
        params.program[1].tphase2 = w;
        params.program[1].tinterphase1 = w;
        params.program[1].tinterphase2 = w;
        params.program[1].multiple = multiple1;
        params.program[1].tphase3 = 10;
        params.program[1].amplitude_p = 4000;  /* 2mA */
        params.program[1].amplitude_n = 4000;
        params.program[1].polar.dir = pin_dir;  /* 电极2 */
        params.program[1].polar.on = pin_on;
        log("Program 1: Electrode2, 300us, 2.0mA");
    }
    if(program >= 2) {
        /* 程序2: 电极3, 400us, 1.5mA */
        params.program[2].tphase1 = w;
        params.program[2].tphase2 = w;
        params.program[2].tinterphase1 = w;
        params.program[2].tinterphase2 = w;
        params.program[2].multiple = multiple2;
        params.program[2].tphase3 = 10;
        params.program[2].amplitude_p = 8000;  /* 1.5mA */
        params.program[2].amplitude_n = 8000;
        params.program[2].polar.dir = 1;  /* 电极3 */
        params.program[2].polar.on = 3;
        log("Program 2: Electrode3, 400us, 1.5mA");
    }
    if(program >= 3) {
        /* 程序3: 电极4, 500us, 2.5mA */
        params.program[3].tphase1 = w;
        params.program[3].tphase2 = w;
        params.program[3].tinterphase1 = w;
        params.program[3].tinterphase2 = w;
        params.program[3].multiple = multiple3;
        params.program[3].tphase3 = 10;
        params.program[3].amplitude_p = 14000;  /* 2.5mA */
        params.program[3].amplitude_n = 14000;
        params.program[3].polar.dir = pin_dir;  /* 电极4 */
        params.program[3].polar.on = pin_on;
        log("Program 3: Electrode4, 500us, 2.5mA");
    }
    /* 打印配置摘要 */

    log("Frequency: %dHz, Period: %dus", freq, period_us);

    int err = x5_rf_config_more_simple(&params);
    if (err == 0) {
        if (pwm_duty > 0) {
            test_pwm_puty = pwm_duty;
            test_pwm_hz = freq;
            rf_pwm_chang(test_pwm_puty, test_pwm_hz);
        }
        log("Multi-program stimulation started");
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5MPROG, test_x5_multi_prog,
                 "X5: Multi-program test [pwm%%] [freq]");

/*==============================================================================
 * 爆发刺激模式测试 - 根据测试方案文档3.5.7
 *============================================================================*/

/**
 * @brief 爆发刺激测试
 * @param pwm_duty PWM占空比(%)
 * @param width 脉宽(ms)
 * @param freq 频率(Hz)
 * @param ma 电流(0.1mA单位)
 * @param pin_dir 电极方向
 * @param pin_on 电极开启
 * @param burst_cnt 每个burst的脉冲数
 */
static void test_x5_burst(uint8_t pwm_duty, uint8_t width, uint16_t freq,
                         uint16_t ua, uint8_t stim_on, uint8_t burst_cnt, uint8_t power)
{

    uint8_t pin_dir;
    uint8_t pin_on;
    pin_dir = (1 << (stim_on /10)) | (0 << (stim_on%10));
    logInfo("set key = 0x%x", pin_dir);
    pin_on = (1 << (stim_on/10)) | (1 << (stim_on%10));
    logInfo("set_on = 0x%x", pin_on);

    lead.stim_pulse_power = power;  //设置刺激功率

    x5_simple_params_t params = {0};
    if (x5.efuse_id == 0) {
        x5_rf_send_query();
    }
    /* 配置简单参数 - 程序0 */
    params.pulse_mode = X5_BURST_MODE; //刺激波形爆发模式
    params.base_freq = freq;
    params.low_power_mode = 0;
    params.program[0].tphase1 = width;
    params.program[0].tphase2 = width;
    params.program[0].tphase3 = 10;
    params.program[0].tinterphase1 = 10;
    params.program[0].tinterphase2 = 10;
    params.program[0].tinterpulse = 10;
    params.program[0].amplitude_p = ua;
    params.program[0].amplitude_n = ua;
    params.program[0].polar.dir = pin_dir;  /* 电极4 */
    params.program[0].polar.on = pin_on;
    /* 爆发参数 */
    params.burst_pulse_cnt = burst_cnt;
    log("Burst Parameters:");
    log("  Pulse Count per Burst: %d", burst_cnt);

    int err = x5_rf_config_burst(&params);
    if (err == 0) {
        if (pwm_duty > 0) {
            rf_pwm_chang(pwm_duty, freq);
        }
        log("Burst stimulation started");
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 X5BURST, test_x5_burst,
                 "X5: Burst stim [pwm%%] [w(ms)] [freq] [ma] [dir] [on] [cnt]");
#endif
