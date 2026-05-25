///**
// * @file lmrs1616_test.c
// * @brief LMRS1616X1A 测试功能 - 根据测试方案文档
// * @version 2.0
// * @date 2026-04-07
// */
//
//#include "leadinno_app.h"
//#include "LMRS1616X1.h"
//#include "main_user.h"
//#include "string.h"
//
//#define ENABLE_TEST_LMRS1616
//
///*==============================================================================
// * 宏定义
// *============================================================================*/
//
//#ifdef  ENABLE_TEST_LMRS1616
//#if 1
//    #define log(...) logInfo(__VA_ARGS__)
//#else
//    #define log(...) do {} while(0)
//#endif
//
///*==============================================================================
// * 基本功能测试
// *============================================================================*/
//
///**
// * @brief 设置RF功率
// * @param power 功率值 0-15
// */
//static int test_lmrs1616_set_power(uint8_t power)
//{
//    if (power > 15) power = 15;
//    log("Set RF power: %d", power);
//    st25r3916_set_rf_power_level(power);
//    st25r3916_open_rf(OPEN);
//    lead.stim_pulse_power = power;
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616PWR, test_lmrs1616_set_power, "LMRS1616: Set RF power (0-15)");
//
///**
// * @brief RF开关控制
// * @param onoff 1-开启, 0-关闭
// */
//static int test_lmrs1616_rf_onoff(uint8_t onoff)
//{
//    if (onoff) {
//        log("RF ON");
//        st25r3916_open_rf(OPEN);
//    } else {
//        log("RF OFF");
//        st25r3916_open_rf(CLOSE);
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616RF, test_lmrs1616_rf_onoff, "LMRS1616: RF ON/OFF (1-on, 0-off)");
//
///*==============================================================================
// * Query测试 - 测试方案文档4.3
// *============================================================================*/
//
///**
// * @brief 广播Query - 获取响应
// */
//static int test_lmrs1616_query_broadcast(void)
//{
//    int err = lmrs1616_rf_send_query();
//    if (err == 0) {
//        log("===== Query Response =====");
//        log("Status Value : 0x%04x", lead.lmrs1616_query.value);
//        log("EFUSE ID     : 0x%02x", lead.lmrs1616_query.bits.efuse_id);
//        log("");
//        log("Bit15 - UnderVolt occurred     : %d", lead.lmrs1616_query.bits.undervolt);
//        log("Bit14 - Program0 active        : %d", lead.lmrs1616_query.bits.prog0_active);
//        log("Bit13 - Overlap occurred       : %d", lead.lmrs1616_query.bits.overlap);
//        log("Bit12 - Stimulation complete   : %d", lead.lmrs1616_query.bits.stim_complete);
//        log("Bit11 - Temperature done       : %d", lead.lmrs1616_query.bits.temp_done);
//        log("Bit10 - Voltage/Current done   : %d", lead.lmrs1616_query.bits.volt_done);
//        log("Bit9  - Impedance done         : %d", lead.lmrs1616_query.bits.imp_done);
//        log("Bit8  - Reserved               : %d", lead.lmrs1616_query.bits.reserved);
//        log("Bit7-0- EFUSE ID low 8 bits    : 0x%02x", lead.lmrs1616_query.bits.efuse_id);
//        log("==========================");
//    }
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616QRYB, test_lmrs1616_query_broadcast, "LMRS1616: Broadcast query");
//
///**
// * @brief 指定功率Query测试
// * @param power 功率值 0-15
// */
//static int test_lmrs1616_query_with_power(uint8_t power)
//{
//    int err = lmrs1616_rf_send_query_with_power(power);
//    if (err == 0) {
//        log("Query with power is %d OK", power);
//    }
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616QRYPWR, test_lmrs1616_query_with_power, "LMRS1616: Query with power [power]");
//
///**
// * @brief 所有功率点Query测试
// */
//static int test_lmrs1616_query_all_power(void)
//{
//    lmrs1616_rf_send_query_all_power();
//    test_lmrs1616_set_power(0);
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616QRYA, test_lmrs1616_query_all_power, "LMRS1616: Query all power levels");
//
///*==============================================================================
// * 寄存器读写测试
// *============================================================================*/
//
///**
// * @brief 寄存器读写测试
// * @param reg 寄存器地址
// * @param data 要写入的数据
// */
//static int test_lmrs1616_rfid_rw(uint8_t reg, uint16_t data)
//{
//    uint16_t readback = 0;
//
//    // 确认efuse_id
//    if (lmrs1616.efuse_id == 0) {
//        lmrs1616_rf_send_query();
//    }
//
//    int err = lmrs1616_cmd_write(reg, data);
//
//    if (err == 0) {
//        delay_ms(10);
//        err = lmrs1616_cmd_read(reg, &readback);
//        if (err == 0) {
//            log("RFID Readback: reg 0x%02x -> 0x%04x %s",
//                reg, readback, (readback == data) ? "MATCH" : "MISMATCH");
//        }
//    }
//
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616RFID, test_lmrs1616_rfid_rw, "LMRS1616: RFID test [reg] [data]");
//
///*==============================================================================
// * 测量功能测试 - 测试方案文档3.5.2-3.5.5
// *============================================================================*/
//
///**
// * @brief 获取整流电压测试
// */
//static int test_lmrs1616_get_vrecp(void)
//{
//    float vrecp[2] = {0};
//    if (lmrs1616_rf_get_vrecp(vrecp, 0) == 0) {
//        log("VRECP Voltage: %.2fV", vrecp[0]);
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616VRECP, test_lmrs1616_get_vrecp, "LMRS1616: Get VRECP voltage");
//
///**
// * @brief 延迟测量的电压测试
// * @param delay 延迟的刺激周期数
// */
//static int test_lmrs1616_get_vrecp_delay(uint16_t delay)
//{
//    float vrecp[2] = {0};
//    if (lmrs1616_rf_get_vrecp(vrecp, delay) == 0) {
//        log("VRECP Delayed Measurement:");
//        log("  Min (before stim): %.2fV", vrecp[0]);
//        log("  Max (during stim) : %.2fV", vrecp[1]);
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616VRD, test_lmrs1616_get_vrecp_delay, "LMRS1616: Get VRECP with delay [cycles]");
//
///**
// * @brief 获取温度测试
// */
//static int test_lmrs1616_get_temp(void)
//{
//    float temp = 0;
//    if (lmrs1616_rf_get_temp(&temp) == 0) {
//        log("Temperature: %.1fC", temp);
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616TEMP, test_lmrs1616_get_temp, "LMRS1616: Get temperature");
//
///**
// * @brief 获取阻抗测试
// * @param base_ch 基准通道(0-7)
// * @param osc 时钟trim选择
// */
//static int test_lmrs1616_get_res(uint8_t base_ch, uint8_t osc)
//{
//    uint16_t res[16] = {0};
//    lmrs1616_rf_get_res(base_ch, res, osc);
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616RES, test_lmrs1616_get_res, "LMRS1616: Get impedance [base_ch(0-7)] [osc]");
//
///**
// * @brief 获取刺激电流测试
// */
//static int test_lmrs1616_get_current(void)
//{
//    float p = 0, n = 0;
//    if (lmrs1616_rf_get_current(&p, &n) == 0) {
//        log("Stimulation Current:");
//        log("  Positive: %.2f mA", p);
//        log("  Negative: %.2f mA", n);
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616CUR, test_lmrs1616_get_current, "LMRS1616: Get stimulation current");
//
///*==============================================================================
// * eFuse操作测试
// *============================================================================*/
//
///**
// * @brief 写eFuse测试
// * @param efuse 24位efuse值(十六进制)
// */
//static int test_lmrs1616_write_efuse(uint32_t efuse)
//{
//    log("Writing EFUSE: 0x%06x", efuse & 0xFFFFFF);
//    log("Note: This will reset the chip and change its ID");
//    return lmrs1616_rf_write_efuse(efuse);
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616EFW, test_lmrs1616_write_efuse, "LMRS1616: Write EFUSE [24-bit hex]");
//
///**
// * @brief 读eFuse测试
// */
//static int test_lmrs1616_read_efuse(void)
//{
//    uint32_t efuse = 0;
//    if (lmrs1616_rf_read_efuse(&efuse) == 0) {
//        log("EFUSE Value: 0x%06x", efuse);
//        log("  Low 16 bits: 0x%04x", efuse & 0xFFFF);
//        log("  High 8 bits : 0x%02x", (efuse >> 16) & 0xFF);
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616EFR, test_lmrs1616_read_efuse, "LMRS1616: Read EFUSE");
//
///*==============================================================================
// * 刺激控制测试
// *============================================================================*/
//
///**
// * @brief 停止刺激测试
// */
//static int test_lmrs1616_stop_pulse(void)
//{
//    log("Stop stimulation");
//    return lmrs1616_rf_stop_pulse();
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616STOP, test_lmrs1616_stop_pulse, "LMRS1616: Stop stimulation");
//
///**
// * @brief 复位芯片测试
// */
//static int test_lmrs1616_reset(void)
//{
//    log("Reset chip");
//    rf_reset_chip();
//    delay_ms(100);
//    // 复位后查询
//    lmrs1616_rf_send_query();
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616RST, test_lmrs1616_reset, "LMRS1616: Reset chip");
//
///*==============================================================================
// * 简单模式刺激测试
// *============================================================================*/
//
///**
// * @brief 简单模式刺激测试
// * @param pwm_duty PWM占空比(%)
// * @param width 脉宽(us)
// * @param freq 频率(Hz)
// * @param ua 电流(μA)
// * @param stim_on 电极选择(格式: 阳极编号+阴极编号, 如12表示电极1-2)
// * @param power 功率等级
// */
//static void test_lmrs1616_simple(uint8_t pwm_duty, uint16_t width, uint16_t freq,
//                                uint16_t ua, uint8_t stim_on, uint8_t power)
//{
//    lmrs1616_simple_params_t params = {0};
//
//    lead.stim_pulse_power = power;  // 设置刺激功率
//
//    // 先查询获取efuse_id
//    if (lmrs1616.efuse_id == 0) {
//        lmrs1616_rf_send_query();
//    }
//
//    uint8_t pin_dir, pin_on;
//    // 解析电极配置，例如stim_on=12表示电极1-2
//    uint8_t anode = stim_on / 10;  // 十位数表示阳极
//    uint8_t cathode = stim_on % 10;  // 个位数表示阴极
//
//    pin_dir = (1 << anode) | (0 << cathode);
//    pin_on = (1 << anode) | (1 << cathode);
//
//    log("Electrode Configuration:");
//    log("  Anode: %d, Cathode: %d", anode, cathode);
//    log("  Direction bits: 0x%04x", pin_dir);
//    log("  Enable bits: 0x%04x", pin_on);
//
//    params.pulse_mode = LMRS1616_SIMPLE_MODE;
//    params.program_cnt = 0;
//    params.low_power_mode = 0;
//    params.base_freq = freq;
//
//    // 程序0配置
//    params.program[0].tphase1 = width;
//    params.program[0].tphase2 = width;
//    params.program[0].tphase3 = 10;
//    params.program[0].tinterphase1 = 10;
//    params.program[0].tinterphase2 = 10;
//    params.program[0].amplitude_p = ua;
//    params.program[0].amplitude_n = ua;
//    params.program[0].polar.dir = pin_dir;
//    params.program[0].polar.on = pin_on;
//    params.program[0].multiple = 1;
//
//    // 打印配置参数
//    log("Stimulation Parameters:");
//    log("  Mode: Simple");
//    log("  Frequency: %d Hz", freq);
//    log("  Pulse Width: %d us", width);
//    log("  Current: %d uA", ua);
//    log("  Anode: Electrode %d", anode);
//    log("  Cathode: Electrode %d", cathode);
//    log("  RF Power Level: %d", power);
//
//    int err = lmrs1616_rf_config_simple(&params);
//    if (err == 0) {
//        if (pwm_duty > 0) {
//            rf_pwm_chang(pwm_duty, freq);
//            log("PWM enabled: duty=%d%%, freq=%dHz", pwm_duty, freq);
//        }
//        log("Simple stimulation started - OK!");
//    } else {
//        log("Stimulation configuration FAILED!");
//    }
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616SIMPLE, test_lmrs1616_simple,
//                 "LMRS1616: Simple stim [pwm%%] [w(us)] [freq] [ua] [electrode] [power]");
//
///*==============================================================================
// * 软启动刺激测试
// *============================================================================*/
//
///**
// * @brief 软启动刺激测试
// * @param width 脉宽(us)
// * @param freq 频率(Hz)
// * @param ua 电流(μA)
// * @param stim_on 电极选择
// * @param inc_value 增量值
// * @param step 步进周期数
// */
//static void test_lmrs1616_soft_simple(uint16_t width, uint16_t freq, uint16_t ua,
//                                     uint8_t stim_on, uint8_t inc_value, uint8_t step)
//{
//    __lmrs1616_rf_reset();
//
//    lmrs1616_softstart_params_t soft;
//    soft.enable = 1;
//    soft.inc_value = inc_value;
//    soft.step = step;
//
//    lmrs1616_rf_config_softstart(LMRS1616_PROGRAM_0, &soft);
//    lmrs1616_rf_config_softstop(1);
//
//    test_lmrs1616_simple(100, width, freq, ua, stim_on, 0);
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616SOFT, test_lmrs1616_soft_simple,
//                 "LMRS1616: Soft start stim [w(us)] [freq] [ua] [electrode] [inc] [step]");
//
///**
// * @brief 软停止测试
// */
//static void test_lmrs1616_soft_stop(void)
//{
//    lmrs1616_rf_stop_pulse();
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616SOFT_STOP, test_lmrs1616_soft_stop,
//                 "LMRS1616: Soft stop stimulation");
//
///*==============================================================================
// * 多程序刺激测试
// *============================================================================*/
//
///**
// * @brief 多程序刺激测试
// * @param stim_on 电极选择
// * @param pwm_duty PWM占空比
// * @param freq 频率
// * @param program 程序数量
// * @param multiple0 程序0倍数
// * @param multiple1 程序1倍数
// * @param multiple2 程序2倍数
// * @param multiple3 程序3倍数
// */
//static void test_lmrs1616_multi_prog(uint8_t stim_on, uint8_t pwm_duty, uint16_t freq, uint8_t program,
//                                    uint16_t multiple0, uint16_t multiple1, uint16_t multiple2, uint16_t multiple3)
//{
//    lmrs1616_simple_params_t params = {0};
//
//    if (lmrs1616.efuse_id == 0) {
//        lmrs1616_rf_send_query();
//    }
//
//    uint8_t anode = stim_on / 10;
//    uint8_t cathode = stim_on % 10;
//    uint8_t pin_dir = (1 << anode) | (0 << cathode);
//    uint8_t pin_on = (1 << anode) | (1 << cathode);
//
//    log("===== LMRS1616 Multi-Program Test =====");
//    log("Electrode: Anode=%d, Cathode=%d", anode, cathode);
//
//    uint8_t w = 200;  // 默认脉宽200us
//
//    params.pulse_mode = LMRS1616_SIMPLE_MODE;
//    params.program_cnt = program;
//    params.low_power_mode = 0;
//    params.base_freq = freq;
//
//    // 程序0配置
//    if(program >= 0) {
//        params.program[0].tphase1 = w;
//        params.program[0].tphase2 = w;
//        params.program[0].tinterphase1 = 10;
//        params.program[0].tinterphase2 = 10;
//        params.program[0].multiple = multiple0;
//        params.program[0].tphase3 = 10;
//        params.program[0].amplitude_p = 1000;  // 1mA
//        params.program[0].amplitude_n = 1000;
//        params.program[0].polar.dir = pin_dir;
//        params.program[0].polar.on = pin_on;
//        log("Program 0: Electrode %d-%d, 200us, 1.0mA", anode, cathode);
//    }
//
//    // 程序1配置
//    if(program >= 1) {
//        params.program[1].tphase1 = w;
//        params.program[1].tphase2 = w;
//        params.program[1].tinterphase1 = 10;
//        params.program[1].tinterphase2 = 10;
//        params.program[1].multiple = multiple1;
//        params.program[1].tphase3 = 10;
//        params.program[1].amplitude_p = 2000;  // 2mA
//        params.program[1].amplitude_n = 2000;
//        params.program[1].polar.dir = pin_dir;
//        params.program[1].polar.on = pin_on;
//        log("Program 1: Electrode %d-%d, 200us, 2.0mA", anode, cathode);
//    }
//
//    // 程序2配置
//    if(program >= 2) {
//        params.program[2].tphase1 = w;
//        params.program[2].tphase2 = w;
//        params.program[2].tinterphase1 = 10;
//        params.program[2].tinterphase2 = 10;
//        params.program[2].multiple = multiple2;
//        params.program[2].tphase3 = 10;
//        params.program[2].amplitude_p = 3000;  // 3mA
//        params.program[2].amplitude_n = 3000;
//        params.program[2].polar.dir = pin_dir;
//        params.program[2].polar.on = pin_on;
//        log("Program 2: Electrode %d-%d, 200us, 3.0mA", anode, cathode);
//    }
//
//    // 程序3配置
//    if(program >= 3) {
//        params.program[3].tphase1 = w;
//        params.program[3].tphase2 = w;
//        params.program[3].tinterphase1 = 10;
//        params.program[3].tinterphase2 = 10;
//        params.program[3].multiple = multiple3;
//        params.program[3].tphase3 = 10;
//        params.program[3].amplitude_p = 4000;  // 4mA
//        params.program[3].amplitude_n = 4000;
//        params.program[3].polar.dir = pin_dir;
//        params.program[3].polar.on = pin_on;
//        log("Program 3: Electrode %d-%d, 200us, 4.0mA", anode, cathode);
//    }
//
//    log("Frequency: %dHz", freq);
//    log("Number of programs: %d", program + 1);
//
//    int err = lmrs1616_rf_config_more_simple(&params);
//    if (err == 0) {
//        if (pwm_duty > 0) {
//            rf_pwm_chang(pwm_duty, freq);
//        }
//        log("Multi-program stimulation started");
//    }
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616MPROG, test_lmrs1616_multi_prog,
//                 "LMRS1616: Multi-program test [electrode] [pwm%%] [freq] [prog] [mult0] [mult1] [mult2] [mult3]");
//
///*==============================================================================
// * 突发模式刺激测试
// *============================================================================*/
//
///**
// * @brief 突发模式刺激测试
// * @param pwm_duty PWM占空比
// * @param width 脉宽(us)
// * @param freq 频率(Hz)
// * @param ua 电流(μA)
// * @param stim_on 电极选择
// * @param burst_cnt 每burst脉冲数
// * @param power 功率等级
// */
//static void test_lmrs1616_burst(uint8_t pwm_duty, uint8_t width, uint16_t freq,
//                               uint16_t ua, uint8_t stim_on, uint8_t burst_cnt, uint8_t power)
//{
//    uint8_t anode = stim_on / 10;
//    uint8_t cathode = stim_on % 10;
//    uint8_t pin_dir = (1 << anode) | (0 << cathode);
//    uint8_t pin_on = (1 << anode) | (1 << cathode);
//
//    lead.stim_pulse_power = power;
//
//    lmrs1616_simple_params_t params = {0};
//    if (lmrs1616.efuse_id == 0) {
//        lmrs1616_rf_send_query();
//    }
//
//    // 基本参数配置
//    params.pulse_mode = LMRS1616_BURST_MODE;
//    params.base_freq = freq;
//    params.low_power_mode = 0;
//    params.program[0].tphase1 = width;
//    params.program[0].tphase2 = width;
//    params.program[0].tphase3 = 10;
//    params.program[0].tinterphase1 = 10;
//    params.program[0].tinterphase2 = 10;
//    params.program[0].amplitude_p = ua;
//    params.program[0].amplitude_n = ua;
//    params.program[0].polar.dir = pin_dir;
//    params.program[0].polar.on = pin_on;
//
//    // 突发模式参数
//    params.burst_pulse_cnt = burst_cnt;
//
//    log("Burst Mode Parameters:");
//    log("  Mode: Burst");
//    log("  Frequency: %d Hz", freq);
//    log("  Pulse Width: %d us", width);
//    log("  Current: %d uA", ua);
//    log("  Burst Count: %d", burst_cnt);
//    log("  Electrode: %d-%d", anode, cathode);
//
//    int err = lmrs1616_rf_config_burst(&params);
//    if (err == 0) {
//        if (pwm_duty > 0) {
//            rf_pwm_chang(pwm_duty, freq);
//        }
//        log("Burst stimulation started");
//    }
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMRS1616BURST, test_lmrs1616_burst,
//                 "LMRS1616: Burst stim [pwm%%] [w(us)] [freq] [ua] [electrode] [cnt] [power]");
//
//#endif
