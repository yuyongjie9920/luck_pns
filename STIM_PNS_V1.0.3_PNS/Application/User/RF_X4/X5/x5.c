/**
 * @file x5.c
 * @brief LMS0820X5 驱动实现
 * @version 2.0
 * @date 2026-03-17
 */

#include "leadinno_app.h"
#include "main_user.h"
#include "crc.h"
#include "x5.h"

/*==============================================================================
 * 调试日志宏
 *============================================================================*/

#if 1
    #define log(...) logInfo(__VA_ARGS__)
#else
    #define log(...) do {} while(0)  // 绌烘搷浣滐紝閬垮厤璇硶閿欒
#endif

extern uint8_t NO_RX_DATA;

#define CHECK_ERR(err) if((err) != 0) return -1

/*==============================================================================
 * 全局变量
 *============================================================================*/

struct x5_data x5 = {0};

/*==============================================================================
 * 常量定义
 *============================================================================*/

#define DAC_TO_UA_FACTOR    2439        /**< DAC值转uA系数 2.439 * 1000 */
#define UA_TO_DAC_FACTOR    410         /**< uA转DAC系数 1000/2.439 ≈ 410 */

// 校准系数定义，基于实际测量调整
#define US_CALIBRATION_NUMERATOR   2308  // 实际值
#define US_CALIBRATION_DENOMINATOR 2000  // 理论值

uint32_t x5_us_to_clk(uint32_t us, uint8_t clk_div)
{
    uint32_t clk;
    // 应用校准系数
    clk = (uint32_t)((uint64_t)us * US_CALIBRATION_DENOMINATOR / US_CALIBRATION_NUMERATOR) + 1;
//    clk = us;
    return clk / (clk_div + 1);
}


/**
 * @brief DAC值转换为电流值(uA)
 */
uint32_t x5_dac_to_ma(uint16_t dac)
{
    return (dac * DAC_TO_UA_FACTOR + 500) / 1000;
}

/**
 * @brief 电流值(uA)转换为DAC值
 */
uint16_t x5_ma_to_dac(uint32_t ma_uA)
{
    return (ma_uA * UA_TO_DAC_FACTOR + 500) / 1000;
}

uint16_t ua_to_dac(uint32_t target_ua)
{
    // 校准参数
    const uint32_t cal_dac = 250;      // 校准点的DAC值
    const uint32_t cal_ua = 10540;     // 校准点的电流值(μA)

    uint32_t dac_value = (target_ua * cal_dac) / cal_ua;
    if (dac_value > 4095) {
        dac_value = 4095;
    }
    return (uint16_t)dac_value;
}

/*==============================================================================
 * RF通信基础函数 - 根据测试方案文档修正
 *============================================================================*/

/**
 * @brief 发送Query命令
 */
static inline int __x5_rf_send_query(void)
{
    uint8_t bit_len;
    uint16_t crc, check_crc;
    uint8_t send_data[5] = {0x80,0,0,0,0};
    uint8_t raw_data[8] = {0};
    uint8_t *out_data;
    uint8_t uid = 0;//0 & 0x7fff;
    int err;

    send_data[0] |= (uid >> 12) & 0x0F;    // UID楂�4浣� -> bytes[0]浣�4浣�
    send_data[1] = (uid >> 4) & 0xFF;      // UID涓棿8浣� -> bytes[1]
    send_data[2] = (uid & 0x0F) << 4;      // UID浣�4浣� -> bytes[2]楂�4浣�

    crc =  crc16Bitwise(send_data, 20);

    send_data[20/8] |= (crc >> 12) & 0x0F; // CRC楂�4浣嶅埌bytes[2]浣�4浣�
    send_data[20/8+1] = (crc >> 4) & 0xFF; // CRC涓棿8浣嶅埌bytes[3]
    send_data[20/8+2] = (crc & 0x0F) << 4; // CRC浣�4浣嶅埌bytes[4]楂�4浣�

    rf_tx_data(send_data, 36); //cmd quwey is  8 4bit  uid 16bit  crc 16bit

    if(NO_RX_DATA == 1) { //盲发
        for(uint8_t i = 0; i < 8; i++) {
            SET_RF_POWER(i);
            rf_tx_data(send_data, 36);
            err = rf_rx_data(raw_data, &bit_len);
            delay_ms(10);
        }
        SET_RF_POWER(0);
        delay_ms(10);
        logInfo("blind_sned_query ok");
        return 0;
    }

    err = rf_rx_data(raw_data, &bit_len);
    if (err != 0) {
        log("query broadcast rx error: %d", err);
        return err;
    }

    if(crc16Bitwise(raw_data, 8*2) != ((uint16_t)raw_data[2]<<8 | raw_data[3] )) { //校验crc
        logInfo("query crc fail");
        return -1;
    }
    lead.x5_query.value = (uint16_t)raw_data[0]<<8 | raw_data[1];
//    logInfo("lead uid = 0x%x", lead.x5_query.bits.efuse_id);
/* 解析返回数据: 8位状态 + 8位efuse_id + 16位CRC = 32位 */
//    if (bit_len >= 32) {
//        /* 打印详细的状态信息 */
//
//        log("===== Query Response =====");
//        log("Status Value : 0x%04x", lead.query.value);
//        log("EFUSE ID     : 0x%02x", lead.query.bits.efuse_id);
//        log("");
//        log("Bit15 - UnderVolt occurred     : %d", lead.query.bits.undervolt);
//        log("Bit14 - Program0 active        : %d", lead.query.bits.prog0_active);
//        log("Bit13 - Overlap occurred       : %d", lead.query.bits.overlap);
//        log("Bit12 - Stimulation complete   : %d", lead.query.bits.stim_complete);
//        log("Bit11 - Temperature done       : %d", lead.query.bits.temp_done);
//        log("Bit10 - Voltage/Current done   : %d", lead.query.bits.volt_done);
//        log("Bit9  - Impedance done         : %d", lead.query.bits.imp_done);
//        log("Bit8  - Reserved               : %d", lead.query.bits.reserved);
//        log("Bit7-0- EFUSE ID low 8 bits    : 0x%02x", lead.query.bits.efuse_id);
//        log("==========================");
//
//        /* 保存efuse_id到全局变量（如果需要） */
//        x5.efuse_id = lead.query.bits.efuse_id;
//        x5.last_status.value = lead.query.value;
//        return 0;
//    }
    return 0;
}

