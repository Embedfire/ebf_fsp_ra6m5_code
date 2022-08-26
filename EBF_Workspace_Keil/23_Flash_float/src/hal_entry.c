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
    uint8_t cal_flag = 0;
    uint8_t k;

    /*�洢С�������������飬��7��*/
    long double double_buffer[7] = {0};
    int int_bufffer[7] = {0};

    QSPI_FLASH_Init();
    Debug_UART_Init();
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);


    /* ��ȡ Flash Device ID */
    FlashDeviceID = SPI_FLASH_ReadDeviceID();
    Delay(200);

    /* ��ȡ SPI Flash ID */
    FlashID = QSPI_FLASH_ReadID();
    printf("\r\nFlashID is 0x%X,Manufacturer Device ID is 0x%X\r\n", FlashID, FlashDeviceID);

    /* ���� SPI Flash ID */
    if (FlashID == sFLASH_ID)
    {

        printf("\r\n��⵽SPI FLASH AT25SF321B !\r\n");

        /*��ȡ���ݱ�־λ*/
        SPI_FLASH_BufferRead(&cal_flag, SPI_FLASH_PageSize * 0, 1);

        if (cal_flag == 0xCD)    /*����־����0xcd����ʾ֮ǰ����д������*/
        {
            printf("\r\n��⵽���ݱ�־\r\n");

            /*��ȡС������*/
            SPI_FLASH_BufferRead((void *)double_buffer, SPI_FLASH_PageSize * 1, sizeof(double_buffer));

            /*��ȡ��������*/
            SPI_FLASH_BufferRead((void *)int_bufffer, SPI_FLASH_PageSize * 2, sizeof(int_bufffer));


            printf("\r\n��оƬ�������ݣ�\r\n");
            for (k = 0; k < 7; k++)
            {
                printf("С�� rx = %LF \r\n", double_buffer[k]);
                printf("���� rx = %d \r\n", int_bufffer[k]);
            }

            SPI_FLASH_SectorErase(0);
        }
        else
        {
            printf("\r\nû�м�⵽���ݱ�־��FLASHû�д洢���ݣ���������С��д��ʵ��\r\n");
            cal_flag = 0xCD;
            /*��������*/
            SPI_FLASH_SectorErase(0);

            /*д���־����0ҳ*/
            SPI_FLASH_BufferWrite(&cal_flag, SPI_FLASH_PageSize * 0, 1);

            /*����Ҫд�������*/
            for (k = 0; k < 7; k++)
            {
                double_buffer[k] = k + 0.12;
                int_bufffer[k] = k * 500 + 1 ;
            }

            /*д��С�����ݵ���һҳ*/
            SPI_FLASH_BufferWrite((void *)double_buffer, SPI_FLASH_PageSize * 1, sizeof(double_buffer));
            /*д���������ݵ��ڶ�ҳ*/
            SPI_FLASH_BufferWrite((void *)int_bufffer, SPI_FLASH_PageSize * 2, sizeof(int_bufffer));

            printf("��оƬд�����ݣ�");
            /*��ӡ������*/
            for (k = 0; k < 7; k++)
            {
                printf("С��tx = %LF\r\n", double_buffer[k]);
                printf("����tx = %d\r\n", int_bufffer[k]);
            }

            printf("\r\n�븴λ�����壬�Զ�ȡ���ݽ��м���\r\n");
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
