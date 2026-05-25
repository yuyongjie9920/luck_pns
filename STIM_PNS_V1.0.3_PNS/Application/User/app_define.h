#ifndef __APP_DEGINE_H__
#define __APP_DEGINE_H__

#include <stddef.h>
#include <stdint.h>
#include "stdlib.h"
#include <string.h>
#include <icall.h>
#include <math.h>
#include "util.h"

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/AESCBC.h>
#include "ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h"
#include <ti/drivers/TRNG.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include "ti/sysbios/knl/Mailbox.h"
#include <ti/drivers/Watchdog.h>
#include <ti/drivers/DAC.h>
#include <ti/drivers/ADC.h>
#include <ti/devices/DeviceFamily.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>
#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>
#include <ti_drivers_config.h>
#include <ti/drivers/NVS.h>
#include <ti/common/cc26xx/flash_interface/flash_interface.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#ifndef ARRAY_SIZE
#define  ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif


#include <stddef.h>
#include <stdint.h>
#include <ti/sysbios/knl/Clock.h>

#include "log.h"

#ifndef ARRAY_SIZE
#define  ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

extern uint32_t app_get_systick_1ms(void);
#define  sys_get_tick_ms() ((uint32_t)(app_get_systick_1ms()))


//#define USING_RTC_SD8563
//#define USE_CC2652_KEY
#define USE_ELOG
//#define USE_LED
////#define USE_BEEP
////#define USER_COPROCESSOR
//#define USE_ADC
#define USE_BLE
//#define USE_DOG
//#define USE_IN_NVS

#include "app_log.h"
#include "log.h"


#ifdef USE_BLE
#include "simple_peripheral.h"
#include "icall_ble_api.h"
#include "ble_data_gatt.h"
//#include "app_verify_ble.h"
//#include "app_ble_data.h"
#include "ble_data_gatt.h"
#endif

#ifdef USE_DOG
#include <ti/drivers/Watchdog.h>
#endif

#ifdef USE_ADC
#include <ti/drivers/DAC.h>
#include <ti/drivers/ADC.h>
#endif


#ifdef USER_COPROCESSOR
#include "scif_framework.h"
#include "scif_osal_tirtos.h"
#include "scif.h"
#include <ti/devices/DeviceFamily.h>
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>
#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>
#include "coprocessor.h"
#include DeviceFamily_constructPath(driverlib/aux_adc.h)
#include "ti_drivers_config.h"
#endif

#include "app_user.h"

#ifdef USE_BEEP
#include "ti_buzzer.h"
#endif

#ifdef USE_LED
#include "CC2652_led.h"
#include "bsp_led.h"
#endif

#ifdef USE_CC2652_KEY
#include "multi_button.h"
#include "CC2652_key.h"
#include "key_activity.h"
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>
#include "util.h"
#include <ti_drivers_config.h>
#endif

#ifdef USING_RTC_SD8563
#define TI_EPOCH_YEAR 1900
#include "sd8563.h"
#include "app_rtc.h"
#endif

#ifdef USE_IN_NVS
#include <ti/drivers/NVS.h>
#endif


#ifdef USE_ELOG
#include <elog.h>                 // 必须在 LOG_TAG 与 LOG_LVL 下面
#include <easyflash.h>
#include <elog_flash.h>
#include "app_flash_log.h"
#include "flash_interface_ext_rtos_NVS.h"
#include <ti/common/cc26xx/flash_interface/flash_interface.h>
#include "app_flash_log.h"
#define LOG_E                      log_e
#define LOG_W                      log_w
#define LOG_I                      log_i
#define LOG_D                      log_d
#endif

#endif
