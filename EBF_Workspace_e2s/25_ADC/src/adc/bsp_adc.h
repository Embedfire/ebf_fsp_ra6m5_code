#ifndef _BSP_ADC_H_
#define _BSP_ADC_H_

#include "hal_data.h"

void adc_Init(void);
double adc_read(void);
void adc_callback(adc_callback_args_t * p_args);

#endif
