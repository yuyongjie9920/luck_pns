//经皮刺激器业务逻辑代码

#include <app_verify_ble.h>
#include "app_user.h"
#include "main_user.h"
#include "bsp_led.h"
#include "max17055_battery.h"
#include "CC2652_led.h"
#include "key_activity.h"

#include <elog.h>                 // 必须在 LOG_TAG 与 LOG_LVL 下面
#include <easyflash.h>
#include <elog_flash.h>
#include "app_flash_log.h"
#include "flash_interface_ext_rtos_NVS.h"
#include <ti/common/cc26xx/flash_interface/flash_interface.h>
#include "app_flash_log.h"


#include "leadinno_app.h"

static Mailbox_Handle user_mailbox_struct = NULL;
static Clock_Struct user_handle_timer; //刺激

app_user_data_t user;


//开机控制
void power_on(void)  //需要添加串口烧录 设备信息的接口
{
    size_t saved_size;
    user.power_status = POWER_ON;
    GPIO_write(CONFIG_GPIO_POWER_3V3_EN, 1);  //开机hold 3.3v
    led_set_mode(LED_GREEN, EM_LED_MODE_ON); //蓝牙连接过程中闪烁
    delay_ms(10);

}

//关机控制
void power_off(void)
{

    led_set_mode(LED_GREEN, EM_LED_MODE_OFF); //蓝牙连接过程中闪烁
    led_set_mode(LED_BLUE, EM_LED_MODE_OFF); // LED熄灭
//    save_user_blob("pluse_info", &user.pluse, sizeof(test_stim_out_pluse));

    GPIO_write(CONFIG_GPIO_POWER_3V3_EN, 0);  //关机
    user.power_status = POWER_OFF;
}

///获取电池电量百分比进行显示状态
void get_bat_percentage(void)
{

}

//处理按键加
void increase_parameter(void)
{

}

//处理按键减
void decrease_parameter(void)
{

}

int test_stim(uint16_t stim_on, uint32_t pulse, uint32_t fre, uint32_t amp, uint8_t PWM_percent, uint8_t power)
{
    int err;
    void pa_pwm(uint32_t frequencyHz, char dutyCycle);



    return err;
}

int check_reset(void)
{
    int err;
//    lead.reg.reg = 0x0a;
//    err = leadinno_crtl(LEAD_READ_REG);
//    if(err == 0) {
//        logInfo(" ########   lead.reg.data = %d", lead.reg.data);
//        if(lead.reg.data == 0) {
//            err = -1;
//        }
//    }
    return err;
}

// 对应的消息处理函数示例
void app_user_main(app_user_message_t  *data) //处理整个系统的消息
{
    int err;
    app_user_message_t *msg = data;
    int pwm_puty, power;
    switch(msg->type) {
        case MSG_SYS_RESET:
            logInfo("MSG_SYS_RESET");  //恢复出厂设置
            break;

        case MSG_KEY_INCREASE:
            logInfo("Key Increase");  // 按键加
            increase_parameter();
            break;

        case MSG_KEY_DECREASE:
            logInfo("Key Decrease");  // 按键减
            decrease_parameter();
            break;

        case MSG_POWER_ON:
            delay_ms(1000);
            elog_i("POWER", "PowerOn!");  // 用户开机
            power_on();
            break;

        case MSG_POWER_OFF:
            elog_i("POWER", "PowerCut!");  // 用户开机
            delay_ms(200);
            power_off();
            break;

        case MSG_BLUETOOTH_CONNECTED:
            logInfo("Bluetooth Connected");  // 蓝牙连接
            led_set_mode(LED_BLUE, EM_LED_MODE_2_FLASH); //蓝牙连接过程中闪烁
            break;

        case MSG_BLUETOOTH_CONNECTED_SUCCEED:
            logInfo("Bluetooth Connected");  // // 蓝牙连接成功
            led_set_mode(LED_BLUE, EM_LED_MODE_ON); //蓝牙连接成功常亮
            break;

        case MSG_BLUETOOTH_DISCONNECTED:
            logInfo("Bluetooth Disconnected");  // 蓝牙断开
            led_set_mode(LED_BLUE, EM_LED_MODE_OFF); // LED熄灭
            break;

        case MSG_POWER_MANAGEMENT:
            get_bat_percentage();
            break;

        case MSG_POWER_LOW:
            logInfo("MSG_POWER_LOW");
            break;

        case MSG_POWER_CHARGING:
            logInfo("MSG_POWER_CHARGING"); // 充电状态
            break;

        case MSG_START_STIM:
            logInfo("MSG_START_STIM"); // 开始刺激
            break;

        case MSG_STOP_STIM:
            logInfo("MSG_STOP_STIM"); //停止刺激
            user.stim_stata = 0;
            leadinno_crtl(LEAD_STOP_PULSE);
            break;

        case MSG_CHECK_STIM:
            logInfo("MSG_POWER_CHARGING"); //检查刺激状态

            break;

        default:
            logWarning("Unknown message type: %d", msg->type);  // 未知消息类型
            break;
    }
}


