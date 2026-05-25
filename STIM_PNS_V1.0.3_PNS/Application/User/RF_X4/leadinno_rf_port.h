#ifndef __LEADINNO_RF_PORT_H__
#define __LEADINNO_RF_PORT_H__

#include "main_user.h"

#define OPEN_MCU_INT(a)  HwiP_restore(a)   //打开mcu中断
#define CLOSE_MCU_INT()  HwiP_disable()    //关闭mcu中断

typedef struct
{
    uint32_t    numBits;  //需要发送的数据位数
    uint8_t     *data;    //发送的数据指针
} rf_rx_tx_t;

void rf_tx_data(uint8_t *data, uint16_t bit_len);
int rf_rx_data(uint8_t *get_data, uint8_t *get_data_len);

#endif
