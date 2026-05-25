/*
 * app_ble_data.c
 *
 *  Created on: 2026年4月16日
 *      Author: BLE Protocol v1.0.0
 *
 *  Description: BLE数据处理模块实现
 *               负责解析和处理APP通过蓝牙发送的各种命令
 *               协议版本: v1.0.0
 *               参考文档: 《体外刺激器版程控仪与APP通信接口协议》
 */

#include "app_ble_data.h"
#include "app_verify_ble.h"
#include "main_user.h"
#include "leadinno_app.h"
#include "bsp_led.h"
#include <string.h>
#include <stdio.h>

/*==============================================================================
 * 全局变量定义
 *============================================================================*/
user_ble_data_t ble;                /**< BLE数据管理结构体 */

#define DEBUG_ENABLE  1


/*==============================================================================
 * 辅助函数声明
 *============================================================================*/
static int save_packet_data(uint8_t cmd, const char *dir_name, uint8_t *data, uint16_t index, uint16_t len);
static int read_packet_data(uint8_t cmd, const char *dir_name, uint16_t packet_count);

#if DEBUG_ENABLE
void print_program_01(cmd_01_set_group_t *param)
{
    if (param == NULL) return;

    uint8_t group_id = (param->progID >> 4) & 0x0F;
    uint8_t prog_id = param->progID & 0x0F;

    logInfo("=== Program Group Info (CMD 0x01) ===");

    /* 打印SN号 (12字节ASCII) */
    char sn_str[13] = {0};
    memcpy(sn_str, param->stim_sn, 12);
    for (int i = 0; i < 12; i++) {
        if (sn_str[i] < 0x20 || sn_str[i] > 0x7E) {
            sn_str[i] = '\0';
            break;
        }
    }
    logInfo("  Stim SN: %s", sn_str);

    logInfo("  Stim Efuse: 0x%X", param->stim_efuse);

    logInfo("  ProgID: 0x%02X (Group ID: %d, Program ID: %d)",
            param->progID, group_id, prog_id);

    logInfo("  Test Mode: %d %s", param->test_mode,
            param->test_mode == 0 ? "(Normal)" : "(Test)");

    char name_str[16] = {0};
    memcpy(name_str, param->name, 15);
    for (int i = 0; i < 15; i++) {
        if (name_str[i] < 0x20 || name_str[i] > 0x7E) {
            name_str[i] = '\0';
            break;
        }
    }
    logInfo("  Group Name: %s", name_str);

    logInfo("  Group Mode: %d %s", param->group_mode,
            param->group_mode == 0 ? "(Simple)" : "(Serial)");

    logInfo("  Group Switch: %d %s", param->group_switch,
            param->group_switch == 0 ? "(Off)" : "(On)");

    logInfo("  Database ID: %u (0x%08X)", param->id, param->id);

    logInfo("  Version Info:");
    logInfo("    Doctor Timestamp: %u (0x%08X)",
            param->ver.doctorTimestamp, param->ver.doctorTimestamp);
    logInfo("    Patient Timestamp: %u (0x%08X)",
            param->ver.patientTimestamp, param->ver.patientTimestamp);

    logInfo("  Delete Flag: %d-%s", param->delete_flag,
            param->delete_flag == 0 ? "(Not Deleted)" : "(Deleted)");
    logInfo("=====================================");
}

void print_program_02(cmd_02_get_group_t *param)
{
    if (param == NULL) return;
    uint8_t group_id = (param->progID >> 4) & 0x0F;
    logInfo("  ProgID: 0x%02X (Group ID: %d)", param->progID, group_id);
}

void print_program_03(cmd_03_set_program_t *param)
{
    if (param == NULL) return;

    uint8_t group_id = (param->progID >> 4) & 0x0F;
    uint8_t prog_id = param->progID & 0x0F;

    logInfo("=== Program Parameter Info (CMD 0x03) ===");

    char sn_str[13] = {0};
    memcpy(sn_str, param->stim_sn, 12);
    logInfo("  Stim SN: %.12s", sn_str);

    logInfo("  Stim Efuse: 0x%X", param->stim_efuse);

    logInfo("  ProgID: 0x%02X (Group ID: %d, Program ID: %d)",
            param->progID, group_id, prog_id);

    logInfo("  Switches: 0x%04X", *(uint16_t*)&param->switches);
    logInfo("    - Prog Switch: %d", param->switches.prog_switch);
    logInfo("    - Test Mode: %d", param->switches.test_mode);
    logInfo("    - Cycle Time Switch: %d", param->switches.cycle_time_switch);
    logInfo("    - Cycle Interval Switch: %d", param->switches.cycle_interval_switch);
    logInfo("    - Run Time Switch: %d", param->switches.run_time_switch);
    logInfo("    - Interval Time Switch: %d", param->switches.interval_time_switch);
    logInfo("    - Doctor Mode: %d", param->switches.doctor_mode);

    logInfo("  Program Parameters:");
    logInfo("    - Program Mode: %d", param->prog_params.program_mode);
    logInfo("    - Cycle Mode: %d", param->prog_params.cycle_mode);
    logInfo("    - Run Time: %d min", param->prog_params.run_time);
    logInfo("    - Run Interval Time: %d min", param->prog_params.run_interval_time);
    logInfo("    - Cycle Start Time: %d s", param->prog_params.cycle_start_time);
    logInfo("    - Cycle Interval: %d s", param->prog_params.cycle_interval);
    logInfo("    - Doctor Run Time: %d min", param->prog_params.doc_run_time);
    logInfo("    - Doctor Run Interval: %d min", param->prog_params.doc_run_interval_time);
    logInfo("    - Doctor Cycle Start: %d s", param->prog_params.doc_cycle_start_time);
    logInfo("    - Doctor Cycle Interval: %d s", param->prog_params.doc_cycle_interval);

    logInfo("  Stimulus Parameters:");
    logInfo("    - Frequency: %d Hz", param->stim_params.frequency);
    logInfo("    - Burst Frequency: %d Hz", param->stim_params.burst_frequency);
    logInfo("    - Burst Number: %d", param->stim_params.stimBurstNum);

    for (int i = 0; i < 4; i++) {
        area_params_t *area = (area_params_t*)&param->stim_params.areas[i];

        logInfo("  === Area %d ===", i);
        logInfo("    Area ID: 0x%02X", area->area_id);
        logInfo("    Patient Limits:");
        logInfo("      - Current: %d-%d uA (Doctor: %d uA)",
                area->limit.current_down, area->limit.current_up, area->limit.doc_current);
        logInfo("      - Pulse Width: %d-%d us (Doctor: %d us)",
                area->limit.pulse_width_down, area->limit.pulse_width_up, area->limit.doc_pulse_width);
        logInfo("      - Frequency: %d-%d Hz (Doctor: %d Hz)",
                area->limit.freq_down, area->limit.freq_up, area->limit.doc_freq);

        logInfo("    Area Switch: %d", area->area_switch);
        logInfo("    Soft Start Switch: %d", area->soft_switch);
        logInfo("    Soft Start Time: %d s", area->soft_start);
        logInfo("    Stim Polar: 0x%04X", area->stimPolar);
        logInfo("    Stim On: 0x%04X", area->stimOn);
        logInfo("    Pulse Width Ratio: 0x%02X", area->pulse_width_ratio);
        logInfo("    Inter-Phase Interval: %d us", area->interPhase);
        logInfo("    Current: %d uA", area->current);
        logInfo("    Pulse Width: %d us", area->pulse_width);
    }
    logInfo("=====================================");
}

