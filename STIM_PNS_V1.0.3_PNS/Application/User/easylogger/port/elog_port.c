/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */
 
#include <elog.h>
#include <elog_flash.h>
#include <string.h>
#include <stdio.h>

#include <time.h>
#include <ti/sysbios/hal/Seconds.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include "ti/sysbios/knl/Semaphore.h"
#include "app_log.h"

void elogTask_Entry(UArg a0, UArg a1);

#ifdef ELOG_ASYNC_OUTPUT_USING_TIRTOS
static Semaphore_Handle elogSem;
// Task configuration
#define ELOG_TASK_PRIORITY           1

#ifndef ELOG_TASK_STACK_SIZE
#define ELOG_TASK_STACK_SIZE         4096
#endif

// Task configuration
static uint8_t elogTaskStack[ELOG_TASK_STACK_SIZE];
static Task_Struct elogTask_Struct;
//static Task_Handle elogTask_Handle;

#endif

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    /* add your code here */
#ifdef ELOG_ASYNC_OUTPUT_ENABLE
#ifdef ELOG_ASYNC_OUTPUT_USING_TIRTOS
	Semaphore_Params sem_params;
	Semaphore_Params_init(&sem_params);
	sem_params.mode = Semaphore_Mode_BINARY;
	elogSem = Semaphore_create(0, &sem_params, NULL);

	Task_Params taskParams;
	// Configure task
	 Task_Params_init(&taskParams);
	 // create shell task
	 taskParams.stack = elogTaskStack;
	 taskParams.stackSize = ELOG_TASK_STACK_SIZE;
	 taskParams.priority = ELOG_TASK_PRIORITY;
	 taskParams.arg0 = (UArg)NULL;

	 Task_construct(&elogTask_Struct, elogTask_Entry, &taskParams, NULL);
#endif
#endif
    return result;
}

/**
 * EasyLogger port deinitialize
 *
 */
void elog_port_deinit(void) {

    /* add your code here */

	return;
}

#ifdef ELOG_COLOR_ENABLE
#define CSI_START                      "\033["
#define CSI_END                        "\033[0m"
/* output log front color */
#define F_BLACK                        "30;"
#define F_RED                          "31;"
#define F_GREEN                        "32;"
#define F_YELLOW                       "33;"
#define F_BLUE                         "34;"
#define F_MAGENTA                      "35;"
#define F_CYAN                         "36;"
#define F_WHITE                        "37;"
/* output log background color */
#define B_NULL
#define B_BLACK                        "40;"
#define B_RED                          "41;"
#define B_GREEN                        "42;"
#define B_YELLOW                       "43;"
#define B_BLUE                         "44;"
#define B_MAGENTA                      "45;"
#define B_CYAN                         "46;"
#define B_WHITE                        "47;"
/* output log fonts style */
#define S_BOLD                         "1m"
#define S_UNDERLINE                    "4m"
#define S_BLINK                        "5m"
#define S_NORMAL                       "22m"

/* color output info */
static const char *color_output_info[] = {
        [ELOG_LVL_ASSERT]  = ELOG_COLOR_ASSERT,
};
#endif

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) { 
//	userShellWrite((char *)log, size);
#ifdef ELOG_TERMINAL_ENABLE
	rt_kprintf("%.*s", size, log);
#endif

#ifdef ELOG_COLOR_ENABLE
	size_t color_hdr_len = 0, output_len = size;
	const char *output_log = log;

	color_hdr_len = (sizeof(CSI_START) - 1) + strlen(color_output_info[ELOG_LVL_ASSERT]);
	
	output_log += color_hdr_len;
	output_len -= (color_hdr_len + (sizeof(CSI_END) - 1) + (sizeof(ELOG_NEWLINE_SIGN) - 1));
#endif
	
	elog_flash_write(output_log, output_len);
	elog_flash_write(ELOG_NEWLINE_SIGN, (sizeof(ELOG_NEWLINE_SIGN) - 1));

    return;
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
    
    /* add your code here */

	return;
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    
    /* add your code here */

	return;
}

static char str[22] = {0, };

const char *elog_port_get_time(void) {
    static char str[32];  // 需要添加静态缓冲区
    time_t t1;
    struct tm *ltm;

    t1 = time(NULL);
    ltm = localtime(&t1);

    if (ltm == NULL) {
        logInfo("localtime() failed, using default time");
        return "0000-00-00 00:00:00 ";
    }

    // 格式化为: "YYYY-MM-DD HH:MM:SS "
    snprintf(str, sizeof(str), "%02d-%02d-%02d %02d:%02d:%02d ",
            (ltm->tm_year + 1900)%100,
            ltm->tm_mon + 1,
            ltm->tm_mday,
            ltm->tm_hour,
            ltm->tm_min,
            ltm->tm_sec);

//    // 打印生成的时间字符串
//    logInfo("Generated timestamp: %s", str);

    return str;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    
    /* add your code here */

	return "";
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    
    /* add your code here */

	return "";
}




#ifdef ELOG_ASYNC_OUTPUT_ENABLE
void elog_async_output_notice(void) {
#ifdef ELOG_ASYNC_OUTPUT_USING_TIRTOS
	Semaphore_post(elogSem);
#endif
}

void elogTask_Entry(UArg a0, UArg a1) {



    size_t get_log_size = 0;
#ifdef ELOG_ASYNC_LINE_OUTPUT
    static char poll_get_buf[ELOG_LINE_BUF_SIZE - 4];
#else
    static char poll_get_buf[ELOG_ASYNC_OUTPUT_BUF_SIZE - 4];
#endif

    for(;;)
    {
#ifdef ELOG_ASYNC_OUTPUT_USING_TIRTOS
        /* waiting log */
		Semaphore_pend(elogSem, BIOS_WAIT_FOREVER);
#endif
        /* polling gets and outputs the log */
        while (1) {
#ifdef ELOG_ASYNC_LINE_OUTPUT
            get_log_size = elog_async_get_line_log(poll_get_buf, sizeof(poll_get_buf));
#else
            get_log_size = elog_async_get_log(poll_get_buf, sizeof(poll_get_buf));
#endif
            if (get_log_size) {
                elog_port_output(poll_get_buf, get_log_size);
            } else {
                break;
            }
            delay_ms(10);
        }
    }
}

#endif

