#include <sd8563.h>


#define SD8563_DEV_ADDR 			 0x51 //0xA2   //sd8563 addr

#define SD8563_REG_CTR1 			 0x00
#define SD8563_REG_CTR2 			 0x01

#define SD8563_REG_CLKOUT	    	 0x0D
#define SD8563_REG_TIMER_CTR		 0x0E
#define SD8563_REG_COUNT_DOWN  		 0x0F

#define SD8563_REG_SEC 		    	 0x02
#define SD8563_REG_MIN				 0x03
#define SD8563_REG_HOUR			     0x04
#define SD8563_REG_DAY				 0x05
#define SD8563_REG_WEEK			     0x06
#define SD8563_REG_MONTH		     0x07
#define SD8563_REG_YEAR			     0x08

#define SD8563_REG_ALARM_MIN         0x09
#define SD8563_REG_ALARM_HOUR        0x0A
#define SD8563_REG_ALARM_DAY	     0x0B
#define SD8563_REG_ALARM_WEEK        0x0C

#define BIT(x) (1 << x)
/*CTR1*/
#define SD8563_TEST_MODE      		BIT(7)
#define SD8563_PROTECT         		BIT(6)
#define SD8563_CLK_STOP			    BIT(5)
#define SD8563_TESTC           		BIT(3)
/*CTR2*/
#define SD8563_IM					BIT(4)
#define SD8563_INTAF				BIT(3)
#define SD8563_INTDF				BIT(2)
#define SD8563_INTAE				BIT(1)
#define SD8563_INTDE				BIT(0)
/*CLKOUT*/
#define SD8563_FE					BIT(7)
/*COUNT DOWN*/
#define SD8563_DE					BIT(7)

#define SD8563_OSF				BIT(7)
#define SD8563_CENTURY				BIT(7)
#define SD8563_MIN_AL				BIT(7)
#define SD8563_HOUR_AL				BIT(7)
#define SD8563_DAY_AL				BIT(7)
#define SD8563_WEEK_AL				BIT(7)


static iic_init_s s_iic_init_info = { 0 };
/**
 * @brief read time hex to bcd
 */
static unsigned char bcd_hex_convert(unsigned char bcd)
{
	return (bcd >> 4) * 10 + (bcd & 0x0f);
}

/**
 * @brief write time bcd to hex
 */
static unsigned char hex_bcd_convert(unsigned char hex)
{
	return ((hex / 10) << 4) | (hex % 10);
}


/**
 * @brief sd8563_read_reg
 */
sd8563_exit_code_e sd8563_read_reg(unsigned char reg_addr, unsigned char *read_buf, unsigned char length)
{
	if (!s_iic_init_info.p_read || !s_iic_init_info.p_write) {
		return SD8563_ERR;
	}
	if(s_iic_init_info.p_read(SD8563_DEV_ADDR, &reg_addr, 1, read_buf, length) != I2C_OK)
		return SD8563_EXIT_NO_ACK;
	return SD8563_EXIT_OK;
}

/**
 * @brief sd8563_write_reg
 */
sd8563_exit_code_e sd8563_write_reg(unsigned char reg_addr, unsigned char *write_buf, unsigned char length)
{  
	if (!s_iic_init_info.p_read || !s_iic_init_info.p_write) {
		return SD8563_ERR;
	}
	
	if(s_iic_init_info.p_write(SD8563_DEV_ADDR, &reg_addr, 1, write_buf, length) != I2C_OK)
	{
		return SD8563_EXIT_NO_ACK;
	}

	return SD8563_EXIT_OK;
}

/**
 * @brief write protect enable()
 */
