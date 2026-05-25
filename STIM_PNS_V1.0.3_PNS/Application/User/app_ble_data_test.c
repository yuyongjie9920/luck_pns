#include "main_user.h"
#include "app_ble_data.h"

#if 1
/*==============================================================================
 * Command 0x01: Set Program Group Test
 * Usage: ble_cmd01_test <sn> <efuse> <progID> <test_mode> <name> <group_mode> <group_switch> <id> <doctor_ts> <patient_ts> <delete_flag>
 * Example: ble_cmd01_test 123456789012 12345678 12 00 746573745f67726f7570 00 01 1 102 102 00
 *============================================================================*/
static int ble_cmd01_test(int argc, char *argv[])
{
    if (argc < 12) {
        logError("Usage: ble_cmd01_test <sn> <efuse> <progID> <test_mode> <name> <group_mode> <group_switch> <id> <doctor_ts> <patient_ts> <delete_flag>");
        logError("Example: ble_cmd01_test 123456789012 12345678 12 00 746573745f67726f7570 00 01 1 102 102 00");
        return -1;
    }

    uint8_t uart_data[sizeof(cmd_01_set_group_t)] = {0};
    uint8_t uart_data_len = 0;

    int idx = 0;

    /* 1. 解析 stim_sn (12字节，支持ASCII字符串) */
    char *sn_str = (char*)argv[idx++];
    int sn_len = strlen(sn_str);
    if (sn_len > 12) {
        logError("SN too long: got %d, max 12", sn_len);
        return -2;
    }
    memset(&uart_data[uart_data_len], 0, 12);
    memcpy(&uart_data[uart_data_len], sn_str, sn_len);
    uart_data_len += 12;

    /* 2. 解析 stim_efuse (4字节) */
    char *efuse_str = (char*)argv[idx++];
    uint32_t efuse_val = (uint32_t)strtoul(efuse_str, NULL, 16);
    memcpy(&uart_data[uart_data_len], &efuse_val, 4);
    uart_data_len += 4;

    /* 3. 解析 progID */
    uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

    /* 4. 解析 test_mode */
    uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

    /* 5. 解析 name (15字节，30个十六进制字符) */
    char *name_str = (char*)argv[idx++];
    if (strlen(name_str) != 30) {
        logError("Name must be 30 hex characters (15 bytes), got %d", strlen(name_str));
        return -4;
    }
    for (int i = 0; i < 15; i++) {
        char hex_byte[3] = {name_str[i*2], name_str[i*2+1], 0};
        uart_data[uart_data_len++] = (uint8_t)strtol(hex_byte, NULL, 16);
    }

    /* 6. 解析 group_mode */
    uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

    /* 7. 解析 group_switch */
    uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

    /* 8. 解析 id (4字节) */
    uint32_t id_val = (uint32_t)strtoul((char*)argv[idx++], NULL, 16);
    memcpy(&uart_data[uart_data_len], &id_val, 4);
    uart_data_len += 4;

    /* 9. 解析 doctorTimestamp */
    uint32_t doctor_ts = (uint32_t)strtoul((char*)argv[idx++], NULL, 16);
    memcpy(&uart_data[uart_data_len], &doctor_ts, 4);
    uart_data_len += 4;

    /* 10. 解析 patientTimestamp */
    uint32_t patient_ts = (uint32_t)strtoul((char*)argv[idx++], NULL, 16);
    memcpy(&uart_data[uart_data_len], &patient_ts, 4);
    uart_data_len += 4;

    /* 11. 解析 delete_flag */
    uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

    if (uart_data_len != sizeof(cmd_01_set_group_t)) {
        logError("Data length mismatch: got %d, expected %d", uart_data_len, sizeof(cmd_01_set_group_t));
        return -5;
    }

    logDebug("Received %d bytes", uart_data_len);
    put_buf(uart_data, uart_data_len);

    cmd_01_set_group_t *param = (cmd_01_set_group_t*)uart_data;

    return handle_set_program_group_01(CMD_01_SET_PROGRAM_GROUP, param, uart_data_len);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd01_test, ble_cmd01_test, test set group command);

/*==============================================================================
 * Command 0x02: Get Program Group Test
 * Usage: ble_cmd02_test <progID>
 * Example: ble_cmd02_test 12
 *============================================================================*/
static int ble_cmd02_test(int argc, char *argv[])
{
    if (argc < 1) {
        logError("Usage: ble_cmd02_test <progID>");
        logError("Example: ble_cmd02_test 12");
        return -1;
    }

    uint8_t progID = (uint8_t)strtol((char*)argv[0], NULL, 16);
    cmd_02_get_group_t *param = (cmd_02_get_group_t*)&progID;
    return handle_get_program_group_02(CMD_02_GET_PROGRAM_GROUP, param, sizeof(progID));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd02_test, ble_cmd02_test, test get program group command);

/*==============================================================================
 * Command 0x03: Set Program Parameter Test
 *============================================================================*/
