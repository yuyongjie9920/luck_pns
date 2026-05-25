/**
 * @file x4.c
 * @brief X4芯片射频通信及控制实现
 *
 * 本文件实现了与X4芯片的射频通信、参数配置、刺激波形输出等功能。
 * 包含芯片初始化、寄存器读写、刺激参数配置、电压/温度/阻抗测量等。
 */

#include "leadinno_app.h"
#include "main_user.h"
#include "crc.h"
#include "x4.h"

uint8_t NO_RX_DATA;
/**
 * @def DEBUG_LOG_ENABLE
 * @brief 调试日志开关宏
 *
 * 控制调试日志输出：
 * - 1: 启用调试日志，调用logInfo输出
 * - 0: 禁用调试日志，无任何输出
 */
#if 1
    #define log(...) logInfo(__VA_ARGS__)
#else
    #define log(...) do {} while(0)  /* 空操作，避免语法错误 */
#endif


#define CHECK_ERR(err) if((err) != 0) return -1

/** X4芯片全局数据结构 */
struct x4_data x4;

/**
 * @def NONDATA_SYS_CLOCK
 * @brief 系统时钟转换系数 (0.85 * 100)
 *
 * 用于将时间参数转换为系统时钟周期数
 */
#define NONDATA_SYS_CLOCK 85

/**
 * @def SCALE_FACTOR
 * @brief 通用缩放因子
 */
#define SCALE_FACTOR 100


#define DAC_TO_UA_FACTOR    2439        /**< DAC值转uA系数 2.439 * 1000 */
#define UA_TO_DAC_FACTOR    410         /**< uA转DAC系数 1000/2.439 ≈ 410 */
#define US_CALIBRATION_NUMERATOR   2308  // 实际值
#define US_CALIBRATION_DENOMINATOR 1900  // 理论值


/**
 * @brief 带精度的数值缩放函数
 * @param x 原始值
 * @param scale 缩放系数
 * @param precision 精度分母
 * @return 缩放后的整数值
 * @note 使用四舍五入方式计算
 */
static inline int scale_value_with_precision(int x, int scale, int precision) {
    return (x * scale + precision/2) / precision;
}

/**
 * @brief 微秒转换为时钟周期
 */
uint32_t x4_us_to_clk(uint32_t us, uint8_t clk_div)
{
    uint32_t clk;
    // 应用校准系数
    clk = (uint32_t)((uint64_t)us * US_CALIBRATION_DENOMINATOR / US_CALIBRATION_NUMERATOR) + 1;
    return clk / (clk_div + 1);
}

uint32_t x4_clk_to_us(uint32_t clk, uint8_t clk_div)
{
    uint32_t clk_raw = clk * (clk_div + 1);
    return ((uint64_t)(clk_raw - 1) * US_CALIBRATION_NUMERATOR) / US_CALIBRATION_DENOMINATOR;
}


/** @def SCALE_FACTOR1
 *  @brief 定点数运算缩放因子(1000倍) */
#define SCALE_FACTOR1 1000

/** @def CONV_FACTOR
 *  @brief 电流转换系数 2.439 * 1000 */
#define CONV_FACTOR 2439

/**
 * @brief 旧ADC值转换为新电流值(mA)
 * @param adc_value ADC原始值
 * @return 转换后的电流值(mA)
 * @note 使用定点数运算，提高计算精度
 */
uint32_t to_sys_ma(uint32_t adc_value) {
    return (adc_value * CONV_FACTOR + SCALE_FACTOR1/2) / SCALE_FACTOR1;
}

/**
 * @brief 执行查询命令(内部函数)
 * @return 0-成功，非0-失败
 *
 * 发送查询命令并接收响应，获取芯片UID和复位标志
 */
int __x4_rf_send_query(void)
{
    uint8_t bit_len;
    uint16_t crc;
    uint8_t send_data[5] = {0x80,0,0,0,0};
    uint8_t raw_data[8] = {0};
    uint8_t uid = 0;  /* UID初始值 */
    int err;

    /* 构建查询命令帧 */
    send_data[0] |= (uid >> 12) & 0x0F;    /* UID高4位 -> bytes[0]低4位 */
    send_data[1] = (uid >> 4) & 0xFF;      /* UID中间8位 -> bytes[1] */
    send_data[2] = (uid & 0x0F) << 4;      /* UID低4位 -> bytes[2]高4位 */

    /* 计算CRC校验 */
    crc = crc16Bitwise(send_data, 20);

    /* 添加CRC到数据帧 */
    send_data[20/8] |= (crc >> 12) & 0x0F; /* CRC高4位到bytes[2]低4位 */
    send_data[20/8+1] = (crc >> 4) & 0xFF; /* CRC中间8位到bytes[3] */
    send_data[20/8+2] = (crc & 0x0F) << 4; /* CRC低4位到bytes[4]高4位 */

    /* 发送查询命令(36位: 8位命令 + 16位UID + 16位CRC) */
    rf_tx_data(send_data, 36);

    if(NO_RX_DATA == 1) { //盲发
        for(uint8_t i = 0; i < 3; i++) {
            SET_RF_POWER(i);
            rf_tx_data(send_data, 36);
            err = rf_rx_data(raw_data, &bit_len);
            if(err != 0) {
                log("err = %d",err);
            } else {
                /* 解析响应数据 */
                x4.filler.data.uid = ((raw_data[0] & 0x7f) << 8) | raw_data[1];
                x4.filler.data.rest_flag = raw_data[0]>>7;
                log("x4.filler.bits.uid = 0x%x, x4.filler.bits.rest = %d",
                    x4.filler.data.uid, x4.filler.data.rest_flag);
                log("query ok! tx opower is %d", i);
                lead.set_query_power = st3916.tx_power;
            }
            delay_ms(10);
        }
        SET_RF_POWER(0);
        delay_ms(10);
        logInfo("blind_sned_query ok");
        return 0;
    }

    /* 接收响应数据 */
    err = rf_rx_data(raw_data, &bit_len);

    if(err != 0) {
        log("err = %d",err);
    } else {
        /* 解析响应数据 */
        x4.filler.data.uid = ((raw_data[0] & 0x7f) << 8) | raw_data[1];
        x4.filler.data.rest_flag = raw_data[0]>>7;
        log("x4.filler.bits.uid = 0x%x, x4.filler.bits.rest = %d",
            x4.filler.data.uid, x4.filler.data.rest_flag);
        log("query ok! tx opower is %d", st3916.tx_power);
        lead.set_query_power = st3916.tx_power;
    }
    return err;
}

