#include "hal_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    /* TODO: add your own code here */

    /* 取消写保护 */
    R_PMISC->PWPR = 0;                               ///< Clear BOWI bit - writing to PFSWE bit enabled
    R_PMISC->PWPR = 1U << BSP_IO_PWPR_PFSWE_OFFSET;  ///< Set PFSWE bit - writing to PFS register enabled

    /* LED1：配置引脚 P400 对应的PFS寄存器 */
    R_PFS->PORT[BSP_IO_PORT_04_PIN_00>>8].PIN[BSP_IO_PORT_04_PIN_00 & 0xFF].PmnPFS =
        IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_LOW;
    /* LED2：配置引脚 P403 对应的PFS寄存器 */
    R_PFS->PORT[BSP_IO_PORT_04_PIN_03>>8].PIN[BSP_IO_PORT_04_PIN_03 & 0xFF].PmnPFS =
        IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_LOW;
    /* LED3：配置引脚 P404 对应的PFS寄存器 */
    R_PFS->PORT[BSP_IO_PORT_04_PIN_04>>8].PIN[BSP_IO_PORT_04_PIN_04 & 0xFF].PmnPFS =
        IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_LOW;

    /** 此时3个LED灯的引脚默认输出的是低电平
     *  所以3个LED灯都会默认亮起来
     *  我们在 while 循环里让 LED1 闪烁：每秒钟翻转一次状态
     */

    while(1)
    {
        /* 翻转LED灯：LED1 */
        //R_PORT4->PODR |= 1<<(BSP_IO_PORT_04_PIN_00 & 0xFF);
        //R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
        //R_PORT4->PODR &= (uint16_t)~(1 << (BSP_IO_PORT_04_PIN_00 & 0xFF));
        //R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);

        /* 或者也可以这样用位异或操作来翻转LED1 */
        R_PORT4->PODR ^= 1<<(BSP_IO_PORT_04_PIN_00 & 0xFF);
        R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
    }


    //这后面的代码无需理会
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
