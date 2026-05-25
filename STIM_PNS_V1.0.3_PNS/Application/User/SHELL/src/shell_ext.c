/**
 * @file shell_ext.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief shell extensions
 * @version 3.0.0
 * @date 2019-12-31
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "shell_cfg.h"
#include "shell.h"
#include "shell_ext.h"
#include "string.h"
#include "app_log.h"
#include "log.h"
#define size_t  unsigned int 

extern ShellCommand* shellSeekCommand(Shell *shell,
                                      const char *cmd,
                                      ShellCommand *base,
                                      unsigned short compareLength);
extern int shellGetVarValue(Shell *shell, ShellCommand *command);

#if SHELL_SUPPORT_ARRAY_PARAM == 1
extern int shellSplit(char *string, unsigned short strLen, char *array[], char splitKey, short maxNum);

static int shellExtParseArray(Shell *shell, char *string, char *type, size_t *result);
static int shellExtCleanerArray(Shell *shell, char *type, void *param);
#endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */

#if SHELL_USING_FUNC_SIGNATURE == 1
/**
 * @brief 閼惧嘲褰囨稉瀣╃娑擃亜寮弫鎵閸拷
 * 
 * @param signature 閸戣姤鏆熺粵鎯ф倳
 * @param index 閸欏倹鏆熼柆宥呭坊閸︺劎顒烽崥宥勮厬閻ㄥ嫯鎹ｆ慨瀣偍瀵拷
 * @param type 閼惧嘲褰囬崚鎵畱閸欏倹鏆熺猾璇茬��
 * 
 * @return int 娑撳绔存稉顏勫棘閺佹澘婀粵鎯ф倳娑擃厾娈戠槐銏犵穿
 */
static int shellGetNextParamType(const char *signature, int index, char *type)
{
    const char *p = signature + index;
#if SHELL_SUPPORT_ARRAY_PARAM == 1
    if (*p == '[')
    {
        *type++ = *p++;
        index++;
    }
#endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */
    if (*p == 'L')
    {
        while (*p != ';' && *p != 0)
        {
            *type++ = *p++;
            index++;
        }
        *type++ = *p++;
        index++;
    }
    else if (*p != 0)
    {
        *type++ = *p;
        index++;
    }
    *type = '\0';
    return index;
}

/**
 * @brief 閼惧嘲褰囬張鐔风窡閻ㄥ嫬寮弫棰侀嚋閺侊拷
 * 
 * @param signature 閸戣姤鏆熺粵鎯ф倳
 * 
 * @return int 閸欏倹鏆熸稉顏呮殶
 */
static int shellGetParamNumExcept(const char *signature)
{
    int num = 0;
    const char *p = signature;
    
    while (*p)
    {
    #if SHELL_SUPPORT_ARRAY_PARAM == 1
        if (*p == '[')
        {
            p++;
        }
    #endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */
        if (*p == 'L')
        {
            while (*p != ';' && *p != 0)
            {
                p++;
            }
            p++;
        }
        else
        {
            p++;
        }
        num++;
    }
    return num;
}
#endif

/**
 * @brief 閸掋倖鏌囬弫鏉跨摟鏉╂稑鍩�
 * 
 * @param string 閸欏倹鏆熺�涙顑佹稉锟�
 * @return ShellNumType 鏉╂稑鍩�
 */
static ShellNumType shellExtNumType(char *string)
{
    char *p = string;
    ShellNumType type = NUM_TYPE_DEC;

    if ((*p == '0') && ((*(p + 1) == 'x') || (*(p + 1) == 'X')))
    {
        type = NUM_TYPE_HEX;
    }
    else if ((*p == '0') && ((*(p + 1) == 'b') || (*(p + 1) == 'B')))
    {
        type = NUM_TYPE_BIN;
    }
    else if (*p == '0')
    {
        type = NUM_TYPE_OCT;
    }
    
    while (*p++)
    {
        if (*p == '.' && *(p + 1) != 0)
        {
            type = NUM_TYPE_FLOAT;
            break;
        }
    }

    return type;
}