void print_program_05(cmd_05_set_switch_t *param)
{
    if (param == NULL) return;

    uint16_t progID = param->progID;
    uint8_t group_id = (progID >> 8) & 0x0F;
    uint8_t prog_id = (progID >> 4) & 0x0F;
    uint8_t area_id = progID & 0x0F;

    char sn_str[25] = {0};
    memcpy(sn_str, param->stim_sn, 12);
    logInfo("  Stim SN: %.12s", sn_str);
    logInfo("  Stim Efuse: 0x%08X", param->stim_efuse);
    logInfo("  ProgID: 0x%04X (Group=%d, Program=%d, Area=%d)", param->progID, group_id, prog_id, area_id);
    logInfo("  Switch: %d %s", param->prog_switch, param->prog_switch ? "ON" : "OFF");
}

void print_program_07(cmd_07_set_device_switch_t *param)
{
    if (param == NULL) return;

    char sn_str[25] = {0};
    memcpy(sn_str, param->stim_sn, 12);
    logInfo("  Stim SN: %.12s", sn_str);
    logInfo("  Stim Efuse: 0x%08X", param->stim_efuse);
    logInfo("  Switch Bits: 0x%02X", param->switch_bits);
    logInfo("    - RF Disable: %d", (param->switch_bits >> 0) & 0x01);
    logInfo("    - Surgery Mode: %d", (param->switch_bits >> 1) & 0x01);
    logInfo("    - RF Blind Mode: %d", (param->switch_bits >> 2) & 0x01);
    logInfo("    - Log Zip: %d", (param->switch_bits >> 3) & 0x01);
    logInfo("    - Auto Open Stim: %d", (param->switch_bits >> 4) & 0x01);
}

void print_program_09(cmd_09_start_stim_t *param)
{
    if (param == NULL) return;

    char sn_str[25] = {0};
    memcpy(sn_str, param->stim_sn, 12);
    logInfo("  Stim SN: %.12s", sn_str);
    logInfo("  Stim Efuse: 0x%08X", param->stim_efuse);
    logInfo("  ProgID: 0x%02X", param->progID);
}

void print_program_0a(cmd_0a_stop_stim_t *param)
{
    if (param == NULL) return;

    char sn_str[25] = {0};
    memcpy(sn_str, param->stim_sn, 12);
    logInfo("  Stim SN: %.12s", sn_str);
    logInfo("  Stim Efuse: 0x%08X", param->stim_efuse);
}

void print_program_0b(cmd_0b_emergency_stop_t *param)
{
    if (param == NULL) return;
    logInfo("  WARNING: Emergency Stop!");
}


void print_program_15(cmd_15_get_program_count_t *param)
{
    if (param == NULL) return;
    char sn_str[25] = {0};
    memcpy(sn_str, param->stim_sn, 12);
    logInfo("  Stim SN: %.12s", sn_str);
    logInfo("  Stim Efuse: 0x%08X", param->stim_efuse);
}


void print_program_18(cmd_18_get_impedance_t *param)
{
    if (param == NULL) return;

    logInfo("  Stim ID: %d", param->stimID);
    char efuse_str[17] = {0};
    for(int i = 0; i < 8; i++) {
        sprintf(efuse_str + i*2, "%02X", param->stimSerialNum[i]);
    }
    logInfo("  Stim Serial Num: %s", efuse_str);
    logInfo("  Reference Pole: %d", param->pole);
}

void print_program_1c(cmd_1c_antenna_detect_t *param)
{
    if (param == NULL) return;
    logInfo("  State: %d %s", param->state,
            param->state ? "Start Detection" : "Stop Detection");
}

void print_program_1e(cmd_1e_set_temp_t *param)
{
    if (param == NULL) return;
    logInfo("  Temperature Limit: %d C", param->temperature);
}

void print_program_1f(cmd_1f_clear_all_t *param)
{
    if (param == NULL) return;
    logInfo("  WARNING: All configurations will be cleared!");
}