/**
 * @brief 发送写寄存器命令(内部函数)
 * @param reg 寄存器地址
 * @param reg_data 要写入的16位数据
 * @return 0-成功，-1-失败
 */
static int x4_rf_send_cmd_write(uint8_t reg, uint16_t reg_data)
{
    /* 恢复为查询功率 */
    SET_RF_POWER(lead.set_query_power);

    x4_rf_write_send_data_t write_data;
    x4_rf_write_dec_data_t tx;
    uint8_t rx_data[8];
    uint16_t crc;
    uint8_t nbits=0;
    uint16_t uid;
    int err ;

    /* 构建写命令帧 */
    write_data.cmd = 0xc3;
    write_data.reg = reg;
    write_data.reg_data = ((reg_data & 0xff00) >> 8) | ((reg_data & 0x00ff) << 8);
    uid = x4.filler.data.uid;
    uid = (uid<<8) | (uid >> 8);
    write_data.uid = uid;

    /* 计算CRC */
    crc = crc16Bitwise((uint8_t *)&write_data,
                       (sizeof(write_data.cmd) + sizeof(write_data.reg) +
                        sizeof(write_data.reg_data) + sizeof(write_data.uid)) * 8);
    write_data.crc_h = crc>>8;
    write_data.crc_l = crc;

    /* 发送写命令 */
    rf_tx_data((uint8_t *)&write_data, sizeof(write_data)*8);

    if(NO_RX_DATA == 1) { //盲发
        for(uint8_t i = 0; i < 8; i++) {
            SET_RF_POWER(i);
            rf_tx_data((uint8_t *)&write_data, sizeof(write_data)*8);
            /* 接收响应 */
            rf_rx_data(rx_data, &nbits);
            /* 解析响应中的CRC */
            tx.crc = ((rx_data[2] & 0x7F) << 9) | (rx_data[3] << 1) | ((rx_data[4] >> 7) & 0x01);

            /* 验证响应数据 */
            if((nbits/8) > 2) {
                crc = crc16Bitwise(rx_data, nbits-16);
                if(crc == tx.crc) {
                    log("write reg 0x%x->0x%04x ok", reg, reg_data);
                    err = 0;
                } else {
                    err = -1;
                }
            } else {
                err = -1;
            }
            delay_ms(10);
        }
        SET_RF_POWER(0);
        log("blind write reg 0x%x->0x%04x ok", reg, reg_data);
        return 0;
    }

    /* 接收响应 */
    err = rf_rx_data(rx_data, &nbits);

    /* 解析响应中的CRC */
    tx.crc = ((rx_data[2] & 0x7F) << 9) | (rx_data[3] << 1) | ((rx_data[4] >> 7) & 0x01);

    /* 验证响应数据 */
    if((nbits/8) > 2) {
        crc = crc16Bitwise(rx_data, nbits-16);
        if(crc == tx.crc) {
            log("write reg 0x%x->0x%04x ok", reg, reg_data);
            err = 0;
        } else {
            err = -1;
        }
    } else {
        err = -1;
    }

    if(err != 0) {
        log("write reg 0x%x->0x%x fail", reg, reg_data);
    }
    return err;
}

/**
 * @brief 发送读寄存器命令(内部函数)
 * @param reg 寄存器地址
 * @param get_reg_data 读取到的数据存储指针
 * @return 0-成功，-1-失败
 */
static int x4_rf_send_cmd_read(uint8_t reg, uint16_t *get_reg_data)
{
    x4_rf_read_send_data_t send_data;
    x4_rf_rx_dec_data_t rx;
    uint8_t rx_data[8];
    uint16_t crc;
    uint8_t nbits=0;

    /* 构建读命令帧 */
    send_data.cmd = 0xc2;
    send_data.reg = reg;
    uint16_t uid;

    uid = x4.filler.data.uid;
    uid = (uid<<8) | (uid >> 8);
    send_data.uid = uid;

    /* 计算CRC */
    crc = crc16Bitwise((uint8_t *)&send_data,
                       (sizeof(send_data.cmd) + sizeof(send_data.reg) +
                        sizeof(send_data.uid)) * 8);
    send_data.crc_h = crc>>8;
    send_data.crc_l = crc;

    /* 发送读命令 */
    rf_tx_data((uint8_t *)&send_data, sizeof(send_data)*8);

    if(NO_RX_DATA == 1) { //盲发
        log("read reg 0x%x<-0x%04x ok", reg, rx.reg_data);
        delay_ms(10);
        return 0;
    }

    /* 接收响应 */
    int err = rf_rx_data(rx_data, &nbits);

    /* 解析响应数据 */
    rx.header = (rx_data[0] >> 7) & 0x01;          /* 头部(1位) */
    rx.reg_data = ((rx_data[0] & 0x7F) << 9) |     /* 寄存器数据(16位) */
                  (rx_data[1] << 1) |
                  ((rx_data[2] >> 7) & 0x01);
    rx.rn = ((rx_data[2] & 0x7F) << 9) |           /* 随机数(16位) */
            (rx_data[3] << 1) |
            ((rx_data[4] >> 7) & 0x01);
    rx.crc = ((rx_data[4] & 0x7F) << 9) |           /* CRC(16位) */
             (rx_data[5] << 1) |
             ((rx_data[6] >> 7) & 0x01);

    /* 验证响应数据 */
    if((nbits/8) > 2) {
        crc = crc16Bitwise(rx_data, nbits-16);
        if(crc == rx.crc) {
            *get_reg_data = rx.reg_data;
            err = 0;
            log("read reg 0x%x<-0x%04x ok", reg, rx.reg_data);
        } else {
            err = -1;
            log("read reg 0x%x fail", reg);
        }
    } else {
        err = -1;
        log("read reg 0x%04x fail", reg);
    }

    /* 恢复为刺激功率 */
    SET_RF_POWER(lead.stim_pulse_power);
    return err;
}

