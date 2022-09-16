#ifndef DEBUG_UART_H_
#define DEBUG_UART_H_

#include "hal_data.h"
#include "stdio.h"


void Debug_UART_Init(void);

//在头文件中定义声明此函数，防止编译报警
#if defined __GNUC__ && !defined __clang__
int _write(int fd, char *pBuffer, int size);
#else
int fputc(int ch, FILE *f);
#endif

#endif /* DEBUG_UART_H_ */
