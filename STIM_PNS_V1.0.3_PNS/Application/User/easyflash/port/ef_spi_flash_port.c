/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015-2019, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-01-16
 */

#include <easyflash.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ti_drivers_config.h>
#include <ti/sysbios/BIOS.h>
#include <ti/drivers/NVS.h>
#include "ti/sysbios/knl/Semaphore.h"
#include "app_log.h"

//#define USE_FAL

#ifdef USE_FAL
#include <fal.h>

/* EasyFlash partition name on FAL partition table */
#define FAL_EF_PART_NAME               "easyflash"
#endif

static char log_buf[256];

/* default environment variables set for user */
static const ef_env default_env_set[] = {
	{"stiSVer", "0"},		/* 閸掔儤绺洪崳銊ㄨ拫娴犲墎澧楅張锟� */
	{"stiHVer", "0"},		/* 閸掔儤绺洪崳銊р�栨禒鍓佸閺堬拷 */
};

static bool isOpen = false;

#ifndef USE_FAL
static NVS_Handle nvsHandle;
static NVS_Attrs regionAttrs;
static NVS_Params nvsParams;
#endif

static Semaphore_Handle easyflashSem;

/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size) {
    EfErrCode result = EF_NO_ERR;
//    logInfo("ef_port_erase");
    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);

    if (!isOpen)
    {
    	Semaphore_Params sem_params;
		Semaphore_Params_init(&sem_params);
		sem_params.mode = Semaphore_Mode_BINARY;
		easyflashSem = Semaphore_create(1, &sem_params, NULL);
		
#ifdef USE_FAL
		if(fal_init() <= 0)
		{
			result = EF_READ_ERR;
		}
		else
		{
			if(nor_flash0.ops.init() != 0)
			{
				result = EF_READ_ERR;
			}
		}
#else
		NVS_init();
		NVS_Params_init(&nvsParams);

		nvsHandle = NVS_open(CONFIG_NVSEXTERNAL, &nvsParams);
		if (nvsHandle != NULL)
		{
			isOpen = true;
			// Also read the region's attribute
			NVS_getAttrs(nvsHandle, &regionAttrs);
		}
		else
		{
			result = EF_READ_ERR;
		}
#endif
    }

    return result;
}

/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;
//    logInfo("ef_port_read");
    /* You can add your code under here. */
#ifdef USE_FAL
	if(nor_flash0.ops.read(addr, (uint8_t *)buf, size) < 0)
	{
		result = EF_READ_ERR;
	}
#else
	if(isOpen)
	{
		if(NVS_read(nvsHandle, addr, buf, size) != NVS_STATUS_SUCCESS)
		{
			result = EF_READ_ERR;
		}
	}
	else
	{
		result = EF_READ_ERR;
	}
#endif

    return result;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size) {
    EfErrCode result = EF_NO_ERR;
//    logInfo("ef_port_erase");
    /* make sure the start address is a multiple of EF_ERASE_MIN_SIZE */
    EF_ASSERT(addr % EF_ERASE_MIN_SIZE == 0);

    /* You can add your code under here. */
#ifdef USE_FAL
	if(nor_flash0.ops.erase(addr, size) < 0)
	{
		result = EF_ERASE_ERR;
	}
#else
	if(isOpen)
	{
	    logInfo("NVS_erase addr = 0x%d", addr);
		if(NVS_erase(nvsHandle, addr, size) != NVS_STATUS_SUCCESS)
		{
			result = EF_ERASE_ERR;
		}
	}
	else
	{
		result = EF_ERASE_ERR;
	}
#endif

    return result;
}
/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;
//    logInfo("ef_port_write");
    /* You can add your code under here. */
//#ifdef USE_FAL
//	if(nor_flash0.ops.write(addr, (uint8_t *)buf, size) < 0)
//	{
//		result = EF_WRITE_ERR;
//	}
//#else
    if(isOpen)
    {
        if(NVS_write(nvsHandle, addr, (uint8_t *)buf, size, NVS_WRITE_PRE_VERIFY | NVS_WRITE_POST_VERIFY)
                 != NVS_STATUS_SUCCESS)
       {
            logInfo("EF_WRITE_ERR addr = 0x%x", addr);
			result = EF_WRITE_ERR;
       }
    }
	else
	{
		result = EF_WRITE_ERR;
	}
