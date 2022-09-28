#include "bsp_adc.h"
#include "led/bsp_led.h"

volatile bool scan_complete_flag = false;
void adc_callback(adc_callback_args_t * p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    scan_complete_flag = true;
}

void adc_Init(void)
{
    fsp_err_t err;
    err = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
    err = R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
    assert(FSP_SUCCESS == err);
}


double adc_read(void)
{
    uint16_t adc_data;
    double a0;

    (void)R_ADC_ScanStart(&g_adc0_ctrl);
    scan_complete_flag = false;
    while (!scan_complete_flag)
    {
        ;
    }

    R_ADC_Read(&g_adc0_ctrl, ADC_CHANNEL_0, &adc_data);
    a0 = (double)(adc_data*3.3/4095);
    return a0;
}
