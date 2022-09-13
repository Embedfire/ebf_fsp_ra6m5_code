#include "hal_data.h"
#include "bsp_debug_uart.h"
#include "bsp_key_irq.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


void LED_Task();
/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    /* TODO: add your own code here */
    Debug_UART_Init(); //初始化debug串口
    Key_Init(); //初始化按键
    R_BSP_PinAccessEnable(); //启用对PFS寄存器的访问
    R_LPM_Open(Snooze.p_ctrl, Snooze.p_cfg); //打开LPM
    R_GPT_Open(GPT_PWM.p_ctrl, GPT_PWM.p_cfg);  //打开GPT计时器
    R_GPT_Start(GPT_PWM.p_ctrl); //启动GPT计时器
    while(1)
    {
        LED_Task(); //led任务
    }
#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

void LED_Task()
{
    /*流水灯任务*/
    R_BSP_PinWrite(LED_B, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
    R_BSP_PinWrite(LED_G, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);

    R_BSP_PinWrite(LED_B, BSP_IO_LEVEL_HIGH);
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
    R_BSP_PinWrite(LED_G, BSP_IO_LEVEL_HIGH);

    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
    /*进入小睡模式前打印*/
    APP_PRINT("MCU enters SW standby mode\r\n");
    /*执行完流水灯任务，进入小睡模式*/
    R_LPM_LowPowerModeEnter(Snooze.p_ctrl);
    /*被唤醒后打印*/
    APP_PRINT("MCU has been woken up\r\n");
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
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
        R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif
