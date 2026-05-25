///**
// * @file lmp1616x1.c
// * @brief LMP1616X1 驱动实现
// * @version 1.0
// * @date 2026-04-07
// */
//
//#include "leadinno_app.h"
//#include "main_user.h"
//#include "crc.h"
//#include "LMP1616X1.h"
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
//struct lmp1616x1_data lmp1616x1 = {0};
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
// * 工具函数
// *============================================================================*/
//
///**
// * @brief 微秒转时钟周期
// */
//uint32_t lmp1616x1_us_to_clk(uint32_t us, uint8_t clk_div)
//{
//    uint32_t clk;
//    // 应用校准系数
//    clk = (uint32_t)((uint64_t)us * US_CALIBRATION_DENOMINATOR / US_CALIBRATION_NUMERATOR) + 1;
//    return clk / (clk_div + 1);
//}
//
///**
// * @brief 时钟周期转微秒
// */
//uint32_t lmp1616x1_clk_to_us(uint32_t clk, uint8_t clk_div)
//{
//    return (clk * (clk_div + 1) * US_CALIBRATION_NUMERATOR) / US_CALIBRATION_DENOMINATOR;
//}
//
///**
// * @brief DAC值转换为电流值(uA)
// */
//uint32_t lmp1616x1_dac_to_ma(uint16_t dac)
//{
//    return (dac * DAC_TO_UA_FACTOR + 500) / 1000;
//}
//
///**
// * @brief 电流值(uA)转换为DAC值
// */
//uint16_t lmp1616x1_ma_to_dac(uint32_t ma_uA)
//{
//    return (ma_uA * UA_TO_DAC_FACTOR + 500) / 1000;
//}
//
///**
// * @brief uA值转换为DAC值（校准版本）
// */
//uint16_t lmp1616x1_ua_to_dac(uint32_t target_ua)
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
// */
//int __lmp1616x1_rf_send_query(void)
//{
//    uint8_t bit_len;
//    uint16_t crc, check_crc;
//    uint8_t send_data[5] = {0x80,0,0,0,0};
//    uint8_t raw_data[8] = {0};
//    uint8_t *out_data;
//    uint8_t uid = 0;
//    send_data[0] |= (uid >> 12) & 0x0F;    // UID高4位
//    send_data[1] = (uid >> 4) & 0xFF;      // UID中间8位
//    send_data[2] = (uid & 0x0F) << 4;      // UID低4位
//
//    crc =  crc16Bitwise(send_data, 20);
//
//    send_data[20/8] |= (crc >> 12) & 0x0F; // CRC高4位
//    send_data[20/8+1] = (crc >> 4) & 0xFF; // CRC中间8位
//    send_data[20/8+2] = (crc & 0x0F) << 4; // CRC低4位
//
//    rf_tx_data(send_data, 36); // cmd query is 8 4bit uid 16bit crc 16bit
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
//    // 保存查询状态
//    lmp1616x1.last_query_status.value = (uint16_t)raw_data[0]<<8 | raw_data[1];
//    lmp1616x1.efuse_id = lmp1616x1.last_query_status.bits.efuse_id_low;
//
//    return 0;
//}
//
///**
// * @brief 轮询Query
// */
//int lmp1616x1_rf_send_query(void)
//{
//    for (uint8_t power = 0; power < 15; power++) {
//        SET_RF_POWER(power);
//        if (__lmp1616x1_rf_send_query() == 0) {
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
// */
//int lmp1616x1_rf_send_query_with_power(uint8_t power)
//{
//    int err;
//    if (power > 15) power = 15;
//    SET_RF_POWER(power);
//    err = __lmp1616x1_rf_send_query();
//    return err;
//}
//
///**
// * @brief 所有功率点测试
// */
//int lmp1616x1_rf_send_query_all_power(void)
//{
//    int err = 0;
//    for (uint8_t power = 0; power < 15; power++) {
//        SET_RF_POWER(power);
//        err = __lmp1616x1_rf_send_query();
//        log("power %2d: %s", power, err == 0 ? "OK" : "FAIL");
//    }
//    return 0;
//}
//
///**
// * @brief 发送写命令
// */
//static inline int __lmp1616x1_rf_send_cmd_write(uint8_t reg, uint16_t reg_data)
//{
//    SET_RF_POWER(lead.set_query_power);
//
//    lmp1616x1_rf_write_send_data_t write_data;
//    uint8_t rx_data[8];
//    uint16_t crc;
//    uint8_t nbits = 0;
//    uint16_t uid;
//
//    write_data.cmd = 0xc3;
//    write_data.reg = reg;
//    write_data.reg_data = ((reg_data & 0xff00) >> 8) | ((reg_data & 0x00ff) << 8);
//    uid = lmp1616x1.last_query_status.bits.efuse_id_low;
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
//        return err;
//    }
//
//    if((nbits/8) > 2) {
//        crc = crc16Bitwise(rx_data, nbits - 16);
//        uint16_t rx_crc = ((rx_data[2] & 0x7F) << 9) | (rx_data[3] << 1) | ((rx_data[4] >> 7) & 0x01);
//        if(crc == rx_crc) {
//            log("write reg 0x%x->0x%04x ok", reg, reg_data);
//            err = 0;
//        } else {
//            err = -1;
//            log("write reg 0x%x->0x%x crc fail", reg, reg_data);
//        }
//    } else {
//        err = -1;
//        log("write reg 0x%x->0x%x rx data too short", reg, reg_data);
//    }
//
//    return err;
//}
//
///**
// * @brief 发送读命令
// */
//static inline int __lmp1616x1_rf_send_cmd_read(uint8_t reg, uint16_t *get_reg_data)
//{
//    lmp1616x1_rf_read_send_data_t send_data;
//    uint8_t rx_data[8];
//    uint16_t crc;
//    uint8_t nbits = 0;
//    uint16_t uid;
//
//    send_data.cmd = 0xc2; // 读命令
//    send_data.reg = reg;
//    uid = lmp1616x1.last_query_status.bits.efuse_id_low;
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
//    if(err != 0) {
//        log("rf_rx_data err = %d", err);
//        return err;
//    }
//
//    if((nbits/8) > 2) {
//        // 解析返回数据
//        uint8_t header = (rx_data[0] >> 7) & 0x01;
//        uint16_t reg_data = ((rx_data[0] & 0x7F) << 9) | (rx_data[1] << 1) | ((rx_data[2] >> 7) & 0x01);
//        uint16_t rn = ((rx_data[2] & 0x7F) << 9) | (rx_data[3] << 1) | ((rx_data[4] >> 7) & 0x01);
//        uint16_t rx_crc = ((rx_data[4] & 0x7F) << 9) | (rx_data[5] << 1) | ((rx_data[6] >> 7) & 0x01);
//
//        crc = crc16Bitwise(rx_data, nbits - 16);
//        if(crc == rx_crc) {
//            *get_reg_data = reg_data;
//            err = 0;
//            log("read reg 0x%x<-0x%04x ok", reg, reg_data);
//        } else {
//            err = -1;
//            log("read reg 0x%x crc fail", reg);
//        }
//    } else {
//        err = -1;
//        log("read reg 0x%x rx data too short", reg);
//    }
//
//    SET_RF_POWER(lead.stim_pulse_power);
//    return err;
//}
//
///*==============================================================================
// * 带重试的寄存器读写
// *============================================================================*/
//
///**
// * @brief 写寄存器（带重试）
// */
//int lmp1616x1_cmd_write(uint8_t reg, uint16_t reg_data)
//{
//    int err = 0;
//    int retry = 10;
//
//    do {
//        err = __lmp1616x1_rf_send_cmd_write(reg, reg_data);
//        if (err != 0) {
//            lmp1616x1_rf_send_query();  /* 重新查询 */
//        }
//    } while (err != 0 && retry-- > 0);
//
//    return (retry > 0) ? 0 : -1;
//}
//
///**
// * @brief 读寄存器（带重试）
// */
//int lmp1616x1_cmd_read(uint8_t reg, uint16_t *reg_data)
//{
//    int err = 0;
//    uint8_t retry = 10;
//
//    do {
//        err = __lmp1616x1_rf_send_cmd_read(reg, reg_data);
//        if (err != 0) {
//            lmp1616x1_rf_send_query();  /* 重新查询 */
//        }
//    } while (err != 0 && retry-- > 0);
//
//    return (retry > 0) ? 0 : -1;
//}
//
///*==============================================================================
// * 测量函数
// *============================================================================*/
//
///**
// * @brief 获取整流电压
// */
//int lmp1616x1_rf_get_vrecp(float *vrecp, uint16_t delay_count)
//{
//    int err = 0;
//    uint16_t vdd_low = 0, vdd_high = 0;
//    lmp1616x1_volt_delay_sample_t volt_delay = {0};
//    lmp1616x1_ctrlreg0_t ctrlreg0 = {0};
//
//    /* 配置电压延迟采样 */
//    volt_delay.bits.volt_delay_sample = delay_count;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_65_VOLT_DELAY_SAMPLE, volt_delay.value);
//    CHECK_ERR(err);
//
//    if (delay_count > 0) {
//        SET_RF_POWER(0);
//        delay_ms(10);
//        SET_RF_POWER(lead.set_query_power);
//    }
//
//    /* 启动电压测量 */
//    ctrlreg0.bits.start_en = LMP1616X1_MEAS_ENABLE_VOLT;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//
//    delay_ms(5);  /* 等待测量完成 */
//
//    /* 读取测量结果 */
//    err = lmp1616x1_cmd_read(LMP1616X1_REG_72_VDD_LOW, &vdd_low);
//    CHECK_ERR(err);
//    err = lmp1616x1_cmd_read(LMP1616X1_REG_73_VDD_HIGH, &vdd_high);
//    CHECK_ERR(err);
//
//    /* 清零测量使能 */
//    ctrlreg0.bits.start_en = LMP1616X1_MEAS_ENABLE_NONE;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//
//    /* ADC值转换为电压 */
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
// */
//int lmp1616x1_rf_get_res(uint8_t channel_sel, uint16_t *res, uint8_t osc)
//{
//    int err = 0;
//    uint16_t res_raw[18] = {0};  // 最多18个通道
//    lmp1616x1_ctrlreg0_t ctrlreg0 = {0};
//    float f_mhz;
//
//    if(osc == 0) {
//        f_mhz = 0.9f;    /* 默认大范围低精度 */
//    } else {
//        f_mhz = 1.24f;   /* 小范围高精度 */
//    }
//
//    /* 启动阻抗测量 */
//    ctrlreg0.bits.start_en = LMP1616X1_MEAS_ENABLE_IMP;
//    ctrlreg0.bits.imp_channel_sel = channel_sel;
//    ctrlreg0.bits.imp_test_trim_osc = osc;
//
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//
//    delay_ms(10);
//
//    /* 读取阻抗数据 */
//    for (uint8_t i = 0; i < 18; i++) {
//        uint16_t reg_addr = LMP1616X1_REG_74_TEMP_DATA + i;
//        if (reg_addr <= LMP1616X1_REG_7C_IMPEDANCE_DATA16) {
//            err = lmp1616x1_cmd_read(reg_addr, &res_raw[i]);
//            if (err != 0) break;
//        }
//    }
//
//    /* 清零使能 */
//    ctrlreg0.bits.start_en = 0;
//    lmp1616x1_cmd_write(LMP1616X1_REG_68_CTRLREG0, ctrlreg0.value);
//
//    /* 根据公式计算电阻值：res_cal = (res[i] * 2.7717 - 5722) / f_mhz */
//    for (int i = 0; i < 18 && i < 15; i++) {  // LMP1616X1只有15个电极
//        /* 第一个通道是参考通道，不计算电阻值 */
//        if (i == 0) {
//            res[i] = res_raw[i] & 0x0FFF;
//            log("ref channel = %d (raw=0x%03x)", res_raw[i], res_raw[i]);
//            continue;
//        }
//
//        /* 应用公式计算 */
//        float res_cal = (res_raw[i] * 2.7717f - 5722.0f) / f_mhz;
//        if (res_cal < 0) {
//            res_cal = 0;
//        }
//        res[i] = (uint16_t)(res_cal + 0.5f);
//        log("R[%d] = %dR (raw=0x%03x, cal=%.2f)", i, res[i], res_raw[i], res_cal);
//    }
//
//    return err;
//}
//
///**
// * @brief 获取温度
// */
//int lmp1616x1_rf_get_temp(float *temp)
//{
//    int err = 0;
//    uint16_t temp_adc = 0;
//    lmp1616x1_ctrlreg0_t ctrlreg0 = {0};
//
//    ctrlreg0.bits.start_en = LMP1616X1_MEAS_ENABLE_TEMP;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//
//    delay_ms(10);
//    err = lmp1616x1_cmd_read(LMP1616X1_REG_74_TEMP_DATA, &temp_adc);
//    CHECK_ERR(err);
//
//    ctrlreg0.bits.start_en = LMP1616X1_MEAS_ENABLE_NONE;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_68_CTRLREG0, ctrlreg0.value);
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
// */
//int lmp1616x1_rf_get_current(float *current_p, float *current_n)
//{
//    int err = 0;
//    lmp1616x1_ctrlreg0_t ctrlreg0 = {0};
//    uint16_t data_p = 0, data_n = 0;
//
//    /* 使能电流测量 */
//    ctrlreg0.bits.current_measure = 1;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//
//    delay_us(500);
//
//    /* 读取电流值 */
//    err = lmp1616x1_cmd_read(LMP1616X1_REG_7D_CURRENT_DATA_NEG, &data_n);
//    CHECK_ERR(err);
//    err = lmp1616x1_cmd_read(LMP1616X1_REG_7E_CURRENT_DATA_PLUS, &data_p);
//    CHECK_ERR(err);
//
//    /* 清零使能 */
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_68_CTRLREG0, 0);
//    CHECK_ERR(err);
//
//    *current_p = lmp1616x1_dac_to_ma(data_p & 0x0FFF) / 1000.0f;
//    *current_n = lmp1616x1_dac_to_ma(data_n & 0x0FFF) / 1000.0f;
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
// */
//int lmp1616x1_rf_write_efuse(uint32_t efuse)
//{
//    int err = 0;
//    uint32_t efuse_read = 0;
//
//    efuse &= 0xFFFFFF;  /* 24位有效 */
//
//    rf_pwm_close();
//
//    lmp1616x1_efuse_w_data0_t wdata0 = {0};
//    lmp1616x1_efuse_w_data1_t wdata1 = {0};
//    lmp1616x1_efuse_tburn_t tburn = {0};
//    lmp1616x1_ldo_trim_t trim = {0};
//
//    /* 1. 先清零数据有效位 */
//    wdata1.bits.efuse_data_valid = 0;
//    wdata1.bits.efuse_data_enable = 0x3F;  /* 烧录所有bit */
//    wdata1.bits.efuse_w_data_high = (efuse >> 16) & 0xFF;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_11_EFUSE_W_DATA1, wdata1.value);
//    CHECK_ERR(err);
//
//    /* 2. 配置烧写时间 */
//    tburn.value = 0xFFFF;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_14_EFUSE_TBURN, tburn.value);
//    CHECK_ERR(err);
//
//    trim.value = 6;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_64_LDO_TRIM, trim.value);
//    CHECK_ERR(err);
//
//    /* 3. 写入低16位数据 */
//    wdata0.value = efuse & 0xFFFF;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_10_EFUSE_W_DATA0, wdata0.value);
//    CHECK_ERR(err);
//
//    /* 4. 写入高8位并启动烧录 */
//    wdata1.bits.efuse_data_valid = 1;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_11_EFUSE_W_DATA1, wdata1.value);
//    CHECK_ERR(err);
//
//    /* 5. 最大功率供电 */
//    SET_RF_POWER(15);
//    log("Waiting for efuse write (3 seconds)...");
//    delay_ms(3000);
//
//    /* 6. 复位芯片 */
//    rf_reset_chip();
//
//    /* 7. 重新广播查询获取新efuse_id */
//    err = lmp1616x1_rf_send_query();
//    CHECK_ERR(err);
//
//    /* 8. 读取验证 */
//    err = lmp1616x1_rf_read_efuse(&efuse_read);
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
// */
//int lmp1616x1_rf_read_efuse(uint32_t *efuse)
//{
//    int err = 0;
//    uint16_t data0 = 0, data1 = 0;
//
//    err = lmp1616x1_cmd_read(LMP1616X1_REG_12_EFUSE_R_DATA0, &data0);
//    if (err != 0) return err;
//
//    err = lmp1616x1_cmd_read(LMP1616X1_REG_13_EFUSE_R_DATA1, &data1);
//    if (err != 0) return err;
//
//    *efuse = data0 | ((data1 & 0xFF) << 16);
//    return err;
//}
///*==============================================================================
// * 刺激控制函数
// *============================================================================*/
//
///**
// * @brief 停止刺激输出
// *
// * 该函数通过设置STIM_MODE寄存器的stim_stop位来立即停止所有刺激输出。
// * 停止刺激后，芯片将恢复到空闲状态，电极输出被禁用。
// *
// * @return 0-成功，-1-失败
// */
//int lmp1616x1_rf_stop_pulse(void)
//{
//    lmp1616x1_stim_mode_t stim_mode = {0};
//
//    /* 设置停止刺激位为1 */
//    stim_mode.bits.stim_stop = 1;
//
//    /* 写入STIM_MODE寄存器(0x06) */
//    return lmp1616x1_cmd_write(LMP1616X1_REG_06_STIM_MODE, stim_mode.value);
//}
//
///**
// * @brief 软件复位芯片
// *
// * 该函数通过设置TRIMDATA_CTRL0寄存器的cmd_rst位来复位芯片的数字电路。
// * 复位后，芯片的所有寄存器将恢复到默认值，需要重新配置。
// *
// * 注意：由于芯片复位期间不响应通信，需要盲发三次复位命令。
// *
// * @return 0-成功，-1-失败
// */
//int lmp1616x1_rf_reset(void)
//{
//    int err = 0;
//    lmp1616x1_trimdata_ctrl0_t ctrl0 = {0};
//
//    /* 配置控制寄存器，设置复位位 */
//    ctrl0.value = 0xc0;            /* 默认值 */
//    ctrl0.bits.cmd_rst = 1;        /* 置位复位位 */
//
//    /* 芯片复位期间无回复，盲发三次复位命令确保可靠 */
//    __lmp1616x1_rf_send_cmd_write(LMP1616X1_REG_60_TRIMDATA_CTRL0, ctrl0.value);
//    __lmp1616x1_rf_send_cmd_write(LMP1616X1_REG_60_TRIMDATA_CTRL0, ctrl0.value);
//    __lmp1616x1_rf_send_cmd_write(LMP1616X1_REG_60_TRIMDATA_CTRL0, ctrl0.value);
//
//    /* 等待复位完成(200ms) */
//    delay_ms(200);
//
//    /* 清除复位位，恢复正常工作 */
//    ctrl0.bits.cmd_rst = 0;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_60_TRIMDATA_CTRL0, ctrl0.value);
//
//    return err;
//}
//
///*==============================================================================
// * 刺激配置函数
// *============================================================================*/
//
///**
// * @brief 配置相位参数
// *
// * 设置刺激时钟分频值，影响刺激波形的时序精度和功耗。
// * 分频值越大，刺激时钟频率越低，功耗越小，但时序精度也越低。
// *
// * @param config 相位配置参数指针
// * @return 0-成功，-1-失败
// */
//int lmp1616x1_rf_config_phase(lmp1616x1_phase_config_t *config)
//{
//    int err = 0;
//    lmp1616x1_stim_mode_t stim_mode = {0};
//
//    /* 读取当前STIM_MODE寄存器值 */
//    err = lmp1616x1_cmd_read(LMP1616X1_REG_06_STIM_MODE, &stim_mode.value);
//    CHECK_ERR(err);
//
//    /* 更新时钟分频值 */
//    stim_mode.bits.stim_clk_div_value = config->stim_clk_div;
//
//    /* 写回STIM_MODE寄存器 */
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_06_STIM_MODE, stim_mode.value);
//    CHECK_ERR(err);
//
//    return err;
//}
//
///**
// * @brief 配置软启动参数
// *
// * 软启动功能用于在刺激开始时逐渐增加刺激幅度，避免电流突变。
// * 可以配置增量值和步进周期数，控制幅度增加的速度。
// *
// * @param prog_idx 程序索引(0-3)
// * @param params 软启动参数指针
// * @return 0-成功，-1-失败
// */
//int lmp1616x1_rf_config_softstart(lmp1616x1_program_idx_t prog_idx,
//                                   lmp1616x1_softstart_params_t *params)
//{
//    int err = 0;
//
//    /* 配置软启动控制寄存器(0x0B) */
//    lmp1616x1_softstart_ctrl_t soft_ctrl = {0};
//    soft_ctrl.bits.softstart_en = params->enable ? 1 : 0;           /* 使能位 */
//    soft_ctrl.bits.softstart_step = params->step;                  /* 步进周期数 */
//
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_0B_SOFTSTART_CTRL, soft_ctrl.value);
//    CHECK_ERR(err);
//
//    /* 如果使能软启动，配置多个DAC的增量值 */
//    if (params->enable) {
//        /* 配置软启动增量DAC值寄存器(0x56-0x59) */
//        /* 这里配置第一个寄存器(0x56)，包含4个DAC的增量值 */
//        uint16_t inc_dac_value = 0;
//
//        /* 将前4个DAC的增量值打包到16位寄存器中 */
//        for (int i = 0; i < 4 && i < 16; i++) {
//            if (i < 4) {
//                /* 每个DAC增量值占4位，范围0-15 */
//                inc_dac_value |= (params->inc_value[i] & 0x0F) << (i * 4);
//            }
//        }
//
//        err = lmp1616x1_cmd_write(LMP1616X1_REG_56_SOFTSTART_INC_DAC_0, inc_dac_value);
//        CHECK_ERR(err);
//
//        /* 注：实际应用中需要继续配置0x57-0x59寄存器，支持最多16个DAC */
//    }
//
//    return err;
//}
//
///**
// * @brief 配置软停止功能
// *
// * 软停止功能集成在STIM_MODE寄存器中，可以设置在刺激一定周期后自动停止。
// * 用于实现定时刺激或保护功能。
// *
// * @param enable_mask 使能掩码，bit0对应自动停止功能
// * @return 0-成功，-1-失败
// */
//int lmp1616x1_rf_config_softstop(uint8_t enable_mask)
//{
//    lmp1616x1_stim_mode_t stim_mode = {0};
//
//    /* 读取当前STIM_MODE寄存器 */
//    int err = lmp1616x1_cmd_read(LMP1616X1_REG_06_STIM_MODE, &stim_mode.value);
//    CHECK_ERR(err);
//
//    /* 设置自动停止位 */
//    stim_mode.bits.stim_auto_stop = (enable_mask & 0x01) ? 1 : 0;
//
//    /* 写回寄存器 */
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_06_STIM_MODE, stim_mode.value);
//
//    return err;
//}
//
///*==============================================================================
// * 设置刺激波形时序（内部函数）
// *============================================================================*/
//
///**
// * @brief 设置刺激波形时序参数（内部函数）
// *
// * 将用户配置的时间参数(us)转换为芯片的时钟周期数，并写入相应的时序寄存器。
// * 主要配置程序0的时序参数，包括正负脉冲宽度、间隔时间等。
// *
// * @param params 刺激参数指针
// * @return 0-成功，-1-失败
// */
//static int __lmp1616x1_rf_set_simple_feature(lmp1616x1_simple_params_t *params)
//{
//    int err = 0;
//
//    /* 设置程序0的时序参数 */
//    if (params->program_cnt >= 0) {
//        lmp1616x1_tphase1_t tphase1_0 = {0};
//        lmp1616x1_tphase2_t tphase2_0 = {0};
//        lmp1616x1_tphase3_t tphase3_0 = {0};
//        lmp1616x1_tinterphase_t tinterphase_0 = {0};
//        lmp1616x1_tinterpulse_t tinterpulse_0 = {0};
//
//        /* 将时间参数(us)转换为时钟周期数 */
//        uint16_t tphase1 = lmp1616x1_us_to_clk(params->program[0].tphase1,
//                                               params->phase.stim_clk_div);
//        uint16_t tphase2 = lmp1616x1_us_to_clk(params->program[0].tphase2,
//                                               params->phase.stim_clk_div);
//        uint16_t tphase3 = lmp1616x1_us_to_clk(params->program[0].tphase3,
//                                               params->phase.stim_clk_div);
//        uint16_t tinterphase1 = lmp1616x1_us_to_clk(params->program[0].tinterphase1,
//                                                   params->phase.stim_clk_div);
//        uint16_t tinterphase2 = lmp1616x1_us_to_clk(params->program[0].tinterphase2,
//                                                   params->phase.stim_clk_div);
//
//        /* 计算总周期和脉冲间隔 */
//        uint32_t period_clk = lmp1616x1_us_to_clk(1000000 / params->base_freq,
//                                                 params->phase.stim_clk_div) /
//                             params->program[0].multiple;
//        uint16_t tinterpulse = period_clk - tphase1 - tphase2 - tphase3 -
//                              tinterphase1 - tinterphase2;
//
//        /* 填充寄存器结构体 */
//        tphase1_0.bits.tphase1 = tphase1;         /* 正脉冲宽度 */
//        tphase2_0.bits.tphase2 = tphase2;         /* 负脉冲宽度 */
//        tphase3_0.bits.tphase3 = tphase3;         /* 电荷平衡时间 */
//        tinterphase_0.bits.tinterphase1 = tinterphase1;  /* 正负脉冲间隔1 */
//        tinterphase_0.bits.tinterphase2 = tinterphase2;  /* 正负脉冲间隔2 */
//        tinterpulse_0.bits.tinterpulse = tinterpulse;    /* 脉冲间隔 */
//
//        /* 写入时序寄存器 */
//        err = lmp1616x1_cmd_write(LMP1616X1_REG_00_TPHASE1, tphase1_0.value);
//        CHECK_ERR(err);
//        err = lmp1616x1_cmd_write(LMP1616X1_REG_01_TPHASE2, tphase2_0.value);
//        CHECK_ERR(err);
//        err = lmp1616x1_cmd_write(LMP1616X1_REG_03_TPHASE3, tphase3_0.value);
//        CHECK_ERR(err);
//        err = lmp1616x1_cmd_write(LMP1616X1_REG_02_TINTERPHASE, tinterphase_0.value);
//        CHECK_ERR(err);
//        err = lmp1616x1_cmd_write(LMP1616X1_REG_04_TINTERPULSE, tinterpulse_0.value);
//        CHECK_ERR(err);
//    }
//
//    /* 注：程序1-3的时序参数配置需要扩展实现 */
//    /* 实际应用中需要根据params->program_cnt配置多个程序 */
//
//    return err;
//}
//
///**
// * @brief 设置刺激幅度（内部函数）
// *
// * 配置15个电极的正向和负向刺激幅度(DAC值)。
// * 需要根据电极分组和寄存器位映射进行复杂配置。
// *
// * @param params 刺激参数指针
// * @return 0-成功，-1-失败
// */
//static int __lmp1616x1_rf_set_range(lmp1616x1_simple_params_t *params)
//{
//    int err = 0;
//
//    /* 遍历所有使能的刺激程序 */
//    for (int prog = 0; prog <= params->program_cnt && prog < MAX_PROGRAM_NUM; prog++) {
//        /* 计算基础寄存器地址：每个程序占用10个寄存器(0x15-0x1E, 0x1F-0x28等) */
//        uint8_t base_reg = 0x15 + (prog * 0x0A);
//
//        /* 遍历电极配置DAC值 */
//        for (int elec = 0; elec < 15 && elec < 8; elec++) {  /* 简化：只配置前8个电极 */
//            /* 将电流值(uA)转换为DAC值 */
//            uint16_t dac_p = lmp1616x1_ua_to_dac(params->program[prog].amplitude_p[elec]);
//            uint16_t dac_n = lmp1616x1_ua_to_dac(params->program[prog].amplitude_n[elec]);
//
//            /* 实际应用中需要根据电极索引计算具体寄存器地址和位映射 */
//            /* 由于寄存器映射复杂，这里不展开完整配置逻辑 */
//
//            /* 示例：将DAC值写入对应寄存器 */
//            /* 需要根据电极分组(01, 123, 34, 456等)和位映射进行配置 */
//        }
//    }
//
//    return err;
//}
//
///**
// * @brief 设置电极极性（内部函数）
// *
// * 配置刺激时使用的电极对，包括阳极(DANSEL)和阴极(DCASEL)。
// * 每个程序有独立的电极开关控制寄存器。
// *
// * @param params 刺激参数指针
// * @return 0-成功，-1-失败
// */
//static int __lmp1616x1_rf_set_polar(lmp1616x1_simple_params_t *params)
//{
//    int err = 0;
//
//    /* 遍历所有使能的刺激程序 */
//    for (int prog = 0; prog <= params->program_cnt && prog < MAX_PROGRAM_NUM; prog++) {
//        /* 计算STIM_ON寄存器地址 */
//        /* 程序0: 0x3D(STIM_ON0), 0x3E(STIM_ON1) */
//        /* 程序1: 0x3F(STIM_ON0), 0x40(STIM_ON1) */
//        /* 程序2: 0x41(STIM_ON0), 0x42(STIM_ON1) */
//        /* 程序3: 0x43(STIM_ON0), 0x44(STIM_ON1) */
//        uint8_t stim_on0_reg = 0x3D + (prog * 2);
//        uint8_t stim_on1_reg = stim_on0_reg + 1;
//
//        /* 写入电极极性配置 */
//        err = lmp1616x1_cmd_write(stim_on0_reg, params->program[prog].polar.dir);
//        CHECK_ERR(err);
//        err = lmp1616x1_cmd_write(stim_on1_reg, params->program[prog].polar.on);
//        CHECK_ERR(err);
//    }
//
//    return err;
//}
//
///*==============================================================================
// * 刺激配置主函数
// *============================================================================*/
//
///**
// * @brief 配置简单模式刺激
// *
// * 完整的简单模式刺激配置流程，包括：
// * 1. 配置相位参数(时钟分频)
// * 2. 设置波形时序
// * 3. 设置刺激幅度
// * 4. 设置电极极性
// * 5. 启动刺激输出
// *
// * @param params 刺激参数指针
// * @return 0-成功，-1-失败
// */
//int lmp1616x1_rf_config_simple(lmp1616x1_simple_params_t *params)
//{
//    int err = 0;
//
//    /* 1. 配置相位参数(时钟分频) */
//    err = lmp1616x1_rf_config_phase(&params->phase);
//    CHECK_ERR(err);
//
//    /* 2. 设置波形时序 */
//    err = __lmp1616x1_rf_set_simple_feature(params);
//    CHECK_ERR(err);
//
//    /* 3. 设置刺激幅度 */
//    err = __lmp1616x1_rf_set_range(params);
//    CHECK_ERR(err);
//
//    /* 4. 设置电极极性 */
//    err = __lmp1616x1_rf_set_polar(params);
//    CHECK_ERR(err);
//
//    /* 5. 启动刺激输出 */
//    lmp1616x1_stim_mode_t stim_mode = {0};
//    stim_mode.bits.stim_mode = LMP1616X1_STIM_MODE_SIMPLE;  /* 简单模式 */
//    stim_mode.bits.program_cnt = params->program_cnt;       /* 程序数量 */
//    stim_mode.bits.stim_low_power = params->low_power_mode ? 1 : 0;  /* 低功耗模式 */
//    stim_mode.bits.stim_clk_div_value = params->phase.stim_clk_div;  /* 时钟分频 */
//    stim_mode.bits.para_vld = 1;                            /* 参数有效 */
//    stim_mode.bits.stim_start = 1;                          /* 启动刺激 */
//
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_06_STIM_MODE, stim_mode.value);
//    CHECK_ERR(err);
//
//    /* 6. 清除启动标志，刺激持续输出 */
//    stim_mode.bits.para_vld = 0;
//    stim_mode.bits.stim_start = 0;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_06_STIM_MODE, stim_mode.value);
//
//    return err;
//}
//
///**
// * @brief 配置多程序刺激
// *
// * 多程序刺激配置，支持多个刺激程序交替或并行运行。
// * 首先复位芯片确保干净的状态，然后调用简单模式配置。
// *
// * @param params 刺激参数指针
// * @return 0-成功，-1-失败
// */
//int lmp1616x1_rf_config_more_simple(lmp1616x1_simple_params_t *params)
//{
//    int err = 0;
//
//    /* 1. 复位芯片，确保干净的状态 */
//    err = lmp1616x1_rf_reset();
//    CHECK_ERR(err);
//
//    /* 2. 调用简单模式配置 */
//    err = lmp1616x1_rf_config_simple(params);
//
//    return err;
//}
//
///**
// * @brief 配置突发模式刺激
// *
// * 突发模式刺激配置，支持脉冲簇输出。
// * 配置簇间隔时间和每簇脉冲数，然后启动突发模式。
// *
// * @param params 刺激参数指针
// * @return 0-成功，-1-失败
// */
//int lmp1616x1_rf_config_burst(lmp1616x1_simple_params_t *params)
//{
//    int err = 0;
//
//    /* 1. 配置突发模式参数(簇间隔时间) */
//    lmp1616x1_tinterburst_t tinterburst = {0};
//    tinterburst.value = lmp1616x1_us_to_clk(params->burst_tinter,
//                                           params->phase.stim_clk_div);
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_08_TINTERBURST, tinterburst.value);
//    CHECK_ERR(err);
//
//    /* 2. 配置刺激模式为突发模式 */
//    lmp1616x1_stim_mode_t stim_mode = {0};
//    stim_mode.bits.stim_mode = LMP1616X1_STIM_MODE_BURST;    /* 突发模式 */
//    stim_mode.bits.program_cnt = params->program_cnt;        /* 程序数量 */
//    stim_mode.bits.stim_low_power = params->low_power_mode ? 1 : 0;  /* 低功耗 */
//    stim_mode.bits.stim_clk_div_value = params->phase.stim_clk_div;  /* 时钟分频 */
//
//    /* 突发脉冲数设置在STIM_MODE寄存器的pulse_cnt字段([15:8]位) */
//    stim_mode.value |= (params->burst_pulse_cnt << 8);
//
//    stim_mode.bits.para_vld = 1;      /* 参数有效 */
//    stim_mode.bits.stim_start = 1;    /* 启动刺激 */
//
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_06_STIM_MODE, stim_mode.value);
//    CHECK_ERR(err);
//
//    /* 3. 清除启动标志，刺激开始输出 */
//    stim_mode.bits.para_vld = 0;
//    stim_mode.bits.stim_start = 0;
//    err = lmp1616x1_cmd_write(LMP1616X1_REG_06_STIM_MODE, stim_mode.value);
//
//    return err;
//}
