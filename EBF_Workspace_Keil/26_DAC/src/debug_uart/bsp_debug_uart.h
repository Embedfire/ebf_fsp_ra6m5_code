#ifndef __BSP_DEBUG_UART_H
#define	__BSP_DEBUG_UART_H
#include "hal_data.h"
#include "stdio.h"


void Debug_UART4_Init(void);

//在头文件中定义声明此函数，防止编译报警
#if defined __GNUC__ && !defined __clang__
int _write(int fd, char *pBuffer, int size);
#else
int fputc(int ch, FILE *f);
#endif

#endif
