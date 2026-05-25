#include "app_log.h"
#include "drivers/UART2.h"
#include "elog.h"
#include "ef_def.h"
#include "main_user.h"
#include "app_rtc.h"
#include "elog_flash.h"
#include "easyflash.h"
#include "app_rtc.h"


#include "main_user.h"
#if 1
    #define log(...) logInfo(__VA_ARGS__)
#else
    #define log(...) do {} while(0)
#endif


static void shellLogWrite(char *buffer, short len);

static Shell shell;
static UART2_Handle logUartHandle = 0;
static UART2_Params params;
static Log uartLog = {
    .write = shellLogWrite,
    .active = true,
    .level = LOG_DEBUG
};

char shellBuffer[APP_LOG_BUFF_SIZE];


void put_buf(const uint8_t *buf, size_t len) {

    void *return_addr = __builtin_return_address(0);

    mylog("put_buf len = %d  \r\ndata : ", len);
    for (size_t i = 0; i < len; i++) {
        mylog("0x%02x", buf[i]);
        // 判断是否换行
        if (i == len - 1) {
            mylog("\r\n");  // 最后一个数据
        } else if ((i + 1) % 16 == 0) {
            mylog(",\r\n"); // 每16个换行
        } else {
            mylog(", ");    // 同一行内
        }
    }
    mylog("\r\n");  // 最后一个数据
}
void put_buf1(const uint8_t *buf, size_t len)
{
    mylog("put_buf len = %d  \r\ndata : \r\n", len);

    for (size_t i = 0; i < len; i++) {
        mylog("%02x", buf[i]);
    }
    mylog("\r\n");
    mylog("\r\n");
}


static short shellRead(char *data, unsigned short len)
{
    unsigned short bytes;
    UART2_read(logUartHandle, data, 1, (size_t *)&bytes);
    return 1;
}


static short shellWrite(char *data, unsigned short len)
{
    unsigned short bytes;
    UART2_write(logUartHandle, data, len, (size_t *)&bytes);  
    return bytes;  
}

void printHelloWorld(void) {
    static const char *hello = "hello world";
    unsigned short len = strlen(hello);
    shellWrite((char *)hello, len);
}

void shellWriteEndLine1(Shell *shell, char *buffer, int len);

static void shellLogWrite(char *buffer, short len)
{
    if (uartLog.shell) {
        shellWriteEndLine1(uartLog.shell, buffer, (int)len);
    }
    
}

#if SHELL_USING_LOCK
static int shellLock(struct shell_def *shell)
{
    //mutex_key = GateMutexPri_enter(shellMutex);
    return 0;  
}

static int shellUnlock(struct shell_def *shell)
{
    //GateMutexPri_leave(shellMutex, mutex_key);
    return 0;  
}
#endif

// 直接设置UART2中断优先级为最高
#include <ti/sysbios/family/arm/m3/Hwi.h>

void app_log_init(void) 
{
    UART2_Params_init(&params);
    params.baudRate = 921600;
    params.readMode = UART2_Mode_BLOCKING;
#if 1
    params.writeMode = UART2_Mode_BLOCKING;
#else //如果遇到死机可以尝试用非阻塞  如果阻塞模式在中断中加打印 会触发 中断嵌套 有概率死机
    params.writeMode = UART2_Mode_NONBLOCKING;//非阻塞模式可以在中断中调用打印
#endif
    logUartHandle = UART2_open(CONFIG_UART2_0_SHELL_LOG_CONST, &params);

    UART2_rxEnable(logUartHandle);

    shell.read = shellRead;
    shell.write = shellWrite;
    #if SHELL_USING_LOCK
    shell.lock = shellLock;
    shell.unlock = shellUnlock;
    #endif
    shellInit(&shell, shellBuffer, APP_LOG_BUFF_SIZE);

    logRegister(&uartLog, &shell);    
}

void init_eflash_log(void) //这个初始化不能放在main里面 需要放在线程里面初始化  有点小坑
{
   if(easyflash_init() != EF_NO_ERR) {
     rt_kprintf("easyflash_init ERROR\r\n");
   }
   int err = elog_init();
   if(err == ELOG_NO_ERR) {
     elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO);
     elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
     elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
     elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
     elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_P_INFO));
     elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_P_INFO));
     elog_flash_init();
     elog_start();
   } else {
       logInfo("init fail");
   }
}


void app_log_task(UArg arg0, UArg arg1)
{
    app_log_init();
    init_eflash_log(); //初始化 flash存储log
    app_rtc_init();      //初始化RCT
    for (;;) {
        shellTask(&shell);
//        app_task_check();

        delay_ms(1);
    } 
}

