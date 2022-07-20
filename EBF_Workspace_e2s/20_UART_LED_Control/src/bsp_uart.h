/*
 * bsp_uart.h
 *
 *  Created on: 2022年7月15日
 *      Author: admin
 */

#ifndef BSP_UART_H_
#define BSP_UART_H_

#include "hal_data.h"

extern volatile uint8_t Show_Message_flag;

void UART_Init(void); //初始化串口
void Show_Message(void); //打印提示

#endif /* BSP_UART_H_ */
