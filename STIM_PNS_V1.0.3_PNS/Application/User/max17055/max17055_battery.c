#include "max17055_battery.h"
#include "app_i2c.h"
#include "app_log.h"
#include "main_user.h"

#include <elog.h>                 // 蹇呴』鍦� LOG_TAG 涓� LOG_LVL 涓嬮潰
#include <easyflash.h>
#include <elog_flash.h>
#include "app_flash_log.h"
#include "flash_interface_ext_rtos_NVS.h"
#include <ti/common/cc26xx/flash_interface/flash_interface.h>
#include "app_flash_log.h"


#if 1
    #define logdebug(...) logInfo(__VA_ARGS__)
#else
    #define logdebug(...) do {} while(0)  // 绌烘搷浣滐紝閬垮厤璇硶閿欒
#endif

static max_iic_init_s max_iic_init_info = { 0 };
struct max17055_device_info max17055_dev = {0, };

static int max17055_battery_read(uint8_t reg, uint16_t *val)
{
    uint8_t data[2];

    if(max_iic_init_info.p_read(MAX17055_I2C_DEVICE_ADD,  &reg, 1, data, 2) != I2C_STATUS_SUCCESS) {
        return I2C_STATUS_ERROR;
    }

    *val = (data[1] << 8) | data[0] ;
    return I2C_STATUS_SUCCESS;
}

static int8_t max17055_battery_write(uint8_t reg, uint16_t val)
{
    uint8_t data[2] = {val, val>>8};

    if(max_iic_init_info.p_write(MAX17055_I2C_DEVICE_ADD,  &reg, 1, data, 2) != I2C_STATUS_SUCCESS) {
            return I2C_STATUS_ERROR;
    }
    return I2C_STATUS_SUCCESS;
}

/**
  * @brief  鑾峰彇鐢垫睜娓╁害
  * @param  di: MAX17055璁惧淇℃伅鎸囬拡
  * @param  temp: 娓╁害鍊硷紝杈撳嚭鍙傛暟锛堟憚姘忓害锛�
  * @retval RT_EOK: 鎴愬姛, 璐熸暟: 澶辫触
  */
static int max17055_get_temperature(int *temp)
{
    int ret;
    uint16_t val;

    ret = max17055_dev.read(MAX17055_TEMP_REG, &val);
    if (ret < 0)
    {
        return ret;
    }

    *temp = val;
    /* 澶勭悊鏈夌鍙锋暟锛堜簩杩涘埗琛ョ爜锛� */
    if (*temp & 0x8000)
        *temp |= 0xFFFF0000;

    /* 杞崲涓烘憚姘忓害锛歀SB = 1/256 鎽勬皬搴� */
    *temp >>= 8;

    return 0;
}

/**
  * @brief  鑾峰彇娓╁害闄愬埗鍊�
  * @param  di: MAX17055璁惧淇℃伅鎸囬拡
  * @param  temp: 娓╁害鍊硷紝杈撳嚭鍙傛暟
  * @param  shift: 绉讳綅鍙傛暟锛堢敤浜庡尯鍒嗛珮浣庡瓧鑺傦級
  * @retval 0: 鎴愬姛, 璐熸暟: 澶辫触
  */
static int max17055_get_temperature_limit(int *temp, int shift)
{
    int ret;
    uint16_t val;

    ret = max17055_dev.read(MAX17055_TALRTTH_REG, &val);
    if (ret < 0)
    {
        logdebug("read temperature limit");
        return ret;
    }

    *temp = val >> shift;
    /* 澶勭悊鏈夌鍙锋暟 */
    if(*temp & 0x80)
        *temp |= 0xFFFFFF00;

    /* LSB涓�1鎽勬皬搴� */
    return 0;
}