/**
 * @brief 写寄存器(带重试机制)
 * @param reg 寄存器地址
 * @param reg_data 要写入的数据
 * @return 0-成功，1-失败(重试10次后)
 *
 * 写失败时自动重新查询并重试，最多重试10次
 */
int x4_cmd_write(uint8_t reg, uint16_t reg_data)
{
    int err = 0;
    uint8_t i = 10;
    do {
        err = x4_rf_send_cmd_write(reg, reg_data);
        if(err != 0) {
            x4_rf_send_query();
        }
    } while(err != 0 && (i--) > 0);

    return (i > 0) ? 0 : 1;
}

/**
 * @brief 读寄存器(带重试机制)
 * @param reg 寄存器地址
 * @param get_reg_data 读取数据存储指针
 * @return 0-成功，1-失败(重试10次后)
 */
int x4_cmd_read(uint8_t reg, uint16_t *get_reg_data)
{
    int err;
    uint8_t i = 10;
    do {
        err = x4_rf_send_cmd_read(reg, get_reg_data);
        if(err != 0) {
            x4_rf_send_query();
        }
    } while(err != 0 && (i--) > 0);

    return (i > 0) ? 0 : 1;
}

/**
 * @brief 轮询查询合适的发射功率
 * @return RF_OK-成功，RF_FIAL-失败
 *
 * 从功率0到15依次尝试查询，直到查询成功
 */
int x4_rf_send_query(void)
{
    int state = 0;
    for(uint8_t i = 0; i < 15; i++) {
        SET_RF_POWER(i);
        state = __x4_rf_send_query();
        if(state == 0) {
            return RF_OK;
        }
    }
    log("query power 0 - 15 all fail");
    return RF_FIAL;
}

/**
 * @brief 获取芯片整流电压(VRECP)
 * @param[out] vrecp 输出电压值指针
 *        - delay_count=0: 指向单个float值
 *        - delay_count>0: 指向包含[min, max]的数组
 * @param[in] delay_count 延时计数(0-255)
 *        - 0: 立即采样
 *        - 非0: 延时采样(需要输出波形才能采样到电压)
 * @return 0-成功，负数-错误码
 */
int x4_rf_get_leadinno_vrecp(float *vrecp, uint16_t delay_count)
{
    int err = 0;
    uint16_t vdd_low = 0, vdd_high = 0;
    lms0820x4_volt_delay_sample_t volt_delay_sample = {0};
    lms0820x4_ctrlreg0_t ctrlreg0 = {0};

    /* 配置电压延时采样寄存器(地址0x65) */
    volt_delay_sample.bits.sample_enable = (delay_count == 0) ?
                                           X4_VRECP_NEW_MODE : X4_VRECP_DELAY_MODE;
    volt_delay_sample.bits.sample_delay = (uint8_t)(delay_count & 0xFF);
    err |= x4_cmd_write(LMS0820X4_REG_65_VOLT_DELAY_SAMPLE, volt_delay_sample.value);
    CHECK_ERR(err);

    /* 短暂提高功率充电后恢复 */
    SET_RF_POWER(POWER_MAX);
    delay_ms(10);
    SET_RF_POWER(st3916.tx_power);

    /* 启动电压测量 */
    ctrlreg0.bits.start_en = X4_MEAS_ENABLE_VOLT;
    err |= x4_cmd_write(LMS0820X4_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);
    /* 读取测量结果 */
    err |= x4_cmd_read(LMS0820X4_REG_72_VDD_LOW, &vdd_low);
    CHECK_ERR(err);
    err |= x4_cmd_read(LMS0820X4_REG_73_VDD_HIGH, &vdd_high);
    CHECK_ERR(err);

    /* 清除测量使能 */
    ctrlreg0.bits.start_en = X4_MEAS_ENABLE_NONE;
    err |= x4_cmd_write(LMS0820X4_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);

    /* ADC值转换为电压 */
    const float ADC_OFFSET = 98.0f;
    const float ADC_SCALE = 122.0f;

    float vlt_min = 0.0f, vlt_max = 0.0f;
    uint16_t data_adc_low = vdd_low & 0x0FFF;
    uint16_t data_adc_high = vdd_high & 0x0FFF;

    if (data_adc_low > ADC_OFFSET) {
        vlt_min = (data_adc_low - ADC_OFFSET) / ADC_SCALE;
    }

    if (delay_count > 0 && data_adc_high > ADC_OFFSET) {
        vlt_max = (data_adc_high - ADC_OFFSET) / ADC_SCALE;
    }

    /* 四舍五入保留2位小数 */
    vlt_min = roundf(vlt_min * 100.0f) / 100.0f;
    vlt_max = roundf(vlt_max * 100.0f) / 100.0f;

    /* 返回结果 */
    if (delay_count > 0) {
        vrecp[0] = vlt_min;
        vrecp[1] = vlt_max;
    } else {
        *vrecp = vlt_min;
    }

    return err;
}

/**
 * @brief 获取芯片温度
 * @param[out] temp 温度值指针(单位：℃)
 * @return 0-成功，负数-错误码
 */
int x4_rf_get_leadinno_temp(float *temp)
{
    int err = 0;
    uint16_t temp_adc = 0;
    float temperature;
    lms0820x4_ctrlreg0_t ctrlreg0 = {0};

    /* 启动温度测量 */
    ctrlreg0.bits.start_en = X4_MEAS_ENABLE_TEMP;
    err |= x4_cmd_write(LMS0820X4_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);
    /* 等待测量完成 */
    delay_ms(10);

    /* 读取温度测量结果 */
    err |= x4_cmd_read(LMS0820X4_REG_74_TEMP_DATA, &temp_adc);
    CHECK_ERR(err);
    /* 清除测量使能 */
    ctrlreg0.bits.start_en = X4_MEAS_ENABLE_NONE;
    err |= x4_cmd_write(LMS0820X4_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);
    /* ADC值转换为温度 */
    temperature = 65.856f - (float)(temp_adc & 0x0FFF) / 75.188f;
    *temp = roundf(temperature * 10.0f) / 10.0f;  /* 保留1位小数 */

    return 0;
}

