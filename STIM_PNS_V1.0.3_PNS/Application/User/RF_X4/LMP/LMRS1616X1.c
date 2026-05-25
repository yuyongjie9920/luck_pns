///**
// * @file lmrs1616.c
// * @brief LMRS1616X1A 驱动实现
// * @version 2.0
// * @date 2026-04-07
// *
// * 基于测试方案文档《LMRS1616X1A数字寄存器列表_寄存器展开版本.xlsx》实现
// * 支持16通道刺激、多程序配置、软启动/软停止等功能
// */
//
//#include "leadinno_app.h"
//#include "main_user.h"
//#include "crc.h"
//#include "LMRS1616X1.h"
//
///*==============================================================================
// * 调试日志宏
// *============================================================================*/
//
//#if 1
//    #define log(...) logInfo(__VA_ARGS__)
//#else
//    #define log(...) do {} while(0)
//#endif
//
//#define CHECK_ERR(err) if((err) != 0) return -1
//
///*==============================================================================
// * 全局变量
// *============================================================================*/
//
//struct lmrs1616_data lmrs1616 = {0};
//
///*==============================================================================
// * 常量定义
// *============================================================================*/
//
//#define DAC_TO_UA_FACTOR    2439        /**< DAC值转uA系数 2.439 * 1000 */
//#define UA_TO_DAC_FACTOR    410         /**< uA转DAC系数 1000/2.439 ≈ 410 */
//
//// 校准系数定义，基于实际测量调整
//#define US_CALIBRATION_NUMERATOR   2308  // 实际值
//#define US_CALIBRATION_DENOMINATOR 2000  // 理论值
//
///*==============================================================================
// * 转换函数
// *============================================================================*/
//
///**
// * @brief 微秒转换为时钟周期数
// * @param us 微秒数
// * @param clk_div 时钟分频值 (0-62)
// * @return 时钟周期数
// */
//uint32_t lmrs1616_us_to_clk(uint32_t us, uint8_t clk_div)
//{
//    uint32_t clk;
//    // 应用校准系数
//    clk = (uint32_t)((uint64_t)us * US_CALIBRATION_DENOMINATOR / US_CALIBRATION_NUMERATOR) + 1;
//    return clk / (clk_div + 1);
//}
//
///**
// * @brief 时钟周期数转换为微秒
// * @param clk 时钟周期数
// * @param clk_div 时钟分频值 (0-62)
// * @return 微秒数
// */
//uint32_t lmrs1616_clk_to_us(uint32_t clk, uint8_t clk_div)
//{
//    uint32_t us = clk * (clk_div + 1);
//    return (uint32_t)((uint64_t)us * US_CALIBRATION_NUMERATOR / US_CALIBRATION_DENOMINATOR);
//}
//
///**
// * @brief DAC值转换为电流值(mA)
// * @param dac DAC值 (0-4095)
// * @return 电流值(mA)
// */
//uint32_t lmrs1616_dac_to_ma(uint16_t dac)
//{
//    return (dac * DAC_TO_UA_FACTOR + 500) / 1000;
//}
//
///**
// * @brief 电流值(uA)转换为DAC值
// * @param ma_uA 电流值(微安)
// * @return DAC值
// */
//uint16_t lmrs1616_ma_to_dac(uint32_t ma_uA)
//{
//    return (ma_uA * UA_TO_DAC_FACTOR + 500) / 1000;
//}
//
///**
// * @brief 目标电流(uA)转换为DAC值（校准版本）
// * @param target_ua 目标电流(微安)
// * @return DAC值
// */
//uint16_t lmrs1616_ua_to_dac(uint32_t target_ua)
//{
//    // 校准参数
//    const uint32_t cal_dac = 250;      // 校准点的DAC值
//    const uint32_t cal_ua = 10540;     // 校准点的电流值(μA)
//
//    uint32_t dac_value = (target_ua * cal_dac) / cal_ua;
//    if (dac_value > 4095) {
//        dac_value = 4095;
//    }
//    return (uint16_t)dac_value;
//}
//
///*==============================================================================
// * RF通信基础函数
// *============================================================================*/
//
///**
// * @brief 发送Query命令
// * @return 0-成功，非0-失败
// */
//static inline int __lmrs1616_rf_send_query(void)
//{
//    uint8_t bit_len;
//    uint16_t crc, check_crc;
//    uint8_t send_data[5] = {0x80,0,0,0,0};
//    uint8_t raw_data[8] = {0};
//    uint8_t uid = 0;
//
//    send_data[0] |= (uid >> 12) & 0x0F;    // UID高4位 -> bytes[0]低4位
//    send_data[1] = (uid >> 4) & 0xFF;      // UID中间8位 -> bytes[1]
//    send_data[2] = (uid & 0x0F) << 4;      // UID低4位 -> bytes[2]高4位
//
//    crc = crc16Bitwise(send_data, 20);
//
//    send_data[20/8] |= (crc >> 12) & 0x0F; // CRC高4位到bytes[2]低4位
//    send_data[20/8+1] = (crc >> 4) & 0xFF; // CRC中间8位到bytes[3]
//    send_data[20/8+2] = (crc & 0x0F) << 4; // CRC低4位到bytes[4]高4位
//
//    rf_tx_data(send_data, 36); // 命令格式: 8个4bit = 32bit
//
//    int err = rf_rx_data(raw_data, &bit_len);
//    if (err != 0) {
//        log("query broadcast rx error: %d", err);
//        return err;
//    }
//
//    if(crc16Bitwise(raw_data, 8 * 2) != ((uint16_t)raw_data[2]<<8 | raw_data[3])) {
//        logInfo("query crc fail");
//        return -1;
//    }
//
//    lead.lmrs1616_query.value = (uint16_t)raw_data[0]<<8 | raw_data[1];
//
//    return 0;
//}
//
///**
// * @brief 自动功率轮询Query
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_send_query(void)
//{
//    for (uint8_t power = 0; power < 15; power++) {
//        SET_RF_POWER(power);
//        if (__lmrs1616_rf_send_query() == 0) {
//            logInfo("query ok power = %d", power);
//            return RF_OK;
//        } else {
//            logInfo("query fail power is %d", power);
//        }
//    }
//    log("broadcast query failed: all power levels");
//    return RF_FAIL;
//}
//
///**
// * @brief 指定功率发送Query
// * @param power 功率等级 (0-15)
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_send_query_with_power(uint8_t power)
//{
//    int err;
//    if (power > 15) power = 15;
//    SET_RF_POWER(power);
//    err = __lmrs1616_rf_send_query();
//    return err;
//}
//
///**
// * @brief 所有功率点测试
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_send_query_all_power(void)
//{
//    int err = 0;
//    for (uint8_t power = 0; power < 15; power++) {
//        SET_RF_POWER(power);
//        err = __lmrs1616_rf_send_query();
//        log("power %2d: %s", power, err == 0 ? "OK" : "FAIL");
//    }
//    return 0;
//}
//
///*==============================================================================
// * 寄存器读写函数
// *============================================================================*/
//
///**
// * @brief 发送写命令
// * @param reg 寄存器地址
// * @param reg_data 要写入的数据
// * @return 0-成功，非0-失败
// */
//static inline int __lmrs1616_rf_send_cmd_write(uint8_t reg, uint16_t reg_data)
//{
//    lmrs1616_rf_write_send_data_t write_data;
//    lmrs1616_rf_write_dec_data_t tx;
//    uint8_t rx_data[8];
//    uint16_t crc;
//    uint8_t nbits = 0;
//    uint16_t uid;
//
//    SET_RF_POWER(lead.set_query_power);
//
//    write_data.cmd = 0xc3;
//    write_data.reg = reg;
//    write_data.reg_data = ((reg_data & 0xff00) >> 8) | ((reg_data & 0x00ff) << 8);
//    uid = lead.lmrs1616_query.bits.efuse_id;
//    uid = (uid << 8) | (uid >> 8);
//    write_data.uid = uid;
//
//    crc = crc16Bitwise((uint8_t *)&write_data,
//                      (sizeof(write_data.cmd) + sizeof(write_data.reg) +
//                       sizeof(write_data.reg_data) + sizeof(write_data.uid)) * 8);
//    write_data.crc_h = crc >> 8;
//    write_data.crc_l = crc;
//
//    rf_tx_data((uint8_t *)&write_data, sizeof(write_data) * 8);
//
//    int err = rf_rx_data(rx_data, &nbits);
//    if(err != 0) {
//        logInfo("RX err = %d", err);
//    }
//
//    tx.crc = ((rx_data[2] & 0x7F) << 9) | (rx_data[3] << 1) | ((rx_data[4] >> 7) & 0x01);
//
//    if((nbits/8) > 2) {
//        crc = crc16Bitwise(rx_data, nbits - 16);
//        if(crc == tx.crc) {
//            log("write reg 0x%02x->0x%04x ok", reg, reg_data);
//            err = 0;
//        } else {
//            err = -1;
//        }
//    } else {
//        err = -1;
//    }
//
//    if(err != 0) {
//        log("write reg 0x%02x->0x%04x fail err = %d", reg, reg_data, err);
//    }
//    return err;
//}
//
///**
// * @brief 发送读命令
// * @param reg 寄存器地址
// * @param get_reg_data 读取到的数据指针
// * @return 0-成功，非0-失败
// */
//static inline int __lmrs1616_rf_send_cmd_read(uint8_t reg, uint16_t *get_reg_data)
//{
//    lmrs1616_rf_read_send_data_t send_data;
//    lmrs1616_rf_rx_dec_data_t rx;
//    uint8_t rx_data[8];
//    uint16_t crc;
//    uint8_t nbits = 0;
//
//    send_data.cmd = 0xc2; // 读命令
//    send_data.reg = reg;
//    uint16_t uid = lead.lmrs1616_query.bits.efuse_id;
//    uid = (uid << 8) | (uid >> 8);
//    send_data.uid = uid;
//
//    crc = crc16Bitwise((uint8_t *)&send_data,
//                      (sizeof(send_data.cmd) + sizeof(send_data.reg) +
//                       sizeof(send_data.uid)) * 8);
//    send_data.crc_h = crc >> 8;
//    send_data.crc_l = crc;
//
//    rf_tx_data((uint8_t *)&send_data, sizeof(send_data) * 8);
//
//    int err = rf_rx_data(rx_data, &nbits);
//
//    // 解析返回数据
//    rx.header = (rx_data[0] >> 7) & 0x01;
//    rx.reg_data = ((rx_data[0] & 0x7F) << 9) | (rx_data[1] << 1) | ((rx_data[2] >> 7) & 0x01);
//    rx.rn = ((rx_data[2] & 0x7F) << 9) | (rx_data[3] << 1) | ((rx_data[4] >> 7) & 0x01);
//    rx.crc = ((rx_data[4] & 0x7F) << 9) | (rx_data[5] << 1) | ((rx_data[6] >> 7) & 0x01);
//
//    if((nbits/8) > 2) {
//        crc = crc16Bitwise(rx_data, nbits - 16);
//        if(crc == rx.crc) {
//            *get_reg_data = rx.reg_data;
//            err = 0;
//            log("read reg 0x%02x<-0x%04x ok", reg, rx.reg_data);
//        } else {
//            err = -1;
//            log("read reg 0x%02x fail", reg);
//        }
//    } else {
//        err = -1;
//        log("read reg 0x%02x fail", reg);
//    }
//
//    SET_RF_POWER(lead.stim_pulse_power);
//    return err;
//}
//
///**
// * @brief 带重试的寄存器写操作
// * @param reg 寄存器地址
// * @param reg_data 要写入的数据
// * @return 0-成功，非0-失败
// */
//int lmrs1616_cmd_write(uint8_t reg, uint16_t reg_data)
//{
//    int err = 0;
//    int retry = 10;
//
//    do {
//        err = __lmrs1616_rf_send_cmd_write(reg, reg_data);
//        if (err != 0) {
//            lmrs1616_rf_send_query();  // 重新查询
//        }
//    } while (err != 0 && retry-- > 0);
//    return (retry > 0) ? 0 : -1;
//}
//
///**
// * @brief 带重试的寄存器读操作
// * @param reg 寄存器地址
// * @param reg_data 读取数据的指针
// * @return 0-成功，非0-失败
// */
//int lmrs1616_cmd_read(uint8_t reg, uint16_t *reg_data)
//{
//    int err = 0;
//    uint8_t retry = 10;
//
//    do {
//        err = __lmrs1616_rf_send_cmd_read(reg, reg_data);
//        if (err != 0) {
//            lmrs1616_rf_send_query();  // 重新查询
//        }
//    } while (err != 0 && retry-- > 0);
//
//    return (retry > 0) ? 0 : -1;
//}
//
///*==============================================================================
// * 测量功能函数
// *============================================================================*/
//
///**
// * @brief 获取整流电压
// * @param vrecp 电压值数组指针
// * @param delay_count 延迟测量的刺激周期数
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_get_vrecp(float *vrecp, uint16_t delay_count)
//{
//    int err = 0;
//    uint16_t vdd_low = 0, vdd_high = 0;
//    lmrs1616x1a_volt_delay_sample_t volt_delay = {0};
//    lmrs1616x1a_ctrlreg0_t ctrlreg0 = {0};
//
//    // 配置电压延迟采样
//    volt_delay.bits.sample_delay = delay_count;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_65_VOLT_DELAY_SAMPLE, volt_delay.value);
//    CHECK_ERR(err);
//
//    if (delay_count > 0) {
//        SET_RF_POWER(0);
//        delay_ms(10);
//        SET_RF_POWER(lead.set_query_power);
//    }
//
//    // 启动电压测量
//    ctrlreg0.bits.start_en = LMRS1616_MEAS_ENABLE_VOLT;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//
//    delay_ms(5);  // 等待测量完成
//
//    // 读取测量结果
//    err = lmrs1616_cmd_read(LMRS1616X1A_REG_72_VDD_LOW, &vdd_low);
//    CHECK_ERR(err);
//    err = lmrs1616_cmd_read(LMRS1616X1A_REG_73_VDD_HIGH, &vdd_high);
//    CHECK_ERR(err);
//
//    // 清零测量使能
//    ctrlreg0.bits.start_en = LMRS1616_MEAS_ENABLE_NONE;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//
//    // ADC值转换为电压
//    const float ADC_OFFSET = 98.0f;
//    const float ADC_SCALE = 122.0f;
//
//    uint16_t data_low = vdd_low & 0x0FFF;
//    uint16_t data_high = vdd_high & 0x0FFF;
//
//    float v_min = (data_low > ADC_OFFSET) ? (data_low - ADC_OFFSET) / ADC_SCALE : 0;
//    float v_max = (data_high > ADC_OFFSET) ? (data_high - ADC_OFFSET) / ADC_SCALE : 0;
//
//    v_min = (float)((int)(v_min * 100 + 0.5f)) / 100.0f;
//    v_max = (float)((int)(v_max * 100 + 0.5f)) / 100.0f;
//
//    if (delay_count > 0) {
//        vrecp[0] = v_min;
//        vrecp[1] = v_max;
//        log("VRECP: min=%.2fV, max=%.2fV", v_min, v_max);
//    } else {
//        *vrecp = v_min;
//        log("VRECP: %.2fV", v_min);
//    }
//
//    return err;
//}
//
///**
// * @brief 获取阻抗
// * @param channel_sel 通道选择 (0-7)
// * @param res 阻抗值数组指针 (16个通道)
// * @param osc 时钟trim选择
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_get_res(uint8_t channel_sel, uint16_t *res, uint8_t osc)
//{
//    int err = 0;
//    uint16_t res_raw[16] = {0};
//    lmrs1616x1a_ctrlreg0_t ctrlreg0 = {0};
//    lmrs1616x1a_imp_data_t imp_data[16];
//    float f_mhz;
//    float res_cal;
//
//    // 启动阻抗测量
//    ctrlreg0.bits.start_en = LMRS1616_MEAS_ENABLE_IMP;
//    ctrlreg0.bits.imp_channel_sel = channel_sel;
//    ctrlreg0.bits.imp_test_trim_osc = osc;
//
//    f_mhz = (ctrlreg0.bits.imp_test_trim_osc == 0) ? 0.9f : 1.24f;
//
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//
//    delay_ms(10);
//
//    // 读取16通道数据
//    for (uint8_t i = 0; i < 16; i++) {
//        err = lmrs1616_cmd_read(LMRS1616X1A_REG_6A_IMP_DATA1 + i, &imp_data[i].value);
//        res_raw[i] = imp_data[i].bits.imp_data & 0x0FFF;
//    }
//
//    // 清零使能
//    ctrlreg0.bits.start_en = 0;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//
//    // 根据公式计算电阻值
//    for (int i = 0; i < 16; i++) {
//        if (i == 0) {  // 参考通道
//            res[i] = res_raw[i];
//            log("ref channel = %d (raw=0x%03x)", res_raw[i], res_raw[i]);
//            continue;
//        }
//        res_cal = (res_raw[i] * 2.7717f - 5722.0f) / f_mhz;
//        if (res_cal < 0) {
//            res_cal = 0;
//        }
//        res[i] = (uint16_t)(res_cal + 0.5f);
//        log("R[%d-%d] = %dR (raw=0x%03x, cal=%.2f)",
//            channel_sel, (channel_sel + i) % 16, res[i], res_raw[i], res_cal);
//    }
//
//    return err;
//}
//
///**
// * @brief 获取温度
// * @param temp 温度值指针
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_get_temp(float *temp)
//{
//    int err = 0;
//    uint16_t temp_adc = 0;
//    lmrs1616x1a_ctrlreg0_t ctrlreg0 = {0};
//
//    ctrlreg0.bits.start_en = LMRS1616_MEAS_ENABLE_TEMP;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//
//    delay_ms(10);
//    err = lmrs1616_cmd_read(LMRS1616X1A_REG_74_TEMP_DATA, &temp_adc);
//    CHECK_ERR(err);
//
//    ctrlreg0.bits.start_en = LMRS1616_MEAS_ENABLE_NONE;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//
//    float temperature = 65.856f - (float)(temp_adc & 0x0FFF) / 75.188f;
//    *temp = roundf(temperature * 10.0f) / 10.0f;
//    logInfo("temp = %.1f", *temp);
//
//    return err;
//}
//
///**
// * @brief 获取电流
// * @param current_p 正向电流指针
// * @param current_n 负向电流指针
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_get_current(float *current_p, float *current_n)
//{
//    int err = 0;
//    lmrs1616x1a_ctrlreg0_t ctrlreg0 = {0};
//    uint16_t data_p = 0, data_n = 0;
//
//    // 使能电流测量
//    ctrlreg0.bits.current_measure = 1;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//
//    delay_us(500);  // 等待模拟采样完成
//
//    // 读取电流值
//    err = lmrs1616_cmd_read(LMRS1616X1A_REG_7E_CURRENT_DATA_PLUS, &data_p);
//    CHECK_ERR(err);
//    err = lmrs1616_cmd_read(LMRS1616X1A_REG_7D_CURRENT_DATA_NEG, &data_n);
//    CHECK_ERR(err);
//
//    // 清零使能
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_68_CTRLREG0, 0);
//    CHECK_ERR(err);
//
//    *current_p = lmrs1616_dac_to_ma(data_p & 0x0FFF) / 1000.0f;
//    *current_n = lmrs1616_dac_to_ma(data_n & 0x0FFF) / 1000.0f;
//
//    log("CURRENT: P=%.2fmA (0x%03x), N=%.2fmA (0x%03x)",
//        *current_p, data_p & 0x0FFF, *current_n, data_n & 0x0FFF);
//    return err;
//}
//
///*==============================================================================
// * eFuse操作函数
// *============================================================================*/
//
///**
// * @brief 写eFuse
// * @param efuse 24位efuse值
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_write_efuse(uint32_t efuse)
//{
//    int err = 0;
//    uint32_t efuse_read = 0;
//
//    efuse &= 0xFFFFFF;  // 24位有效
//
//    rf_pwm_close();
//
//    lmrs1616x1a_efuse_w_data0_t wdata0 = {0};
//    lmrs1616x1a_efuse_w_data1_t wdata1 = {0};
//    lmrs1616x1a_efuse_tburn_t tburn = {0};
//    lmrs1616x1a_ldo_trim_t trim = {0};
//
//    // 1. 先清零数据有效位
//    wdata1.bits.efuse_data_valid = 0;
//    wdata1.bits.efuse_data_enable = 0x3F;  // 烧录所有bit
//    wdata1.bits.efuse_w_data_high = (efuse >> 16) & 0xFF;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_10_EFUSE_W_DATA0, wdata1.value);
//    CHECK_ERR(err);
//
//    // 2. 配置烧写时间
//    tburn.value = 0xFFFF;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_14_EFUSE_TBURN, tburn.value);
//    CHECK_ERR(err);
//
//    trim.value = 6;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_64_LDO_TRIM, trim.value);
//    CHECK_ERR(err);
//
//    // 3. 写入低16位数据
//    wdata0.value = efuse & 0xFFFF;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_10_EFUSE_W_DATA0, wdata0.value);
//    CHECK_ERR(err);
//
//    // 4. 写入高8位并启动烧录
//    wdata1.bits.efuse_data_valid = 1;  // 上升沿启动烧写
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_11_EFUSE_W_DATA1, wdata1.value);
//    CHECK_ERR(err);
//
//    // 5. 最大功率供电
//    SET_RF_POWER(POWER_MAX);
//    log("Waiting for efuse write (3 seconds)...");
//    delay_ms(3000);
//
//    // 6. 复位芯片
//    rf_reset_chip();
//
//    // 7. 重新广播查询获取新efuse_id
//    err = lmrs1616_rf_send_query();
//    CHECK_ERR(err);
//
//    // 8. 读取验证
//    err = lmrs1616_rf_read_efuse(&efuse_read);
//    CHECK_ERR(err);
//
//    if (efuse_read == efuse) {
//        log("EFUSE write OK: 0x%06x", efuse_read);
//    } else {
//        log("EFUSE verify FAIL: write 0x%06x, read 0x%06x", efuse, efuse_read);
//    }
//
//    return err;
//}
//
///**
// * @brief 读eFuse
// * @param efuse efuse值指针
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_read_efuse(uint32_t *efuse)
//{
//    int err = 0;
//    uint16_t data0 = 0, data1 = 0;
//
//    err = lmrs1616_cmd_read(LMRS1616X1A_REG_12_EFUSE_R_DATA0, &data0);
//    err = lmrs1616_cmd_read(LMRS1616X1A_REG_13_EFUSE_R_DATA1, &data1);
//
//    *efuse = data0 | ((data1 & 0xFF) << 16);
//    return err;
//}
//
///*==============================================================================
// * 刺激控制函数
// *============================================================================*/
//
///**
// * @brief 停止刺激
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_stop_pulse(void)
//{
//    lmrs1616x1a_stim_mode_t stim_mode = {0};
//    stim_mode.bits.stim_stop = 1;
//    return lmrs1616_cmd_write(LMRS1616X1A_REG_06_STIM_MODE, stim_mode.value);
//}
//
///**
// * @brief 软件复位
// * @return 0-成功，非0-失败
// */
//int __lmrs1616_rf_reset(void)
//{
//    int err = 0;
//    lmrs1616x1a_trimdata_ctrl0_t ctrl0;
//    ctrl0.value = 0xc0;
//    ctrl0.bits.cmd_rst = 1;
//
//    __lmrs1616_rf_send_cmd_write(LMRS1616X1A_REG_60_TRIMDATA_CTRL0, ctrl0.value);
//    __lmrs1616_rf_send_cmd_write(LMRS1616X1A_REG_60_TRIMDATA_CTRL0, ctrl0.value);
//    __lmrs1616_rf_send_cmd_write(LMRS1616X1A_REG_60_TRIMDATA_CTRL0, ctrl0.value);
//
//    delay_ms(200);
//    ctrl0.bits.cmd_rst = 0;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_60_TRIMDATA_CTRL0, ctrl0.value);
//    return err;
//}
//
///**
// * @brief 启动脉冲输出(内部函数)
// * @param mode 刺激模式
// * @param pulse_cnt 脉冲个数
// * @param low_power 低功耗模式
// * @param program_cnt 程序个数
// * @return 0-成功，非0-失败
// */
//static int __lmrs1616_rf_start_pulse(uint8_t mode, uint8_t pulse_cnt, uint8_t low_power, uint8_t program_cnt, uint8_t div)
//{
//    int err = 0;
//    lmrs1616x1a_stim_mode_t stim_mode = {0};
//
//    stim_mode.bits.stim_start = 1;
//    stim_mode.bits.para_vld = 1;
//    stim_mode.bits.stim_stop = 0;
//    stim_mode.bits.stim_mode = mode;
//    stim_mode.bits.stim_low_power = low_power;
//    stim_mode.bits.program_cnt = program_cnt;
//    stim_mode.bits.stim_clk_div_value = div;//;
//    stim_mode.bits.stim_auto_stop = 1;
//
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_06_STIM_MODE, stim_mode.value);
//    CHECK_ERR(err);
//
//    // 清除启动标志，开始刺激输出
//    stim_mode.bits.stim_stop = 0;
//    stim_mode.bits.para_vld = 0;
//    stim_mode.bits.stim_start = 0;
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_06_STIM_MODE, stim_mode.value);
//
//    return err;
//}
//
///*==============================================================================
// * 波形配置函数
// *============================================================================*/
//
///**
// * @brief 设置刺激波形时间
// * @param params 刺激参数
// * @return 0-成功，非0-失败
// */
//static int __lmrs1616_rf_set_simple_feature(lmrs1616_simple_params_t *params)
//{
//    uint16_t tphase1[4], tphase2[4], tphase3[4];
//    uint16_t tinterphase1[4], tinterphase2[4], tinterpulse[4];
//    uint32_t total_clk[4];
//    uint32_t tinterpulse_save, burst_tinter_save;
//    int err = 0;
//
//start:
//    for(uint8_t i = 0; i < (params->program_cnt + 1); i++) {
//        tphase1[i] = lmrs1616_us_to_clk(params->program[i].tphase1, params->phase.stim_clk_div);
//        tphase2[i] = lmrs1616_us_to_clk(params->program[i].tphase2, params->phase.stim_clk_div);
//        tphase3[i] = lmrs1616_us_to_clk(params->program[i].tphase3, params->phase.stim_clk_div);
//        tinterphase1[i] = lmrs1616_us_to_clk(params->program[i].tinterphase1, params->phase.stim_clk_div);
//        tinterphase2[i] = lmrs1616_us_to_clk(params->program[i].tinterphase2, params->phase.stim_clk_div);
//
//        total_clk[i] = lmrs1616_us_to_clk(1000000 / params->base_freq, params->phase.stim_clk_div) /
//                      params->program[i].multiple - 1;
//
//        if(params->pulse_mode == LMRS1616_SIMPLE_MODE) {
//            logInfo("total_clk[%d] = %d", i, total_clk[i]);
//            tinterpulse_save = total_clk[i] - tphase1[i] - tphase2[i] -
//                              tinterphase1[i] - tinterphase2[i] - tphase3[i];
//            if(tinterpulse_save >= 0xffff) {
//                params->phase.stim_clk_div++;
//                if(params->phase.stim_clk_div > 63) {
//                    return 0;
//                }
//                logInfo("[error hz moro min] !!!!!!!!!");
//                goto start;
//            } else {
//                tinterpulse[i] = tinterpulse_save;
//            }
//        } else if(params->pulse_mode == LMRS1616_BURST_MODE) {
//            burst_tinter_save = total_clk[0] - (tphase1[0] + tphase2[0] + tphase3[0] +
//                              tinterphase1[0] + tinterphase2[0] + tinterpulse[0]) * params->burst_pulse_cnt;
//            if(burst_tinter_save >= 0xffff) {
//                params->phase.stim_clk_div++;
//                if(params->phase.stim_clk_div > 63) {
//                    return 0;
//                }
//                logInfo("[error hz moro min] !!!!!!!!!");
//                goto start;
//            }
//            lmrs1616x1a_tinterburst_t burst = {0};
//            burst.value = burst_tinter_save;
//            err = lmrs1616_cmd_write(LMRS1616X1A_REG_08_TINTERBURST, burst.value);
//        }
//    }
//
//    // 配置各程序的波形时间寄存器
//    // 程序0
//    lmrs1616x1a_tphase1_0_t tphase1_0 = {0};
//    lmrs1616x1a_tphase2_0_t tphase2_0 = {0};
//    lmrs1616x1a_tphase3_0_t tphase3_0 = {0};
//    lmrs1616x1a_tinterphase0_t tinterphase_0 = {0};
//    lmrs1616x1a_tinterpulse0_t tinterpulse_0 = {0};
//
//    tphase1_0.bits.tphase1 = tphase1[0];
//    tphase2_0.bits.tphase2 = tphase2[0];
//    tphase3_0.bits.tphase3 = tphase3[0];
//    tinterphase_0.bits.tinterphase1 = tinterphase1[0];
//    tinterphase_0.bits.tinterphase2 = tinterphase2[0];
//    tinterpulse_0.bits.tinterpulse = tinterpulse[0];
//
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_00_TPHASE1_0, tphase1_0.value);
//    CHECK_ERR(err);
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_01_TPHASE2_0, tphase2_0.value);
//    CHECK_ERR(err);
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_03_TPHASE3_0, tphase3_0.value);
//    CHECK_ERR(err);
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_02_TINTERPHASE0, tinterphase_0.value);
//    CHECK_ERR(err);
//    err = lmrs1616_cmd_write(LMRS1616X1A_REG_04_TINTERPULSE0, tinterpulse_0.value);
//    CHECK_ERR(err);
//
//    // 其他程序的时间配置类似，根据文档中的寄存器地址配置
//    // 这里省略详细实现，实际使用时需要根据寄存器文档完整实现
//
//    return err;
//}
//
///**
// * @brief 设置刺激幅度
// * @param params 刺激参数
// * @return 0-成功，非0-失败
// */
//static int __lmrs1616_rf_set_range(lmrs1616_simple_params_t *params)
//{
//    int err = 0;
//    uint16_t amp_p[4][16], amp_n[4][16];  // 4个程序，每个程序16个通道
//
//    // 根据文档中的寄存器映射配置各通道的DAC值
//    // 这里需要根据文档中0x15-0x1E的寄存器定义来配置
//    // 由于寄存器较多，这里只给出框架，实际需要完整实现
//
//    for(uint8_t prog = 0; prog < (params->program_cnt + 1); prog++) {
//        for(uint8_t ch = 0; ch < 16; ch++) {
//            amp_p[prog][ch] = lmrs1616_ua_to_dac(params->program[prog].amplitude_p);
//            amp_n[prog][ch] = lmrs1616_ua_to_dac(params->program[prog].amplitude_n);
//
//            // 根据通道和程序索引配置对应的寄存器
//            // 具体寄存器地址映射参考文档
//        }
//    }
//
//    return err;
//}
//
///**
// * @brief 设置电极极性配置
// * @param params 刺激参数
// * @return 0-成功，非0-失败
// */
//static int __lmrs1616_rf_set_polar(lmrs1616_simple_params_t *params)
//{
//    int err = 0;
//    uint16_t stim0[4], stim1[4];
//
//    for(uint8_t i = 0; i < (params->program_cnt + 1); i++) {
//        stim0[i] = LMRS1616X1A_CALC_STIM_ON0(params->program[i].polar.dir, params->program[i].polar.on);
//        stim1[i] = LMRS1616X1A_CALC_STIM_ON1(params->program[i].polar.dir, params->program[i].polar.on);
//
//        // 配置对应的电极开关寄存器
//        // 程序0使用0x1F, 0x20
//        // 其他程序使用对应的寄存器地址
//    }
//
//    return err;
//}
//
///**
// * @brief 设置波形参数
// * @param params 刺激参数
// * @return 0-成功，非0-失败
// */
//static int __lmrs1616_rf_set_pulse(lmrs1616_simple_params_t *params)
//{
//    int err;
//    err = __lmrs1616_rf_set_simple_feature(params);  // 设置波形时间
//    CHECK_ERR(err);
//
//    err = __lmrs1616_rf_set_range(params);  // 设置幅度
//    CHECK_ERR(err);
//
//    err = __lmrs1616_rf_set_polar(params);  // 设置电极方向
//    CHECK_ERR(err);
//
//    return err;
//}
//
///*==============================================================================
// * 软启动/软停止配置
// *============================================================================*/
//
///**
// * @brief 配置软启动
// * @param prog_idx 程序索引
// * @param params 软启动参数
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_config_softstart(lmrs1616_program_idx_t prog_idx, lmrs1616_softstart_params_t *params)
//{
//    uint8_t reg_addr = LMRS1616X1A_REG_2D_SOFTSTART_0 + prog_idx;
//    lmrs1616x1a_softstart_0_t reg = {0};
//
//    reg.bits.softstart_en = params->enable ? 1 : 0;
//    reg.bits.softstart_inc_value = params->inc_value;
//    reg.bits.softstart_step = params->step;
//
//    return lmrs1616_cmd_write(reg_addr, reg.value);
//}
//
///**
// * @brief 配置软停止
// * @param enable_mask 使能掩码
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_config_softstop(uint8_t enable_mask)
//{
//    lmrs1616x1a_softstop_en_t reg = {0};
//    reg.bits.softstop_en = enable_mask & 0x0F;
//    return lmrs1616_cmd_write(LMRS1616X1A_REG_31_SOFTSTOP_EN, reg.value);
//}
//
///*==============================================================================
// * 刺激配置主函数
// *============================================================================*/
//
///**
// * @brief 配置简单模式刺激
// * @param params 刺激参数
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_config_simple(lmrs1616_simple_params_t *params)
//{
//    int err = 0;
//    __lmrs1616_rf_set_pulse(params);
//    err = __lmrs1616_rf_start_pulse(LMRS1616_SIMPLE_MODE, 1, params->low_power_mode, 1, params->phase.stim_clk_div);
//    CHECK_ERR(err);
//    return err;
//}
//
///**
// * @brief 配置多程序刺激
// * @param params 刺激参数
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_config_more_simple(lmrs1616_simple_params_t *params)
//{
//    int err = 0;
//    err = __lmrs1616_rf_reset();  // 复位芯片
//    CHECK_ERR(err);
//    err = __lmrs1616_rf_set_pulse(params);
//    CHECK_ERR(err);
//    err = __lmrs1616_rf_start_pulse(LMRS1616_SIMPLE_MODE, 1, params->low_power_mode, params->program_cnt, params->phase.stim_clk_div);
//    CHECK_ERR(err);
//    return err;
//}
//
///**
// * @brief 配置爆发模式刺激
// * @param params 刺激参数
// * @return 0-成功，非0-失败
// */
//int lmrs1616_rf_config_burst(lmrs1616_simple_params_t *params)
//{
//    int err = 0;
//    __lmrs1616_rf_reset();  // 复位芯片
//    __lmrs1616_rf_set_pulse(params);
//    err = __lmrs1616_rf_start_pulse(LMRS1616_BURST_MODE, params->burst_pulse_cnt, params->low_power_mode, params->program_cnt, params->phase.stim_clk_div);
//    CHECK_ERR(err);
//    return err;
//}
