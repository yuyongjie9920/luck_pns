///**
// * @file lmp1616x1_test.c
// * @brief LMP1616X1 芯片测试函数
// * @version 1.0
// * @date 2026-04-07
// *
// * 基于LMP1616X1芯片规格，支持16个电极
// * 提供完整的Shell命令接口用于测试
// */
//
//#include "leadinno_app.h"
//#include "LMP1616X1.h"
//#include "main_user.h"
//#include "string.h"
//
////#define ENABLE_TEST_LMP1616X1
//
///*==============================================================================
// * 调试宏
// *============================================================================*/
//
//#ifdef ENABLE_TEST_LMP1616X1
//#if 1
//    #define log(...) logInfo(__VA_ARGS__)
//#else
//    #define log(...) do {} while(0)
//#endif
//
///*==============================================================================
// * 测试函数
// *============================================================================*/
//
///**
// * @brief 设置RF功率
// * @param power 功率值 0-15
// * @return 0-成功，-1-失败
// *
// * 设置RF发射功率，功率值越大，通信距离越远
// */
//static int test_lmp1616x1_set_power(uint8_t power)
//{
//    if (power > 15) power = 15;
//    log("Set RF power: %d", power);
//    st25r3916_set_rf_power_level(power);
//    st25r3916_open_rf(OPEN);
//    lead.stim_pulse_power = power;
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16PWR, test_lmp1616x1_set_power, "LMP1616X1: Set RF power (0-15)");
//
///**
// * @brief 广播Query测试
// * @return 0-成功，-1-失败
// *
// * 发送广播Query命令，获取芯片状态和efuse ID
// */
//static int test_lmp1616x1_query_broadcast(void)
//{
//    int err = lmp1616x1_rf_send_query();
//    if (err == 0) {
//        log("===== LMP1616X1 Query Response =====");
//        log("Status Value       : 0x%04x", lmp1616x1.last_query_status.value);
//        log("EFUSE ID           : 0x%02x", lmp1616x1.last_query_status.bits.efuse_id_low);
//        log("");
//        log("Bit15 - UnderVolt occurred     : %d", lmp1616x1.last_query_status.bits.undervolt);
//        log("Bit14 - Program0 active        : %d", lmp1616x1.last_query_status.bits.prog0_active);
//        log("Bit13 - Overlap occurred       : %d", lmp1616x1.last_query_status.bits.overlap);
//        log("Bit12 - Stimulation complete   : %d", lmp1616x1.last_query_status.bits.stim_complete);
//        log("Bit11 - Temperature done       : %d", lmp1616x1.last_query_status.bits.temp_done);
//        log("Bit10 - Voltage/Current done   : %d", lmp1616x1.last_query_status.bits.volt_done);
//        log("Bit9  - Impedance done         : %d", lmp1616x1.last_query_status.bits.imp_done);
//        log("Bit8  - Reserved               : %d", lmp1616x1.last_query_status.bits.reserved);
//        log("Bit7-0- EFUSE ID low 8 bits    : 0x%02x", lmp1616x1.last_query_status.bits.efuse_id_low);
//        log("==========================");
//    }
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16QRY, test_lmp1616x1_query_broadcast, "LMP1616X1: Broadcast query");
//
///**
// * @brief 指定功率Query测试
// * @param power 功率值 0-15
// * @return 0-成功，-1-失败
// *
// * 使用指定功率发送Query命令
// */
//static int test_lmp1616x1_query_with_power(uint8_t power)
//{
//    int err = lmp1616x1_rf_send_query_with_power(power);
//    if (err == 0) {
//        log("Query with power %d: OK", power);
//    } else {
//        log("Query with power %d: FAIL", power);
//    }
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16QRYPWR, test_lmp1616x1_query_with_power, "LMP1616X1: Query with power [power]");
//
///**
// * @brief 所有功率点Query测试
// * @return 0-成功，-1-失败
// *
// * 测试所有功率点(0-15)的通信情况
// */
//static int test_lmp1616x1_query_all_power(void)
//{
//    log("===== LMP1616X1 All Power Test =====");
//    int success_count = 0;
//    for (uint8_t power = 0; power < 15; power++) {
//        SET_RF_POWER(power);
//        int err = __lmp1616x1_rf_send_query();
//        log("Power %2d: %s", power, err == 0 ? "Communication OK" : "Communication FAIL");
//        if (err == 0) success_count++;
//    }
//    log("Success Count: %d/15", success_count);
//    log("==============================");
//
//    /* 恢复默认功率 */
//    test_lmp1616x1_set_power(0);
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16QRYA, test_lmp1616x1_query_all_power, "LMP1616X1: All power points test");
//
///**
// * @brief 寄存器读写测试
// * @param reg 寄存器地址
// * @param data 要写入的数据
// * @return 0-成功，-1-失败
// *
// * 测试寄存器读写功能，先写入再读取验证
// */
//static int test_lmp1616x1_rfid_rw(uint8_t reg, uint16_t data)
//{
//    uint16_t readback = 0;
//
//    /* 确保efuse_id已获取 */
//    if (lmp1616x1.efuse_id == 0) {
//        lmp1616x1_rf_send_query();
//    }
//
//    /* 写入寄存器 */
//    int err = lmp1616x1_cmd_write(reg, data);
//
//    if (err == 0) {
//        /* 等待稳定 */
//        delay_ms(10);
//
//        /* 读取验证 */
//        err = lmp1616x1_cmd_read(reg, &readback);
//        if (err == 0) {
//            log("RFID R/W Test: Register 0x%02x -> Write 0x%04x, Read 0x%04x %s",
//                reg, data, readback, (readback == data) ? "MATCH" : "MISMATCH");
//        } else {
//            log("RFID Read Test FAIL: Register 0x%02x", reg);
//        }
//    } else {
//        log("RFID Write Test FAIL: Register 0x%02x, Data 0x%04x", reg, data);
//    }
//
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16RFID, test_lmp1616x1_rfid_rw, "LMP1616X1: RFID test [reg] [data]");
//
///**
// * @brief 获取整流电压测试
// * @return 0-成功，-1-失败
// *
// * 测量芯片整流电压(VRECP)
// */
//static int test_lmp1616x1_get_vrecp(void)
//{
//    float vrecp[2] = {0};
//    int err = lmp1616x1_rf_get_vrecp(vrecp, 0);
//    if (err == 0) {
//        log("Rectified Voltage: %.2fV", vrecp[0]);
//    } else {
//        log("Get Rectified Voltage FAIL");
//    }
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16VRECP, test_lmp1616x1_get_vrecp, "LMP1616X1: Get rectified voltage");
//
///**
// * @brief 延迟测量电压测试
// * @param delay 延迟周期数
// * @return 0-成功，-1-失败
// *
// * 在刺激开始后延迟指定周期测量电压
// */
//static int test_lmp1616x1_get_vrecp_delay(uint16_t delay)
//{
//    float vrecp[2] = {0};
//    int err = lmp1616x1_rf_get_vrecp(vrecp, delay);
//    if (err == 0) {
//        log("Delayed Voltage Measurement:");
//        log("  Min before stimulation: %.2fV", vrecp[0]);
//        log("  Max during stimulation: %.2fV", vrecp[1]);
//    }
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16VRD, test_lmp1616x1_get_vrecp_delay, "LMP1616X1: Delayed voltage measurement [cycles]");
//
///**
// * @brief 获取温度测试
// * @return 0-成功，-1-失败
// *
// * 测量芯片温度
// * 公式: OUT_RTV = 1.48716 - 0.021598 * T
// */
//static int test_lmp1616x1_get_temp(void)
//{
//    float temp = 0;
//    int err = lmp1616x1_rf_get_temp(&temp);
//    if (err == 0) {
//        log("Chip Temperature: %.1fC", temp);
//    } else {
//        log("Get Temperature FAIL");
//    }
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16TEMP, test_lmp1616x1_get_temp, "LMP1616X1: Get temperature");
//
///**
// * @brief 获取阻抗测试
// * @param base_ch 基准通道(0-7)
// * @param osc 振荡器修调(0-1)
// * @return 0-成功，-1-失败
// *
// * 测量电极阻抗
// * 公式: res_cal = (res[i] * 2.7717 - 5722) / f_mhz
// */
//static int test_lmp1616x1_get_res(uint8_t base_ch, uint8_t osc)
//{
//    uint16_t res[16] = {0};
//    int err = lmp1616x1_rf_get_res(base_ch, res, osc);
//
//    if (err == 0) {
//        log("Impedance Measurement Results (Base Channel %d):", base_ch);
//        for (int i = 0; i < 16; i++) {
//            if (i == 0) {
//                log("  Reference Channel: %dR (Raw: 0x%03x)", res[i], res[i]);
//            } else {
//                log("  Electrode %2d: %dR", (base_ch + i) % 16, res[i]);
//            }
//        }
//    }
//
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16RES, test_lmp1616x1_get_res, "LMP1616X1: Get impedance [base_ch(0-7)] [osc(0-1)]");
//
///**
// * @brief 获取电流测试
// * @return 0-成功，-1-失败
// *
// * 测量刺激电流(正向和负向)
// */
//static int test_lmp1616x1_get_current(void)
//{
//    float p = 0, n = 0;
//    int err = lmp1616x1_rf_get_current(&p, &n);
//    if (err == 0) {
//        log("Stimulation Current Measurement:");
//        log("  Positive Current: %.2f mA", p);
//        log("  Negative Current: %.2f mA", n);
//    } else {
//        log("Get Current FAIL");
//    }
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16CUR, test_lmp1616x1_get_current, "LMP1616X1: Get stimulation current");
//
///**
// * @brief 写eFuse测试
// * @param efuse 24位efuse值(十六进制)
// * @return 0-成功，-1-失败
// *
// * 烧写eFuse，会改变芯片ID
// * 注意: 此操作不可逆
// */
//static int test_lmp1616x1_write_efuse(uint32_t efuse)
//{
//    log("Burning eFuse: 0x%06x", efuse & 0xFFFFFF);
//    log("WARNING: This operation will change chip ID and is irreversible!");
//
//    int err = lmp1616x1_rf_write_efuse(efuse);
//    if (err == 0) {
//        log("eFuse Write SUCCESS");
//    } else {
//        log("eFuse Write FAIL");
//    }
//
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16EFW, test_lmp1616x1_write_efuse, "LMP1616X1: Write eFuse [24-bit hex]");
//
///**
// * @brief 读eFuse测试
// * @return 0-成功，-1-失败
// *
// * 读取芯片eFuse值
// */
//static int test_lmp1616x1_read_efuse(void)
//{
//    uint32_t efuse = 0;
//    int err = lmp1616x1_rf_read_efuse(&efuse);
//    if (err == 0) {
//        log("eFuse Value: 0x%06x", efuse);
//        log("  Low 16 bits: 0x%04x", efuse & 0xFFFF);
//        log("  High 8 bits: 0x%02x", (efuse >> 16) & 0xFF);
//    } else {
//        log("Read eFuse FAIL");
//    }
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16EFR, test_lmp1616x1_read_efuse, "LMP1616X1: Read eFuse");
//
///**
// * @brief 停止刺激测试
// * @return 0-成功，-1-失败
// *
// * 立即停止所有刺激输出
// */
//static int test_lmp1616x1_stop_pulse(void)
//{
//    log("Stop Stimulation Output");
//    int err = lmp1616x1_rf_stop_pulse();
//    if (err == 0) {
//        log("Stimulation Stopped");
//    } else {
//        log("Stop Stimulation FAIL");
//    }
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16STOP, test_lmp1616x1_stop_pulse, "LMP1616X1: Stop stimulation");
//
///**
// * @brief 复位芯片测试
// * @return 0-成功，-1-失败
// *
// * 软件复位芯片，所有寄存器恢复到默认值
// */
//static int test_lmp1616x1_reset(void)
//{
//    log("Reset Chip");
//    int err = lmp1616x1_rf_reset();
//    if (err == 0) {
//        log("Reset SUCCESS, waiting for reinitialization...");
//        delay_ms(100);
//
//        /* 复位后重新查询 */
//        lmp1616x1_rf_send_query();
//        log("Re-query Complete");
//    } else {
//        log("Reset FAIL");
//    }
//
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16RST, test_lmp1616x1_reset, "LMP1616X1: Reset chip");
//
///**
// * @brief 简单刺激测试
// * @param pwm_duty PWM占空比(%)
// * @param width 脉冲宽度(us)
// * @param freq 刺激频率(Hz)
// * @param ua 刺激电流(uA)
// * @param electrode_pair 电极对(阳极*10+阴极，如12表示电极1->2)
// * @param power RF功率(0-15)
// *
// * 配置简单模式刺激，在指定电极对之间输出刺激
// */
//static void test_lmp1616x1_simple(uint8_t pwm_duty, uint16_t width, uint16_t freq,
//                                 uint16_t ua, uint8_t electrode_pair, uint8_t power)
//{
//    lmp1616x1_simple_params_t params = {0};
//
//    /* 设置刺激功率 */
//    lead.stim_pulse_power = power;
//
//    /* 确保efuse_id已获取 */
//    if (lmp1616x1.efuse_id == 0) {
//        lmp1616x1_rf_send_query();
//    }
//
//    /* 解析电极对 */
//    uint8_t anode = electrode_pair / 10;      /* 十位为阳极 */
//    uint8_t cathode = electrode_pair % 10;    /* 个位为阴极 */
//
//    /* 电极有效性检查 */
//    if (anode >= 16) {
//        log("ERROR: Anode number %d out of range (0-15)", anode);
//        return;
//    }
//    if (cathode >= 16) {
//        log("ERROR: Cathode number %d out of range (0-15)", cathode);
//        return;
//    }
//
//    uint16_t pin_dir = (1 << anode);
//    uint16_t pin_on = (1 << anode) | (1 << cathode);
//
//    log("Electrode Pair Config: Anode=%d, Cathode=%d", anode, cathode);
//    log("pin_dir = 0x%04x, pin_on = 0x%04x", pin_dir, pin_on);
//
//    /* 配置参数 */
//    params.pulse_mode = LMP1616X1_STIM_MODE_SIMPLE;  /* 简单模式 */
//    params.program_cnt = 0;                          /* 使用程序0 */
//    params.low_power_mode = 0;                       /* 正常功耗模式 */
//    params.base_freq = freq;                         /* 基准频率 */
//
//    /* 配置程序0 */
//    params.program[0].tphase1 = width;               /* 正脉冲宽度 */
//    params.program[0].tphase2 = width;               /* 负脉冲宽度 */
//    params.program[0].tphase3 = 10;                  /* 电荷平衡时间 */
//    params.program[0].tinterphase1 = 10;             /* 正负脉冲间隔1 */
//    params.program[0].tinterphase2 = 10;             /* 正负脉冲间隔2 */
//
//    /* 设置所有电极的幅度 */
//    for (int i = 0; i < 16; i++) {
//        params.program[0].amplitude_p[i] = (i == anode) ? ua : 0;
//        params.program[0].amplitude_n[i] = (i == cathode) ? ua : 0;
//    }
//
//    params.program[0].polar.dir = pin_dir;           /* 电极方向 */
//    params.program[0].polar.on = pin_on;             /* 电极使能 */
//    params.program[0].multiple = 1;                  /* 频率倍率 */
//
//    /* 相位配置 */
//    params.phase.stim_clk_div = 0;                   /* 时钟不分频 */
//    params.phase.phase1_config = LMP1616X1_PHASE_DAC_1;  /* 每相位1个DAC */
//    params.phase.phase2_config = LMP1616X1_PHASE_DAC_1;  /* 每相位1个DAC */
//
//    /* 打印参数 */
//    log("Simple Stimulation Parameters:");
//    log("  Frequency: %d Hz", freq);
//    log("  Pulse Width: %d us", width);
//    log("  Current: %d uA", ua);
//    log("  Anode: Electrode %d", anode);
//    log("  Cathode: Electrode %d", cathode);
//    log("  PWM Duty: %d%%", pwm_duty);
//    log("  RF Power: %d", power);
//
//    /* 配置刺激 */
//    int err = lmp1616x1_rf_config_simple(&params);
//    if (err == 0) {
//        if (pwm_duty > 0) {
//            rf_pwm_chang(pwm_duty, freq);
//            log("PWM Enabled: duty=%d%%, freq=%dHz", pwm_duty, freq);
//        }
//        log("Simple Stimulation Started SUCCESS!");
//    } else {
//        log("Stimulation Configuration FAILED!");
//    }
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16SIMPLE, test_lmp1616x1_simple,
//                 "LMP1616X1: Simple stimulation [pwm%%] [w(us)] [freq] [ua] [electrode_pair] [power]");
//
///**
// * @brief 多程序刺激测试
// * @param electrode_pair 电极对(阳极*10+阴极)
// * @param pwm_duty PWM占空比(%)
// * @param freq 刺激频率(Hz)
// * @param program_cnt 程序数量(0-3对应1-4个程序)
// *
// * 测试多程序刺激功能，最多支持4个程序交替运行
// */
//static void test_lmp1616x1_multi_prog(uint8_t electrode_pair, uint8_t pwm_duty,
//                                      uint16_t freq, uint8_t program_cnt)
//{
//    lmp1616x1_simple_params_t params = {0};
//
//    /* 解析电极对 */
//    uint8_t anode = electrode_pair / 10;
//    uint8_t cathode = electrode_pair % 10;
//
//    /* 电极有效性检查 */
//    if (anode >= 16) anode = 0;
//    if (cathode >= 16) cathode = 0;
//
//    uint16_t pin_dir = (1 << anode);
//    uint16_t pin_on = (1 << anode) | (1 << cathode);
//
//    /* 确保efuse_id已获取 */
//    if (lmp1616x1.efuse_id == 0) {
//        lmp1616x1_rf_send_query();
//    }
//
//    log("===== LMP1616X1 Multi-Program Test =====");
//    log("Chip EFUSE ID: 0x%02x", lmp1616x1.efuse_id);
//    log("Electrode Pair: %d->%d", anode, cathode);
//    log("Program Count: %d", program_cnt + 1);
//
//    /* 基本配置 */
//    params.pulse_mode = LMP1616X1_STIM_MODE_SIMPLE;  /* 简单模式 */
//    params.program_cnt = (program_cnt < 3) ? program_cnt : 3;  /* 限制最大3 */
//    params.low_power_mode = 0;                       /* 正常功耗 */
//    params.base_freq = freq;                         /* 基准频率 */
//
//    /* 相位配置 */
//    params.phase.stim_clk_div = 0;                   /* 时钟不分频 */
//    params.phase.phase1_config = LMP1616X1_PHASE_DAC_1;  /* 每相位1个DAC */
//    params.phase.phase2_config = LMP1616X1_PHASE_DAC_1;  /* 每相位1个DAC */
//
//    /* 配置各个程序 */
//    for (int prog = 0; prog <= params.program_cnt && prog < MAX_PROGRAM_NUM; prog++) {
//        params.program[prog].tphase1 = 100 + prog * 50;  /* 递增的脉冲宽度 */
//        params.program[prog].tphase2 = 100 + prog * 50;
//        params.program[prog].tphase3 = 10;
//        params.program[prog].tinterphase1 = 10;
//        params.program[prog].tinterphase2 = 10;
//        params.program[prog].multiple = 1 << prog;  /* 频率倍率递增 */
//
//        /* 设置幅度 */
//        for (int i = 0; i < 16; i++) {
//            params.program[prog].amplitude_p[i] = (i == anode) ? (1000 + prog * 500) : 0;
//            params.program[prog].amplitude_n[i] = (i == cathode) ? (1000 + prog * 500) : 0;
//        }
//
//        params.program[prog].polar.dir = pin_dir;  /* 电极方向 */
//        params.program[prog].polar.on = pin_on;    /* 电极使能 */
//
//        log("Program %d: Width=%dus, Current=%duA",
//            prog, params.program[prog].tphase1, 1000 + prog * 500);
//    }
//
//    int err = lmp1616x1_rf_config_more_simple(&params);
//    if (err == 0) {
//        if (pwm_duty > 0) {
//            rf_pwm_chang(pwm_duty, freq);
//            log("PWM Enabled: duty=%d%%, freq=%dHz", pwm_duty, freq);
//        }
//        log("Multi-Program Stimulation Started SUCCESS!");
//    } else {
//        log("Multi-Program Stimulation Configuration FAILED!");
//    }
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16MPROG, test_lmp1616x1_multi_prog,
//                 "LMP1616X1: Multi-program test [electrode_pair] [pwm%%] [freq] [program_cnt]");
//
///**
// * @brief 突发模式刺激测试
// * @param pwm_duty PWM占空比(%)
// * @param width 脉冲宽度(us)
// * @param freq 刺激频率(Hz)
// * @param ua 刺激电流(uA)
// * @param electrode_pair 电极对(阳极*10+阴极)
// * @param burst_cnt 每簇脉冲数(1-255)
// * @param power RF功率(0-15)
// *
// * 突发模式刺激，输出脉冲簇
// */
//static void test_lmp1616x1_burst(uint8_t pwm_duty, uint16_t width, uint16_t freq,
//                                uint16_t ua, uint8_t electrode_pair, uint8_t burst_cnt, uint8_t power)
//{
//    lmp1616x1_simple_params_t params = {0};
//
//    /* 设置刺激功率 */
//    lead.stim_pulse_power = power;
//
//    /* 解析电极对 */
//    uint8_t anode = electrode_pair / 10;
//    uint8_t cathode = electrode_pair % 10;
//
//    /* 电极有效性检查 */
//    if (anode >= 16) anode = 0;
//    if (cathode >= 16) cathode = 0;
//
//    uint16_t pin_dir = (1 << anode);
//    uint16_t pin_on = (1 << anode) | (1 << cathode);
//
//    /* 确保efuse_id已获取 */
//    if (lmp1616x1.efuse_id == 0) {
//        lmp1616x1_rf_send_query();
//    }
//
//    /* 参数检查 */
//    if (burst_cnt == 0 || burst_cnt > 255) {
//        log("ERROR: Burst count %d out of range (1-255)", burst_cnt);
//        return;
//    }
//
//    /* 配置参数 */
//    params.pulse_mode = LMP1616X1_STIM_MODE_BURST;  /* 突发模式 */
//    params.program_cnt = 0;                          /* 使用程序0 */
//    params.low_power_mode = 0;                       /* 正常功耗 */
//    params.base_freq = freq;                         /* 基准频率 */
//    params.burst_pulse_cnt = burst_cnt;              /* 每簇脉冲数 */
//    params.burst_tinter = 1000;                      /* 1ms簇间隔 */
//
//    /* 配置程序0 */
//    params.program[0].tphase1 = width;               /* 正脉冲宽度 */
//    params.program[0].tphase2 = width;               /* 负脉冲宽度 */
//    params.program[0].tphase3 = 10;                  /* 电荷平衡 */
//    params.program[0].tinterphase1 = 10;             /* 间隔1 */
//    params.program[0].tinterphase2 = 10;             /* 间隔2 */
//
//    /* 设置幅度 */
//    for (int i = 0; i < 16; i++) {
//        params.program[0].amplitude_p[i] = (i == anode) ? ua : 0;
//        params.program[0].amplitude_n[i] = (i == cathode) ? ua : 0;
//    }
//
//    params.program[0].polar.dir = pin_dir;           /* 电极方向 */
//    params.program[0].polar.on = pin_on;             /* 电极使能 */
//    params.program[0].multiple = 1;                  /* 频率倍率 */
//
//    /* 相位配置 */
//    params.phase.stim_clk_div = 0;                   /* 时钟不分频 */
//    params.phase.phase1_config = LMP1616X1_PHASE_DAC_1;  /* 每相位1个DAC */
//    params.phase.phase2_config = LMP1616X1_PHASE_DAC_1;  /* 每相位1个DAC */
//
//    log("Burst Stimulation Parameters:");
//    log("  Frequency: %d Hz", freq);
//    log("  Pulse Width: %d us", width);
//    log("  Current: %d uA", ua);
//    log("  Burst Count: %d pulses/burst", burst_cnt);
//    log("  Electrode Pair: %d->%d", anode, cathode);
//    log("  PWM Duty: %d%%", pwm_duty);
//    log("  RF Power: %d", power);
//
//    int err = lmp1616x1_rf_config_burst(&params);
//    if (err == 0) {
//        if (pwm_duty > 0) {
//            rf_pwm_chang(pwm_duty, freq);
//            log("PWM Enabled: duty=%d%%, freq=%dHz", pwm_duty, freq);
//        }
//        log("Burst Stimulation Started SUCCESS!");
//    } else {
//        log("Burst Stimulation Configuration FAILED!");
//    }
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16BURST, test_lmp1616x1_burst,
//                 "LMP1616X1: Burst stimulation [pwm%%] [w(us)] [freq] [ua] [electrode_pair] [burst_cnt] [power]");
//
///**
// * @brief 软启动刺激测试
// * @param width 脉冲宽度(us)
// * @param freq 刺激频率(Hz)
// * @param ua 刺激电流(uA)
// * @param electrode_pair 电极对(阳极*10+阴极)
// * @param inc_value 软启动增量值(1-31)
// * @param step 软启动步进周期数(1-1023)
// *
// * 测试软启动功能，刺激幅度逐渐增加
// */
//static void test_lmp1616x1_soft_simple(uint16_t width, uint16_t freq,
//                                      uint16_t ua, uint8_t electrode_pair,
//                                      uint8_t inc_value, uint8_t step)
//{
//    lmp1616x1_softstart_params_t soft = {0};
//
//    /* 参数检查 */
//    if (inc_value < 1 || inc_value > 31) {
//        log("ERROR: Softstart increment value %d out of range (1-31)", inc_value);
//        return;
//    }
//    if (step < 1 || step > 1023) {
//        log("ERROR: Softstart step count %d out of range (1-1023)", step);
//        return;
//    }
//
//    /* 配置软启动 */
//    soft.enable = 1;
//    for (int i = 0; i < 16; i++) {
//        soft.inc_value[i] = inc_value;  /* 所有DAC使用相同的增量值 */
//    }
//    soft.step = step;
//
//    log("Configure Softstart:");
//    log("  Enable: Yes");
//    log("  Increment Value: %d", inc_value);
//    log("  Step Count: %d", step);
//
//    /* 配置软启动参数 */
//    int err = lmp1616x1_rf_config_softstart(LMP1616X1_PROGRAM_0, &soft);
//    if (err != 0) {
//        log("Softstart Configuration FAILED!");
//        return;
//    }
//
//    /* 使能软停止 */
//    err = lmp1616x1_rf_config_softstop(1);
//    if (err != 0) {
//        log("Softstop Configuration FAILED!");
//        return;
//    }
//
//    /* 启动简单刺激 */
//    test_lmp1616x1_simple(100, width, freq, ua, electrode_pair, 0);
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16SOFT, test_lmp1616x1_soft_simple,
//                 "LMP1616X1: Soft-start stimulation [w(us)] [freq] [ua] [electrode_pair] [inc_value] [step]");
//
///**
// * @brief 软停止测试
// *
// * 停止所有刺激输出
// */
//static void test_lmp1616x1_soft_stop(void)
//{
//    log("Execute Soft Stop");
//    lmp1616x1_rf_stop_pulse();
//    log("Stimulation Stopped");
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16SOFT_STOP, test_lmp1616x1_soft_stop,
//                 "LMP1616X1: Soft-stop stimulation");
//
///**
// * @brief 自动测试序列
// *
// * 执行完整的自动测试序列
// */
//static void test_lmp1616x1_auto_test(void)
//{
//    log("===== LMP1616X1 Auto Test Sequence Start =====");
//
//    /* 1. 查询芯片 */
//    log("Step 1: Broadcast Query");
//    if (test_lmp1616x1_query_broadcast() != 0) {
//        log("Query FAIL, test aborted");
//        return;
//    }
//
//    /* 2. 测试电压测量 */
//    log("Step 2: Voltage Measurement");
//    test_lmp1616x1_get_vrecp();
//
//    /* 3. 测试温度测量 */
//    log("Step 3: Temperature Measurement");
//    test_lmp1616x1_get_temp();
//
//    /* 4. 测试阻抗测量 */
//    log("Step 4: Impedance Measurement");
//    test_lmp1616x1_get_res(0, 1);
//
//    /* 5. 测试简单刺激 */
//    log("Step 5: Simple Stimulation Test");
//    test_lmp1616x1_simple(100, 100, 100, 1000, 12, 5);
//    delay_ms(1000);
//
//    /* 6. 测试电流测量 */
//    log("Step 6: Stimulation Current Measurement");
//    test_lmp1616x1_get_current();
//
//    /* 7. 停止刺激 */
//    log("Step 7: Stop Stimulation");
//    test_lmp1616x1_stop_pulse();
//
//    log("===== LMP1616X1 Auto Test Sequence Complete =====");
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMP16AUTO, test_lmp1616x1_auto_test,
//                 "LMP1616X1: Auto test sequence");
//
//#endif /* ENABLE_TEST_LMP1616X1 */