/**
  * @brief  鑾峰彇鐢垫睜鍋ュ悍鐘舵��
  * @param  di: MAX17055璁惧淇℃伅鎸囬拡
  * @param  health: 鍋ュ悍鐘舵�侊紝杈撳嚭鍙傛暟
  * @retval 0: 鎴愬姛, 璐熸暟: 澶辫触
  */
static int max17055_get_battery_health(int *health)
{
    int temp;
    int vbatt;
    int ret;
    uint16_t val;

    /* 璇诲彇鐢垫睜鐢靛帇 */
    ret = max17055_dev.read(MAX17055_VCELL_REG, &val);
    if (ret < 0)
        goto health_error;

    /* bits [0-3] unused */
    vbatt = MAX17055_LSB_TO_UVOLTS(val);
    /* 杞崲涓烘浼� */
    vbatt /= 1000;

    /* 妫�鏌ヨ繃鍘� */
    if (vbatt > max17055_dev.cache.volt_max + 150) { /* 鐢靛帇鏈�澶у宸紙150mV锛� */
        *health = POWER_SUPPLY_HEALTH_OVERVOLTAGE;
        goto out;
    }

    /* 璇诲彇娓╁害 */
    ret = max17055_get_temperature(&temp);
    if (ret < 0)
        goto health_error;

    /* 妫�鏌ヨ繃鐑� */
    if (temp >= max17055_dev.cache.temp_max) {
        *health = POWER_SUPPLY_HEALTH_OVERHEAT;
        goto out;
    }
    max17055_dev.cache.temperature = temp;   //灏嗘俯搴︿俊鎭�佸叆鐢ㄦ埛淇℃伅

    *health = POWER_SUPPLY_HEALTH_GOOD;

out:
    return 0;

health_error:
    return ret;
}

/**
  * @brief  鍐欏叆骞堕獙璇佸瘎瀛樺櫒鍊�
  * @param  di: MAX17055璁惧淇℃伅鎸囬拡
  * @param  reg: 瀵勫瓨鍣ㄥ湴鍧�
  * @param  value: 瑕佸啓鍏ョ殑鍊�
  * @retval 0: 鎴愬姛, 璐熸暟: 澶辫触
  */
static int max17055_write_verify_reg(uint8_t reg, uint16_t value)
{
    int retries = 8;
    int ret;
    uint16_t read_value;

    do {
        ret = max17055_dev.write(reg, value);
        delay_ms(3);
        ret = max17055_dev.read(reg, &read_value);
        if (read_value != value) {
            ret = -1;
            retries--;
        }
    } while (retries && read_value != value);

    if (ret < 0)
        logdebug("Couldn't verify write 0x%4x to reg 0x%2x\n", value, reg);

    return ret;
}

static int max17055_raw_current_to_uamps(uint16_t curr)
{
    int res = curr;
    /* Negative */
    if (res & 0x8000)
        res |= 0xFFFF0000;

    res *= 1562500 / (max17055_dev.cache.rsense * 1000);
    return res;
}

