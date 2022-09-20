#include "hal_data.h"
#include "led/bsp_led.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

bsp_io_level_t button2_status = BSP_IO_LEVEL_LOW;                                           //状态结构体
bsp_io_level_t button3_status = BSP_IO_LEVEL_LOW;

void button1_callback(external_irq_callback_args_t *p_args)                                    //当产生中断时会访问中断服务函数
{
    (void) p_args;
    R_IOPORT_PinWrite(&g_ioport_ctrl, LED_Red, button2_status);    //点亮LED
    button2_status=~ button2_status;                                                            //状态反转
}

void button2_callback(external_irq_callback_args_t *p_args)                                    //当产生中断时会访问中断服务函数
{
    (void) p_args;
    R_IOPORT_PinWrite(&g_ioport_ctrl, LED_Bule, button3_status);    //点亮LED
    button3_status=~ button3_status;                                                            //状态反转
}

void hal_entry(void)
{
    R_ICU_ExternalIrqOpen(button1.p_ctrl, button1.p_cfg);
    R_ICU_ExternalIrqEnable(button1.p_ctrl);
    R_ICU_ExternalIrqOpen(button2.p_ctrl, button2.p_cfg);
    R_ICU_ExternalIrqEnable(button2.p_ctrl);

    while(1)
    {
        R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);
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
