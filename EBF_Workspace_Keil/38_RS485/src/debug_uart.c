/**
 ******************************************************************************
 * @file    debug_uart.c
 * @author  fire
 * @version V1.0
 * @date    2022-7-25
 * @brief   debug串口初始化函数，串口中断回调函数和重定向printf函数
 ******************************************************************************
 * @attention
 *
 * 实验平台 : RA6M5
 * 公司     :http://www.embedfire.com
 * 论坛     :http://www.firebbs.cn
 * 淘宝     :http://firestm32.taobao.com
 *
 ******************************************************************************
 */

#include "debug_uart.h"

static volatile uint8_t uart_send_complete_flag = false; //必须加上volatile，否则可能被编译器优化


/**
 * @brief 串口初始化并开始接收
 * @param 无
 * @return 无
 */
void Debug_UART_Init(void)
{
    R_SCI_UART_Open (&Debug_UART_ctrl, &Debug_UART_cfg); //打开串口
}

void Debug_UART_Callback(uart_callback_args_t * p_args)
{
    switch(p_args->event)
    {
        case UART_EVENT_RX_CHAR:  //收到数据后发给RS485_1
            R_SCI_UART_Write(&RS485_1_ctrl, (uint8_t *)&p_args->data, 1);
            break;
        case UART_EVENT_TX_COMPLETE:
            uart_send_complete_flag = true;
            break;
        default:
            break;
    }
}


//重定向printf输出
 #if defined __GNUC__ && !defined __clang__
 int _write(int fd, char *pBuffer, int size)
 {
    (void)fd;
    R_SCI_UART_Write(&Debug_UART_ctrl, (uint8_t *)pBuffer, (uint32_t)size);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;
    return size;
 }
 #else
 int fputc(int ch, FILE *f)
 {
    (void)f;
    R_SCI_UART_Write(&Debug_UART_ctrl, (uint8_t *)&ch, 1);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;
    return ch;
 }
 #endif