/**
@brief 鑾峰彇鐢垫睜灞炴��
@param di: MAX17055璁惧淇℃伅鎸囬拡锛堝嚱鏁板師鍨嬩腑澹版槑浣嗗疄闄呮湭浣跨敤锛屽彲鑳芥槸涓敊璇級
@param psp: 瑕佽幏鍙栫殑灞炴�ф灇涓惧��
@param val: 灞炴�у�硷紝杈撳嚭鍙傛暟
@retval 0: 鎴愬姛, 璐熸暟: 澶辫触
*/
static int max17055_get_property(enum max17055_prop_e psp, int *intval)
{
    uint16_t reg; // 瀛樺偍浠庡瘎瀛樺櫒璇诲彇鐨勫師濮嬫暟鎹�
    int ret; // 瀛樺偍鍑芥暟璋冪敤杩斿洖鍊�

    // 鏍规嵁灞炴�х被鍨嬪垎鍒鐞�
    switch (psp) {
        case POWER_SUPPLY_PROP_TEMP: // 鑾峰彇褰撳墠娓╁害锛堣皟鐢ㄤ笓鐢ㄥ嚱鏁帮級
        if(max17055_get_temperature(intval) < 0)
            return -1;
        break;

        case POWER_SUPPLY_PROP_HEALTH:   // 鑾峰彇鐢垫睜鍋ュ悍鐘舵��
        if(max17055_get_battery_health(intval) < 0)
            return -1;
        break;

        case POWER_SUPPLY_PROP_VOLTAGE_NOW:// 璇诲彇褰撳墠鐢靛帇瀵勫瓨鍣� 鍗曚綅: mV
        if(max17055_dev.read(MAX17055_VCELL_REG, &reg) < 0)
            return -1;
        *intval = MAX17055_LSB_TO_UVOLTS(reg) / 1000; // 杞崲涓烘浼忥紙mV锛�
        break;

        case POWER_SUPPLY_PROP_CURRENT_AVG:  // 璇诲彇骞冲潎鐢垫祦瀵勫瓨鍣� 鍗曚綅: mA
        if(max17055_dev.read(MAX17055_AVGCURRENT_REG, &reg) < 0)
            return -1;
        *intval = max17055_raw_current_to_uamps(reg) / 1000; // 杞崲涓烘瀹夛紙mA锛�
        break;

        case POWER_SUPPLY_PROP_CURRENT_NOW: // 璇诲彇褰撳墠鐢垫祦瀵勫瓨鍣� 鍗曚綅: mA
        if(max17055_dev.read(MAX17055_CURRENT_REG, &reg) < 0)
            return -1;
        *intval = max17055_raw_current_to_uamps(reg) / 1000; // 杞崲涓烘瀹夛紙mA锛�
        break;

        case POWER_SUPPLY_PROP_CAPACITY: // 鍓╀綑瀹归噺鐧惧垎姣�
        if(max17055_dev.read(MAX17055_REPSOC_REG, &reg) < 0)
            return -1;
        *intval = reg >> 8; /* RepSOC LSB: 1/256 %锛屽彇楂�8浣嶄负鐧惧垎姣� */
        break;

        case POWER_SUPPLY_PROP_CYCLE_COUNT: // 璇诲彇鍏呯數寰幆娆℃暟瀵勫瓨鍣�
        ret |= max17055_dev.read(MAX17055_CYCLES_REG, &reg);
        if (ret < 0)
        return ret;
        *intval = reg; /* 鍏呯數寰幆娆℃暟锛岀洿鎺ヨ鍙�16浣嶆暟鍊� */
        break;

        case POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN: /* 璁捐瀹归噺 */
        // 璇诲彇璁捐瀹归噺瀵勫瓨鍣�
        ret = max17055_dev.read(MAX17055_DESIGNCAP_REG, &reg);
        if (ret < 0)
        return ret;
        *intval = (reg) * 5 / max17055_dev.cache.rsense; // 鍗曚綅锛氭瀹夋椂锛坢Ah锛�
        break;

        case POWER_SUPPLY_PROP_TIME_TO_EMPTY_AVG: /* 鍗曚綅: 绉� */
        // 璇诲彇骞冲潎鏀剧數鏃堕棿瀵勫瓨鍣�
        ret = max17055_dev.read(MAX17055_TTE_REG, &reg);
        if (ret < 0)
        return ret;
        *intval = (reg * 45) >> 3; /* TTE LSB: 5.625绉掞紝杞崲涓虹 */
        *intval = *intval / 60; //杞崲涓哄垎閽�
        break;

        case POWER_SUPPLY_PROP_CHARGE_FULL:
        // 璇诲彇鎶ュ憡婊″厖瀹归噺瀵勫瓨鍣�
        ret = max17055_dev.read(MAX17055_FULLCAPREP_REG, &reg);
        if (ret < 0)
        return ret;
        *intval = (reg) * 5 / max17055_dev.cache.rsense; /* 婊″厖瀹归噺锛屽崟浣嶏細姣畨鏃讹紙mAh锛�*/
        break;

#if 0

        case POWER_SUPPLY_PROP_VOLTAGE_MAX:
        // 璇诲彇鏈�澶�/鏈�灏忕數鍘嬪瘎瀛樺櫒
        ret = max17055_dev.read(MAX17055_MAXMINVOLT_REG, &reg);
        if (ret < 0)
        return ret;
        *intval = reg >> 8; // 鍙栭珮8浣嶏紙鏈�澶х數鍘嬪�硷級
        *intval *= 20000; /* LSB鍗曚綅: 20mV锛岃浆鎹负寰紡锛坲V锛� */
        break;

        case POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN:
        // 璇诲彇绌鸿浇鐢靛帇瀵勫瓨鍣紙VEMPTY锛�
        ret = max17055_dev.read(MAX17055_VEMPTY_REG, &reg);
        if (ret < 0)
        return ret;
        *intval = reg >> 7; // 鍙栭珮9浣嶏紙绌鸿浇鐢靛帇鍊硷級
        *intval *= 10000; /* LSB鍗曚綅: 10mV锛岃浆鎹负寰紡锛坲V锛� */
        break;

        case POWER_SUPPLY_PROP_VOLTAGE_AVG:
        // 璇诲彇骞冲潎鐢靛帇瀵勫瓨鍣�
        ret = max17055_dev.read(MAX17055_AVGVCELL_REG, &reg);
        if (ret < 0)
        return ret;
        *intval = MAX17055_LSB_TO_UVOLTS(reg); // 杞崲涓哄井浼忥紙uV锛�
        break;

        case POWER_SUPPLY_PROP_VOLTAGE_OCV:
        // 璇诲彇寮�璺數鍘嬶紙OCV锛夊瘎瀛樺櫒
        ret = max17055_dev.read(MAX17055_OCV_REG, &reg);
        if (ret < 0)
        return ret;
        *intval = MAX17055_LSB_TO_UVOLTS(reg); // 杞崲涓哄井浼忥紙uV锛�
        break;




        case POWER_SUPPLY_PROP_CHARGE_NOW: /* 鍓╀綑瀹归噺 */
        // 璇诲彇鎶ュ憡瀹归噺瀵勫瓨鍣�
        ret = max17055_dev.read(MAX17055_REPCAP_REG, &reg);
        if (ret < 0)
        return ret;
        *intval = (reg) * 5 / max17055_dev.cache.rsense; // 鍗曚綅锛氭瀹夋椂锛坢Ah锛�
        break;

        case POWER_SUPPLY_PROP_TEMP_ALERT_MIN:
        // 鑾峰彇娓╁害鎶ヨ涓嬮檺
        ret = max17055_get_temperature_limit(intval, 0);
        if (ret < 0)
        return ret;
        *intval *= 10; /* 杞崲涓�0.1鎽勬皬搴﹀崟浣嶏紙椹卞姩妗嗘灦鏍囧噯鍗曚綅锛� */
        break;

        case POWER_SUPPLY_PROP_TEMP_ALERT_MAX:
        // 鑾峰彇娓╁害鎶ヨ涓婇檺
        ret = max17055_get_temperature_limit(intval, 8);
        if (ret < 0)
        return ret;
        *intval *= 10; /* 杞崲涓�0.1鎽勬皬搴﹀崟浣嶏紙椹卞姩妗嗘灦鏍囧噯鍗曚綅锛� */
        break;


#endif

        default:
        // 涓嶆敮鎸佺殑灞炴�х被鍨�
        return -1;
    }
    return 0;
}


