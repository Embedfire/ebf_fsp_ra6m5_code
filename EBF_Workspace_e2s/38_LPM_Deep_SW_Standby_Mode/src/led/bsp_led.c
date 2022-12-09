#include "bsp_led.h"
#include "debug_uart/bsp_debug_uart.h"


/* IO初始化函数 */
void IO_Init(void)
{
    R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
}

void LED_Flicker(uint8_t num)
{
   /*LED灯闪烁*/
	for(int i = 0;i <= num*2;i++)
	{
		LED1_TOGGLE;
		R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);
	}
}