/**
 * @brief 鐎涙顑佹潪顒佹殶鐎涳拷
 * 
 * @param code 鐎涙顑�
 * @return char 閺佹澘鐡�
 */
static char shellExtToNum(char code)
{
    if ((code >= '0') && (code <= '9'))
    {
        return code -'0';
    }
    else if ((code >= 'a') && (code <= 'f'))
    {
        return code - 'a' + 10;
    }
    else if ((code >= 'A') && (code <= 'F'))
    {
        return code - 'A' + 10;
    }
    else
    {
        return 0;
    }
}


/**
 * @brief 鐟欙絾鐎界�涙顑侀崣鍌涙殶
 * 
 * @param string 鐎涙顑佹稉鎻掑棘閺侊拷
 * @return char 鐟欙絾鐎介崙铏规畱鐎涙顑�
 */
static char shellExtParseChar(char *string)
{
    char *p = (*string == '\'') ? (string + 1) : string;
    char value = 0;

    if (*p == '\\')
    {
        switch (*(p + 1))
        {
        case 'b':
            value = '\b';
            break;
        case 'r':
            value = '\r';
            break;
        case 'n':
            value = '\n';
            break;
        case 't':
            value = '\t';
            break;
        case '0':
            value = 0;
            break;
        default:
            value = *(p + 1);
            break;
        }
    }
    else
    {
        value = *p;
    }
    return value;
}


/**
 * @brief 鐟欙絾鐎界�涙顑佹稉鎻掑棘閺侊拷
 * 
 * @param string 鐎涙顑佹稉鎻掑棘閺侊拷
 * @return char* 鐟欙絾鐎介崙铏规畱鐎涙顑佹稉锟�
 */
static char* shellExtParseString(char *string)
{
    char *p = string;
    unsigned short index = 0;

    if (*string == '\"')
    {
        p = ++string;
    }

    while (*p)
    {
        if (*p == '\\')
        {
            *(string + index) = shellExtParseChar(p);
            p++;
        }
        else if (*p == '\"')
        {
            *(string + index) = 0;
        }
        else
        {
            *(string + index) = *p;
        }
        p++;
        index ++;
    }
    *(string + index) = 0;
    return string;
}


/**
 * @brief 鐟欙絾鐎介弫鏉跨摟閸欏倹鏆�
 * 
 * @param string 鐎涙顑佹稉鎻掑棘閺侊拷
 * @return size_t 鐟欙絾鐎介崙铏规畱閺佹澘鐡�
 */
static size_t shellExtParseNumber(char *string)
{
    ShellNumType type = NUM_TYPE_DEC;
    char radix = 10;
    char *p = string;
    char offset = 0;
    signed char sign = 1;
    size_t valueInt = 0;
    float valueFloat = 0.0;
    size_t devide = 0;

    if (*string == '-')
    {
        sign = -1;
    }

    type = shellExtNumType(string + ((sign == -1) ? 1 : 0));

    switch ((char)type)
    {
    case NUM_TYPE_HEX:
        radix = 16;
        offset = 2;
        break;
    
    case NUM_TYPE_OCT:
        radix = 8;
        offset = 1;
        break;

    case NUM_TYPE_BIN:
        radix = 2;
        offset = 2;
        break;
    
    default:
        break;
    }

    p = string + offset + ((sign == -1) ? 1 : 0);

    while (*p)
    {
        if (*p == '.')
        {
            devide = 1;
            p++;
            continue;
        }
        valueInt = valueInt * radix + shellExtToNum(*p);
        devide *= 10;
        p++;
    }
    if (type == NUM_TYPE_FLOAT && devide != 0)
    {
        valueFloat = (float)valueInt / devide * sign;
        return *(size_t *)(&valueFloat);
    }
    else
    {
        return valueInt * sign;
    }
}