/**
 * @brief ok轮询Query
 */
int x5_rf_send_query(void)
{
    for (uint8_t power = 0; power < 15; power++) {
        SET_RF_POWER(power);
        if (__x5_rf_send_query() == 0) {
            logInfo("query ok power = %d", power);
            return RF_OK;
        } else {
            logInfo("query fail power is %d!!!!!!!!!!!", power);
        }
    }
    log("broadcast query failed: all power levels");
    return RF_FAIL;
}

/**
 * @brief ok指定功率发送Query
 */
int x5_rf_send_query_with_power(uint8_t power)
{
    int err;
    if (power > 15) power = 15;
    SET_RF_POWER(power);
    err = __x5_rf_send_query();
    return err;
}

/**
 * @brief ok所有功率点测试
 */
int x5_rf_send_query_all_power(void)
{
    int err = 0;
    for (uint8_t power = 0; power < 15; power++) {
        SET_RF_POWER(power);
        err = __x5_rf_send_query();
        log("power %2d: %s", power, err == 0 ? "OK" : "FAIL");
    }
    return 0;
}

/**
 * @brief ok发送写命令
 */
int __x5_rf_send_cmd_write(uint8_t reg, uint16_t reg_data)
{


    SET_RF_POWER(lead.set_query_power);

    x5_rf_write_send_data_t write_data;
    x5_rf_write_dec_data_t tx;
    uint8_t rx_data[8];
    uint16_t crc;
    uint8_t nbits=0;
    uint16_t uid;
    int err;

    write_data.cmd = 0xc3;
    write_data.reg = reg;
    write_data.reg_data = ((reg_data & 0xff00) >> 8) | ((reg_data & 0x00ff) << 8);
    uid = lead.x5_query.bits.efuse_id;
    uid = (uid<<8) | (uid >> 8);
    write_data.uid = uid;//lead.x5_query.bits.efuse_id;//uid;//x4.filler.data.uid;

    crc = crc16Bitwise((uint8_t *)&write_data, (sizeof(write_data.cmd)+sizeof(write_data.reg)+sizeof(write_data.reg_data)+sizeof(write_data.uid))* 8);  //璁板緱璺熸崲crc  *******************
    write_data.crc_h = crc>>8;
    write_data.crc_l = crc;

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


    err = rf_rx_data(rx_data, &nbits);
    if(err != 0) {
        logInfo("RX err = %d", err);
    }

    tx.crc = ((rx_data[2] & 0x7F) << 9) | (rx_data[3] << 1) | ((rx_data[4] >> 7) & 0x01);

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
        log("write reg 0x%x->0x%x fail err = %d", reg, reg_data, err);
    }
    return err;
}

/**
 * @brief ok发送读命令
 */
static inline int __x5_rf_send_cmd_read(uint8_t reg, uint16_t *get_reg_data)
{
    x5_rf_read_send_data_t send_data;
    x5_rf_rx_dec_data_t rx;

    uint8_t rx_data[8];
    uint16_t crc;
    uint8_t nbits=0;
    send_data.cmd = 0xc2; //璇诲懡浠�
    send_data.reg = reg;
    uint16_t uid;

    uid = lead.x5_query.bits.efuse_id;
    uid = (uid<<8) | (uid >> 8);

    send_data.uid = uid;//x4.filler.data.uid;
    crc = crc16Bitwise((uint8_t *)&send_data, (sizeof(send_data.cmd)+sizeof(send_data.reg)+sizeof(send_data.uid))* 8);  //璁板緱璺熸崲crc  *******************
    send_data.crc_h = crc>>8;
    send_data.crc_l = crc;
    rf_tx_data((uint8_t *)&send_data, sizeof(send_data)*8);

    if(NO_RX_DATA == 1) { //盲发
        delay_ms(50);
        return 0;
    }

    int err = rf_rx_data(rx_data, &nbits);
//    log("rf_rx_data nbits = %d", nbits);

    // header: 1浣�->bit7
    rx.header = (rx_data[0] >> 7) & 0x01;
    // reg_data: 16浣嶏紝浠巄it1寮�濮�
    rx.reg_data = ((rx_data[0] & 0x7F) << 9) | (rx_data[1] << 1) | ((rx_data[2] >> 7) & 0x01);
    // rn: 16浣�
    rx.rn = ((rx_data[2] & 0x7F) << 9) | (rx_data[3] << 1) | ((rx_data[4] >> 7) & 0x01);
    // crc: 16浣�
    rx.crc = ((rx_data[4] & 0x7F) << 9) | (rx_data[5] << 1) | ((rx_data[6] >> 7) & 0x01);

//    log("reg_data = 0x%04X", rx.reg_data);
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

    SET_RF_POWER(lead.stim_pulse_power);  //璁剧疆灏勯鍔熺巼涓哄埡婵�鍔熺巼
    return err;
}

/*==============================================================================
 * ok带重试的寄存器读写
 *============================================================================*/

