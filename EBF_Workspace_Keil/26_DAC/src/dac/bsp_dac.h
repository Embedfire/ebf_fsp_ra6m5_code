#ifndef _BSP_DAC_H_
#define _BSP_DAC_H_

#include "hal_data.h"

void DAC_Init();
void DAC_SetVoltage(float voltage);
void DAC_SinWave_Cycle(uint32_t time_interval);

#endif
