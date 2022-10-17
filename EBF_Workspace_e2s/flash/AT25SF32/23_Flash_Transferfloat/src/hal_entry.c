#include "hal_data.h"
#include "led/bsp_led.h"
#include "qspi_flash/bsp_qspi_flash.h"
#include "debug_uart/bsp_debug_uart.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


#define OK 0
#define notOK 1
int Buffercmp(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t BufferLength);

#define Delay(a)    R_BSP_SoftwareDelay(a,BSP_DELAY_UNITS_MILLISECONDS)
#define  BufferSize (countof(Tx_Buffer)-1)


/* 发送缓冲区初始化 */
uint8_t Tx_Buffer[] = "感谢您选用瑞萨RA6M5开发板";
uint8_t Rx_Buffer[sizeof(Tx_Buffer)];

uint32_t FlashDeviceID = 0;
uint32_t FlashID = 0;

#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress


/*
 * 函数名：Buffercmp
 * 描述  ：比较两个缓冲区中的数据是否相等
 * 输入  ：-pBuffer1     src缓冲区指针
 *         -pBuffer2     dst缓冲区指针
 *         -BufferLength 缓冲区长度
 * 输出  ：无
 * 返回  ：-OK pBuffer1 等于   pBuffer2
 *         -notOK pBuffer1 不同于 pBuffer2
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

    /*存储小数和整数的数组，各7个*/
    long double double_buffer[7] = {0};
    int int_bufffer[7] = {0};

    QSPI_FLASH_Init();
    Debug_UART4_Init();
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);


    /* 获取 Flash Device ID */
    FlashDeviceID = QSPI_FLASH_ReadDeviceID();
    Delay(200);

    /* 获取 SPI Flash ID */
    FlashID = QSPI_FLASH_ReadID();
    printf("\r\nFlashID is 0x%X,Manufacturer Device ID is 0x%X\r\n", FlashID, FlashDeviceID);

    /* 检验 SPI Flash ID */
    if (FlashID == sFLASH_ID)
    {

        printf("\r\n检测到SPI FLASH W25Q64 !\r\n");

        /*读取数据标志位*/
        QSPI_FLASH_BufferRead(&cal_flag, SPI_FLASH_PageSize * 0, 1);

        if (cal_flag == 0xCD)    /*若标志等于0xcd，表示之前已有写入数据*/
        {
            printf("\r\n检测到数据标志\r\n");

            /*读取小数数据*/
            QSPI_FLASH_BufferRead((void *)double_buffer, SPI_FLASH_PageSize * 1, sizeof(double_buffer));

            /*读取整数数据*/
            QSPI_FLASH_BufferRead((void *)int_bufffer, SPI_FLASH_PageSize * 2, sizeof(int_bufffer));


            printf("\r\n从芯片读到数据：\r\n");
            for (k = 0; k < 7; k++)
            {
                printf("小数 rx = %LF \r\n", double_buffer[k]);
                printf("整数 rx = %d \r\n", int_bufffer[k]);
            }
						QSPI_FLASH_SectorErase(0);
        }
        else
        {
            printf("\r\n没有检测到数据标志，FLASH没有存储数据，即将进行小数写入实验\r\n");
            cal_flag = 0xCD;
            /*擦除扇区*/
            QSPI_FLASH_SectorErase(0);

            /*写入标志到第0页*/
            QSPI_FLASH_BufferWrite(&cal_flag, SPI_FLASH_PageSize * 0, 1);

            /*生成要写入的数据*/
            for (k = 0; k < 7; k++)
            {
                double_buffer[k] = k + 0.1;
                int_bufffer[k] = k * 500 + 1 ;
            }

            /*写入小数数据到第一页*/
            QSPI_FLASH_BufferWrite((void *)double_buffer, SPI_FLASH_PageSize * 1, sizeof(double_buffer));
            /*写入整数数据到第二页*/
            QSPI_FLASH_BufferWrite((void *)int_bufffer, SPI_FLASH_PageSize * 2, sizeof(int_bufffer));

            printf("向芯片写入数据：");
            /*打印到串口*/
            for (k = 0; k < 7; k++)
            {
                printf("小数tx = %LF\r\n", double_buffer[k]);
                printf("整数tx = %d\r\n", int_bufffer[k]);
            }

            printf("\r\n请复位开发板，以读取数据进行检验\r\n");
        }

    }
    else
    {
        printf("获取不到ID!\n\r");
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
