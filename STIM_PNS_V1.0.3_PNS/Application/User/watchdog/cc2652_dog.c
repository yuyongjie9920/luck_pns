#include "cc2652_dog.h"
#include <ti/drivers/Watchdog.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>



// 看门狗超时时间（毫秒）
#define WATCHDOG_TIMEOUT_MS  8000         // 看门狗超时时间
#define WATCHDOG_CHECK_INTERVAL_MS  2000  // 检查间隔


static Watchdog_Handle g_watchdogHandle;

/* 线程状态位图 - bit=1表示线程未更新心跳，bit=0表示线程已更新心跳 */
static volatile uint16_t g_taskStatusBits = ALL_MONITORED_TASKS;


// 任务名称数组，用于打印卡死的任务
static const char* task_names[] = {
    [em_app_task_log]        = "Log Task",
    [em_app_task_user]       = "User Task",
    [em_app_task_dog]        = "Watchdog Task",
    [em_task_ble]            = "BLE Stack Task",
    [em_app_task_ble]        = "BLE App Task",
    [em_app_task_flash_log]  = "Flash Log Task",
};

/*
 * 更新线程心跳（线程正常运行时调用）
 */
void task_watchdog(uint16_t taskId)
{
    g_taskStatusBits &= ~(taskId);
//    delay_ms(1);
}

void clear_dog(void)
{
//    Watchdog_clear(g_watchdogHandle);
//    logInfo("Watc1hdog_clear !!!!!!!!!!!!!!!");
}

static void watchdogTimeoutCallback(uintptr_t arg)
{
    // 可以在这里尝试记录一些信息到非易失性存储
    // 注意：时间很短，只能做简单操作
    logError("Stuck tasks: 0x%04X", g_taskStatusBits);

    // 打印具体卡死的任务
    for (int i = 0; i < 16; i++) {
        if (g_taskStatusBits & (1 << i)) {
            if (i < sizeof(task_names)/sizeof(task_names[0])) {
                logError("  - Task stuck: %s", task_names[i]);
            } else {
                logError("  - Task stuck: Unknown task ID %d", i);
            }
        }
    }
}

/*
 * 看门狗任务线程
 */
void app_watch_dog_task(UArg arg0, UArg arg1)
{

    // 初始化线程看门狗系统
    Watchdog_Params params;
    // 初始化看门狗驱动
    Watchdog_init();

    Watchdog_Params_init(&params);
    params.resetMode = Watchdog_RESET_ON;      // 超时时系统复位
    params.debugStallMode = Watchdog_DEBUG_STALL_OFF ; // 调试时是否暂停
    params.callbackFxn = watchdogTimeoutCallback;
    params.resetMode = Watchdog_RESET_ON;           // 明确启用复位模式
    // 打开看门狗
    g_watchdogHandle = Watchdog_open(CONFIG_WATCHDOG_0, &params);

    // 设置超时时间
    uint32_t reloadValue = Watchdog_convertMsToTicks(g_watchdogHandle, WATCHDOG_TIMEOUT_MS);
    if (reloadValue != 0) {
        Watchdog_setReload(g_watchdogHandle,             reloadValue);
    }

    // 初始喂狗
    Watchdog_clear(g_watchdogHandle);

    while (1) {
        // 检查并喂狗
//        if (g_taskStatusBits == 0) {
            Watchdog_clear(g_watchdogHandle);
//            logInfo("Watc1hdog_clear !!!!!!!!!!!!!!!");
//            // 重置所有已注册线程位为1，等待下一轮更新
//            g_taskStatusBits = ALL_MONITORED_TASKS;  // 所有位初始为1
//        } else { //调试阶段需要打开  可以排查出卡死线程 注意线程需要  清狗标志
//            logInfo("Tasks stuck: 0x%04X", g_taskStatusBits);
//        }
        delay_ms(200);
    }
}
