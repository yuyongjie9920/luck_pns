#ifndef __LED_H
#define __LED_H
#include "main_user.h"

#ifndef LED_MAX_NUM
#define LED_MAX_NUM   3		///< 指定共多少LED灯
#endif

enum led_colour {
    LED_GREEN = 0,
    LED_ORANGE,
    LED_BLUE
};

#define FOREVER_TIME  0xFFFF

typedef int (*led_func)(uint16_t led_id);

enum led_mode
{
    EM_LED_MODE_OFF = 0,
    EM_LED_MODE_ON = 1,
    EM_LED_MODE_SYS_IND = 2,
    EM_LED_MODE_FAST = 3,
    EM_LED_MODE_SLOW = 4,
    EM_LED_MODE_2_FLASH = 5,
    EM_LED_MODE_MAX
};

typedef struct
{
    led_func led_init_hw;
    led_func led_on;
    led_func led_off;
    led_func led_toggle;

    uint32_t (*sys_get_tick_1ms)(void);	///< 实现1ms的周期调用时基的当前时间（单位：ms）
} led_usr_init_s;

/**
 * @brief 初始化led驱动.
 * 			需要调用者实现4个底层驱动接口
 * @param led_usr_init  led的底层驱动结构体指针。需调用者分配结构体变量
 *
 * @retval 成功返回0；失败返回非0
 * @author
 * @date
 * @note
 */
int led_init(led_usr_init_s *led_usr_init);

/**
 * @brief 设置led的显示模式.
 * @param ledx  led的编号
 * @param mode led的显示模式
 *
 * @retval no
 * @author
 * @date
 * @note
 */
void led_set_mode(uint8_t ledx, enum led_mode mode);

/**
 * @brief 实现led功能的显示逻辑，需要周期性调用.
 * @param none
 *
 * @retval no
 * @author
 * @date
 * @note
 */
void led_loop_check(void);

#endif /* __LED_H */

