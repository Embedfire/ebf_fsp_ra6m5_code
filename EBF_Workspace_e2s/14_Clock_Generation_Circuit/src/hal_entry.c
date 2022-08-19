#include "hal_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

R_SYSTEM_Type* my_R_SYSTEM = R_SYSTEM;

void Delay(__IO uint32_t nCount);

//  软件延时函数，使用不同的系统时钟，延时不一样
void Delay(__IO uint32_t nCount)
{
    for(; nCount != 0; nCount--);
}

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    /* TODO: add your own code here */

    while (1) {
        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_06, BSP_IO_LEVEL_HIGH); //评估板
        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_07, BSP_IO_LEVEL_HIGH);
        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_08, BSP_IO_LEVEL_HIGH);

//        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_01, BSP_IO_LEVEL_HIGH); //启明
//        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_02, BSP_IO_LEVEL_HIGH);
//        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_04, BSP_IO_LEVEL_HIGH);

        Delay(0x0FFFFFF);
        //R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);

        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_06, BSP_IO_LEVEL_LOW); //评估板
        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_07, BSP_IO_LEVEL_LOW);
        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_00_PIN_08, BSP_IO_LEVEL_LOW);

//        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_01, BSP_IO_LEVEL_LOW); //启明
//        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_02, BSP_IO_LEVEL_LOW);
//        g_ioport.p_api->pinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_04, BSP_IO_LEVEL_LOW);

        Delay(0x0FFFFFF);
        //R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
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
