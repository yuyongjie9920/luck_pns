///**
// * @file LMP1602X1.c
// * @brief LMP1602X1 驱动实现
// * @version 2.0
// * @date 2026-04-07
// * 需要添加一个获取芯片所以寄存器的函数  测试发现query 概率间隔不回复
// */
//
//#include "leadinno_app.h"
//#include "main_user.h"
//#include "crc.h"
//#include "LMP1602X1.h"
//#include "LMP1602X1_reg.h"
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
//struct lmp1602x1_data lmp = {0};
//
///*==============================================================================
// * 常量定义
// *============================================================================*/
//
//#define DAC_TO_UA_FACTOR    2439        /**< DAC值转uA系数 2.439 * 1000 */
//#define UA_TO_DAC_FACTOR    410         /**< uA转DAC系数 1000/2.439 ≈ 410 */
//#define US_CALIBRATION_NUMERATOR   2308  // 实际值
//#define US_CALIBRATION_DENOMINATOR 1900  // 理论值
//
///*==============================================================================
// * 辅助函数
// *============================================================================*/
//
///**
// * @brief 微秒转换为时钟周期
// */
//uint32_t lmp_us_to_clk(uint32_t us, uint8_t clk_div)
//{
//    uint32_t clk;
//    // 应用校准系数
//    clk = (uint32_t)((uint64_t)us * US_CALIBRATION_DENOMINATOR / US_CALIBRATION_NUMERATOR) + 1;
//    return clk / (clk_div + 1);
//}
//
//uint32_t lmp_clk_to_us(uint32_t clk, uint8_t clk_div)
//{
//    uint32_t clk_raw = clk * (clk_div + 1);
//    return ((uint64_t)(clk_raw - 1) * US_CALIBRATION_NUMERATOR) / US_CALIBRATION_DENOMINATOR;
//}
//
//
///**
// * @brief DAC值转换为电流值(uA)
// */
//uint32_t lmp_dac_to_ua(uint16_t dac)
//{
//    return (dac * DAC_TO_UA_FACTOR + 500) / 1000;
//}
//
///**
// * @brief 电流值(uA)转换为DAC值
// */
//uint16_t lmp_ua_to_dac(uint32_t ua)
//{
//    // 校准参数
//    const uint32_t cal_dac = 250;      // 校准点的DAC值
//    const uint32_t cal_ua = 10540;     // 校准点的电流值(μA)
//
//    uint32_t dac_value = (ua * cal_dac) / cal_ua;
//    if (dac_value > 4095) {
//        dac_value = 4095;
//    }
//    return (uint16_t)dac_value;
//}
//
///**
// * @brief 校准后的UA转DAC
// */
//uint16_t lmp_calibrated_ua_to_dac(uint32_t target_ua)
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
//static inline int __lmp_rf_send_query(void)
//{
//    uint8_t bit_len;
//    uint16_t crc, check_crc;
//    uint8_t send_data[5] = {0x80,0,0,0,0};
//    uint8_t raw_data[8] = {0};
//    uint8_t *out_data;
//    uint8_t uid = 0;
//
//    send_data[0] |= (uid >> 12) & 0x0F;    // UID高4位 -> bytes[0]低4位
//    send_data[1] = (uid >> 4) & 0xFF;      // UID中间8位 -> bytes[1]
//    send_data[2] = (uid & 0x0F) << 4;      // UID低4位 -> bytes[2]高4位
//
//    crc =  crc16Bitwise(send_data, 20);
//
//    send_data[20/8] |= (crc >> 12) & 0x0F; // CRC高4位到bytes[2]低4位
//    send_data[20/8+1] = (crc >> 4) & 0xFF; // CRC中间8位到bytes[3]
//    send_data[20/8+2] = (crc & 0x0F) << 4; // CRC低4位到bytes[4]高4位
//
//    rf_tx_data(send_data, 36); // cmd query is 8 4bit uid 16bit crc 16bit
//
//    int err = rf_rx_data(raw_data, &bit_len);
//    if (err != 0) {
//        log("query broadcast rx error: %d", err);
//        return err;
//    }
//
//    if(crc16Bitwise(raw_data, 8 * 2) != ((uint16_t)raw_data[2]<<8 | raw_data[3] )) {
//        logInfo("query crc fail");
//        return -1;
//    }
//
//    // 解析返回数据
//    if (bit_len >= 32) {
//        // 保存查询状态
//        lmp.last_status.value = (uint16_t)raw_data[0]<<8 | raw_data[1];
//        lmp.efuse_id = lmp.last_status.bits.efuse_id;
////        log("===== Query Response =====");
////        log("Status Value : 0x%04x", lmp.last_status.value);
////        log("EFUSE ID     : 0x%02x", lmp.last_status.bits.efuse_id);
////        log("");
////        log("Bit15 - UnderVolt occurred     : %d", lmp.last_status.bits.undervolt);
////        log("Bit14 - Program0 active        : %d", lmp.last_status.bits.prog0_active);
////        log("Bit13 - Overlap occurred       : %d", lmp.last_status.bits.overlap);
////        log("Bit12 - Stimulation complete   : %d", lmp.last_status.bits.stim_complete);
////        log("Bit11 - Temperature done       : %d", lmp.last_status.bits.temp_done);
////        log("Bit10 - Voltage/Current done   : %d", lmp.last_status.bits.volt_done);
////        log("Bit9  - Impedance done         : %d", lmp.last_status.bits.imp_done);
////        log("Bit8  - Reserved               : %d", lmp.last_status.bits.reserved);
////        log("Bit7-0- EFUSE ID low 8 bits    : 0x%02x", lmp.last_status.bits.efuse_id);
////        log("==========================");
//        return 0;
//    }
//    return -1;
//}
//
///**
// * @brief 发送Query命令(带功率扫描)
// */
//int lmp_rf_send_query(void)
//{
//    for (uint8_t power = 0; power < 15; power++) {
//        SET_RF_POWER(power);
//        if (__lmp_rf_send_query() == 0) {
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
//int lmp_rf_send_query_with_power(uint8_t power)
//{
//    int err;
//    if (power > 15) power = 15;
//    SET_RF_POWER(power);
//    err = __lmp_rf_send_query();
//    return err;
//}
//
///**
// * @brief 所有功率点测试
// */
//int lmp_rf_send_query_all_power(void)
//{
//    int err = 0;
//    for (uint8_t power = 0; power < 15; power++) {
//        SET_RF_POWER(power);
//        err = __lmp_rf_send_query();
//        log("power %2d: %s", power, err == 0 ? "OK" : "FAIL");
//    }
//    return 0;
//}
//
///**
// * @brief 发送写命令
// */
//static inline int __lmp_rf_send_cmd_write(uint8_t reg, uint16_t reg_data)
//{
//    SET_RF_POWER(lead.set_query_power);
//
//    lmp_rf_write_send_data_t write_data;
//    lmp_rf_write_dec_data_t tx;
//    uint8_t rx_data[8];
//    uint16_t crc;
//    uint8_t nbits=0;
//    uint16_t uid;
//
//    write_data.cmd = 0xc3;
//    write_data.reg = reg;
//    write_data.reg_data = ((reg_data & 0xff00) >> 8) | ((reg_data & 0x00ff) << 8);
//    uid = lmp.efuse_id;
//    uid = (uid<<8) | (uid >> 8);
//    write_data.uid = uid;
//
//    crc = crc16Bitwise((uint8_t *)&write_data,
//                      (sizeof(write_data.cmd)+sizeof(write_data.reg)+
//                       sizeof(write_data.reg_data)+sizeof(write_data.uid))* 8);
//    write_data.crc_h = crc>>8;
//    write_data.crc_l = crc;
//
//    rf_tx_data((uint8_t *)&write_data, sizeof(write_data)*8);
//
//    int err = rf_rx_data(rx_data, &nbits);
//    if(err != 0) {
//        logInfo("RX err = %d", err);
//    }
//
//    tx.crc = ((rx_data[2] & 0x7F) << 9) | (rx_data[3] << 1) | ((rx_data[4] >> 7) & 0x01);
//
//    if((nbits/8) > 2) {
//        crc = crc16Bitwise(rx_data, nbits-16);
//        if(crc == tx.crc) {
//            log("write reg 0x%x->0x%04x ok", reg, reg_data);
//            err = 0;
//        } else {
//            err = -1;
//        }
//    } else {
//        err = -1;
//    }
//
//    if(err != 0) {
//        log("write reg 0x%x->0x%x fail err = %d", reg, reg_data, err);
//    }
//
//    SET_RF_POWER(lead.stim_pulse_power);  // 设置射频功率为刺激功率
//
//    return err;
//}
//
///**
// * @brief 发送读命令
// */
//static inline int __lmp_rf_send_cmd_read(uint8_t reg, uint16_t *get_reg_data)
//{
//    lmp_rf_read_send_data_t send_data;
//    lmp_rf_rx_dec_data_t rx;
//    uint8_t rx_data[8];
//    uint16_t crc;
//    uint8_t nbits=0;
//
//    send_data.cmd = 0xc2; // 读命令
//    send_data.reg = reg;
//    uint16_t uid = lmp.efuse_id;
//    uid = (uid<<8) | (uid >> 8);
//    send_data.uid = uid;
//
//    crc = crc16Bitwise((uint8_t *)&send_data,
//                      (sizeof(send_data.cmd)+sizeof(send_data.reg)+
//                       sizeof(send_data.uid))* 8);
//    send_data.crc_h = crc>>8;
//    send_data.crc_l = crc;
//
//    rf_tx_data((uint8_t *)&send_data, sizeof(send_data)*8);
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
//        crc = crc16Bitwise(rx_data, nbits-16);
//        if(crc == rx.crc) {
//            *get_reg_data = rx.reg_data;
//            err = 0;
//            log("read reg 0x%x<-0x%04x ok", reg, rx.reg_data);
//        } else {
//            err = -1;
//            log("read reg 0x%x fail", reg);
//        }
//    } else {
//        err = -1;
//        log("read reg 0x%04x fail", reg);
//    }
//
//    SET_RF_POWER(lead.stim_pulse_power);  // 设置射频功率为刺激功率
//    return err;
//}
//
///*==============================================================================
// * 带重试的寄存器读写
// *============================================================================*/
//inline int lmp_cmd_write(uint8_t reg, uint16_t reg_data)
//{
//    int err = 0;
//    int retry = 3;  // 重试3次
//
//    do {
//        err = __lmp_rf_send_cmd_write(reg, reg_data);
//        if (err != 0) {
//            err = __lmp_rf_send_cmd_write(reg, reg_data);
//        }
//        if(err != 0) {
//            lmp_rf_send_query();  /* 重新查询 */
//        }
//    } while (err != 0 && retry-- > 0);
//
//    return (retry >= 0) ? 0 : -1;  // 重试次数用尽返回-1
//}
//
//inline int lmp_cmd_read(uint8_t reg, uint16_t *reg_data)
//{
//    int err = 0;
//    uint8_t retry = 3;
//
//    do {
//        err = __lmp_rf_send_cmd_read(reg, reg_data);
//        if (err != 0) {
//            lmp_rf_send_query();  /* 重新查询 */
//        }
//    } while (err != 0 && retry-- > 0);
//
//    return (retry > 0) ? 0 : -1;
//}
//
///**
// * @brief 软件复位
// */
//int __lmp_rf_reset(void)
//{
//    int err = 0;
//    uint16_t ctrl0 = 0xC0;
//    ctrl0 |= 0x0004;  // cmd_rst = 1
//
//    __lmp_rf_send_cmd_write(LMP1602X1_REG_60_TRIMDATA_CTRL0, ctrl0);
//    __lmp_rf_send_cmd_write(LMP1602X1_REG_60_TRIMDATA_CTRL0, ctrl0);
//    __lmp_rf_send_cmd_write(LMP1602X1_REG_60_TRIMDATA_CTRL0, ctrl0);
//
//    delay_ms(200);
//
//    ctrl0 &= ~0x0004;  // cmd_rst = 0LMP1602X1_REG_60_TRIMDATA_CTRL0
////    err = lmp_cmd_write(, ctrl0);
//    return err;
//}
//
//
///*==============================================================================
// * 测量功能
// *============================================================================*/
//
//static int imp_ctl_choice(lmp_meas_enable_mask_e mask, uint8_t program_sel, uint8_t res_ch, uint8_t trim_osc, uint8_t sel_ab)
//{
//    int err = 0;
//    lmp1602x1_ctrlreg0_t ctrlreg0 = {0};
//    ctrlreg0.value = 0;
//    ctrlreg0.bits.start_en = mask;
//    ctrlreg0.bits.program_sel = program_sel;
//    ctrlreg0.bits.imp_channel_sel = res_ch;
//    ctrlreg0.bits.imp_test_trim_osc = trim_osc;
//    ctrlreg0.bits.current_measure = sel_ab;
//    err = lmp_cmd_write(LMP1602X1_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//    delay_ms(10);
//    return err;
//}
//
//int imp_ctl_clear(void)
//{
//    int err = 0;
//    lmp1602x1_ctrlreg0_t ctrlreg0 = {0};
//    /* 清零测量使能 */
//    ctrlreg0.value = 0;
//    err = lmp_cmd_write(LMP1602X1_REG_68_CTRLREG0, ctrlreg0.value);
//    CHECK_ERR(err);
//    return err;
//}
//
///**
// * @brief 获取整流电压
// */
//int lmp_rf_get_vrecp(float *vrecp, uint16_t delay_count)
//{
//    int err = 0;
//    uint16_t vdd_low = 0, vdd_high = 0;
//    uint16_t volt_delay = 0;
//    lmp1602x1_ctrlreg0_t ctrlreg0 = {0};
//
//
//    /* 配置电压延迟采样 */
//    volt_delay = delay_count;
//    err = lmp_cmd_write(LMP1602X1_REG_65_VOLT_DELAY_SAMPLE, volt_delay);
//    CHECK_ERR(err);
//
//    if (delay_count > 0) {
//        SET_RF_POWER(0);
//        delay_ms(10);
//        SET_RF_POWER(lead.set_query_power);
//    }
//
//    imp_ctl_choice(LMP_MEAS_ENABLE_VOLT, 0, 0, 0, 0);
//
//
//    /* 读取测量结果 */
//    err = lmp_cmd_read(LMP1602X1_REG_72_VDD_LOW, &vdd_low);
//    CHECK_ERR(err);
//    err = lmp_cmd_read(LMP1602X1_REG_73_VDD_HIGH, &vdd_high);
//    CHECK_ERR(err);
//
//    imp_ctl_clear();
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
//int lmp_rf_get_res(uint8_t channel_sel, uint16_t *res, uint8_t osc)
//{
//    int err = 0;
//    uint16_t res_raw[16] = {0};
//    lmp1602x1_ctrlreg0_t ctrlreg0 = {0};
//    lmp1602x1_impedance_data1_t imp_data[16];
//    float f_mhz;
//    float res_cal;
//
//    f_mhz = 0.9f;    /* 默认大范围低精度 */
//
//    /* 启动阻抗测量 */
//    imp_ctl_choice(LMP_MEAS_ENABLE_IMP, 0, channel_sel, osc, 0);
//
//
//    /* 读取8通道数据 */
//    for (uint8_t i = 0; i < 8; i++) {
//        err = lmp_cmd_read(LMP1602X1_REG_6A_IMPEDANCE_DATA1 + i, &imp_data[i].value);
//        res_raw[i] = imp_data[i].bits.data;
//    }
//    imp_ctl_clear();
//
//    /* 启动阻抗测量 */
//    imp_ctl_choice(LMP_MEAS_ENABLE_IMP, 0, channel_sel, osc, 1);
//
//
//    /* 读取8通道数据 */
//    for (uint8_t i = 0; i < 8; i++) {
//        err = lmp_cmd_read(LMP1602X1_REG_75_IMPEDANCE_DATA9 + i, &imp_data[i+8].value);
//        res_raw[i+8] = imp_data[i].bits.data;
//    }
//
//    imp_ctl_clear();
//
//    /* 根据原公式计算电阻值：res_cal = (res[i] * 2.7717 - 5722) / f_mhz */
//    for (int i = 0; i < 16; i++) {
//        if (i == 0) {
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
//            channel_sel, (channel_sel + i) % 8, res[i], res_raw[i], res_cal);
//    }
//
//    return err;
//}
//
///**
// * @brief 获取温度
// */
//int lmp_rf_get_temp(float *temp)
//{
//    int err = 0;
//    uint16_t temp_adc = 0;
//    lmp1602x1_ctrlreg0_t ctrlreg0 = {0};
//
//    imp_ctl_choice(LMP_MEAS_ENABLE_TEMP, 0, 0, 0, 0);
//
//    err = lmp_cmd_read(LMP1602X1_REG_74_TEMP_DATA, &temp_adc);
//    CHECK_ERR(err);
//
//    imp_ctl_clear();
//
//    float temperature;
//    temperature = 65.856f - (float)(temp_adc & 0x0FFF) / 75.188f;
//    *temp = roundf(temperature * 10.0f) / 10.0f;
//    logInfo("temp = %.1f", *temp);
//
//    return err;
//}
//
///**
// * @brief 获取电流
// */
//int lmp_rf_get_current(float *current_p, float *current_n, lmp_current_source_e source)
//{
//    int err = 0;
//    lmp1602x1_ctrlreg0_t ctrlreg0 = {0};
//    uint16_t data_p = 0, data_n = 0;
//
//    imp_ctl_choice(LMP_MEAS_ENABLE_NONE, 0, 0, 0, source);
//
//    /* 读取电流值 */
//    err = lmp_cmd_read(LMP1602X1_REG_7D_CURRENT_DATA_NEG, &data_n);
//    CHECK_ERR(err);
//    err = lmp_cmd_read(LMP1602X1_REG_7E_CURRENT_DATA_PLUS, &data_p);
//    CHECK_ERR(err);
//
//    /* 清零使能 */
//    imp_ctl_clear();
//
//    *current_p = lmp_dac_to_ua(data_p & 0x0FFF) / 1000.0f;
//    *current_n = lmp_dac_to_ua(data_n & 0x0FFF) / 1000.0f;
//
//    log("CURRENT %s: P=%.2fmA (0x%03x), N=%.2fmA (0x%03x)",
//        (source == LMP_CURRENT_SOURCE_A) ? "A" : "B",
//        *current_p, data_p & 0x0FFF, *current_n, data_n & 0x0FFF);
//    return err;
//}
//
///*==============================================================================
// * eFuse操作
// *============================================================================*/
//
///**
// * @brief 写eFuse
// */
//int lmp_rf_write_efuse(uint32_t efuse)
//{
//    int err = 0;
//    uint32_t efuse_read = 0;
//
//    /* 1. 复位芯片 */
//    rf_reset_chip();
//
//    efuse &= 0xFFFFFF;  /* 24位有效 */
//    rf_pwm_close();
//
//    lmp1602x1_efuse_w_data1_t w_data1;
//    lmp1602x1_efuse_tburn_t tburn;
//    lmp1602x1_efuse_w_data0_t w_data0;
//
//    /* 4. 写入高8位并启动烧录 */
//    w_data1.bits.efuse_data_enable = 0x3F; /* 烧录所有bit */
//    w_data1.bits.efuse_data_valid = 0;
//    w_data1.bits.efuse_w_data_16_23 = (efuse >> 16) & 0xFF;
//    err = lmp_cmd_write(LMP1602X1_REG_11_EFUSE_W_DATA1, w_data1.value);
//    CHECK_ERR(err);
//
//    /* 2. 配置烧写时间 */
//    uint16_t tburn_time = 0xFFFF;
//    tburn.bits.efuse_burn_clk_en = 1;
//    tburn.bits.efuse_tburn = tburn_time; //
//    err = lmp_cmd_write(LMP1602X1_REG_14_EFUSE_TBURN, tburn.value);
//    CHECK_ERR(err);
//
//    /* 3. 写入低16位数据 */
//    uint16_t wdata0 = efuse & 0xFFFF;
//    err = lmp_cmd_write(LMP1602X1_REG_10_EFUSE_W_DATA0, wdata0);
//    CHECK_ERR(err);
//
//    w_data1.bits.efuse_data_valid = 1;
//    err = lmp_cmd_write(LMP1602X1_REG_11_EFUSE_W_DATA1, w_data1.value);
//    CHECK_ERR(err);
//
//    /* 5. 最大功率供电 */
//    SET_RF_POWER(0);
//    log("Waiting for efuse write (3 seconds)..."); //只有在读到efuse bit【23】为1时，关闭时钟，表示读efuse已经完成。    有空测试这个地方 可以优化
//    delay_ms(3000);
//
//    /* 6. 复位芯片 */
//    rf_reset_chip();
//
//    /* 7. 重新广播查询获取新efuse_id */
//    err = lmp_rf_send_query();
//    CHECK_ERR(err);
//
//    /* 8. 读取验证 */
//    err = lmp_rf_read_efuse(&efuse_read);
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
//int lmp_rf_read_efuse(uint32_t *efuse)
//{
//    int err = 0;
//    uint16_t data0 = 0, data1 = 0;
//
//    err = lmp_cmd_read(LMP1602X1_REG_12_EFUSE_R_DATA0, &data0);
//    CHECK_ERR(err);
//    err = lmp_cmd_read(LMP1602X1_REG_13_EFUSE_R_DATA1, &data1);
//    CHECK_ERR(err);
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
// */
//int lmp_rf_stop_pulse(void)
//{
//    lmp1602x1_stim_mode_t stim_mode = {0};
//    stim_mode.bits.stim_stop = 1;
//    return lmp_cmd_write(LMP1602X1_REG_06_STIM_MODE, stim_mode.value);
//}
//
///**
// * @brief 停止指定电流源的刺激
// */
//int lmp_rf_stop_pulse_current_source()
//{
//    // LMP1602X1通过STIM_MODE寄存器控制，暂不支持单独停止A/B电流源
//    return lmp_rf_stop_pulse();
//}
//
///*==============================================================================
// * 刺激配置函数
// *============================================================================*/
//
///**
// * @brief 配置软启动
// */
//int lmp_rf_config_softstart(lmp_program_idx_t prog_idx, lmp_softstart_params_t *params)
//{
//    uint16_t reg_value = 0;
//
//    if (params->enable) {
//        reg_value |= 0x8000;  // softstart_en
//    }
//    reg_value |= ((params->inc_value & 0x1F) << 10);  // softstart_inc_value
//    reg_value |= (params->step & 0x3FF);              // softstart_step
//
//    return lmp_cmd_write(LMP1602X1_REG_0B_SOFTSTART_CTRL, reg_value);
//}
//
//int lmp_auot_stop(uint16_t stop_clk_count)
//{
//    return lmp_cmd_write(LMP1602X1_REG_0A_AUTO_STOP_CYCLE, stop_clk_count);;
//}
//
//
///*==============================================================================
// * 简单模式刺激配置
// *============================================================================*/
//uint16_t test_cc;
//static int test1(uint16_t test_count)
//{
//    test_cc = test_count;
//    logInfo("test_cc = %d", test_cc);
//    return 0;
//
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
//                 test1, test1,
//                 "LMP: Simple stim [pwm%%] [w(us)] [freq] [ua] [electrode_xy] [power]");
//
//
//
//// 计算第 n 项（n 从 1 开始）
//int sequence(lmp_simple_params_t *params, int n) {
//
//    return (121) - n + (n - 1) / 5;
//}
//
///**
// * @brief 内部函数：设置刺激波形参数
// */
//static int __lmp_rf_set_stim_params(lmp_simple_params_t *params)
//{
//    int err = 0;
//
//    lmp1602x1_amp_dac_a_p_0_t adc_a_0;
//    lmp1602x1_amp_dac_b_p_0_t adc_b_0;
//    lmp1602x1_stim_on_a_0_t stim_on_a_0;
//    lmp1602x1_stim_on_b_0_t stim_on_b_0;
//    lmp1602x1_stim_on_a_1_t stim_on_a_1;
//    lmp1602x1_stim_on_b_1_t stim_on_b_1;
//
//    uint32_t tphase1[4] = {0}, tphase2[4] = {0}, tphase3[4] = {0}, tinterphase1[4] = {0}, tinterphase2[4] = {0}, tinterpulse[4] = {0};
//    uint32_t total_clk[4] = {0};
//    uint32_t tphase3_save = 0;
//    uint32_t tinterpulse_save = 0;
//    uint32_t burst_tinter_save = 0;
//    uint16_t burst_tinter = 0;
//
//
//    uint16_t start_count;
//    //通过pwm_duty 推算出delay_cnt 让波形在能量最后
//    uint32_t power_len;
//    int move_len;
//    uint32_t start_len;
//    uint32_t pluse_len; //刺激波形长度
//
//    start:
//    for(uint8_t j=0; j<2; j++) {
//        for(uint8_t i=0; i<2; i++) { //一次性获取完所以程序的时间值
//            logInfo("params->program[%d][%d].tphase1 = %d i+j*2 = %d",j,i, params->program[j][i].tphase1, i+j*2);
//            if(params->program[j][i].tphase1 < 2) {
//                continue;
//            }
//            tphase1[i+j*2]      = lmp_us_to_clk(params->program[j][i].tphase1, params->phase.stim_clk_div);//设置正向脉宽
//            tphase2[i+j*2]      = lmp_us_to_clk(params->program[j][i].tphase2, params->phase.stim_clk_div);//设置负向脉宽
//            tphase3[i+j*2]      = lmp_us_to_clk(params->program[j][i].tphase3, params->phase.stim_clk_div);//电荷平衡时间
//            tinterphase1[i+j*2] = lmp_us_to_clk(params->program[j][i].tinterphase1, params->phase.stim_clk_div);//设置正向脉宽后的延时
//            tinterphase2[i+j*2] = lmp_us_to_clk(params->program[j][i].tinterphase2, params->phase.stim_clk_div);//设置负向脉宽后的延时
//            total_clk[i+j*2]    = lmp_us_to_clk(1000000 / params->base_freq, params->phase.stim_clk_div) * params->program[j][i].multiple - 1;
//
//            if(params->pulse_mode == LMP_SIMPLE_MODE) { //简单模式  和多程序
//                logInfo("total_clk[%d] = %d", i+j*2,  total_clk[i+j*2]); //计算刺激波形的总clk数
//                    tinterpulse_save = total_clk[i+j*2] - tphase1[i+j*2] - tphase2[i+j*2] - tinterphase1[i+j*2] - tinterphase2[i+j*2] - tphase3[i+j*2];
//                    if(tinterpulse_save >= 0xffff) {
//                        params->phase.stim_clk_div++;
//                        if(params->phase.stim_clk_div>63) {
//                            logInfo("params->phase.stim_clk_div>63 return 0; !!!!!!!!!!!");
//                            return 0;
//                        }
//                        logInfo("[error hz moro min] !!!!!!!!!");
//                        goto start;
//                    } else {
//                        tinterpulse[i+j*2] = total_clk[i+j*2] - tphase1[i+j*2] - tphase2[i+j*2] - tinterphase1[i+j*2] - tinterphase2[i+j*2] - tphase3[i+j*2];
//                    }
//            } else if(params->pulse_mode == LMP_BURST_MODE) {
//                tinterpulse[i+j*2] = lmp_us_to_clk(params->program[j][i].tinterpulse, params->phase.stim_clk_div);//设置负向脉宽后的延时
//                burst_tinter_save = total_clk[i+j*2] - (tphase1[i+j*2] + tphase2[i+j*2] + tphase3[i+j*2] + tinterphase1[i+j*2] + tinterphase2[i+j*2] + tinterpulse[i+j*2]) * params->burst_pulse_cnt;
//                if(burst_tinter_save >= 0xffff) {
//                    params->phase.stim_clk_div++;
//                    if(params->phase.stim_clk_div>63) {
//                        return 0;
//                    }
//                    logInfo("[error hz moro min] !!!!!!!!!");
//                    goto start;
//                }
//            }
//
//            if(params->pwm_sync_mode) {
//                pluse_len = pluse_len + tphase1[i+j*2] + tphase2[i+j*2] + tinterphase1[i+j*2] + tinterphase2[i+j*2];
//                logInfo("pluse_len = %d", pluse_len);
//            }
//        }
//    }
//
//    if(params->pulse_mode == LMP_BURST_MODE) {
//        lmp1602x1_tinterburst_t burst = {0};
//        burst.value = burst_tinter_save;
//        err = x5_cmd_write(LMP1602X1_REG_08_TINTERBURST, burst.value);
//        CHECK_ERR(err);
//
//        lmp1602x1_tdefine_t tdefine;
//        tdefine.bits.stim_td2 = 0;
//        tdefine.bits.stim_td3 = 0x1f;
//        tdefine.bits.pulse_cnt = params->burst_pulse_cnt;
//        err = lmp_cmd_write(LMP1602X1_REG_05_TDEFINE, tdefine.value);
//        CHECK_ERR(err);
//    }
//
//    if(params->p_en.en_0 == 1 || params->p_en.en_1 == 1) {
//        imp_ctl_choice(0, 1, 0, 0, 0);  //同步基准选择
//    } else {
//        imp_ctl_choice(0, 2, 0, 0, 0);  //同步基准选择
//    }
//
//    if(params->pwm_sync_mode) {
//            power_len = (1000*1000 / params->base_freq) * ( params->pwm_duty)/100; //能量时间长度us 322为delay_cnt = 0 的位置
//            if(params->base_freq > 10000) {
//                start_count = 0x1;
//            } else {
//                if((power_len / 3) > 500) {
//                    if(params->base_freq > 9) {
//                        start_count = 0x1F;
//                    } else {
//                        start_count = 0x1FF;
//                    }
//                } else {
//                    start_count =  power_len / 8 ;
//                }
//            }
//
//            lmp1602x1_rfid_cfg3_t rfid_cfg3;
//            rfid_cfg3.bits.demod_pon_cnt = start_count;  //过滤demod噪声翻转
//            rfid_cfg3.bits.rfid_tx_data_period = 0x0a;
//            err = lmp_cmd_write(LMP1602X1_REG_0F_RFID_CFG3, rfid_cfg3.value);
//            CHECK_ERR(err);
//
//            power_len = lmp_us_to_clk(power_len, params->phase.stim_clk_div);
//
//            if(tinterpulse[0] > (power_len*3/4)) {
//                if(params->base_freq > 9) {
//                    tinterpulse[0] =  tinterpulse[0] - power_len*3/4;
//                } else {
//                    tinterpulse[0] = tinterpulse[0]*0.5;
//                }
//            } else {
//                if(params->base_freq > 10000) {
//                    tinterpulse[0] = 1;
//                } else {
//                    tinterpulse[0] = tinterpulse[0]*0.5;
//                }
//            }
//
//        start1:
//            power_len = (1000*1000 / params->base_freq) * ( params->pwm_duty)/100; //能量时间长度us 322为delay_cnt = 0 的位置
//            power_len = lmp_us_to_clk(power_len, params->phase.stim_clk_div);
//
//            if(params->base_freq > 2000) {  //手动校准刺激波形在能量尾部
//                move_len = power_len - pluse_len ;
//            } else if(params->base_freq > 600) {
//                move_len = power_len - pluse_len - start_count - 4;
//            } else if(params->base_freq > 300) {
//                move_len = power_len - pluse_len - start_count - 8;
//            } else if(params->base_freq > 100) {
//                move_len = power_len - pluse_len - start_count - 18;
//            } else if(params->base_freq > 50) {
//                move_len = power_len - pluse_len - start_count - 43;
//            } else if(params->base_freq > 30) {
//                move_len = power_len - pluse_len - start_count - 48;
//            } else if(params->base_freq > 14) {
//                move_len = power_len - pluse_len - start_count - 65;
//            } else if(params->base_freq > 12) {
//                move_len = power_len - pluse_len - start_count - 75;
//            } else if(params->base_freq > 9) {
//                move_len = power_len - pluse_len - start_count - 115;
//            } else { // 特殊处理
//                move_len = power_len - pluse_len - start_count - 200;
//            }
//            if(move_len>(8191/(params->phase.stim_clk_div+1))) {
//                (params->pwm_duty)--;
//                goto start1;
//            }
//            if(move_len < 1) {
//                move_len = 0;
//            }
//            params->pwm_delay_count = move_len;
//            lmp1602x1_pwm_mode_t pwm_mode;
//            pwm_mode.bits.pwm_mode = params->pwm_sync_mode;
//            pwm_mode.bits.pwm_power_dly = move_len*(params->phase.stim_clk_div+1);
//            err = lmp_cmd_write(LMP1602X1_REG_07_PWM_MODE, pwm_mode.value);
//            CHECK_ERR(err);
//    }
//
//    logInfo("params->p_en.en_0 = %d",params->p_en.en_0);
//    if(params->p_en.en_0) {
//        lmp1602x1_tphase1_t tphase1_0;
//        lmp1602x1_tphase2_t tphase2_0;
//        lmp1602x1_tinterphase_t tinterphase_0;
//        lmp1602x1_tphase3_t tphase3_0;
//        lmp1602x1_tinterpulse_t tinterpulse_0;
//        tphase1_0.bits.tphase1 = tphase1[0];
//        tphase2_0.bits.tphase2 = tphase2[0];
//        tphase3_0.bits.tphase3 = tphase3[0];
//        tinterphase_0.bits.tinterphase1 = tinterphase1[0];
//        tinterphase_0.bits.tinterphase2 = tinterphase2[0];
//        tinterpulse_0.bits.tinterpulse = tinterpulse[0];
//        err = lmp_cmd_write(LMP1602X1_REG_00_TPHASE1, tphase1_0.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_01_TPHASE2, tphase2_0.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_02_TINTERPHASE, tinterphase_0.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_03_TPHASE3, tphase3_0.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_04_TINTERPULSE, tinterpulse_0.value);
//        CHECK_ERR(err);
//    }
//    if(params->p_en.en_1) { //程序1波形时间
//        lmp1602x1_tphase1_a_1_t tphase1_a_1;
//        lmp1602x1_tphase2_a_1_t tphase2_a_1;
//        lmp1602x1_tphase3_a_1_t tphase3_a_1;
//        lmp1602x1_tinterphase_a_1_t tinterphase_a_1;
//        lmp1602x1_tinterpulse_a_1_t tinterpulse_a_1;
//        tphase1_a_1.bits.tphase1 = tphase1[1];
//        tphase2_a_1.bits.tphase2 = tphase2[1];
//        tphase3_a_1.bits.tphase3 = tphase3[1];
//        tinterphase_a_1.bits.tinterphase1 = tinterphase1[1];
//        tinterphase_a_1.bits.tinterphase2 = tinterphase2[1];
//        tinterpulse_a_1.bits.tinterpulse = tinterpulse[1];
//        err = lmp_cmd_write(LMP1602X1_REG_28_TPHASE1_A_1, tphase1_a_1.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_29_TPHASE2_A_1, tphase2_a_1.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_2A_TPHASE3_A_1, tphase3_a_1.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_2B_TINTERPHASE_A_1, tinterphase_a_1.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_2C_TINTERPULSE_A_1, tinterpulse_a_1.value);
//        CHECK_ERR(err);
//    }
//    if(params->p_en.en_2) {//程序0波形时间
//        lmp1602x1_tphase1_b_0_t tphase1_b_0;
//        lmp1602x1_tphase2_b_0_t tphase2_b_0;
//        lmp1602x1_tphase3_b_0_t tphase3_b_0;
//        lmp1602x1_tinterphase_b_0_t tinterphase_b_0;
//        lmp1602x1_tinterpulse_b_0_t tinterpulse_b_0;
//        tphase1_b_0.bits.tphase1 = tphase1[2];
//        tphase2_b_0.bits.tphase2 = tphase2[2];
//        tphase3_b_0.bits.tphase3 = tphase3[2];
//        tinterphase_b_0.bits.tinterphase1 = tinterphase1[2];
//        tinterphase_b_0.bits.tinterphase2 = tinterphase2[2];
//        tinterpulse_b_0.bits.tinterpulse = tinterpulse[2];
//        err = lmp_cmd_write(LMP1602X1_REG_21_TPHASE1_B_0, tphase1_b_0.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_22_TPHASE2_B_0, tphase2_b_0.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_23_TPHASE3_B_0, tphase3_b_0.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_24_TINTERPHASE_B_0, tinterphase_b_0.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_25_TINTERPULSE_B_0, tinterpulse_b_0.value);
//        CHECK_ERR(err);
//    }
//    if(params->p_en.en_3) { //程序1波形时间
//        lmp1602x1_tphase1_b_1_t tphase1_b_1;
//        lmp1602x1_tphase2_b_1_t tphase2_b_1;
//        lmp1602x1_tphase3_b_1_t tphase3_b_1;
//        lmp1602x1_tinterphase_b_1_t tinterphase_b_1;
//        lmp1602x1_tinterpulse_b_1_t tinterpulse_b_1;
//        tphase1_b_1.bits.tphase1 = tphase1[3];
//        tphase2_b_1.bits.tphase2 = tphase2[3];
//        tphase3_b_1.bits.tphase3 = tphase3[3];
//        tinterphase_b_1.bits.tinterphase1 = tinterphase1[3];
//        tinterphase_b_1.bits.tinterphase2 = tinterphase2[3];
//        tinterpulse_b_1.bits.tinterpulse = tinterpulse[3];
//        err = lmp_cmd_write(LMP1602X1_REG_2D_TPHASE1_B_1, tphase1_b_1.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_2E_TPHASE2_B_1, tphase2_b_1.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_2F_TPHASE3_B_1, tphase3_b_1.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_30_TINTERPHASE_B_1, tinterphase_b_1.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_31_TINTERPULSE_B_1, tinterpulse_b_1.value);
//        CHECK_ERR(err);
//    }
//
//    return err;
//}
//
///**
// * @brief 设置刺激幅度
// * @return 0-成功，非0-失败
// */
//static int __lmp_rf_set_range(lmp_simple_params_t *params)
//{
//    int err;
//    lmp1602x1_amp_dac_a_p_0_t dac_a_p_0;
//    lmp1602x1_amp_dac_a_n_0_t dac_a_n_0;
//    lmp1602x1_amp_dac_b_p_0_t dac_b_p_0;
//    lmp1602x1_amp_dac_b_n_0_t dac_b_n_0;
//    lmp1602x1_amp_dac_a_p_1_t dac_a_p_1;
//    lmp1602x1_amp_dac_a_n_1_t dac_a_n_1;
//    lmp1602x1_amp_dac_b_p_1_t dac_b_p_1;
//    lmp1602x1_amp_dac_b_n_1_t dac_b_n_1;
//
//    uint16_t amp_p[4],  amp_n[4];
//
//    for(uint8_t j=0; j<2; j++) {
//        for(uint8_t i=0; i<2; i++) { //一次性获取完所以程序的时间值
//            amp_p[i+j*2] = lmp_ua_to_dac(params->program[j][i].amplitude_p);
//            amp_n[i+j*2] = lmp_ua_to_dac(params->program[j][i].amplitude_n);
//        }
//    }
//
//    //配置电流
//    if(params->p_en.en_0) {//A程序0波形时间
//        dac_a_p_0.bits.amplitude_dac_a_p_0 = amp_p[0];
//        dac_a_p_0.bits.dual_a_en  = 1;  //使能A电流源  0：不使能A电流源（没有0至7电极），1：使能A电流源（使能0至7电极）
//        dac_a_n_0.bits.amplitude_dac_a_n_0 = amp_n[0];
//        err = lmp_cmd_write(LMP1602X1_REG_15_AMP_DAC_A_P_0, dac_a_p_0.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_16_AMP_DAC_A_N_0, dac_a_n_0.value);
//        CHECK_ERR(err);
//    }
//
//    if(params->p_en.en_1) { //A程序1波形时间
//        dac_a_p_1.bits.amplitude_dac_a_p_1 = amp_p[1];
//        dac_a_n_1.bits.amplitude_dac_a_n_1 = amp_n[1];
//        err = lmp_cmd_write(LMP1602X1_REG_19_AMP_DAC_A_P_1, dac_a_p_1.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_1A_AMP_DAC_A_N_1, dac_a_n_1.value);
//        CHECK_ERR(err);
//    }
//
//    if(params->p_en.en_2) {//B程序0波形时间
//        dac_b_p_0.bits.amplitude_dac_b_p_0 = amp_p[2];
//        dac_b_p_0.bits.dual_b_en  = 1; //B电流源使能信号，0：不使能B电流源（没有8至15电极），1：使能B电流源（使能8至15电极）
//        dac_b_n_0.bits.amplitude_dac_b_n_0 = amp_n[2];
//        err = lmp_cmd_write(LMP1602X1_REG_17_AMP_DAC_B_P_0, dac_b_p_0.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_18_AMP_DAC_B_N_0, dac_b_n_0.value);
//        CHECK_ERR(err);
//    }
//    if(params->p_en.en_3) { //B程序1波形时间
//        dac_b_p_1.bits.amplitude_dac_b_p_1 = amp_p[3];
//        dac_b_n_1.bits.amplitude_dac_b_n_1 = amp_n[3];
//        err = lmp_cmd_write(LMP1602X1_REG_1B_AMP_DAC_B_P_1, dac_b_p_1.value);
//        CHECK_ERR(err);
//        err = lmp_cmd_write(LMP1602X1_REG_1C_AMP_DAC_B_N_1, dac_b_n_1.value);
//        CHECK_ERR(err);
//    }
//    return err;
//}
//
///**
// * @brief 设置电极极性配置
// * @return 0-成功，非0-失败
// */
//static int __lmp_rf_set_polar(lmp_simple_params_t *params)
//{
//    int err;
//    lmp1602x1_stim_on_a_0_t stim_on_a_0;
//    lmp1602x1_stim_on_a_1_t stim_on_a_1;
//    lmp1602x1_stim_on_b_0_t stim_on_b_0;
//    lmp1602x1_stim_on_b_1_t stim_on_b_1;
//
//    if(params->p_en.en_0) {//A程序0波形时间
//        stim_on_a_0.bits.stim_on_a_0 = LMP1602X1_STIM_ON0(params->program[0][0].polar.dir, params->program[0][0].polar.on);
//        err = lmp_cmd_write(LMP1602X1_REG_1D_STIM_ON_A_0, stim_on_a_0.value);
//        CHECK_ERR(err);
//    }
//    if(params->p_en.en_1) {//A程序1波形时间
//        stim_on_a_1.bits.stim_on_a_1 = LMP1602X1_STIM_ON1(params->program[0][1].polar.dir, params->program[0][1].polar.on);
//        err = lmp_cmd_write(LMP1602X1_REG_1F_STIM_ON_A_1, stim_on_a_1.value);
//        CHECK_ERR(err);
//    }
//
//    if(params->p_en.en_2) {//B程序0波形时间
//        stim_on_b_0.bits.stim_on_b_0 = LMP1602X1_STIM_ON0(params->program[1][0].polar.dir, params->program[1][0].polar.on);
//        err = lmp_cmd_write(LMP1602X1_REG_1E_STIM_ON_B_0, stim_on_b_0.value);
//        CHECK_ERR(err);
//    }
//    if(params->p_en.en_3) {//B程序1波形时间
//        stim_on_b_1.bits.stim_on_b_1 = LMP1602X1_STIM_ON1(params->program[1][1].polar.dir, params->program[1][1].polar.on);
//        err = lmp_cmd_write(LMP1602X1_REG_20_STIM_ON_B_1, stim_on_b_1.value);
//        CHECK_ERR(err);
//    }
//
//    return err;
//}
///**
// * @brief 设置波形
// * @return 0-成功，非0-失败
// */
//static int __lmp_rf_set_pulse(lmp_simple_params_t *params)
//{
//    int err;
//    err = __lmp_rf_set_stim_params(params); /* 设置波形形状 */
//    CHECK_ERR(err);
//
//    err = __lmp_rf_set_range(params); /* 设置幅度 */
//    CHECK_ERR(err);
//
//    err = __lmp_rf_set_polar(params); /* 设置电极方向 */
//    CHECK_ERR(err);
//    return err;
//}
//
///**
// * @brief 启动脉冲输出(内部函数)
// * @param[in] mode 刺激模式(简单/爆发等)
// * @param[in] pulse_cnt 脉冲个数
// * @param[in] low_power 低功耗模式使能
// * @param[in] inturn_mode 交替模式使能
// * @return 0-成功，非0-失败
// */
//
//static int __lmp1602x1_rf_start_pulse(lmp_simple_params_t *params, uint8_t mode, uint8_t low_power, uint8_t program_cnt, uint8_t div, uint8_t start)
//{
//    int err = 0;
//
//    /* 配置刺激模式寄存器 */
//    lmp1602x1_stim_mode_t stim_mode = {0};
//
//    stim_mode.bits.stim_start = start;                /* 启动位 */
//    stim_mode.bits.para_vld = 1;                      /* 参数有效 */
//    stim_mode.bits.stim_stop = 0;                     /* 停止位 */
//    stim_mode.bits.stim_mode = mode;                  /* 刺激模式 */
//    stim_mode.bits.stim_low_power = low_power;        /* 低功耗模式 */
//    stim_mode.bits.program_cnt = program_cnt;         /* 程序个数 */
//    stim_mode.bits.stim_clk_div_value = div;          /* 刺激分频 */
//    stim_mode.bits.stim_auto_stop_en = 0;             /* 默认使能自动停止 */
//    stim_mode.bits.ab_alt_stim_en  = 1;           /* AB交替开关 实际使用不能让刺激波形重叠会导致电荷不平衡*/
//    err = lmp_cmd_write(LMP1602X1_REG_06_STIM_MODE, stim_mode.value);
//    CHECK_ERR(err);
//
////    /* 清除启动标志，开始刺激输出 */
////    stim_mode.bits.stim_stop = 0;
////    stim_mode.bits.para_vld = 0;
////    stim_mode.bits.stim_start = 0;
////    err = lmp_cmd_write(LMP1602X1_REG_06_STIM_MODE, stim_mode.value);
//
//    if(params->pwm_sync_mode) {
//        delay_ms(2000/params->base_freq);
//    }
//
//    return err;
//}
//
///**
// * @brief 配置简单模式刺激
// */
//int lmp_rf_config_simple(lmp_simple_params_t *params)
//{
//    int err;
//
//    __lmp_rf_reset(); //复位芯片  多程序刺激的时候 不复位会有问题 统一复位操作
//
//    err = __lmp_rf_set_pulse(params);
//    CHECK_ERR(err);
//
//
//    if(params->p_en.en_0 || params->p_en.en_1) {
//        if(params->p_en.en_0 && params->p_en.en_1) {
//            params->program_cnt = 1;
//            err = __lmp1602x1_rf_start_pulse(params, LMP_SIMPLE_MODE, params->low_power_mode, params->program_cnt, params->phase.stim_clk_div, 0); /* 启动输出 */
//        } else {
//            params->program_cnt = 0;
//            err = __lmp1602x1_rf_start_pulse(params, LMP_SIMPLE_MODE, params->low_power_mode, params->program_cnt, params->phase.stim_clk_div, 0); /* 启动输出 */
//        }
//    }
//
//    if(params->p_en.en_2 || params->p_en.en_3) {
//        if(params->p_en.en_2 && params->p_en.en_3) {
//            params->program_cnt = 3;
//            err = __lmp1602x1_rf_start_pulse(params, LMP_SIMPLE_MODE, params->low_power_mode, params->program_cnt, params->phase.stim_clk_div, 1); /* 启动输出 */
//        } else {
//            params->program_cnt = 2;
//            err = __lmp1602x1_rf_start_pulse(params, LMP_SIMPLE_MODE, params->low_power_mode, params->program_cnt, params->phase.stim_clk_div, 1); /* 启动输出 */
//        }
//    }
//    CHECK_ERR(err);
//    return err;
//}
//
///**
// * @brief 配置爆发模式刺激
// */
//int lmp_rf_config_burst(lmp_simple_params_t *params)
//{
//    int err = 0;
//
//    __lmp_rf_reset(); //复位芯片  多程序刺激的时候 不复位会有问题 统一复位操作
//    err = __lmp_rf_set_pulse(params);
//    CHECK_ERR(err);
//    err = __lmp1602x1_rf_start_pulse(params, LMP_BURST_MODE, params->low_power_mode, params->program_cnt, params->phase.stim_clk_div, 1); /* 启动输出 */
//    CHECK_ERR(err);
//    return err;
//}
//
