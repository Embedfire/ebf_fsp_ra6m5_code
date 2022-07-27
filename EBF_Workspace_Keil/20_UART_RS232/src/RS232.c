/**
 ******************************************************************************
 * @file    debug_uart.c
 * @author  fire
 * @version V1.0
 * @date    2022-7-25
 * @brief   RS232串口初始化函数，中断回调函数。
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

#include "RS232.h"

//暂时使用板间通信来测试232，BOARD1是与电脑连接的板，BOARD2是与BOARD1通信的板子。BOARD1收到上位机的数据，转发给BOARD2，BOARD2再传回BOARD1，BOARD1再传给上位机
#define BOARD 1

void RS232_Init(void)
{
	R_SCI_UART_Open(RS232.p_ctrl,RS232.p_cfg);
}

#if BOARD==2
void RS232_Callback(uart_callback_args_t *p_args)
{
	static int i=0;
	i++;
	switch(p_args->event)
	{
		case UART_EVENT_RX_CHAR:  //收到数据后直接发给debug串口
			R_SCI_UART_Write(Debug_UART.p_ctrl,(uint8_t *)&p_args->data,1);
		default:
			break;
	}
}
#endif
#if BOARD==2
void RS232_Callback(uart_callback_args_t *p_args)
{
	static int i=0;
	i++;
	switch(p_args->event)
	{
		case UART_EVENT_RX_CHAR:  //收到数据后直接发回去
			R_SCI_UART_Write(RS232.p_ctrl,(uint8_t *)&p_args->data,1);
		default:
			break;
	}
}
#endif