//#endif
	
    return result;
}

/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void) {
    
    /* You can add your code under here. */
    Semaphore_pend(easyflashSem, BIOS_WAIT_FOREVER);
}

/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void) {
    
    /* You can add your code under here. */
    Semaphore_post(easyflashSem);
}


/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...) {

#ifdef PRINT_DEBUG

    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
//    rt_kprintf("[Flash] (%s:%ld) ", file, line);
    vsprintf(log_buf, format, args);	
	rt_kprintf("%s", log_buf);
    va_end(args);

#endif

}

/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
    
    vsprintf(log_buf, format, args);	
	rt_kprintf("%s", log_buf);
    va_end(args);
}
/**
 * This function is print flash non-package info.
 *
 * @param format output format
 * @param ... args
 */
void ef_print(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
    vsprintf(log_buf, format, args);	
	rt_kprintf("%s", log_buf);
	
    va_end(args);
}



// 瀛樺偍鐢ㄦ埛鏁版嵁 - 瀛楃涓茬被鍨�
bool save_user_string(const char* key, const char* value)
{
    return ef_set_env(key, value) == EF_NO_ERR;
}

// 璇诲彇鐢ㄦ埛鏁版嵁 - 瀛楃涓茬被鍨�
char* read_user_string(const char* key)
{
    return ef_get_env(key);
}

// 瀛樺偍鐢ㄦ埛鏁版嵁 - 浜岃繘鍒舵暟鎹紙鏀寔浠绘剰鏁版嵁绫诲瀷锛�
bool save_user_blob(const char* key, const void* data, size_t len)
{
    return ef_set_env_blob(key, data, len) == EF_NO_ERR;
}

// 璇诲彇鐢ㄦ埛鏁版嵁 - 浜岃繘鍒舵暟鎹�
size_t read_user_blob(const char* key, void* buf, size_t buf_len, size_t* saved_len)
{
    return ef_get_env_blob(key, buf, buf_len, saved_len);
}

// 鍒犻櫎鐢ㄦ埛鏁版嵁
bool delete_user_data(const char* key)
{
    return ef_del_env(key) == EF_NO_ERR;
}

// 淇濆瓨鎵�鏈夋洿鏀瑰埌Flash
bool save_all_changes(void)
{
    return ef_save_env() == EF_NO_ERR;
}

/*******************************************************************************
 * 测试flash的函数  写整个flash大小  这里有个坑 擦4M会假成功 实际上擦除失败了
 ******************************************************************************/
#if 0
#define EXT_FLASH_TEST_SIZE  (4*1024*1024)
#define EXT_PAGE_SIZE        (4*1024)
#define EXT_ERASE_SIZE       (64*1024)
#define EXT_FLASH_START_ADDR  0
uint8_t test_buffer[EXT_PAGE_SIZE];
uint8_t verify_buffer[EXT_PAGE_SIZE];

