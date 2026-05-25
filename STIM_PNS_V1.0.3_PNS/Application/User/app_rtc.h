/**
  ******************************************************************************
  * @file    app_rtc.h
  * @author  Taoking
  * @brief   实现了RTC芯片的应用接口
  *          This is the common part of the file
  * @version V0.0
  * @since   2025-04-10
  * @verbatim
  * Detailed_description_of_the_document
  * @endverbatim
  ******************************************************************************
  * @note CopyRight By XXXX Co. Ltd All right reserve
  * @attention
  *
  ******************************************************************************
  */

#ifndef __APP_RTC_H_
#define __APP_RTC_H_

#include "sd8563.h"
#include "main_user.h"

typedef struct{
    unsigned char year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
    unsigned char week;
}rtc_time_t;

#define  IIC_WRITE_MAX_BYTE        32  //length:reg+data 

int app_rtc_iic_init(void);
void app_rtc_check_status_when_task_start(void);
int8_t app_rtc_init(void);
int app_rtc_set_time(rtc_time_t time);
int app_rtc_get_time(rtc_time_t *time);
void app_rtc_int_log(void);
int shell_write_rtc(int argc, char **argv);
int shell_read_rtc(void);
//记录开始时间
int app_rtc_start_timer_now(void);
int app_rtc_stop_timer_and_get_diff(void);
void app_rtc_get_timestamp(void);

#endif //__APP_RTC_H_
