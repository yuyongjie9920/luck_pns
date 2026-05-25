/*
 *  ======== ti_drivers_config.h ========
 *  Configured TI-Drivers module declarations
 *
 *  The macros defines herein are intended for use by applications which
 *  directly include this header. These macros should NOT be hard coded or
 *  copied into library source code.
 *
 *  Symbols declared as const are intended for use with libraries.
 *  Library source code must extern the correct symbol--which is resolved
 *  when the application is linked.
 *
 *  DO NOT EDIT - This file is generated for the CC2652R1FSIP
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_CC2652R1FSIP
#ifndef DeviceFamily_CC26X2
#define DeviceFamily_CC26X2
#endif

#include <ti/devices/DeviceFamily.h>

#include <stdint.h>

/* support C++ sources */
#ifdef __cplusplus
extern "C" {
#endif


/*
 *  ======== CCFG ========
 */


/*
 *  ======== ADC ========
 */

/* DIO26 */
extern const uint_least8_t              CONFIG_ADC_V_TEMP_CONST;
#define CONFIG_ADC_V_TEMP               0
#define CONFIG_TI_DRIVERS_ADC_COUNT     1


/*
 *  ======== AESCBC ========
 */

extern const uint_least8_t                  CONFIG_AESCBC_0_CONST;
#define CONFIG_AESCBC_0                     0
#define CONFIG_TI_DRIVERS_AESCBC_COUNT      1


/*
 *  ======== AESCCM ========
 */

extern const uint_least8_t                  CONFIG_AESCCM0_CONST;
#define CONFIG_AESCCM0                      0
#define CONFIG_TI_DRIVERS_AESCCM_COUNT      1


/*
 *  ======== AESCTRDRBG ========
 */

extern const uint_least8_t                      CONFIG_AESCTRDRBG_0_CONST;
#define CONFIG_AESCTRDRBG_0                     0
#define CONFIG_TI_DRIVERS_AESCTRDRBG_COUNT      1


/*
 *  ======== AESECB ========
 */

extern const uint_least8_t                  CONFIG_AESECB0_CONST;
#define CONFIG_AESECB0                      0
#define CONFIG_TI_DRIVERS_AESECB_COUNT      1


/*
 *  ======== ECDH ========
 */

extern const uint_least8_t              CONFIG_ECDH0_CONST;
#define CONFIG_ECDH0                    0
#define CONFIG_TI_DRIVERS_ECDH_COUNT    1


/*
 *  ======== GPIO ========
 */
/* Owned by CONFIG_ADC_V_TEMP as  */
extern const uint_least8_t CONFIG_GPIO_ADC_V_TEMP_AIN_CONST;
#define CONFIG_GPIO_ADC_V_TEMP_AIN 26

extern const uint_least8_t CONFIG_GPIO_SPI_IRQ_CONST;
#define CONFIG_GPIO_SPI_IRQ 22

extern const uint_least8_t CONFIG_GPIO_STIM_CS_CONST;
#define CONFIG_GPIO_STIM_CS 23

extern const uint_least8_t CONFIG_GPIO_POWER_3V3_EN_CONST;
#define CONFIG_GPIO_POWER_3V3_EN 9

extern const uint_least8_t CONFIG_GPIO_RGB_G_CONST;
#define CONFIG_GPIO_RGB_G 14

extern const uint_least8_t CONFIG_GPIO_RGB_O_CONST;
#define CONFIG_GPIO_RGB_O 15

extern const uint_least8_t CONFIG_GPIO_RGB_B_CONST;
#define CONFIG_GPIO_RGB_B 11

extern const uint_least8_t CONFIG_GPIO_C_STAT_CONST;
#define CONFIG_GPIO_C_STAT 7

extern const uint_least8_t CONFIG_GPIO_KEY_MF_EN_CONST;
#define CONFIG_GPIO_KEY_MF_EN 0

extern const uint_least8_t CONFIG_GPIO_KEY_UP_CONST;
#define CONFIG_GPIO_KEY_UP 3

extern const uint_least8_t CONFIG_GPIO_KEY_DOWN_CONST;
#define CONFIG_GPIO_KEY_DOWN 8

extern const uint_least8_t CONFIG_GPIO_RTC_INT_CONST_CONST;
#define CONFIG_GPIO_RTC_INT_CONST 29

extern const uint_least8_t CONFIG_GPIO_EN_5V_CONST;
#define CONFIG_GPIO_EN_5V 25

extern const uint_least8_t CONFIG_GPIO_CH_SENSE_CONST;
#define CONFIG_GPIO_CH_SENSE 1

extern const uint_least8_t CONFIG_GPIO_ALRT_CONST;
#define CONFIG_GPIO_ALRT 5

extern const uint_least8_t CONFIG_GPIO_KEY_LOCK_CONST;
#define CONFIG_GPIO_KEY_LOCK 30

/* Owned by CONFIG_I2C_RTC as  */
extern const uint_least8_t CONFIG_GPIO_I2C_RTC_SDA_CONST;
#define CONFIG_GPIO_I2C_RTC_SDA 4

/* Owned by CONFIG_I2C_RTC as  */
extern const uint_least8_t CONFIG_GPIO_I2C_RTC_SCL_CONST;
#define CONFIG_GPIO_I2C_RTC_SCL 6

/* Owned by CONFIG_SPI_STIM as  */
extern const uint_least8_t CONFIG_GPIO_SPI_STIM_SCLK_CONST;
#define CONFIG_GPIO_SPI_STIM_SCLK 24

/* Owned by CONFIG_SPI_STIM as  */
extern const uint_least8_t CONFIG_GPIO_SPI_STIM_POCI_CONST;
#define CONFIG_GPIO_SPI_STIM_POCI 20

/* Owned by CONFIG_SPI_STIM as  */
extern const uint_least8_t CONFIG_GPIO_SPI_STIM_PICO_CONST;
#define CONFIG_GPIO_SPI_STIM_PICO 21

/* Owned by CONFIG_SPI_FLASH as  */
extern const uint_least8_t CONFIG_GPIO_SPI_FLASH_SCLK_CONST;
#define CONFIG_GPIO_SPI_FLASH_SCLK 17

/* Owned by CONFIG_SPI_FLASH as  */
extern const uint_least8_t CONFIG_GPIO_SPI_FLASH_POCI_CONST;
#define CONFIG_GPIO_SPI_FLASH_POCI 18

/* Owned by CONFIG_SPI_FLASH as  */
extern const uint_least8_t CONFIG_GPIO_SPI_FLASH_PICO_CONST;
#define CONFIG_GPIO_SPI_FLASH_PICO 16

/* Owned by CONFIG_UART2_0_SHELL_LOG as  */
extern const uint_least8_t CONFIG_GPIO_UART2_0_SHELL_LOG_TX_CONST;
#define CONFIG_GPIO_UART2_0_SHELL_LOG_TX 13

/* Owned by CONFIG_UART2_0_SHELL_LOG as  */
extern const uint_least8_t CONFIG_GPIO_UART2_0_SHELL_LOG_RX_CONST;
#define CONFIG_GPIO_UART2_0_SHELL_LOG_RX 12

/* Owned by CONFIG_NVS_SPI_1 as  */
extern const uint_least8_t CONFIG_GPIO_FLASH_CS_CONST;
#define CONFIG_GPIO_FLASH_CS 19

/* The range of pins available on this device */
extern const uint_least8_t GPIO_pinLowerBound;
extern const uint_least8_t GPIO_pinUpperBound;

/* LEDs are active high */
#define CONFIG_GPIO_LED_ON  (1)
#define CONFIG_GPIO_LED_OFF (0)

#define CONFIG_LED_ON  (CONFIG_GPIO_LED_ON)
#define CONFIG_LED_OFF (CONFIG_GPIO_LED_OFF)


/*
 *  ======== I2C ========
 */

/*
 *  SCL: DIO6
 *  SDA: DIO4
 */
extern const uint_least8_t              CONFIG_I2C_RTC_CONST;
#define CONFIG_I2C_RTC                  0
#define CONFIG_TI_DRIVERS_I2C_COUNT     1

/* ======== I2C Addresses and Speeds ======== */
#include <ti/drivers/I2C.h>

/* ---- CONFIG_I2C_RTC I2C bus components ---- */

/* no components connected to CONFIG_I2C_RTC */

/* max speed unspecified, defaulting to 100 kbps */
#define CONFIG_I2C_RTC_MAXSPEED   (100U) /* kbps */
#define CONFIG_I2C_RTC_MAXBITRATE ((I2C_BitRate)I2C_100kHz)


/*
 *  ======== NVS ========
 */

extern const uint_least8_t              CONFIG_NVSINTERNAL_CONST;
#define CONFIG_NVSINTERNAL              0
/*
 *  PICO: DIO16
 *  POCI: DIO18
 *  SCLK: DIO17
 *  CSN: undefined
 */
extern const uint_least8_t              CONFIG_NVSEXTERNAL_CONST;
#define CONFIG_NVSEXTERNAL              1
#define CONFIG_TI_DRIVERS_NVS_COUNT     2




/*
 *  ======== SPI ========
 */

/*
 *  PICO: DIO21
 *  POCI: DIO20
 *  SCLK: DIO24
 */
extern const uint_least8_t              CONFIG_SPI_STIM_CONST;
#define CONFIG_SPI_STIM                 0
/*
 *  PICO: DIO16
 *  POCI: DIO18
 *  SCLK: DIO17
 */
extern const uint_least8_t              CONFIG_SPI_FLASH_CONST;
#define CONFIG_SPI_FLASH                1
#define CONFIG_TI_DRIVERS_SPI_COUNT     2


/*
 *  ======== TRNG ========
 */

extern const uint_least8_t              CONFIG_TRNG_0_CONST;
#define CONFIG_TRNG_0                   0
extern const uint_least8_t              CONFIG_TRNG_1_CONST;
#define CONFIG_TRNG_1                   1
#define CONFIG_TI_DRIVERS_TRNG_COUNT    2


/*
 *  ======== Timer ========
 */

extern const uint_least8_t                  CONFIG_TIMER_PWM_CONST;
#define CONFIG_TIMER_PWM                    0
#define CONFIG_TI_DRIVERS_TIMER_COUNT       1


/*
 *  ======== UART2 ========
 */

/*
 *  TX: DIO13
 *  RX: DIO12
 */
extern const uint_least8_t                  CONFIG_UART2_0_SHELL_LOG_CONST;
#define CONFIG_UART2_0_SHELL_LOG            0
#define CONFIG_TI_DRIVERS_UART2_COUNT       1


/*
 *  ======== Watchdog ========
 */

extern const uint_least8_t                  CONFIG_WATCHDOG_0_CONST;
#define CONFIG_WATCHDOG_0                   0
#define CONFIG_TI_DRIVERS_WATCHDOG_COUNT    1


/*
 *  ======== GPTimer ========
 */

extern const uint_least8_t                  CONFIG_GPTIMER_PWM_CONST;
#define CONFIG_GPTIMER_PWM                  0
extern const uint_least8_t                  CONFIG_GPTIMER_CAPTURE_CONST;
#define CONFIG_GPTIMER_CAPTURE              1
#define CONFIG_TI_DRIVERS_GPTIMER_COUNT     2


/*
 *  ======== Board_init ========
 *  Perform all required TI-Drivers initialization
 *
 *  This function should be called once at a point before any use of
 *  TI-Drivers.
 */
extern void Board_init(void);

/*
 *  ======== Board_initGeneral ========
 *  (deprecated)
 *
 *  Board_initGeneral() is defined purely for backward compatibility.
 *
 *  All new code should use Board_init() to do any required TI-Drivers
 *  initialization _and_ use <Driver>_init() for only where specific drivers
 *  are explicitly referenced by the application.  <Driver>_init() functions
 *  are idempotent.
 */
#define Board_initGeneral Board_init

#ifdef __cplusplus
}
#endif

#endif /* include guard */
