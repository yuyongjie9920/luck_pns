#ifndef CC2652_TASK_WATCHDOG_H
#define CC2652_TASK_WATCHDOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main_user.h"

/* 线程ID定义 - 每个位代表一个线程 */
#define TASK_ID_LOG          0x0010  /* 日志打印线程 */
#define TASK_ID_BLE          0x0001  /* 系统BLE线程 */
#define TASK_ID_APP_BLE      0x0002  /* 蓝牙应用线程 */
#define TASK_ID_FLASH_LOG    0x0004  /* flash日志线程 */
#define TASK_ID_USER         0x0008  /* 主任务线程 */

/* 定义需要监控的线程标志集合 */
#define ALL_MONITORED_TASKS  (TASK_ID_BLE      |\
                              TASK_ID_APP_BLE  |\
                              TASK_ID_FLASH_LOG|\
                              TASK_ID_USER)

void task_watchdog(uint16_t taskId);

void app_watch_dog_task(UArg arg0, UArg arg1);

void clear_dog(void);

#ifdef __cplusplus
}
#endif

#endif /* CC2652_TASK_WATCHDOG_H */