inline int x5_cmd_write(uint8_t reg, uint16_t reg_data)
{
    int err = 0;
    int retry = 10;

#if 1
    do {
        err = __x5_rf_send_cmd_write(reg, reg_data);
        if (err != 0) {
            err = __x5_rf_send_cmd_write(reg, reg_data);
            if(err != 0) {
                x5_rf_send_query();  /* 重新查询 */
            }
        }
    } while (err != 0 && retry-- > 0);
    return (retry > 0) ? 0 : -1;
#else
    err = __x5_rf_send_cmd_write(reg, reg_data);
    return err;
#endif

}

inline int x5_cmd_read(uint8_t reg, uint16_t *reg_data)
{
    int err = 0;
    uint8_t retry = 10;

    do {
        err = __x5_rf_send_cmd_read(reg, reg_data);
        if (err != 0) {
            x5_rf_send_query();  /* 重新查询 */
        }
    } while (err != 0 && retry-- > 0);

    return (retry > 0) ? 0 : -1;
}

/**
 * @brief ok获取整流电压
 */
int x5_rf_get_vrecp(float *vrecp, uint16_t delay_count)
{
    int err = 0;
    uint16_t vdd_low = 0, vdd_high = 0;
    lms0820x5_volt_delay_sample_t volt_delay = {0};
    lms0820x5_ctrlreg0_t ctrlreg0 = {0};

    /* 配置电压延迟采样 */
    volt_delay.bits.sample_delay = delay_count;
    err = x5_cmd_write(LMS0820X5_REG_65_VOLT_DELAY_SAMPLE, volt_delay.value);
    CHECK_ERR(err);

    if (delay_count > 0) {
        SET_RF_POWER(0);
        delay_ms(10);
        SET_RF_POWER(lead.set_query_power);
    }

    /* 启动电压测量 */
    ctrlreg0.bits.start_en = X5_MEAS_ENABLE_VOLT;
    err = x5_cmd_write(LMS0820X5_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);

    delay_ms(5);  /* 等待测量完成 */

    /* 读取测量结果 */
    err = x5_cmd_read(LMS0820X5_REG_72_VDD_LOW, &vdd_low);
    CHECK_ERR(err);
    err = x5_cmd_read(LMS0820X5_REG_73_VDD_HIGH, &vdd_high);
    CHECK_ERR(err);

    /* 清零测量使能 */
    ctrlreg0.bits.start_en = X5_MEAS_ENABLE_NONE;
    err = x5_cmd_write(LMS0820X5_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);

    /* ADC值转换为电压 - 根据测试方案文档公式 */
    /* 公式: OUT_RTV = VOUT_DAC/50 * R * 4/30 + 0.305 */
    /* 这里简化为线性转换，实际需要校准 */
    const float ADC_OFFSET = 98.0f;
    const float ADC_SCALE = 122.0f;

    uint16_t data_low = vdd_low & 0x0FFF;
    uint16_t data_high = vdd_high & 0x0FFF;

    float v_min = (data_low > ADC_OFFSET) ? (data_low - ADC_OFFSET) / ADC_SCALE : 0;
    float v_max = (data_high > ADC_OFFSET) ? (data_high - ADC_OFFSET) / ADC_SCALE : 0;

    v_min = (float)((int)(v_min * 100 + 0.5f)) / 100.0f;
    v_max = (float)((int)(v_max * 100 + 0.5f)) / 100.0f;

    if (delay_count > 0) {
        vrecp[0] = v_min;
        vrecp[1] = v_max;
        log("VRECP: min=%.2fV, max=%.2fV", v_min, v_max);
    } else {
        *vrecp = v_min;
        log("VRECP: %.2fV", v_min);
    }

    return err;
}


/**
 * @brief ok获取阻抗 - 根据get_polar_res_trim函数的公式
 * 公式: res_cal = (res[i] * 2.7717 - 5722) / f_mhz
 * 其中f_mhz根据trim_osc配置：0时用0.9，其他值时用1.24
 */
int x5_rf_get_res(uint8_t channel_sel, uint16_t *res, uint8_t osc)
{
    int err = 0;
    uint16_t res_raw[8] = {0};
    lms0820x5_ctrlreg0_t ctrlreg0 = {0};
    lms0820x5_imp_data_t imp_data[8];
    float f_mhz;
    float res_cal;

    /* 启动阻抗测量 */
    ctrlreg0.bits.start_en = X5_MEAS_ENABLE_IMP;
    ctrlreg0.bits.imp_channel_sel = channel_sel;
    ctrlreg0.bits.imp_test_trim_osc = osc;
    if(ctrlreg0.bits.imp_test_trim_osc == 0) {
        f_mhz = 0.9f;    /* 默认大范围低精度 */
    } else {
        f_mhz = 1.24f;   /* 小范围高精度 */
    }

    err = x5_cmd_write(LMS0820X5_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);

    delay_ms(10);

    /* 读取8通道数据 */
    for (uint8_t i = 0; i < 8; i++) {
        err = x5_cmd_read(LMS0820X5_REG_6A_IMP_DATA1 + i, &imp_data[i].value);
        res_raw[i] = imp_data[i].bits.imp_data & 0x0FFF;  /* 只取低12位有效数据 */
    }

    /* 清零使能 */
    ctrlreg0.bits.start_en = 0;
    err = x5_cmd_write(LMS0820X5_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);

    /* 根据原公式计算电阻值：res_cal = (res[i] * 2.7717 - 5722) / f_mhz */
    for (int i = 0; i < 8; i++) {
        /* 第一个通道(索引0)是参考通道，不计算电阻值，直接返回原始值 */
        if (i == 0) {
            res[i] = res_raw[i];
            log("ref channel = %d (raw=0x%03x)", res_raw[i], res_raw[i]);
            continue;
        }
        /* 应用原公式计算 */
        res_cal = (res_raw[i] * 2.7717f - 5722.0f) / f_mhz;
        /* 确保结果非负 */
        if (res_cal < 0) {
            res_cal = 0;
        }
        res[i] = (uint16_t)(res_cal + 0.5f);  /* 四舍五入 */
        log("R[%d-%d] = %dR (raw=0x%03x, cal=%.2f)",
            channel_sel, (channel_sel + i) % 8, res[i], res_raw[i], res_cal);
    }

    return err;
}


