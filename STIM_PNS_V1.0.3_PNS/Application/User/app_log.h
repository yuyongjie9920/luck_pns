#ifndef __APP_LOG_H_
#define __APP_LOG_H_
#include "main_user.h"
#include "log.h"
//#include "elog.h"

#define  APP_LOG_BUFF_SIZE   (1024*2)

void put_buf(const uint8_t *buf, size_t len); //打印内存内容 16进制
void put_buf1(const uint8_t *buf, size_t len); //打印内存内容 没有 0x 空格
void app_log_task(uint32_t arg0, uint32_t arg1);

#endif //__APP_LOG_H_
