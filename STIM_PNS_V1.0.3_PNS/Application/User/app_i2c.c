#include <app_log.h>
#include "string.h"
#include <ti/drivers/I2C.h>
#include <ti/drivers/GPIO.h>
#include "ti_drivers_config.h"

#define BLE_DEBUG         1
#if BLE_DEBUG
#define logdebug(fmt, ...) logInfo(fmt, ##__VA_ARGS__)
#else
#define logdebug(fmt) ((void)0)
#endif

#define  IIC_MAX_BYTE        32  //length:reg+data

//----------------hw------------
static I2C_Handle i2cHandle;

#define IIC_OPEN_STATE  1
#define IIC_CLOSE_STATE 0

//初始化IIC
int app_i2c_init(void)
{
    static bool iic_open = IIC_CLOSE_STATE;
    if(!iic_open) {
        iic_open = IIC_OPEN_STATE;
        // initialize optional I2C bus parameters
        I2C_Params params;
        I2C_Params_init(&params);
        params.bitRate = I2C_100kHz;
        params.addressMode = I2C_ADDRESS_MODE_7_BIT;
        params.transferMode = I2C_MODE_BLOCKING;

        // Open I2C bus for usage
        i2cHandle = I2C_open(CONFIG_I2C_RTC, &params);
        if (i2cHandle == NULL) {
            return I2C_STATUS_ERROR;
        }
    }
    return I2C_STATUS_SUCCESS;
}

//IIC写数据 地址  寄存器 寄存器长度  数据 数据长度
int app_i2c_write(uint16_t addr, uint8_t *reg, uint8_t reglen, uint8_t *data,  uint16_t datalen)
{
    if(i2cHandle == NULL) {
        return I2C_STATUS_ERROR;
    }
    I2C_Transaction i2cTransaction = {0};

    if (reglen + datalen > IIC_MAX_BYTE) {
        logdebug("iic write len error");
        return I2C_STATUS_ERROR;
    }
    uint8_t temp[IIC_MAX_BYTE] = { 0 };

    if (reglen != 0) {
        memcpy(temp, reg, reglen);
    }
    if (datalen != 0) {
        memcpy(temp+reglen, data, datalen);
    }

    i2cTransaction.targetAddress = addr;
    i2cTransaction.writeBuf = temp;
    i2cTransaction.writeCount = reglen+datalen;
    i2cTransaction.readBuf = 0;
    i2cTransaction.readCount = 0;
    int status = I2C_transfer(i2cHandle, &i2cTransaction);
    if (status == false) {
        logdebug("iic write error, i2cTransaction.status = %d, 0x%x, 0x%x", i2cTransaction.status, addr, *reg);
        return i2cTransaction.status;
    }

    return I2C_STATUS_SUCCESS;
}

//IIC读数据guo 地址  寄存器 寄存器长度  数据 数据长度
int app_i2c_read(uint16_t addr, uint8_t *reg, uint8_t reglen, uint8_t *data,  uint16_t datalen)
{
    I2C_Transaction i2cTransaction = {0};

    i2cTransaction.targetAddress = addr;
    i2cTransaction.writeBuf = reg;
    i2cTransaction.writeCount = reglen;
    i2cTransaction.readBuf = data;
    i2cTransaction.readCount = datalen;
    int status = I2C_transfer(i2cHandle, &i2cTransaction);
    if (status == false) {
        if (i2cTransaction.status == I2C_STATUS_ADDR_NACK) {
            // I2C target address not acknowledged
        }
        logdebug("iic read error, i2cTransaction.status = %d, 0x%x, 0x%x", i2cTransaction.status, addr, *reg);
        return i2cTransaction.status;
    }

    return I2C_STATUS_SUCCESS;
}


