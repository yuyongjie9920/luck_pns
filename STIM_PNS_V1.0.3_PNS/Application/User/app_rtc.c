#include <app_log.h>                  ///< 必须在 LOG_TAG 与 LOG_LVL 下面
#include <app_verify_ble.h>

#include "string.h"
#include "app_rtc.h"
#include <time.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/GPIO.h>
#include "ti_drivers_config.h"
#include <time.h>
#include <ti/sysbios/hal/Seconds.h>
#include "app_i2c.h"


#define BLE_DEBUG         1
#if BLE_DEBUG
#define rtc_log(fmt, ...) logInfo(fmt, ##__VA_ARGS__)
#else
#define rtc_log(fmt) ((void)0)
#endif

#define TEST_RTC   0


//----------------hw------------
static void app_rtc_gpio_handler(uint_least8_t index);

static int app_rtc_gpio_int_init(void)
{
    GPIO_setCallback(CONFIG_GPIO_RTC_INT_CONST, app_rtc_gpio_handler);
    GPIO_enableInt(CONFIG_GPIO_RTC_INT_CONST);
    return 0;
}

static uint8_t s_rtc_int = 0;
static void app_rtc_gpio_handler(uint_least8_t index)
{
    s_rtc_int = 1;
}

void app_rtc_check_status_when_task_start(void)
{
    sd8563_osf_flag_e osf_flag = 0;

    if (sd8563_get_osf_flag(&osf_flag) == SD8563_EXIT_OK)
    {
        if (osf_flag == OSF_H)
        {
            rtc_log("power on or stop vibration for the first time.");
            sd8563_clear_osf_flag();
        }
    }
}

static int8_t app_set_cc26xx_rtc_date_time(void)
{
    sd8563_time_t sd8563_time;
    sd8563_osf_flag_e osf_flag;

    if(sd8563_get_osf_flag(&osf_flag) == SD8563_EXIT_OK)
    {
        // 发生过掉电或停振，不能用RTC芯片时间设置cc2652时间
        if (osf_flag == OSF_H)
        {
            rtc_log("sd8563'rtc not set");

            return 0;
        }
        else // 获取当前的rtc时间，并将其设置到cc2652中
        {
            if(sd8563_read_time(&sd8563_time) == SD8563_EXIT_OK)
            {

                struct tm sltm;
                memset(&sltm, 0, sizeof(struct tm));

                // Set date/time
                sltm.tm_year = 2000 + sd8563_time.year - 1900;
                sltm.tm_mon = sd8563_time.month - 1;
                sltm.tm_mday = sd8563_time.day;
                sltm.tm_hour = sd8563_time.hour;
                sltm.tm_min = sd8563_time.minute;
                sltm.tm_sec = sd8563_time.second;

                logInfo("rtc time: %d-%d-%d,%d:%d:%d",
                                sltm.tm_year, sltm.tm_mon, sltm.tm_mday,
                                sltm.tm_hour, sltm.tm_min, sltm.tm_sec);

                // Convert to number of seconds, this will also fill up tm_wday and tm_yday
                // 用来将参数 tm 所指的tm结构数据转换成从公元1970年1月1日0时0分0秒算起至今的UTC时间所经过的秒数
                time_t seconds = mktime(&sltm);

                // Set the date into the system
                Seconds_set(seconds);
            }
            else
            {
                rtc_log("sd8563 read time");

                return 1;
            }
        }
    }
    else
    {
        rtc_log("sd8563 read osf");
        return 1;
    }
    return 0;
}


int8_t app_rtc_init(void)
{
    app_i2c_init();

    iic_init_s iic_init;
    iic_init.p_read = app_i2c_read;
    iic_init.p_write = app_i2c_write;
    sd8563_init(iic_init);

    app_rtc_gpio_int_init();

    app_rtc_get_timestamp();
//    app_set_cc26xx_rtc_date_time(); //没调 会死机
    return 0;
}

void app_rtc_int_log(void)
{
    if (s_rtc_int)
    {
        rtc_log("rtc gpio interrupt.");
        s_rtc_int = 0;
    }
}

int app_rtc_set_time(rtc_time_t time)
{
    sd8563_time_t time1;
    time1.year    =  time.year;
    time1.month =  time.month;
    time1.day     =  time.day;
    time1.hour    =  time.hour;
    time1.minute =  time.minute;
    time1.second =  time.second;
    int err = sd8563_write_time(&time1);

    app_rtc_get_timestamp();
    return err;
}

int app_rtc_get_time(rtc_time_t *time)
{
    sd8563_time_t time1;
    sd8563_read_time(&time1);

    time->year    =  time1.year;
    time->month   =  time1.month;
    time->day     =  time1.day;
    time->hour    =  time1.hour;
    time->minute  =  time1.minute;
    time->second  =  time1.second;

    rtc_log("rtc time: %d-%d-%d,%d:%d:%d",
            time->year, time->month, time->day,
            time->hour, time->minute, time->second);

    return 0;
}


// 在文件开头添加以下定义
static rtc_time_t s_start_time = {0};
static rtc_time_t s_end_time = {0};
static bool s_timer_started = false;

/**
 * @brief 将RTC时间转换为时间戳（秒）
 * @param time - RTC时间结构体
 * @return 时间戳（秒）
 */
