#ifndef __MAIN_USER_H_
#define __MAIN_USER_H_


#include "app_define.h"



#define APP_HWREG(x)  (*((volatile unsigned long *)(x)))
#define APP_HWREGB(x) (*((volatile unsigned char *)(x)))
#define APP_IO_TOGGLE(index)     (APP_HWREG( GPIO_BASE + GPIO_O_DOUTTGL31_0 ) = ( 1 << index ))  //copy from "GPIO_toggle"
#define APP_IO_WRITE(index, val) (APP_HWREGB( GPIO_BASE + index ) = val)
#define APP_IO_READ(index)       (( APP_HWREG( GPIO_BASE + GPIO_O_DIN31_0 ) >> index ) & 1 )




/*
 * 32-bit integer manipulation macros (little endian)
 */
#ifndef GET_ULONG_LE
#define GET_ULONG_LE(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ]       )        \
        | ( (unsigned long) (b)[(i) + 1] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 2] << 16 )        \
        | ( (unsigned long) (b)[(i) + 3] << 24 );       \
}
#endif

#ifndef PUT_ULONG_LE
#define PUT_ULONG_LE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n)       );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 24 );       \
}
#endif

#ifndef GET_UINT32_LE
#define GET_UINT32_LE(b,i)                              \
    (                                                   \
              ( (unsigned long) (b)[(i)    ]       )    \
            | ( (unsigned long) (b)[(i) + 1] <<  8 )    \
            | ( (unsigned long) (b)[(i) + 2] << 16 )    \
            | ( (unsigned long) (b)[(i) + 3] << 24 )    \
    )
#endif

#ifndef PUT_UINT32_LE
#define PUT_UINT32_LE(n,b,i)                                \
    {                                                   \
        (b)[(i)    ] = (unsigned char) ( (n)       );   \
        (b)[(i) + 1] = (unsigned char) ( (n) >>  8 );   \
        (b)[(i) + 2] = (unsigned char) ( (n) >> 16 );   \
        (b)[(i) + 3] = (unsigned char) ( (n) >> 24 );   \
    }
#endif

#ifndef GET_UINT16_LE
#define GET_UINT16_LE(b,i)                              \
    (                                                   \
              ( (unsigned long) (b)[(i)    ]       )    \
            | ( (unsigned long) (b)[(i) + 1] <<  8 )    \
    )
#endif

#ifndef PUT_UINT16_LE
#define PUT_UINT16_LE(n,b,i)                                \
    {                                                   \
        (b)[(i)    ] = (unsigned char) ( (n)       );   \
        (b)[(i) + 1] = (unsigned char) ( (n) >>  8 );   \
    }
#endif



#define RT_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            logInfo("Assertion failed: %s at %s:%d\n", #condition, __FILE__, __LINE__); \
            while(1);\
        } \
    } while(0)

struct app_task_param {
    uint8_t priority;
    uint16_t stackSize;
    uint8_t *stack;
    int arg0;
    int arg1;
    void (*taskCB)(uint32_t, uint32_t);
    uint16_t used;
    Task_Struct taskStru;    //addr is task handle.
};

enum em_app_task {
    em_app_task_log = 0,
    em_task_ble,
    em_app_task_ble,
    em_app_task_user,
    em_app_task_dog,
    em_app_task_flash_log,
    em_app_task_rf_client,
    em_app_task_stim,
    em_app_task_max
};

void delay_us(int us);
void delay_ms(int ms);


void app_task_check();

uint8_t main_init(void);


#endif //__MAIN_USER_H_
