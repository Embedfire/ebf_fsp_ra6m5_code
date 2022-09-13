#ifndef BSP_GPT_H_
#define BSP_GPT_H_

#include "hal_data.h"

#define LED_R_PWM GPT_IO_PIN_GTIOCA

void GPT_Init(void);
void GPT_PWM_Duty_Cycle_Set(timer_ctrl_t * const GPT_p_ctrl, uint32_t duty_cycle);


#endif
