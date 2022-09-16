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
#include "stdio.h"
#include "bsp_circular_queue.h"


volatile uint8_t uart_receiving = 0; //用于判断串口是否正在接收
static volatile bool uart_send_complete_flag = false; //用于判断printf是否发送完毕，为了防止编译器优化，建议加上volatile，其他flag同理

//使用e2studio则需要此全局变量，自定义的print需要使用到
#if defined __GNUC__ && !defined __clang__
uint8_t debug_str[DEBUG_STR_LEN];
#endif

void UART_Init(void)
{
    R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg); //打开串口
}

void UART_Callback(uart_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case UART_EVENT_RX_CHAR:
            /*接收到数据后马上写入队列中*/
            Queue_Wirte (&Circular_queue, (uint8_t*) &p_args->data, 1);
            /*将标志位置5，表示正在接收数据*/
            uart_receiving = 5;
            break;
        case UART_EVENT_TX_DATA_EMPTY:
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
    (void) fd;
    R_SCI_UART_Write (&g_uart0_ctrl, (uint8_t*) pBuffer, (uint32_t) size);
    while (uart_send_complete_flag == false)
        ;
    uart_send_complete_flag = false;
    return size;
}
#else
 int fputc(int ch, FILE *f)
 {
    (void)f;
    R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *)&ch, 1);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;
    return ch;
 }
 #endif