/**
 * @brief 鐟欙絾鐎介崣姗�鍣洪崣鍌涙殶
 * 
 * @param shell shell鐎电钖�
 * @param var 閸欐﹢鍣�
 * @return size_t 閸欐﹢鍣洪崐锟�
 */
static size_t shellExtParseVar(Shell *shell, char *var)
{
    ShellCommand *command = shellSeekCommand(shell,
                                             var + 1,
                                             shell->commandList.base,
                                             0);
    if (command)
    {
        return shellGetVarValue(shell, command);
    }
    else
    {
        return 0;
    }
}


/**
 * @brief 鐟欙絾鐎介崣鍌涙殶
 * 
 * @param shell shell鐎电钖�
 * @param string 閸欏倹鏆�
 * @param type 閸欏倹鏆熺猾璇茬��
 * @param result 鐟欙絾鐎界紒鎾寸亯
 * 
 * @return int 0 鐟欙絾鐎介幋鎰 --1 鐟欙絾鐎芥径杈Е
 */
int shellExtParsePara(Shell *shell, char *string, char *type, unsigned int *result)
{
    if (type == NULL || (*string == '$' && *(string + 1)))
    {
        if (*string == '\'' && *(string + 1))
        {
            *result = (size_t)shellExtParseChar(string);
            return 0;
        }
        else if (*string == '-' || (*string >= '0' && *string <= '9'))
        {
            *result = shellExtParseNumber(string);
            return 0;
        }
        else if (*string == '$' && *(string + 1))
        {
            *result = shellExtParseVar(shell, string);
            return 0;
        }
        else if (*string)
        {
            *result = (size_t)shellExtParseString(string);
            return 0;
        }
    }
#if SHELL_USING_FUNC_SIGNATURE == 1
    else
    {
        if (*string == '$' && *(string + 1))
        {
            *result = shellExtParseVar(shell, string);
            return 0;
        }
    #if SHELL_SUPPORT_ARRAY_PARAM == 1
        else if (type[0] == '[')
        {
            return shellExtParseArray(shell, string, type, result);
        }
    #endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */
        else if (strcmp("c", type) == 0)
        {
            *result = (size_t)shellExtParseChar(string);
            return 0;
        }
        else if (strcmp("q", type) == 0
                 || strcmp("h", type) == 0
                 || strcmp("i", type) == 0
                 || strcmp("f", type) == 0
                 || strcmp("p", type) == 0)
        {
            *result = shellExtParseNumber(string);
            return 0;
        }
        else if (strcmp("s", type) == 0)
        {
            *result = (size_t)shellExtParseString(string);
            return 0;
        }
        else
        {
            ShellCommand *command = shellSeekCommand(shell,
                                                     type,
                                                     shell->commandList.base,
                                                     0);
            if (command != NULL)
            {
                void *param;
                if (command->data.paramParser.parser(shellExtParseString(string), &param) == 0)
                {
                    *result = (size_t)param;
                    return 0;
                }
                else
                {
                    shellWriteString(shell, "Parse param for type: ");
                    shellWriteString(shell, type);
                    shellWriteString(shell, " failed\r\n");
                    return -1;
                }
            }
            else
            {
                shellWriteString(shell, "Can't find the param parser for type: ");
                shellWriteString(shell, type);
                shellWriteString(shell, "\r\n");
                return -1;
            }
        }
    }
#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */
    return -1;
}


#if SHELL_USING_FUNC_SIGNATURE == 1
/**
 * @brief 濞撳懐鎮婇崣鍌涙殶
 * 
 * @param shell shell 
 * @param type 閸欏倹鏆熺猾璇茬��
 * @param param 閸欏倹鏆�
 * 
 * @return int 0 濞撳懐鎮婇幋鎰 -1 濞撳懐鎮婃径杈Е
 */
