#include "hal_data.h"
#include "debug_uart.h"
#include "bsp_qspi_flash.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


#define OK 0
#define notOK 1
int Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);

#define Delay(a)    R_BSP_SoftwareDelay(a,BSP_DELAY_UNITS_MILLISECONDS)
#define  BufferSize (countof(Tx_Buffer)-1)


/* ���ͻ�������ʼ�� */
uint8_t Tx_Buffer[] = "��л��ѡ������RA6M5������";
uint8_t Rx_Buffer[sizeof(Tx_Buffer)];

uint32_t FlashDeviceID = 0;
uint32_t FlashID = 0;

#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress


/*
 * ��������Buffercmp
 * ����  ���Ƚ������������е������Ƿ����
 * ����  ��-pBuffer1     src������ָ��
 *         -pBuffer2     dst������ָ��
 *         -BufferLength ����������
 * ���  ����
 * ����  ��-OK pBuffer1 ����   pBuffer2
 *         -notOK pBuffer1 ��ͬ�� pBuffer2
 */
int Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer1 != *pBuffer2)
        {
            return notOK;
        }

        pBuffer1++;
        pBuffer2++;
    }
    return OK;
}


void hal_entry(void)
{
    QSPI_FLASH_Init();
    Debug_UART_Init();
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);


    /* ��ȡ Flash Device ID */
    FlashDeviceID = QSPI_FLASH_ReadDeviceID();
    Delay(200);

    /* ��ȡ SPI Flash ID */
    FlashID = QSPI_FLASH_ReadID();
    printf("\r\nFlashID is 0x%X,Manufacturer Device ID is 0x%X\r\n", FlashID, FlashDeviceID);

    if (FlashID == sFLASH_ID)
    {
        printf("��⵽����flash W25Q64 !\r\n");

        /* ������Ҫд��� SPI FLASH ������FLASHд��ǰҪ�Ȳ��� */
        // �������4K����һ����������������С��λ������
        QSPI_FLASH_SectorErase(FLASH_SectorToErase);

        /* �����ͻ�����������д��flash�� */
        // ����дһҳ��һҳ�Ĵ�СΪ256���ֽ�
        QSPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, sizeof(Tx_Buffer));
        printf("д�������Ϊ��%s \r\n", Tx_Buffer);

        /* ���ո�д������ݶ������ŵ����ջ������� */
        QSPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, sizeof(Tx_Buffer));
        printf("����������Ϊ��%s \r\n", Rx_Buffer);

        int a;
        a = Buffercmp(Tx_Buffer, Rx_Buffer, sizeof(Tx_Buffer));

        if (a == OK)
        {
            printf("\r\n32M����flash���Գɹ�!\n\r");
        }
        else
        {
            printf("\r\n32M����flash����ʧ��!\n\r");
        }

    }
    else
    {
        printf("��ȡ����ID!\n\r");
    }


    while (1);

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
        R_IOPORT_Open(&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable()
{

}
#endif
