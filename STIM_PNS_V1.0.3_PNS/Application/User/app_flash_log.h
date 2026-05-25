/**
  ******************************************************************************
  * @file    app_log.h
  * @author  Taoking
  * @brief   实现了log的应用接口
  * @version V0.0
  * @since
  * @verbatim
  * Detailed_description_of_the_document
  * @endverbatim
  ******************************************************************************
  * @note CopyRight By XXXX Co. Ltd All right reserve
  * @attention
  *
  ******************************************************************************
  */


#ifndef __APP_FLAH_LOG_H__
#define __APP_FLAH_LOG_H__

#include "main_user.h"

#define LOG_THREAD_PRIORITY         3       ///< LOG任务优先级
#define LOG_THREAD_STACK_SIZE       1536    ///< LOG任务堆栈大小
#define LOG_THREAD_TIMESLICE        10


/* RT-Thread error code definitions */
#define RT_EOK                          0               /**< There is no error */
#define RT_ERROR                        1               /**< A generic error happens */
#define RT_ETIMEOUT                     2               /**< Timed out */
#define RT_EFULL                        3               /**< The resource is full */
#define RT_EEMPTY                       4               /**< The resource is empty */
#define RT_ENOMEM                       5               /**< No memory */
#define RT_ENOSYS                       6               /**< No system */
#define RT_EBUSY                        7               /**< Busy */
#define RT_EIO                          8               /**< IO error */
#define RT_EINTR                        9               /**< Interrupted system call */
#define RT_EINVAL                       10              /**< Invalid argument */


// 用于控制是否允许传输日志
#define TRAN_LOG_ENABLE     0
#define TRAN_LOG_DISABLE    1

// 用于指示传输日志的状态
#define TRAN_LOG_RUNNING    0   ///< 正在传输日志
#define TRAN_LOG_STOP       1   ///< 未在传输日志


/*! 发送的每条日志信息的包大小          。注意：此处的大小是要发送的日志数据大小，不是整个数据帧大小
 *  即可选数据大小。如果使用加密传输，考虑到crc128_cbc只支持16整数倍的加解密，为了方便
 *  (APP_LOG_EVERY_PACKAGE_SIZE + 6) % 16 == 0，其中6表示除了帧头、帧尾和可选数据的其他字段
 *  占用的字节数，获取日志的大小还需要4字节对齐。
*/
//#define APP_LOG_EVERY_PACKAGE_SIZE  ((((BLE_MTU - 2) / 16) * 16 - 6) / 4 *4)

#define RT_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

struct app_log_client
{
//  rt_size_t recv_bufsz;
    uint32_t log_size;          /* 可发送的log消息字节总数 */
//  rt_mailbox_t mailbox;       /* 用于发送log消息的邮箱 */
  Mailbox_Struct *mailbox;      /* 用于发送log消息的邮箱 */
//  rt_thread_t thread;
};

typedef struct app_log_client *log_client_t;

extern int8_t send_data_to_mailbox(struct Mailbox_Struct *mailbox, char *data, size_t size);

extern uint8_t transmit_log_switch;
// 存储传输日志状态
extern uint8_t transmit_log_state;

extern struct app_log_client log_client;

void log_thread_entry(UArg arg0, UArg arg1);

#endif