int shellExtCleanerPara(Shell *shell, char *type, size_t param)
{
    if (type == NULL)
    {
        return 0;
    }
    else
    {
    #if SHELL_SUPPORT_ARRAY_PARAM == 1
        if (type[0] == '[') {
            return shellExtCleanerArray(shell, type, (void *) param);
        }
        else
    #endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */
        if (strcmp("c", type) == 0
            || strcmp("q", type) == 0
            || strcmp("h", type) == 0
            || strcmp("i", type) == 0
            || strcmp("f", type) == 0
            || strcmp("p", type) == 0
            || strcmp("s", type) == 0)
        {
            return 0;
        }
        else
        {
            ShellCommand *command = shellSeekCommand(shell,
                                                     type,
                                                     shell->commandList.base,
                                                     0);
            if (command != NULL && command->data.paramParser.cleaner != NULL)
            {
                return command->data.paramParser.cleaner((void *)param);
            }
        }
    }
    return -1;
}
#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */

#if SHELL_SUPPORT_ARRAY_PARAM == 1
/**
 * @brief 娴兼壆鐣婚弫鎵矋闂�鍨
 * 
 * @param string 閺佹壆绮嶉崣鍌涙殶
 * 
 * @return int 娴兼壆鐣婚惃鍕殶缂佸嫰鏆辨惔锟�
 */
static int shellEstimateArrayLength(char *string)
{
    int length = 0;
    char *p = string;
    while (*p)
    {
        if (*p == ',')
        {
            length++;
        }
        p++;
    }
    return length + 1;
}

/**
 * @brief 閸掑棗澹婇弫鎵矋閸欏倹鏆�
 * 
 * @param string 閺佹壆绮嶉崣鍌涙殶
 * @param array 閸掑棗澹婇崥搴ｆ畱鐎涙顑佹稉鍙夋殶缂侊拷
 * 
 * @return int 閺佹壆绮嶉梹鍨
 */
static int shellSplitArray(char *string, char ***array)
{
    int strLen = strlen(string);
    if (string[strLen - 1] == ']')
    {
        string[--strLen] = 0;
    }
    if (string[0] == '[')
    {
        --strLen;
        string++;
    }
    int size = shellEstimateArrayLength(string);
    *array = SHELL_MALLOC(size * sizeof(char *));
    return shellSplit(string, strLen, *array, ',', size);
}

/**
 * @brief 鐟欙絾鐎介弫鎵矋閸欏倹鏆�
 * 
 * @param shell shell 鐎电钖�
 * @param string 閺佹壆绮嶉崣鍌涙殶
 * @param type 閸欏倹鏆熺猾璇茬��
 * @param result 鐟欙絾鐎界紒鎾寸亯
 * 
 * @return int 0 鐟欙絾鐎介幋鎰 -1 鐟欙絾鐎芥径杈Е
 */
static int shellExtParseArray(Shell *shell, char *string, char *type, size_t *result)
{
    char **params;
    int size = shellSplitArray(string, &params);
    int elementBytes = sizeof(void *);

    if (strcmp(type + 1, "q") == 0)
    {
        elementBytes = sizeof(char);
    }
    else if (strcmp(type + 1, "h") == 0)
    {
        elementBytes = sizeof(short);
    }
    else if (strcmp(type + 1, "i") == 0)
    {
        elementBytes = sizeof(int);
    }

    ShellArrayHeader *header = SHELL_MALLOC(elementBytes * size + sizeof(ShellArrayHeader));
    *result = (size_t) ((size_t) header + sizeof(ShellArrayHeader));
    header->size = size;
    header->elementBytes = elementBytes;
    for (short i = 0; i < size; i++)
    {
        size_t value;
        if (shellExtParsePara(shell, params[i], type + 1, &value) != 0)
        {
            SHELL_FREE(header);
            SHELL_FREE(params);
            return -1;
        }
        memcpy((void *) ((size_t) *result + elementBytes * i), &value, elementBytes);
    }

    SHELL_FREE(params);
    return 0;
}

