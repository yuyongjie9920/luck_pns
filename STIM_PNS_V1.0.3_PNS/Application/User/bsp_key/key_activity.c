#include "key_activity.h"
#include "CC2652_key.h"
#include "multi_button.h"
#include "main_user.h"
#include "multi_button.h"
#include "CC2652_key.h"
#include "key_activity.h"
#include "util.h"


#if 0
    #define logdebug(...) logInfo(__VA_ARGS__)
#else
    #define logdebug(...) do {} while(0)  // 空操作，避免语法错误
#endif


static void key_event_callback(key_id_t key_id, PressEvent event)
{
    static u8 power_stata = 0;
    static u8 key_up_flag = 0;
    switch (event) {
        case PRESS_DOWN:
            if(key_id == KEY_ID_UP) {
//                logInfo("KEY_ID_UP");
                send_mailbox_to_app_user(MSG_KEY_INCREASE, NULL, 0); //发送消息到 app_user 处理逻辑  处理按键 加
            }
            if(key_id == KEY_ID_DOWN) {
                logInfo("KEY_ID_DOWN");
               send_mailbox_to_app_user(MSG_KEY_DECREASE, NULL, 0); //发送消息到 app_user 处理逻辑  处理按键 减
            }
            if(key_id == KEY_ID_POWER_EN) {
//                logInfo("KEY_ID_POWER_EN");
                  send_mailbox_to_app_user(MSG_STIM_START, NULL, 0); //发送消息到 app_user 处理逻辑
            }
            if(key_id == KEY_ID_CLOCK) {
                logInfo("KEY_ID_CLOCK");
            }

            break;
        case LONG_RRESS_START:
            if(key_id == KEY_ID_POWER_EN && key_up_flag == 0) {
                key_up_flag = 1;
                if(user.power_status == POWER_OFF) { //系统状态为关机  发送开机消息
                    send_mailbox_to_app_user(MSG_POWER_ON, NULL, 0); //发送消息到 app_user 处理逻辑
                }
                if(user.power_status == POWER_ON) {
                    send_mailbox_to_app_user(MSG_POWER_OFF, NULL, 0); //发送消息到 app_user 处理逻辑
                }
            }
        case PRESS_UP:
            if(key_id == KEY_ID_POWER_EN) {
                key_up_flag = 0;
            }
            break;
        default:
            break;
    }
}

void key_activity_init(void)
{
    // 初始化按键驱动
    key_driver_init();
    // 注册按键回调
    key_register_callback(key_event_callback);
}