static int ble_cmd03_test(int argc, char *argv[])
{
    uint8_t uart_data[sizeof(cmd_03_set_program_t)] = {0};
    int uart_data_len = 0;
    int idx = 0;

    /* 1. stim_sn (12 bytes) */
    char *sn_str = (char*)argv[idx++];
    memset(&uart_data[uart_data_len], 0, 12);
    memcpy(&uart_data[uart_data_len], sn_str, strlen(sn_str) > 12 ? 12 : strlen(sn_str));
    uart_data_len += 12;

    /* 2. stim_efuse (4 bytes) */
    uint32_t efuse_val = (uint32_t)strtoul((char*)argv[idx++], NULL, 16);
    memcpy(&uart_data[uart_data_len], &efuse_val, 4);
    uart_data_len += 4;

    /* 3. progID (1 byte) */
    uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

    /* 4. switches (2 bytes) */
    uint16_t switches_val = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
    memcpy(&uart_data[uart_data_len], &switches_val, 2);
    uart_data_len += 2;

    /* 5. program_params (18 bytes) */
    program_params_t prog_params;
    prog_params.program_mode = (uint8_t)strtol((char*)argv[idx++], NULL, 16);
    prog_params.cycle_mode = (uint8_t)strtol((char*)argv[idx++], NULL, 16);
    prog_params.run_time = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
    prog_params.run_interval_time = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
    prog_params.cycle_start_time = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
    prog_params.cycle_interval = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
    prog_params.doc_run_time = prog_params.run_time;
    prog_params.doc_run_interval_time = prog_params.run_interval_time;
    prog_params.doc_cycle_start_time = prog_params.cycle_start_time;
    prog_params.doc_cycle_interval = prog_params.cycle_interval;

    memcpy(&uart_data[uart_data_len], &prog_params, sizeof(program_params_t));
    uart_data_len += sizeof(program_params_t);

    /* 6. stim_params base fields */
    uint16_t frequency = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
    memcpy(&uart_data[uart_data_len], &frequency, 2);
    uart_data_len += 2;

    uint16_t burst_frequency = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
    memcpy(&uart_data[uart_data_len], &burst_frequency, 2);
    uart_data_len += 2;

    uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

    /* 7. 配置4个区域参数 */
    for (int area = 0; area < 4; area++) {
        uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

        uint16_t current_down = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
        uint16_t current_up = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
        uint16_t pulse_width_down = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
        uint16_t pulse_width_up = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
        uint16_t freq_down = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
        uint16_t freq_up = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
        uint8_t current_limit_switch = (uint8_t)strtol((char*)argv[idx++], NULL, 16);
        uint8_t pulse_width_limit_switch = (uint8_t)strtol((char*)argv[idx++], NULL, 16);
        uint8_t freq_limit_switch = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

        memcpy(&uart_data[uart_data_len], &current_down, 2); uart_data_len += 2;
        memcpy(&uart_data[uart_data_len], &current_up, 2); uart_data_len += 2;
        memcpy(&uart_data[uart_data_len], &current_down, 2); uart_data_len += 2;
        memcpy(&uart_data[uart_data_len], &pulse_width_down, 2); uart_data_len += 2;
        memcpy(&uart_data[uart_data_len], &pulse_width_up, 2); uart_data_len += 2;
        memcpy(&uart_data[uart_data_len], &pulse_width_down, 2); uart_data_len += 2;
        memcpy(&uart_data[uart_data_len], &freq_down, 2); uart_data_len += 2;
        memcpy(&uart_data[uart_data_len], &freq_up, 2); uart_data_len += 2;
        memcpy(&uart_data[uart_data_len], &freq_down, 2); uart_data_len += 2;
        uart_data[uart_data_len++] = current_limit_switch;
        uart_data[uart_data_len++] = pulse_width_limit_switch;
        uart_data[uart_data_len++] = freq_limit_switch;

        uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);
        uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);
        uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

        uint16_t stimPolar = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
        uint16_t stimOn = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
        memcpy(&uart_data[uart_data_len], &stimPolar, 2); uart_data_len += 2;
        memcpy(&uart_data[uart_data_len], &stimOn, 2); uart_data_len += 2;

        for (int i = 0; i < 8; i++) {
            uint16_t ele_percent = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
            memcpy(&uart_data[uart_data_len], &ele_percent, 2);
            uart_data_len += 2;
        }

        uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);
        uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

        uint16_t current = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
        uint16_t pulse_width = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
        memcpy(&uart_data[uart_data_len], &current, 2); uart_data_len += 2;
        memcpy(&uart_data[uart_data_len], &pulse_width, 2); uart_data_len += 2;
    }

    if (uart_data_len != sizeof(cmd_03_set_program_t)) {
        logError("Data length mismatch: got %d, expected %d", uart_data_len, sizeof(cmd_03_set_program_t));
        return -5;
    }

    logInfo("=== Testing CMD 0x03: Set Program Parameter ===");
    logInfo("Total packet size: %d bytes", uart_data_len);
    put_buf(uart_data, uart_data_len);

    return handle_set_program_param_03(CMD_03_SET_PROGRAM_PARAM, (cmd_03_set_program_t*)uart_data, uart_data_len);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd03_test, ble_cmd03_test, test set program param command);

