#include "CC2652_key.h"
#include "multi_button.h"
#include "key_activity.h"
#include "main_user.h"


// rtos timer
static Clock_Struct keyChangeClock;
static key_callback_t key_user_callback = NULL;

void key_event_handler(Button* handle)
{
    if (key_user_callback == NULL) return;
    key_id_t key_id = get_button_id(handle);
    if (key_id >= KEY_ID_MAX) return;

    PressEvent event = get_button_event(handle);
    switch (event) {
        case PRESS_DOWN:
            key_user_callback(key_id, PRESS_DOWN);
            break;
        case PRESS_UP:
            key_user_callback(key_id, PRESS_UP);
            break;
        case SINGLE_CLICK:
            key_user_callback(key_id, SINGLE_CLICK);
            break;
        case DOUBLE_CLICK:
            key_user_callback(key_id, DOUBLE_CLICK);
            break;
        case LONG_RRESS_START:
            key_user_callback(key_id, LONG_RRESS_START);
            break;
        case LONG_PRESS_HOLD:
            key_user_callback(key_id, LONG_PRESS_HOLD);
            break;
        default:
            break;
    }
}

void key_interrupt_cb(u8 index)
{  //调试功耗再来调这里  设置按键中断 再开定时器
//    logInfo("key_interrupt_cb");
}

static uint8_t key_read_level(uint32_t port, uint32_t pin)
{
    return GPIO_read(pin);
}

void KeyTimerCb(UArg a0)
{
    timer_button_callback();
//    Util_startClock(&keyChangeClock);
}

void key_driver_init(void)
{
    uint32_t pin[KEY_COUNT];
    uint8_t active_level[KEY_COUNT];
    uint8_t port[KEY_COUNT] = {0}; //未使用

    //配置按键映射关系
    pin[0] = CONFIG_GPIO_KEY_MF_EN;
    pin[1] = CONFIG_GPIO_KEY_DOWN;
    pin[2] = CONFIG_GPIO_KEY_UP;
    pin[3] = CONFIG_GPIO_KEY_LOCK;

    active_level[0] = 1; //活动电平
    active_level[1] = 0;
    active_level[2] = 0;
    active_level[3] = 0;

//    GPIO_setConfig(pin[0], GPIO_CFG_IN_PD | GPIO_CFG_IN_INT_FALLING);  //下拉高电平活动
//    GPIO_setConfig(pin[1], GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
//    GPIO_setConfig(pin[2], GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);
//    GPIO_setConfig(pin[2], GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING);

    GPIO_setConfig(pin[0], GPIO_CFG_IN_PD);  //下拉高电平活动
    GPIO_setConfig(pin[1], GPIO_CFG_IN_PU);
    GPIO_setConfig(pin[2], GPIO_CFG_IN_PU);
    GPIO_setConfig(pin[3], GPIO_CFG_IN_PU);

    for(u8 i=0; i<KEY_COUNT; i++) {
//        GPIO_setCallback(pin[i], key_interrupt_cb);
        GPIO_enableInt(pin[i]);
    }

    //启动定时器
    Util_constructClock(&keyChangeClock, KeyTimerCb, 0, 30, true, 0);

    // 初始化每个按键
    for (key_id_t i = 0; i < KEY_COUNT; i++) {
        button_init(&button[i], key_read_level, active_level[i], port[i], pin[i]);
        // 注册事件回调
        button_attach(&button[i], PRESS_DOWN,      (BtnCallback)key_event_handler);
//        button_attach(&button[i], PRESS_UP,        (BtnCallback)key_event_handler);
//        button_attach(&button[i], SINGLE_CLICK,    (BtnCallback)key_event_handler);
//        button_attach(&button[i], DOUBLE_CLICK,    (BtnCallback)key_event_handler);
        button_attach(&button[i], LONG_RRESS_START,(BtnCallback)key_event_handler);
//        button_attach(&button[i], LONG_PRESS_HOLD, (BtnCallback)key_event_handler);
        // 启动按键检测
        button_start(&button[i]);
    }
}

void key_interrupt_handler(key_id_t key_id)
{
    if (key_id >= KEY_ID_MAX) return;

}

void key_register_callback(key_callback_t callback)
{
    if(callback != NULL) {
        key_user_callback = callback;
    }
}
