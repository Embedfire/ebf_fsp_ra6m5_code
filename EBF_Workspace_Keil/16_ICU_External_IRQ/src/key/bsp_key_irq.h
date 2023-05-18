#ifndef __BSP_KEY_EXTERNAL_IRQ_H
#define __BSP_KEY_EXTERNAL_IRQ_H
#include "hal_data.h"


/* 两个按键引脚定义 */
#define KEY1_SW2_PIN    BSP_IO_PORT_00_PIN_04
#define KEY2_SW3_PIN    BSP_IO_PORT_00_PIN_05

/* KEY 外部中断初始化函数 */
void Key_IRQ_Init(void);

#endif
