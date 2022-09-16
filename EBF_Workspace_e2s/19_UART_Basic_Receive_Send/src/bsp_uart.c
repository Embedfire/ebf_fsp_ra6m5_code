/**
 ******************************************************************************
 * @file    bsp_uart.c
 * @author  fire
 * @version V1.0
 * @date    2022-7-15
 * @brief   串口初始化函数，串口中断回调函数和重定向printf函数
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

#include "bsp_uart.h"

//使用e2studio则需要此全局变量，自定义的print需要使用到
#if defined __GNUC__ && !defined __clang__
uint8_t debug_str[DEBUG_STR_LEN];
#endif

static volatile uint8_t uart_send_complete_flag = false; //必须加上volatile，否则可能被编译器优化


/**
 * @brief 串口初始化并开始接收
 * @param 无
 * @return 无
 */
void UART_Init(void)
{
    R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg); //打开串口
}

void UART_Callback(uart_callback_args_t * p_args)
{
    switch(p_args->event)
    {
        case UART_EVENT_RX_CHAR:
            R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *)&p_args->data, 1);
            break;
        case UART_EVENT_TX_COMPLETE:
            uart_send_complete_flag = true;
            break;
        default:
            break;
    }
}


/**
 * @brief 因为直接使用printf需要修改C开发环境，且需要分配堆，较为麻烦且移植不便，故这里写了个自定义的printf
 * @param str 字符串
 * @param bytes 字符串长度
 * @return 无
 */
void UART_print(uint8_t *str, int bytes)
{
    uart_send_complete_flag = false;
    R_SCI_UART_Write(&g_uart0_ctrl, str, (uint32_t)bytes);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;
}

//重定向printf输出
 #if defined __GNUC__ && !defined __clang__
 int _write(int fd, char *pBuffer, int size)
 {
    (void)fd;
    uart_send_complete_flag = false;
    R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *)pBuffer, (uint32_t)size);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;
    return size;
 }
 #else
 int fputc(int ch, FILE *f)
 {
    (void)f;
    uart_send_complete_flag = false;
    R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *)&ch, 1);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;
    return ch;
 }
 #endif
