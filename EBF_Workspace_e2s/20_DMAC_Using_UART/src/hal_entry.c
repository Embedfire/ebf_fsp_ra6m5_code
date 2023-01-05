#include "hal_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


/* 用户头文件包含 */
#include "led/bsp_led.h"
#include "debug_uart/bsp_debug_uart.h"
#include "dmac/bsp_dmac.h"


#define BUFFER_SIZE     (37)

uint8_t sci_tx_data[BUFFER_SIZE] = {"embedfire-野火 www.embedfire.com\r\n"};
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

    LED_Init();         // LED 初始化
    Debug_UART4_Init(); // SCI4 UART 调试串口初始化

    printf("\r\n实验2：DMAC+UART串口收发\r\n");
    printf("- 蓝、绿灯亮 - 接收成功、发送成功\r\n");
    printf("- 蓝色灯亮 - 接收失败、发送成功\r\n");
    printf("- 绿色灯亮 - 接收成功、发送失败\r\n");
    printf("- 红色灯亮 - 接收失败、发送失败\r\n");
    printf("\r\n发送内容如下：\r\n");


    /* 通过CPU和中断处理程序发送数据 */
    R_SCI_UART_Write(&g_uart4_ctrl, &sci_tx_data[0], BUFFER_SIZE);

    /* 等待传输完成中断 - 标志位在 UART 的回调函数中 debug_uart4_callback() */
    while( false == uart_send_complete_flag );
    uart_send_complete_flag = false;

    /* 通过 DMAC 和中断处理程序发送数据 */

    /* 清零 ICU IELSR 寄存器 */
    R_ICU->IELSR[SCI4_RXI_IRQn] = 0U;
    R_ICU->IELSR[SCI4_TXI_IRQn] = 0U;

    /* 配置串口接收DMA 源地址、目标地址、长度 */
    set_transfer_dst_src_address(g_transfer_dmac_sci4_rx.p_cfg,
                                 &R_SCI4->RDR,
                                 &sci_rx_data[0]);
    set_transfer_length(g_transfer_dmac_sci4_rx.p_cfg, BUFFER_SIZE);

    /* 配置串口发送DMA 源地址、目标地址、长度 */
    set_transfer_dst_src_address(g_transfer_dmac_sci4_tx.p_cfg,
                                 &sci_tx_data[0],
                                 (void*)&R_SCI4->TDR);
    set_transfer_length(g_transfer_dmac_sci4_tx.p_cfg, BUFFER_SIZE);

    /* 开启DMAC */
    DMAC_Init();

    /* 手动触发传输数据寄存器空中断 */
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
            //蓝、绿灯亮 - 接收成功、发送成功
            LED1_OFF;
            LED2_ON;
            LED3_ON;
        }
        else if(( 1 == dmac_sci4_tx_flag ) && ( 0 == dmac_sci4_rx_flag ))
        {
            //蓝色灯亮 - 接收失败、发送成功
            LED1_OFF;
            LED2_ON;
            LED3_OFF;
        }
        else if(( 0 == dmac_sci4_tx_flag ) && ( 1 == dmac_sci4_rx_flag ))
        {
            //绿色灯亮 - 接收成功、发送失败
            LED1_OFF;
            LED2_OFF;
            LED3_ON;
        }
        else
        {
            //红色灯亮 - 接收失败、发送失败
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
  * 判断指定长度的两个数据源是否完全相等，
  * 如果完全相等返回1，只要其中一对数据不相等返回0
  */
uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
    /* 数据长度递减 */
    while(BufferLength--)
    {
        /* 判断两个数据源是否对应相等 */
        if(*pBuffer != *pBuffer1)
        {
            /* 对应数据源不相等马上退出函数，并返回0 */
            return 0;
        }
        /* 递增两个数据源的地址指针 */
        pBuffer++;
        pBuffer1++;
    }
    /* 完成判断并且对应数据相对 */
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