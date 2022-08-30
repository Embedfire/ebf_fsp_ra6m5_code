/**
  ******************************************************************************
  * @file    hal_entry.c
  * @author  fire
  * @version V1.0
  * @date    2022-07-xx
  * @brief
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火_瑞萨启明6M5开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "hal_data.h"
#include "retarget.h"
#include "bsp_uart.h"



FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

uart_info_t my_uart_info;
uint8_t sci_tx_data[] = {"embedfire-野火 www.embedfire.com\r\n"};

void led_task(uint16_t Led_state);  //LED 闪烁任务


/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    /* TODO: add your own code here */

    fsp_pack_version_t version = {0};
    fsp_err_t err = FSP_SUCCESS;
    uint16_t _temp = 0;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    bsp_uart4_init();  //开启串口4

    err = g_transfer_on_dtc.enable(g_uart4.p_cfg->p_transfer_rx->p_ctrl);
    assert(FSP_SUCCESS == err);

    while (1) {
        //printf("embedfire-野火 www.embedfire.com\r\n");

        // 可以尝试在 configuration.xml 中将 DTC 模块去除，再进行代码调试，看看会有什么区别
        g_uart_on_sci.write(g_uart4.p_ctrl, (uint8_t *)(sci_tx_data), sizeof(sci_tx_data)/sizeof(sci_tx_data[0]));

        led_task(_temp++);  //LED 闪烁任务

        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
    }

#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}


void led_task(uint16_t Led_state) {

    if (Led_state % 2 == 1) {
        g_ioport_on_ioport.pinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_01, BSP_IO_LEVEL_HIGH);
        g_ioport_on_ioport.pinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_02, BSP_IO_LEVEL_HIGH);
        g_ioport_on_ioport.pinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_04, BSP_IO_LEVEL_HIGH);
    } else {
        g_ioport_on_ioport.pinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_01, BSP_IO_LEVEL_LOW);
        g_ioport_on_ioport.pinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_02, BSP_IO_LEVEL_LOW);
        g_ioport_on_ioport.pinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_04, BSP_IO_LEVEL_LOW);
    }
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