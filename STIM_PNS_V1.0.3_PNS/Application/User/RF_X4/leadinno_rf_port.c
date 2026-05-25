//实现rf收发数据
#include "leadinno_rf_port.h"
#include "leadinno_app.h"

#include <ti/drivers/timer/GPTimerCC26XX.h>
#include <ti/drivers/dma/UDMACC26XX.h>
#include "ti/devices/cc13x2_cc26x2/driverlib/event.h"
#include "ti/devices/cc13x2_cc26x2/inc/hw_event.h"
#include "ti/devices/cc13x2_cc26x2/driverlib/udma.h"
GPTimerCC26XX_Handle hTimer_capture;

#define DMA_CPATURE_MAX_LEN 150
uint16_t dma_capture_tick[DMA_CPATURE_MAX_LEN];      //dma raw data
uint16_t dma_tick_to_interval[DMA_CPATURE_MAX_LEN];  //the interval between dma data.(filter NFC reverse jump edge)
uint16_t dma_interval_len = 0;  //the valid data length for 'dma_tick_to_interval'.

UDMACC26XX_Handle hdma;
#define DMA_CH_GPT2   9
ALLOCATE_CONTROL_TABLE_ENTRY(dmaTimer2captureTableEntry, DMA_CH_GPT2);

#define DATA_IS_IN_RANGE(data, range_min, range_max)  ( (data)>(range_min) && (data)<(range_max) )
//-------------------------copy form bitbang.c-------------------------------------------
#define CYCLE_FM0                     10800//11380           // FM0的周期长度，单位nS 10400 - 11380

#define MEDIAN_PULSE_TIME_NS    (CYCLE_FM0 / 2)             // FM0的半个周期长度
#define LONG_PULSE_TIME_NS      (CYCLE_FM0)                 // FM0的周期长度

#define MEDIAN_PULSE_CYCLES()     NS_TO_CYCLES( MEDIAN_PULSE_TIME_NS )
#define LONG_PULSE_CYCLES()       NS_TO_CYCLES( LONG_PULSE_TIME_NS   )
//-------------------------copy form bitbang.c end----------------------------------------

void app_stim_capture_mode_init(void)
{
    GPTimerCC26XX_Params params;
    GPTimerCC26XX_Params_init(&params);
    params.width          = GPT_CONFIG_16BIT;
    params.mode           = GPT_MODE_EDGE_TIME;
    params.direction      = GPTimerCC26XX_DIRECTION_UP;
    params.debugStallMode = GPTimerCC26XX_DEBUG_STALL_OFF;
    hTimer_capture = GPTimerCC26XX_open(CONFIG_GPTIMER_CAPTURE_CONST, &params);
    if(hTimer_capture == NULL) {
        logError("Failed to open GPTimer");
    }

    GPTimerCC26XX_setLoadValue(hTimer_capture, 0xFFFF);
    GPTimerCC26XX_setMatchValue(hTimer_capture, 0xFFFF);
}

void init_gpt_timer(void)
{
    if (!hTimer_capture) {
        app_stim_capture_mode_init();
    }

    GPTimerCC26XX_setLoadValue(hTimer_capture, 0xFFFF);
    GPTimerCC26XX_setCaptureEdge(hTimer_capture, GPT_CTL_TAEVENT_BOTH);
    GPIO_setConfigAndMux(IOID_20, GPIO_CFG_INPUT, GPTimerCC26XX_getPinMux(hTimer_capture));
    //ev fabric
    EventRegister(EVENT_O_UDMACH9SSEL, EVENT_UDMACH9SSEL_EV_GPT2A_DMABREQ);
    //uDMA
    dmaTimer2captureTableEntry.pvSrcEndAddr = &APP_HWREG(GPT2_BASE + GPT_O_TAR);
    dmaTimer2captureTableEntry.pvDstEndAddr = &dma_capture_tick[DMA_CPATURE_MAX_LEN-1];
    dmaTimer2captureTableEntry.ui32Control = (UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_1 | UDMA_MODE_BASIC);
    dmaTimer2captureTableEntry.ui32Control |= UDMACC26XX_SET_TRANSFER_SIZE(DMA_CPATURE_MAX_LEN);
    uDMAChannelEnable(UDMA0_BASE, DMA_CH_GPT2);
    //dma ev
    APP_HWREG(GPT2_BASE+GPT_O_DMAEV) |= (1<<2);
}

uint16_t dma_captrue_len = 0;

static uint16_t PW = 4500;
//#define PW 24500
static void change_pw(uint16_t set_pw)
{
    PW = set_pw;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC),
                 change_pw, change_pw, "change_query_len");



