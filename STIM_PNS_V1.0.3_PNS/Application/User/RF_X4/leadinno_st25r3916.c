//该文件实现了驱动st25r3916 RF射频芯片   透传模式 pwm
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#include "ti_drivers_config.h"
#include "drivers/GPIO.h"
#include "drivers/SPI.h"

#include "main_user.h"
#include "leadinno_st25r3916.h"

#include "leadinno_app.h"

#include <unistd.h>
#include <stddef.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTimerCC26XX.h>
#include "ti_drivers_config.h"


st25r3916_data_t st3916;

// 调试日志开关宏
// 1: 启用调试日志（与logdebug功能相同）
//····· 0: 禁用调试日志（无任何输出）
#if 1
    #define log(...) logInfo(__VA_ARGS__)
#else
    #define log(...) do {} while(0)  // 空操作，避免语法错误
#endif


static SPI_Handle  spi = NULL;

//rf芯片电源开关设置
void rf_power_state(rf_power_e power)
{
    GPIO_write(CONFIG_GPIO_EN_5V, power);
}

//开打spi接口
int spi_open(void)  //spi 接口收发数据 还没有优化 目前 u8  u8  的发送的
{
    //open spi
    static SPI_Params  spiParams;

    SPI_Params_init(&spiParams);

    spiParams.transferMode = SPI_MODE_BLOCKING;
    spiParams.bitRate = 1000000;
    spiParams.dataSize = 8;
    spiParams.frameFormat = SPI_POL0_PHA1;
    spi = SPI_open(CONFIG_SPI_STIM_CONST, &spiParams);

    if (spi == NULL) {
        logInfo("open spi error!");
        while(spi == NULL) {
            SPI_close(spi);
            SPI_Params_init(&spiParams);
            spiParams.transferMode = SPI_MODE_BLOCKING;
            spiParams.bitRate = 1000000;
            spiParams.dataSize = 8;
            spiParams.frameFormat = SPI_POL0_PHA1;
            spi = SPI_open(CONFIG_SPI_STIM_CONST, &spiParams);
            logError("open spi error!");
        }
        // Error opening SPI, return.
        return 1;
    }
    return 0;
}

//spi接口收发
bool spi_txrx(uint8_t *txdata, uint8_t *rxdata, uint32_t size)
{

    uint8_t transferOK = 0;
    SPI_Transaction spiTransaction;

    spiTransaction.count = size;
    spiTransaction.txBuf = txdata;
    spiTransaction.rxBuf = rxdata;

    transferOK = SPI_transfer(spi, &spiTransaction);
    if (!transferOK) { //返回1 表示 收发正常
        log("spi_txrx fail");
        // Error in SPI or transfer already in progress.
    }
    return !transferOK; //取反0 正常
}

//spi发送接口
bool spi_tx(uint8_t txdata)
{
    return spi_txrx(&txdata, NULL, 1);
}
//sp接收接口
bool spi_rx(uint8_t *get_data)
{
    return spi_txrx(get_data, get_data, 1);
}

//SPI 接口 cs电平设置
void st25r3916_spi_cs(cs_value_e select)
{
    GPIO_write(CONFIG_GPIO_STIM_CS_CONST, select);
}
//中断接口
void st25r3916_isr(uint8_t indx)
{
//    log("st25r3916_isr");
}

//设置寄存器值
static bool w_reg(uint8_t reg, uint8_t reg_data, rf_reg_mode_e  mode)
{
    bool err = 0;
    st25r3916_spi_cs(CS_ENABLE);
    if(mode == REG_A || mode == REG_B) {
        if(mode == REG_B) {
            err |= spi_tx(ST25R3916_CMD_SPACE_B_ACCESS);
        }
        err |= spi_tx(reg | WROTE_MODE);
        err |= spi_tx(reg_data);
    } else if(mode == REG_CMD) {
        err |= spi_tx(reg | CMD_MODE);
    }
    st25r3916_spi_cs(CS_DISABLE);
    return err;
}
//读寄存器值
static bool r_reg(uint8_t reg, uint8_t *get_reg_data, rf_reg_mode_e  mode)
{
    bool err = 0;
    st25r3916_spi_cs(CS_ENABLE);
    if(mode == REG_B) {
        err |= spi_tx(ST25R3916_CMD_TEST_ACCESS);
    }
    err |= spi_tx(reg | READ_MODE);
    err |= spi_rx(get_reg_data);
    st25r3916_spi_cs(CS_DISABLE);
    return err;
}