/**
 * @brief 获取芯片efuse值
 * @param[out] efuse efuse值指针(24位有效)
 * @return 0-成功，负数-错误码
 */
int x4_rf_get_leadinno_efuse(uint32_t *efuse)
{
    int err = 0;
    lms0820x4_efuse_r_data0_t rdata0 = {0};
    lms0820x4_efuse_r_data1_t rdata1 = {0};

    err |= x4_cmd_read(LMS0820X4_REG_26_EFUSE_R_DATA0, &rdata0.value);
    CHECK_ERR(err);
    err |= x4_cmd_read(LMS0820X4_REG_27_EFUSE_R_DATA1, &rdata1.value);
    CHECK_ERR(err);
    *efuse = rdata0.value | (rdata1.bits.efuse_r_data_high & 0x7f) << 16;

    return err;
}

/**
 * @brief 设置芯片efuse(芯片唯一标识符)
 * @param[in] efuse 要烧写的24位efuse值
 * @return 0-成功，负数-错误码
 * @note
 * - 需要较高的射频能量支持
 * - 烧写后芯片会自动复位，需要重新查询
 * - 烧写过程约3秒
 */
int x4_rf_set_leadinno_efuse(uint32_t efuse)
{
    int err = 0;
    uint32_t efuse_new = 0;

    /* 关闭PWM输出(如果已开启) */
    rf_pwm_close();

    /* 限制efuse为24位有效值 */
    efuse = efuse & 0xFFFFFF;

    /* 初始化寄存器结构体 */
    lms0820x4_efuse_w_data0_t wdata0 = {0};
    lms0820x4_efuse_w_data1_t wdata1 = {0};
    lms0820x4_efuse_tburn_t tburn = {0};

    /* 1. 清零efuse数据有效位 */
    wdata1.bits.efuse_data_valid = 0;
    err |= x4_cmd_write(LMS0820X4_REG_25_EFUSE_W_DATA1, wdata1.value);
    CHECK_ERR(err);
    /* 2. 配置烧写时间 */
    tburn.bits.efuse_burn_clk_en = 1;      /* 常开efuse时钟 */
    tburn.bits.efuse_tburn = 0x3FAC;       /* 烧写时间：0x3FAC us */
    err |= x4_cmd_write(LMS0820X4_REG_28_EFUSE_TBURN, tburn.value);
    CHECK_ERR(err);

    /* 3. 配置efuse低16位数据 */
    wdata0.value = efuse & 0xFFFF;
    err |= x4_cmd_write(LMS0820X4_REG_24_EFUSE_W_DATA0, wdata0.value);
    CHECK_ERR(err);

    /* 4. 配置efuse高8位并启动烧录 */
    wdata1.bits.efuse_data_valid = 1;       /* 开始烧录 */
    wdata1.bits.efuse_data_enable = 0x3F;   /* 烧录的bit位数 */
    wdata1.bits.efuse_w_data_high = (efuse >> 16) & 0xFF;  /* efuse最高8位 */
    err |= x4_cmd_write(LMS0820X4_REG_25_EFUSE_W_DATA1, wdata1.value);
    CHECK_ERR(err);

    /* 5. 设置最大射频功率 */
    SET_RF_POWER(POWER_MAX);

    /* 6. 等待efuse烧写完成 */
    log("Waiting for efuse write completion (3 seconds)...\n");
    delay_ms(3000);

    /* 7. 复位芯片使efuse生效 */
    rf_reset_chip();

    /* 8. 重新查询芯片 */
    err |= x4_rf_send_query();
    CHECK_ERR(err);
    /* 9. 读取烧写的efuse进行验证 */
    err |= x4_rf_get_leadinno_efuse(&efuse_new);
    CHECK_ERR(err);
    /* 10. 验证写入结果 */
    if (efuse_new == efuse) {
        log("EFUSE write successfully: 0x%06x\n", efuse_new);
    } else {
        log("EFUSE verification failed: write 0x%06x, read 0x%06x\n",
            efuse, efuse_new);
    }

    return err;
}

/**
 * @brief 获取电极阻抗
 * @param[in] channel_sel 参考电极通道(0-7)
 * @param[out] res 电阻值数组指针(8个通道)
 * @return 0-成功，负数-错误码
 */
int x4_rf_get_leadinno_res(uint8_t channel_sel, uint16_t *res)
{
    int err = 0;
    uint16_t res_raw[8] = {0};
    float f_mhz = 0.4916f;  /* 电阻计算系数 */

    lms0820x4_ctrlreg0_t ctrlreg0 = {0};
    lms0820x4_imp_data_t imp_data[8];

    /* 配置测量寄存器：启动阻抗测量，设置基准电极 */
    ctrlreg0.bits.start_en = X4_MEAS_ENABLE_IMP;
    ctrlreg0.bits.imp_channel_sel = channel_sel;  /* 设置阳极参考位置 */
    ctrlreg0.bits.imp_test_trim_osc = 0x08;       /* 设置时钟 */
    err |= x4_cmd_write(LMS0820X4_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);
    /* 等待阻抗测量完成 */
    delay_ms(10);

    /* 读取8个通道的阻抗数据 */
    for(uint8_t i = 0; i < 8; i++) {
        err |= x4_cmd_read(LMS0820X4_REG_6A_IMP_DATA1 + i, &imp_data[i].value);
        CHECK_ERR(err);
        res_raw[i] = imp_data[i].bits.imp_data;
    }

    /* 清空控制寄存器 */
    err |= x4_cmd_write(LMS0820X4_REG_68_CTRLREG0, 0);
    CHECK_ERR(err);
    /* 转换为实际电阻值 */
    for (int i = 1; i < 8; i++) {  /* 通道0存放电压基准值 */
//        res[i] = ((float)res_raw[i] - (float)imp_data[0].value - 127.45f) / f_mhz;
        res[(i+channel_sel)%8] = ((float)res_raw[i] - (float)imp_data[0].value - 127.45f) / f_mhz;
        log("R[%d-%d] = %dR", channel_sel, (i+channel_sel)%8, res[i]);
    }

    return err;
}

