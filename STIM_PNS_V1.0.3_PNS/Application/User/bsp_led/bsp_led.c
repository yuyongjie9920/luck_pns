#include <bsp_led/bsp_led.h>
#include "main_user.h"

// 如果没有特别说明，以下时间相关的单位为ms
enum led_step {
    EM_LED_STEP_NONE = 0,
    EM_LED_STEP_ON,
    EM_LED_STEP_ON_WAIT,
    EM_LED_STEP_OFF,
    EM_LED_STEP_OFF_WAIT,
    EM_LED_STEP_IDLE,

    EM_LED_STEP_MAX
};
typedef struct {
    uint16_t  on_time_ms;	///< 每个周期亮多长时间
    uint16_t  off_time_ms;	///< 每个周期灭多长时间
    uint16_t  onoff_count;	///< 每个周期执行多少次亮灭

    uint16_t  idle_time_ms;	///< 每个周期空闲时间
    uint16_t  cycle_count;  //共执行多少个周期。0xFFFF-forever, 0-stop
} led_param_s;

typedef struct {
    uint8_t  ledx;			///< 灯的编号

    enum led_mode mode;  //current mode.
    enum led_step step;		///< 当前处于什么状态

    led_param_s led_param;	///< 灯的参数
    
    uint16_t tick_ms;		///< 当前状态共执行多长时间
    uint32_t start_tick;	///< 开始运行的时间戳
} led_info_s;

static led_info_s led_info[LED_MAX_NUM];
static led_usr_init_s *pfunc = NULL;

static const led_param_s c_led_mode_param[EM_LED_MODE_MAX] =
{
    //(time unit:ms)
    [EM_LED_MODE_OFF] = { 
        .on_time_ms         = 0,
        .off_time_ms        = FOREVER_TIME,
        .onoff_count        = FOREVER_TIME,
        .idle_time_ms        = FOREVER_TIME,
        .cycle_count        = FOREVER_TIME
    },
    [EM_LED_MODE_ON] = { 
        .on_time_ms         = FOREVER_TIME,
        .off_time_ms        = 0,
        .onoff_count        = FOREVER_TIME,
        .idle_time_ms       = FOREVER_TIME,
        .cycle_count        = FOREVER_TIME
    },

    [EM_LED_MODE_SYS_IND] = { 
        .on_time_ms         = 1000,
        .off_time_ms        = 1000,
        .onoff_count        = FOREVER_TIME,
        .idle_time_ms       = 0,
        .cycle_count        = FOREVER_TIME
    },

    [EM_LED_MODE_FAST] = { 
        .on_time_ms         = 100,
        .off_time_ms        = 100,
        .onoff_count        = 0,
        .idle_time_ms       = 0,
        .cycle_count        = FOREVER_TIME
    },

    [EM_LED_MODE_SLOW] = { 
        .on_time_ms         = 500,
        .off_time_ms        = 500,
        .onoff_count        = 0,
        .idle_time_ms       = 500,
        .cycle_count        = FOREVER_TIME
    },

    [EM_LED_MODE_2_FLASH] = { 
        .on_time_ms         = 200,
        .off_time_ms        = 200,
        .onoff_count        = 2,
        .idle_time_ms       = 1000,
        .cycle_count        = FOREVER_TIME
    },

};


int led_init(led_usr_init_s *led_usr_init)
{
    led_info_s *pled = led_info;

    pfunc = led_usr_init;

    if(!pfunc->led_init_hw || !pfunc->led_on || !pfunc->led_off ||
        !pfunc->led_toggle || !pfunc->sys_get_tick_1ms)
    {
        return -1;
    }

    for(uint8_t i = 0; i < LED_MAX_NUM; i++)
	{
        pled[i].ledx = i;
        pfunc->led_init_hw(pled[i].ledx);
        pfunc->led_off(pled[i].ledx);
        pled[i].step = EM_LED_STEP_NONE;
        pled[i].mode = EM_LED_MODE_OFF;
    }

    return 0;
}

