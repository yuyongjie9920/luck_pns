#include "main_user.h"
#include "CC2652_led.h"


const static uint8_t led_gpio[EM_LED_MAX] = {
    [EM_LED_ID_STIM] = CONFIG_GPIO_RGB_G, //CONFIG_GPIO_LED_G_CONST
    [EM_LED_ID_BAT] = CONFIG_GPIO_RGB_O, //CONFIG_GPIO_LED_G_CONST
    [EM_LED_ID_BLE] = CONFIG_GPIO_RGB_B, //CONFIG_GPIO_LED_G_CONST
};

static led_usr_init_s led_init_s;

static Clock_Struct led_timer;

static int app_led_init_hw(uint16_t led_id)
{
    if (led_id >= LED_MAX_NUM || led_id >= EM_LED_MAX) {
        logError("error: led_id. %d %d %d.", led_id, LED_MAX_NUM, EM_LED_MAX);
        return -1;
    }
    return 0;
}

static int app_led_on(uint16_t led_id)
{
    if (led_id >= LED_MAX_NUM || led_id >= EM_LED_MAX) {
        logError("error: led_id. %d %d %d.", led_id, LED_MAX_NUM, EM_LED_MAX);
        return -1;
    }


    GPIO_write(led_gpio[led_id], 0);
    return 0;
}

static int app_led_off(uint16_t led_id)
{
    if (led_id >= LED_MAX_NUM || led_id >= EM_LED_MAX) {
        logError("error: led_id. %d %d %d.", led_id, LED_MAX_NUM, EM_LED_MAX);
        return -1;
    }

    GPIO_write(led_gpio[led_id], 1);
    return 0;
}

static int app_led_toggle(uint16_t led_id)
{
    if (led_id >= LED_MAX_NUM || led_id >= EM_LED_MAX) {
        logError("error: led_id. %d %d %d.", led_id, LED_MAX_NUM, EM_LED_MAX);
        return -1;
    }

    GPIO_toggle(led_gpio[led_id]);
    return 0;
}

void LedTimerCb(UArg a0)
{
    led_loop_check();
}

void app_led_init(void)
{
    led_init_s.led_init_hw = app_led_init_hw;
    led_init_s.led_on      = app_led_on;
    led_init_s.led_off     = app_led_off;
    led_init_s.led_toggle  = app_led_toggle;
    led_init_s.sys_get_tick_1ms = app_get_systick_1ms;
    led_init(&led_init_s);

    //启动定时器
    Util_constructClock(&led_timer, LedTimerCb, 0, 100, true, 0); //100ms循环
}