//开关启动透传后开关rf能量
void st25r3916_open_rf(rf_power_e key)
{
    static bool open_flag = CLOSE;
    if(key == OPEN) {
        if(open_flag == OPEN) {
            return;
        }
        open_flag = OPEN;
        APP_IO_WRITE(CONFIG_GPIO_SPI_STIM_PICO_CONST, 0);
        delay_ms(10); //过滤掉射频开启的瞬间 波形不稳定
        logInfo("open rf");
    } else if (key == CLOSE) {
        if(open_flag == CLOSE) {
            return;
        }
        open_flag = CLOSE;
        rf_pwm_close();
        APP_IO_WRITE(CONFIG_GPIO_SPI_STIM_PICO_CONST, 1);
        logInfo("close rf");
    }
}


//设置发射功率
void st25r3916_set_rf_power_level(uint8_t level)
{
    st3916.tx_power = level;
    if(level > 15) {
        level = 15;
    }
    st25r3916_w_reg(ST25R3916_REG_A_TX_DRIVER, 0xf0|st3916.tx_power, REG_A); //功率配置4
}

//spi接口模式选择 可以选择透传模式 和 spi模式
bool st25r3916_gpio_spi_mode_set(spi_gpio_mode_e mode)
{
    bool err = 0;
    if(mode == TRANSMISSION_MODE) { //透传模式为 gpio翻转->场强开关
        if(st3916.mode == TRANSMISSION_MODE) {
            return 0;
        }
        err |= w_reg(ST25R3916_REG_A_AUX, ST25R3916_REG_AUX_dis_corr, REG_A); // dis_corr RFU = 1;
        err |= w_reg(ST25R3916_REG_A_OP_CONTROL, ST25R3916_EN_OP_CTL, REG_A);
        err |= w_reg(ST25R3916_CMD_TRANSPARENT_MODE, 0, REG_CMD); //直接命令启动透传
        SPI_close(spi);
        spi = NULL;
        st3916.mode = TRANSMISSION_MODE;
    } else if(mode == SPI_MODE) {
        if(st3916.mode == SPI_MODE) {
            return 0;
        }
        if(spi == NULL) {
            spi_open();
        }
        err |= w_reg(ST25R3916_REG_A_AUX, 0, REG_A); // dis_corr RFU = 1;
        err |= w_reg(ST25R3916_REG_A_OP_CONTROL, ST25R3916_CLOSE_OP_CTL, REG_A);
        st3916.mode = SPI_MODE;
    }
    return err;
}

bool st25r3916_w_reg(uint8_t reg, uint8_t reg_data, rf_reg_mode_e  mode)
{
    if(st3916.mode == TRANSMISSION_MODE) {
        st25r3916_gpio_spi_mode_set(SPI_MODE);
    }

    int err = w_reg(reg, reg_data, mode);

    st25r3916_gpio_spi_mode_set(TRANSMISSION_MODE);

    return err;
}


//读寄存器值
bool st25r3916_r_reg(uint8_t reg, uint8_t *get_reg_data, rf_reg_mode_e  mode)
{
    if(st3916.mode == TRANSMISSION_MODE) {
        st25r3916_gpio_spi_mode_set(SPI_MODE);
    }

    bool err = r_reg(reg, get_reg_data, mode);

    st25r3916_gpio_spi_mode_set(TRANSMISSION_MODE);
    return err;
}

//获取3916 id
void st25r3916_get_id(void)
{
    uint8_t chip_id = 0;
    if(st25r3916_r_reg(ST25R3916_REG_A_IC_IDENTITY, &chip_id, REG_A) == RF_ERR_NONE) {
        logInfo("read st25r3916b chip:0x%02x.", chip_id);
    }
    if(chip_id != 0x31) {
        logInfo("st25r3916b error.");
    }
}

//配置3916是否进入低功耗
void st25r3916_sleep(rf_sleep_e sleep)
{

}


void test_w_3916_reg(uint8_t reg, uint8_t data, uint8_t page)
{
    if(page == 0) {
        st25r3916_w_reg(reg, data, REG_A);
    } if(page == 1) {
        st25r3916_w_reg(reg, data, REG_B);
    } else {
        st25r3916_w_reg(reg, data, REG_CMD);
    }
    logInfo("test_w_3916_reg ok w reg = 0x%x data = 0x%x", reg, data);

}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 w3916, test_w_3916_reg,
                 "reg data page");
