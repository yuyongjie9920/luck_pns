/**
 * @file app_verify_ble.c
 * @brief 蓝牙连接验证与数据通信协议实现
 * @details 实现完整的4步安全握手验证流程和加密数据传输
 * @version 1.0
 * @date 2026-04-28
 */

#include <app_verify_ble.h>
#include "main_user.h"
#include "simple_peripheral.h"
#include "icall_ble_api.h"
#include "app_ble_data.h"
#include "sm2.h"
#include "tiny_md5.h"
#include "app_rtc.h"
#include "crc.h"
#include <ti/sysbios/gates/GateMutexPri.h>
#include <ti/sysbios/knl/Task.h>

/*==============================================================================
 * 1. 全局变量定义
 *============================================================================*/

extern IArg key_lock;                             /**< 互斥锁句柄 */
extern GateMutexPri_Handle myGate;                /**< 优先级互斥锁 */

struct app_ble_client ble_client = {0};           /**< BLE客户端全局实例 */
static Mailbox_Struct AppMailbox_Struct;          /**< APP邮箱结构体 */
static Clock_Struct verify_timer;                 /**< 验证超时定时器 */

/*==============================================================================
 * 2. 工具函数实现
 *============================================================================*/

/**
 * @brief 命令处理回调函数(弱函数实现)
 * @details 用户可重写此函数实现具体的命令处理逻辑
 */
__attribute__((weak)) int process_command(uint8_t cmd, uint16_t indx,
                                            uint8_t *data, uint16_t len)
{
    return -1;  // 默认返回-1表示未实现
}

/**
 * @brief 单个字符转十六进制数值
 * @param input 输入字符('0'-'9', 'A'-'F', 'a'-'f')
 * @return 对应的十六进制数值(0-15)
 */
int C2Hex(char input)
{
    if (input >= '0' && input <= '9')
        return input - '0';
    if (input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if (input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    return 0;
}

/**
 * @brief 十六进制字符串转二进制数组
 * @param src 十六进制字符串(如"A8B3C5")
 * @param target 输出二进制数组
 * @note 字符串长度必须为偶数
 */
void Hex2Bin(const char* src, char* target)
{
    while (*src && src[1]) {
        *(target++) = C2Hex(*src) * 16 + C2Hex(src[1]);
        src += 2;
    }
}

/*==============================================================================
 * 4. 随机数相关函数实现
 *============================================================================*/

/**
 * @brief 生成随机数序列(使用硬件TRNG)
 * @param buf 输出缓冲区
 * @param buf_size 需要生成的随机数字节数
 * @return SUCCESS:成功, FAILURE:失败
 */
uint8_t generate_random_numbers(uint8_t *buf, uint16_t buf_size)
{
    BLE_ASSERT(buf != NULL);
    BLE_ASSERT(buf_size != 0);

    int_fast16_t result;
    TRNG_Handle handle;

    TRNG_init();
    handle = TRNG_open(CONFIG_TRNG_1, NULL);
    if (!handle) {
        rt_kprintf("TRNG_open err!!!\r\n");
        return FAILURE;
    }

    result = TRNG_getRandomBytes(handle, buf, buf_size);
    if (result != TRNG_STATUS_SUCCESS) {
        rt_kprintf("TRNG_getRandomBytes err[%d]!!!\r\n", result);
        return FAILURE;
    }

    TRNG_close(handle);
    return SUCCESS;
}

/**
 * @brief 从随机数池中提取8字节随机数
 * @param buf 随机数池缓冲区
 * @param buf_size 缓冲区大小
 * @param random_data 输出8字节随机数
 * @return SUCCESS:成功, FAILURE:失败
 * @note 使用前8字节作为索引，从池中选取8个字节组成新随机数
 */
uint8_t get_random_8bytes_from_datas(uint8_t *buf, uint8_t buf_size, uint8_t *random_data)
{
    BLE_ASSERT(buf != NULL);
    BLE_ASSERT(buf_size != 0);
    BLE_ASSERT(random_data != NULL);

    uint8_t i, temp;
    for (i = 0; i < 8; i++) {
        temp = buf[i] % buf_size;      // 用前8字节作为索引
        *(random_data + i) = buf[temp];
    }
    return SUCCESS;
}


/*==============================================================================
 * 3. 加密相关函数实现
 *============================================================================*/

/**
 * @brief 生成第一阶段密钥key1
 * @param cMAC MAC地址字符串或字节数组
 * @param cMAC_size MAC地址长度(6字节或17字节)
 * @param key1 输出16字节密钥
 * @details key1 = HMAC-MD5(KEY1_SALT, cMAC)
 */
void generate_key1(uint8_t *cMAC, uint8_t cMAC_size, uint8_t *key1)
{
    BLE_ASSERT((cMAC_size == 6) || (cMAC_size == 17));
    BLE_ASSERT(cMAC != NULL);
    BLE_ASSERT(key1 != NULL);

    uint8_t key[] = KEY1_SALT;
    tiny_md5_hmac(key, sizeof(key) - 1, cMAC, cMAC_size, key1);
}

/**
 * @brief AES-128-CBC加解密实现
 * @param input 输入数据(长度必须是16的倍数)
 * @param size 输入数据大小
 * @param key 16字节密钥
 * @param mode AES_ENCRYPT或AES_DECRYPT
 * @return 加解密后的数据指针(需调用free释放)
 * @note IV固定为0x01重复16次
 */
uint8_t *aes128_crypt_cbc(uint8_t *input, uint8_t size, uint8_t *key, aes_e mode)
{
    BLE_ASSERT(input != NULL);
    BLE_ASSERT(size % 16 == 0);
    BLE_ASSERT(key != NULL);
    BLE_ASSERT((mode == AES_ENCRYPT) || (mode == AES_DECRYPT));

    uint8_t *output = NULL;

    uint8_t iv[16];

    // 设置固定IV: 全0x01  不能动
    for (uint8_t i = 0; i < 16; i++) {
        iv[i] = 0x01;
    }

    output = calloc(1, size + 16);
    if (output == NULL) {
        return NULL;
    }

    AESCBC_Handle handle;
    CryptoKey cryptoKey;
    int_fast16_t encryptionResult;

    AESCBC_init();
    handle = AESCBC_open(CONFIG_AESCBC_0, NULL);
    if (handle == NULL) {
        logInfo("AESCBC_open err");
    }

    CryptoKeyPlaintext_initKey(&cryptoKey, key, 16);
    AESCBC_Operation operation;
    AESCBC_Operation_init(&operation);
    operation.key = &cryptoKey;
    operation.input = input;
    operation.output = output;
    operation.inputLength = size;
    operation.iv = iv;
    operation.ivInternallyGenerated = false;

    if (mode == AES_ENCRYPT) {
        encryptionResult = AESCBC_oneStepEncrypt(handle, &operation);
        if (encryptionResult != AESCBC_STATUS_SUCCESS) {
            logInfo("AESCBC_oneStepEncrypt err: %d", encryptionResult);
        }
    } else {
        encryptionResult = AESCBC_oneStepDecrypt(handle, &operation);
        if (encryptionResult != AESCBC_STATUS_SUCCESS) {
            logInfo("AESCBC_oneStepDecrypt err: %d", encryptionResult);
        }
    }
    AESCBC_close(handle);

    return output;
}


/*==============================================================================
 * 5. 设备信息函数实现
 *============================================================================*/

/**
 * @brief 保存BLE MAC地址
 * @param addr MAC地址字节数组(6字节)
 * @note 同时保存字节数组和字符串两种格式
 */
void user_save_ble_mac_addr(uint8_t *addr)
{
    memcpy(ble_client.macaddr_byte, addr, B_ADDR_LEN);
    // 转换为字符串格式，如"AA:BB:CC:DD:EE:FF"
    sprintf(ble_client.macaddr_str, "%02X:%02X:%02X:%02X:%02X:%02X",
            ble_client.macaddr_byte[5], ble_client.macaddr_byte[4],
            ble_client.macaddr_byte[3], ble_client.macaddr_byte[2],
            ble_client.macaddr_byte[1], ble_client.macaddr_byte[0]);
}


/*==============================================================================
 * 7. 通信函数实现
 *============================================================================*/

/**
 * @brief 向手机APP发送BLE数据(添加帧头帧尾)
 * @param data 待发送数据(不含帧头帧尾)
 * @param len 数据长度
 * @note 使用互斥锁保护发送过程
 */
void user_send_ble_data_to_iphone(uint8_t *data, uint16_t len)
{
    static uint8_t send_buf[BLE_MTU];

    if (len == 0 || data == NULL || len > BLE_MTU) {
        return;
    }

    // 进入互斥锁保护
    key_lock = GateMutexPri_enter(myGate);

    send_buf[0] = PROTOCOL_FRAME_HEADER;           // 添加帧头
    send_buf[len + 1] = PROTOCOL_FRAME_TAIL;       // 添加帧尾
    memcpy(&send_buf[1], data, len);               // 复制数据
    BLE_send_notify(send_buf, len + 2);            // 发送通知

    GateMutexPri_leave(myGate, key_lock);          // 退出互斥锁
}

/**
 * @brief 向APP发送错误响应帧
 * @param cmd 命令码
 * @param err 错误类型
 */
void send_err_frame_to_app(uint8_t cmd, uint8_t err)
{
    ble_error_packet_t out;
    if (err == 0XFF) {
        return;
    }

    uint8_t buf[16];
    uint8_t *result = buf;
    uint8_t data_len = 0;

    // 帧序号(低字节和高字节)
    *result++ = 0;
    *result++ = 0;

    // 帧长度
    if (err == 0 || err == 1) {
        *result++ = 1;         // 正常响应长度为1
    } else {
        *result++ = 2;         // 错误响应长度为2(命令码+错误码)
    }
    data_len += 3;

    // 数据部分
    if (err == 0 || err == 1) {
        *result++ = cmd;       // 正常响应只返回命令码
        data_len++;
    } else {
        *result++ = cmd | 0x80; // 错误响应命令码最高位置1
        *result++ = err;        // 错误码
        data_len += 2;
    }

    // CRC16校验
    uint16_t crc16 = crc16Bytewise(buf, sizeof(ble_error_packet_t)-sizeof(out.header)-sizeof(out.tail)-sizeof(out.crc));
    *result++ = (uint8_t)crc16;
    *result++ = (crc16 >> 8) & 0xff;
    data_len += 2;

    logInfo("send data to app buf");
    put_buf(buf, sizeof(buf));

    // 使用key2加密后发送
    uint8_t *ciphertext = aes128_crypt_cbc(buf, sizeof(buf), ble_client.key2, AES_ENCRYPT);
    if (ciphertext == NULL) {
        logInfo("cmd[%d]:malloc fail", cmd);
        BLE_send_notify(&err, 1);  // 加密失败直接发送错误码
        return;
    }

    user_send_ble_data_to_iphone(ciphertext, sizeof(buf));
    free(ciphertext);
}

/**
 * @brief 向APP发送正常响应帧
 * @param cmd 命令码
 * @param frame_index 帧序号
 * @param data 可选数据指针(NULL表示无)
 * @param size 可选数据大小
 * @return 执行结果
 */
uint8_t send_normal_frame_to_app(uint8_t cmd, uint16_t frame_index,
                                               uint8_t *data, size_t size)
{
    uint16_t data_len = 0;
    ble_packet_t out;

    // 计算明文数据长度: 帧序号(2) + 帧长度(1) + 命令码(1) + 可选数据 + CRC(2)

    uint16_t result_size = sizeof(out.pack_indx) + sizeof(out.length) + sizeof(out.cmd) + size + sizeof(out.crc);

    // 加密需要16字节对齐，不足时补齐
    result_size = (result_size + 15) & ~15;
    BLE_ASSERT(result_size <= BLE_MTU);

    uint8_t *save_result = malloc(result_size);
    uint8_t *result = save_result;
    if (result == NULL) {
        logInfo("cmd[%d]:malloc fail", cmd);
        return 0x25;
    }

    // 构建数据帧
    *result++ = (uint8_t)frame_index;          // 帧序号低字节
    *result++ = (frame_index >> 8) & 0xff;     // 帧序号高字节
    *result++ = sizeof(out.cmd) + size;        // 帧长度
    *result++ = cmd;                           // 命令码
    data_len += 4;
    // 复制可选数据
    memcpy(result, data, size);
    result += size;
    data_len += size;
    // CRC16校验
    uint16_t crc16 = crc16Bytewise(save_result, data_len);
    *result++ = (uint8_t)crc16;
    *result++ = (crc16 >> 8) & 0xff;
    data_len += 2;

    logInfo("ble_send_ok_data !!");
    put_buf(save_result, result_size);
    // 使用key2加密后发送
    uint8_t *ciphertext = aes128_crypt_cbc(save_result, result_size, ble_client.key2, AES_ENCRYPT);
    if (ciphertext == NULL) {
        free(save_result);
        logInfo("cmd[%d]:malloc fail", cmd);
        uint8_t err = 0x25;
        BLE_send_notify(&err, 1);
        return err;
    }

    user_send_ble_data_to_iphone(ciphertext, result_size);
    free(save_result);
    free(ciphertext);
    return 0;
}

/*==============================================================================
 * 8. 验证流程函数(4步握手)
 *============================================================================*/

/**
 * @brief 验证步骤1: 根据蓝牙MAC地址生成key1
 * @details 在蓝牙模块上电后调用，提前生成key1等待验证
 */
void verify_part1(void)
{
    generate_key1((uint8_t *)(ble_client.macaddr_str),
                  strlen(ble_client.macaddr_str), ble_client.key1);
}

/**
 * @brief 验证步骤2: 验证APP的SM2签名
 * @param data APP发送的原始数据
 * @param size 数据大小
 * @return 0:成功, -1:失败
 */
int verify_part2(uint8_t *data, size_t size)
{
    auth_step1_t in;
    SM2_KEY sm2_key;

    // 解析数据
    memcpy(in.app_dev_id, data + sizeof(in.fh), sizeof(in.app_dev_id));
    in.ciphertext = data + sizeof(in.fh) + sizeof(in.app_dev_id);
    in.ciphertext_len = size - sizeof(in.fh) - sizeof(in.app_dev_id) - sizeof(in.ft);

    // 将私钥十六进制转换为字节数组
    Hex2Bin(APP_PRIVATE_KEY, (char *)(sm2_key.private_key));

    // SM2解密并验证设备ID是否匹配
    if ((sm2_decrypt(&sm2_key, in.ciphertext, in.ciphertext_len,
                     in.decode_dev_id, &in.decode_dev_id_len) != 1) ||
        (in.decode_dev_id_len != sizeof(in.app_dev_id)) ||
        (memcmp(in.decode_dev_id, in.app_dev_id, sizeof(in.app_dev_id)) != 0)) {
        return -1;  // 验证失败
    }

    // 验证通过，回复APP的设备ID
    user_send_ble_data_to_iphone(in.app_dev_id, sizeof(in.app_dev_id));

    logInfo("pass APP_CONNECT_STATE_CONNECTED");
    ble_client.connect_state = APP_CONNECT_STATE_VERIFY0;
    return 0;
}

/**
 * @brief 验证步骤3: 生成随机数和key2
 * @param data APP发送的加密MAC地址数据
 * @param size 数据大小
 * @return 0:成功, -1:失败
 */
int verify_part3(uint8_t *data, size_t size)
{
    auth_step3_t *in = (auth_step3_t *)data;
    auth_step3_plain_t out;

    if (size != sizeof(auth_step3_t)) {
        return -1;
    }

    //验证MAC地址是否匹配 解码数据
    uint8_t *plain = aes128_crypt_cbc(in->encrypted_data, sizeof(in->encrypted_data), ble_client.key1, AES_DECRYPT);
    if(plain != NULL) {
        if(memcmp(plain + sizeof(in->encrypted_data) - strlen(ble_client.macaddr_str),
                  (uint8_t *)(ble_client.macaddr_str), strlen(ble_client.macaddr_str)) != 0) {
            return -1;
        }
        free(plain);

        // 生成48字节随机数
       generate_random_numbers(out.random_bytes, sizeof(out.random_bytes));
       // 从48字节中提取前8字节作为key2前半部分
       get_random_8bytes_from_datas(out.random_bytes, sizeof(out.random_bytes), ble_client.key2);

       // 使用key1加密48字节随机数
       uint8_t *ciphertext = aes128_crypt_cbc(out.random_bytes, sizeof(out.random_bytes),
                                               ble_client.key1, AES_ENCRYPT);
       if (ciphertext == NULL) {
           return -1;
       }
       memcpy(out.random_bytes, ciphertext, sizeof(out.random_bytes));
       free(ciphertext);

    } else {
        return -1;
    }
    // 发送加密的随机数给APP
    user_send_ble_data_to_iphone(out.random_bytes, sizeof(out.random_bytes));

    logInfo("pass APP_CONNECT_STATE_VERIFY0");
    ble_client.connect_state = APP_CONNECT_STATE_VERIFY1;
    return 0;
}

/**
 * @brief 验证步骤4: 解析key2后半部分并设置RTC时间
 * @param data APP发送的加密时间戳数据
 * @param size 数据大小
 * @return 0:成功, -1:失败
 */
int verify_part4(uint8_t *data, size_t size)
{
    auth_step4_t *in = (auth_step4_t *)data;
    auth_step4_plain_t *out;

    if (size != sizeof(auth_step4_t)) {
        return -1;
    }

    // 使用key1解密数据
    out = (auth_step4_plain_t *)aes128_crypt_cbc((uint8_t *)in->encrypted_data,
                                                  sizeof(in->encrypted_data),
                                                  ble_client.key1, AES_DECRYPT);
    if (out == NULL) {
        return -1;
    }

    // 从42字节随机数据中提取后8字节作为key2后半部分
    get_random_8bytes_from_datas(out->random_data, sizeof(out->random_data), ble_client.key2 + 8);

    // 设置RTC时间
    rtc_time_t time;
    memcpy(&time, out->time, sizeof(out->time));
    if (app_rtc_set_time(time) != 0) {
        logInfo("set rtc fail");
    }

    // 构建设备信息结构体
    dev_info_t dev_info = {
        .padding = {0},
        .patient_id = 0x12345678,
        .api_version = APP_API_VERSION,
        .controller_sn = "",
        .controller_sw = CONTROLLER_SOFTWARE_VERSION,
        .controller_hw = CONTROLLER_HARDWARE_VERSION,
        .stimulator_sn = STIMULATOR_SERIAL_NUMBER,
        .stimulator_sw = STIMULATOR_SOFTWARE_VERSION,
        .stimulator_hw = STIMULATOR_HARDWARE_VERSION,
        .ble_sw = BLE_SOFTWARE_VERSION,
        .stimulator_mac = {0}
    };

    // 使用key2加密设备信息
    uint8_t *ciphertext = aes128_crypt_cbc((uint8_t *)&dev_info, sizeof(dev_info),
                                            ble_client.key2, AES_ENCRYPT);
    if (ciphertext == NULL) {
        logError("AES encryption failed");
        return -1;
    }
    memcpy(&dev_info, ciphertext, sizeof(dev_info));
    free(ciphertext);
    ciphertext = NULL;

    // 验证通过，停止验证超时定时器
    Util_stopClock(ble_client.verify_timer);

    // 发送加密的设备信息给APP
    user_send_ble_data_to_iphone((uint8_t *)&dev_info, sizeof(dev_info));
    logInfo("pass APP_CONNECT_STATE_VERIFY1_OKOK");

    // 通知应用层蓝牙连接成功
    send_mailbox_to_app_user(MSG_BLUETOOTH_CONNECTED_SUCCEED, NULL, 0);
    ble_client.connect_state = APP_CONNECT_STATE_VERIFY2;

    free(out);
    return 0;
}

/**
 * @brief 解密并解析命令帧数据
 * @param data 接收到的加密数据
 * @param size 数据大小
 * @return 0:成功处理, 其他:错误
 */
int enc_cmd_data(uint8_t *data, size_t size)
{
    ble_packet_t in;

    // 加密数据长度必须是16的倍数
    if ((size - 2) % 16 != 0) {
        send_err_frame_to_app(0xFF, 0x12);
        logInfo("decrypt len(%d)", size);
        return 0;
    }

    // 使用key2解密数据
    uint8_t *ciphertext = aes128_crypt_cbc((data + sizeof(in.header)),
                                            size - (sizeof(in.header) + sizeof(in.tail)),
                                            ble_client.key2, AES_DECRYPT);
    if (ciphertext == NULL) {
        send_err_frame_to_app(0xFF, 0x25);
        logInfo("malloc fail");
        return 0;
    }

    // 解析命令帧
    memcpy(&in.pack_indx, ciphertext, size - (sizeof(in.header) + sizeof(in.tail) + sizeof(in.crc)));
    memcpy(&in.crc, &ciphertext[sizeof(in.pack_indx) + sizeof(in.length) + in.length], sizeof(in.crc));

    // 验证CRC
    if (crc16Bytewise(ciphertext, sizeof(in.pack_indx) + sizeof(in.length) + in.length) != in.crc) {
        free(ciphertext);
        return 0;
    }

#if 1
    logInfo("cmd = 0x%x indx = %d len = %d", in.cmd, in.pack_indx, in.length);
    put_buf(in.data, in.length - sizeof(in.cmd));
#endif

    // 调用用户命令处理函数
    process_command(in.cmd, in.pack_indx, in.data, in.length - sizeof(in.cmd));

    free(ciphertext);
    return 0;
}

/*==============================================================================
 * 9. 协议处理主函数
 *============================================================================*/
/**
 * @brief 协议处理函数
 * @param client BLE客户端结构体指针
 * @param data 接收到的数据
 * @param size 数据大小
 * @details 根据当前连接状态分发到对应的验证步骤处理函数
 */
void protocol_operate_func(struct app_ble_client *client, uint8_t *data, size_t size)
{
    // 验证帧头帧尾
    if (ble_client.connect_state != APP_CONNECT_STATE_STARTED &&
        ble_client.connect_state != APP_CONNECT_STATE_DISCONNECTED) {
        if ((data[0] != PROTOCOL_FRAME_HEADER) || (data[size - 1] != PROTOCOL_FRAME_TAIL)) {
            logInfo("data error");
            put_buf(data, size);
            return;
        } else {
            // 添加边界检查
            if (size > sizeof(ble_client.in_data)) {
                logError("Data size exceeds buffer: %zu", size);
                return;
            }
            memcpy(ble_client.in_data, data, size);  // 复制数据防止被篡改
        }
    }
    // 状态机处理
    switch (ble_client.connect_state) {
        case APP_CONNECT_STATE_STARTED:
            verify_part1();  // 蓝牙上电后生成key1
            break;

        case APP_CONNECT_STATE_CONNECTED:
            if (verify_part2(ble_client.in_data, size) != 0) {
                goto disconnt_ble;
            }
            break;

        case APP_CONNECT_STATE_VERIFY0:
            if (verify_part3(ble_client.in_data, size) != 0) {
                goto disconnt_ble;
            }
            break;

        case APP_CONNECT_STATE_VERIFY1:
            if (verify_part4(ble_client.in_data, size) != 0) {
                goto disconnt_ble;
            }
            break;

        case APP_CONNECT_STATE_VERIFY2:
            if (enc_cmd_data(ble_client.in_data, size) != 0) {
                goto disconnt_ble;
            }
            break;

        case APP_CONNECT_STATE_DISCONNECTED:
            send_mailbox_to_app_user(MSG_BLUETOOTH_DISCONNECTED, NULL, 0);
            break;

        default:
            break;
    }
    return;

disconnt_ble:
     logInfo("__line__ = %d", __LINE__);
     BLE_disconnEvtCB();  // 可选:断开连接
}

/*==============================================================================
 * 10. 邮箱通信函数
 *============================================================================*/
/**
 * @brief 将BLE数据通过邮箱发送到任务线程
 * @param data 数据指针
 * @param len 数据长度
 * @return 0:成功, -1:失败
 */
int user_get_ble_data_send_mailbox(uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0) {
        logInfo("data == NULL || len == 0");
        return -1;
    }

    struct mailbox_msg msg;
    msg.ptr = (uint8_t *)data;
    msg.size = len;

    if (Mailbox_post(ble_client.mailbox, &msg, BIOS_WAIT_FOREVER) != true) {
        logInfo("maibox faile");
        return -1;
    }
    return 0;
}

/**
 * @brief 发送BLE状态变化通知
 * @param ble_state 新的连接状态
 * @return 0:成功, -1:失败
 */
int user_send_mailbox_ble_state(uint8_t ble_state)
{
    struct mailbox_msg msg;
    ble_client.connect_state = ble_state;

    if (Mailbox_post(ble_client.mailbox, &msg, BIOS_WAIT_FOREVER) != true) {
        logInfo("maibox faile");
        return -1;
    }
    return 0;
}

/*==============================================================================
 * 6. 定时器回调函数
 *============================================================================*/

/**
 * @brief 身份验证超时回调函数
 * @param arg 未使用的参数
 * @details 验证超时后断开蓝牙连接
 */
static void verify_timeout(UArg arg)
{
    logInfo(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>verify_timeout, discon");
    Util_stopClock(ble_client.verify_timer);
    ble_client.connect_state = APP_CONNECT_STATE_INVALID;
    BLE_disconnEvtCB();  // 断开蓝牙连接
}

/*==============================================================================
 * 11. BLE任务主函数
 *============================================================================*/
/**
 * @brief BLE任务主函数
 * @param arg0 任务参数0(未使用)
 * @param arg1 任务参数1(未使用)
 */
void app_ble_task(UArg arg0, UArg arg1)
{
    struct mailbox_msg msg = {0};
    // 初始化邮箱
    ble_client.mailbox = &AppMailbox_Struct;

    Mailbox_Params mailboxParams;
    Mailbox_Params_init(&mailboxParams);
    Mailbox_construct(ble_client.mailbox, sizeof(struct mailbox_msg), 20, &mailboxParams, NULL);

    ble_client.verify_timer = &verify_timer;

    // 创建验证超时定时器
    Util_constructClock(ble_client.verify_timer, verify_timeout,
                        VERIFY_TIMEOUT_TIME, 0, false, (UArg)NULL);

    // 主循环
    while (1) {
        if (Mailbox_pend(ble_client.mailbox, &msg, 1000000) != true) {
            continue;
        }

        // 处理接收到的数据
        protocol_operate_func(&ble_client, msg.ptr, msg.size);
    }
}