#define SCS_BASE            (0xE000E000UL)
#define DWT_BASE            (0xE0001000UL)
#define DWT_CTRL_OFFSET     (0x00)
#define DWT_CYCCNT_OFFSET   (0x04)
#define CoreDebug_DEMCR_TRCENA_Pos         24U                                            /*!< \deprecated CoreDebug DEMCR: TRCENA Position */
#define CoreDebug_DEMCR_TRCENA_Msk         (1UL << CoreDebug_DEMCR_TRCENA_Pos)
#define DWT_CTRL_CYCCNTENA_Msk             (0x1UL /*<< DWT_CTRL_CYCCNTENA_Pos*/)


#define SET_PULSE_HIGH()        { APP_IO_WRITE(CONFIG_GPIO_SPI_STIM_PICO_CONST, 0); }//{DM_TX_SET;     RESET_DWT();}
#define SET_PULSE_LOW()         { APP_IO_WRITE(CONFIG_GPIO_SPI_STIM_PICO_CONST, 1); }//{DM_TX_RESET;   RESET_DWT();}
#define INIT_DWT()              { APP_HWREG(SCS_BASE+CPU_SCS_O_DEMCR) |= CoreDebug_DEMCR_TRCENA_Msk;    \
                                 APP_HWREG(DWT_BASE+DWT_CTRL_OFFSET) |= DWT_CTRL_CYCCNTENA_Msk;               \
                                 RESET_DWT();                                       \
                                }
#define RESET_DWT()             (APP_HWREG(DWT_BASE+DWT_CYCCNT_OFFSET) = 0) //DWT->CYCCNT = 0
#define GET_DWT()               (APP_HWREG(DWT_BASE+DWT_CYCCNT_OFFSET))     //(DWT->CYCCNT)

#define NS_TO_CYCLES(ns)        (ns*48/1000)   //20.83ns ~ 1tick   //(((ns)*2)/10) //200m/80m * 2

//need >= 800ns
#define nsdelay(nsec) {                                                  \
    INIT_DWT();                                                          \
    uint32_t cycles = NS_TO_CYCLES(nsec) - 34;                           \
    while(GET_DWT() < cycles);                                           \
}

static void highLowPulse(uint32_t nbTC)
{
    // High pulse
    nsdelay(nbTC*PW);   // nbTC * TC (12500ns or 6250ns)
    // Low Pulse
    SET_PULSE_LOW();
    nsdelay(PW);      // 1 TC: 12500ns or 6250ns
    SET_PULSE_HIGH();
}

static void separatorPulse(void)
{
    SET_PULSE_LOW();
    nsdelay(5 * PW);
    SET_PULSE_HIGH();
}

//射频发送数据
void rf_send_v_data(void)
{
    // Ensure pulse state is HIGH
    SET_PULSE_HIGH();
    // Lead Code
    separatorPulse();   // 12.5us
    highLowPulse(1);    // 1 High + 1 Low=2 TC (25us  @ tc=12.5)
    highLowPulse(5);    // 5 High + 1 Low=6 TC (75us @ tc=12.5)
}
//发送有效数据
void rf_send_data(uint8_t *data, uint16_t bit_len)
{
    for (uint16_t i = 0; i < bit_len; i++) {
       uint8_t mask = 0x80 >> (i % 8);  // 高位掩码
       if (data[i / 8] & mask) {
           highLowPulse(3);
       } else {
           highLowPulse(1);
       }
    }
}

//射频发送数据
void rf_tx_data(uint8_t *data, uint16_t bit_len)
{



    delay_ms(1);
    int key = CLOSE_MCU_INT();
//    st25r3916_open_rf(OPEN);
    rf_send_v_data();
    rf_send_data(data, bit_len);
    OPEN_MCU_INT(key);

}