/**
  * @brief  杞鏍囧織浣嶆竻闆�
  * @param  di: MAX17055璁惧淇℃伅鎸囬拡
  * @param  regno: 瀵勫瓨鍣ㄥ湴鍧�
  * @param  mask: 鏍囧織浣嶆帺鐮�
  * @param  timeout: 瓒呮椂鏃堕棿(姣)
  * @retval 0: 鎴愬姛, 璐熸暟: 澶辫触
  */
static int max17055_poll_flag_clear(int regno, int mask, int timeout)
{
   uint16_t data;
   int ret;

   do
   {
       delay_ms(10);
       ret = max17055_dev.read(regno, &data);
       if(ret < 0)
           return ret;

       if(!(data & mask))
           return 0;

       timeout -= 10;
   } while(timeout > 0);

   return -1;
}


/**
  * @brief  MAX17055璁惧鍒濆鍖�
  * @param  di: MAX17055璁惧淇℃伅鎸囬拡
  * @retval 0: 鎴愬姛, 璐熸暟: 澶辫触
  */
static int max17055_dev_init(void)
{
    int ret;
    uint16_t reg;
    uint16_t fgrev, hibcfg, dpacc;

    /* 璇诲彇鑺墖鐗堟湰 */
    ret |= max17055_dev.read(MAX17055_VERSION_REG, &fgrev);



    /* 妫�鏌OR鏍囧織 */
    max17055_dev.read(MAX17055_STATUS_REG, &reg);
    if (!(reg & (1U << 1))){
        logdebug("POR is not set. fouce init\r\n");
    }

    /* 妫�鏌Stat.DNR鏍囧織浣� */
    ret |= max17055_poll_flag_clear(MAX17055_FSTAT_REG, 0x01, 500);

    /* 閫�鍑轰紤鐪犳ā寮� */
    max17055_dev.read(MAX17055_HIBCFG_REG, &hibcfg);
    max17055_dev.write(MAX17055_SoftWakeup_REG, 0x90);
    max17055_dev.write(MAX17055_HIBCFG_REG, 0x0);
    max17055_dev.write(MAX17055_SoftWakeup_REG, 0x0);
    /* 鏍规嵁妯″瀷閫夐」杩涜鍒濆鍖� */

    /* 閫夐」1锛欵Z閰嶇疆妯″紡 */
    max17055_dev.write(MAX17055_DESIGNCAP_REG, max17055_dev.cache.designcap);
    max17055_dev.write(MAX17055_DQACC_REG, max17055_dev.cache.designcap >> 5);
    max17055_dev.write(MAX17055_ICHGTERM_REG, max17055_dev.cache.ichgterm);
    max17055_dev.write(MAX17055_VEMPTY_REG, max17055_dev.cache.vempty);

//    if (max17055_dev.cache.vcharge > 4275){
//        dpacc = (max17055_dev.cache.designcap >> 5) * 0xC800 / (max17055_dev.cache.designcap);
//        max17055_dev.write(MAX17055_DPACC_REG, dpacc);
//        max17055_dev.write(MAX17055_MODELCFG_REG, 0x8400);
//    } else{
        dpacc = (max17055_dev.cache.designcap >> 5) * 0xAC6A / max17055_dev.cache.designcap;
        max17055_dev.write(MAX17055_DPACC_REG, dpacc);
        max17055_dev.write(MAX17055_MODELCFG_REG, 0x8000);
//    }

    /* 璁剧疆浣跨敤澶栭儴鐑晱鐢甸樆 */
    max17055_dev.read(MAX17055_CONFIG_REG, &reg);
    max17055_dev.write(MAX17055_CONFIG_REG, reg | (1U << 15));

    /* 绛夊緟妯″瀷鍒锋柊瀹屾垚 */
    ret |= max17055_poll_flag_clear(MAX17055_MODELCFG_REG, (1U << 15), 500);
    if(ret < 0){
        logdebug("[Option1 model refresh not completed!\r\n");
        return ret;
    }

    /* 鎭㈠鍘熷浼戠湢閰嶇疆 */
    max17055_dev.write(MAX17055_HIBCFG_REG, hibcfg);

    /* 娓呴櫎POR鏍囧織 */
    max17055_dev.read(MAX17055_STATUS_REG, &reg);
    max17055_write_verify_reg(MAX17055_STATUS_REG, reg & ~(1U << 1));

    return 0;
}

