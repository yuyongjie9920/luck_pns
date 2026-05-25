//#include "main_user.h"
//
//
//static int shell_get_flash_log_size(void)
//{
//    uint32_t log_size = (uint32_t)ef_log_get_used_size();
//    logInfo("log_size = %d", log_size);
//
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), shell_get_flash_log_size, shell_get_flash_log_size, [test_flash]);
//
///**
// * @brief 擦除整个4M Flash
// * @return 0-成功, -1-失败
// */
//static int shell_erase_whole_flash(void)
//{
//    logInfo("=== Starting Whole Flash Erase (4M) ===");
//
//    EfErrCode result = EF_NO_ERR;
//    uint32_t total_size = 4 * 1024 * 1024; // 4MB
//    uint32_t erase_size = EF_ERASE_MIN_SIZE; // 使用EasyFlash的擦除最小单位
//    uint32_t address = 0;
//
//    // 确认操作
//    logWarning("This will erase ALL data in 4M Flash!");
//    logWarning("Are you sure? This operation cannot be undone!");
//
//    // 在实际产品中，可以添加额外的确认步骤
//    // 这里直接执行擦除
//
//    // 分块擦除整个Flash
//    uint32_t erased_bytes = 0;
//    uint32_t blocks = total_size / erase_size;
//
//    logInfo("Erase block size: %lu, Total blocks: %lu", erase_size, blocks);
//
//    for (uint32_t i = 0; i < blocks; i++) {
//        result = ef_port_erase(address + (i * erase_size), erase_size);
//
//        if (result != EF_NO_ERR) {
//            logError("Erase failed at address 0x%08lX, error: %d",
//                     address + (i * erase_size), result);
//            return -1;
//        }
//
//        erased_bytes += erase_size;
//
//        // 每擦除64KB显示一次进度
//        if ((i * erase_size) % (64 * 1024) == 0) {
//            uint32_t progress = (erased_bytes * 100) / total_size;
//            logInfo("Erase progress: %lu/%lu bytes (%lu%%)",
//                    erased_bytes, total_size, progress);
//        }
//    }
//
//    logInfo("=== Whole Flash Erase Completed ===");
//    logInfo("Total erased: %lu bytes", erased_bytes);
//
//    // 验证擦除结果（可选）
//    logInfo("Verifying erase result...");
//    bool verify_success = true;
//    uint32_t verify_buffer[16]; // 小缓冲区用于验证
//
//    // 随机抽查几个位置验证是否为0xFF
//    for (int i = 0; i < 10; i++) {
//        uint32_t test_addr = address + (i * (total_size / 10));
//        test_addr = RT_ALIGN_DOWN(test_addr, erase_size);
//
//        result = ef_port_read(test_addr, verify_buffer, sizeof(verify_buffer));
//        if (result == EF_NO_ERR) {
//            for (int j = 0; j < 16; j++) {
//                if (verify_buffer[j] != 0xFFFFFFFF) {
//                    logWarning("Verify failed at 0x%08lX: 0x%08lX",
//                              test_addr + j*4, verify_buffer[j]);
//                    verify_success = false;
//                }
//            }
//        }
//    }
//
//    if (verify_success) {
//        logInfo("Erase verification: PASSED");
//    } else {
//        logWarning("Erase verification: Some areas not properly erased");
//    }
//
//    return 0;
//}
//SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), shell_erase_whole_flash, shell_erase_whole_flash, Erase whole 4M Flash);