//射频接收数据
int rf_rx_data(uint8_t *get_data, uint8_t *get_nbits)
{

    init_gpt_timer();


    uint8_t data;
    uint16_t mis_time;
    if (!get_data || !get_nbits) {
        return -1;
    }
    if(hTimer_capture == NULL) {
//        logInfo("[error] gpt_timer = NULL");
        return -2;
    }

    GPTimerCC26XX_start(hTimer_capture);

    delay_us(2000);
    dma_captrue_len = DMA_CPATURE_MAX_LEN - UDMACC26XX_GET_TRANSFER_SIZE(dmaTimer2captureTableEntry.ui32Control);

    GPTimerCC26XX_stop(hTimer_capture);

    int key = CLOSE_MCU_INT();

//    st25r3916_open_rf(CLOSE);

//    logInfo("dma_captrue_len = %d", dma_captrue_len);
    //parse data
    if (dma_captrue_len <= 5 || dma_captrue_len > DMA_CPATURE_MAX_LEN) {
//        logInfo("__LINE__ = %d", __LINE__);
        return -3;
    }
    //备注 波形会出现毛刺由于天线质量不好 和对3916寄存器不够熟 勉强能用  过滤毛刺的时候没有将时间补上 所以最小值时间比较小 fm0_value1_tick_min fm0_value0_tick_min
    //下面时间勿动调试出来的
    const uint16_t fm0_value1_tick_max = 1450;//13us
    const uint16_t fm0_value1_tick_min = 850;//10us
    const uint16_t fm0_value0_tick_max = 840; //7.5us
    const uint16_t fm0_value0_tick_min = 280; //4.1us
    const uint8_t fm0_misd_tick = 200; //错误数据包
    const uint16_t fm0_valueV_tick_max = 1900;
    const uint16_t fm0_valueV_tick_min = 1460;
    uint16_t right_tick_cont = 0;
    //计算每段电平的时间长度
    for (uint16_t i = 0; i < (dma_captrue_len-1); i++) {
        if((dma_capture_tick[i+1] - dma_capture_tick[i])*2 > fm0_misd_tick) {
            dma_tick_to_interval[right_tick_cont++] = (dma_capture_tick[i+1] - dma_capture_tick[i]) * 2 + mis_time;// //做差输出的数据x2 为标准时间us dma_capture_tick[i+1] 这个是存储定时器的时间点
            mis_time = 0;
//            if(i<7) {
//                logInfo("tick[%d] = %d", i, dma_tick_to_interval[i]); //电平时间长度
//            }
//            logInfo(" dma_capture_tick[i+1] = %d, dma_capture_tick[i] = %d", dma_capture_tick[i+1], dma_capture_tick[i]  );
        } else if(dma_capture_tick[i+1] < dma_capture_tick[i]) { //溢出 初始化的时候没有重置定时器conut
            dma_tick_to_interval[right_tick_cont++] = (65535 - dma_capture_tick[i] + dma_capture_tick[i+1]) * 2 + mis_time;// //做差输出的数据x2 为标准时间us dma_capture_tick[i+1] 这个是存储定时器的时间点
            mis_time = 0;
//            if(i<7) {
//                logInfo("tick_to_interval[%d] = %d", i, dma_tick_to_interval[i]); //电平时间长度
//                logInfo("max count timer");
//            }
        } else { //毛刺波形  中间分割的毛刺软件可以处理麻烦 就认为通讯不稳定 切功率通讯
//            logInfo("have mis pulse");
            mis_time += (dma_capture_tick[i+1] - dma_capture_tick[i])*2;
        }
    }

    uint8_t data_start_indx = 0;

    for(uint8_t i=0; i<9; i++){
        if(DATA_IS_IN_RANGE(dma_tick_to_interval[i], fm0_valueV_tick_min, fm0_valueV_tick_max)) { //找V码
            if(i>=4) {
                if(DATA_IS_IN_RANGE(dma_tick_to_interval[i+1], fm0_value1_tick_min, fm0_value1_tick_max)
                 && DATA_IS_IN_RANGE(dma_tick_to_interval[i-1], fm0_value0_tick_min, fm0_value0_tick_max)
                 && DATA_IS_IN_RANGE(dma_tick_to_interval[i-2], fm0_value1_tick_min, fm0_value1_tick_max)
                 && DATA_IS_IN_RANGE(dma_tick_to_interval[i-3], fm0_value0_tick_min, fm0_value0_tick_max)) {
//                 && DATA_IS_IN_RANGE(dma_tick_to_interval[i-4], fm0_value0_tick_min, fm0_value0_tick_max)) {
                    data_start_indx = i + 2;
                    break;
                }
            }
        }
    }
    if(data_start_indx == 0) {
         logInfo("Preamble error  V error");
        return -20;
    }

    uint8_t bit_len=0;

    memset(get_data, 0, (right_tick_cont - data_start_indx + 7) / 8);  // 预估最大字节数

    for(uint16_t i = data_start_indx; i < right_tick_cont; i++) {
        if ( DATA_IS_IN_RANGE(dma_tick_to_interval[i], fm0_value1_tick_min, fm0_value1_tick_max) ) {//判断1
            get_data[(bit_len)/8] |= (1<<(7-bit_len%8));
        } else if (DATA_IS_IN_RANGE(dma_tick_to_interval[i], fm0_value0_tick_min, fm0_value0_tick_max) &&
            DATA_IS_IN_RANGE(dma_tick_to_interval[i+1], fm0_value0_tick_min, fm0_value0_tick_max)) { //判断0
            i++; //judged two data, so add one here
        } else  { //数据错误
            return -60;
        }
        bit_len++;
    }

    *get_nbits = bit_len;

    OPEN_MCU_INT(key);

//    logInfo("rf_get_data_len = %d", *get_data_len);
    return 0;
}


