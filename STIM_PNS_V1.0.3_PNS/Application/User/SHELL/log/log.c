/**
 * @file log.c
 * @author Letter (nevermindzzt@gmail.com)
 * @brief log
 * @version 1.0.0
 * @date 2020-07-30
 * 
 * @copyright (c) 2020 Letter
 * 
 */
#include "log.h"
#include "stdio.h"
#include "stdarg.h"
#include "shell.h"





#if LOG_USING_COLOR == 1
#define memPrintHead CSI(31) \
    "    Offset: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" \
    CSI(39) \
    "\r\n"
#define memPrintAddr CSI(31)"0x%08x: "CSI(39)
#else
#define memPrintHead "    Offset: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n"
#define memPrintAddr "0x%08x: "
#endif

Log *logList[LOG_MAX_NUMBER] = {0};
static char logBuffer[LOG_BUFFER_SIZE];

#if LOG_USING_LOCK == 1
/**
 * @brief   濞戞挸锕弨顤瞣g閻庣數顢婇挅锟�
 * @param   log log閻庣數顢婇挅锟�
 */
static void logLock(Log *log)
{
    if (log == LOG_ALL_OBJ)
    {
        for (short i = 0; i < LOG_MAX_NUMBER; i++)
        {
            if (logList[i] && logList[i]->active)
            {
                if (logList[i]->lock)
                {
                    LOG_LOCK(logList[i]);
                }
            }
        }
    }
    else if (log->lock)
    {
        LOG_LOCK(log);
    }
}

/**
 * @brief   閻熸瑱缍侀弨顤瞣g閻庣數顢婇挅锟�
 * @param   log log閻庣數顢婇挅锟�
 */
static void logUnlock(Log *log)
{
    if (log == LOG_ALL_OBJ)
    {
        for (short i = 0; i < LOG_MAX_NUMBER; i++)
        {
            if (logList[i] && logList[i]->active)
            {
                if (logList[i]->unlock)
                {
                    LOG_UNLOCK(logList[i]);
                }
            }
        }
    }
    else if (log->unlock)
    {
        LOG_UNLOCK(log);
    }
}
#endif /* LOG_USING_LOCK == 1 */

/**
 * @brief 婵炲鍔岄崬绲g閻庣數顢婇挅锟�
 * 
 * @param log log閻庣數顢婇挅锟�
 */
void logRegister(Log *log, Shell *shell)
{
    if (shell)
    {
        log->shell = shell;
    #if SHELL_USING_COMPANION == 1
        shellCompanionAdd(shell, SHELL_COMPANION_ID_LOG, log);
    #endif
    }
    for (short i = 0; i < LOG_MAX_NUMBER; i++)
    {
        if (logList[i] == 0)
        {
            logList[i] = log;
            return;
        }
    }
}


/**
 * @brief 婵炲鍔戦弨顣抩g閻庣數顢婇挅锟�
 * 
 * @param log log閻庣數顢婇挅锟�
 */
void logUnRegister(Log *log)
{
    for (short i = 0; i < LOG_MAX_NUMBER; i++)
    {
        if (logList[i] == log)
        {
            logList[i] = 0;
            return;
        }
    }
}


/**
 * @brief 鐠佸墽鐤唋og閺冦儱绻旂痪褍鍩�
 * 
 * @param log log鐎电钖�
 * @param level 閺冦儱绻旂痪褍鍩�
 */
void logSetLevel(Log *log, LogLevel level)
{
    //logAssert(log, return);
    if (log == NULL) {

        return;
    }
    log->level = level;
}
#if SHELL_USING_COMPANION == 1
SHELL_EXPORT_CMD_AGENCY(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
logSetLevel, logSetLevel,  set log level\r\n logSetLevel [level],
(void *)shellCompanionGet(shellGetCurrent(), SHELL_COMPANION_ID_LOG), p1);
#else
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
logSetLevel, logSetLevel, set log level\r\n logSetLevel [log] [level]);
#endif /** SHELL_USING_COMPANION == 1 */


/**
 * @brief log闁告劖鐛弖ffer
 * 
 * @param log log閻庣數顢婇挅锟�
 * @param level 闁哄啨鍎辩换鏃傜棯瑜嶉崺锟�
 * @param buffer buffer
 * @param len buffer闂傦拷閸喖顔�
 */
static void logWriteBuffer(Log *log, LogLevel level, char *buffer, short len)
{
#if LOG_USING_LOCK == 1
    logLock(log);
#endif /* LOG_USING_LOCK == 1 */
    if (log == LOG_ALL_OBJ)
    {
        for (short i = 0; i < LOG_MAX_NUMBER; i++)
        {
            if (logList[i] 
                && logList[i]->active
                && logList[i]->level >= level)
            {
                logList[i]->write(logBuffer, len);
            }
        }
    }
    else if (log && log->active && log->level >= level)
    {
        log->write(logBuffer, len);
    }
#if LOG_USING_LOCK == 1
    logUnlock(log);
#endif /* LOG_USING_LOCK == 1 */
}

/**
 * @brief log闁哄秶鍘х槐锟犲礌閺嵮冩櫢闁稿繈鍎查弳鐔煎箲閿燂拷
 * 
 * @param log log閻庣數顢婇挅锟�
 * @param level log缂佺嫏鍐ㄧ劶
 * @param fmt 闁哄秶鍘х槐锟�
 * @param ... 闁告瑥鍊归弳锟�
 */