/*==============================================================================
 * Command 0x04: Get Program Parameter Test
 * Usage: ble_cmd04_test <progID>
 * Example: ble_cmd04_test 12
 *============================================================================*/
static int ble_cmd04_test(int argc, char *argv[])
{
    if (argc < 1) {
        logError("Usage: ble_cmd04_test <progID>");
        logError("Example: ble_cmd04_test 12");
        return -1;
    }

    uint8_t progID = (uint8_t)strtol((char*)argv[0], NULL, 16);
    cmd_04_get_program_t *param = (cmd_04_get_program_t*)&progID;
    return handle_get_program_param_04(CMD_04_GET_PROGRAM_PARAM, param, sizeof(progID));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd04_test, ble_cmd04_test, test get program param command);

/*==============================================================================
 * Command 0x05: Set Program Switch Test
 * Usage: ble_cmd05_test <sn> <efuse> <progID> <switch>
 * Example: ble_cmd05_test 123456789012 12345678 1200 01
 *============================================================================*/
static int ble_cmd05_test(int argc, char *argv[])
{
    if (argc < 4) {
        logError("Usage: ble_cmd05_test <sn> <efuse> <progID> <switch>");
        logError("Example: ble_cmd05_test 123456789012 12345678 1200 01");
        return -1;
    }

    uint8_t uart_data[sizeof(cmd_05_set_switch_t)] = {0};
    int uart_data_len = 0;
    int idx = 0;

    char *sn_str = (char*)argv[idx++];
    int sn_len = strlen(sn_str);
    if (sn_len > 12) {
        logError("SN too long: got %d, max 12", sn_len);
        return -2;
    }
    memset(&uart_data[uart_data_len], 0, 12);
    memcpy(&uart_data[uart_data_len], sn_str, sn_len);
    uart_data_len += 12;

    uint32_t efuse_val = (uint32_t)strtoul((char*)argv[idx++], NULL, 16);
    memcpy(&uart_data[uart_data_len], &efuse_val, 4);
    uart_data_len += 4;

    uint16_t progID = (uint16_t)strtol((char*)argv[idx++], NULL, 16);
    memcpy(&uart_data[uart_data_len], &progID, 2);
    uart_data_len += 2;

    uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

    logInfo("=== Testing CMD 0x05: Set Program Switch ===");
    cmd_05_set_switch_t *param = (cmd_05_set_switch_t*)uart_data;
    return handle_set_program_switch_05(CMD_05_SET_PROGRAM_SWITCH, param, uart_data_len);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd05_test, ble_cmd05_test, test set program switch command);

/*==============================================================================
 * Command 0x06: Get Program Switch Test
 * Usage: ble_cmd06_test
 * Example: ble_cmd06_test
 *============================================================================*/
