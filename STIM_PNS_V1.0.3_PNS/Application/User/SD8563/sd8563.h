#ifndef __SD8563_H__
#define __SD8563_H__
#include "main_user.h"
typedef int (*iic_func)(uint16_t addr, \
						uint8_t *reg, uint8_t reglen,\
						uint8_t *data,  uint16_t datalen);

typedef struct {
	iic_func p_write;
	iic_func p_read;
} iic_init_s;

typedef enum
{
	I2C_OK = 0,     			
	I2C_START_FAIL,								
	I2C_NO_ACK					
}i2c_exit_code_e;

typedef enum{
	SD8563_EXIT_OK = 0,
	SD8563_EXIT_NO_ACK = -1,
	SD8563_ERR = -2,
}sd8563_exit_code_e;

typedef enum{
	CENTURY_20 = 0,
	CENTURY_19 = 1,
}century_type_e;
	
typedef struct{
    unsigned char year;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char second;
    unsigned char week;
	century_type_e century_type;
}sd8563_time_t;

typedef enum{
	MINUTE_ALARM_ENABLE = 0,  
	MINUTE_ALARM_DISABLE,
	HOUR_ALARM_ENABLE,
	HOUR_ALARM_DISABLE,
	DAY_ALARM_ENABLE,
	DAY_ALARM_DISABLE,
	WEEK_ALARM_ENABLE,
	WEEK_ALARM_DISABLE,
	ALL_ALARM_DISABLE
}alaem_enable_e;  

typedef struct{
	unsigned char minute_a;
	unsigned char hour_a;
	unsigned char day_a;
	unsigned char week_a;	
	alaem_enable_e state; 
}sd8563_time_alarm_t;

typedef enum{
	CTR2_IM = 0,
	CTR2_INTAF,
	CTR2_INTDF,
	CTR2_INTAE,
	CTR2_INTDE
}ctr2_flag_e; 

typedef enum{
	SD8563_DISABLE = 0,
	SD8563_ENABLE = 1,
}flag_enable_e; 

typedef enum{
	OSF_L = 0,
	OSF_H = 1,
}sd8563_osf_flag_e; 

typedef enum{
	CLK_OUT_32KHZ  = 0x00,
	CLK_OUT_1024HZ = 0x01,
	CLK_OUT_32HZ   = 0x02,
	CLK_OUT_1HZ    = 0x03,
}clk_out_e;

typedef enum{
	COUNT_DOWN_4096HZ = 0x00,
	COUNT_DOWN_64HZ   = 0x01,
	COUNT_DOWN_1HZ    = 0x02,
	COUNT_DOWN_1_60HZ = 0x03,
}count_down_e;

sd8563_exit_code_e sd8563_read_reg(unsigned char reg_addr, unsigned char *read_buf, unsigned char length);
sd8563_exit_code_e sd8563_write_reg(unsigned char reg_addr, unsigned char *write_buf, unsigned char length);

sd8563_exit_code_e sd8563_read_time(sd8563_time_t *time);
sd8563_exit_code_e sd8563_write_time(sd8563_time_t *time);

sd8563_exit_code_e sd8563_get_ctr2_flag(ctr2_flag_e flag, flag_enable_e *enable);
sd8563_exit_code_e sd8563_set_ctr2_flag(ctr2_flag_e flag, flag_enable_e enable);

sd8563_exit_code_e sd8563_get_osf_flag(sd8563_osf_flag_e *osf_flag);
sd8563_exit_code_e sd8563_clear_osf_flag(void);

sd8563_exit_code_e sd8563_set_clk_out(clk_out_e freq, flag_enable_e enable);
sd8563_exit_code_e sd8563_set_count_down_vaule(unsigned char vaule);
sd8563_exit_code_e sd8563_get_count_down_vaule(unsigned char *vaule);
sd8563_exit_code_e sd8563_set_count_down(count_down_e freq, flag_enable_e enable);

int sd8563_init(iic_init_s iic_init);
#endif
