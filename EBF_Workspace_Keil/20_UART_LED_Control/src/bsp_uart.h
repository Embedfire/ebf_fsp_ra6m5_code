/*
 * bsp_uart.h
 *
 *  Created on: 2022年7月15日
 *      Author: admin
 */

#ifndef BSP_UART_H_
#define BSP_UART_H_

#include "hal_data.h"
#include "stdio.h"

extern volatile uint8_t Show_Message_flag;

void UART_Init(void); //初始化串口
void Show_Message(void); //打印提示

//在头文件中定义声明此函数，防止编译报警
#if defined __GNUC__ && !defined __clang__
int _write(int fd, char *pBuffer, int size);
#else
int fputc(int ch, FILE *f);
#endif

#endif /* BSP_UART_H_ */