static uint32_t rtc_time_to_timestamp(const rtc_time_t *time)
{
    struct tm time_tm;
    memset(&time_tm, 0, sizeof(struct tm));

    // RTC年份是2位数，需要转换为4位数（假设是2000年以后）
    time_tm.tm_year = 2000 + time->year - 1900; // tm_year是从1900开始的年数
    time_tm.tm_mon = time->month - 1;           // tm_mon是0-11
    time_tm.tm_mday = time->day;
    time_tm.tm_hour = time->hour;
    time_tm.tm_min = time->minute;
    time_tm.tm_sec = time->second;
    // 使用mktime转换为时间戳，会自动处理闰年、闰月、跨月等情况
    return (uint32_t)mktime(&time_tm);
}


//获取RTC时间 设置到系统时间  用于flash log 获取时间
void app_rtc_get_timestamp(void)
{
    rtc_time_t rtc_time;
    struct tm timeinfo;
    logInfo("app_rtc_get_timestamp");
    // 读取RTC时间
    if (app_rtc_get_time(&rtc_time) != 0) {
        return;
    }

    // 填充tm结构体
    timeinfo.tm_year = rtc_time.year + 2000 - 1900;  // 年份从1900开始
    timeinfo.tm_mon  = rtc_time.month - 1;    // 月份0-11
    timeinfo.tm_mday = rtc_time.day;
    timeinfo.tm_hour = rtc_time.hour;
    timeinfo.tm_min  = rtc_time.minute;
    timeinfo.tm_sec  = rtc_time.second;

    time_t seconds = mktime(&timeinfo);

    Seconds_set(seconds);  //设置系统时间
}

/**
 * @brief  计算两个时间点之间的时间差（分钟）
 * @param start - 开始时间
 * @param end - 结束时间
 * @return 时间差（分钟），负数表示时间错误
 */
static int calculate_time_diff_minutes(const rtc_time_t *start, const rtc_time_t *end)
{
    uint32_t start_timestamp, end_timestamp;
    int32_t diff_seconds;

    // 将RTC时间转换为时间戳
    start_timestamp = rtc_time_to_timestamp(start);
    end_timestamp = rtc_time_to_timestamp(end);

    // 计算时间差（秒）
    if (end_timestamp >= start_timestamp) {
        diff_seconds = end_timestamp - start_timestamp;
    } else {
        // 结束时间早于开始时间，可能是跨年或者时间设置错误
        rtc_log("Warning: End time is earlier than start time!");
        diff_seconds = end_timestamp - start_timestamp; // 这里可能是负数
    }

    // 转换为分钟
    if (diff_seconds < 0) {
        rtc_log("Error: Negative time difference!");
        return -1;
    }
    return (int)(diff_seconds / 60);
}

/**
 * @brief 使用当前RTC时间记录开始时间
 * @return 0成功，其他失败
 */
int app_rtc_start_timer_now(void)
{

    // 获取当前RTC时间  // 保存开始时间
    if (app_rtc_get_time(&s_start_time) != 0) {
        rtc_log("Failed to get current RTC time!");
        return -1;
    }

    s_timer_started = true;

    rtc_log("Timer started: %02d-%02d-%02d %02d:%02d:%02d",
            s_start_time.year, s_start_time.month, s_start_time.day,
            s_start_time.hour, s_start_time.minute, s_start_time.second);

    return 0;
}

/**
 * @brief 停止计时并计算与开始时间的时间差（分钟）
 * @param end_time - 结束时间（可选，如果为NULL则使用当前RTC时间）
 * @return 时间差（分钟），-1表示错误
 */
int app_rtc_stop_timer_and_get_diff(void)
{
    if(s_timer_started == false) {
        return -1;
    }
    // 获取当前RTC时间  // 保存开始时间
    if (app_rtc_get_time(&s_end_time) != 0) {
       rtc_log("Failed to get current RTC time!");
       return -1;
    }
    rtc_log("Timer end_time: %02d-%02d-%02d %02d:%02d:%02d",
            s_end_time.year, s_end_time.month, s_end_time.day,
            s_end_time.hour, s_end_time.minute, s_end_time.second);
    // 计算时间差
    int minutes = calculate_time_diff_minutes(&s_start_time, &s_end_time);

    if (minutes < 0) {
        rtc_log("Error calculating time difference!");
        return -1;
    }

    // 重置计时状态
    s_timer_started = false;

    logInfo("go to minutes = %d", minutes);

    return minutes;
}

#if TEST_RTC  //test pase ok
int shell_write_rtc(int argc, char **argv)
{
    uint32_t get_data[8];
    sd8563_time_t time;
    if(argc < 6) {
        rtc_log("you can set  25 9 18 15 54 00 2");
        return 0;
    }

    logDebug("%d parameter(s)", argc);
    for (char i = 1; i < argc; i++)
    {
        sscanf((char*)argv[i], "%d", &get_data[i]);
        rtc_log("get_data[%d] = %d", i, get_data[i]);
    }
    time.year =  get_data[1] ;
    time.month =  get_data[2];
    time.day =  get_data[3];
    time.hour =  get_data[4];
    time.minute =  get_data[5];
    time.second =  get_data[6];
    time.week =  get_data[7];

    rtc_log("rtc write time: %d-%d-%d,%d:%d:%d week %d",
                time.year, time.month, time.day,
                time.hour, time.minute, time.second, time.week);
    sd8563_write_time(&time);
    return 0;
}

int shell_read_rtc(void)
{
    sd8563_time_t time;
    sd8563_read_time(&time);
    rtc_log("rtc time: %d-%d-%d,%d:%d:%d week %d",
            time.year, time.month, time.day,
            time.hour, time.minute, time.second, time.week);
    return 0;
}
#endif