/**
 * @brief 获取刺激电流
 * @param[out] currentm 电流值数组指针(正负两路)
 * @param[in] delay_sample_mode 延时采样模式标志
 * @return 0-成功，负数-错误码
 * @note 刺激波形脉宽需大于25us，否则采样不准确
 */
int x4_rf_get_leadinno_i_stim(float *currentm, bool delay_sample_mode)
{
    int err = 0;
    lms0820x4_ctrlreg0_t ctrlreg0 = {0};

    /* 使能电流测量 */
    ctrlreg0.bits.current_measure = 1;
    err |= x4_cmd_write(LMS0820X4_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);
    lms0820x4_current_data_t current_data_neg;  /* 0x75 */
    lms0820x4_current_data_t current_data_plus; /* 0x76 */

    /* 延时等待采样 */
    delay_us(delay_sample_mode);

    /* 读取正负电流值 */
    err |= x4_cmd_read(LMS0820X4_REG_75_CURRENT_DATA_NEG, &current_data_neg.value);
    CHECK_ERR(err);
    err |= x4_cmd_read(LMS0820X4_REG_76_CURRENT_DATA_PLUS, &current_data_plus.value);
    CHECK_ERR(err);

    log("I1 = %d ua I2 = %d ua", current_data_neg.value, current_data_plus.value);

    /* 清空控制寄存器 */
    err |= x4_cmd_write(LMS0820X4_REG_68_CTRLREG0, 0);
    CHECK_ERR(err);
    return err;
}

/**
 * @brief 设置A组刺激波形参数(内部函数)
 * @param[in] wave_params 简单模式波形参数
 * @return 0-成功，非0-失败
 */
static int __x4_rf_set_simple_feature(x4_simple_params *wave_params)
{
    int err = 0;

    uint32_t tphase1, tphase2, tphase3, tinterphase1, tinterphase2, tinterpulses, total_clk;
    uint32_t tinterpulse_save;
    uint32_t burst_tinter_save;
    uint16_t burst_tinter;


    start:

    tphase1      = x4_clk_to_us(wave_params->pulse_forward, wave_params->stim_clk_div);//设置正向脉宽
    tphase2      = x4_clk_to_us(wave_params->pulse_backward, wave_params->stim_clk_div);//设置负向脉宽
    tphase3      = x4_clk_to_us(wave_params->time_phase3, wave_params->stim_clk_div);//电荷平衡时间
    tinterphase1 = x4_clk_to_us(wave_params->time_interphase1, wave_params->stim_clk_div);//设置正向脉宽后的延时
    tinterphase2 = x4_clk_to_us(wave_params->time_interphase2, wave_params->stim_clk_div);//设置负向脉宽后的延时
    total_clk    = x4_clk_to_us(1000000 / wave_params->base_freq, wave_params->stim_clk_div) - 1; //计算出总的clk数  - 1是芯片内部会自己换算


    if(wave_params->pulse_mode == X4_SIMPLE_MODE) { //简单模式  和多程序
        tinterpulse_save = total_clk - tphase1 - tphase2 - tinterphase1 - tinterphase2 - tphase3;
        if(tinterpulse_save >= 0xffff) {
            wave_params->stim_clk_div++;
            if(wave_params->stim_clk_div>63) {
                return 0;
            }
            logInfo("[error hz moro min] !!!!!!!!!");
            goto start;
        } else {
            tinterpulses = total_clk - tphase1 - tphase2 - tinterphase1 - tinterphase2 - tphase3;
        }
    } else if(wave_params->pulse_mode == X4_BURST_MODE) {
//        tinterpulse = lmp_us_to_clk(wave_params->time_interpulse, wave_params->stim_clk_div);//设置负向脉宽后的延时
//        burst_tinter_save = total_clk - (tphase1 + tphase2 + tphase3 + tinterphase1 + tinterphase2 + tinterpulse) * wave_params->inter_burst;
//        if(burst_tinter_save >= 0xffff) {
//            wave_params->stim_clk_div++;
//            if(wave_params->stim_clk_div>63) {
//                return 0;
//            }
//            logInfo("[error hz moro min] !!!!!!!!!");
//            goto start;
//        }

    }
    if(wave_params->pwm_sync_mode) {
        uint16_t start_count;
        //通过pwm_duty 推算出delay_cnt 让波形在能量最后
        uint32_t power_len;
        int move_len;
        uint32_t start_len;

        power_len = (1000*1000 / wave_params->base_freq) * ( wave_params->pwm_duty)/100; //能量时间长度us 322为delay_cnt = 0 的位置
        power_len = x4_us_to_clk(power_len, wave_params->stim_clk_div);

        if(tinterpulses > (power_len*3/4)) {
            if(wave_params->base_freq > 9) {
                tinterpulses =  tinterpulses - power_len*3/4;
            } else {
                tinterpulses = tinterpulses*0.5;
            }
        } else {
            if(wave_params->base_freq > 10000) {
                tinterpulses = 1;
            } else {
                tinterpulses = tinterpulses*0.5;
            }
        }

    start1:
        power_len = (1000*1000 / wave_params->base_freq) * ( wave_params->pwm_duty)/100; //能量时间长度us 322为delay_cnt = 0 的位置
        power_len = x4_us_to_clk(power_len, wave_params->stim_clk_div);

        if(wave_params->base_freq > 2000) {  //手动校准刺激波形在能量尾部
            move_len = power_len - tphase1 - tphase2 - tinterphase1 - tinterphase2 ;
        } else if(wave_params->base_freq > 600) {
            move_len = power_len - tphase1 - tphase2 - tinterphase1 - tinterphase2 - start_count - 4;
        } else if(wave_params->base_freq > 300) {
            move_len = power_len - tphase1 - tphase2 - tinterphase1 - tinterphase2 - start_count - 8;
        } else if(wave_params->base_freq > 100) {
            move_len = power_len - tphase1 - tphase2 - tinterphase1 - tinterphase2 - start_count - 18;
        } else if(wave_params->base_freq > 50) {
            move_len = power_len - tphase1 - tphase2 - tinterphase1 - tinterphase2 - start_count - 43;
        } else if(wave_params->base_freq > 30) {
            move_len = power_len - tphase1 - tphase2 - tinterphase1 - tinterphase2 - start_count - 48;
        } else if(wave_params->base_freq > 14) {
            move_len = power_len - tphase1 - tphase2 - tinterphase1 - tinterphase2 - start_count - 65;
        } else if(wave_params->base_freq > 12) {
            move_len = power_len - tphase1 - tphase2 - tinterphase1 - tinterphase2 - start_count - 75;
        } else if(wave_params->base_freq > 9) {
            move_len = power_len - tphase1 - tphase2 - tinterphase1 - tinterphase2 - start_count - 115;
        } else { // 特殊处理
            move_len = power_len - tphase1 - tphase2 - tinterphase1 - tinterphase2 - start_count - 200;
        }
        if(move_len>(8191/(wave_params->stim_clk_div+1))) {
            (wave_params->pwm_duty)--;
            goto start1;

        }
        if(move_len < 1) {
            move_len = 0;
        }
        wave_params->pwm_delay_count = move_len;
        lms0820x4_pwm_setting_t pwm_mode;
        pwm_mode.bits.pwm_mode = wave_params->pwm_sync_mode;
        pwm_mode.bits.pwm_power_dly = move_len*(wave_params->stim_clk_div+1);
        err = x4_cmd_write(LMS0820X4_REG_07_PWM_SETTING, pwm_mode.value);
        CHECK_ERR(err);
    }

    /* 1. 设置刺激波形脉宽 */
    lms0820x4_tphase1_t tphase_1 = {0};
    tphase_1.bits.tphase1 = tphase1;
    err |= x4_cmd_write(LMS0820X4_REG_00_TPHASE1, tphase_1.value);
    CHECK_ERR(err);
    lms0820x4_tphase2_t tphase_2 = {0};
    tphase_2.bits.tphase2 = tphase2;
    err |= x4_cmd_write(LMS0820X4_REG_01_TPHASE2, tphase_2.value);
    CHECK_ERR(err);
    /* 2. 设置刺激相位间隔 */
    lms0820x4_tinterphase_t tinterphase_ = {0};
    tinterphase_.bits.tinterphase1 = tinterphase1;
    tinterphase_.bits.tinterphase2 = tinterphase2;
    err |= x4_cmd_write(LMS0820X4_REG_02_TINTERPHASE, tinterphase_.value);
    CHECK_ERR(err);
    lms0820x4_tphase3_t tphase_3 = {0};
    tphase_3.bits.tphase3 = tphase3;
    err |= x4_cmd_write(LMS0820X4_REG_03_TPHASE3, tphase_3.value);
    CHECK_ERR(err);
    lms0820x4_tinterpulse_t tinterpulse = {0};
    tinterpulse.bits.tinterpulse = tinterpulses;
    err |= x4_cmd_write(LMS0820X4_REG_04_TINTERPULSE, tinterpulse.value);
    CHECK_ERR(err);
    lms0820x4_phase_config_t phase_config = {0};
    phase_config.bits.stim_clk_div_value = wave_params->stim_clk_div;
    err |= x4_cmd_write(LMS0820X4_REG_09_PHASE_CONFIG, phase_config.value);
    CHECK_ERR(err);
    if(err != 0) {
        logInfo("[error] __LINE__ = %d", __LINE__);
    }

    return err;
}

