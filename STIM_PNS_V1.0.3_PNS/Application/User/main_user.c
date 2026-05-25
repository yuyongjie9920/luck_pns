 #include <app_verify_ble.h>
#include <string.h>

#include <sys/cdefs.h>
#include <ti/sysbios/knl/Task.h>
#include <ti_drivers_config.h>

#include "main_user.h"
#include "app_log.h"
#include "log.h"
#include "cc2652_dog.h"
#include "simple_peripheral.h"
#include "app_log.h"
#include "app_flash_log.h"
#include "key_activity.h"
#include "CC2652_led.h"
#include "max17055_battery.h"

//log task
#define LOG_TASK_PRIORITY                     1
#define LOG_TASK_STACK_SIZE                   2048
static uint8_t spApp_Log_TaskStack[LOG_TASK_STACK_SIZE]  __attribute__((aligned(8)));


//BLE task
#define BLE_TASK_PRIORITY                     3
#define BLE_TASK_STACK_SIZE                   (2048 * 2)
static uint8_t ble_task_stack[BLE_TASK_STACK_SIZE]  __attribute__((aligned(8)));

//APP_BLE task
#define APP_BLE_TASK_PRIORITY                     4
#define APP_BLE_TASK_STACK_SIZE                   (2048 * 2)
static uint8_t app_ble_task_stack[APP_BLE_TASK_STACK_SIZE]  __attribute__((aligned(8)));


//app_user_task
#define USER_TASK_PRIORITY                     2
#define USER_TASK_STACK_SIZE                   (2048 * 2)
static uint8_t app_user_task_stack[USER_TASK_STACK_SIZE]  __attribute__((aligned(8)));


//app_dog_task
#define DOG_TASK_PRIORITY                     1
#define DOG_TASK_STACK_SIZE                   2048
static uint8_t app_dog_task_stack[DOG_TASK_STACK_SIZE]  __attribute__((aligned(8)));


//flash_log_task
#define FALSH_LOG_TASK_PRIORITY                     2
#define FALSH_LOG_TASK_STACK_SIZE                   2048
static uint8_t app_flash_log_task_stack[DOG_TASK_STACK_SIZE]  __attribute__((aligned(8)));

//RF_client_task
#define RF_CLIENT_TASK_PRIORITY                     1
#define RF_CLIENT_TASK_STACK_SIZE                   (1024* 2)
static uint8_t app_rf_client_task_stack[RF_CLIENT_TASK_STACK_SIZE]  __attribute__((aligned(8)));

//stim -spi
#define STIM_TASK_PRIORITY                     1
#define STIM_TASK_STACK_SIZE                   2048
static uint8_t spApp_Stim_TaskStack[STIM_TASK_STACK_SIZE] __attribute__((aligned(8)));


void test_task1(UArg arg0, UArg arg1);

extern void app_stim_task(UArg arg0, UArg arg1);

extern void app_shell_test_task(void *arg0, void *arg1);
//task param
/*static*/ struct app_task_param s_app_task_param[em_app_task_max] = {

    //log
    [em_app_task_log] = {
      .stack     = spApp_Log_TaskStack,
      .stackSize = LOG_TASK_STACK_SIZE,
      .priority  = LOG_TASK_PRIORITY,
      .taskCB    = app_log_task,
      .arg0      = 0,
      .arg1      = 0,
      //.taskStru  //init when Task_construct
    },

//    [em_app_task_dog] = {
//      .stack     = app_dog_task_stack,
//      .stackSize = DOG_TASK_STACK_SIZE,
//      .priority  = DOG_TASK_PRIORITY,
//      .taskCB    = app_watch_dog_task,
//      .arg0      = 0,
//      .arg1      = 0,
//      //.taskStru  //init when Task_construct
//    },

    [em_task_ble] = {
          .stack     = ble_task_stack,
          .stackSize = BLE_TASK_STACK_SIZE,
          .priority  = BLE_TASK_PRIORITY,
          .taskCB    = BLE_taskFxn,
          .arg0      = 0,
          .arg1      = 0,
          //.taskStru  //init when Task_construct
      },
//
    [em_app_task_ble] = {
          .stack     = app_ble_task_stack,
          .stackSize = APP_BLE_TASK_STACK_SIZE,
          .priority  = APP_BLE_TASK_PRIORITY,
          .taskCB    = app_ble_task,
          .arg0      = 0,
          .arg1      = 0,
          //.taskStru  //init when Task_construct
      },

//    [em_app_task_flash_log] = {
//          .stack     = app_flash_log_task_stack,
//          .stackSize = FALSH_LOG_TASK_STACK_SIZE,
//          .priority  = FALSH_LOG_TASK_PRIORITY,
//          .taskCB    = log_thread_entry,
//          .arg0      = 0,
//          .arg1      = 0,
//          //.taskStru  //init when Task_construct
//      },

//    stim
    [em_app_task_stim] = {
        .stack     = spApp_Stim_TaskStack,
        .stackSize = STIM_TASK_STACK_SIZE,
        .priority  = STIM_TASK_PRIORITY,
        .taskCB    = app_stim_task,
        .arg0      = 0,
        .arg1      = 0,
        //.taskStru  //init when Task_construct
    },

//    [em_app_task_rf_client] = {
//            .stack     = app_rf_client_task_stack,
//            .stackSize = RF_CLIENT_TASK_STACK_SIZE,
//            .priority  = RF_CLIENT_TASK_PRIORITY,
//            .taskCB    = rfTask_entry,
//            .arg0      = 0,
//            .arg1      = 0,
//            //.taskStru  //init when Task_construct
//        },

    [em_app_task_user] = {
      .stack     = app_user_task_stack,
      .stackSize = USER_TASK_STACK_SIZE,
      .priority  = USER_TASK_PRIORITY,
      .taskCB    = app_user_task,
      .arg0      = 0,
      .arg1      = 0,
      //.taskStru  //init when Task_construct
    },

    //stim

};

void app_task_check()
{
#if 1
    Task_Handle taskhandle = 0;
    Task_Stat statbuf;  /* declare buffer */

    taskhandle = Task_self();
    Task_stat(taskhandle, &statbuf);

    logWarning("Task 0x%08X stack usage: %d/%d bytes (%.1f%%)",
                      taskhandle,
                      statbuf.used,
                      statbuf.stackSize,
                      (statbuf.used * 100.0f) / statbuf.stackSize);
#endif
}

void app_createTask(void)
{
    Task_Params taskParams;
    Task_Handle taskHdl = NULL;
    for (uint8_t i = 0; i < ARRAY_SIZE(s_app_task_param); i++) {
        Task_Params_init(&taskParams);
        if(s_app_task_param[i].stack != NULL) {
            taskParams.stack = s_app_task_param[i].stack;
            taskParams.stackSize = s_app_task_param[i].stackSize;
            taskParams.priority = s_app_task_param[i].priority;
            taskHdl = Task_construct(&s_app_task_param[i].taskStru, s_app_task_param[i].taskCB, &taskParams, NULL);
        }
        if (taskHdl == NULL) {
            logError("task creat fail, i=%d.", i);
        }
    }
}

uint32_t app_get_systick_1ms(void)
{
    return (uint32_t)Clock_getTicks();
}

void delay_us(int us) //us: min-10us
{
    Task_sleep(us / Clock_tickPeriod);
}

void delay_ms(int ms)
{
    Task_sleep((ms * 1000) / Clock_tickPeriod);
}

uint8_t main_init(void)
{
    app_createTask();  //创建多个线程




    return 0;
}

