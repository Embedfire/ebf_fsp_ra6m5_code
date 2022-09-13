/*
 * debug_uart.h
 *
 *  Created on: 2022年7月25日
 *      Author: admin
 */

#ifndef BSP_DEBUG_UART_H_
#define BSP_DEBUG_UART_H_

#include "hal_data.h"
#include "stdio.h"

#define DEBUG_STR_LEN 0x1000

#if defined __GNUC__ && !defined __clang__
#define APP_PRINT(fn, ...) Debug_UART_print(debug_str,sprintf((char *)debug_str, fn , ##__VA_ARGS__));
extern uint8_t debug_str[DEBUG_STR_LEN];
#else
#define APP_PRINT(fn, ...) printf(fn, ##__VA_ARGS__);
#endif

void Debug_UART_print(uint8_t *str, int bytes); //自定义的串口打印，配合宏APP_PRINT使用或者直接用都行

void Debug_UART_Init(void);

//在头文件中定义声明此函数，防止编译报警
#if defined __GNUC__ && !defined __clang__
int _write(int fd, char *pBuffer, int size);
#else
int fputc(int ch, FILE *f);
#endif

#endif /* BSP_DEBUG_UART_H_ */
