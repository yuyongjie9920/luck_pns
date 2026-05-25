#ifndef __APP_IIC_H_
#define __APP_IIC_H_


int app_i2c_init(void);
int app_i2c_write(uint16_t addr, uint8_t *reg, uint8_t reglen, uint8_t *data,  uint16_t datalen);

int app_i2c_read(uint16_t addr, uint8_t *reg, uint8_t reglen, uint8_t *data,  uint16_t datalen);


#endif //__APP_RTC_H_
