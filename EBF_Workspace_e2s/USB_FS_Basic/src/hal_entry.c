#include "hal_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER

/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
#define LINE_CODING_LENGTH       (0x07U)                // Line coding length
#define DATA_LEN                 (512U)                 // Data Length

static usb_pcdc_linecoding_t g_line_coding;
uint8_t g_buf[DATA_LEN] = {0};

void hal_entry(void)
{
    /* TODO: add your own code here */
    usb_event_info_t event_info;
    usb_status_t     event;
    g_usb_on_usb.open(&g_basic0_ctrl, &g_basic0_cfg);
    while (1)
    {
        /* Get USB event data */
        g_usb_on_usb.eventGet(&event_info, &event);
        /* Handle the received event (if any) */
        switch (event)
        {
            case USB_STATUS_CONFIGURED:
            case USB_STATUS_WRITE_COMPLETE:
                /* Initialization complete; get data from host */
                g_usb_on_usb.read(&g_basic0_ctrl, g_buf, DATA_LEN, USB_CLASS_PCDC);
                break;
            case USB_STATUS_READ_COMPLETE:
                /* Loop back received data to host */
                g_usb_on_usb.write(&g_basic0_ctrl, g_buf, event_info.data_size, USB_CLASS_PCDC);
                break;
            case USB_STATUS_REQUEST:   /* Receive Class Request */
                if (USB_PCDC_SET_LINE_CODING == (event_info.setup.request_type & USB_BREQUEST))
                {
                    /* Configure virtual UART settings */
                    g_usb_on_usb.periControlDataGet(&g_basic0_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);
                }
                else if (USB_PCDC_GET_LINE_CODING == (event_info.setup.request_type & USB_BREQUEST))
                {
                    /* Send virtual UART settings back to host */
                    g_usb_on_usb.periControlDataSet(&g_basic0_ctrl, (uint8_t *) &g_line_coding, LINE_CODING_LENGTH);
                }
                else
                {
                    /* ACK all other status requests */
                    g_usb_on_usb.periControlStatusSet(&g_basic0_ctrl, USB_SETUP_STATUS_ACK);
                }
                break;
            case USB_STATUS_SUSPEND:
            case USB_STATUS_DETACH:
                break;
            default:
                break;
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