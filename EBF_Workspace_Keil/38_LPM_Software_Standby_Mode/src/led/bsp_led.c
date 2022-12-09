#include "bsp_led.h"
#include "bsp_debug_uart.h"


/* LED初始化函数 */
void IO_Init(void)
{
    R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
}

void LED_Task(void)
{
   /*流水灯任务*/
   LED1_ON;
   R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
   LED2_ON;
   R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
   LED3_ON;
   R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);

   LED1_OFF;
   R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
   LED2_OFF;
   R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
   LED3_OFF;
   /*睡眠前打印*/
   printf("MCU enters sleep mode\r\n");
   /*执行完流水灯任务，进入睡眠模式*/
   R_LPM_LowPowerModeEnter(Standby.p_ctrl);
   /*被唤醒后打印*/
   printf("MCU has been woken up\r\n");
}