/**
  * @brief  MAX17055鐢垫睜妫�娴嬩笌鍒濆鍖�
  * @param  pdev: MAX17055璁惧淇℃伅鎸囬拡
  * @retval RT_EOK: 鎴愬姛, RT_ERROR: 澶辫触
  */
#define BATTERY_CAP              450    //1000MA
#define BATTERY_MAX_CHARGE_MA    600     //鍏呯數鎴鐢垫祦
#define BATTERY_VE               3000   //VE = 3.0V 鏄�滄墜鏈鸿嚜鍔ㄥ叧鏈衡�濈殑閭ｄ釜鐢靛帇  0%鐢甸噺鐨勭數鍘� 鎴鐢靛帇
#define BATTERY_VR               3400   //VR = 3.4v 鏄�滄墜鏈鸿兘閲嶆柊寮�鏈烘樉绀哄厖鐢典腑鈥濈殑閭ｄ釜鐢靛帇  鍏佽寮�鏈虹殑鐢靛帇 绯荤粺璁や负鍏呯數鍒拌繖涓數鍘嬩笉琛ㄧず0v
#define BATTERY_FULL_V           4150   //婊＄數鐢靛帇4.15v
#define BATTERY_TEMP_MAX         60     // 鏈�楂樺伐浣滄俯搴�(鈩�)
#define BATTERY_TEMP_MIN         (-5)   // 鏈�浣庡伐浣滄俯搴�(鈩�)
#define BATTERY_VOLT_CHGING_MAX  4200   // 鍏呯數鏃舵渶澶х數鍘嬮檺鍒�(mV)
#define BATTERY_VOLT_EMPTY       3000    // 绌鸿浇鐢靛帇闃堝��(mV)

