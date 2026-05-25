#include "shell_func_export.h"
//#include "app_stim.h"
#include "drivers/GPIO.h"
#include "log.h"
#include "main_user.h"
#include "shell.h"
#include "string.h"

#include "app_rtc.h"
//#include "gb29768.h"



//static int ble_cmd_test(int argc, char *argv[])
//{
//    uint8_t uart_data[256];
//    uint8_t uart_data_len;
//    uart_data_len = argc-1;
//    logDebug("%d parameter(s)", argc);
//    for (char i = 0; i < uart_data_len; i++) {
//        sscanf((char*)argv[i+1], "%x", &uart_data[i]);
////        uart_data[i] = strtol((char*)argv[i+1], NULL, 0);
//    }
//    put_buf(uart_data, uart_data_len);
//
//    uint8_t cmd;
//    uint16_t index;
//    uint8_t get_data[200];
//    uint8_t data_len;
//
//    cmd = uart_data[0];
//    if(uart_data_len>1){
//        memcpy(&index, &uart_data[1], sizeof(index));
//        memcpy(get_data, uart_data, uart_data_len-3);
//        data_len = uart_data_len-3;
//    } else {
//        index = 0;
//        data_len = 0;
//    }
//
//    process_command(cmd, index, get_data, data_len);
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), ble_cmd_test, ble_cmd_test, test_argx);



//static bool check_simple_start_flag = 0;
//
//static int shell_test_argx(int argc, char *argv[])
//{
//    logDebug("%d parameter(s)", argc);
//    for (char i = 1; i < argc; i++)
//    {
//        logDebug("%s", argv[i]);
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), shell_test_argx, shell_test_argx, "test");
//
//
//static int shell_test_param(int i, char ch, char *str)
//{
//    logDebug("input int: %d, char: %c, string: %s", i, ch, str);
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), shell_test_param, shell_test_param, test_param);

//
//#define SHELL_TEST_RTC
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
//#ifdef SHELL_X4_TEST
//extern int asic_read(uint8_t addr);
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
//
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
//
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD_03,asic_read_msh,"测试中文 CMD03 addr");
//#endif
//
//
////query测试  qurey_mode = 0 固定功率  1轮询
//#include "app_asic_client.h"
//
//static uint8_t rfid_power_mode = 0;
//static int shell_test_query(uint8_t qurey_mode)
//{
//    if(rfid_power_mode == 0) {
//        en_rfid_power();
//        delay_ms(1);
//    }
//
//    int err = asic_query(qurey_mode, &asic_par.uid[asic_par.active_idx]);
//
//    if(rfid_power_mode == 0) {
//        close_rfid_power();
//    }
//
//    if(err !=0) {
//        logInfo("test_query fail");
//        return -1;
//    }
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), CMD03, shell_test_query, "[u8]mode 测试query mode = 0当前功率 1为轮询功率");
//
//
//static void asic_read_vlt_msh(uint8_t argc, char **argv)
//{
//    extern uint32_t adc_word15_data;
//
//    uint8_t err_code = ASIC_OK;
//    uint32_t delay_sample_mode, delay_cnt;
//    float vlt;
//
//    if (argc == 3)
//    {
//        sscanf((char*)argv[1], "%d", &delay_sample_mode);
//        sscanf((char*)argv[2], "%d", &delay_cnt);
//
//        if(delay_sample_mode == FALSE)
//        {
//            err_code = get_asic_vlt(&vlt, FALSE); //立即采样
//             logInfo("asic vrecp is : %f V", vlt);
//        }
//        else
//        {
//            delay_cnt = (delay_cnt > MAX_VLT_SAMPLE_DELAY) ? MAX_VLT_SAMPLE_DELAY : delay_cnt;
//            err_code = get_asic_vlt(&vlt, TRUE); //延时采样
//            ADC_VLT_I_DELAY_CNT(delay_cnt); //设置延时采样的时间
//            err_code |= start_vlt_i_delay_sample();
//             logInfo("asic vlt is : %f V \r\n", vlt);
//        }
//
//        logInfo("err_code is : %x \r\n", err_code);
//
//    }
//    else
//    {
//        logInfo("Please input: check_asic_vlt <delay_sam_mode>[0/1] <delay_cnt> \n");
//        return;
//    }
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), CMD04, asic_read_vlt_msh, "获取vrecp电压");
//
//
//
//
//
//
//
