#include "leadinno_app.h"


#define ENABLE_TEST_X4
/*==============================================================================
 * 调试日志宏
 *============================================================================*/

#ifdef  ENABLE_TEST_X4

static int test_set_rf_power(uint8_t power)
{
    if(power > 15) {
        power = 15;
    }
    logInfo("set_rf power is %d", power);
    st25r3916_set_rf_power_level(power);
    st25r3916_open_rf(OPEN);
    lead.stim_pulse_power = power;
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD01,test_set_rf_power,"set rf_power");

static int test_state_rf(uint8_t key)
{
    if(key >2) {
        key = 2;
    }
    if(key == CLOSE) {
        logInfo("close rf!!!");
    } else {
        logInfo("open rf!!!");
    }
    st25r3916_open_rf(key);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD02,test_state_rf,"open close rf");

static int send_query_test(x4_query_mode_e mode)
{
    int err;
    if(mode == X4_OLD_POWER_QUERY) {
        err = __x4_rf_send_query();
    } else if (mode == X4_SET_POWER_QUERY) {
        SET_RF_POWER(lead.set_query_power);
        err = __x4_rf_send_query();
    } else if (mode == X4_ALL_POWER_QUERY) {
        for(uint8_t i=0; i<12; i++) {
            SET_RF_POWER(i);
            err = __x4_rf_send_query();
            if(err == 0) {
                logInfo(">>>>>>>>>>>>>>> poer = %d ok", i);
            } else {
                logInfo("ERROR############## poer = %d fail", i);
            }
        }
    } else if (mode == X4_AUTO_POWER_QUERY) {
        __x4_rf_send_query();
    }
    return err;
}

static int test_query(x4_query_mode_e mode, uint8_t power) //模式 0  以之前的query的功率query   1 设置 发送功率   2 所有功率通讯  3 自动query
{
    logInfo("query mode = %d", mode);
    if(mode == X4_SET_POWER_QUERY) {
        lead.set_query_power = power;
    }
    send_query_test(mode);
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD03,test_query,"test qury  [arg] 0 1 2 3");

static int test_get_vrecp(void)
{
    leadinno_x4_crtl(LEAD_GET_VRECP);
    logInfo("VRECP value: %.3f V", lead.vrecp);  // 打印获取到的电压值
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD04,test_get_vrecp,"get vrecp");

static int test_get_temp(void)
{
    leadinno_x4_crtl(LEAD_GET_TEMP);
    logInfo("Temperature value: %.2fC", lead.temp);  // 打印获取到的温度值
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD05,test_get_temp,"get temp");

static int test_x4_reset(void)
{
    leadinno_x4_crtl(LEAD_RESET);
    logInfo("X4 reset command executed");  // 打印复位命令执行信息
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD06,test_x4_reset,"test reset");

