/**
 ******************************************************************************
 * @file    bsp_debug_uart.c
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

#include <bsp_debug_uart.h>

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
void Debug_UART_Init(void)
{
    R_SCI_UART_Open (&Debug_UART_ctrl, &Debug_UART_cfg); //打开串口
}

/**
 * @brief 串口中断回调函数
 * @param p_args
 */
void Debug_UART_Callback(uart_callback_args_t * p_args)
{
    switch(p_args->event)
    {
        case UART_EVENT_RX_CHAR:  //收到数据后发给上位机
            R_SCI_UART_Write(Debug_UART.p_ctrl, (uint8_t *)&p_args->data, 1);
            break;
        case UART_EVENT_TX_COMPLETE:
            /*发送完成后设置此标志位置1，即可开始下一次printf打印*/
            uart_send_complete_flag = true;
            break;
        default:
            break;
    }
}

/**
 * @brief 因为直接使用printf需要修改C开发环境，需要分配堆，且需要/n或者手动fflush或者等缓冲区满了才会打印，不方便移植，故这里写了个自定义的print
 * @param str 字符串
 * @param bytes 字符串长度
 */
void Debug_UART_print(uint8_t *str, int bytes)
{
    uart_send_complete_flag = false;
    R_SCI_UART_Write(Debug_UART.p_ctrl, str, (uint32_t)bytes);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;
}

//重定向printf输出
 #if defined __GNUC__ && !defined __clang__
 int _write(int fd, char *pBuffer, int size)
 {
    (void)fd; //防止编译器警告
    /*若在printf之前串口使用了R_SCI_UART_Write()来发送数据，会导致这个标志位被置1，故进入函数时首先将该标志位置0*/
    uart_send_complete_flag = false;
    R_SCI_UART_Write(&Debug_UART_ctrl, (uint8_t *)pBuffer, (uint32_t)size);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;
    return size;
 }
 #else
 int fputc(int ch, FILE *f)
 {
    (void)f; //防止编译器警告
    /*若在printf之前串口使用了R_SCI_UART_Write()来发送数据，会导致这个标志位被置1，故进入函数时首先将该标志位置0*/
    uart_send_complete_flag = false;
    R_SCI_UART_Write(&Debug_UART_ctrl, (uint8_t *)&ch, 1);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;
    return ch;
 }
 #endif