/**
 * @brief ok获取温度
 */
int x5_rf_get_temp(float *temp)
{
    int err = 0;
    uint16_t temp_adc = 0;
    lms0820x5_ctrlreg0_t ctrlreg0 = {0};

    ctrlreg0.bits.start_en = X5_MEAS_ENABLE_TEMP;
    err = x5_cmd_write(LMS0820X5_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);

    delay_ms(10);
    err = x5_cmd_read(LMS0820X5_REG_74_TEMP_DATA, &temp_adc);
    CHECK_ERR(err);

    ctrlreg0.bits.start_en = X5_MEAS_ENABLE_NONE;
    err = x5_cmd_write(LMS0820X5_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);

    float temperature;
    temperature = 65.856f - (float)(temp_adc & 0x0FFF) / 75.188f; /* ADC鍊艰浆鎹负娓╁害 */
    *temp = roundf(temperature * 10.0f) / 10.0f;/* 鍥涜垗浜斿叆淇濈暀1浣嶅皬鏁� */
    logInfo("temp = %.1f", *temp);

    return err;
}

/**
 * @brief ok获取电流
 */
int x5_rf_get_current(float *current_p, float *current_n)
{
    int err = 0;
    lms0820x5_ctrlreg0_t ctrlreg0 = {0};
    uint16_t data_p = 0, data_n = 0;

    /* 使能电流测量 */
    ctrlreg0.bits.current_measure = 1;
    err = x5_cmd_write(LMS0820X5_REG_68_CTRLREG0, ctrlreg0.value);
    CHECK_ERR(err);

    delay_us(500);  /* 等待模拟采样完成 */

    /* 读取电流值 */
    err = x5_cmd_read(LMS0820X5_REG_75_CURRENT_DATA_NEG, &data_n);
    CHECK_ERR(err);

    err = x5_cmd_read(LMS0820X5_REG_76_CURRENT_DATA_PLUS, &data_p);
    CHECK_ERR(err);

    /* 清零使能 */
    err = x5_cmd_write(LMS0820X5_REG_68_CTRLREG0, 0);
    CHECK_ERR(err);

    *current_p = x5_dac_to_ma(data_p & 0x0FFF) / 1000.0f;
    *current_n = x5_dac_to_ma(data_n & 0x0FFF) / 1000.0f;

    log("CURRENT: P=%.2fmA (0x%03x), N=%.2fmA (0x%03x)",
        *current_p, data_p & 0x0FFF, *current_n, data_n & 0x0FFF);
    return err;
}

/**
 * @brief ok写eFuse - 根据测试方案文档
 * 使用VDDA5给TMVDDA5供电烧写efuse
 */
int x5_rf_write_efuse(uint32_t efuse)
{
    int err = 0;
    uint32_t efuse_read = 0;

    efuse &= 0xFFFFFF;  /* 24位有效 */

    rf_pwm_close();

    /* 关闭PWM */
    rf_pwm_close();

    lms0820x5_efuse_w_data0_t wdata0 = {0};
    lms0820x5_efuse_w_data1_t wdata1 = {0};
    lms0820x5_efuse_tburn_t tburn = {0};
    lms0820x5_ldo_trim_t trim = {0};
    /* 1. 先清零数据有效位 */
    wdata1.bits.efuse_data_valid = 0;
    wdata1.bits.efuse_data_enable = 0x3F;  /* 烧录所有bit */
    wdata1.bits.efuse_w_data_high = (efuse >> 16) & 0xFF;
    err = x5_cmd_write(LMS0820X5_REG_25_EFUSE_W_DATA1, wdata1.value);
//    CHECK_ERR(err);

    /* 2. 配置烧写时间 - X5增加了一位，时间范围更大 */
    tburn.value = 0xFFFF;
    err = x5_cmd_write(LMS0820X5_REG_28_EFUSE_TBURN, tburn.value);
//    CHECK_ERR(err);

    trim.value = 6;
    err = x5_cmd_write(LMS0820X5_REG_64_LDO_TRIM, trim.value);
//    CHECK_ERR(err);

    /* 3. 写入低16位数据 */
    wdata0.value = efuse & 0xFFFF;
    err = x5_cmd_write(LMS0820X5_REG_24_EFUSE_W_DATA0, wdata0.value);
//    CHECK_ERR(err);

    /* 4. 写入高8位并启动烧录 */
    wdata1.bits.efuse_data_valid = 1;  /* 上升沿启动烧写 */
    err = x5_cmd_write(LMS0820X5_REG_25_EFUSE_W_DATA1, wdata1.value);
//    CHECK_ERR(err);

    /* 5. 最大功率供电 */
    SET_RF_POWER(POWER_MAX);
    log("Waiting for efuse write (3 seconds)...");
    delay_ms(3000);

    /* 6. 复位芯片 */
    rf_reset_chip();

    /* 7. 重新广播查询获取新efuse_id */
    err = x5_rf_send_query();
//    CHECK_ERR(err);

    /* 8. 读取验证 */
    err = x5_rf_read_efuse(&efuse_read);
//    CHECK_ERR(err);

    if (efuse_read == efuse) {
        log("EFUSE write OK: 0x%06x", efuse_read);
    } else {
        log("EFUSE verify FAIL: write 0x%06x, read 0x%06x", efuse, efuse_read);
    }

    return err;
}