sd8563_exit_code_e sd8563_write_enable()
{
	unsigned char buf;
	
	if(sd8563_read_reg(SD8563_REG_TIMER_CTR, &buf, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	buf &= ~0x7C; 
	if(sd8563_write_reg(SD8563_REG_TIMER_CTR, &buf, 1) != SD8563_EXIT_OK) 
	{
		return SD8563_ERR;
	}
	buf |= 0x70;	
	if(sd8563_write_reg(SD8563_REG_TIMER_CTR, &buf, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	buf &= ~0x7C;
	buf |=  0x0C; 
	if(sd8563_write_reg(SD8563_REG_TIMER_CTR, &buf, 1) != SD8563_EXIT_OK) 
	{
		return SD8563_ERR;
	}
	buf &= ~0x7C;
	buf |=  0x38;  	 
	if(sd8563_write_reg(SD8563_REG_TIMER_CTR, &buf, 1) != SD8563_EXIT_OK) 
	{
		return SD8563_ERR;
	}	
	return SD8563_EXIT_OK;
}

/**
 * @brief write protect disable
 */
sd8563_exit_code_e sd8563_write_disable()
{
	unsigned char buf;
	
	if(sd8563_read_reg(SD8563_REG_TIMER_CTR, &buf, 1) != SD8563_EXIT_OK)  
	{
		return SD8563_ERR;
	}
	buf &= ~0x7C; 
	if(sd8563_write_reg(SD8563_REG_TIMER_CTR, &buf, 1) != SD8563_EXIT_OK) 
	{
		return SD8563_ERR;
	}
	buf |= 0x54;	
	if(sd8563_write_reg(SD8563_REG_TIMER_CTR, &buf, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	buf &= ~0x7C;
	buf |=  0x28;  
	if(sd8563_write_reg(SD8563_REG_TIMER_CTR, &buf, 1) != SD8563_EXIT_OK) 
	{
		return SD8563_ERR;
	}
	buf &= ~0x7C;
	buf |=  0x5C;   
	if(sd8563_write_reg(SD8563_REG_TIMER_CTR, &buf, 1) != SD8563_EXIT_OK) 
	{
		return SD8563_ERR;
	}		
	return SD8563_EXIT_OK;
}


/**
 * @brief sd8563 read time
 */
sd8563_exit_code_e sd8563_read_time(sd8563_time_t *time)
{
	unsigned char buf[7];
	
	if(sd8563_read_reg(SD8563_REG_SEC, buf, 7) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	time->second = bcd_hex_convert(buf[0] & 0x7f);
	time->minute = bcd_hex_convert(buf[1] & 0x7f);
	time->hour   = bcd_hex_convert(buf[2] & 0x3f);
	time->day    = bcd_hex_convert(buf[3] & 0x3f);
	time->week   = bcd_hex_convert(buf[4] & 0x07);
	time->month  = bcd_hex_convert(buf[5] & 0x1f);
	if(buf[5] & 0x80)   
	{
		time->century_type =CENTURY_19;
	}
	else
	{
		time->century_type = CENTURY_20;
	}
	time->year   = bcd_hex_convert(buf[6] & 0xff);
	
	return SD8563_EXIT_OK;
}

/**
 * @brief sd8563 write time
 */
sd8563_exit_code_e sd8563_write_time(sd8563_time_t *time)
{
	unsigned char buf[7];
	
	buf[0] = hex_bcd_convert(time->second);
	buf[1] = hex_bcd_convert(time->minute);
	buf[2] = hex_bcd_convert(time->hour  );
	buf[3] = hex_bcd_convert(time->day   );
	buf[4] = hex_bcd_convert(time->week  );			
	buf[5] = hex_bcd_convert(time->month ); 
	if(time->century_type == CENTURY_19)     
	{
		buf[5] |=  SD8563_CENTURY;
	}
	else
	{
		buf[5] &=  ~SD8563_CENTURY;
	}		
	buf[6] = hex_bcd_convert(time->year);
	
	if(sd8563_write_reg(SD8563_REG_SEC, buf, 7) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	return SD8563_EXIT_OK;
}

/**
 * @brief sd8563 get ctr2 flag state
 */
sd8563_exit_code_e sd8563_get_ctr2_flag(ctr2_flag_e flag, flag_enable_e *enable)
{	
	unsigned char buf;
	
	if(sd8563_read_reg(SD8563_REG_CTR2, &buf, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	
	switch(flag)
	{
		case CTR2_IM:
			buf &= 0x10;
		break;
		case CTR2_INTAF:
			buf &= 0x08;
		break;
		case CTR2_INTDF:
			buf &= 0x04;
		break;
		case CTR2_INTAE:
			buf &= 0x02;
		break;
		case CTR2_INTDE:
			buf &= 0x01;
		break;
		default: 
			return SD8563_ERR;		
	}
	if(buf)
		*enable = SD8563_ENABLE;
	else
		*enable = SD8563_DISABLE;
	return SD8563_EXIT_OK;
}

/**
 * @brief sd8563 set ctr2 flag
 */
sd8563_exit_code_e sd8563_set_ctr2_flag(ctr2_flag_e flag, flag_enable_e enable)
{
	unsigned char buf;

	if(sd8563_read_reg(SD8563_REG_CTR2, &buf, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	
	if(enable == SD8563_ENABLE)
	{
		switch(flag)
		{
			case CTR2_IM:
				buf |= 0x10;
			break;
			case CTR2_INTAF:
				buf |= 0x08;
			break;
			case CTR2_INTDF:
				buf |= 0x04;
			break;
			case CTR2_INTAE:
				buf |= 0x02;
			break;
			case CTR2_INTDE:
				buf |= 0x01;
			break;
			default: 
				return SD8563_ERR;		
		}
	}
	else
	{
		switch(flag)
		{
			case CTR2_IM:
				buf &= ~0x10;
			break;
			case CTR2_INTAF:
				buf &= ~0x08;
			break;
			case CTR2_INTDF:
				buf &= ~0x04;
			break;
			case CTR2_INTAE:
				buf &= ~0x02;
			break;
			case CTR2_INTDE:
				buf &= ~0x01;
			break;
			default: 
				return SD8563_ERR;		
		}
	}
	
	if(sd8563_write_reg(SD8563_REG_CTR2, &buf, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	
	return SD8563_EXIT_OK;
}

/**
 * @brief sd8563 get osf bit
 */
sd8563_exit_code_e sd8563_get_osf_flag(sd8563_osf_flag_e *osf_flag)
{
	unsigned char buf;
	
	if(sd8563_read_reg(SD8563_REG_SEC, &buf, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	
	if(buf & SD8563_OSF)
	{
		*osf_flag = OSF_H;
	}
	else
	{
		*osf_flag = OSF_L;
	}
	
	return SD8563_EXIT_OK;
}

/**
 * @brief sd8563 clear osf bit
 */
sd8563_exit_code_e sd8563_clear_osf_flag(void)
{
	unsigned char buf;
	
	if(sd8563_read_reg(SD8563_REG_SEC, &buf, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	
	buf &= ~SD8563_OSF;
	
	if(sd8563_write_reg(SD8563_REG_SEC, &buf, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	
	return SD8563_EXIT_OK;
}

/**
 * @brief sd8563 set clkout
 */
sd8563_exit_code_e sd8563_set_clk_out(clk_out_e freq, flag_enable_e enable)
{
	unsigned char buf;
	
	switch(freq)
	{
		case CLK_OUT_32KHZ:
			buf &= 0xfc;
			buf |= CLK_OUT_32KHZ;
		break;
		case CLK_OUT_1024HZ:
			buf &= 0xfc;
			buf |= CLK_OUT_1024HZ;
		break;
		case CLK_OUT_32HZ:
			buf &= 0xfc;
			buf |= CLK_OUT_32HZ;
		break;
		case CLK_OUT_1HZ:
			buf &= 0xfc;
			buf |= CLK_OUT_1HZ;
		break;
		default: 
			return SD8563_ERR;			
	}
	
	if(enable == SD8563_ENABLE)
	{
		buf |= SD8563_FE;
	}
	else
	{
		buf &= ~SD8563_FE;
	}
	
	if(sd8563_write_reg(SD8563_REG_CLKOUT, &buf, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	
	return SD8563_EXIT_OK;
}

/**
 * @brief sd8563 set count dowm vaule
 */
sd8563_exit_code_e sd8563_set_count_down_vaule(unsigned char vaule)
{
	if(vaule < 0 || vaule > 0xff)
	{
		return SD8563_ERR;
	}
	if(sd8563_write_reg(SD8563_REG_COUNT_DOWN, &vaule, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	return SD8563_EXIT_OK;
}

/**
 * @brief sd8563 get count dowm vaule
 */
sd8563_exit_code_e sd8563_get_count_down_vaule(unsigned char *vaule)
{
	unsigned char buf;
	
	if(sd8563_read_reg(SD8563_REG_COUNT_DOWN, &buf, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	*vaule = buf;
	return SD8563_EXIT_OK;
}

/**
 * @brief sd8563 set count dowm
 */
sd8563_exit_code_e sd8563_set_count_down(count_down_e freq, flag_enable_e enable)
{
	unsigned char buf;
	
	switch(freq)
	{
		case COUNT_DOWN_4096HZ:
			buf &= 0xfc;  
			buf |= COUNT_DOWN_4096HZ;
		break;
		case COUNT_DOWN_64HZ:
			buf &= 0xfc;
			buf |= COUNT_DOWN_64HZ;
		break;
		case COUNT_DOWN_1HZ:
			buf &= 0xfc;
			buf |= COUNT_DOWN_1HZ;
		break;
		case COUNT_DOWN_1_60HZ:
			buf &= 0xfc;
			buf |= COUNT_DOWN_1_60HZ;
		break;
		default: 
			return SD8563_ERR;			
	}
	
	if(enable == SD8563_ENABLE)
	{
		buf |= SD8563_DE;
	}
	else
	{
		buf &= ~SD8563_DE;
	}
	
	if(sd8563_write_reg(SD8563_REG_TIMER_CTR, &buf, 1) != SD8563_EXIT_OK)
	{
		return SD8563_ERR;
	}
	
	return SD8563_EXIT_OK;
}

/**
  * @brief sd8563搴曞眰绔彛鍒濆鍖�
  * @param void
  *
  * @retval 
  *
  * @author Taoking
  * @date 
  * @note Remark_What_this_function_might_en
	counter
  */
int sd8563_init(iic_init_s iic_init)
{
	if (!iic_init.p_read || !iic_init.p_write) {
		return -1;
	}
	s_iic_init_info = iic_init;
	return 0;
}

