/*
 * bsp_uart.h
 *
 *  Created on: 2022年7月15日
 *      Author: admin
 */

#ifndef BSP_UART_H_
#define BSP_UART_H_

#include "hal_data.h"


#define DEBUG_STR_LEN 0x1000 //缓冲区长度

#if defined __GNUC__ && !defined __clang__  //在GNU环境下使用自定义的UART_print函数,若需要打印浮点数，则需要分配堆，并在C++ Project Setting中设置printf支持浮点数输出
#define APP_PRINT(fn, ...) UART_print(debug_str,sprintf((char *)debug_str, fn , ##__VA_ARGS__));
extern uint8_t debug_str[DEBUG_STR_LEN];
#else  //标准库环境下直接使用printf
#define APP_PRINT(fn, ...) printf(fn, ##__VA_ARGS__);
#endif

extern volatile uint8_t uart_receiving;

void UART_Init(void); //初始化串口
void UART_print(uint8_t *str, int bytes);

//在头文件中定义声明此函数，防止编译报警
#if defined __GNUC__ && !defined __clang__
int _write(int fd, char *pBuffer, int size);
#else
int fputc(int ch, FILE *f);
#endif

#endif /* BSP_UART_H_ */