#define VEMPTY_REG_VALUE(ve_mV, vr_mV)   ((((uint16_t)((ve_mV) / 10)) << 7) | ((uint16_t)((vr_mV) / 40)))

int max17055_battery_probe(void)
{
    int result = 0;
    
    max17055_dev.read = max17055_battery_read;
    max17055_dev.write = max17055_battery_write;

    // 閲囨牱鐢甸樆10m惟
    max17055_dev.cache.rsense = 10; /* 10m惟閲囨牱鐢甸樆 */
    //鐢垫睜瀹归噺璁剧疆
    max17055_dev.cache.designcap = BATTERY_CAP * max17055_dev.cache.rsense / 5;
    //鍏呯數鎴鐢垫祦 100mA
    max17055_dev.cache.ichgterm = (BATTERY_MAX_CHARGE_MA*4) << 4;
    //鎴鐢靛帇 3V
    max17055_dev.cache.vempty = VEMPTY_REG_VALUE(BATTERY_VE, BATTERY_VR);
    //璁剧疆鍏呯數鐢靛帇 4150mV
    max17055_dev.cache.vcharge = BATTERY_FULL_V;
    //璁剧疆鏈�澶у厖鐢电數鍘�
    max17055_dev.cache.volt_max = BATTERY_VOLT_CHGING_MAX;
    //璁剧疆鐢垫睜娓╁害涓婇檺 60鈩�
    max17055_dev.cache.temp_max = BATTERY_TEMP_MAX;
    //璁剧疆鐢垫睜娓╁害涓嬮檺 -5鈩�
    max17055_dev.cache.temp_min = BATTERY_TEMP_MIN;

    /* 妫�鏌ュ嚱鏁版寚閽堟槸鍚︽湁鏁� */
    if((max17055_dev.read == NULL) || (max17055_dev.write == NULL) ) {
        return -1;
    }

    delay_ms(10); //test

    max17055_dev_init();

    max17055_battery_update();  /* 鏇存柊涓�娆＄數姹犱俊鎭� */

    return 0;
}