/**
 * @brief 设置B组刺激波形参数(交替刺激模式用)
 * @param[in] wave_params B组波形参数
 * @return 0-成功，非0-失败
 */
static int __x4_rf_set_simple_feature_b(x4_simple_params *wave_params)
{
    int err = 0;

//    /* 1. 设置B组刺激波形脉宽 */
//    lms0820x4_amp_dac_p_5_t amp_dac_p_5 = {0};
//    amp_dac_p_5.ab_bits.tphase1_B = to_sysclock_time(wave_params->pulse_forward);
//    err |= x4_cmd_write(LMS0820X4_REG_15_AMP_DAC_P_5, amp_dac_p_5.value);
//
//    lms0820x4_amp_dac_p_6_t amp_dac_p_6 = {0};
//    amp_dac_p_6.ab_bits.tphase2_B = to_sysclock_time(wave_params->pulse_backward);
//    err |= x4_cmd_write(LMS0820X4_REG_16_AMP_DAC_P_6, amp_dac_p_6.value);
//
//    /* 2. 设置B组刺激相位间隔 */
//    lms0820x4_amp_dac_p_7_t amp_dac_p_7 = {0};
//    amp_dac_p_7.ab_bits.tinterphase1_B = to_sysclock_time(wave_params->time_interphase1);
//    amp_dac_p_7.ab_bits.tinterphase2_B = to_sysclock_time(wave_params->time_interphase2);
//    err |= x4_cmd_write(LMS0820X4_REG_17_AMP_DAC_P_7, amp_dac_p_7.value);
//
//    lms0820x4_amp_dac_p_8_t amp_dac_p_8 = {0};
//    amp_dac_p_8.ab_bits.tphase3_B = to_sysclock_time(wave_params->time_phase3);
//    err |= x4_cmd_write(LMS0820X4_REG_18_AMP_DAC_P_8, amp_dac_p_8.value);
//
//    lms0820x4_amp_dac_p_9_t amp_dac_p_9 = {0};
//    amp_dac_p_9.ab_bits.tinterpulse_B = to_sysclock_time(wave_params->time_interpulse);
//    err |= x4_cmd_write(LMS0820X4_REG_19_AMP_DAC_P_9, amp_dac_p_9.value);
//
//    if(err != 0) {
//        logInfo("[error] __x4_rf_set_simple_feature_b __LINE__ = %d", __LINE__);
//    }

    return err;
}

/**
 * @brief 设置刺激幅度
 * @param[in] pulse_a A组幅度参数
 * @param[in] pulse_b B组幅度参数(交替模式用)
 * @return 0-成功，非0-失败
 */
