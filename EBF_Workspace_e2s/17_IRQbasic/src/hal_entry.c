#include "hal_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER




bsp_io_level_t Pin_status = BSP_IO_LEVEL_LOW;                                           //状态结构体

void exit_back(external_irq_callback_args_t *p_args)                                    //当产生中断时会访问中断服务函数
{
    (void) p_args;
    if(Pin_status)
        R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_06, BSP_IO_LEVEL_HIGH);    //点亮LED
    else
        R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_06, BSP_IO_LEVEL_LOW);     //关闭LED
    Pin_status=~ Pin_status;                                                            //状态反转
}


void hal_entry(void)
{
    fsp_err_t err;
    err = R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
    assert(FSP_SUCCESS == err);
    err = R_ICU_ExternalIrqOpen(&g_external_irq10_ctrl, &g_external_irq10_cfg);
    assert(FSP_SUCCESS == err);
    err = R_ICU_ExternalIrqEnable(&g_external_irq10_ctrl);
    assert(FSP_SUCCESS == err);
    err = R_ICU_ExternalIrqOpen(&g_external_irq9_ctrl, &g_external_irq9_cfg);
    assert(FSP_SUCCESS == err);
    err = R_ICU_ExternalIrqEnable(&g_external_irq9_ctrl);
    assert(FSP_SUCCESS == err);

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
