///**
//  ******************************************************************************
//  * @file    app_log.c
//  * @author  Taoking
//  * @brief   实现了log的应用接口
//  * @version V0.0
//  * @since
//  * @verbatim
//  * Detailed_description_of_the_document
//  * @endverbatim
//  ******************************************************************************
//  * @note CopyRight By XXXX Co. Ltd All right reserve
//  * @attention
//  *
//  ******************************************************************************
//  */
//
///* Includes ------------------------------------------------------------------*/
//#define LOG_TAG     "APP.LOG"       ///< 该模块对应的标签。不定义时，默认：NO_TAG
//#define LOG_LVL     ELOG_LVL_INFO   ///< 该模块对应的日志输出级别。不定义时，默认：调试级别
//#include <elog.h>                   ///< 必须在 LOG_TAG 与 LOG_LVL 下面
//#include <app_flash_log.h>
//
//#include <elog.h>                 // 必须在 LOG_TAG 与 LOG_LVL 下面
//#include <easyflash.h>
//#include <elog_flash.h>
//#include "app_flash_log.h"
//#include "flash_interface_ext_rtos_NVS.h"
//#include <ti/common/cc26xx/flash_interface/flash_interface.h>
//#include "app_flash_log.h"
//#include "app_ble.h"
//#include "main_user.h"
//
//#define DEBUG_MOREPRINT
////ALIGN(RT_ALIGN_SIZE)
//// static char log_stack[LOG_THREAD_STACK_SIZE];
//static char logTaskStack[LOG_THREAD_STACK_SIZE];
//
//struct app_log_client log_client;
//// static struct rt_mailbox log_mailbox;            ///< log任务的邮箱
//// static uint8_t log_mailbox_pool[8];              ///< 用于放邮件的内存池 此处可以存放2条邮箱信息
//static Mailbox_Struct logMailbox_Struct;        ///< log任务的邮箱
//// static struct rt_thread log_thread;
//
//static Task_Struct logTask_Struct;
////static Task_Handle logTask_Handle;
//
//
//
//// 允许传输日志
//uint8_t transmit_log_switch = TRAN_LOG_ENABLE;
//// 存储传输日志状态
//uint8_t transmit_log_state = TRAN_LOG_STOP; ///< 初始为：未在传输日志
//
//
//
///**
//  * @brief  发送数据到指定邮箱
//  * @param mailbox 邮箱对象指针。指定发送给哪个邮箱
//  * @param  data 发送数据的指针,需要外部分配内存空间
//  * @param  size 数据的大小
//  *
//  * @retval void
//  *
//  * @author Taoking
//  * @date
//  * @note 接收此邮箱数据的函数需要调用free(msg_ptr)释放邮箱的内存
//  */
//int8_t send_data_to_mailbox(struct Mailbox_Struct *mailbox, char *data, size_t size)
//{
//    RT_ASSERT(mailbox);
//    RT_ASSERT(data);
////  RT_ASSERT(size);
//
//    struct mailbox_msg msg;
//
//    struct mailbox_msg *msg_ptr = &msg;
//    int8_t err= RT_EOK;
//
////  logInfo("send data to mailbox");
//
////  msg_ptr = malloc(sizeof(struct mailbox_msg));
////  if(msg_ptr == NULL)
////  {
////      logInfo("malloc fail");
////      return RT_ENOMEM;
////  }
//
//    msg_ptr->ptr = (uint8_t *)data;
//    msg_ptr->size = size;
//
//    if(Mailbox_post(mailbox, msg_ptr, BIOS_WAIT_FOREVER) != true)
//    {
//        err = RT_ERROR;
//
//        logInfo("send data to mailbox err[%d]", err);
//    }
//    return err;
//}
//
///**
//  * @brief 从easyflash分区中读出日志并以二进制方式发送到APP端(通过蓝牙模块).
//  *
//  * @param index 初始的日志偏移量(单位是字节)，需要4字节对齐
//  *        Minimum index is 0.
//  *        Maximum index is log used flash total size - 1.
//  * @param size 输出日志总的的字节数，需要4字节对齐
//  * @retval void
//  *
//  * @author Taoking
//  * @date   2021-03-22
//  * @note 需要保证4字节对齐后的index+size不能超过实际存储的日志大小
//  */
//int8_t send_log_to_app(size_t index, size_t size)
//{
//    uint8_t *buf = NULL;
//    size_t read_size = 0;
//    size_t package_size = 0;
//    uint16_t elog_num = 0;              // 当前日志包编号
//    uint16_t elog_num_total;                // 日志总包数
////  uint8_t elog_num_percent = 0;       // 日志传输包数百分比
////  uint8_t elog_num_percent_tmp = 0;   // 日志传输包数百分比
//
//    leadinno_cmd_error_t err = LEAD_CMD_OK;
//    EfErrCode result = EF_NO_ERR;
//
//    // 创建日志发送缓冲区
//    buf = malloc(APP_LOG_EVERY_PACKAGE_SIZE);
//    if(buf == NULL)
//    {
//        return -RT_ENOMEM;
//    }
//
//    // 日志总包数
//    elog_num_total = (size / APP_LOG_EVERY_PACKAGE_SIZE) + ((size % APP_LOG_EVERY_PACKAGE_SIZE)?1:0);
//
//#ifdef DEBUG_MOREPRINT
//    logInfo("logs_num_total:[%d]", elog_num_total);
//#endif
//
//    delay_ms(20);
//    // 设置为正在传输日志状态
//    transmit_log_state = TRAN_LOG_RUNNING;
//
//    while (1)
//    {
//        err = LEAD_CMD_OK;
//
//        // 每次发送日志给APP前，需要判断APP连接状态
//        if(ble_client.connect_state == APP_CONNECT_STATE_VERIFY2)
//        {
//            // 收到停止日志传输的命令，退出循环
//            if(transmit_log_switch == TRAN_LOG_DISABLE)
//            {
//                delay_ms(300);
//                err = send_normal_frame_to_app(STOP_LOG_TRAN_CMD, 0, NULL, 0);
//                if(err == LEAD_FRAME_FORMAT_OK)
//                {
//#ifdef DEBUG_MOREPRINT
//                    logInfo("stop logs[num:%d] transmit", elog_num);
//#endif
//                }
//                else
//                {
//                    logInfo("stop logs transmit[0x%02x]", err);
//                }
//
//                err = LEAD_CMD_OK;
//                logInfo("transmit_log_switch == TRAN_LOG_DISABLE");
//                break;
//            }
//
//            if(read_size + APP_LOG_EVERY_PACKAGE_SIZE < size)
//            {
//                package_size = APP_LOG_EVERY_PACKAGE_SIZE;
//            }
//            else
//            {
//                package_size = size - read_size;
//            }
//
//            result = ef_log_read(index + read_size, (uint32_t *)buf, package_size);
//            if(result != EF_NO_ERR)
//            {
//                logInfo("ef_log_read[%d]", result);
//            }
//
////          logInfo("package_size[%d]", package_size + 8);
//            // 等待蓝牙模块的PA04(RTS)管脚为高电平，指示MCU可以给蓝牙模块发送数据
////          while(gpio_input_bit_get(BLE_RTS_PORT, BLE_RTS_PIN) == 0);
//
//            err = send_normal_frame_to_app(GET_LOG_CMD, (elog_num << 8) | elog_num_total, buf, package_size);
//            if(err == LEAD_CMD_OK)
//            {
//                  logInfo("send logs22[%d]", elog_num);
//            }
//            else
//            {
//                logInfo("send logs11[0x%02x]", err);
//            }
//
////          LOG_W("elog_num[%d]", elog_num);
////          elog_num_percent = elog_num * 100 / elog_num_total;
////          if(elog_num_percent != elog_num_percent_tmp)
////          {
////              elog_num_percent_tmp = elog_num_percent;
////              logInfo("send logs: [%d%]", elog_num_percent_tmp);
////          }
//
//            // 传输未完成，增加读索引
//            if(read_size + APP_LOG_EVERY_PACKAGE_SIZE < size)
//            {
//                read_size += APP_LOG_EVERY_PACKAGE_SIZE;
//            }
//            else // 传输完成，退出while循环
//            {
//                logInfo("end_send_flash_log");
//
//                break;
//            }
//        }
//        else
//        {
//            logInfo("APP disconnect,stop log");
//            err = LEAD_GENERAL_ERR;
//            break;
//        }
//
//        elog_num++;
//
//        delay_ms(60);
//    }
//
//    // 设置为未在传输日志状态
//    transmit_log_state = TRAN_LOG_STOP;
//
//    // 允许传输日志
//    transmit_log_switch = TRAN_LOG_ENABLE;
//
//    free(buf);
//
//    if(err == LEAD_CMD_OK)
//    {
//        logInfo("send logs:[100%%]");
//    }
//
//    delay_ms(40);
//
//    return err;
//}
//
//
///**
//  * @brief 发送日志到app处理任务.
//  * 收到APP端发送的获取日志信息后，首先发送一条确认收到信息，其中包含接下来要发送的总的字节数
//  * 然后再逐条发送日志信息到APP @see send_read_flash_log
//  *
//  * @param client 日志对象的结构体指针
//  * @retval void
//  *
//  * @author Taoking
//  * @date   2021-03-22
//  * @note
//  */
//
//void log_thread_entry(UArg arg0, UArg arg1)
//{
////static void log_thread_entry(log_client_t client)
//    int8_t err;
////  char * result = NULL;
//    // 当前已存储的日志大小
//
//    uint32_t log_size;
//
//    size_t offset;
//    size_t size;
//
//    size_t offset_1;
//    size_t size_1;
//
//    uint8_t *data = NULL;
//    struct mailbox_msg msg = {0, };
//    struct mailbox_msg *msg_ptr = &msg;
//
//
//    log_client.log_size = 0;   /* 可发送的log消息总字节数初始化为0 */
//    log_client.mailbox = &logMailbox_Struct;
//
//    /* 初始化一个 mailbox */
//    Mailbox_Params mailboxParams;
//    // Configure mailbox
//    Mailbox_Params_init(&mailboxParams);
//    Mailbox_construct(log_client.mailbox, sizeof(struct mailbox_msg), 4, &mailboxParams, NULL);
//
//    delay_ms(2000);
//
//
//    while (1)
//    {
//        /* 阻塞等待接收邮箱数据 */
//        if(Mailbox_pend(log_client.mailbox, msg_ptr, BIOS_WAIT_FOREVER) != true)
//        {
//            logInfo("get log_mailbox fail");
//            continue;
//        }
//
//        data = msg_ptr->ptr;
//
//        // 获取当前已存储的日志大小
//        log_size = (uint32_t)ef_log_get_used_size();
//
//        /* word alignment for index and size */
//        log_size = RT_ALIGN_DOWN(log_size, 4);
//
//        // 获取APP请求的日志偏移量和日志大小
//        offset_1 = GET_UINT32_LE(data, 0);
//        size_1 = GET_UINT32_LE(data, 4);
////#ifdef DEBUG_MOREPRINT
//        logInfo("log_offsetooo[%d] log_size[%d]", offset_1, size_1);
////#endif
//        /* word alignment for index and size */
//        offset = RT_ALIGN_DOWN(offset_1, 4);
//        size = RT_ALIGN_DOWN(size_1, 4);
//
//        // 如果获取的日志偏移量超过总的日志大小
//        if(offset > log_size)
//        {
//            err = send_normal_frame_to_app(GET_LOG_CMD, 0, NULL, 0);
//            if(err == LEAD_CMD_OK)
//            {
////              logInfo("send logs[%d]", elog_num);
//            }
//            else
//            {
//                logInfo("log_offset[%d] log_size[%d]", offset, size);
//                logInfo("send logs111[0x%02x]", err);
//            }
//        }
//        else
//        {
//            if((offset + size) > log_size)
//            {
//                size = log_size - offset;
//            }
//
////#ifdef DEBUG_MOREPRINT
//            logInfo("start log transmit");
//            logInfo("actual log offset00[%d] size[%d]", offset, size);
//            logInfo("actual log offset11[%d] size[%d]", offset_1, size_1);
//            logInfo("start log transmit");
////#endif
//            err = send_log_to_app(offset, size);
//
//            if(err != RT_EOK)
//            {
//                logInfo("log_offset[%d] log_size[%d]", offset, size);
//                logInfo("get logs err111[%d]", err);
//            } else {
//                logInfo("ok send_log_to_app");
//            }
//        }
//
//        free(msg_ptr->ptr);
////      free(msg_ptr);
//    }
//}
//