static void test_x4_write_efuse(uint32_t efuse)
{
    logInfo("Writing efuse - Input value: 0x%x (hex), %u (dec)", efuse, efuse);
    lead.write_efuse = efuse;
    leadinno_x4_crtl(LEAD_WRITE_EFUSE);

    // 如果需要验证写入后的值，可以添加读取efuse的命令
    // leadinno_x4_crtl(LEAD_GET_EFUSE);
    // logInfo("EFUSE readback: 0x%x", lead.get_efuse);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD07,test_x4_write_efuse,"write efuse");

static void test_x4_res(uint8_t res_pin_base)
{
    logInfo("Setting impedance measurement - Base channel: %u", res_pin_base);
    lead.set_res_ch = res_pin_base;
    leadinno_x4_crtl(LEAD_GET_RES);
    // 打印其他7个电极的阻抗值
    for(int i = 1; i < 8; i++) {
        // 计算实际电极编号：基准电极编号 + i，并确保在0-7范围内
        uint8_t electrode_num = (res_pin_base + i) % 8;
        logInfo("[%d - %d]: %dR", res_pin_base, electrode_num, lead.get_res[i]);
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD08,test_x4_res,"res test");

                                        //占空比     脉宽         频率       电流 0.1ma单位      电极方向         电极开关
static void test_out_pluse_simple(uint8_t pwm_puty, uint16_t w, uint16_t hz, uint16_t ma_0_1, uint8_t pin_dir, uint8_t pin_on, uint8_t power, uint8_t pwm_mode_en)
{
    lead.stim_pulse_power = power;  //设置刺激功率

    // 初始化 simple 参数结构体
    lead.simple.pulse_mode = X4_SIMPLE_MODE;
    lead.simple.low_power_mode = false;
    lead.simple.base_freq = hz;  // 计算周期 us
    lead.simple.pulse_forward = w;  // 将ms转换为us
    lead.simple.pulse_backward = w ;  // 将ms转换为us
    lead.simple.time_interphase1 = 10;  // 间隔10us
    lead.simple.time_interphase2 = 10;  // 间隔10us
    lead.simple.time_phase3 = 100;  // 默认值
    lead.simple.time_interpulse = 100;  // 默认值
    lead.simple.range.amplitude_forward = ma_0_1;
    lead.simple.range.amplitude_backward = ma_0_1;
    lead.simple.stim_strategy = 0;  // 默认主动平衡

    // 配置电极
    lead.simple.polar.dir = pin_dir;
    lead.simple.polar.on = pin_on;

    lead.simple.pwm_duty = pwm_puty;
    lead.simple.pwm_sync_mode = pwm_mode_en;

    // 打印所有配置参数
    logInfo("===== Simple Pulse Stimulation Configuration =====");
    logInfo("Input Parameters:");
    logInfo("  PWM Duty           : %u %%", pwm_puty);
    logInfo("  Pulse Width        : %u ms", w);
    logInfo("  Frequency          : %u Hz", hz);
    logInfo("  Amplitude          : %u uA", ma_0_1*100);
    logInfo("  Polarity Direction : 0x%02X", pin_dir);
    logInfo("  Polarity Enable    : 0x%02X", pin_on);

    logInfo("Configured Parameters (us):");
    logInfo("  Period             : %u us", lead.simple.base_freq);
    logInfo("  Forward Pulse      : %u us", lead.simple.pulse_forward);
    logInfo("  Backward Pulse     : %u us", lead.simple.pulse_backward);
    logInfo("  Interphase 1       : %u us", lead.simple.time_interphase1);
    logInfo("  Interphase 2       : %u us", lead.simple.time_interphase2);
    logInfo("  Phase 3            : %u us", lead.simple.time_phase3);
    logInfo("  Interpulse         : %u us", lead.simple.time_interpulse);
    logInfo("  Forward Amplitude  : %u uA", lead.simple.amplitude_forward);
    logInfo("  Backward Amplitude : %u uA", lead.simple.amplitude_backward);
    logInfo("==================================================");

    // 调用控制接口输出脉冲
    leadinno_x4_crtl(LEAD_OUT_PULSE_SIMPLE);

    // 配置PWM输出
    rf_pwm_chang(pwm_puty, hz);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD09, test_out_pluse_simple, "out simple pulse: pwm_duty(%) w(ms) hz(Hz) ma(mA) pin_dir pin_on");

 static void test_out_pluse_burst(uint8_t pwm_puty, uint8_t w, uint16_t hz, uint16_t ma_0_1, uint8_t pin_dir, uint8_t pin_on, uint8_t burst_count)
{
//    // 初始化 simple 参数结构体
//    lead.simple.low_power_mode = false;
//    lead.simple.base_freq = hz;  // 计算周期 us
//    lead.simple.pulse_forward = w;      // 将ms转换为us
//    lead.simple.pulse_backward = w;     // 将ms转换为us
//    lead.simple.time_interphase1 = 10;  // 间隔10us
//    lead.simple.time_interphase2 = 10;  // 间隔10us
//    lead.simple.time_phase3 = 100;      // 平衡时间100us
//    lead.simple.time_interpulse = 100;  // 脉冲间隔100us
//
//    lead.simple.range.amplitude_forward = ma_0_1;
//    lead.simple.range.amplitude_backward = ma_0_1;
//    lead.simple.stim_strategy = 0;      // 默认主动平衡
//
//    // 配置电极
//    lead.simple.polar.dir = pin_dir;
//    lead.simple.polar.on = pin_on;
//
//    // 配置爆发参数
//    lead.burst.period = lead.simple.period;
//    lead.burst.pulse_count = burst_count;
//
//    // 计算爆发间隔
//    uint32_t single_pulse_total = lead.simple.pulse_forward + lead.simple.pulse_backward +
//                                   lead.simple.time_interphase1 + lead.simple.time_interphase2 +
//                                   lead.simple.time_phase3 + lead.simple.time_interpulse;
//
//    uint32_t burst_total = single_pulse_total * burst_count;
//
//    if (burst_total < lead.simple.period) {
//        lead.burst.inter_burst = lead.simple.period - burst_total;
//    } else {
//        lead.burst.inter_burst = 1000;  // 默认间隔1ms
//        logInfo("Warning: Burst total time exceeds period, using default inter_burst=1000us");
//    }
//
//    lead.burst.invert_div = 0;
//    lead.burst.invert_ori_period = lead.simple.period;
//
//    // 打印所有配置参数
//    logInfo("===== Burst Pulse Stimulation Configuration =====");
//    logInfo("Input Parameters:");
//    logInfo("  PWM Duty           : %u %%", pwm_puty);
//    logInfo("  Pulse Width        : %u ms", w);
//    logInfo("  Frequency          : %u Hz", hz);
//    logInfo("  Amplitude          : %u uA", ma_0_1);
//    logInfo("  Polarity Direction : 0x%02X", pin_dir);
//    logInfo("  Polarity Enable    : 0x%02X", pin_on);
//    logInfo("  Burst Count        : %u", burst_count);
//    logInfo("Configured Parameters:");
//    logInfo("  Simple Parameters (us):");
//    logInfo("    Period           : %u us", lead.simple.period);
//    logInfo("    Forward Pulse    : %u us", lead.simple.pulse_forward);
//    logInfo("    Backward Pulse   : %u us", lead.simple.pulse_backward);
//    logInfo("    Interphase 1     : %u us", lead.simple.time_interphase1);
//    logInfo("    Interphase 2     : %u us", lead.simple.time_interphase2);
//    logInfo("    Phase 3          : %u us", lead.simple.time_phase3);
//    logInfo("    Interpulse       : %u us", lead.simple.time_interpulse);
//    logInfo("    Forward Amplitude: %u uA", lead.simple.amplitude_forward);
//    logInfo("    Backward Amplitude: %u uA", lead.simple.amplitude_backward);
//    logInfo("  Burst Parameters:");
//    logInfo("    Pulse Count      : %u", lead.burst.pulse_count);
//    logInfo("    Inter Burst      : %u us", lead.burst.inter_burst);
//    logInfo("==================================================");

    // 调用控制接口输出脉冲
    leadinno_x4_crtl(LEAD_OUT_PULSE_BURST);

    // 配置PWM输出
    rf_pwm_chang(pwm_puty, hz);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD10, test_out_pluse_burst, "out burst pulse: pwm_duty(%) w(ms) hz(Hz) ma(mA) pin_dir pin_on burst_count");


/**
 * @brief 交替刺激(TWA)测试函数 - A/B波形相同，幅度和电极不同
 * @param pwm_puty PWM占空比 (%)
 * @param w 脉宽 (ms)
 * @param hz 频率 (Hz)
 * @param ma_a A组幅度 (0.1mA单位)
 * @param ma_b B组幅度 (0.1mA单位)
 * @param pin_dir_a A组电极方向
 * @param pin_on_a A组电极开关
 * @param pin_dir_b B组电极方向
 * @param pin_on_b B组电极开关
 */
static void test_out_pluse_twa(uint8_t pwm_puty, uint8_t w, uint16_t hz,
                               uint16_t ma_a, uint16_t ma_b,
                               uint8_t pin_dir_a, uint8_t pin_on_a)
{
    // 初始化TWA参数结构体
    x4_TWA_params twa_params = {0};

    // 计算周期 us
    uint32_t period = 1000000 / hz;

    // 将ms转换为us
    uint32_t pulse_width = w * 1000;

    /*************************** 配置A组参数 ***************************/
    twa_params.puse_a.low_power_mode = false;
    twa_params.puse_a.pulse_forward = pulse_width;      ///< 前向波形脉宽 us
    twa_params.puse_a.time_interphase1 = 2;   ///< 前向波形与后向波形的间隔  us 1-255us
    twa_params.puse_a.pulse_backward = pulse_width;     ///< 后向波形脉宽 us
    twa_params.puse_a.time_interphase2 = 1;   ///< 后向波形与time_phase3的间隔  us 1-255us
    twa_params.puse_a.time_phase3 = 1;        ///< 被动平衡延时 us 1-16383us
    twa_params.puse_a.time_interpulse = 100;    ///< 被动平衡之后的延时 us  1-65535us
    twa_params.puse_a.amplitude_forward = ma_a;  ///< 前向波形幅度 单位ua
    twa_params.puse_a.amplitude_backward = ma_a; ///< 后向波形幅度 单位ua

    // A组电极配置
    twa_params.polar_a.dir = pin_dir_a;
    twa_params.polar_a.on = pin_on_a;
    // A组幅度配置
    twa_params.range_a.amplitude_forward = ma_a;
    twa_params.range_a.amplitude_backward = ma_a;

    twa_params.puse_b.low_power_mode = false;
    twa_params.puse_b.pulse_forward = pulse_width;      ///< 前向波形脉宽 us
    twa_params.puse_b.time_interphase1 = 10;   ///< 前向波形与后向波形的间隔  us 1-255us
    twa_params.puse_b.pulse_backward = pulse_width;     ///< 后向波形脉宽 us
    twa_params.puse_b.time_interphase2 = 10;   ///< 后向波形与time_phase3的间隔  us 1-255us
    twa_params.puse_b.time_phase3 = 10;        ///< 被动平衡延时 us 1-16383us
    twa_params.puse_b.time_interpulse = 100;    ///< 被动平衡之后的延时 us  1-65535us
    twa_params.puse_b.amplitude_forward = ma_a;  ///< 前向波形幅度 单位ua
    twa_params.puse_b.amplitude_backward = ma_a; ///< 后向波形幅度 单位ua

    // B组电极配置（不同）
    twa_params.polar_b.dir = pin_dir_a;
    twa_params.polar_b.on = pin_on_a;
    // B组幅度配置（不同）
    twa_params.range_b.amplitude_forward = ma_b;
    twa_params.range_b.amplitude_backward = ma_b;

    // 保存参数到全局结构体
    memcpy(&lead.TWA, &twa_params, sizeof(x4_TWA_params));

    // 调用控制接口输出交替脉冲
    leadinno_x4_crtl(LEAD_OUT_PULSE_TWA);

    // 配置PWM输出
    rf_pwm_chang(pwm_puty, hz);
}

// Shell命令导出 - 交替刺激测试
// 参数: pwm_duty(%) w(ms) hz(Hz) ma_a(0.1mA) ma_b(0.1mA) pin_dir_a pin_on_a pin_dir_b pin_on_b
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 CMD11,
                 test_out_pluse_twa,
                 "TWA test: pwm_duty(%) w(ms) hz(Hz) ma_a(0.1mA) ma_b(0.1mA) dir_a on_a dir_b on_b");

static void test_stop_or_run_pulse(uint8_t run_flag)
{
    if(run_flag) {
        logInfo("run pulse");
//        leadinno_x4_crtl(LEAD_RUN_PULSE);
    } else {
        logInfo("stop pulse");
        leadinno_x4_crtl(LEAD_STOP_PULSE);
    }
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD13,test_stop_or_run_pulse,"set 1 run   0 stop");


static void test_get_pulse_i_stim(void)
{
    leadinno_x4_crtl(LEAD_GET_I_STIM);
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD14,test_get_pulse_i_stim,"test_get_pulse_i_stim");

#endif
