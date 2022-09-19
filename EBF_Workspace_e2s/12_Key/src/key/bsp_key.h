#ifndef __BSP_KEY_H
#define __BSP_KEY_H
#include "hal_data.h"

#define Key_1 (BSP_IO_PORT_00_PIN_04)
#define Key_2 (BSP_IO_PORT_00_PIN_05)

/* LED初始化函数 */
void KEY_Init(void);
uint32_t Key_read(bsp_io_port_pin_t key);
void Key_SW2_test(void);
void Key_SW3_test(void);

#endif