int flash_self_test(void)
{
    // 测试结果：0=成功，非0=失败
    int test_result = 0;
    // 计算2MB对应的扇区总数（4KB/扇区 → 2MB=512个扇区）
    uint32_t total_sectors = EXT_FLASH_TEST_SIZE / EXT_PAGE_SIZE;
    //需要按64k内存进行擦除 4k会显示假擦除成功 一坑
    uint32_t total_erase_count = EXT_FLASH_TEST_SIZE / EXT_ERASE_SIZE;
    // 用于存储写入/读取的数据（扇区序号）
    uint32_t sector_num = 0;
    uint32_t read_num = 0;
    // 记录失败的扇区数
    uint32_t fail_count = 0;
    // 测试数据缓冲区


    logInfo("\n=== Starting Flash Self-Test (EasyFlash Interface) ===\n");
    // 1. 初始化EasyFlash

    if(!isOpen) {
        logInfo("Step 1: Initializing EasyFlash...\n");
        size_t default_env_set_size = 0;
        const ef_env *default_env_set;
        ef_port_init(&default_env_set, &default_env_set_size);
    }
    logInfo("EasyFlash initialized successfully.\n");

    // 2. 擦除测试区域（逐扇区擦除）
    logInfo("\nStep 2: Erasing test area...\n");
    for(uint32_t i = 0; i < total_erase_count; i++) {
        // 计算当前扇区地址
        uint32_t sector_addr = i * EXT_ERASE_SIZE;

        // 擦除当前扇区
        if(ef_port_erase(sector_addr, EXT_ERASE_SIZE) != EF_NO_ERR) {
            logError("Erase sector %d (addr 0x%06X) failed!\n", i, sector_addr);
            test_result = -2;
            goto exit_test;
        }

        // 显示进度
        if((i + 1) % 16 == 0 || (i + 1) == total_erase_count) {
            logInfo("  Erased %d/%d sectors...\n", i + 1, total_erase_count);
        }
    }
    logInfo("Erase completed.\n");

    // 3. 写入测试数据：每个扇区写入不同的模式
    logInfo("\nStep 3: Writing test patterns...\n");
    for(uint32_t i = 0; i < total_sectors; i++) {

        uint32_t sector_addr = i * EXT_PAGE_SIZE;
        // 生成测试数据：扇区序号模式
        memset(test_buffer, 0, EXT_PAGE_SIZE);
        // 在缓冲区开头写入扇区序号
        sector_num = i;
        memcpy(test_buffer, &sector_num, sizeof(sector_num));
        // 填充模式：0xAA, 0x55交替
        for(uint32_t j = sizeof(sector_num); j < EXT_PAGE_SIZE; j++) {
            test_buffer[j] = (j % 2 == 0) ? 0xAA : 0x55;
        }
        // 写入数据
        if(ef_port_write(sector_addr, test_buffer, EXT_PAGE_SIZE) != EF_NO_ERR) {
            logError("Write sector %d (addr 0x%06X) failed!\n", i, sector_addr);
            test_result = -3;
            goto exit_test;
        }

        // 显示进度
        if((i + 1) % 16 == 0 || (i + 1) == total_sectors) {
            logInfo("  Wrote %d/%d sectors...\n", i + 1, total_sectors);
        }
    }
    logInfo("Write completed.\n");

    // 4. 读取并验证数据
    logInfo("\nStep 4: Verifying data...\n");
    for(uint32_t i = 0; i < total_sectors; i++)
    {
        uint32_t sector_addr = i * EXT_PAGE_SIZE;

        // 清空验证缓冲区
        memset(verify_buffer, 0, EXT_PAGE_SIZE);

        // 读取数据
        if(ef_port_read(sector_addr, verify_buffer, EXT_PAGE_SIZE) != EF_NO_ERR) {
            logError("Read sector %d (addr 0x%06X) failed!\n", i, sector_addr);
            fail_count++;
            continue;
        }

        // 验证扇区序号
        uint32_t read_sector_num;
        memcpy(&read_sector_num, verify_buffer, sizeof(read_sector_num));
        if(read_sector_num != i)
        {
            logError("Sector %d: Bad sector number! Expected: %d, Read: %d\n",
                    i, i, read_sector_num);
            fail_count++;
        }

        // 验证数据模式
        bool pattern_ok = true;
        for(uint32_t j = sizeof(read_sector_num); j < EXT_PAGE_SIZE; j++) {
            uint8_t expected = (j % 2 == 0) ? 0xAA : 0x55;
            if(verify_buffer[j] != expected)
            {
                pattern_ok = false;
                break;
            }
        }

        if(!pattern_ok)  {
            logError("Sector %d: Data pattern verification failed!\n", i);
            fail_count++;
        }

        // 显示进度
        if((i + 1) % 16 == 0 || (i + 1) == total_sectors) {
            logInfo("  Verified %d/%d sectors, Failures: %d\n",
                    i + 1, total_sectors, fail_count);
        }
    }
    logInfo("Verification completed.\n");

    // 5. 测试结果汇总
    logInfo("\n=== Test Summary ===\n");
    logInfo("Total sectors tested: %d\n", total_sectors);
    logInfo("Total bytes tested: %d KB\n", total_sectors * EXT_PAGE_SIZE / 1024);
    logInfo("Failed sectors: %d\n", fail_count);

    if(fail_count == 0) {
        logInfo("\n=== Flash Self-Test PASSED! ===\n");
        test_result = 0;
    } else {
        logError("\n=== Flash Self-Test FAILED! ===\n");
        test_result = -4;
    }

exit_test:
    logInfo("\n=== Flash Self-Test Finished ===\n");
    return test_result;
}

// 注册Shell命令（保持原有定义）
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 flash_self_test, flash_self_test, [test_flash]);
#endif