int send_mailbox_to_app_user(stim_message_type_t msg_type, uint8_t *data, uint16_t len)
{
    static app_user_message_t msg_data = {0};
    msg_data.type = msg_type;
    msg_data.data_len = len;



    // 如果有数据需要传递，复制数据
    if (data != NULL && len > 0) {
        memcpy(&msg_data.data, data, len);
    }
    // 发送消息
    bool post_result = Mailbox_post(user_mailbox_struct, &msg_data, BIOS_NO_WAIT);

    if (post_result != true) {
        logInfo("Mailbox post failed");
    }
    return 0;
}

//获取充电状态
int bat_is_full(void)
{
    if(GPIO_read(CONFIG_GPIO_C_STAT) == 0) {
        return 0;
    } else {
        return 1;
    }
}

int usb_power_in(void)
{
    if(GPIO_read(CONFIG_GPIO_CH_SENSE) == 1) {
        return 1;
    } else {
        return 0;
    }
}

int get_bat_cap(void)
{
    return max17055_dev.cache.capacity;
}

extern uint8_t app_stim_spi_gpio_mode_set(uint8_t is_spi);

void save_user_data(void)
{

}

uint8_t get_vrecp_falg = 0;
void app_user_task(UArg arg0, UArg arg1)
{
    static app_user_message_t rx_msg_data = {0};
    static uint8_t reset_flag = 1;
    u8 log_show_time;
    u8 power_log_show_time;
    Mailbox_Params mailboxParams;
    Mailbox_Params_init(&mailboxParams);
    user_mailbox_struct = Mailbox_create(sizeof(app_user_message_t), 8, &mailboxParams, NULL);
    int err = 0;

    max17055_battery_init(); //初始化电量检测
    key_activity_init(); //按键初始化
    app_led_init();      //led初始化

    delay_ms(500);
    user.stim_stata = STIM_OFF;
    while(1) {
        if(Mailbox_pend(user_mailbox_struct, &rx_msg_data, 100000) != true){

            if(usb_power_in()) { //充电中
                if(get_bat_cap() < 90) {
                    led_set_mode(LED_GREEN, EM_LED_MODE_SLOW);
                } else {
                    led_set_mode(LED_GREEN, EM_LED_MODE_ON);
                }
            } else { //没有充电
                if(get_bat_cap() >= 10) {
                    led_set_mode(LED_GREEN, EM_LED_MODE_ON);
                } else {
                    led_set_mode(LED_GREEN, EM_LED_MODE_2_FLASH);
                }
            }

            if(user.stim_stata == STIM_ON){
                send_mailbox_to_app_user(MSG_CHECK_STIM, NULL, 0);
            }
            continue;
        }
        app_user_main(&rx_msg_data);
    }
}
