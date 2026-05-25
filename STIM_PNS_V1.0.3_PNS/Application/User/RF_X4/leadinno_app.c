//该文件实现芯片应用接口
#include "leadinno_app.h"

leadinno_data lead;



#ifdef ENABLE_LEADNNINO_X4
int leadinno_x4_crtl(leadinno_ctl_func_e func)
{
    int err = 0;

    switch(func) {
        case LEAD_GET_VRECP:         //获取电压
            err = x4_rf_get_leadinno_vrecp(&lead.vrecp, 0);  //立即采样
            break;

        case LEAD_GET_TEMP:         //获取温度
            err = x4_rf_get_leadinno_temp(&lead.temp);
            break;

        case LEAD_WRITE_EFUSE:      //烧录efuse
            err = x4_rf_set_leadinno_efuse(lead.write_efuse);
            break;

        case LEAD_READ_EFUSE:       //获取烧录的efuse值
            x4_rf_get_leadinno_efuse(&lead.get_efuse);
            break;

        case LEAD_RESET:            //复位芯片
            rf_reset_chip();
            break;

        case LEAD_GET_RES:          //阻抗测量
            x4_rf_get_leadinno_res(lead.set_res_ch, &lead.get_res[0]);
            break;

        case LEAD_GET_I_STIM:       //测量刺激电流
            x4_rf_get_leadinno_i_stim(&lead.get_i_stim, 0);
            break;

        case LEAD_OUT_PULSE_SIMPLE: //输出简单波形
            x4_rf_out_pulse_simple(&lead.simple);
            break;

        case LEAD_OUT_PULSE_BURST:  //输出爆发刺激波形
            x4_rf_out_pulse_burst(&lead.simple, &lead.burst);
            break;

        case LEAD_OUT_PULSE_TWA:    //输出爆发刺激波形
            x4_rf_pluse_TWA(&lead.TWA);
            break;

        case LEAD_STOP_PULSE:       //停止刺激
            x4_rf_stop_pulse();
            break;

        case LEAD_WRITE_REG:       //写寄存器值
            x4_cmd_write(lead.reg.reg, lead.reg.data);
            break;
        case LEAD_READ_REG:       //写寄存器值
            x4_cmd_read(lead.reg.reg, &lead.reg.data);
            break;
        default:
            break;
    }

    if(st3916.pwm_open_flag == PWM_FLAG_OPEN) {  //如果用户设置了PWM  通讯完需要手动恢复PWM
        rf_pwm_chang(st3916.pwm.duty_percent, st3916.pwm.hz);
    }
    return err;
}
#endif

#ifdef ENABLE_LEADNNINO_X5
int leadinno_x5_crtl(leadinno_ctl_func_e func)
{
    int err = 0;

    switch(func) {
        case LEAD_GET_VRECP:        //获取电压
            err = x5_rf_get_vrecp(&lead.vrecp, 0);  //立即采样
            break;

        case LEAD_GET_TEMP:         //获取温度
            err = x5_rf_get_temp(&lead.temp);
            break;

        case LEAD_WRITE_EFUSE:      //烧录efuse
            err = x5_rf_write_efuse(lead.write_efuse);
            break;

        case LEAD_READ_EFUSE:       //获取烧录的efuse值
            err = x5_rf_read_efuse(&lead.get_efuse);
            break;

        case LEAD_RESET:            //复位芯片
            rf_reset_chip();
            break;

        case LEAD_GET_RES:          //阻抗测量
            err = x5_rf_get_res(lead.set_res_ch, &lead.get_res[0], 1);
            break;

        case LEAD_GET_I_STIM:       //测量刺激电流
            err = x5_rf_get_current(&lead.get_i_p_stim, &lead.get_i_n_stim);
            break;

        case LEAD_OUT_PULSE_SIMPLE: //输出简单波形
            err = x5_rf_config_simple(&lead.x5_pulse);
            break;

        case LEAD_MORO_SIMPLE:      //输出多程序组
            err = x5_rf_config_more_simple(&lead.x5_pulse);
            break;

        case LEAD_OUT_PULSE_BURST:  //输出爆发刺激波形
            err = x5_rf_config_burst(&lead.x5_pulse);
            break;

        case LEAD_STOP_PULSE:       //停止刺激
            logInfo("LEAD_STOP_PULSE");
            st25r3916_open_rf(CLOSE);
            break;
        case LEAD_WRITE_REG:       //写寄存器值
            x5_cmd_write(lead.reg.reg, lead.reg.data);
            break;
        case LEAD_READ_REG:       //写寄存器值
            x5_cmd_read(lead.reg.reg, &lead.reg.data);
            break;
        default:
            break;
    }
    return err;
}
#endif

int leadinno_crtl(leadinno_ctl_func_e func)
{
    int err = 0;

    lead.this.dev_name = DEV_NO_FIND;
    #ifdef ENABLE_LEADNNINO_X5
        if(x5_rf_send_query() == 0) {
            lead.this.dev_name = DEV_X5;
            logInfo("dev is DEV_X5");
        } else
    #endif
    #ifdef ENABLE_LEADNNINO_X4
        if(x4_rf_send_query() == 0) {
            lead.this.dev_name = DEV_X4;
            logInfo("dev is DEV_X4");
        } else
    #endif
        {
            lead.this.dev_name = DEV_NO_FIND;
            return -1;
        }

    #ifdef ENABLE_LEADNNINO_X4
        if(lead.this.dev_name == DEV_X4) {
            err = leadinno_x4_crtl(func);
        }
    #endif

    #ifdef ENABLE_LEADNNINO_X5
        if(lead.this.dev_name == DEV_X5) {
            err = leadinno_x5_crtl(func);
        }
    #endif
        return err;
}
