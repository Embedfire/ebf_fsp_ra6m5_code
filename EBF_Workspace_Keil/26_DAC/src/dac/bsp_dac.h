#ifndef _BSP_DAC_H_
#define _BSP_DAC_H_

#include "hal_data.h"

void dac_Init();
void dac_Set(float voltage);
void dac_Circle(uint32_t timer);

#endif