/**
 * @brief ok读eFuse
 */
int x5_rf_read_efuse(uint32_t *efuse)
{
    int err = 0;
    uint16_t data0 = 0, data1 = 0;

    err = x5_cmd_read(LMS0820X5_REG_26_EFUSE_R_DATA0, &data0);
    err = x5_cmd_read(LMS0820X5_REG_27_EFUSE_R_DATA1, &data1);

    *efuse = data0 | ((data1 & 0xFF) << 16);
    return err;
}

/**
 * @brief 停止刺激
 */
int x5_rf_stop_pulse(void)
{
    lms0820x5_stim_mode_t stim_mode = {0};
    stim_mode.bits.stim_stop = 1;
    return x5_cmd_write(LMS0820X5_REG_06_STIM_MODE, stim_mode.value);
}


/**
 * @brief 配置相位参数
 */
int x5_rf_config_phase(x5_phase_config_t *config)
{
    int err = 0;
    lms0820x5_phase_config_t reg = {0};
    err = x5_cmd_read(LMS0820X5_REG_09_PHASE_CONFIG, &reg.value);
    logInfo("reg.value = 0x%x", reg.value);
    CHECK_ERR(err);
    reg.bits.stim_clk_div_value = config->stim_clk_div;
    err = x5_cmd_write(LMS0820X5_REG_09_PHASE_CONFIG, reg.value);
    CHECK_ERR(err);

    return err;
}
/**
 * @brief 软件复位
 */
