#include "hal_data.h"
#include "oled.h"
#include "oledfont.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


spi_event_t g_transfer_complete;
/* Callback function */
void spi_callback(spi_callback_args_t *p_args)
{
    if (SPI_EVENT_TRANSFER_COMPLETE == p_args->event)
    {
        g_transfer_complete = true;
    }
}



void hal_entry(void)
{
    R_SPI_Open(&g_spi0_ctrl, &g_spi0_cfg);
    R_BSP_SoftwareDelay(200,BSP_DELAY_UNITS_MICROSECONDS);
    OLED_Init();
    OLED_Fill(0x00);
    while(1)
    {
        while(SPI_EVENT_TRANSFER_COMPLETE != 1);
        OLED_CLS();
        for(unsigned int i=0; i<4; i++)//通过点阵显示汉字 -- i表示子表中汉字的位置
        {
            OLED_16x16CN(i*16+32,0,i);
        }
        OLED_WrCmd(0xaf);

        R_BSP_SoftwareDelay(2000,BSP_DELAY_UNITS_MILLISECONDS);
        OLED_CLS();
        OLED_BMP(0,0,128,8,(unsigned char *)BMP1);

        R_BSP_SoftwareDelay(2000,BSP_DELAY_UNITS_MILLISECONDS);

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
