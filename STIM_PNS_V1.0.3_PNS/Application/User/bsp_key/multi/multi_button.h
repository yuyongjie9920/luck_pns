/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */
 
#ifndef _MULTI_BUTTON_H_
#define _MULTI_BUTTON_H_

#include "stdint.h"
#include "string.h"


#define BUTTON_READ_IO_FUNC_PARAM_TYPE  1  //0-no param, 1-gd32-port,pin

#define KEY_MAX     4

//According to your need to modify the constants.
#define TICKS_INTERVAL    30//10//5	//ms
#define DEBOUNCE_TICKS    1//2//3	//MAX 8
#define SHORT_TICKS       (300 /TICKS_INTERVAL)
#define LONG_TICKS        (1000 /TICKS_INTERVAL)


typedef void (*BtnCallback)(void*);

typedef enum {
	PRESS_DOWN = 0,
	PRESS_UP,
	PRESS_REPEAT,
	SINGLE_CLICK,
	DOUBLE_CLICK,
	LONG_RRESS_START,
	LONG_PRESS_HOLD,
	number_of_event,
	NONE_PRESS
}PressEvent;

typedef struct Button {
	uint16_t ticks;
	uint8_t  repeat : 4;
	uint8_t  event : 4;
	uint8_t  state : 3;
	uint8_t  debounce_cnt : 3; 
	uint8_t  active_level : 1;
	uint8_t  button_level : 1;
    
#if (BUTTON_READ_IO_FUNC_PARAM_TYPE == 0)
    uint8_t  (*hal_button_Level)(void);
#elif (BUTTON_READ_IO_FUNC_PARAM_TYPE == 1)
    uint32_t port;
    uint32_t pin;
    uint8_t  (*hal_button_Level)(uint32_t, uint32_t);
#endif
	
	BtnCallback  cb[number_of_event];
	struct Button* next;
}Button;


extern struct Button button[KEY_MAX];
extern struct Button* head_handle;


#ifdef __cplusplus  
extern "C" {  
#endif  

#if (BUTTON_READ_IO_FUNC_PARAM_TYPE == 0)
void button_init(struct Button* handle, uint8_t(*pin_level)(), uint8_t active_level);
#elif (BUTTON_READ_IO_FUNC_PARAM_TYPE == 1)
void button_init(struct Button* handle, uint8_t(*pin_level)(uint32_t, uint32_t), uint8_t active_level, uint32_t port, uint32_t pin);
#endif

void button_attach(struct Button* handle, PressEvent event, BtnCallback cb);
PressEvent get_button_event(struct Button* handle);
int  button_start(struct Button* handle);
void button_stop(struct Button* handle);
void button_ticks(void);

uint8_t get_button_id(Button *pkey);
void timer_button_callback(void);
#ifdef __cplusplus
} 
#endif

#endif
