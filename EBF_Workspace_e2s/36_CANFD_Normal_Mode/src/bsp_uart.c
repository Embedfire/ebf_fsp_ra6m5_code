/**
  ******************************************************************************
  * @file    bsp_uart.c
  * @author  fire
  * @version V1.0
  * @date    2022-07-xx
  * @brief   串口处理文件
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火_瑞萨启明6M5开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "bsp_uart.h"


/*
 * UART API 接口
 *
 * const uart_api_t g_uart_on_sci =
 * {
 *    .open               = R_SCI_UART_Open,
 *     .close              = R_SCI_UART_Close,
 *     .write              = R_SCI_UART_Write,
 *     .read               = R_SCI_UART_Read,
 *     .infoGet            = R_SCI_UART_InfoGet,
 *     .baudSet            = R_SCI_UART_BaudSet,
 *     .communicationAbort = R_SCI_UART_Abort,
 *     .callbackSet        = R_SCI_UART_CallbackSet,
 *     .readStop           = R_SCI_UART_ReadStop,
 * };
 */


/* 开启串口4 */
void bsp_uart4_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = g_uart_on_sci.open(g_uart4.p_ctrl, g_uart4.p_cfg); //开启串口4
    assert(FSP_SUCCESS == err);
}



/* 串口4的回调函数 */
volatile uint8_t uart_send_complete_flag = 0;
volatile bool uart_receive_complete_flag = false;
void g_uart4_callback(uart_callback_args_t *p_args)
{
    if (p_args->channel == 4)
    {
        switch ((uint32_t)p_args->event) {
        case UART_EVENT_RX_COMPLETE:  // 接收完成事件
        {
            uart_receive_complete_flag = true;
            break;
        }
        case UART_EVENT_TX_COMPLETE:  // 发送完成事件
        {
            uart_send_complete_flag = 1;  //printf重定向用
            break;
        }
        case UART_EVENT_RX_CHAR:  ///< Character received 接收到的字符
        {
            break;
        }
        case UART_EVENT_TX_DATA_EMPTY:  ///< Last byte is transmitting, ready for more data
        {
            break;
        }
        }
    }
}