static int __x4_rf_set_range(x4_range_t pulse_a, x4_range_t pulse_b)
{
    int err = 0;
    uint16_t forward_b, backward_b;

    /* 计算B组幅度值 */
    forward_b = to_sys_ma(pulse_b.amplitude_forward);
    backward_b = to_sys_ma(pulse_b.amplitude_backward);

    /* 配置正向幅度 */
    lms0820x4_amp_dac_p_0_t amp_dac_p_0 = {0};
    amp_dac_p_0.bits.amplitude_dac_p_0 = to_sys_ma(pulse_a.amplitude_forward);
    if(pulse_b.amplitude_backward != 0 && pulse_b.amplitude_forward != 0) {
        amp_dac_p_0.bits.amplitude_dac_p_1 = forward_b & 0x3f;  /* B组正向幅度低6位 */
    }

    /* 配置负向幅度 */
    lms0820x4_amp_dac_n_0_t amp_dac_n_0 = {0};
    amp_dac_n_0.bits.amplitude_dac_n_0 = to_sys_ma(pulse_a.amplitude_backward);
    if(pulse_b.amplitude_backward != 0 && pulse_b.amplitude_forward != 0) {
        amp_dac_n_0.bits.amplitude_dac_n_1 = backward_b & 0x3f;  /* B组负向幅度低6位 */
    }

    /* 配置B组幅度高4位 */
    if(pulse_b.amplitude_backward != 0 && pulse_b.amplitude_forward != 0) {
        lms0820x4_amp_dac_p_1_t amp_dac_p_1 = {0};
        amp_dac_p_1.ab_bits.amplitude_dac_p_B_h = (forward_b >> 6) & 0x0F;  /* B组正向幅度高4位 */
        lms0820x4_amp_dac_n_1_t amp_dac_n_1 = {0};
        amp_dac_n_1.ab_bits.amplitude_dac_n_B_h = (backward_b >> 6) & 0x0F; /* B组负向幅度高4位 */

        err |= x4_cmd_write(LMS0820X4_REG_11_AMP_DAC_P_1, amp_dac_p_1.value);
        CHECK_ERR(err);
        err |= x4_cmd_write(LMS0820X4_REG_1B_AMP_DAC_N_1, amp_dac_n_1.value);
        CHECK_ERR(err);
    }

    err |= x4_cmd_write(LMS0820X4_REG_10_AMP_DAC_P_0, amp_dac_p_0.value);
    CHECK_ERR(err);
    err |= x4_cmd_write(LMS0820X4_REG_1A_AMP_DAC_N_0, amp_dac_n_0.value);
    CHECK_ERR(err);
    return err;
}

/**
 * @brief 设置电极极性配置
 * @param[in] polar_a A组极性配置
 * @param[in] polar_b B组极性配置(交替模式用)
 * @return 0-成功，非0-失败
 */
static int __x4_rf_set_polar(x4_polar_config polar_a, x4_polar_config polar_b)
{
    int err = 0;

    /* 配置A组电极 */
    lms0820x4_stim_on0_t stim_on0 = {0};
    stim_on0.bits.dcasel_p = polar_a.dir;           /* 阳极电极位置 */
    stim_on0.bits.dansel_p = (~polar_a.dir) & polar_a.on; /* 阴极电极位置 */
    err |= x4_cmd_write(LMS0820X4_REG_0A_STIM_ON0, stim_on0.value);
    CHECK_ERR(err);
    lms0820x4_stim_on1_t stim_on1 = {0};
    stim_on1.bits.dcasel_n = (~polar_a.dir) & polar_a.on;   /* 阳极电极位置 */
    stim_on1.bits.dansel_n = polar_a.dir;                   /* 阴极电极位置 */
    err |= x4_cmd_write(LMS0820X4_REG_0B_STIM_ON1, stim_on1.value);
    CHECK_ERR(err);
    /* 配置B组电极(交替模式) */
    if(polar_b.dir != 0 && polar_b.on != 0) {
        lms0820x4_amp_dac_p_3_t amp_dac_p_3 = {0};
        amp_dac_p_3.ab_bits.b_dcasel_p = polar_b.dir;
        amp_dac_p_3.ab_bits.b_dansel_p = (~polar_b.dir) & polar_b.on;
        err |= x4_cmd_write(LMS0820X4_REG_13_AMP_DAC_P_3, amp_dac_p_3.value);
        CHECK_ERR(err);
        lms0820x4_amp_dac_p_4_t amp_dac_p_4 = {0};
        amp_dac_p_4.ab_bits.b_dcasel_n = (~polar_b.dir) & polar_b.on;
        amp_dac_p_4.ab_bits.b_dansel_n = polar_b.dir;
        err |= x4_cmd_write(LMS0820X4_REG_14_AMP_DAC_P_4, amp_dac_p_4.value);
        CHECK_ERR(err);
    }

    return err;
}

/**
 * @brief 启动脉冲输出(内部函数)
 * @param[in] mode 刺激模式(简单/爆发等)
 * @param[in] pulse_cnt 脉冲个数
 * @param[in] low_power 低功耗模式使能
 * @param[in] inturn_mode 交替模式使能
 * @return 0-成功，非0-失败
 */
static int __x4_rf_start_pulse(uint8_t mode, uint8_t pulse_cnt,
                              uint8_t low_power, uint8_t inturn_mode)
{
    int err = 0;

    /* 配置刺激模式寄存器 */
    lms0820x4_stim_mode_t stim_mode = {0};
    stim_mode.bits.pulse_cnt = pulse_cnt;            /* 脉冲个数 */
    stim_mode.bits.inturn_mode = inturn_mode;        /* 交替模式 */
    stim_mode.bits.stim_low_power = low_power;       /* 低功耗模式 */
    stim_mode.bits.stim_mode = mode;                  /* 刺激模式 */
    stim_mode.bits.stim_stop = 0;                     /* 停止位 */
    stim_mode.bits.para_vld = 1;                      /* 参数有效 */
    stim_mode.bits.stim_start = 1;                    /* 启动位 */

    err = x4_cmd_write(LMS0820X4_REG_06_STIM_MODE, stim_mode.value);
    CHECK_ERR(err);
    if(err != 0) {
        logInfo("[error] __LINE__ = %d", __LINE__);
        return err;
    }

    /* 清除启动标志，开始刺激输出 */
    stim_mode.bits.stim_stop = 0;
    stim_mode.bits.para_vld = 0;
    stim_mode.bits.stim_start = 0;
    err |= x4_cmd_write(LMS0820X4_REG_06_STIM_MODE, stim_mode.value);
    CHECK_ERR(err);
    delay_ms(2000);
    return err;
}