/**
 * @brief 濞撳懐鎮婇弫鎵矋閸欏倹鏆�
 * 
 * @param shell shell 鐎电钖�
 * @param type 閸欏倹鏆熺猾璇茬��
 * @param param 閸欏倹鏆�
 * 
 * @return int 0 濞撳懐鎮婇幋鎰 -1 濞撳懐鎮婃径杈Е
 */
static int shellExtCleanerArray(Shell *shell, char *type, void *param)
{
    ShellArrayHeader *header = (ShellArrayHeader *) ((size_t) param - sizeof(ShellArrayHeader));
    for (short i = 0; i < header->size; i++)
    {
        if (shellExtCleanerPara(shell, type + 1, *(size_t *) ((size_t) param + header->elementBytes * i)) != 0)
        {
            return -1;
        }
    }
    SHELL_FREE(header);
    return 0;
}

/**
 * @brief 閼惧嘲褰囬弫鎵矋婢堆冪毈
 * 
 * @param param 閺佹壆绮�
 * 
 * @return int 閺佹壆绮嶆径褍鐨�
 */
int shellGetArrayParamSize(void *param)
{
    ShellArrayHeader *header = (ShellArrayHeader *) ((size_t) param - sizeof(ShellArrayHeader));
    return header->size;
}
#endif /** SHELL_SUPPORT_ARRAY_PARAM == 1 */


/**
 * @brief 閹笛嗩攽閸涙垝鎶�
 * 
 * @param shell shell鐎电钖�
 * @param command 閸涙垝鎶�
 * @param argc 閸欏倹鏆熸稉顏呮殶
 * @param argv 閸欏倹鏆�
 * @return int 鏉╂柨娲栭崐锟�
 */
