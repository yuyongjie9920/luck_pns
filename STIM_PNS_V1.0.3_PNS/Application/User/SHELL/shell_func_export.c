//#include "shell_func_export.h"
//#include "app_stim.h"
//#include "drivers/GPIO.h"
//#include "log.h"
//#include "main_user.h"
//#include "shell.h"
//#include "string.h"
//
//#include "st25r3916.h"
//#include "st25r3916_com.h"
//#include "st25r3916_irq.h"
//
//#include "rfal_rf.h"
//#include "rfal_chip.h"
//#include "rfal_analogConfig.h"
//#include "app_asic.h"
//#include "st25r3916.h"
//
//#include "app_rtc.h"
//#include "gb29768.h"
//#include "string.h"
//
////static bool check_simple_start_flag = 0;
////
////static int shell_test_argx(int argc, char *argv[])
////{
////    logDebug("%d parameter(s)", argc);
////    for (char i = 1; i < argc; i++)
////    {
////        logDebug("%s", argv[i]);
////    }
////    return 0;
////}
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), shell_test_argx, shell_test_argx, test_argx);
////
////
////static int shell_test_param(int i, char ch, char *str)
////{
////    logDebug("input int: %d, char: %c, string: %s", i, ch, str);
////    return 0;
////}
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), shell_test_param, shell_test_param, test_param);
////
////static int shell_spi_transparent_send(uint8_t en, uint8_t issend)
////{
////    app_stim_st25r3916_transparent_mode(en);
////    if (en && !issend) {
////        APP_IO_WRITECONFIG_GPIO_SPI_STIM_PICO_CONST, 0);
////    }
////    return 0;
////}
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), shell_spi_transparent_send, shell_spi_transparent_send, [u8 en][u8 issend]);
//
////#define SHELL_TEST_RTC
//#define SHELL_X4_TEST
//
//#ifdef SHELL_TEST_RTC //RTC测试
//int shell_write_rtc(int argc, char **argv)
//{
//    uint32_t get_data[8];
//    sd8563_time_t time;
//    if(argc < 6) {
//        logInfo("you can set  25 9 18 15 54 00 2");
//        return 0;
//    }
//
//    logDebug("%d parameter(s)", argc);
//    for (char i = 1; i < argc; i++)
//    {
//        sscanf((char*)argv[i], "%d", &get_data[i]);
//        logInfo("get_data[%d] = %d", i, get_data[i]);
//    }
//    time.year =  get_data[1] ;
//    time.month =  get_data[2];
//    time.day =  get_data[3];
//    time.hour =  get_data[4];
//    time.minute =  get_data[5];
//    time.second =  get_data[6];
//    time.week =  get_data[7];
//
//    logInfo("rtc write time: %d-%d-%d,%d:%d:%d week %d",
//                time.year, time.month, time.day,
//                time.hour, time.minute, time.second, time.week);
//    sd8563_write_time(&time);
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), CMD_01, shell_write_rtc, "写入RCT CMD_01 25 9 18 15 54 00 2");
//
//int shell_read_rtc(void)
//{
//    sd8563_time_t time;
//    sd8563_read_time(&time);
//    logInfo("rtc time: %d-%d-%d,%d:%d:%d week %d",
//            time.year, time.month, time.day,
//            time.hour, time.minute, time.second, time.week);
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD_02, shell_read_rtc, "获取RTC时间");
//#endif
//
//
//#ifdef SHELL_X4_TEST
//static void asic_read_msh(uint8_t addr)
//{
//    uint8_t read_addr;
//    int err = -1;
//    read_addr = addr;
//    for(uint8_t i=0; i<10 ;i++){
//        err = asic_query(1, &asic_par.uid[asic_par.active_idx]);
//        if(err == GB29768_OKAY) {
//            logInfo("asic_read_addr = 0x%x",addr);
//            logInfo("asic_read_err = %d",err);
//            err = asic_read(read_addr);
////            logInfo("asic_read_err = %d",err);
//        } else {
//            break;
//        }
//    }
//
//    //恢复占空比
//    //pa_pwm(fre_asic_temp, duty_asic_temp);
//    //pa_pwm(100, duty_asic_temp);
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD_03,asic_read_msh,"获取 CMD03 addr");
//#endif
//
//
//
//
//
//
////// PWM控制器结构体
////typedef struct {
////    Timer_Handle timerHandle;
////    uint8_t outputPin;
////    bool isRunning;
////    bool currentState;
////    uint32_t periodUs;    // 周期（微秒）
////    uint32_t highUs;      // 高电平时间（微秒）
////} PWM_Controller;
////extern PWM_Controller pwmCtrl;
////void PWM_Stop();
////
////
/////**
//// * @brief ST25R3916寄存器操作函数
//// * @param space 寄存器空间：0-主空间(A)，1-空间B
//// * @param reg 寄存器地址(0x00-0x7F)
//// * @param value 要写入的值(0x00-0xFF)，-1表示只读
//// * @return 操作结果：0成功，-1失败
//// */
////int shell_st25r3916_reg_ex(uint8_t space, uint8_t reg, int value)
////{
////    // 参数有效性检查
////    if(space > 1) {
////        logError("Error: Invalid register space (0-主空间,1-空间B)");
////        return -1;
////    }
////
////    if(reg > 0x7F) {
////        logError("Error: Register address out of range (0x00-0x7F)");
////        return -1;
////    }
////
////    app_stim_spi_gpio_mode_set(1);
////
////    uint8_t reg_value = 0;
////    int ret = 0;
////    uint8_t original_space = 0;
////
////    // 保存当前寄存器空间(只读0x7F寄存器)
////    st25r3916ReadRegister(0x7F, &original_space);
////
////    // 设置目标寄存器空间(如果需要切换)
////    if(space == 1) { // 空间B
////        st25r3916WriteRegister(0x7F, 0x03);
////    } else { // 主空间(A)
////        st25r3916WriteRegister(0x7F, 0x00);
////    }
////
////    if(value == -1) { // 读操作
////        ret = st25r3916ReadRegister(reg, &reg_value);
////        logDebug("┌────────────────────── ");
////        logDebug("│ %s REG[0x%02X] = 0x%02X    │",
////               (space == 1) ? "B" : "A", reg, reg_value);
////        logDebug("└────────────────────── \r\n");
////    }
////    else { // 写操作
////        if(value < 0 || value > 0xFF) {
////            logError("Error: Value out of range (0x00-0xFF)");
////            // 恢复原始空间设置
////            st25r3916WriteRegister(0x7F, original_space);
////            app_stim_spi_gpio_mode_set(0);
////            return -1;
////        }
////
////        reg_value = (uint8_t)value;
////        ret = st25r3916WriteRegister(reg, reg_value);
////
////        // 验证写入结果
////        uint8_t verify = 0;
////        st25r3916ReadRegister(reg, &verify);
////
////        logDebug("┌───────────────────────");
////        logDebug("│ Write %s REG[0x%02X] = 0x%02X -> 0x%02X %s │",
////               (space == 1) ? "B" : "A", reg, reg_value, verify,
////               (reg_value == verify) ? "✓" : "✗");
////        logDebug("└─────────────────────── \r\n");
////    }
////
////    // 恢复原始寄存器空间
////    st25r3916WriteRegister(0x7F, original_space);
////
////    app_stim_spi_gpio_mode_set(0);
////    app_stim_st25r3916_transparent_mode(1);
////    APP_IO_WRITE(CONFIG_GPIO_SPI_STIM_PICO_CONST, 0);
////    return ret;
////}
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),wreg,shell_st25r3916_reg_ex,[int reg][int value]);
////
////// 完整的ST25R3916寄存器打印命令（修复排版问题）
////static int shell_st25r3916_dump(void)
////{
////    app_stim_spi_gpio_mode_set(1);
////
////    // 打印A组寄存器
////    mylog("\r\n┌───────────────────────────────────────────────────────┐\r\n");
////    mylog("│                ST25R3916 REGISTER DUMP                │\r\n");
////    mylog("├───────┬───────┬───────┬───────┬───────┬───────┬───────┤\r\n");
////
////    // 打印主寄存器空间 (0x00-0x3F)
////    uint8_t printed_count = 0;
////    for (uint8_t i = 0; i <= 0x3F; i++) {
////        if(i != 0x2f && i!= 0x30){
////            uint8_t val = 0;
////            st25r3916ReadRegister(i, &val);
////
////            if (printed_count % 7 == 0) {
////                mylog("│");
////            }
////
////            mylog(" %02X:%02X │", i, val);
////            printed_count++;
////
////            if (printed_count % 7 == 0) {
////                mylog("\r\n");
////                if (i != 0x3F) { // 不是最后一个寄存器时打印分割线
////                    mylog("├───────┼───────┼───────┼───────┼───────┼───────┼───────┤\r\n");
////                }
////            }
////        }
////    }
////
////    // 补充最后一行的边框（如果最后一行不满7个）
////    if (printed_count % 7 != 0) {
////        for (uint8_t i = printed_count % 7; i < 7; i++) {
////            mylog("       │");
////        }
////        mylog("\r\n");
////    }
////    mylog("└───────┴───────┴───────┴───────┴───────┴───────┴───────┘\r\n");
////
////    // 打印B组寄存器
////    mylog("\r\n┌───────────────────────────────────────────────────────┐\r\n");
////    mylog("│                    SPACE B REGISTERS                  │\r\n");
////    mylog("├───────┬───────┬───────┬───────┬───────┬───────┬───────┤\r\n");
////
////    // 切换到Space B
////    st25r3916WriteRegister(0x7F, 0x03);
////
////    // 定义需要打印的B组寄存器列表
////    uint8_t b_registers[] = {
////        0x05, 0x06,                   // 协议配置
////        0x0B, 0x0C, 0x0D,             // 接收器配置
////        0x0F, 0x15,                   // 定时器定义
////        0x28, 0x29,                   // 天线驱动器和调制
////        0x2A, 0x2B, 0x2C,             // 外场检测器阈值和稳压器
////        0x30, 0x31, 0x32, 0x33,      // 主动波束成形
////        0x2E, 0x2F,                   // AWS配置
////        0x34, 0x35, 0x36, 0x37, 0x38, 0x39  // AWS时间
////    };
////    uint8_t b_reg_count = sizeof(b_registers);
////
////    // 打印这些寄存器
////    for (uint8_t i = 0; i < b_reg_count; i++) {
////        if (i % 7 == 0) {
////            if (i != 0) { // 不是第一行时打印分割线
////                mylog("├───────┼───────┼───────┼───────┼───────┼───────┼───────┤\r\n");
////            }
////            mylog("│");
////        }
////
////        uint8_t val = 0;
////        st25r3916ReadRegister(b_registers[i], &val);
////        mylog(" %02X:%02X │", b_registers[i], val);
////
////        if (i % 7 == 6 || i == b_reg_count - 1) {
////            mylog("\r\n");
////        }
////    }
////
////    // 补充最后一行的边框（如果最后一行不满7个）
////    if (b_reg_count % 7 != 0) {
////        for (uint8_t i = b_reg_count % 7; i < 7; i++) {
////            mylog("       │");
////        }
////        mylog("\r\n");
////    }
////    mylog("└───────┴───────┴───────┴───────┴───────┴───────┴───────┘\r\n");
////
////    // 切换回Space A
////    st25r3916WriteRegister(0x7F, 0x00);
////
////    app_stim_spi_gpio_mode_set(0);
////    app_stim_st25r3916_transparent_mode(1);
////    APP_IO_WRITE(CONFIG_GPIO_SPI_STIM_PICO_CONST, 0);
////    return 0;
////}
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),regall,shell_st25r3916_dump,[int reg][int value]);
////
////int choise_fn_flag = 0;
//#include "gb29768.h"
//
//void test_asic_query(uint8_t mode)
//{
//    if(mode == 0){
//        asic_query(0, &asic_par.uid[asic_par.active_idx]);
//    } else {
//        asic_query(1, &asic_par.uid[asic_par.active_idx]);
//    }
//
//    logInfo("get_uid = 0x%x", asic_par.uid[asic_par.active_idx]);
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),asic_query,test_asic_query,[uid]);
//
//static uint8_t stim_on;
//static uint16_t  ma, hz, width;
//
//static int test_out_pluse(int argc, char *argv[])  //扯淡 这个在shell里面出不来波形在线程中可以没有排查
//{
//
//    logDebug("%d parameter(s)", argc);
//
//    for (char i = 1; i < argc; i++)
//    {
//        //logDebug("%s", argv[i]);
//        logDebug("%d", atoi(argv[i]));
//    }
//    stim_on = atoi(argv[1]);
//    ma = atoi(argv[2]);
//    hz =  atoi(argv[3]);
//    width =  atoi(argv[4]);
//    logInfo("stim_on = 0x%x", stim_on);
//    logInfo("ma = %d", ma);
//    logInfo("width = %d", width);
//    logInfo("hz = %d", hz);
//    int check_burst_msh_test(uint16_t stim_on, uint32_t pulse, uint32_t fre, uint32_t amp);
//    check_burst_msh_test(stim_on, width, hz, ma);
//    rest_x4_and_power(0);
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), test_out_pluse, test_out_pluse, test_argx);
//
////extern uint8_t check_demo_mode(uint8_t wave_mode);
////int x4_simple_mode = 0;
////void test_simple(uint8_t test_mode)
////{
////    choise_fn_flag = 1;
////    x4_simple_mode = test_mode;
////}
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),test_simple,test_simple,[test_mode]);
////
////
////uint32_t save_pulse, save_fre, save_amp;
////int test_simple1(uint32_t pulse, uint32_t fre, uint32_t amp)
////{
////    save_pulse = pulse;
////    save_fre = fre;
////    save_amp = amp;
////
////     uint32_t get_uid;
////     rest_x4_and_power(1);
////     extern int asic_test_simple(uint32_t pulse, uint32_t fre, uint32_t amp);
////     int err = asic_test_simple(pulse, fre,  amp);
////     if(err == 0){
////         check_simple_start_flag = 1;
////     }
////     rest_x4_and_power(0);
////     return err;
//////     check_demo_mode(x4_simple_mode);
////}
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),test_simple1,test_simple1,[test_mode]);
////
////void stop_simple(void)
////{
////    check_simple_start_flag = 0;
////    pa_pwm(1000, 0);
////}
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),stop_simple,stop_simple,[test]);
////
////void chang_sync_flag(uint8_t sync_flag);
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),chang_sync_flag,chang_sync_flag,[test_mode]);
////
////gb29768_error_t asic_write(uint8_t addr, uint8_t data_h, uint8_t data_l);
////void shell_write_x4(uint8_t addr, uint16_t data)
////{
////    uint8_t write_time = 10;
////    int err = -1;
////    uint8_t dataL, dataH;
////    dataL = data;
////    dataH = data>>8;
////    for(uint8_t i=0; i<10 ;i++){
////        if(err != 0) {
////           err = asic_write(addr, dataH, dataL);
////        } else {
////            break;
////        }
////    }
////
////}
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), asic_write_msh ,shell_write_x4,[test_mode]);
////
////
////void st3916_MeasurePhase(void)
////{
////    app_stim_spi_gpio_mode_set(1);
////    uint8_t result;
////    logInfo("st3916_MeasurePhase");
////    st25r3916MeasurePhase(&result);
////    logInfo("result = %d", result);
////    app_stim_spi_gpio_mode_set(0);
////}
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), st3916_MeasurePhase ,st3916_MeasurePhase,[sMeasurePhase]);
////
////
////void st3916_rssi(void)
////{
////    app_stim_spi_gpio_mode_set(1);
////    uint8_t result;
////    logInfo("rssi");
////    st25r3916_get_rssi(&result);
////    logInfo("rssi = %d", result);
////    app_stim_spi_gpio_mode_set(0);
////}
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), st3916_rssi ,st3916_rssi,[rssi]);
////
////void st3916_MeasureAmplitude(void)
////{
////    app_stim_spi_gpio_mode_set(1);
////    uint8_t result;
////    logInfo("MeasureAmplitude");
////    st25r3916MeasureAmplitude(&result);
////    logInfo("MeasureAmplitude = %d", result);
////    app_stim_spi_gpio_mode_set(0);
////}
////SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), st3916_MeasureAmplitude ,st3916_MeasureAmplitude,[MeasureAmplitude]);
////
////
////
////


//
//