/**
  * @brief  MAX17055妯″潡鍒濆鍖�
  */
void max17055_battery_init(void)
{
    struct max17055_device_info *pdata = NULL;

    //鍒濆鍖朓IC
    app_i2c_init();
    max_iic_init_info.p_read = app_i2c_read;
    max_iic_init_info.p_write = app_i2c_write;
    max17055_battery_probe();
}

/**
  * @brief  鏇存柊鐢垫睜淇℃伅
  * @param  di: MAX17055璁惧淇℃伅鎸囬拡
  */
void max17055_battery_update(void)
{
    int ret = 0;


//    //鑾峰彇璁惧褰撳墠娓╁害
//    ret |= max17055_get_property(POWER_SUPPLY_PROP_TEMP, &max17055_dev.cache.temperature);
    //鑾峰彇璁惧鍓╀綑浣跨敤鏃堕棿
    ret |= max17055_get_property(POWER_SUPPLY_PROP_TIME_TO_EMPTY_AVG, &max17055_dev.cache.left_time);
    //鑾峰彇璁惧鍏呮弧鐢电數姹犲閲�
    ret |= max17055_get_property(POWER_SUPPLY_PROP_CHARGE_FULL, &max17055_dev.cache.charge_full);
    // 褰撳墠鐢甸噺鐧惧垎姣�
    ret |= max17055_get_property(POWER_SUPPLY_PROP_CAPACITY, &max17055_dev.cache.capacity);
    // 鍋ュ悍鐘跺喌
    ret |= max17055_get_property(POWER_SUPPLY_PROP_HEALTH, &max17055_dev.cache.health);  //鑾峰彇鍋ュ悍閲岄潰鏈夎幏鍙栫數姹犳俯搴�
    // 鐢垫睜鍏呯數寰幆娆℃暟
    ret |= max17055_get_property(POWER_SUPPLY_PROP_CYCLE_COUNT, &max17055_dev.cache.cycle_count);
    // 鐢垫睜鍏呯數寰幆娆℃暟
    ret |= max17055_get_property(POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN, &max17055_dev.cache.design_full);
    // 骞冲潎鐢垫祦
    ret |= max17055_get_property(POWER_SUPPLY_PROP_CURRENT_AVG, &max17055_dev.cache.average_current);
    // 鐬椂鐢靛帇
    ret |= max17055_get_property(POWER_SUPPLY_PROP_VOLTAGE_NOW, &max17055_dev.cache.voltage);

    logInfo("| temperature | time_to_empty | charge_full | capacity | health | cycle_count | design_full | current | voltage |");
    logInfo("| %5dC      | %10dmin | %8dmAh | %7d%% | %6d | %11d | %8dmAh | %5dmA | %5dmV |",
          max17055_dev.cache.temperature,
          max17055_dev.cache.left_time,
          max17055_dev.cache.charge_full,
          max17055_dev.cache.capacity,
          max17055_dev.cache.health,
          max17055_dev.cache.cycle_count,
          max17055_dev.cache.design_full,
          max17055_dev.cache.average_current,
          max17055_dev.cache.voltage);
exit:
    if(ret < 0) {
//        logdebug("battery_update");
    } else {
        if(max17055_dev.cache.health != 1)
        {
            /* 鍙互鍦ㄦ澶勬坊鍔犵數姹犳晠闅滃鐞� */
        }
    }
}

/**
  * @brief  鑾峰彇鐢ㄦ埛鏁版嵁锛堢數姹犱俊鎭級
  * @param  index: 璁惧绱㈠紩
  * @retval 鐢垫睜淇℃伅缁撴瀯浣撴寚閽堬紝澶辫触杩斿洖0
  */
struct max17055_reg_cache *max17055_get_usr_data(void)
{
    return &max17055_dev.cache;
}