/**
 * @brief 设置爆发刺激的脉冲间隔
 * @param[in] params 爆发刺激参数
 * @return 0-成功，非0-失败
 */
static int __x4_rf_set_burst_tinterburst(x4_burst_params *params)
{
    int err = 0;
    lms0820x4_tinterburst_t tinterburst;
    tinterburst.value = params->inter_burst;
    err |= x4_cmd_write(LMS0820X4_REG_08_TINTERBURST, tinterburst.value);
    CHECK_ERR(err);
    return err;
}

/**
 * @brief 停止脉冲输出
 * @return 0-成功，非0-失败
 */
int x4_rf_stop_pulse(void)
{
    int err = 0;
    lms0820x4_stim_mode_t stim_mode = {0};
    stim_mode.bits.stim_stop = 1;  /* 设置停止位 */
    err |= x4_cmd_write(LMS0820X4_REG_06_STIM_MODE, stim_mode.value);
    CHECK_ERR(err);
    return err;
}

/**
 * @brief 恢复脉冲输出
 * @return 0-成功，非0-失败
 */
int x4_rf_run_pulse(void)
{
    int err = 0;
    lms0820x4_stim_mode_t stim_mode = {0};
    stim_mode.bits.stim_start = 1;  /* 设置启动位 */
    err |= x4_cmd_write(LMS0820X4_REG_06_STIM_MODE, stim_mode.value);
    CHECK_ERR(err);
    return err;
}

/**
 * @brief 输出简单刺激脉冲
 * @param[in] params 简单刺激参数
 * @return 0-成功，非0-失败
 */
int x4_rf_out_pulse_simple(x4_simple_params *params)
{
    int err = 0;
    x4_range_t pluse_b = {0};        /* B组未使用，初始化为0 */
    x4_polar_config polar_b = {0};    /* B组未使用，初始化为0 */

    err |= __x4_rf_set_simple_feature(params);  /* 设置波形形状 */
    CHECK_ERR(err);
    err |= __x4_rf_set_range(params->range, pluse_b);  /* 设置幅度 */
    CHECK_ERR(err);
    err |= __x4_rf_set_polar(params->polar, polar_b);  /* 设置电极方向 */
    CHECK_ERR(err);
    err |= __x4_rf_start_pulse(X4_SIMPLE_MODE, 1,      /* 启动输出 */
                               params->low_power_mode, 0);
    CHECK_ERR(err);

    return err;
}

/**
 * @brief 输出爆发刺激脉冲
 * @param[in] params 简单刺激参数
 * @param[in] params2 爆发刺激参数
 * @return 0-成功，非0-失败
 */
int x4_rf_out_pulse_burst(x4_simple_params *params, x4_burst_params *params2)
{
    int err = 0;
    x4_range_t pluse_b = {0};        /* B组未使用，初始化为0 */
    x4_polar_config polar_b = {0};    /* B组未使用，初始化为0 */

    err |= __x4_rf_set_simple_feature(params);       /* 设置波形形状 */
    CHECK_ERR(err);
    err |= __x4_rf_set_range(params->range, pluse_b);  /* 设置幅度 */
    CHECK_ERR(err);
    err |= __x4_rf_set_polar(params->polar, polar_b);  /* 设置电极方向 */
    CHECK_ERR(err);
    err |= __x4_rf_set_burst_tinterburst(params2);     /* 设置爆发间隔 */
    CHECK_ERR(err);
    err |= __x4_rf_start_pulse(X4_BURST_MODE,         /* 启动输出 */
                               params2->pulse_count,
                               params->low_power_mode, 0);
    CHECK_ERR(err);

    return err;
}

/**
 * @brief 配置交替刺激(TWA)模式
 * @param[in] params 交替刺激参数，包含A/B两组独立配置
 * @return 0-成功，非0-失败
 *
 * 支持简单/爆发/随机模式下的AB交替刺激，两组参数完全独立配置
 */
int x4_rf_pluse_TWA(x4_TWA_params *params)
{
    int err = 0;

    /* 设置A/B两组波形参数 */
    err |= __x4_rf_set_simple_feature(&params->puse_a);   /* A组波形 */
    CHECK_ERR(err);
    err |= __x4_rf_set_simple_feature_b(&params->puse_b); /* B组波形 */
    CHECK_ERR(err);
    /* 设置电极方向和幅度 */
    err |= __x4_rf_set_polar(params->polar_a, params->polar_b);  /* 电极方向 */
    CHECK_ERR(err);
    err |= __x4_rf_set_range(params->range_a, params->range_b);  /* 幅度 */
    CHECK_ERR(err);
    /* 配置交替刺激切换时间 */
    lms0820x4_amp_dac_n_2_t amp_dac_n_2 = {0};
    lms0820x4_amp_dac_n_3_t amp_dac_n_3 = {0};

    /* 设置A组和B组的切换时间(默认1:1交替) */
    amp_dac_n_2.ab_bits.invert_cnt = 1;     /* A组持续1个周期 */
    amp_dac_n_3.ab_bits.invert_cnt_b = 1;   /* B组持续1个周期 */

    err |= x4_cmd_write(LMS0820X4_REG_1C_AMP_DAC_N_2, amp_dac_n_2.value);
    CHECK_ERR(err);
    err |= x4_cmd_write(LMS0820X4_REG_1D_AMP_DAC_N_3, amp_dac_n_3.value);
    CHECK_ERR(err);
    /* 启动交替刺激输出 */
    err |= __x4_rf_start_pulse(X4_SIMPLE_MODE, 1,
                               params->low_power_mode, 1);
    CHECK_ERR(err);
    return err;
}

