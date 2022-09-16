#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "hal_data.h"

/* LED引脚置低电平 LED灯亮 */
#define LED1_ON     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_00, BSP_IO_LEVEL_LOW)
#define LED2_ON     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_03, BSP_IO_LEVEL_LOW)
#define LED3_ON     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_04, BSP_IO_LEVEL_LOW)

/* LED引脚置高电平 LED灯灭 */
#define LED1_OFF    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_00, BSP_IO_LEVEL_HIGH)
#define LED2_OFF    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_03, BSP_IO_LEVEL_HIGH)
#define LED3_OFF    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_04_PIN_04, BSP_IO_LEVEL_HIGH)

/* LED初始化函数 */
void LED_Init(void);

#endif