void logWrite(Log *log, LogLevel level, const char *fmt, ...)
{
    va_list vargs;
    int len;
    
#if LOG_USING_LOCK == 1
    logLock(log);
#endif /* LOG_USING_LOCK == 1 */
    va_start(vargs, fmt);
    len = vsnprintf(logBuffer, LOG_BUFFER_SIZE - 1, fmt, vargs);
    va_end(vargs);

    if (len > LOG_BUFFER_SIZE)
    {
        len = LOG_BUFFER_SIZE;
    }

    logWriteBuffer(log, level, logBuffer, len);
#if LOG_USING_LOCK == 1
    logUnlock(log);
#endif /* LOG_USING_LOCK == 1 */

}

// ANSI鎺у埗瀛楃杩囨护鍑芥暟
void filter_ansi_sequences(const char *input, char *output, size_t output_size) {
    const char *src = input;
    char *dst = output;
    int in_escape = 0;

    while (*src && (dst - output) < output_size - 1) {
        if (*src == '\033') {  // ESC瀛楃
            in_escape = 1;
            src++;
            continue;
        }

        if (in_escape) {
            if (*src == '[') {
                src++;
                continue;
            }

            // 璺宠繃鐩村埌鎵惧埌鍛戒护瀛楁瘝
            if ((*src >= 'A' && *src <= 'Z') || (*src >= 'a' && *src <= 'z')) {
                in_escape = 0;
                src++;
                continue;
            }

            src++;
            continue;
        }

        // 澶嶅埗鏅�氬瓧绗�
        *dst++ = *src++;
    }

    *dst = '\0';
}

// r鐎规矮绠熼弮銉ョ箶鏉堟挸鍤崙鑺ユ殶
void my_log(Log *log, const char *fmt, ...) {
    va_list vargs;
    int len;
    va_start(vargs, fmt);
    len = vsnprintf(logBuffer, LOG_BUFFER_SIZE - 1, fmt, vargs);
    va_end(vargs);
    logList[0]->write(logBuffer, len);
}
/**
 * @brief 16閺夆晜绋戦崺妤佹綇閹惧啿姣�
 * 
 * @param log log閻庣數顢婇挅锟�
 * @param level 闁哄啨鍎辩换鏃傜棯瑜嶉崺锟�
 * @param base 闁告劕鎳庨悺銊╁春閸濆嫭绲�
 * @param length 闂傦拷閸喖顔�
 */
void logHexDump(Log *log, LogLevel level, void *base, unsigned int length)
{
    unsigned char *address;
    unsigned int len;
    unsigned int printLen = 0;

    if (length == 0 || (log != LOG_ALL_OBJ && log->level < level))
    {
        return;
    }
#if LOG_USING_LOCK == 1
    logLock(log);
#endif /* LOG_USING_LOCK == 1 */
    len = snprintf(logBuffer, LOG_BUFFER_SIZE - 1, "memory of 0x%08x, size: %d:\r\n%s",
                   (unsigned int)base, length, memPrintHead);
    logWriteBuffer(log, level, logBuffer, len);

    len = length;
    
    address = (unsigned char *)((unsigned int)base & (~0x0000000F));
    length += (unsigned int)base - (unsigned int)address;
    length = (length + 15) & (~0x0000000F);

    while (length)
    {
        printLen += sprintf(logBuffer + printLen, memPrintAddr, (unsigned int)address);
        for (int i = 0; i < 16; i++)
        {
            if ((unsigned int)(address + i) < (unsigned int)base
                || (unsigned int)(address + i) >= (unsigned int)base + len)
            {
                logBuffer[printLen ++] = ' ';
                logBuffer[printLen ++] = ' ';
                logBuffer[printLen ++] = ' ';
            }
            else
            {
                printLen += sprintf(logBuffer + printLen, "%02x ", *(address + i));
            }
        }
        logBuffer[printLen ++] = '|';
        logBuffer[printLen ++] = ' ';
        for (int i = 0; i < 16; i++)
        {
            if ((unsigned int)(address + i) < (unsigned int)base
                || (unsigned int)(address + i) >= (unsigned int)base + len)
            {
                logBuffer[printLen ++] = ' ';
            }
            else
            {
                if (*(address + i) >= 32 && *(address + i) <= 126)
                {
                    printLen += sprintf(logBuffer + printLen, "%c", *(address + i));
                }
                else
                {
                    logBuffer[printLen ++] = '.';
                }
            }
        }
        logBuffer[printLen ++] = ' ';
        logBuffer[printLen ++] = '|';
        logBuffer[printLen ++] = '\r';
        logBuffer[printLen ++] = '\n';
        logWriteBuffer(log, level, logBuffer, printLen);
        address += 16;
        length -= 16;
        printLen = 0;
    }
#if LOG_USING_LOCK == 1
    logUnlock(log);
#endif /* LOG_USING_LOCK == 1 */
}
#if SHELL_USING_COMPANION == 1
SHELL_EXPORT_CMD_AGENCY(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
hexdump, logHexDump, hex dump\r\n hexdump [base] [len],
(void *)shellCompanionGet(shellGetCurrent(), SHELL_COMPANION_ID_LOG), LOG_NONE, (void *)p1, (unsigned int)p2);
#else
SHELL_EXPORT_CMD(
SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC)|SHELL_CMD_DISABLE_RETURN,
hexdump, logHexDump, hex dump\r\n hexdump [log] [level] [base] [len]);
#endif /** SHELL_USING_COMPANION == 1 */

#if SHELL_USING_COMPANION == 1
void logSwitchLevel(Shell *shell)
{
    Log *log = shellCompanionGet(shell, SHELL_COMPANION_ID_LOG);
    SHELL_ASSERT(log, return);
    log->level = (LogLevel)(log->level >= LOG_ALL ? LOG_NONE : (log->level + 1));
    logPrintln("set log level: %d", log->level);
}
SHELL_EXPORT_KEY(SHELL_CMD_PERMISSION(0), 0x04000000, logSwitchLevel, switch log level);
#endif /** SHELL_USING_COMPANION == 1 */



        