void print_program_20(cmd_20_factory_reset_t *param)
{
    if (param == NULL) return;
    logInfo("  WARNING: Factory reset, all data will be lost!");
}

void print_program_21(cmd_21_set_patient_info_t *param)
{
    if (param == NULL) return;
    logInfo("  Command: 0x%02X", 0x21);
    logInfo("  Info pointer: %p", param->info);
}

void print_program_22(cmd_22_set_dict_config_t *param)
{
    if (param == NULL) return;
    logInfo("  Command: 0x%02X", 0x22);
    logInfo("  Info pointer: %p", param->info);
}

void print_program_23(cmd_23_get_patient_info_t *param)
{
    if (param == NULL) return;
}

void print_program_24(cmd_24_get_dict_config_t *param)
{
    if (param == NULL) return;
}

void print_program_25(cmd_25_set_general_config_t *param)
{
    if (param == NULL) return;
    logInfo("  Command: 0x%02X", 0x25);
    logInfo("  Info pointer: %p", param->info);
}

void print_program_26(cmd_26_get_general_config_t *param)
{
    if (param == NULL) return;
}

void print_program_27(cmd_27_delete_stimulator_t *param)
{
    if (param == NULL) return;
    char sn_str[25] = {0};
    memcpy(sn_str, param->stim_sn, 12);
    logInfo("  Stim SN: %.12s", sn_str);
    logInfo("  Stim Efuse: 0x%08X", param->stim_efuse);
}

void print_program_28(cmd_28_set_rf_power_t *param)
{
    if (param == NULL) return;

    logInfo("  SubCmd: %d %s", param->subCmd,
            param->subCmd == 0 ? "(Temporary)" :
            param->subCmd == 1 ? "(Permanent)" : "(Restore Default)");

    if (param->subCmd != 2) {
        logInfo("  TxPower: %d", param->txPower);
        logInfo("  PWM Frequency: %d Hz", param->pwmFreq);

        for (int i = 0; i < 5; i++) {
            logInfo("  PowerGroup[%d]: Freq=%dHz, PW=%dus, Amp=%duA, Duty=%d%%, TxPower=%d",
                    i, param->powerGroup[i].freq, param->powerGroup[i].pw,
                    param->powerGroup[i].amp, param->powerGroup[i].pwmDuty,
                    param->powerGroup[i].dutyTxPower);
        }
    }
}

void print_program_28_get(cmd_28_get_rf_power_t *param)
{
    if (param == NULL) return;
    logInfo("  SubCmd: %d %s", param->subCmd,
            param->subCmd == 0 ? "(Temporary)" : "(Permanent)");
}

void print_program_29(cmd_29_get_vrecp_t *param)
{
    if (param == NULL) return;
}

#else
void print_program_01(cmd_01_set_group_t *param);
void print_program_02(cmd_02_get_group_t *param);
void print_program_03(cmd_03_set_program_t *param);
void print_program_05(cmd_05_set_switch_t *param);
void print_program_07(cmd_07_set_device_switch_t *param);
void print_program_09(cmd_09_start_stim_t *param);
void print_program_0a(cmd_0a_stop_stim_t *param);
void print_program_0b(cmd_0b_emergency_stop_t *param);
void print_program_0d(cmd_0d_bind_stimulator_t *param);
void print_program_13(cmd_13_set_general_config_t *param);
void print_program_15(cmd_15_get_program_count_t *param);
void print_program_16(cmd_16_clear_program_t *param);
void print_program_18(cmd_18_get_impedance_t *param);
void print_program_1c(cmd_1c_antenna_detect_t *param);
void print_program_1e(cmd_1e_set_temp_t *param);
void print_program_1f(cmd_1f_clear_all_t *param);
void print_program_20(cmd_20_factory_reset_t *param);
void print_program_21(cmd_21_set_patient_info_t *param);
void print_program_22(cmd_22_set_dict_config_t *param);
void print_program_23(cmd_23_get_patient_info_t *param);
void print_program_24(cmd_24_get_dict_config_t *param);
void print_program_25(cmd_25_set_general_config_t *param);
void print_program_26(cmd_26_get_general_config_t *param);
void print_program_27(cmd_27_delete_stimulator_t *param);
void print_program_28(cmd_28_set_rf_power_t *param);
void print_program_28_get(cmd_28_get_rf_power_t *param);
void print_program_29(cmd_29_get_vrecp_t *param);
#endif /* DEBUG_ENABLE */

/*==============================================================================
 * 命令处理函数 (根据协议文档v1.0.0修订)
 *============================================================================*/