static int ble_cmd06_test(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    logInfo("=== Testing CMD 0x06: Get Program Switch ===");
    return handle_get_program_switch_06(CMD_06_GET_PROGRAM_SWITCH, NULL, 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd06_test, ble_cmd06_test, test get program switch command);

/*==============================================================================
 * Command 0x07: Set Device Switch Test
 * Usage: ble_cmd07_test <sn> <efuse> <switch_bits>
 * Example: ble_cmd07_test 123456789012 12345678 01
 *============================================================================*/
static int ble_cmd07_test(int argc, char *argv[])
{
    if (argc < 3) {
        logError("Usage: ble_cmd07_test <sn> <efuse> <switch_bits>");
        logError("Example: ble_cmd07_test 123456789012 12345678 01");
        return -1;
    }

    uint8_t uart_data[sizeof(cmd_07_set_device_switch_t)] = {0};
    int uart_data_len = 0;
    int idx = 0;

    char *sn_str = (char*)argv[idx++];
    int sn_len = strlen(sn_str);
    if (sn_len > 12) {
        logError("SN too long: got %d, max 12", sn_len);
        return -2;
    }
    memset(&uart_data[uart_data_len], 0, 12);
    memcpy(&uart_data[uart_data_len], sn_str, sn_len);
    uart_data_len += 12;

    uint32_t efuse_val = (uint32_t)strtoul((char*)argv[idx++], NULL, 16);
    memcpy(&uart_data[uart_data_len], &efuse_val, 4);
    uart_data_len += 4;

    uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

    logInfo("=== Testing CMD 0x07: Set Device Switch ===");
    cmd_07_set_device_switch_t *param = (cmd_07_set_device_switch_t*)uart_data;
    return handle_set_device_switch_07(CMD_07_SET_DEVICE_SWITCH, param, uart_data_len);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd07_test, ble_cmd07_test, test set device switch command);

/*==============================================================================
 * Command 0x08: Get Device Switch Test
 * Usage: ble_cmd08_test
 * Example: ble_cmd08_test
 *============================================================================*/
static int ble_cmd08_test(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    logInfo("=== Testing CMD 0x08: Get Device Switch ===");
    return handle_get_device_switch_08(CMD_08_GET_DEVICE_SWITCH, NULL, 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd08_test, ble_cmd08_test, test get device switch command);

/*==============================================================================
 * Command 0x09: Start Stimulation Test
 * Usage: ble_cmd09_test <sn> <efuse> <progID>
 * Example: ble_cmd09_test 123456789012 12345678 01
 *============================================================================*/
static int ble_cmd09_test(int argc, char *argv[])
{
    if (argc < 3) {
        logError("Usage: ble_cmd09_test <sn> <efuse> <progID>");
        logError("Example: ble_cmd09_test 123456789012 12345678 01");
        return -1;
    }

    uint8_t uart_data[sizeof(cmd_09_start_stim_t)] = {0};
    int uart_data_len = 0;
    int idx = 0;

    char *sn_str = (char*)argv[idx++];
    int sn_len = strlen(sn_str);
    if (sn_len > 12) {
        logError("SN too long: got %d, max 12", sn_len);
        return -2;
    }
    memset(&uart_data[uart_data_len], 0, 12);
    memcpy(&uart_data[uart_data_len], sn_str, sn_len);
    uart_data_len += 12;

    uint32_t efuse_val = (uint32_t)strtoul((char*)argv[idx++], NULL, 16);
    memcpy(&uart_data[uart_data_len], &efuse_val, 4);
    uart_data_len += 4;

    uart_data[uart_data_len++] = (uint8_t)strtol((char*)argv[idx++], NULL, 16);

    logInfo("=== Testing CMD 0x09: Start Stimulation ===");
    cmd_09_start_stim_t *param = (cmd_09_start_stim_t*)uart_data;
    return handle_start_stim_09(CMD_09_START_STIM, param, uart_data_len);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd09_test, ble_cmd09_test, test start stimulation command);

/*==============================================================================
 * Command 0x0A: Stop Stimulation Test
 * Usage: ble_cmd0a_test <sn> <efuse>
 * Example: ble_cmd0a_test 123456789012 12345678
 *============================================================================*/
static int ble_cmd0a_test(int argc, char *argv[])
{
    if (argc < 2) {
        logError("Usage: ble_cmd0a_test <sn> <efuse>");
        logError("Example: ble_cmd0a_test 123456789012 12345678");
        return -1;
    }

    uint8_t uart_data[sizeof(cmd_0a_stop_stim_t)] = {0};
    int uart_data_len = 0;
    int idx = 0;

    char *sn_str = (char*)argv[idx++];
    int sn_len = strlen(sn_str);
    if (sn_len > 12) {
        logError("SN too long: got %d, max 12", sn_len);
        return -2;
    }
    memset(&uart_data[uart_data_len], 0, 12);
    memcpy(&uart_data[uart_data_len], sn_str, sn_len);
    uart_data_len += 12;

    uint32_t efuse_val = (uint32_t)strtoul((char*)argv[idx++], NULL, 16);
    memcpy(&uart_data[uart_data_len], &efuse_val, 4);
    uart_data_len += 4;

    logInfo("=== Testing CMD 0x0A: Stop Stimulation ===");
    cmd_0a_stop_stim_t *param = (cmd_0a_stop_stim_t*)uart_data;
    return handle_stop_stim_0a(CMD_0A_STOP_STIM, param, uart_data_len);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd0a_test, ble_cmd0a_test, test stop stimulation command);

/*==============================================================================
 * Command 0x0B: Emergency Stop Test
 * Usage: ble_cmd0b_test
 * Example: ble_cmd0b_test
 *============================================================================*/
static int ble_cmd0b_test(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    logInfo("=== Testing CMD 0x0B: Emergency Stop ===");
    return handle_emergency_stop_0b(CMD_0B_EMERGENCY_STOP, NULL, 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd0b_test, ble_cmd0b_test, test emergency stop command);

/*==============================================================================
 * Command 0x0C: Get Device Status Test
 * Usage: ble_cmd0c_test
 * Example: ble_cmd0c_test
 *============================================================================*/
static int ble_cmd0c_test(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    logInfo("=== Testing CMD 0x0C: Get Device Status ===");
    return handle_get_device_status_0c(CMD_0C_GET_DEVICE_STATUS, NULL, 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd0c_test, ble_cmd0c_test, test get device status command);

/*==============================================================================
 * Command 0x0D: Set Firmware Size Test
 * Usage: ble_cmd0d_test <size>
 * Example: ble_cmd0d_test 102400
 *============================================================================*/
static int ble_cmd0d_test(int argc, char *argv[])
{
    if (argc < 1) {
        logError("Usage: ble_cmd0d_test <size>");
        logError("Example: ble_cmd0d_test 102400");
        return -1;
    }

    cmd_0d_set_fw_size_t param;
    param.size = (uint32_t)strtoul((char*)argv[0], NULL, 0);

    logInfo("=== Testing CMD 0x0D: Set Firmware Size ===");
    logInfo("  Firmware Size: %u bytes (%.2f KB)", param.size, param.size / 1024.0);

    return handle_set_fw_size_0d(CMD_0D_SET_FW_SIZE, &param, sizeof(param));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd0d_test, ble_cmd0d_test, test set firmware size command);

/*==============================================================================
 * Command 0x0E: Send Firmware Packet Test
 * Usage: ble_cmd0e_test <offset> <hex_data>
 * Example: ble_cmd0e_test 00001000 0102030405060708090A0B0C0D0E0F10
 *============================================================================*/
static int ble_cmd0e_test(int argc, char *argv[])
{
    if (argc < 2) {
        logError("Usage: ble_cmd0e_test <offset> <hex_data>");
        logError("Example: ble_cmd0e_test 00001000 0102030405060708090A0B0C0D0E0F10");
        return -1;
    }

    cmd_0e_send_fw_packet_t param;
    memset(&param, 0, sizeof(param));

    param.offset = (uint32_t)strtoul((char*)argv[0], NULL, 16);

    char *data_str = (char*)argv[1];
    int data_str_len = strlen(data_str);
    int data_bytes = data_str_len / 2;

    if (data_bytes > 128) {
        logError("Data too long: got %d bytes, max 128", data_bytes);
        return -2;
    }

    for (int i = 0; i < data_bytes; i++) {
        char hex_byte[3] = {data_str[i*2], data_str[i*2+1], 0};
        param.data[i] = (uint8_t)strtol(hex_byte, NULL, 16);
    }

    logInfo("=== Testing CMD 0x0E: Send Firmware Packet ===");
    logInfo("  Offset: 0x%08X (%u)", param.offset, param.offset);
    logInfo("  Data size: %d bytes", data_bytes);

    return handle_send_fw_packet_0e(CMD_0E_SEND_FW_PACKET, &param, sizeof(param));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd0e_test, ble_cmd0e_test, test send firmware packet command);

/*==============================================================================
 * Command 0x0F: Get Log Test
 * Usage: ble_cmd0f_test <offset> <size>
 * Example: ble_cmd0f_test 0 1024
 *============================================================================*/
static int ble_cmd0f_test(int argc, char *argv[])
{
    if (argc < 2) {
        logError("Usage: ble_cmd0f_test <offset> <size>");
        logError("Example: ble_cmd0f_test 0 1024");
        return -1;
    }

    cmd_0f_get_log_t param;
    param.offset = (uint32_t)strtoul((char*)argv[0], NULL, 0);
    param.size = (uint32_t)strtoul((char*)argv[1], NULL, 0);

    logInfo("=== Testing CMD 0x0F: Get Log ===");
    logInfo("  Offset: %u", param.offset);
    logInfo("  Size: %u", param.size);

    return handle_get_log_0f(CMD_0F_GET_LOG, &param, sizeof(param));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd0f_test, ble_cmd0f_test, test get log command);

/*==============================================================================
 * Command 0x10: Clear Log Test
 * Usage: ble_cmd10_test
 * Example: ble_cmd10_test
 *============================================================================*/
static int ble_cmd10_test(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    logInfo("=== Testing CMD 0x10: Clear Log ===");
    return handle_clear_log_10(CMD_10_CLEAR_LOG, NULL, 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd10_test, ble_cmd10_test, test clear log command);

/*==============================================================================
 * Command 0x11: Get Log Usage Test
 * Usage: ble_cmd11_test
 * Example: ble_cmd11_test
 *============================================================================*/
static int ble_cmd11_test(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    logInfo("=== Testing CMD 0x11: Get Log Usage ===");
    return handle_get_log_usage_11(CMD_11_GET_LOG_USAGE, NULL, 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd11_test, ble_cmd11_test, test get log usage command);

/*==============================================================================
 * Command 0x12: Stop Log Test
 * Usage: ble_cmd12_test
 * Example: ble_cmd12_test
 *============================================================================*/
static int ble_cmd12_test(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    logInfo("=== Testing CMD 0x12: Stop Log ===");
    return handle_stop_log_12(CMD_12_STOP_LOG, NULL, 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd12_test, ble_cmd12_test, test stop log command);

/*==============================================================================
 * Command 0x13: Delete Stimulator Test
 * Usage: ble_cmd13_test <sn> <efuse>
 * Example: ble_cmd13_test 123456789012 12345678
 *============================================================================*/
static int ble_cmd13_test(int argc, char *argv[])
{
    if (argc < 2) {
        logError("Usage: ble_cmd13_test <sn> <efuse>");
        logError("Example: ble_cmd13_test 123456789012 12345678");
        return -1;
    }

    cmd_13_delete_stimulator_t param;
    memset(&param, 0, sizeof(param));

    char *sn_str = (char*)argv[0];
    int sn_len = strlen(sn_str);
    if (sn_len > 12) {
        logError("SN too long: got %d, max 12", sn_len);
        return -2;
    }
    memcpy(param.stim_sn, sn_str, sn_len);

    param.stim_efuse = (uint32_t)strtoul((char*)argv[1], NULL, 16);

    logInfo("=== Testing CMD 0x13: Delete Stimulator ===");
    logInfo("  Stim SN: %.12s", param.stim_sn);
    logInfo("  Stim Efuse: 0x%08X", param.stim_efuse);

    return handle_delete_stimulator_13(CMD_13_DELETE_STIMULATOR, &param, sizeof(param));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd13_test, ble_cmd13_test, test delete stimulator command);

/*==============================================================================
 * Command 0x14: Clear Program Test
 * Usage: ble_cmd14_test <sn> <efuse> <progID> <area_id>
 * Example: ble_cmd14_test 123456789012 12345678 12 00
 *============================================================================*/
static int ble_cmd14_test(int argc, char *argv[])
{
    if (argc < 4) {
        logError("Usage: ble_cmd14_test <sn> <efuse> <progID> <area_id>");
        logError("Example: ble_cmd14_test 123456789012 12345678 12 00");
        return -1;
    }

    cmd_14_clear_program_t param;
    memset(&param, 0, sizeof(param));

    char *sn_str = (char*)argv[0];
    int sn_len = strlen(sn_str);
    if (sn_len > 12) {
        logError("SN too long: got %d, max 12", sn_len);
        return -2;
    }
    memcpy(param.stim_sn, sn_str, sn_len);

    param.stim_efuse = (uint32_t)strtoul((char*)argv[1], NULL, 16);
    param.progID = (uint8_t)strtol((char*)argv[2], NULL, 16);
    param.area_id = (uint8_t)strtol((char*)argv[3], NULL, 16);

    logInfo("=== Testing CMD 0x14: Clear Program ===");
    logInfo("  Stim SN: %.12s", param.stim_sn);
    logInfo("  ProgID: 0x%02X", param.progID);
    logInfo("  Area ID: 0x%02X", param.area_id);

    return handle_clear_program_14(CMD_14_CLEAR_PROGRAM, &param, sizeof(param));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd14_test, ble_cmd14_test, test clear program command);

/*==============================================================================
 * Command 0x15: Get Program Count Test
 * Usage: ble_cmd15_test <sn> <efuse>
 * Example: ble_cmd15_test 123456789012 12345678
 *============================================================================*/
static int ble_cmd15_test(int argc, char *argv[])
{
    if (argc < 2) {
        logError("Usage: ble_cmd15_test <sn> <efuse>");
        logError("Example: ble_cmd15_test 123456789012 12345678");
        return -1;
    }

    cmd_15_get_program_count_t param;
    memset(&param, 0, sizeof(param));

    char *sn_str = (char*)argv[0];
    int sn_len = strlen(sn_str);
    if (sn_len > 12) {
        logError("SN too long: got %d, max 12", sn_len);
        return -2;
    }
    memcpy(param.stim_sn, sn_str, sn_len);

    param.stim_efuse = (uint32_t)strtoul((char*)argv[1], NULL, 16);

    logInfo("=== Testing CMD 0x15: Get Program Count ===");
    logInfo("  Stim SN: %.12s", param.stim_sn);

    return handle_get_program_count_15(CMD_15_GET_PROGRAM_COUNT, &param, sizeof(param));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd15_test, ble_cmd15_test, test get program count command);

/*==============================================================================
 * Command 0x16: Antenna Threshold Test
 * Usage: ble_cmd16_test <subCmd> [txPower] [voltage]
 * Example: ble_cmd16_test 00 15 05    (set)
 *          ble_cmd16_test 01          (get)
 *          ble_cmd16_test 02          (restore default)
 *============================================================================*/
static int ble_cmd16_test(int argc, char *argv[])
{
    if (argc < 1) {
        logError("Usage: ble_cmd16_test <subCmd> [txPower] [voltage]");
        logError("  subCmd: 00=set, 01=get, 02=restore default");
        logError("Example: ble_cmd16_test 00 15 05");
        logError("         ble_cmd16_test 01");
        logError("         ble_cmd16_test 02");
        return -1;
    }

    uint8_t subCmd = (uint8_t)strtol((char*)argv[0], NULL, 16);
    uint8_t uart_data[4] = {0};

    uart_data[0] = CMD_16_ANTENNA_THRESHOLD;
    uart_data[1] = subCmd;

    if (subCmd == 0x00 && argc >= 3) {
        uart_data[2] = (uint8_t)strtol((char*)argv[1], NULL, 16);
        uart_data[3] = (uint8_t)strtol((char*)argv[2], NULL, 16);
        logInfo("=== Testing CMD 0x16: Set Antenna Threshold ===");
        logInfo("  TxPower: %d", uart_data[2]);
        logInfo("  Voltage: %d V", uart_data[3]);
        return handle_antenna_threshold_16(CMD_16_ANTENNA_THRESHOLD, uart_data, 4);
    } else if (subCmd == 0x01) {
        logInfo("=== Testing CMD 0x16: Get Antenna Threshold ===");
        return handle_antenna_threshold_16(CMD_16_ANTENNA_THRESHOLD, uart_data, 2);
    } else if (subCmd == 0x02) {
        logInfo("=== Testing CMD 0x16: Restore Antenna Threshold Default ===");
        return handle_antenna_threshold_16(CMD_16_ANTENNA_THRESHOLD, uart_data, 2);
    }

    logError("Invalid subCmd");
    return -1;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd16_test, ble_cmd16_test, test antenna threshold command);

/*==============================================================================
 * Command 0x17: Get Stimulator Version Test
 * Usage: ble_cmd17_test
 * Example: ble_cmd17_test
 *============================================================================*/
static int ble_cmd17_test(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    logInfo("=== Testing CMD 0x17: Get Stimulator Version ===");
    return handle_get_stim_version_17(CMD_17_GET_STIM_VERSION, NULL, 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd17_test, ble_cmd17_test, test get stimulator version command);

/*==============================================================================
 * Command 0x18: Get Impedance Test
 * Usage: ble_cmd18_test <stimID> <serial_num> <pole>
 * Example: ble_cmd18_test 01 12345678 01
 *============================================================================*/
static int ble_cmd18_test(int argc, char *argv[])
{
    if (argc < 3) {
        logError("Usage: ble_cmd18_test <stimID> <serial_num> <pole>");
        logError("Example: ble_cmd18_test 01 12345678 01");
        return -1;
    }

    cmd_18_get_impedance_t param;
    memset(&param, 0, sizeof(param));

    param.stimID = (uint8_t)strtol((char*)argv[0], NULL, 16);

    char *serial_str = (char*)argv[1];
    uint64_t serial_val = (uint64_t)strtoull(serial_str, NULL, 16);
    memcpy(param.stimSerialNum, &serial_val, 8);

    param.pole = (uint8_t)strtol((char*)argv[2], NULL, 16);

    logInfo("=== Testing CMD 0x18: Get Impedance ===");
    logInfo("  Stim ID: %d", param.stimID);
    logInfo("  Pole: %d", param.pole);

    return handle_get_impedance_18(CMD_18_GET_IMPEDANCE, &param, sizeof(param));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd18_test, ble_cmd18_test, test get impedance command);

/*==============================================================================
 * Command 0x19: Get Connected Stimulator Test
 * Usage: ble_cmd19_test
 * Example: ble_cmd19_test
 *============================================================================*/
static int ble_cmd19_test(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    logInfo("=== Testing CMD 0x19: Get Connected Stimulator ===");
    return handle_get_connected_stim_19(CMD_19_GET_CONNECTED_STIM, NULL, 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd19_test, ble_cmd19_test, test get connected stimulator command);

/*==============================================================================
 * Command 0x1B: Search Impedance Test
 * Usage: ble_cmd1b_test <stimID> <sn> <pole> <amp>
 * Example: ble_cmd1b_test 01 SA0230530001 01 1000
 *============================================================================*/
static int ble_cmd1b_test(int argc, char *argv[])
{
    if (argc < 4) {
        logError("Usage: ble_cmd1b_test <stimID> <sn> <pole> <amp>");
        logError("Example: ble_cmd1b_test 01 SA0230530001 01 1000");
        return -1;
    }

    cmd_1b_search_impedance_t param;
    memset(&param, 0, sizeof(param));

    param.stimID = (uint8_t)strtol((char*)argv[0], NULL, 16);

    char *sn_str = (char*)argv[1];
    int sn_len = strlen(sn_str);
    if (sn_len > 12) {
        logError("SN too long: got %d, max 12", sn_len);
        return -2;
    }
    memcpy(param.stimSN, sn_str, sn_len);

    param.pole = (uint8_t)strtol((char*)argv[2], NULL, 16);
    param.amp = (uint16_t)strtol((char*)argv[3], NULL, 16);

    logInfo("=== Testing CMD 0x1B: Search Impedance ===");
    logInfo("  Stim ID: %d", param.stimID);
    logInfo("  Pole: %d", param.pole);
    logInfo("  Amp: %d uA", param.amp);

    return handle_search_impedance_1b(CMD_1B_SEARCH_IMPEDANCE, &param, sizeof(param));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd1b_test, ble_cmd1b_test, test search impedance command);

/*==============================================================================
 * Command 0x1C: Antenna Detect Test
 * Usage: ble_cmd1c_test <state>
 * Example: ble_cmd1c_test 01    (start)
 *          ble_cmd1c_test 00    (stop)
 *============================================================================*/
static int ble_cmd1c_test(int argc, char *argv[])
{
    if (argc < 1) {
        logError("Usage: ble_cmd1c_test <state>");
        logError("  state: 0=stop, 1=start detection");
        logError("Example: ble_cmd1c_test 01");
        return -1;
    }

    cmd_1c_antenna_detect_t param;
    param.state = (uint8_t)strtol((char*)argv[0], NULL, 16);

    logInfo("=== Testing CMD 0x1C: Antenna Detect ===");
    logInfo("  State: %d %s", param.state, param.state ? "Start" : "Stop");

    return handle_antenna_detect_1c(CMD_1C_ANTENNA_DETECT, &param, sizeof(param));
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd1c_test, ble_cmd1c_test, test antenna detect command);

/*==============================================================================
 * Command 0x1E: Temperature Limit Test
 * Usage: ble_cmd1e_test <subCmd> [temperature]
 * Example: ble_cmd1e_test 00 45    (set)
 *          ble_cmd1e_test 01       (get)
 *          ble_cmd1e_test 02       (restore default)
 *============================================================================*/
static int ble_cmd1e_test(int argc, char *argv[])
{
    if (argc < 1) {
        logError("Usage: ble_cmd1e_test <subCmd> [temperature]");
        logError("  subCmd: 00=set, 01=get, 02=restore default");
        logError("Example: ble_cmd1e_test 00 45");
        logError("         ble_cmd1e_test 01");
        return -1;
    }

    uint8_t subCmd = (uint8_t)strtol((char*)argv[0], NULL, 16);
    uint8_t uart_data[3] = {0};

    uart_data[0] = CMD_1E_TEMP_LIMIT;
    uart_data[1] = subCmd;

    if (subCmd == 0x00 && argc >= 2) {
        uart_data[2] = (uint8_t)strtol((char*)argv[1], NULL, 16);
        logInfo("=== Testing CMD 0x1E: Set Temperature Limit ===");
        logInfo("  Temperature: %d C", uart_data[2]);
        return handle_temperature_limit_1e(CMD_1E_TEMP_LIMIT, uart_data, 3);
    } else if (subCmd == 0x01) {
        logInfo("=== Testing CMD 0x1E: Get Temperature Limit ===");
        return handle_temperature_limit_1e(CMD_1E_TEMP_LIMIT, uart_data, 2);
    } else if (subCmd == 0x02) {
        logInfo("=== Testing CMD 0x1E: Restore Temperature Limit Default ===");
        return handle_temperature_limit_1e(CMD_1E_TEMP_LIMIT, uart_data, 2);
    }

    logError("Invalid subCmd");
    return -1;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd1e_test, ble_cmd1e_test, test temperature limit command);

/*==============================================================================
 * Command 0x1F: Clear All Config Test
 * Usage: ble_cmd1f_test
 * Example: ble_cmd1f_test
 *============================================================================*/
static int ble_cmd1f_test(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    logInfo("=== Testing CMD 0x1F: Clear All Config ===");
    logInfo("  WARNING: This will clear all configurations!");
    return handle_clear_all_config_1f(CMD_1F_CLEAR_ALL_CONFIG, NULL, 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd1f_test, ble_cmd1f_test, test clear all config command);

/*==============================================================================
 * Command 0x20: Factory Reset Test
 * Usage: ble_cmd20_test
 * Example: ble_cmd20_test
 *============================================================================*/
static int ble_cmd20_test(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    logInfo("=== Testing CMD 0x20: Factory Reset ===");
    logInfo("  WARNING: Factory reset will erase all data!");
    return handle_factory_reset_20(CMD_20_FACTORY_RESET, NULL, 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd20_test, ble_cmd20_test, test factory reset command);

/*==============================================================================
 * Command 0x28: RF Power Test
 * Usage: ble_cmd28_test <subCmd> [txPower] [pwmFreq]
 * Example: ble_cmd28_test 00 15 1000    (set temporary)
 *          ble_cmd28_test 01 15 1000    (set permanent)
 *          ble_cmd28_test 02             (restore default)
 *          ble_cmd28_test 03             (get temporary)
 *          ble_cmd28_test 04             (get permanent)
 *============================================================================*/
static int ble_cmd28_test(int argc, char *argv[])
{
    if (argc < 1) {
        logError("Usage: ble_cmd28_test <subCmd> [txPower] [pwmFreq]");
        logError("  subCmd: 00=temporary set, 01=permanent set, 02=restore default, 03=get temporary, 04=get permanent");
        logError("Example: ble_cmd28_test 00 15 1000");
        return -1;
    }

    uint8_t subCmd = (uint8_t)strtol((char*)argv[0], NULL, 16);
    uint8_t uart_data[45] = {0};

    uart_data[0] = CMD_28_RF_POWER;
    uart_data[1] = subCmd;

    if ((subCmd == 0x00 || subCmd == 0x01) && argc >= 3) {
        uart_data[2] = (uint8_t)strtol((char*)argv[1], NULL, 16);
        uint16_t pwmFreq = (uint16_t)strtol((char*)argv[2], NULL, 16);
        memcpy(&uart_data[3], &pwmFreq, 2);
        logInfo("=== Testing CMD 0x28: Set RF Power ===");
        logInfo("  SubCmd: %d", subCmd);
        logInfo("  TxPower: %d", uart_data[2]);
        logInfo("  PWM Freq: %d Hz", pwmFreq);
        return handle_rf_power_28(CMD_28_RF_POWER, uart_data, 45);
    } else if (subCmd == 0x02) {
        logInfo("=== Testing CMD 0x28: Restore RF Power Default ===");
        return handle_rf_power_28(CMD_28_RF_POWER, uart_data, 2);
    } else if (subCmd == 0x03 || subCmd == 0x04) {
        logInfo("=== Testing CMD 0x28: Get RF Power ===");
        return handle_rf_power_28(CMD_28_RF_POWER, uart_data, 2);
    }

    logError("Invalid subCmd");
    return -1;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd28_test, ble_cmd28_test, test rf power command);

/*==============================================================================
 * Command 0x29: Get Vrecp Test
 * Usage: ble_cmd29_test
 * Example: ble_cmd29_test
 *============================================================================*/
static int ble_cmd29_test(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    logInfo("=== Testing CMD 0x29: Get Vrecp Voltage ===");
    return handle_get_vrecp_29(CMD_29_GET_VRECP, NULL, 0);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN),
                 ble_cmd29_test, ble_cmd29_test, test get vrecp command);

#endif