int __x5_rf_reset(void)
{
    int err = 0;
    lms0820x5_trimdata_ctrl0_t ctrl0;
    ctrl0.value = 0xc0;
    ctrl0.bits.cmd_rst = 1;
    __x5_rf_send_cmd_write(LMS0820X5_REG_60_TRIMDATA_CTRL0, ctrl0.value);  //芯片问题 复位无回复 忽略 盲发
    __x5_rf_send_cmd_write(LMS0820X5_REG_60_TRIMDATA_CTRL0, ctrl0.value);
    __x5_rf_send_cmd_write(LMS0820X5_REG_60_TRIMDATA_CTRL0, ctrl0.value);
    delay_ms(200);
    ctrl0.bits.cmd_rst = 0;
    err = x5_cmd_write(LMS0820X5_REG_60_TRIMDATA_CTRL0, ctrl0.value);
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

static int __x5_rf_start_pulse(uint8_t mode, uint8_t pulse_cnt, uint8_t low_power, uint8_t program_cnt)
{
    int err = 0;

    /* 配置刺激模式寄存器 */
    lms0820x5_stim_mode_t stim_mode = {0};
    stim_mode.bits.pulse_cnt = pulse_cnt;             /* 脉冲个数 */
    stim_mode.bits.stim_low_power = low_power;        /* 低功耗模式 */
    stim_mode.bits.stim_mode = mode;                  /* 刺激模式 */
    stim_mode.bits.stim_stop = 0;                     /* 停止位 */
    stim_mode.bits.program_cnt = program_cnt;         /* 程序个数 */
    stim_mode.bits.para_vld = 1;                      /* 参数有效 */
    stim_mode.bits.stim_start = 1;                    /* 启动位 */

    err = x5_cmd_write(LMS0820X5_REG_06_STIM_MODE, stim_mode.value);
    CHECK_ERR(err);

    /* 清除启动标志，开始刺激输出 */
    stim_mode.bits.stim_stop = 0;
    stim_mode.bits.para_vld = 0;
    stim_mode.bits.stim_start = 0;
    err = x5_cmd_write(LMS0820X5_REG_06_STIM_MODE, stim_mode.value);

    return err;
}

/**
 * @brief 设置刺激波形时间  多程序波形周期必须需要为整倍数    列入波形1 100hz  波形二为200hz
 * @return 0-成功，非0-失败
 */
static int __x5_rf_set_simple_feature(x5_simple_params_t *params)
{
    uint16_t tphase1[4], tphase2[4], tphase3[4], tinterphase1[4], tinterphase2[4], tinterpulse[4];
    uint32_t total_clk[4];
    uint32_t tphase3_save;
    uint32_t tinterpulse_save;
    uint32_t burst_tinter_save;
    uint16_t burst_tinter;
    int err = 0;
start:


    for(uint8_t i=0; i<(params->program_cnt+1); i++) { //一次性获取完所以程序的时间值
        tphase1[i]      = x5_us_to_clk(params->program[i].tphase1, params->phase.stim_clk_div);//设置正向脉宽
        tphase2[i]      = x5_us_to_clk(params->program[i].tphase2, params->phase.stim_clk_div);//设置负向脉宽
        tphase3[i]      = x5_us_to_clk(params->program[i].tphase3, params->phase.stim_clk_div);//电荷平衡时间
        tinterphase1[i] = x5_us_to_clk(params->program[i].tinterphase1, params->phase.stim_clk_div);//设置正向脉宽后的延时
        tinterphase2[i] = x5_us_to_clk(params->program[i].tinterphase2, params->phase.stim_clk_div);//设置负向脉宽后的延时
        total_clk[i]    = x5_us_to_clk(1000000 / params->base_freq, params->phase.stim_clk_div) / params->program[i].multiple - 1; //计算出总的clk数  - 1是芯片内部会自己换算

        if(params->pulse_mode == X5_SIMPLE_MODE) { //简单模式  和多程序
            logInfo("total_clk[%d] = %d", i,total_clk[i]); //计算刺激波形的总clk数
            tinterpulse_save = total_clk[i] - tphase1[i] - tphase2[i] - tinterphase1[i] - tinterphase2[i] - tphase3[i];
            if(tinterpulse_save >= 0xffff) {
                params->phase.stim_clk_div++;
                if(params->phase.stim_clk_div>63) {
                    return 0;
                }
                logInfo("[error hz moro min] !!!!!!!!!");
                goto start;
            } else {
                tinterpulse[i] = total_clk[i] - tphase1[i] - tphase2[i] - tinterphase1[i] - tinterphase2[i] - tphase3[i];
            }
        } else if(params->pulse_mode == X5_BURST_MODE) {
            burst_tinter_save = total_clk[0] - (tphase1[0] + tphase2[0] + tphase3[0] + tinterphase1[0] + tinterphase2[0] + tinterpulse[0]) * params->burst_pulse_cnt;
            if(burst_tinter_save >= 0xffff) {
                params->phase.stim_clk_div++;
                if(params->phase.stim_clk_div>63) {
                    return 0;
                }
                logInfo("[error hz moro min] !!!!!!!!!");
                goto start;
            }
            lms0820x5_tinterburst_t burst = {0};
            burst.value = burst_tinter_save;
            err = x5_cmd_write(LMS0820X5_REG_08_TINTERBURST, burst.value);
        }
    }


    //设置系统分频
    x5_phase_config_t cfg;
    cfg.stim_clk_div = params->phase.stim_clk_div;
    x5_rf_config_phase(&cfg);

    lms0820x5_tphase1_0_t tphase1_0 = {0};
    lms0820x5_tphase2_0_t tphase2_0 = {0};
    lms0820x5_tphase3_0_t tphase3_0 = {0};

    lms0820x5_tphase1_1_t tphase1_1 = {0};
    lms0820x5_tphase2_1_t tphase2_1 = {0};
    lms0820x5_tphase3_1_t tphase3_1 = {0};

    lms0820x5_tphase2_2_t tphase2_2 = {0};
    lms0820x5_tphase3_2_t tphase3_2 = {0};

    lms0820x5_tphase1_3_t tphase1_3 = {0};
    lms0820x5_tphase3_3_t tphase3_3 = {0};

    lms0820x5_tinterphase0_t tinterphase_0 = {0};
    lms0820x5_tinterphase1_t tinterphase_1 = {0};
    lms0820x5_tinterphase2_t tinterphase_2 = {0};
    lms0820x5_tinterphase3_t tinterphase_3 = {0};

    lms0820x5_tinterpulse0_t tinterpulse_0 = {0};
    lms0820x5_tinterpulse1_t tinterpulse_1 = {0};
    lms0820x5_tinterpulse2_t tinterpulse_2 = {0};
    lms0820x5_tinterpulse3_t tinterpulse_3 = {0};

    if(params->program_cnt >= 0) { //程序0波形时间
        tphase1_0.bits.tphase1 = tphase1[0];
        tphase2_0.bits.tphase2 = tphase2[0];
        tphase3_0.bits.tphase3 = tphase3[0];
        tinterphase_0.bits.tinterphase1 = tinterphase1[0];
        tinterphase_0.bits.tinterphase2 = tinterphase2[0];
        tinterpulse_0.bits.tinterpulse = tinterpulse[0];
        err = x5_cmd_write(LMS0820X5_REG_00_TPHASE1_0, tphase1_0.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_01_TPHASE2_0, tphase2_0.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_03_TPHASE3_0, tphase3_0.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_02_TINTERPHASE0, tinterphase_0.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_04_TINTERPULSE0, tinterpulse_0.value);
        CHECK_ERR(err);
    }
    if(params->program_cnt >= 1) { //程序1波形时间
        tphase1_1.bits.tphase1 = tphase1[1];       //bit 0-11
        tphase1_1.bits.tphase2 = tphase2[1];       //bit 0-3
        tphase2_1.bits.tphase2 = tphase2[1] >> 4;  //bit 4-11
        tphase2_1.bits.tphase3 = tphase3[1];
        tphase3_1.bits.tphase3_1 = tphase3[1]>>8;  //bit 8-11
        if(params->program_cnt >= 2) {//程序2 tphase1
            tphase3_1.bits.tphase1_2 = tphase1[2]; //bit 0-11
        }
        tinterphase_1.bits.tinterphase1 = tinterphase1[1];
        tinterphase_1.bits.tinterphase2 = tinterphase2[1];
        tinterpulse_1.bits.tinterpulse = tinterpulse[1];

        err = x5_cmd_write(LMS0820X5_REG_15_TPHASE1_1, tphase1_1.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_16_TPHASE2_1, tphase2_1.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_17_TPHASE3_1, tphase3_1.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_1C_TINTERPHASE1, tinterphase_1.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_1F_TINTERPULSE1, tinterpulse_1.value);
        CHECK_ERR(err);
    }
    if(params->program_cnt >= 2) { //程序2波形时间

        tphase2_2.bits.tphase2_2 = tphase2[2]; //bit 0-11
        tphase2_2.bits.tphase3_2 = tphase3[2]; //bit 0-3
        tphase3_2.bits.tphase3_2 = tphase3[2]>>4; //bit 4-11

        if(params->program_cnt >= 3) {//程序3 tphase1
            tphase3_2.bits.tphase1_3 = tphase1[3]; //0-7
        }
        tinterphase_2.bits.tinterphase1 = tinterphase1[2];
        tinterphase_2.bits.tinterphase2 = tinterphase2[2];
        tinterpulse_2.bits.tinterpulse = tinterpulse[2];

        logInfo("tphase2_2.bits.tphase3_2 = 0x%x", tphase2_2.bits.tphase3_2);
        logInfo("tphase3_2.bits.tphase3_2 = 0x%x", tphase3_2.bits.tphase3_2);
        logInfo("tphase2_2.value = 0x%x", tphase2_2.value);
        logInfo("tphase3_2.value = 0x%x", tphase3_2.value);
        err = x5_cmd_write(LMS0820X5_REG_18_TPHASE2_2, tphase2_2.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_19_TPHASE3_2, tphase3_2.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_1D_TINTERPHASE2, tinterphase_2.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_20_TINTERPULSE2, tinterpulse_2.value);
        CHECK_ERR(err);
    }
    if(params->program_cnt >= 3) { //程序3波形时间
        tphase1_3.bits.tphase1_3 = tphase1[3]>>8; //bit 8-11
        tphase1_3.bits.tphase2_3 = tphase2[3];    //bit 0-11
        tphase3_3.bits.tphase3 = tphase3[3];

        tinterphase_3.bits.tinterphase1 = tinterphase1[3];
        tinterphase_3.bits.tinterphase2 = tinterphase2[3];
        tinterpulse_3.bits.tinterpulse3 = tinterpulse[3];

        err = x5_cmd_write(LMS0820X5_REG_1A_TPHASE1_3, tphase1_3.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_1B_TPHASE3_3, tphase3_3.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_1E_TINTERPHASE3, tinterphase_3.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_21_TINTERPULSE3, tinterpulse_3.value);
        CHECK_ERR(err);
    }
    return err;
}

/**
 * @brief 设置刺激幅度
 * @return 0-成功，非0-失败
 */
static int __x5_rf_set_range(x5_simple_params_t *params)
{

    int err = 0;
    lms0820x5_amp_dac_p_0_t amp_p0 = {0};
    lms0820x5_amp_dac_p_1_t amp_p1 = {0};
    lms0820x5_amp_dac_p_2_t amp_p2 = {0};
    lms0820x5_amp_dac_p_3_t amp_p3 = {0};
    lms0820x5_amp_dac_p_4_t amp_p4 = {0};

    uint16_t amp_p[4],  amp_n[4];

    for(uint8_t i=0; i<(params->program_cnt+1); i++) { //一次性获取完所以程序的幅度值
        amp_p[i] = ua_to_dac(params->program[i].amplitude_p);
        amp_n[i] = ua_to_dac(params->program[i].amplitude_n);
    }

    //0x10
    amp_p0.bits.amplitude_dac_p_0 = amp_p[0];           //程序0 正向刺激幅度 0-9 bit
    amp_p0.bits.amplitude_dac_n_0 = amp_n[0];           //程序0 负向刺激 0-5 bit 数据会防止溢出 所以不用换算

    //0x11
    amp_p1.bits.amplitude_dac_n_0 = amp_n[0] >> 6;      //程序0 负向刺激 6-9 bit
    if(params->program_cnt >= 1) {
        amp_p1.bits.amplitude_dac_p_1 = amp_p[1] & 0x1ff;       //程序1 正向刺激 0-9 bit
        amp_p1.bits.amplitude_dac_n_1 = amp_n[1] & 0x03;       //程序1 负向刺激 0-1 bit
    }

    //0x12
    if(params->program_cnt >= 1) {
        amp_p2.bits.amplitude_dac_n_1 = amp_n[1] >> 2;  //程序1 负向刺激 2-9 bit
    }
    if(params->program_cnt >= 2) {
        amp_p2.bits.amplitude_dac_p_2 = amp_p[2] & 0xff;       //程序2 正向刺激 7-0 bit
    }

    //0x13
    if(params->program_cnt >= 2) {
        amp_p3.bits.amplitude_dac_p_2 = amp_p[2] >> 8;  //程序2 正向刺激 8-9 bit
        amp_p3.bits.amplitude_dac_n_2 = amp_n[2];       //程序2 负向刺激 0-9 bit
    }
    if(params->program_cnt >= 3) {
        amp_p3.bits.amplitude_dac_p_3 = amp_p[3];       //程序3 正向刺激 0-3 bit
    }

    //0x14
    if(params->program_cnt >= 3) {
        amp_p4.bits.amplitude_dac_p_3 = amp_n[3] >> 4;  //程序3 正向刺激 4-9 bit
        amp_p4.bits.amplitude_dac_n_3 = amp_p[3];       //程序3 负向刺激 0-9 bit
    }

    err = x5_cmd_write(LMS0820X5_REG_10_AMP_DAC_P_0, amp_p0.value);

    CHECK_ERR(err);

    err = x5_cmd_write(LMS0820X5_REG_11_AMP_DAC_P_1, amp_p1.value);
    CHECK_ERR(err);

    if(params->program_cnt >= 1) {
        err = x5_cmd_write(LMS0820X5_REG_12_AMP_DAC_P_2, amp_p2.value);
        CHECK_ERR(err);
    }

    if(params->program_cnt >= 2) {
        err = x5_cmd_write(LMS0820X5_REG_13_AMP_DAC_P_3, amp_p3.value);
        CHECK_ERR(err);
    }

    if(params->program_cnt >= 3) {
        err = x5_cmd_write(LMS0820X5_REG_14_AMP_DAC_P_4, amp_p4.value);
        CHECK_ERR(err);
    }
    return err;
}

/**
 * @brief 设置电极极性配置
 * @return 0-成功，非0-失败
 */
static int __x5_rf_set_polar(x5_simple_params_t *params)
{
    int err = 0;

    lms0820x5_stim_on0_0_t stim_on0_0 = {0};
    lms0820x5_stim_on0_1_t stim_on0_1 = {0};
    lms0820x5_stim_on1_0_t stim_on1_0 = {0};
    lms0820x5_stim_on1_1_t stim_on1_1 = {0};
    lms0820x5_stim_on2_0_t stim_on2_0 = {0};
    lms0820x5_stim_on2_1_t stim_on2_1 = {0};
    lms0820x5_stim_on3_0_t stim_on3_0 = {0};
    lms0820x5_stim_on3_1_t stim_on3_1 = {0};

    uint16_t stim0[4], stim1[4];

    for(uint8_t i=0; i<(params->program_cnt+1); i++) { //一次性获取完所以程序的幅度值
        stim0[i] = LMS0820X5_CALC_STIM_ON0(params->program[i].polar.dir, params->program[i].polar.on);
        stim1[i] = LMS0820X5_CALC_STIM_ON1(params->program[i].polar.dir, params->program[i].polar.on);
    }

    stim_on0_0.value = stim0[0];
    stim_on0_1.value = stim1[0];

    stim_on1_0.value = stim0[1];
    stim_on1_1.value = stim1[1];

    stim_on2_0.value = stim0[2];
    stim_on2_1.value = stim1[2];

    stim_on3_0.value = stim0[3];
    stim_on3_1.value = stim1[3];

    err = x5_cmd_write(LMS0820X5_REG_0A_STIM_ON0, stim_on0_0.value);
    CHECK_ERR(err);
    err = x5_cmd_write(LMS0820X5_REG_0B_STIM_ON1, stim_on0_1.value); //电极相关
    CHECK_ERR(err);

    if(params->program_cnt >= 1) {
        err = x5_cmd_write(LMS0820X5_REG_22_STIM_ON1_0, stim_on1_0.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_23_STIM_ON1_1, stim_on1_1.value);
        CHECK_ERR(err);
    }
    if(params->program_cnt >= 2) {
        err = x5_cmd_write(LMS0820X5_REG_29_STIM_ON2_0, stim_on2_0.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_2A_STIM_ON2_1, stim_on2_1.value);
        CHECK_ERR(err);
    }
    if(params->program_cnt >= 3) {
        err = x5_cmd_write(LMS0820X5_REG_2B_STIM_ON3_0, stim_on3_0.value);
        CHECK_ERR(err);
        err = x5_cmd_write(LMS0820X5_REG_2C_STIM_ON3_1, stim_on3_1.value);
        CHECK_ERR(err);
    }
    return err;
}


/**
 * @brief 设置波形
 * @return 0-成功，非0-失败
 */
static int __x5_rf_set_pulse(x5_simple_params_t *params)
{
    int err;
    err = __x5_rf_set_simple_feature(params); /* 设置波形形状 */
    CHECK_ERR(err);

    err = __x5_rf_set_range(params); /* 设置幅度 */
    CHECK_ERR(err);

    err = __x5_rf_set_polar(params); /* 设置电极方向 */
    CHECK_ERR(err);
    return err;
}



/**
 * @brief 配置软启动
 */
int x5_rf_config_softstart(x5_program_idx_t prog_idx, x5_softstart_params_t *params)
{
    uint8_t reg_addr = LMS0820X5_REG_2D_SOFTSTART_0 + prog_idx;
    lms0820x5_softstart_0_t reg = {0};

    reg.bits.softstart_en = params->enable ? 1 : 0;
    reg.bits.softstart_inc_value = params->inc_value;
    reg.bits.softstart_step = params->step;

    return x5_cmd_write(reg_addr, reg.value);
}

/**
 * @brief 配置软停止
 */
int x5_rf_config_softstop(uint8_t enable_mask)
{
    lms0820x5_softstop_en_t reg = {0};
    reg.bits.softstop_en = enable_mask & 0x0F;
    return x5_cmd_write(LMS0820X5_REG_31_SOFTSTOP_EN, reg.value);
}


/**
 * @brief 设置软启动
 * @return 0-成功，非0-失败
 */
static int __x5_rf_set_soft(x5_simple_params_t *params)
{
    int err;
//    sotf_time
    return err;
}


/**
 * @brief 配置简单模式刺激
 */

int x5_rf_config_simple(x5_simple_params_t *params)
{
    int err = 0;
    __x5_rf_set_pulse(params);
    err = __x5_rf_start_pulse(X5_SIMPLE_MODE, 1, params->low_power_mode, 1); /* 启动输出 */
    CHECK_ERR(err);

    return err;
}

/**
 * @brief 多程序刺激
 */
int x5_rf_config_more_simple(x5_simple_params_t *params)
{
    int err = 0;
    err = __x5_rf_reset(); //复位芯片  多程序刺激的时候 不复位会有问题 统一复位操作
    CHECK_ERR(err);
    err = __x5_rf_set_pulse(params);
    CHECK_ERR(err);
    err = __x5_rf_start_pulse(X5_SIMPLE_MODE, 1, params->low_power_mode, params->program_cnt); /* 启动输出 */
    CHECK_ERR(err);
    return err;
}

/**
 * @brief 配置爆发模式刺激
 */
int x5_rf_config_burst(x5_simple_params_t *params)
{
    int err = 0;

    __x5_rf_reset(); //复位芯片  多程序刺激的时候 不复位会有问题 统一复位操作
    __x5_rf_set_pulse(params);
    err = __x5_rf_start_pulse(X5_BURST_MODE, params->burst_pulse_cnt, params->low_power_mode, params->program_cnt); /* 启动输出 */
    CHECK_ERR(err);
    return err;
}



