#ifndef __APP_LED_KEY_H_
#define __APP_LED_KEY_H_
#include <bsp_led/bsp_led.h>
#include "main_user.h"

//According to your need to modify the constants.
//#define TICKS_INTERVAL    50//10	//ms

#define  KEY_CHECK_TIME_MS    TICKS_INTERVAL

typedef enum {
    EM_LED_ID_STIM  = 0,
    EM_LED_ID_BAT = 1,
    EM_LED_ID_BLE = 2,
    EM_LED_MAX   //need to modified 'LED_MAX_NUM' in config_user.h
} led_id_enum;

void app_led_init(void);

void app_led_key_init(void);

#endif //__APP_LED_KEY_H_
