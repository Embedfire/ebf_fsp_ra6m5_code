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
    QSPI_FLASH_Init();
    Debug_UART4_Init();
    R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);


    /* 获取 Flash Device ID */
    FlashDeviceID = QSPI_FLASH_ReadDeviceID();
    Delay(200);

    /* 获取 SPI Flash ID */
    FlashID = QSPI_FLASH_ReadID();
    printf("\r\nFlashID is 0x%X,Manufacturer Device ID is 0x%X\r\n", FlashID, FlashDeviceID);


    if (FlashID == sFLASH_ID)
    {
        printf("检测到串行flash W25Q64 !\r\n");

        /* 擦除将要写入的 SPI FLASH 扇区，FLASH写入前要先擦除 */
        // 这里擦除4K，即一个扇区，擦除的最小单位是扇区
        QSPI_FLASH_SectorErase(FLASH_SectorToErase);

        /* 将发送缓冲区的数据写到flash中 */
        // 这里写一页，一页的大小为256个字节
        QSPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, sizeof(Tx_Buffer));
        printf("写入的数据为：%s \r\n", Tx_Buffer);

        /* 将刚刚写入的数据读出来放到接收缓冲区中 */
        QSPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, sizeof(Tx_Buffer));
        printf("读出的数据为：%s \r\n", Rx_Buffer);

        int a;
        a = Buffercmp(Tx_Buffer, Rx_Buffer, sizeof(Tx_Buffer));

        if (a == OK)
        {
            printf("\r\n32M串行flash测试成功!\n\r");
            LED3_ON;
        }
        else
        {
            printf("\r\n32M串行flash测试失败!\n\r");
            LED1_ON;
        }

    }
    else
    {
        LED1_ON;
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
