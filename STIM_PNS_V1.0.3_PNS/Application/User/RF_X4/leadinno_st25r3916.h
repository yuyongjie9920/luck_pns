#ifndef __LEADINNO_ST25R3916_H__
#define __LEADINNO_ST25R3916_H__

#define POWER_MAX         0

//spi接口的模式选择
typedef enum {
    TRANSMISSION_MODE,  //透传模式
    SPI_MODE,           //spi通讯模式
    TRANSMISSION_PWM_MODE,
}spi_gpio_mode_e;

typedef enum {
    CS_ENABLE = 0,
    CS_DISABLE
}cs_value_e;

typedef enum {
    CLOSE = 0,
    OPEN,
}rf_power_e;

typedef enum {
    POWER_DOWN,    //低功耗模式
    POWER_UP,      //唤醒
}rf_sleep_e;

typedef enum {
    REG_A,
    REG_B,
    REG_CMD
}rf_reg_mode_e;

typedef enum {
    WROTE_MODE = (0U << 6),
    READ_MODE  = (1U << 6),
    CMD_MODE   = (3U << 6)
}rf_mode_e;

typedef enum {
    PWM_FLAG_CLOSE,
    PWM_FLAG_OPEN
}rf_pwm_flag_e;

struct user_set_pwm {
    uint8_t duty_percent;
    uint16_t hz;
};

typedef struct {
    uint8_t tx_power;
    uint8_t mode;
    uint8_t pwm_open_flag;
    struct user_set_pwm pwm;
} st25r3916_data_t;
extern st25r3916_data_t st3916;

#define ST25R3916B


#define RF_ERR_NONE                                          0     /*!< 返回无错误 */

#include "st25r3916_reg.h"

//user define
#define ST25R3916_EN_OP_CTL                (ST25R3916_REG_OP_CONTROL_en |  \
                                            ST25R3916_REG_OP_CONTROL_tx_en | \
                                            ST25R3916_REG_OP_CONTROL_rx_en | \
                                            ST25R3916_REG_OP_CONTROL_rx_chn | \
                                            ST25R3916_REG_OP_CONTROL_rx_man)

#define ST25R3916_CLOSE_OP_CTL              (ST25R3916_REG_OP_CONTROL_en | ST25R3916_REG_OP_CONTROL_tx_en)


void st25r3916_set_rf_power_level(uint8_t level);
bool st25r3916_w_reg(uint8_t reg, uint8_t reg_data, rf_reg_mode_e  mode);
bool st25r3916_r_reg(uint8_t reg, uint8_t *get_reg_data, rf_reg_mode_e  mode);
void rf_pwm_chang(uint8_t duty_percent, uint16_t hz);
void st25r3916_open_rf(rf_power_e key);
void rf_pwm_close(void);
void rf_reset_chip(void);

#endif



