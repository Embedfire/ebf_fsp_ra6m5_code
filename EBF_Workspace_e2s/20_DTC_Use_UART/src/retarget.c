/**
  ******************************************************************************
  * @file    retarget.c
  * @author  fire
  * @version V1.0
  * @date    2022-07-xx
  * @brief   串口重定向文件
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火_瑞萨启明6M5开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "retarget.h"
#include "hal_data.h"
#include "bsp_uart.h"

extern volatile bool uart_send_complete_flag;


//重定向printf输出
#if defined __GNUC__ && !defined __clang__
int _write(int fd, char *pBuffer, int size)
{
    (void)fd;
    R_SCI_UART_Write(&g_uart4_ctrl, (uint8_t *)pBuffer, (uint32_t)size);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;

    return size;
}
#else
int fputc(int ch, FILE *f)
{
    void(f);
    R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *)&ch, 1);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;

    return ch;
}
#endif