//初始化配置
void st25r3916_init(void)
{
    rf_power_state(OPEN);  //使能RF电源
    SPI_init();            //初始化SPI接口
    spi_open();

    GPIO_setCallback(CONFIG_GPIO_SPI_IRQ_CONST, st25r3916_isr); //注册GPIO中断
    GPIO_enableInt(CONFIG_GPIO_SPI_IRQ_CONST); //配置为正常输入 无法正常开关机  配置为输入下拉正常开关机

    st25r3916_w_reg(ST25R3916_CMD_SET_DEFAULT, 0, REG_CMD);

    st25r3916_w_reg(ST25R3916_REG_A_IO_CONF2, 0x1c, REG_A);
    st25r3916_w_reg(ST25R3916_REG_A_OP_CONTROL, 0x80, REG_A);
    st25r3916_get_id();

//    st25r3916_w_reg(ST25R3916_REG_B_P2P_RX_CONF, 0x76, REG_B); //0bh ook 0 1阈值判断

//    st25r3916_w_reg(0xd, 0x28);//接灵敏度
//    st25r3916_w_reg(0xd, 0x08);//接灵敏度  某些位置远的时候可以
//    st25r3916_w_reg(ST25R3916_REG_B_CORR_CONF2, 0xd8, REG_A); //ook 0 1阈值判断

    st25r3916_w_reg(ST25R3916_REG_A_REGULATOR_CONTROL, 0xff, REG_A);  //0x2c
    st25r3916_w_reg(ST25R3916_REG_A_RX_CONF1, 0x03, REG_A); // 0x0b 接收数据滤波
    st25r3916_w_reg(ST25R3916_REG_A_RX_CONF3, 0xb8, REG_A); // 0x0d未知
    st25r3916_w_reg(ST25R3916_REG_A_RX_CONF2, 0xff, REG_A); // 0x0c

    st3916.tx_power = 0; //0-15
    st25r3916_w_reg(ST25R3916_REG_A_TX_DRIVER, 0xf0|st3916.tx_power, REG_A); //功率配置4
}


//pwm 控制 用于在透传模式下输出 占空比供能能量
PWM_Handle pwm = NULL;
static PWMTimerCC26XX_Object pwmObject;
static const PWMTimerCC26XX_HwAttrs pwmAttrs = {
  .pwmPin = CONFIG_GPIO_SPI_STIM_PICO,
  .gpTimerUnit = CONFIG_GPTIMER_PWM,
};
const PWM_Config PWM_config[1]= {  //必须定义 spi和pwm io共用gpio 需要手动定义  库里面定为弱函数
    {
        .fxnTablePtr = &PWMTimerCC26XX_fxnTable,  // 使用Timer模式的PWM函数表
        .object = &pwmObject,                     // 指向硬件对象实例
        .hwAttrs = &pwmAttrs                      // 指向刚刚定义的硬件属性
    }
};
const uint_least8_t PWM_count = 1;                //必须定义 spi和pwm io共用gpio 需要手动定义库里面定为弱函数


void rf_pwm_open(void)
{
    PWM_Params params;
    PWM_Params_init(&params);
    params.dutyUnits = PWM_DUTY_FRACTION;
    params.periodUnits = PWM_PERIOD_HZ;
    pwm = PWM_open(0, &params);
    if (pwm == NULL) {
        // 这里可以添加串口打印
        logInfo("open pwm fial");
    }
}


void rf_pwm_close(void)
{
    if(pwm) {
        PWM_close(pwm);
        pwm = NULL;
        st3916.pwm_open_flag = PWM_FLAG_CLOSE;
    }
}

void rf_pwm_chang(uint8_t duty_percent, uint16_t hz)
{
    uint32_t duty_data = (PWM_DUTY_FRACTION_MAX / 100) * (100-duty_percent);
    st25r3916_gpio_spi_mode_set(TRANSMISSION_MODE);
    if(pwm == NULL) {
        PWM_init();
        rf_pwm_open();
        logInfo("PWM_init rf_pwm_open");
    }

    PWM_setDutyAndPeriod(pwm, duty_data, hz);
//    logInfo("set_pwm duty = %d, hz = %d", duty_percent, hz);

    PWM_start(pwm);

    //保存用户pwm参数 用于后续通讯后自动恢复pwm
    st3916.pwm_open_flag = PWM_FLAG_OPEN;
    st3916.pwm.duty_percent = duty_percent;
    st3916.pwm.hz = hz;
}

void rf_pwm_init(void)
{
    PWM_init();
}

//复位芯片
void rf_reset_chip(void)
{
    st25r3916_open_rf(CLOSE);
    delay_ms(1500);
    st25r3916_open_rf(OPEN);
    delay_ms(200); //等芯片数字初始化OK
}

void app_stim_task(UArg arg0, UArg arg1)
{
    delay_ms(1000);
    logInfo("2025 8 26");
    st25r3916_init();
    delay_ms(10);

    rf_pwm_init();
    rf_pwm_open();

    int err;

    st25r3916_gpio_spi_mode_set(TRANSMISSION_MODE);
    st25r3916_open_rf(CLOSE);

    lead.stim_pulse_power = 0; //设置刺激供能功率最大
//    rf_pwm_chang(20, 500);
    while(1) {

//          leadinno_crtl(LEAD_GET_VRECP);
//          leadinno_crtl(LEAD_GET_TEMP);

          delay_ms(1000);
    }
}















