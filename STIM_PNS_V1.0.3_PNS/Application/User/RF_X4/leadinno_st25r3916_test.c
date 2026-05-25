#include "main_user.h"
#include "string.h"
#include "leadinno_st25r3916.h"


//获取线圈相位偏差值
void test_get_phase_deviation(void)
{
    uint8_t get_data;
    st25r3916_w_reg(ST25R3916_CMD_MEASURE_PHASE, 0, REG_CMD);
    delay_ms(5);
    st25r3916_r_reg(ST25R3916_REG_A_AD_RESULT, &get_data, REG_A);
    logInfo("get_data = %d", get_data);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 test_rssi, test_get_phase_deviation, "test_get_phase_deviation");
