#ifndef CC2652_KEY_H
#define CC2652_KEY_H

#include <stdint.h>
#include <stdbool.h>
#include "multi_button.h"

// 按键数量定义
#define KEY_COUNT 4

// 按键ID定义
typedef enum {
    KEY_ID_POWER_EN = 0,
    KEY_ID_UP,
    KEY_ID_DOWN,
    KEY_ID_CLOCK,
    KEY_ID_MAX
} key_id_t;

// 按键回调函数类型
typedef void (*key_callback_t)(key_id_t key_id, PressEvent event);

// 初始化按键驱动
void key_driver_init(void);

// 注册按键回调函数
void key_register_callback(key_callback_t callback);

// 获取按键状态
bool key_get_state(key_id_t key_id);

// 按键中断处理函数
void key_interrupt_handler(key_id_t key_id);

#endif // CC2652_KEY_H
