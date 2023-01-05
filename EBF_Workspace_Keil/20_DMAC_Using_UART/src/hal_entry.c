#include "hal_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


/* �û�ͷ�ļ����� */
#include "led/bsp_led.h"
#include "debug_uart/bsp_debug_uart.h"
#include "dmac/bsp_dmac.h"


#define BUFFER_SIZE     (37)

uint8_t sci_tx_data[BUFFER_SIZE] = {"embedfire-Ұ�� www.embedfire.com\r\n"};
uint8_t sci_rx_data[BUFFER_SIZE];

extern volatile bool uart_send_complete_flag;
extern volatile uint8_t dmac_sci4_tx_flag;
extern volatile uint8_t dmac_sci4_rx_flag;


uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);


/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    /* TODO: add your own code here */

    LED_Init();         // LED ��ʼ��
    Debug_UART4_Init(); // SCI4 UART ���Դ��ڳ�ʼ��

    printf("\r\nʵ��2��DMAC+UART�����շ�\r\n");
    printf("- �����̵��� - ���ճɹ������ͳɹ�\r\n");
    printf("- ��ɫ���� - ����ʧ�ܡ����ͳɹ�\r\n");
    printf("- ��ɫ���� - ���ճɹ�������ʧ��\r\n");
    printf("- ��ɫ���� - ����ʧ�ܡ�����ʧ��\r\n");
    printf("\r\n�����������£�\r\n");


    /* ͨ��CPU���жϴ������������� */
    R_SCI_UART_Write(&g_uart4_ctrl, &sci_tx_data[0], BUFFER_SIZE);

    /* �ȴ���������ж� - ��־λ�� UART �Ļص������� debug_uart4_callback() */
    while( false == uart_send_complete_flag );
    uart_send_complete_flag = false;

    /* ͨ�� DMAC ���жϴ������������� */

    /* ���� ICU IELSR �Ĵ��� */
    R_ICU->IELSR[SCI4_RXI_IRQn] = 0U;
    R_ICU->IELSR[SCI4_TXI_IRQn] = 0U;

    /* ���ô��ڽ���DMA Դ��ַ��Ŀ���ַ������ */
    set_transfer_dst_src_address(g_transfer_dmac_sci4_rx.p_cfg,
                                 &R_SCI4->RDR,
                                 &sci_rx_data[0]);
    set_transfer_length(g_transfer_dmac_sci4_rx.p_cfg, BUFFER_SIZE);

    /* ���ô��ڷ���DMA Դ��ַ��Ŀ���ַ������ */
    set_transfer_dst_src_address(g_transfer_dmac_sci4_tx.p_cfg,
                                 &sci_tx_data[0],
                                 (void*)&R_SCI4->TDR);
    set_transfer_length(g_transfer_dmac_sci4_tx.p_cfg, BUFFER_SIZE);

    /* ����DMAC */
    DMAC_Init();

    /* �ֶ������������ݼĴ������ж� */
    R_SCI4->SCR_b.TE = 0;
    R_SCI4->SCR_b.RE = 0;
    R_SCI4->SCR |= (0xF0);

//    R_SCI4->SCR_b.TE = 0;
//    R_SCI4->SCR_b.TIE = 0;
//    uint8_t temp = (uint8_t)(R_SCI4->SCR & 0x53);  //0x53 = 101 0011
//    R_SCI4->SCR = (uint8_t)(0xa0 | temp);  //0x0a = 1010 0000


    while(1)
    {
        if(( 1 == dmac_sci4_tx_flag ) && ( 1 == dmac_sci4_rx_flag ))
        {
            //�����̵��� - ���ճɹ������ͳɹ�
            LED1_OFF;
            LED2_ON;
            LED3_ON;
        }
        else if(( 1 == dmac_sci4_tx_flag ) && ( 0 == dmac_sci4_rx_flag ))
        {
            //��ɫ���� - ����ʧ�ܡ����ͳɹ�
            LED1_OFF;
            LED2_ON;
            LED3_OFF;
        }
        else if(( 0 == dmac_sci4_tx_flag ) && ( 1 == dmac_sci4_rx_flag ))
        {
            //��ɫ���� - ���ճɹ�������ʧ��
            LED1_OFF;
            LED2_OFF;
            LED3_ON;
        }
        else
        {
            //��ɫ���� - ����ʧ�ܡ�����ʧ��
            LED1_ON;
            LED2_OFF;
            LED3_OFF;
        }
    }


#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}


/**
  * �ж�ָ�����ȵ���������Դ�Ƿ���ȫ��ȣ�
  * �����ȫ��ȷ���1��ֻҪ����һ�����ݲ���ȷ���0
  */
uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
    /* ���ݳ��ȵݼ� */
    while(BufferLength--)
    {
        /* �ж���������Դ�Ƿ��Ӧ��� */
        if(*pBuffer != *pBuffer1)
        {
            /* ��Ӧ����Դ����������˳�������������0 */
            return 0;
        }
        /* ������������Դ�ĵ�ַָ�� */
        pBuffer++;
        pBuffer1++;
    }
    /* ����жϲ��Ҷ�Ӧ������� */
    return 1;
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