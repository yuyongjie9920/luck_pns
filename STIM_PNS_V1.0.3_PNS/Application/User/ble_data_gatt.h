#ifndef BLE_DATA_GATT_H
#define BLE_DATA_GATT_H

#include <icall_ble_api.h>       // 包含MAX_NUM_CONN的定义
#include <gatt.h>                // 包含charUUID等定义
#include <gattservapp.h>      // 包含 GATT权限宏定义

#include "main_user.h"
#include "simple_peripheral.h"

#define RX_TX_BUFF_SIZE   1024

struct tx_data_pack {
    uint16_t len;
    uint8_t data[RX_TX_BUFF_SIZE];
};

bStatus_t Ble_GATT_User_Init(void);
bStatus_t UartProfile_NotifySenddata(bt_ev_data *in_data);

#endif
