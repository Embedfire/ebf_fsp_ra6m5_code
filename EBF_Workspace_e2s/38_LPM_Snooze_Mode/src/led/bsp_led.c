#include "bsp_led.h"
#include "debug_uart/bsp_debug_uart.h"


/* LED初始化函数 */
void IO_Init(void)
{
    R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
}

void GPT_Init(void)
{
    /*打开GPT，初始化寄存器配置*/
    R_GPT_Open (GPT_PWM.p_ctrl, GPT_PWM.p_cfg);
    /*启动GPT计时*/
    R_GPT_Start (GPT_PWM.p_ctrl);
}

void GPT_PWM_Duty_Cycle_Set(timer_ctrl_t * const GPT_p_ctrl, uint32_t duty_cycle)
{
    timer_info_t info;
    /*获得GPT的信息*/
    R_GPT_InfoGet (GPT_p_ctrl, &info);
    /*获得计时器一个周期需要的计数次数*/
    uint32_t current_period_counts = info.period_counts;
    /*根据占空比和一个周期的计数次数计算GTCCR的值*/
    uint32_t duty_cycle_counts = (uint32_t)(((uint64_t) current_period_counts * duty_cycle) / 100);
    /*设置GTCCR的值*/
    R_GPT_DutyCycleSet(GPT_p_ctrl, duty_cycle_counts, BSP_IO_PORT_04_PIN_00);
}

void GPT_LED1_PWM_ON_Snooze_Mode(void)
{
	  /*调整PWM占空比，实现呼吸灯效果*/
		for (uint32_t duty_cycle = 0; duty_cycle <= 90; duty_cycle++)
		{
				GPT_PWM_Duty_Cycle_Set (GPT_PWM.p_ctrl, duty_cycle);
				R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MILLISECONDS);
		}
}

void LED_Task(void)
{
   /*流水灯任务*/
   LED2_ON;
   R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
   LED3_ON;
   R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);

   LED2_OFF;
   R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
   LED3_OFF;
   /*睡眠前打印*/
   printf("MCU enters SW standby mode\r\n");
   /*执行完流水灯任务，进入睡眠模式*/
   R_LPM_LowPowerModeEnter(Snooze.p_ctrl);
   /*被唤醒后打印*/
   printf("MCU has been woken up\r\n");
}