void led_set_mode(uint8_t ledx, enum led_mode mode)
{
    if(mode > EM_LED_MODE_MAX)
	{
        return;
    }

    led_info_s *pled = led_info;

    for(uint8_t i = 0; i < LED_MAX_NUM; i++)
	{
        if(pled[i].ledx == ledx)
		{

            if(pled[i].mode == mode)
			{
                break;
            }
        
            pled[i].led_param = c_led_mode_param[mode];

            pled[i].mode = mode;
            
            if(pled[i].led_param.on_time_ms == 0)
			{
                pled[i].step = EM_LED_STEP_OFF;
            }
			else
			{
				pled[i].step = EM_LED_STEP_ON;
			}

            pled[i].start_tick = pfunc->sys_get_tick_1ms();
            pled[i].tick_ms = 0;

            break;
        }
    }
}
void led_loop_check(void)
{
    led_info_s *pled = led_info;

    for(uint8_t i = 0; i < LED_MAX_NUM; i++)
	{
        if(pled[i].step >= EM_LED_STEP_MAX || pled[i].mode >= EM_LED_MODE_MAX)
		{
            continue;
        }
        
        if(pfunc->sys_get_tick_1ms() - pled[i].start_tick > pled[i].tick_ms)
		{
            pled[i].start_tick = pfunc->sys_get_tick_1ms();
            
            switch(pled[i].step)
			{
	            case EM_LED_STEP_ON:
				{
	                pfunc->led_on(pled[i].ledx);
	                pled[i].step = EM_LED_STEP_ON_WAIT;
	                pled[i].tick_ms = pled[i].led_param.on_time_ms;

					break;
	            }
	            case EM_LED_STEP_ON_WAIT:
				{
	                if(pled[i].led_param.on_time_ms != FOREVER_TIME)
					{
	                    if(pled[i].led_param.off_time_ms == 0)
						{
	                        pled[i].step = EM_LED_STEP_ON;
	                    }
						else
						{
							pled[i].step = EM_LED_STEP_OFF;
						}

	                    pled[i].tick_ms = 0; //next step is to off now.
	                }
					else
					{
						pled[i].tick_ms = FOREVER_TIME;
					}

	                break;
	            }
	            case EM_LED_STEP_OFF:
				{
	                pfunc->led_off(pled[i].ledx);
	                pled[i].step = EM_LED_STEP_OFF_WAIT;
	                pled[i].tick_ms = pled[i].led_param.off_time_ms;

					break;
	            }
	            case EM_LED_STEP_OFF_WAIT:
				{
	                if(pled[i].led_param.off_time_ms != FOREVER_TIME)
					{
	                    if(pled[i].led_param.onoff_count == FOREVER_TIME)
						{

	                        if(pled[i].led_param.on_time_ms == 0)
							{
	                            pled[i].step = EM_LED_STEP_OFF;
	                        }
							else
							{
								pled[i].step = EM_LED_STEP_ON;
							}

	                        pled[i].tick_ms = 0;
	                    }
						else if (pled[i].led_param.onoff_count <= 1)
						{
	                        pled[i].step = EM_LED_STEP_IDLE;
	                        pled[i].tick_ms = pled[i].led_param.idle_time_ms;
	                        pled[i].led_param.onoff_count = 0;
	                    }
						else
						{
	                        pled[i].led_param.onoff_count--;

	                        if (pled[i].led_param.on_time_ms == 0)
							{
	                            pled[i].step = EM_LED_STEP_OFF;
	                        }
							else
							{
								pled[i].step = EM_LED_STEP_ON;
							}

	                        pled[i].tick_ms = 0;
	                    }
	                }
					else
					{
						pled[i].tick_ms = FOREVER_TIME;
					}

	                break;
	            }
	            case EM_LED_STEP_IDLE:
				{
	                if (pled[i].led_param.cycle_count == FOREVER_TIME)
					{
	                    pled[i].step = EM_LED_STEP_ON;
	                    pled[i].tick_ms = 0;

	                    pled[i].led_param.onoff_count = c_led_mode_param[pled[i].mode].onoff_count;
	                }
					else if (pled[i].led_param.cycle_count <= 1)
					{
	                    led_set_mode(pled[i].ledx, EM_LED_MODE_OFF);
	                }
					else
					{
	                    pled[i].led_param.cycle_count--;
	                    pled[i].step = EM_LED_STEP_ON;
	                    pled[i].tick_ms = 0;
	                    pled[i].led_param.onoff_count = c_led_mode_param[pled[i].mode].onoff_count;
	                }

					break;
	            }
	            default:
	            {
	                pled[i].step = EM_LED_STEP_NONE;

	            	break;
				}
            }
        }
    }
}

