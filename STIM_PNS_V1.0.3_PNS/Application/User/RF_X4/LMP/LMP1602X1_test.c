///**
// * @file lmp1602x1_test.c
// * @brief LMP1602X1 测试程序
// * @version 2.0
// * @date 2026-04-07
// */
//
//#include "leadinno_app.h"
//#include "LMP1602X1.h"
//#include "main_user.h"
//#include "string.h"
//
////#define ENABLE_TEST_LMP
//
///*==============================================================================
// * 调试宏
// *============================================================================*/
//
//#ifdef  ENABLE_TEST_LMP
//#if 1
//    #define log(...) logInfo(__VA_ARGS__)
//#else
//    #define log(...) do {} while(0)
//#endif
//
///*==============================================================================
// * 射频控制
// *============================================================================*/
//
///**
// * @brief 设置射频功率
// * @param power 功率值 0-15
// */
//static int test_lmp_set_power(uint8_t power)
//{
//    if (power > 15) power = 15;
//    log("Set RF power: %d", power);
//    st25r3916_set_rf_power_level(power);
//    st25r3916_open_rf(OPEN);
//    lead.stim_pulse_power = power;
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPPWR, test_lmp_set_power, "LMP: Set RF power (0-15)");
//
///**
// * @brief 射频开关
// * @param onoff 1-打开, 0-关闭
// */
//static int test_lmp_rf_onoff(uint8_t onoff)
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
//                 LMPRF, test_lmp_rf_onoff, "LMP: RF ON/OFF (1-on, 0-off)");
//
///*==============================================================================
// * Query测试
// *============================================================================*/
//
///**
// * @brief 广播Query - 获取响应
// */
//static int test_lmp_query_broadcast(void)
//{
//    int err = lmp_rf_send_query();
//    if (err == 0) {
//        log("===== Query Response =====");
//        log("Status Value : 0x%04x", lmp.last_status.value);
//        log("EFUSE ID     : 0x%02x", lmp.last_status.bits.efuse_id);
//        log("");
//        log("Bit15 - UnderVolt occurred     : %d", lmp.last_status.bits.undervolt);
//        log("Bit14 - Program0 active        : %d", lmp.last_status.bits.prog0_active);
//        log("Bit13 - Overlap occurred       : %d", lmp.last_status.bits.overlap);
//        log("Bit12 - Stimulation complete   : %d", lmp.last_status.bits.stim_complete);
//        log("Bit11 - Temperature done       : %d", lmp.last_status.bits.temp_done);
//        log("Bit10 - Voltage/Current done   : %d", lmp.last_status.bits.volt_done);
//        log("Bit9  - Impedance done         : %d", lmp.last_status.bits.imp_done);
//        log("Bit8  - Reserved               : %d", lmp.last_status.bits.reserved);
//        log("Bit7-0- EFUSE ID low 8 bits    : 0x%02x", lmp.last_status.bits.efuse_id);
//        log("==========================");
//    }
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPQRYB, test_lmp_query_broadcast, "LMP: Broadcast query");
//
///**
// * @brief 指定功率Query
// * @param power 功率值 0-15
// */
//static int test_lmp_query_with_power(uint8_t power)
//{
//    int err = lmp_rf_send_query_with_power(power);
//    if (err == 0) {
//        log("Query with power is %d OK", power);
//    }
//    return err;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPQRYPWR, test_lmp_query_with_power, "LMP: Query with power [power]");
//
///**
// * @brief 所有功率点Query
// */
//static int test_lmp_query_all_power(void)
//{
//    lmp_rf_send_query_all_power();
//    test_lmp_set_power(0);
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPQRYA, test_lmp_query_all_power, "LMP: Query all power levels");
//
///*==============================================================================
// * 测量功能测试
// *============================================================================*/
//
///**
// * @brief 获取整流电压
// */
//static int test_lmp_get_vrecp(void)
//{
//    float vrecp = 0;
//    if (lmp_rf_get_vrecp(&vrecp, 0) == 0) {
//        log("VRECP Voltage: %.2fV", vrecp);
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPVRECP, test_lmp_get_vrecp, "LMP: Get VRECP voltage");
//
///**
// * @brief 获取温度
// */
//static int test_lmp_get_temp(void)
//{
//    float temp = 0;
//    if (lmp_rf_get_temp(&temp) == 0) {
//        log("Temperature: %.1fC", temp);
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPTEMP, test_lmp_get_temp, "LMP: Get temperature");
//
///**
// * @brief 获取阻抗
// * @param base_ch 基准通道(0-7)
// * @param osc 时钟修调(0-默认,1-高精度)
// */
//static int test_lmp_get_res(uint8_t base_ch, uint8_t osc)
//{
//    uint16_t res[8] = {0};
//    lmp_rf_get_res(base_ch, res, osc);
//
//    log("Impedance Measurement Results:");
//    for (int i = 0; i < 8; i++) {
//        if (i == 0) {
//            log("  Reference Channel: %d R", res[i]);
//        } else {
//            log("  Channel %d: %d R", (base_ch + i) % 8, res[i]);
//        }
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPRES, test_lmp_get_res, "LMP: Get impedance [base_ch(0-7)] [osc(0/1)]");
//
///**
// * @brief 获取刺激电流
// * @param source 电流源(0-A,1-B)
// */
//static int test_lmp_get_current(uint8_t source)
//{
//    float p = 0, n = 0;
//    lmp_current_source_e src = (source == 0) ? LMP_CURRENT_SOURCE_A : LMP_CURRENT_SOURCE_B;
//
//    if (lmp_rf_get_current(&p, &n, src) == 0) {
//        log("Stimulation Current (%s):", (source == 0) ? "A" : "B");
//        log("  Positive: %.2f mA", p);
//        log("  Negative: %.2f mA", n);
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPCUR, test_lmp_get_current, "LMP: Get stimulation current [source(0-A,1-B)]");
//
///*==============================================================================
// * eFuse操作测试
// *============================================================================*/
//
///**
// * @brief 写eFuse
// * @param efuse 24位efuse值(十六进制)
// */
//static int test_lmp_write_efuse(uint32_t efuse)
//{
//    log("Writing EFUSE: 0x%06x", efuse & 0xFFFFFF);
//    log("Note: This will reset the chip and change its ID");
//    return lmp_rf_write_efuse(efuse);
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPEFW, test_lmp_write_efuse, "LMP: Write EFUSE [24-bit hex]");
//
///**
// * @brief 读eFuse
// */
//static int test_lmp_read_efuse(void)
//{
//    uint32_t efuse = 0;
//    if (lmp_rf_read_efuse(&efuse) == 0) {
//        log("EFUSE Value: 0x%06x", efuse);
//        log("  Low 16 bits: 0x%04x", efuse & 0xFFFF);
//        log("  High 8 bits : 0x%02x", (efuse >> 16) & 0xFF);
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPEFR, test_lmp_read_efuse, "LMP: Read EFUSE");
//
///*==============================================================================
// * 控制功能测试
// *============================================================================*/
//static int test_lmp_stop_pulse(void)
//{
//    log("Stop stimulation");
//    return lmp_rf_stop_pulse();
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPSTOP, test_lmp_stop_pulse, "LMP: Stop stimulation");
//
///**
// * @brief 软件复位芯片
// */
//static int test_lmp_reset(void)
//{
//    log("Reset chip");
//    rf_reset_chip();
//    delay_ms(100);
//    /* 复位后查询 */
//    lmp_rf_send_query();
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPRST, test_lmp_reset, "LMP: Reset chip");
//
//static int test_lmp_dual_program1(int argc, char *argv[])
//{
//    logDebug("%d parameter(s)", argc);
//
//    // Print parameter usage instructions
//    logInfo("=== Parameter Description ===");
//    logInfo("argv[1] : pwm_mode_en - PWM mode enable");
//    logInfo("argv[2] : soft_cont   - Soft control parameter");
//    logInfo("argv[3] : power       - Power parameter");
//    logInfo("argv[4] : source      - Source parameter");
//    logInfo("argv[5] : pwm_duty    - PWM duty cycle");
//    logInfo("argv[6] : width1      - Width 1");
//    logInfo("argv[7] : width2      - Width 2");
//    logInfo("argv[8] : freq        - Frequency");
//    logInfo("argv[9] : multiple_0  - Multiple 0");
//    logInfo("argv[10]: multiple_1  - Multiple 1");
//    logInfo("argv[11]: ua1         - uA parameter 1");
//    logInfo("argv[12]: ua2         - uA parameter 2");
//    logInfo("argv[13]: stim_on1    - Stimulation on 1");
//    logInfo("argv[14]: stim_on2    - Stimulation on 2");
//    logInfo("argv[15]: stim_on3    - Stimulation on 3");
//    logInfo("argv[16]: stim_on4    - Stimulation on 4");
//    logInfo("argv[17]: p_en_0      - enable_program0");
//    logInfo("argv[18]: p_en_1      - enable_program1");
//    logInfo("argv[19]: p_en_2      - enable_program2");
//    logInfo("argv[20]: p_en_3      - enable_program3");
//
//    // Check parameter count (need 21 total: argv[0] is program name + 20 parameters)
//    const int REQUIRED_PARAMS = 21;  // argc needs to be at least 21 (including program name)
//
//    if (argc < REQUIRED_PARAMS)
//    {
//        logError("Insufficient number of parameters! Current count: %d (including program name), Required: %d (including program name)", argc, REQUIRED_PARAMS);
//        logError("Actual parameter values passed: %d, Required: %d", argc - 1, REQUIRED_PARAMS - 1);
//        logError("Please provide all %d parameters as described above", REQUIRED_PARAMS - 1);
//        return -1;
//    }
//
//    logInfo("Parameter count check passed. Current configured parameters: %d (program name + %d parameters)", argc, argc - 1);
//
//    for (char i = 1; i < argc; i++)
//    {
//        logDebug("argv[%d] = %s", i, argv[i]);
//    }
//
//    // Parse command line parameters (in variable order: index starts from 1)
//    uint16_t pwm_mode_en = atoi(argv[1]);      // Parameter 1
//    uint16_t soft_cont = atoi(argv[2]);        // Parameter 2
//    uint8_t power = atoi(argv[3]);             // Parameter 3
//    uint8_t source = atoi(argv[4]);            // Parameter 4
//    uint8_t pwm_duty = atoi(argv[5]);          // Parameter 5
//    uint16_t width1 = atoi(argv[6]);           // Parameter 6
//    uint16_t width2 = atoi(argv[7]);           // Parameter 7
//    uint16_t freq = atoi(argv[8]);             // Parameter 8
//    uint8_t multiple_0 = atoi(argv[9]);        // Parameter 9
//    uint8_t multiple_1 = atoi(argv[10]);       // Parameter 10
//    uint16_t ua1 = atoi(argv[11]);             // Parameter 11
//    uint16_t ua2 = atoi(argv[12]);             // Parameter 12
//    uint8_t stim_on1 = atoi(argv[13]);         // Parameter 13
//    uint8_t stim_on2 = atoi(argv[14]);         // Parameter 14
//    uint8_t stim_on3 = atoi(argv[15]);         // Parameter 15
//    uint8_t stim_on4 = atoi(argv[16]);         // Parameter 16
//    uint8_t en_0 = atoi(argv[17]);             // Parameter 17
//    uint8_t en_1 = atoi(argv[18]);             // Parameter 18
//    uint8_t en_2 = atoi(argv[19]);             // Parameter 19
//    uint8_t en_3 = atoi(argv[20]);             // Parameter 20
//
//    // Print parsed parameter values
//    logInfo("=== Parsed Parameter Values ===");
//    logInfo("pwm_mode_en = %d", pwm_mode_en);
//    logInfo("soft_cont = %d", soft_cont);
//    logInfo("power = %d", power);
//    logInfo("source = %d", source);
//    logInfo("pwm_duty = %d", pwm_duty);
//    logInfo("width1 = %d", width1);
//    logInfo("width2 = %d", width2);
//    logInfo("freq = %d", freq);
//    logInfo("multiple_0 = %d", multiple_0);
//    logInfo("multiple_1 = %d", multiple_1);
//    logInfo("ua1 = %d", ua1);
//    logInfo("ua2 = %d", ua2);
//    logInfo("stim_on1 = %d", stim_on1);
//    logInfo("stim_on2 = %d", stim_on2);
//    logInfo("stim_on3 = %d", stim_on3);
//    logInfo("stim_on4 = %d", stim_on4);
//    logInfo("en_0 = %d", en_0);
//    logInfo("en_1 = %d", en_1);
//    logInfo("en_2 = %d", en_2);
//    logInfo("en_3 = %d", en_3);
//
//    lmp_simple_params_t params = {0};
//
//    rf_reset_chip();
//
//    lead.stim_pulse_power = power;  // Set stimulation power
//
//    /* Ensure valid efuse_id */
//    if (lmp.efuse_id == 0) {
//        lmp_rf_send_query();
//    }
//
//    // Configure electrode direction for program 1
//    uint8_t pin_dir1 = (1 << (stim_on1/10)) | (0 << (stim_on1%10));
//    uint8_t pin_on1 = (1 << (stim_on1/10)) | (1 << (stim_on1%10));
//
//    // Configure electrode direction for program 2
//    uint8_t pin_dir2 = (1 << (stim_on2/10)) | (0 << (stim_on2%10));
//    uint8_t pin_on2 = (1 << (stim_on2/10)) | (1 << (stim_on2%10));
//
//    // Configure electrode direction for program 3
//    uint8_t pin_dir3 = (1 << (stim_on3/10)) | (0 << (stim_on3%10));
//    uint8_t pin_on3 = (1 << (stim_on3/10)) | (1 << (stim_on3%10));
//
//    // Configure electrode direction for program 4
//    uint8_t pin_dir4 = (1 << (stim_on4/10)) | (0 << (stim_on4%10));
//    uint8_t pin_on4 = (1 << (stim_on4/10)) | (1 << (stim_on4%10));
//
//
//    params.pwm_duty = pwm_duty;
//    if(pwm_mode_en){
//        params.pwm_sync_mode = 1;
//        logInfo("PWM_MODE");
//    } else {
//        params.pwm_sync_mode = 0;
//    }
//
//    if(soft_cont > 0) {
//        /* 配置软启动 */
//        lmp_softstart_params_t soft;
//        soft.enable = 1;
//        soft.inc_value = 1;     // 每次增加的DAC值
//        soft.step = soft_cont;  // 刺激次数
//        lmp_rf_config_softstart(LMP_PROGRAM_0, &soft);
//    }
//
//
//    /* 配置参数 */
//    params.pulse_mode = LMP_SIMPLE_MODE;
//    params.program_cnt = 0;           // 双程序模式
//    params.low_power_mode = 0;
//    params.base_freq = freq;
//
//    /* 源选择限制 */
//    if(source > 2) {
//        source = 2;
//    }
//
//    if(en_0){
//        /* A程序0配置 (第一个刺激程序) */
//        params.p_en.en_0 = 1;
//        params.program[0][0].tphase1 = width1;      /* 正脉宽 */
//        params.program[0][0].tphase2 = width1;      /* 负脉宽 */
//        params.program[0][0].tphase3 = 20;          /* 电荷平衡时间 */
//        params.program[0][0].tinterphase1 = 10;     /* 10us */
//        params.program[0][0].tinterphase2 = 10;     /* 10us */
//        params.program[0][0].amplitude_p = ua1;     /* 刺激电流ua */
//        params.program[0][0].amplitude_n = ua1;     /* 刺激电流ua */
//        params.program[0][0].polar.dir = pin_dir1;  /* 电极方向 */
//        params.program[0][0].polar.on = pin_on1;
//        params.program[0][0].multiple = multiple_0;  /* 频率倍数 */
//
//        logInfo("=== Program en_0 Configuration ===");
//        logInfo("program[0][0].tphase1 = %d", params.program[0][0].tphase1);
//        logInfo("program[0][0].tphase2 = %d", params.program[0][0].tphase2);
//        logInfo("program[0][0].tphase3 = %d", params.program[0][0].tphase3);
//        logInfo("program[0][0].tinterphase1 = %d", params.program[0][0].tinterphase1);
//        logInfo("program[0][0].tinterphase2 = %d", params.program[0][0].tinterphase2);
//        logInfo("program[0][0].amplitude_p = %d", params.program[0][0].amplitude_p);
//        logInfo("program[0][0].amplitude_n = %d", params.program[0][0].amplitude_n);
//        logInfo("program[0][0].polar.dir = %d", params.program[0][0].polar.dir);
//        logInfo("program[0][0].polar.on = %d", params.program[0][0].polar.on);
//        logInfo("program[0][0].multiple = %d", params.program[0][0].multiple);
//    }
//
//    if(en_1) {
//        /* A程序1配置 (第二个刺激程序) */
//        params.p_en.en_1 = 1;
//        params.program[0][1].tphase1 = width2;      /* 正脉宽 */
//        params.program[0][1].tphase2 = width2;      /* 负脉宽 */
//        params.program[0][1].tphase3 = 20;          /* 电荷平衡时间 */
//        params.program[0][1].tinterphase1 = 10;     /* 10us */
//        params.program[0][1].tinterphase2 = 10;     /* 10us */
//        params.program[0][1].amplitude_p = ua2;     /* 刺激电流ua */
//        params.program[0][1].amplitude_n = ua2;     /* 刺激电流ua */
//        params.program[0][1].polar.dir = pin_dir2;  /* 电极方向 */
//        params.program[0][1].polar.on = pin_on2;
//        params.program[0][1].multiple = multiple_1;  /* 频率倍数 */
//
//        logInfo("=== Program en_1 Configuration ===");
//        logInfo("program[0][1].tphase1 = %d", params.program[0][1].tphase1);
//        logInfo("program[0][1].tphase2 = %d", params.program[0][1].tphase2);
//        logInfo("program[0][1].tphase3 = %d", params.program[0][1].tphase3);
//        logInfo("program[0][1].tinterphase1 = %d", params.program[0][1].tinterphase1);
//        logInfo("program[0][1].tinterphase2 = %d", params.program[0][1].tinterphase2);
//        logInfo("program[0][1].amplitude_p = %d", params.program[0][1].amplitude_p);
//        logInfo("program[0][1].amplitude_n = %d", params.program[0][1].amplitude_n);
//        logInfo("program[0][1].polar.dir = %d", params.program[0][1].polar.dir);
//        logInfo("program[0][1].polar.on = %d", params.program[0][1].polar.on);
//        logInfo("program[0][1].multiple = %d", params.program[0][1].multiple);
//    }
//
//
//    /* 配置并启动刺激 */
//    int err = lmp_rf_config_simple(&params);
//    delay_ms(5000);
//    if(en_2) {
//        params.p_en.en_2 = 1;
//        /* B程序0配置 (第三个刺激程序) */
//        params.program[1][0].tphase1 = width1;      /* 正脉宽 */
//        params.program[1][0].tphase2 = width1;      /* 负脉宽 */
//        params.program[1][0].tphase3 = 20;          /* 电荷平衡时间 */
//        params.program[1][0].tinterphase1 = 10;     /* 10us */
//        params.program[1][0].tinterphase2 = 10;     /* 10us */
//        params.program[1][0].amplitude_p = ua1;     /* 刺激电流ua */
//        params.program[1][0].amplitude_n = ua1;     /* 刺激电流ua */
//        params.program[1][0].polar.dir = pin_dir3;  /* 电极方向 */
//        params.program[1][0].polar.on = pin_on3;
//        params.program[1][0].multiple = multiple_0;  /* 频率倍数 */
//
//        logInfo("=== Program en_2 Configuration ===");
//        logInfo("program[1][0].tphase1 = %d", params.program[1][0].tphase1);
//        logInfo("program[1][0].tphase2 = %d", params.program[1][0].tphase2);
//        logInfo("program[1][0].tphase3 = %d", params.program[1][0].tphase3);
//        logInfo("program[1][0].tinterphase1 = %d", params.program[1][0].tinterphase1);
//        logInfo("program[1][0].tinterphase2 = %d", params.program[1][0].tinterphase2);
//        logInfo("program[1][0].amplitude_p = %d", params.program[1][0].amplitude_p);
//        logInfo("program[1][0].amplitude_n = %d", params.program[1][0].amplitude_n);
//        logInfo("program[1][0].polar.dir = %d", params.program[1][0].polar.dir);
//        logInfo("program[1][0].polar.on = %d", params.program[1][0].polar.on);
//        logInfo("program[1][0].multiple = %d", params.program[1][0].multiple);
//    }
//
//    if(en_3) {
//        params.p_en.en_3 = 1;
//        /* B程序1配置 (第四个刺激程序) */
//        params.program[1][1].tphase1 = width2;      /* 正脉宽 */
//        params.program[1][1].tphase2 = width2;      /* 负脉宽 */
//        params.program[1][1].tphase3 = 20;          /* 电荷平衡时间 */
//        params.program[1][1].tinterphase1 = 10;     /* 10us */
//        params.program[1][1].tinterphase2 = 10;     /* 10us */
//        params.program[1][1].amplitude_p = ua2;     /* 刺激电流ua */
//        params.program[1][1].amplitude_n = ua2;     /* 刺激电流ua */
//        params.program[1][1].polar.dir = pin_dir4;  /* 电极方向 */
//        params.program[1][1].polar.on = pin_on4;
//        params.program[1][1].multiple = multiple_1;  /* 频率倍数 */
//
//        logInfo("=== Program en_3 Configuration ===");
//        logInfo("program[1][1].tphase1 = %d", params.program[1][1].tphase1);
//        logInfo("program[1][1].tphase2 = %d", params.program[1][1].tphase2);
//        logInfo("program[1][1].tphase3 = %d", params.program[1][1].tphase3);
//        logInfo("program[1][1].tinterphase1 = %d", params.program[1][1].tinterphase1);
//        logInfo("program[1][1].tinterphase2 = %d", params.program[1][1].tinterphase2);
//        logInfo("program[1][1].amplitude_p = %d", params.program[1][1].amplitude_p);
//        logInfo("program[1][1].amplitude_n = %d", params.program[1][1].amplitude_n);
//        logInfo("program[1][1].polar.dir = %d", params.program[1][1].polar.dir);
//        logInfo("program[1][1].polar.on = %d", params.program[1][1].polar.on);
//        logInfo("program[1][1].multiple = %d", params.program[1][1].multiple);
//    }
//
//    /* 配置并启动刺激 */
//    err = lmp_rf_config_simple(&params);
//    if (err == 0) {
//        if (pwm_duty > 0) {
//            if(freq > 15) {
//                rf_pwm_chang(params.pwm_duty, freq);
//            } else {
//                rf_pwm_chang(params.pwm_duty, freq);
//            }
//            log("PWM enabled: duty=%d%%, freq=%dHz", pwm_duty, freq);
//        }
//        log("Dual program stimulation started - OK!");
//    } else {
//        log("Stimulation configuration FAILED!");
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), LMPDUAL_2, test_lmp_dual_program1, "test");
//
//
//static void test_lmp_dual_program(uint8_t pwm_duty, uint16_t width1, uint16_t width2,
//                                  uint16_t freq,  uint8_t multiple_1, uint16_t ua1, uint16_t ua2,
//                                  uint8_t stim_on1, uint8_t stim_on2,
//                                   uint8_t a, uint8_t b)
//{
//
////
////    logInfo("a = %d b = %d", a, b);
////    multiple_0 = 1;
////    power = 0;
////    source = 0;
////    pwm_mode_en = 0;
////    soft_cont = 0;
////
////    logInfo("=== test_lmp_dual_program called with:");
////    logInfo("pwm_duty=%d, width1=%d, width2=%d, freq=%d", pwm_duty, width1, width2, freq);
////    logInfo("multiple_0=%d, multiple_1=%d, ua1=%d, ua2=%d", multiple_0, multiple_1, ua1, ua2);
////    logInfo("stim_on1=%d, stim_on2=%d, power=%d, source=%d", stim_on1, stim_on2, power, source);
////    logInfo("pwm_mode_en=%d, soft_cont=%d", pwm_mode_en, soft_cont);
////
////    lmp_simple_params_t params = {0};
////
////    rf_reset_chip();
////
////    lead.stim_pulse_power = power;  // 设置刺激功率
////
////    /* 确保有有效的efuse_id */
////    if (lmp.efuse_id == 0) {
////        lmp_rf_send_query();
////    }
////
////    // 配置第一个程序的电极方向
////    uint8_t pin_dir1 = (1 << (stim_on1/10)) | (0 << (stim_on1%10));
////    uint8_t pin_on1 = (1 << (stim_on1/10)) | (1 << (stim_on1%10));
////
////    // 配置第二个程序的电极方向
////    uint8_t pin_dir2 = (1 << (stim_on2/10)) | (0 << (stim_on2%10));
////    uint8_t pin_on2 = (1 << (stim_on2/10)) | (1 << (stim_on2%10));
////
////    params.pwm_duty = pwm_duty;
////    if(pwm_mode_en){
////        params.pwm_sync_mode = 1;
////        logInfo("PWM_MODE");
////    } else {
////        params.pwm_sync_mode = 0;
////    }
////
////    if(soft_cont > 0) {
////        /* 配置软启动 */
////        lmp_softstart_params_t soft;
////        soft.enable = 1;
////        soft.inc_value = 1;     // 每次增加的DAC值
////        soft.step = soft_cont;  // 刺激次数
////        lmp_rf_config_softstart(LMP_PROGRAM_0, &soft);
////    }
////
////    /* 配置参数 */
////    params.pulse_mode = LMP_SIMPLE_MODE;
////    params.program_cnt = 1;           // 双程序模式
////    params.low_power_mode = 0;
////    params.base_freq = freq;
////
////    /* 源选择限制 */
////    if(source > 2) {
////        source = 2;
////    }
////
////    /* 程序0配置 (第一个刺激程序) */
////    params.program[source][0].tphase1 = width1;      /* 正脉宽 */
////    params.program[source][0].tphase2 = width1;      /* 负脉宽 */
////    params.program[source][0].tphase3 = 20;          /* 电荷平衡时间 */
////    params.program[source][0].tinterphase1 = 20;     /* 10us */
////    params.program[source][0].tinterphase2 = 10;     /* 10us */
////    params.program[source][0].amplitude_p = ua1;     /* 刺激电流ua */
////    params.program[source][0].amplitude_n = ua1;     /* 刺激电流ua */
////    params.program[source][0].polar.dir = pin_dir1;  /* 电极方向 */
////    params.program[source][0].polar.on = pin_on1;
////    params.program[source][0].multiple = multiple_0;  /* 频率倍数 */
////
////    /* 程序1配置 (第二个刺激程序) */
////    params.program[source][1].tphase1 = width2;      /* 正脉宽 */
////    params.program[source][1].tphase2 = width2;      /* 负脉宽 */
////    params.program[source][1].tphase3 = 20;          /* 电荷平衡时间 */
////    params.program[source][1].tinterphase1 = a;     /* 10us */
////    params.program[source][1].tinterphase2 = b;     /* 10us */
////    params.program[source][1].amplitude_p = ua2;     /* 刺激电流ua */
////    params.program[source][1].amplitude_n = ua2;     /* 刺激电流ua */
////    params.program[source][1].polar.dir = pin_dir2;  /* 电极方向 */
////    params.program[source][1].polar.on = pin_on2;
////    params.program[source][1].multiple = multiple_1;  /* 频率倍数 */
////
////    /* 打印参数 */
////    log("LMP Dual Program Stimulation Parameters:");
////    log("  PWM Duty: %d%%", pwm_duty);
////    log("  Frequency: %d Hz", freq);
////    log("  Power Level: %d", power);
////    log("--- Program 0 ---");
////    log("  Pulse Width: %d us", width1);
////    log("  Current: %d uA", ua1);
////    log("  Electrode Direction: 0x%02x", pin_dir1);
////    log("  Electrode Enable: 0x%02x", pin_on1);
////    log("--- Program 1 ---");
////    log("  Pulse Width: %d us", width2);
////    log("  Current: %d uA", ua2);
////    log("  Electrode Direction: 0x%02x", pin_dir2);
////    log("  Electrode Enable: 0x%02x", pin_on2);
////
////    /* 配置并启动刺激 */
////    int err = lmp_rf_config_simple(&params, source);
////    if (err == 0) {
////        if (pwm_duty > 0) {
////            if(freq > 15) {
////                rf_pwm_chang(params.pwm_duty, freq);
////            } else {
////                rf_pwm_chang(params.pwm_duty, freq);
////            }
////            log("PWM enabled: duty=%d%%, freq=%dHz", pwm_duty, freq);
////        }
////        log("Dual program stimulation started - OK!");
////    } else {
////        log("Stimulation configuration FAILED!");
////    }
//}
//
///* 导出命令 */
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPDUAL_1, test_lmp_dual_program,
//                 "LMP: Dual stim [pwm%%] [w1(us)] [w2(us)] [freq] [ua1] [ua2] [elec1_xy] [elec2_xy] [power]");
//
///**
// * @brief 双电流源刺激测试
// * @param stim_a 电流源A电极对(格式:XY)
// * @param stim_b 电流源B电极对(格式:XY)
// * @param freq 频率(Hz)
// * @param ua_a 电流源A电流(uA)
// * @param ua_b 电流源B电流(uA)
// */
//static void test_lmp_dual_simple(uint8_t stim_a, uint8_t stim_b, uint16_t freq_a, uint16_t freq_b,
//                                uint16_t ua_a, uint16_t ua_b)
//{
//    int err;
//
//    rf_reset_chip();
//
//    lmp_simple_params_t params = {0};
//
//    if (lmp.efuse_id == 0) {
//        lmp_rf_send_query();
//    }
//
//    /* 解析电极对 */
//    uint8_t anode_a = stim_a / 10;
//    uint8_t cathode_a = stim_a % 10;
//    uint8_t anode_b = stim_b / 10;
//    uint8_t cathode_b = stim_b % 10;
//
//    uint8_t pin_dir_a ;
//    uint8_t pin_on_a ;
//    uint8_t pin_dir_b;
//    uint8_t pin_on_b ;
//    pin_dir_a = (1 << (stim_a /10)) | (0 << (stim_a%10));
//    pin_on_a = (1 << (stim_a/10)) | (1 << (stim_a%10));
//
//    pin_dir_b = (1 << (stim_b /10)) | (0 << (stim_b%10));
//    pin_on_b = (1 << (stim_b/10)) | (1 << (stim_b%10));
//
//    log("===== LMP Dual Current Source Test =====");
//    log("Chip EFUSE ID: 0x%02x", lmp.efuse_id);
//    log("Frequency: %d aHz", freq_a);
//    log("Frequency: %d bHz", freq_b);
//
//    /* 配置参数 */
//    params.pulse_mode = LMP_SIMPLE_MODE;
//    params.program_cnt = 0;           // 双程序(使用A和B电流源)
//    params.low_power_mode = 0;
//    params.base_freq = freq_a;
//
//    /* 程序0: A电流源 */
//    params.program[0][0].tphase1 = 200;     /* 200us */
//    params.program[0][0].tphase2 = 200;     /* 200us */
//    params.program[0][0].tphase3 = 10;
//    params.program[0][0].tinterphase1 = 10;
//    params.program[0][0].tinterphase2 = 10;
//    params.program[0][0].multiple = 1;
//    params.program[0][0].amplitude_p = ua_a;
//    params.program[0][0].amplitude_n = ua_a;
//    params.program[0][0].polar.dir = pin_dir_a;
//    params.program[0][0].polar.on = pin_on_a;
//    /* 配置并启动刺激 */
//    err = lmp_rf_config_simple(&params);
//    if (err == 0) {
//        log("Dual current source stimulation started");
//    } else {
//        log("Dual stimulation configuration FAILED!");
//    }
//
//    log("Program 0 (Current Source A):");
//    log("  Electrode: A%d-C%d", anode_a, cathode_a);
//    log("  Current: %d uA", ua_a);
//    log("  Pulse Width: 200us");
//    params.base_freq = freq_b;
//    /* 程序1: B电流源 */
//    params.program[1][0].tphase1 = 200;     /* 300us */
//    params.program[1][0].tphase2 = 200;     /* 300us */
//    params.program[1][0].tphase3 = 10;
//    params.program[1][0].tinterphase1 = 10;
//    params.program[1][0].tinterphase2 = 10;
//    params.program[1][0].multiple = 1;
//    params.program[1][0].amplitude_p = ua_b;
//    params.program[1][0].amplitude_n = ua_b;
//    params.program[1][0].polar.dir = pin_dir_b;
//    params.program[1][0].polar.on = pin_on_b;
//    /* 配置并启动刺激 */
//     err = lmp_rf_config_simple(&params);
//    if (err == 0) {
//        log("Dual current source stimulation started");
//    } else {
//        log("Dual stimulation configuration FAILED!");
//    }
//    log("Program 1 (Current Source B):");
//    log("  Electrode: A%d-C%d", anode_b, cathode_b);
//    log("  Current: %d uA", ua_b);
//    log("  Pulse Width: 300us");
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPDUAL, test_lmp_dual_simple,
//                 "LMP: Dual stim [elec_a_xy] [elec_b_xy] [freq] [ua_a] [ua_b]");
//
///*==============================================================================
// * 突发模式刺激测试
// *============================================================================*/
//
///**
// * @brief 突发模式刺激
// * @param pwm_duty PWM占空比(%)
// * @param width 脉宽(us)
// * @param freq 频率(Hz)
// * @param ua 电流(uA)
// * @param stim_on 刺激电极对(格式:XY)
// * @param burst_cnt 每burst脉冲数(1-255)
// * @param power 刺激功率(0-15)
// */
//static void test_lmp_burst(uint8_t pwm_duty, uint8_t width, uint16_t freq,
//                          uint16_t ua, uint8_t stim_on, uint8_t burst_cnt, uint8_t power, uint8_t source, uint8_t interval)
//{
//    lmp_simple_params_t params = {0};
//
//    lead.stim_pulse_power = power;  // 设置刺激功率
//
//    if (lmp.efuse_id == 0) {
//        lmp_rf_send_query();
//    }
//
//    /* 解析电极对 */
//    uint8_t anode = stim_on / 10;
//    uint8_t cathode = stim_on % 10;
//    uint8_t pin_dir = (1 << cathode) | (0 << anode);
//    uint8_t pin_on = (1 << cathode) | (1 << anode);
//
//    log("===== LMP Burst Mode Test =====");
//    log("Electrode: A%d-C%d", anode, cathode);
//    log("Pulse Width: %dus", width);
//    log("Frequency: %dHz", freq);
//    log("Current: %duA", ua);
//    log("Burst Count: %d", burst_cnt);
//    log("Power Level: %d", power);
//
//    /* 配置参数 */
//    params.pulse_mode = LMP_BURST_MODE;
//    params.program_cnt = 0;
//    params.low_power_mode = 0;
//    params.base_freq = freq;
//    params.burst_pulse_cnt = burst_cnt;
//    params.burst_tinter = 1000;  /* 突发间隔1ms */
//
//    if(source > 2) {
//        source = 2;
//    }
//
//    /* 程序配置 */
//    params.program[0][0].tphase1 = width;
//    params.program[0][0].tphase2 = width;
//    params.program[0][0].tphase3 = 10;
//    params.program[0][0].tinterphase1 = 10;
//    params.program[0][0].tinterphase2 = 10;
//    params.program[0][0].amplitude_p = ua;
//    params.program[0][0].amplitude_n = ua;
//    params.program[0][0].polar.dir = pin_dir;
//    params.program[0][0].polar.on = pin_on;
//    params.program[0][0].multiple = 1;
//
//    params.program[0][0].tinterpulse = interval;
//    /* 配置并启动刺激 */
//    int err = lmp_rf_config_burst(&params);
//    if (err == 0) {
//        if (pwm_duty > 0) {
//            rf_pwm_chang(pwm_duty, freq);
//        }
//        log("Burst stimulation started");
//    } else {
//        log("Burst stimulation configuration FAILED!");
//    }
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPBURST, test_lmp_burst,
//                 "LMP: Burst stim [pwm%%] [w(us)] [freq] [ua] [elec_xy] [cnt] [power]");
//
//
//
///*==============================================================================
// * 寄存器读写测试
// *============================================================================*/
//
///**
// * @brief 寄存器写测试
// * @param reg 寄存器地址
// * @param data 要写入的数据
// */
//
//static int test_lmp_write_reg(int argc, char *argv[])
//{
//    uint16_t uart_data[256];
//    uint8_t uart_data_len;
//    uart_data_len = argc-1;
//    for (char i = 0; i < uart_data_len; i++) {
//         sscanf((char*)argv[i+1], "%hx", &uart_data[i]);
////         logInfo( "get 0x%x", uart_data[i]);
// //        uart_data[i] = strtol((char*)argv[i+1], NULL, 0);
//     }
//
//    uint8_t reg;
//    uint16_t reg_data;
//    reg = uart_data[0];
//    reg_data = uart_data[1];
//    lmp_cmd_write(reg, reg_data);
//    log("Write reg 0x%02x -> 0x%02x", reg, reg_data);
//    return 0;
//
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
//                 LMPWR, test_lmp_write_reg, "LMP: Write register [reg] [data]");
//
///**
// * @brief 寄存器读测试
// * @param reg 寄存器地址
// */
//static void test_lmp_read_reg(uint8_t reg)
//{
//    uint16_t get_data = 0;
//    int err = lmp_cmd_read(reg, &get_data);
//    if (err == 0) {
//        log("Read reg 0x%02x <- 0x%04x", reg, get_data);
//    } else {
//        log("Read reg 0x%02x failed", reg);
//    }
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 LMPRD, test_lmp_read_reg, "LMP: Read register [reg]");
//
//
//#endif /* ENABLE_TEST_LMP */
