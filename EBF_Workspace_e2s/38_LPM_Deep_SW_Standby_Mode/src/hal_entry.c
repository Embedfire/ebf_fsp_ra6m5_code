#include "hal_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

#include "led/bsp_led.h"
#include "debug_uart/bsp_debug_uart.h"
#include "key/key.h"

extern bool key_flag; //声明标志位

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    /* TODO: add your own code here */
    Debug_UART4_Init(); //初始化debug串口
    IRQ_Init(); //IRQ中断初始化
    R_BSP_PinAccessEnable(); //启用对PFS寄存器的访问,因为后面写IO口都用BSP内联函数

    if (1U == R_SYSTEM->RSTSR0_b.DPSRSTF)
    {
         /*从深度软件待机模式唤醒后清除IOkeep位*/
         R_LPM_IoKeepClear (NULL);

         /*清除唤醒标志位*/
         R_SYSTEM->RSTSR0_b.DPSRSTF = 0;

         /*判断唤醒源是IRQ-9还是IRQ-10，并将判断结果打印出来*/
         if (R_SYSTEM->DPSIFR1 >> 1 & 0x01U)
         {
                 printf("MCU is woke up by IRQ-9\r\n");
         }
         else if (R_SYSTEM->DPSIFR1 >> 2 & 0x01U)
         {
                 printf("MCU is woke up by IRQ-10\r\n");
         }
    }
    LED_Flicker(10);//LED1闪烁10次

    while (1)
    {
             if (key_flag == true)
             {
                     /*标志位置0，防止程序不断进入if的代码块里面*/
                     key_flag = false;

                     /*LED闪烁2次*/
                     LED_Flicker (2);

                     /*进入低功耗模式前打印*/
                     printf("MCU enters Deep SW Standby mode\r\n");

                     /*打开LPM,进入深度软件待机模式*/
                     R_LPM_Open (Deep_SW_Standby.p_ctrl, Deep_SW_Standby.p_cfg);
                     R_LPM_LowPowerModeEnter (Deep_SW_Standby.p_ctrl);

                     /*唤醒后打印，但是在进入深度软件待机模式后不能保存上下文，
                     故这里正常来说无法被执行,如果执行了则说明进入的是睡眠模式，
                     一般刚烧写程序会导致这个问题，重新上电即可恢复正常*/
                     printf("MCU has been woken up without reset\r\n");
             }
    }
	
#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart (bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open(&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif
