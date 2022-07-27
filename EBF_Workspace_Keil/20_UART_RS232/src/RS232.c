/**
 ******************************************************************************
 * @file    debug_uart.c
 * @author  fire
 * @version V1.0
 * @date    2022-7-25
 * @brief   RS232���ڳ�ʼ���������жϻص�������
 ******************************************************************************
 * @attention
 *
 * ʵ��ƽ̨ : RA6M5
 * ��˾     :http://www.embedfire.com
 * ��̳     :http://www.firebbs.cn
 * �Ա�     :http://firestm32.taobao.com
 *
 ******************************************************************************
 */

#include "RS232.h"

//��ʱʹ�ð��ͨ��������232��BOARD1����������ӵİ壬BOARD2����BOARD1ͨ�ŵİ��ӡ�BOARD1�յ���λ�������ݣ�ת����BOARD2��BOARD2�ٴ���BOARD1��BOARD1�ٴ�����λ��
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
		case UART_EVENT_RX_CHAR:  //�յ����ݺ�ֱ�ӷ���debug����
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
		case UART_EVENT_RX_CHAR:  //�յ����ݺ�ֱ�ӷ���ȥ
			R_SCI_UART_Write(RS232.p_ctrl,(uint8_t *)&p_args->data,1);
		default:
			break;
	}
}
#endif