int shellExtRun(Shell *shell, ShellCommand *command, int argc, char *argv[])
{
    int ret = 0;
    size_t params[SHELL_PARAMETER_MAX_NUMBER] = {0};

    int paramNum = command->attr.attrs.paramNum > (argc - 1) ? 
        command->attr.attrs.paramNum : (argc - 1);
#if SHELL_USING_FUNC_SIGNATURE == 1
    char type[16];
    int index = 0;
    
    if (command->data.cmd.signature != NULL)
    {
        int except = shellGetParamNumExcept(command->data.cmd.signature);
        if (except != argc - 1)
        {
            shellWriteString(shell, "Parameters number incorrect\r\n");
            return -1;
        }
    }
#endif
    for (int i = 0; i < argc - 1; i++)
    {
    #if SHELL_USING_FUNC_SIGNATURE == 1
        if (command->data.cmd.signature != NULL) {
            index = shellGetNextParamType(command->data.cmd.signature, index, type);
            if (shellExtParsePara(shell, argv[i + 1], type, &params[i]) != 0)
            {
                return -1;
            }
        }
        else
    #endif /** SHELL_USING_FUNC_SIGNATURE == 1 */
        {
            if (shellExtParsePara(shell, argv[i + 1], NULL, &params[i]) != 0)
            {
                return -1;
            }
        }
    }

    switch (paramNum)
    {
#if SHELL_PARAMETER_MAX_NUMBER >= 1
    case 0:
        ret = command->data.cmd.function();
        break;
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 1 */
#if SHELL_PARAMETER_MAX_NUMBER >= 2
    case 1:
    {
        int (*func)(size_t) = command->data.cmd.function;
        ret = func(params[0]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 2 */
#if SHELL_PARAMETER_MAX_NUMBER >= 3
    case 2:
    {
        int (*func)(size_t, size_t) = command->data.cmd.function;
        ret = func(params[0], params[1]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 3 */
#if SHELL_PARAMETER_MAX_NUMBER >= 4
    case 3:
    {
        int (*func)(size_t, size_t, size_t) = command->data.cmd.function;
        ret = func(params[0], params[1], params[2]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 4 */
#if SHELL_PARAMETER_MAX_NUMBER >= 5
    case 4:
    {
        int (*func)(size_t, size_t, size_t, size_t) = command->data.cmd.function;
        ret = func(params[0], params[1], params[2], params[3]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 5 */
#if SHELL_PARAMETER_MAX_NUMBER >= 6
    case 5:
    {
        int (*func)(size_t, size_t, size_t, size_t, size_t) = command->data.cmd.function;
        ret = func(params[0], params[1], params[2], params[3], params[4]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 6 */
#if SHELL_PARAMETER_MAX_NUMBER >= 7
    case 6:
    {
        int (*func)(size_t, size_t, size_t, size_t, size_t, size_t) = command->data.cmd.function;
        ret = func(params[0], params[1], params[2], params[3], params[4], params[5]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 7 */
#if SHELL_PARAMETER_MAX_NUMBER >= 8
    case 7:
    {
        int (*func)(size_t, size_t, size_t, size_t, size_t, size_t, size_t) = command->data.cmd.function;
        ret = func(params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 8 */
#if SHELL_PARAMETER_MAX_NUMBER >= 9
    case 8:
    {
//        int (*func)(size_t, size_t, size_t, size_t, size_t, size_t, size_t) = command->data.cmd.function;
//                ret = func(params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
//                break;
//        int (*func)(size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t)
//            = command->data.cmd.function;
//        ret = func(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 9 */
#if SHELL_PARAMETER_MAX_NUMBER >= 10
    case 9:
    {
        int (*func)(size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t,
                    size_t) = command->data.cmd.function;
        ret = func(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7],
                   params[8]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 10 */
#if SHELL_PARAMETER_MAX_NUMBER >= 11
    case 10:
    {
        int (*func)(size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t,
                    size_t, size_t) = command->data.cmd.function;
        ret = func(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7],
                   params[8], params[9]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 11 */
#if SHELL_PARAMETER_MAX_NUMBER >= 12
    case 11:
    {
        int (*func)(size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t,
                    size_t, size_t, size_t) = command->data.cmd.function;
        ret = func(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7],
                   params[8], params[9], params[10]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 12 */
#if SHELL_PARAMETER_MAX_NUMBER >= 13
    case 12:
    {
        int (*func)(size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t,
                    size_t, size_t, size_t, size_t) = command->data.cmd.function;
        ret = func(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7],
                   params[8], params[9], params[10], params[11]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 13 */
#if SHELL_PARAMETER_MAX_NUMBER >= 14
    case 13:
    {
        int (*func)(size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t,
                    size_t, size_t, size_t, size_t, size_t) = command->data.cmd.function;
        ret = func(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7],
                   params[8], params[9], params[10], params[11], params[12]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 14 */
#if SHELL_PARAMETER_MAX_NUMBER >= 15
    case 14:
    {
        int (*func)(size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t,
                    size_t, size_t, size_t, size_t, size_t, size_t)
            = command->data.cmd.function;
        ret = func(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7],
                   params[8], params[9], params[10], params[11], params[12], params[13]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 15 */
#if SHELL_PARAMETER_MAX_NUMBER >= 16
    case 15:
    {
        int (*func)(size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t,
                    size_t, size_t, size_t, size_t, size_t, size_t, size_t)
            = command->data.cmd.function;
        ret = func(params[0], params[1], params[2], params[3], params[4], params[5], params[6], params[7],
                   params[8], params[9], params[10], params[11], params[12], params[13], params[14]);
        break;
    }
#endif /** SHELL_PARAMETER_MAX_NUMBER >= 16 */
    default:
        ret = -1;
        break;
    }
    
#if SHELL_USING_FUNC_SIGNATURE == 1
    if (command->data.cmd.signature != NULL) {
        index = 0;
        for (int i = 0; i < argc - 1; i++)
        {
            index = shellGetNextParamType(command->data.cmd.signature, index, type);
            shellExtCleanerPara(shell, type, params[i]);
        }
    }
#endif /** SHELL_USING_FUNC_SIGNATURE == 1 */

    return ret;
}