/* 6.1 (0x01) 设置程序组参数 */
int handle_set_program_group_01(uint8_t cmd, cmd_01_set_group_t *param, uint16_t len)
{

    logInfo("=== CMD 0x01: Set Program Group ===");
    print_program_01(param);

    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.2 (0x02) 获取程序组参数 */
int handle_get_program_group_02(uint8_t cmd, cmd_02_get_group_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x02: Get Program Group ===");
    print_program_02(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.3 (0x03) 设置程序参数 */
int handle_set_program_param_03(uint8_t cmd, cmd_03_set_program_t *param, uint16_t len)
{
    logInfo("=== CMD 0x03: Set Program Parameter ===");
    print_program_03(param);

    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.4 (0x04) 获取程序参数 */
int handle_get_program_param_04(uint8_t cmd, cmd_04_get_program_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x04: Get Program Parameter ===");
#if BLE_TEST_MODE
    uint8_t key_status[2];
    if(len == 0) {//获取开关状态
        send_normal_frame_to_app(cmd, 0, key_status, 2);
    } else {
        send_normal_frame_to_app(cmd, 0, key_status, 1);
    }
#else
    send_normal_frame_to_app(cmd, 0, NULL, 0);
#endif

    return 0;
}

/* 6.5 (0x05) 设置程序开关 */
int handle_set_program_switch_05(uint8_t cmd, cmd_05_set_switch_t *param, uint16_t len)
{
    if (param == NULL || len < sizeof(cmd_05_set_switch_t)) {
        logError("handle_set_program_switch_05: invalid parameter");
        return -1;
    }

    logInfo("=== CMD 0x05: Set Program Switch ===");
    print_program_05(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.6 (0x06) 获取程序开关 */
int handle_get_program_switch_06(uint8_t cmd, cmd_06_get_switch_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x06: Get Program Switch ===");
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.7 (0x07) 设置开关状态(设备级) */
int handle_set_device_switch_07(uint8_t cmd, cmd_07_set_device_switch_t *param, uint16_t len)
{
    logInfo("=== CMD 0x07: Set Device Switch ===");
//    print_program_07(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.8 (0x08) 获取开关状态 */
int handle_get_device_switch_08(uint8_t cmd, cmd_08_get_device_switch_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x08: Get Device Switch ===");
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.9 (0x09) 开始刺激 */
int handle_start_stim_09(uint8_t cmd, cmd_09_start_stim_t *param, uint16_t len)
{

    logInfo("=== CMD 0x09: Start Stimulation ===");
    print_program_09(param);

    send_mailbox_to_app_user(MSG_START_STIM, NULL, 0);
    user.stim_stata = STIM_STATE_RUNNING;
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    send_stim_status(STIM_STATE_RUNNING);

    return 0;
}

/* 6.10 (0x0A) 停止刺激 */
int handle_stop_stim_0a(uint8_t cmd, cmd_0a_stop_stim_t *param, uint16_t len)
{

    logInfo("=== CMD 0x0A: Stop Stimulation ===");
    print_program_0a(param);

    send_mailbox_to_app_user(MSG_STOP_STIM, NULL, 0);
    user.stim_stata = STIM_STATE_STOPPED;
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    send_stim_status(STIM_STATE_STOPPED);

    return 0;
}

/* 6.11 (0x0B) 紧急停止 */
int handle_emergency_stop_0b(uint8_t cmd, cmd_0b_emergency_stop_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x0B: Emergency Stop ===");
    print_program_0b(param);

    send_mailbox_to_app_user(MSG_STOP_STIM, NULL, 0);
    user.stim_stata = STIM_STATE_ABNORMAL_STOP;
    send_normal_frame_to_app(cmd, 0, NULL, 0);

    return 0;
}

/* 6.12 (0x0C) 获取设备状态 */
int handle_get_device_status_0c(uint8_t cmd, cmd_0c_get_status_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x0C: Get Device Status ===");
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.13 (0x0D) 设置升级固件大小 */
int handle_set_fw_size_0d(uint8_t cmd, cmd_0d_set_fw_size_t *param, uint16_t len)
{

    logInfo("=== CMD 0x0D: Set Firmware Size ===");
    logInfo("  Firmware Size: %u bytes", param->size);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.14 (0x0E) 发送升级固件包 */
int handle_send_fw_packet_0e(uint8_t cmd, cmd_0e_send_fw_packet_t *param, uint16_t len)
{
    logInfo("=== CMD 0x0E: Send Firmware Packet ===");
    logInfo("  Offset: 0x%08X", param->offset);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.15 (0x0F) 获取日志信息 */
int handle_get_log_0f(uint8_t cmd, cmd_0f_get_log_t *param, uint16_t len)
{

    logInfo("=== CMD 0x0F: Get Log ===");
    logInfo("  Offset: %u", param->offset);
    logInfo("  Size: %u", param->size);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.16 (0x10) 清除全部日志信息 */
int handle_clear_log_10(uint8_t cmd, cmd_10_clear_log_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x10: Clear All Log ===");
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.17 (0x11) 获取当前日志使用率 */
int handle_get_log_usage_11(uint8_t cmd, cmd_11_get_log_usage_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x11: Get Log Usage ===");
    cmd_11_ack_log_usage_t usage;
    usage.percent = 25;
    usage.log_size = 102400;
    send_normal_frame_to_app(cmd, 0, (uint8_t *)&usage, sizeof(usage));
    return 0;
}

/* 6.18 (0x12) 停止获取日志信息 */
int handle_stop_log_12(uint8_t cmd, cmd_12_stop_log_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x12: Stop Getting Log ===");
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.19 (0x13) 删除某个刺激器的相关配置 */
int handle_delete_stimulator_13(uint8_t cmd, cmd_13_delete_stimulator_t *param, uint16_t len)
{

    logInfo("=== CMD 0x13: Delete Stimulator Config ===");
    char sn_str[13] = {0};
    memcpy(sn_str, param->stim_sn, 12);
    logInfo("  Stim SN: %.12s", sn_str);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.20 (0x14) 清除指定的程序组/程序/区域 */
int handle_clear_program_14(uint8_t cmd, cmd_14_clear_program_t *param, uint16_t len)
{

    logInfo("=== CMD 0x14: Clear Program ===");

    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.21 (0x15) 获取程序组和程序ID */
int handle_get_program_count_15(uint8_t cmd, cmd_15_get_program_count_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x15: Get Program Group and ID ===");
    print_program_15(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.22 (0x16) 修改天线位置判定条件 */
int handle_antenna_threshold_16(uint8_t cmd, uint8_t *data, uint16_t len)
{
    logInfo("=== CMD 0x16: Antenna Threshold ===");

    uint8_t subCmd = data[1];
    logInfo("  SubCmd: 0x%02X", subCmd);

    if (subCmd == 0x00 && len >= 4) {
        uint8_t txPower = data[2];
        uint8_t voltage = data[3];
        logInfo("  Set: TxPower=%d, Voltage=%dV", txPower, voltage);
    } else if (subCmd == 0x01) {
        logInfo("  Get threshold");
        cmd_16_ack_antenna_threshold_t ack;
        ack.txPower = 15;
        ack.voltage = 5;
        send_normal_frame_to_app(cmd, 0, (uint8_t *)&ack, sizeof(ack));
        return 0;
    } else if (subCmd == 0x02) {
        logInfo("  Restore default");
    } else {
        logError("Invalid subCmd: 0x%02X", subCmd);
        send_err_frame_to_app(cmd, ERR_PARAM_RANGE);
        return -1;
    }

    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.23 (0x17) 获取刺激器版本 */
int handle_get_stim_version_17(uint8_t cmd, cmd_17_get_stim_version_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x17: Get Stimulator Version ===");
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.24 (0x18) 获取刺激器电极一组阻抗值 */
int handle_get_impedance_18(uint8_t cmd, cmd_18_get_impedance_t *param, uint16_t len)
{
    logInfo("=== CMD 0x18: Get Impedance Group ===");
    print_program_18(param);

    int err = leadinno_crtl(LEAD_GET_RES);
    if (err == 0) {
        cmd_18_ack_impedance_t ack;
        memcpy(ack.resistance, lead.get_res, sizeof(ack.resistance));
        send_normal_frame_to_app(cmd, 0, (uint8_t *)&ack, sizeof(ack));
    } else {
        logError("Failed to get impedance, error: %d", err);
        send_err_frame_to_app(cmd, ERR_STIM_COMM_FAIL);
    }
    return 0;
}

/* 6.25 (0x19) 获取当前连接刺激器序列号 */
int handle_get_connected_stim_19(uint8_t cmd, cmd_19_get_connected_stim_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x19: Get Connected Stimulator ===");
#if BLE_TEST_MODE
    device_status_response_t test_data;
    send_normal_frame_to_app(cmd, 0, &test_data, sizeof(device_status_response_t));
#else
    send_normal_frame_to_app(cmd, 0, NULL, 0);
#endif
    return 0;
}

/* 6.26 (0x1A) 预留 */
int handle_reserved_1a(uint8_t cmd, uint8_t *data, uint16_t len)
{
    logInfo("=== CMD 0x1A: Reserved ===");
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.27 (0x1B) 检索刺激器电极一组阻抗值 */
int handle_search_impedance_1b(uint8_t cmd, cmd_1b_search_impedance_t *param, uint16_t len)
{
    int err;
    uint16_t get_res[16];

    lead.set_res_ch = param->pole-1;
    err = leadinno_crtl(LEAD_GET_RES);
    if(err == 0) {
        send_normal_frame_to_app(cmd, 0, (uint8_t *)lead.get_res, 32);
    } else {
        send_err_frame_to_app(cmd, ERR_STIM_COMM_FAIL);
    }

    return 0;
}

/* 6.28 (0x1C) 设置开始和暂停天线检测 */
int handle_antenna_detect_1c(uint8_t cmd, cmd_1c_antenna_detect_t *param, uint16_t len)
{
    logInfo("=== CMD 0x1C: Antenna Detect ===");
    print_program_1c(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.29 (0x1D) 预留 */
int handle_reserved_1d(uint8_t cmd, uint8_t *data, uint16_t len)
{
    logInfo("=== CMD 0x1D: Reserved ===");
    send_err_frame_to_app(cmd, ERR_GENERAL);
    return 0;
}

/* 6.30 (0x1E) 设置或获取刺激器温度上限值 */
int handle_temperature_limit_1e(uint8_t cmd, uint8_t *data, uint16_t len)
{
    logInfo("=== CMD 0x1E: Temperature Limit ===");

    if (len < 2) {
        send_err_frame_to_app(cmd, ERR_PDU_LENGTH);
        return -1;
    }

    uint8_t subCmd = data[1];
    logInfo("  SubCmd: 0x%02X", subCmd);

    if (subCmd == 0x00 && len >= 3) {
        uint8_t temperature = data[2];
        logInfo("  Set Temperature: %d C", temperature);
        print_program_1e((cmd_1e_set_temp_t*)data);
    } else if (subCmd == 0x01) {
        logInfo("  Get temperature limit");
        cmd_1e_ack_temp_t ack;
        ack.temperature = 50;
        send_normal_frame_to_app(cmd, 0, (uint8_t *)&ack, sizeof(ack));
        return 0;
    } else if (subCmd == 0x02) {
        logInfo("  Restore default");
    } else {
        logError("Invalid subCmd: 0x%02X", subCmd);
        send_err_frame_to_app(cmd, ERR_PARAM_RANGE);
        return -1;
    }

    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.31 (0x1F) 清除所有配置 */
int handle_clear_all_config_1f(uint8_t cmd, cmd_1f_clear_all_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x1F: Clear All Config ===");
    print_program_1f(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.32 (0x20) 恢复出厂设置 */
int handle_factory_reset_20(uint8_t cmd, cmd_20_factory_reset_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x20: Factory Reset ===");
    print_program_20(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.33 (0x21) 设置患者植入信息 */
int handle_set_patient_info_21(uint8_t cmd, cmd_21_set_patient_info_t *param, uint16_t index, uint16_t len)
{
    logInfo("=== CMD 0x21: Set Patient Info (index=%d, len=%d) ===", index, len);
    print_program_21(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.34 (0x22) 设置字典配置信息 */
int handle_set_dict_config_22(uint8_t cmd, cmd_22_set_dict_config_t *param, uint16_t index, uint16_t len)
{
    logInfo("=== CMD 0x22: Set Dictionary Config (index=%d, len=%d) ===", index, len);
    print_program_22(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.35 (0x23) 获取患者植入信息 */
int handle_get_patient_info_23(uint8_t cmd, cmd_23_get_patient_info_t *param, uint16_t len)
{
    (void)param;
    (void)len;
    logInfo("=== CMD 0x23: Get Patient Info ===");
#if BLE_TEST_MODE
    send_err_frame_to_app(CMD_26_GET_GENERAL_CONFIG, ERR_RESOURCE_EMPTY);
#else
    print_program_23(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
#endif



    return 0;
}

/* 6.36 (0x24) 获取字典配置信息 */
int handle_get_dict_config_24(uint8_t cmd, cmd_24_get_dict_config_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x24: Get Dictionary Config ===");
    print_program_24(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.37 (0x25) 设置配置信息 */
int handle_set_general_config_25(uint8_t cmd, cmd_25_set_general_config_t *param, uint16_t index, uint16_t len)
{
    logInfo("=== CMD 0x25: Set General Config (index=%d, len=%d) ===", index, len);
    print_program_25(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.38 (0x26) 获取配置信息 */
int handle_get_general_config_26(uint8_t cmd, cmd_26_get_general_config_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x26: Get General Config ===");
    print_program_26(param);
#if BLE_TEST_MODE
    send_err_frame_to_app(CMD_26_GET_GENERAL_CONFIG, ERR_RESOURCE_EMPTY);
#else
    send_normal_frame_to_app(cmd, 0, NULL, 0);
#endif
    return 0;
}

/* 6.39 (0x27) 删除某个刺激器的相关配置 */
int handle_delete_stimulator_27(uint8_t cmd, cmd_27_delete_stimulator_t *param, uint16_t len)
{

    logInfo("=== CMD 0x27: Delete Stimulator Config ===");
    print_program_27(param);
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 6.40 (0x28) 设置和获取射频功率值 */
int handle_rf_power_28(uint8_t cmd, uint8_t *data, uint16_t len)
{
    logInfo("=== CMD 0x28: RF Power ===");


    uint8_t subCmd = data[1];

    if (subCmd <= 0x01 && len >= 45) {
        logInfo("  Set RF Power");
        print_program_28((cmd_28_set_rf_power_t*)data);
        send_normal_frame_to_app(cmd, 0, NULL, 0);
    } else if (subCmd == 0x02) {
        logInfo("  Restore default");
        send_normal_frame_to_app(cmd, 0, NULL, 0);
    } else if (subCmd == 0x00 || subCmd == 0x01) {
        logInfo("  Get RF Power");
        print_program_28_get((cmd_28_get_rf_power_t*)data);
        send_normal_frame_to_app(cmd, 0, NULL, 0);
    } else {
        logError("Invalid subCmd: 0x%02X", subCmd);
        send_err_frame_to_app(cmd, ERR_PARAM_RANGE);
        return -1;
    }

    return 0;
}

/* 6.41 (0x29) 获取刺激器Vrecp电压 */
int handle_get_vrecp_29(uint8_t cmd, cmd_29_get_vrecp_t *param, uint16_t len)
{
    (void)param;
    (void)len;

    logInfo("=== CMD 0x29: Get Vrecp Voltage ===");
    print_program_29(param);

    int err = leadinno_crtl(LEAD_GET_VRECP);
    if (err == 0) {
        cmd_29_ack_vrecp_t vrecp;
        vrecp.Vrecp = lead.vrecp * 1000;
        logInfo("  Vrecp: %d mV", vrecp.Vrecp);
        send_normal_frame_to_app(cmd, 0, (uint8_t *)&vrecp, sizeof(vrecp));
    } else {
        logError("Failed to get Vrecp, error: %d", err);
        send_err_frame_to_app(cmd, ERR_STIM_COMM_FAIL);
    }
    return 0;
}

/* 绑定刺激器 (0x0D在协议中是设置固件大小，但原代码有0x0D绑定功能) */
int handle_bind_stimulator(uint8_t cmd, cmd_bind_stimulator_t *param, uint16_t len)
{
    logInfo("=== Bind Stimulator ===");
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/* 获取绑定刺激器 */
int handle_get_bound_stimulator(uint8_t cmd, uint8_t *data, uint16_t len)
{
    (void)data;
    (void)len;

    logInfo("=== Get Bound Stimulator ===");
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

/*==============================================================================
 * 辅助函数实现
 *============================================================================*/

static void record_command(uint8_t cmd)
{
    if (cmd != ble.record.old_cmd_index) {
        ble.record.new_cmd_flag = 1;
    }
    ble.record.old_cmd_index = cmd;
}

static int save_packet_data(uint8_t cmd, const char *dir_name, uint8_t *data,
                            uint16_t index, uint16_t len)
{
    char save_name[16];
    pack_info_t *pack_info = NULL;

    if (strcmp(dir_name, SAVE_DICTIONARIES_DIR) == 0) {
        pack_info = &ble.info.dictionaries;
    } else if (strcmp(dir_name, SAVE_PATIENT_DIR) == 0) {
        pack_info = &ble.info.patient;
    } else if (strcmp(dir_name, SAVE_CONFIG_DIR) == 0) {
        pack_info = &ble.info.config;
    } else {
        logError("Unknown dir_name: %s", dir_name);
        return -1;
    }

    if (ble.record.new_cmd_flag == 1) {
        ble.record.new_cmd_flag = 0;
        pack_info->pack_count = 0;
    }

    if (index != ble.record.old_index) {
        snprintf(save_name, sizeof(save_name), "%s%d", dir_name, pack_info->pack_count);
        logInfo("  Saving packet to %s, size=%d", save_name, len);

        if (save_user_blob(save_name, data, len) != 0) {
            logError("Failed to save packet %s", save_name);
            send_err_frame_to_app(cmd, ERR_IO);
            return -2;
        }
        pack_info->pack_count++;
    } else {
        logInfo("  Duplicate index %d, skip save", index);
    }

    ble.record.old_index = index;
    save_user_blob(SAVE_PACK_MUB, &ble.info, sizeof(save_info_t));
    send_normal_frame_to_app(cmd, 0, NULL, 0);
    return 0;
}

static int read_packet_data(uint8_t cmd, const char *dir_name, uint16_t packet_count)
{
    char read_name[16];
    uint8_t send_buffer[256];
    size_t read_len;

    logInfo("  Reading %d packets from %s", packet_count, dir_name);

    for (uint16_t i = 0; i < packet_count; i++) {
        snprintf(read_name, sizeof(read_name), "%s%d", dir_name, i);

        if (read_user_blob(read_name, send_buffer, sizeof(send_buffer), &read_len) != 0) {
            logError("Failed to read packet %s", read_name);
            send_err_frame_to_app(cmd, ERR_IO);
            return -1;
        }

        logInfo("  Sending packet %d, size=%d", i, (int)read_len);
        send_normal_frame_to_app(cmd, 0, send_buffer, read_len);
    }

    return 0;
}

void process_init(void)
{
    size_t get_len;

    if (read_user_blob(SAVE_PACK_MUB, &ble.info, sizeof(save_info_t), &get_len) != 0) {
        memset(&ble.info, 0, sizeof(save_info_t));
        logInfo("No saved packet info, initialized to zero");
    }

    memset(&ble.record, 0, sizeof(record_t));

    logInfo("BLE Data Module initialized");
    logInfo("  Patient packets: %d", ble.info.patient.pack_count);
    logInfo("  Dictionary packets: %d", ble.info.dictionaries.pack_count);
    logInfo("  Config packets: %d", ble.info.config.pack_count);
}

void send_stim_status(uint8_t status)
{
    cmd_0c_ack_status_t ack;

    memset(&ack, 0, sizeof(ack));
    ack.stimRun = status;
    ack.stimCon = STIM_CONN_CONNECTED;
    ack.batteryCapacity = 85;
    ack.batteryTemperature = 25;
    ack.batteryHealth = 1;
    ack.chargingState = 1;
    ack.left_time = 30;

    send_normal_frame_to_app(CMD_0C_GET_DEVICE_STATUS, 0,
                             (uint8_t *)&ack, sizeof(ack));
}

/*==============================================================================
 * 主命令分发函数 (根据协议文档v1.0.0修订)
 *============================================================================*/
int process_command(uint8_t cmd, uint16_t index, uint8_t *data, uint16_t len)
{
    int ret = 0;

    record_command(cmd);
    logInfo(">>>>>>>>>>>[CMD] 0x%02X, index=%d, len=%d", cmd, index, len);

    switch (cmd) {
        /* 6.1 设置程序组参数 */
        case CMD_01_SET_PROGRAM_GROUP:
            ret = handle_set_program_group_01(cmd, (cmd_01_set_group_t *)data, len);
            break;

        /* 6.2 获取程序组参数 */
        case CMD_02_GET_PROGRAM_GROUP:
            ret = handle_get_program_group_02(cmd, (cmd_02_get_group_t *)data, len);
            break;

        /* 6.3 设置程序参数 */
        case CMD_03_SET_PROGRAM_PARAM:
            ret = handle_set_program_param_03(cmd, (cmd_03_set_program_t *)data, len);
            break;

        /* 6.4 获取程序参数 */
        case CMD_04_GET_PROGRAM_PARAM:
            ret = handle_get_program_param_04(cmd, (cmd_04_get_program_t *)data, len);
            break;

        /* 6.5 设置程序开关 */
        case CMD_05_SET_PROGRAM_SWITCH:
            ret = handle_set_program_switch_05(cmd, (cmd_05_set_switch_t *)data, len);
            break;

        /* 6.6 获取程序开关 */
        case CMD_06_GET_PROGRAM_SWITCH:
            ret = handle_get_program_switch_06(cmd, (cmd_06_get_switch_t *)data, len);
            break;

        /* 6.7 设置开关状态(设备级) */
        case CMD_07_SET_DEVICE_SWITCH:
            ret = handle_set_device_switch_07(cmd, (cmd_07_set_device_switch_t *)data, len);
            break;

        /* 6.8 获取开关状态 */
        case CMD_08_GET_DEVICE_SWITCH:
            ret = handle_get_device_switch_08(cmd, (cmd_08_get_device_switch_t *)data, len);
            break;

        /* 6.9 开始刺激 */
        case CMD_09_START_STIM:
            ret = handle_start_stim_09(cmd, (cmd_09_start_stim_t *)data, len);
            break;

        /* 6.10 停止刺激 */
        case CMD_0A_STOP_STIM:
            ret = handle_stop_stim_0a(cmd, (cmd_0a_stop_stim_t *)data, len);
            break;

        /* 6.11 紧急停止 */
        case CMD_0B_EMERGENCY_STOP:
            ret = handle_emergency_stop_0b(cmd, (cmd_0b_emergency_stop_t *)data, len);
            break;

        /* 6.12 获取设备状态 */
        case CMD_0C_GET_DEVICE_STATUS:
            ret = handle_get_device_status_0c(cmd, (cmd_0c_get_status_t *)data, len);
            break;

        /* 6.13 设置升级固件大小 */
        case CMD_0D_SET_FW_SIZE:
            ret = handle_set_fw_size_0d(cmd, (cmd_0d_set_fw_size_t *)data, len);
            break;

        /* 6.14 发送升级固件包 */
        case CMD_0E_SEND_FW_PACKET:
            ret = handle_send_fw_packet_0e(cmd, (cmd_0e_send_fw_packet_t *)data, len);
            break;

        /* 6.15 获取日志信息 */
        case CMD_0F_GET_LOG:
            ret = handle_get_log_0f(cmd, (cmd_0f_get_log_t *)data, len);
            break;

        /* 6.16 清除全部日志信息 */
        case CMD_10_CLEAR_LOG:
            ret = handle_clear_log_10(cmd, (cmd_10_clear_log_t *)data, len);
            break;

        /* 6.17 获取当前日志使用率 */
        case CMD_11_GET_LOG_USAGE:
            ret = handle_get_log_usage_11(cmd, (cmd_11_get_log_usage_t *)data, len);
            break;

        /* 6.18 停止获取日志信息 */
        case CMD_12_STOP_LOG:
            ret = handle_stop_log_12(cmd, (cmd_12_stop_log_t *)data, len);
            break;

        /* 6.19 删除某个刺激器的相关配置 */
        case CMD_13_DELETE_STIMULATOR:
            ret = handle_delete_stimulator_13(cmd, (cmd_13_delete_stimulator_t *)data, len);
            break;

        /* 6.20 清除指定的程序组/程序/区域 */
        case CMD_14_CLEAR_PROGRAM:
            ret = handle_clear_program_14(cmd, (cmd_14_clear_program_t *)data, len);
            break;

        /* 6.21 获取程序组和程序ID */
        case CMD_15_GET_PROGRAM_COUNT:
            ret = handle_get_program_count_15(cmd, (cmd_15_get_program_count_t *)data, len);
            break;

        /* 6.22 修改天线位置判定条件 */
        case CMD_16_ANTENNA_THRESHOLD:
            ret = handle_antenna_threshold_16(cmd, data, len);
            break;

        /* 6.23 获取刺激器版本 */
        case CMD_17_GET_STIM_VERSION:
            ret = handle_get_stim_version_17(cmd, (cmd_17_get_stim_version_t *)data, len);
            break;

        /* 6.24 获取刺激器电极一组阻抗值 */
        case CMD_18_GET_IMPEDANCE:
            ret = handle_get_impedance_18(cmd, (cmd_18_get_impedance_t *)data, len);
            break;

        /* 6.25 获取当前连接刺激器序列号 */
        case CMD_19_GET_CONNECTED_STIM:
            ret = handle_get_connected_stim_19(cmd, (cmd_19_get_connected_stim_t *)data, len);
            break;

        /* 6.26 预留 */
        case CMD_1A_RESERVED:
            ret = handle_reserved_1a(cmd, data, len);
            break;

        /* 6.27 检索刺激器电极一组阻抗值 */
        case CMD_1B_SEARCH_IMPEDANCE:
            ret = handle_search_impedance_1b(cmd, (cmd_1b_search_impedance_t *)data, len);
            break;

        /* 6.28 设置开始和暂停天线检测 */
        case CMD_1C_ANTENNA_DETECT:
            ret = handle_antenna_detect_1c(cmd, (cmd_1c_antenna_detect_t *)data, len);
            break;

        /* 6.29 预留 */
        case CMD_1D_RESERVED:
            ret = handle_reserved_1d(cmd, data, len);
            break;

        /* 6.30 设置或获取刺激器温度上限值 */
        case CMD_1E_TEMP_LIMIT:
            ret = handle_temperature_limit_1e(cmd, data, len);
            break;

        /* 6.31 清除所有配置 */
        case CMD_1F_CLEAR_ALL_CONFIG:
            ret = handle_clear_all_config_1f(cmd, (cmd_1f_clear_all_t *)data, len);
            break;

        /* 6.32 恢复出厂设置 */
        case CMD_20_FACTORY_RESET:
            ret = handle_factory_reset_20(cmd, (cmd_20_factory_reset_t *)data, len);
            break;

        /* 6.33 设置患者植入信息 */
        case CMD_21_SET_PATIENT_INFO:
            ret = handle_set_patient_info_21(cmd, (cmd_21_set_patient_info_t *)data, index, len);
            break;

        /* 6.34 设置字典配置信息 */
        case CMD_22_SET_DICT_CONFIG:
            ret = handle_set_dict_config_22(cmd, (cmd_22_set_dict_config_t *)data, index, len);
            break;

        /* 6.35 获取患者植入信息 */
        case CMD_23_GET_PATIENT_INFO:
            ret = handle_get_patient_info_23(cmd, (cmd_23_get_patient_info_t *)data, len);
            break;

        /* 6.36 获取字典配置信息 */
        case CMD_24_GET_DICT_CONFIG:
            ret = handle_get_dict_config_24(cmd, (cmd_24_get_dict_config_t *)data, len);
            break;

        /* 6.37 设置配置信息 */
        case CMD_25_SET_GENERAL_CONFIG:
            ret = handle_set_general_config_25(cmd, (cmd_25_set_general_config_t *)data, index, len);
            break;

        /* 6.38 获取配置信息 */
        case CMD_26_GET_GENERAL_CONFIG:
            ret = handle_get_general_config_26(cmd, (cmd_26_get_general_config_t *)data, len);
            break;

        /* 6.39 删除某个刺激器的相关配置 */
        case CMD_27_DELETE_STIMULATOR_CONFIG:
            ret = handle_delete_stimulator_27(cmd, (cmd_27_delete_stimulator_t *)data, len);
            break;

        /* 6.40 设置和获取射频功率值 */
        case CMD_28_RF_POWER:
            ret = handle_rf_power_28(cmd, data, len);
            break;

        /* 6.41 获取刺激器Vrecp电压 */
        case CMD_29_GET_VRECP:
            ret = handle_get_vrecp_29(cmd, (cmd_29_get_vrecp_t *)data, len);
            break;

        default:
            ret = -1;
            logError("Unknown command: 0x%02X", cmd);
            send_err_frame_to_app(cmd, ERR_GENERAL);
            break;
    }

    return ret;
}
