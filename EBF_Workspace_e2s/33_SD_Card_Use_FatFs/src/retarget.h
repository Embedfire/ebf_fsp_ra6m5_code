/*
 * retarget.h
 *
 *  Created on: 2022年7月18日
 *      Author: 野火
 */

#ifndef RETARGET_H_
#define RETARGET_H_
#include <stdio.h>  //必须包含

//重定向printf输出
#if defined __GNUC__ && !defined __clang__
int _write(int fd, char *pBuffer, int size);
#else
int fputc(int ch, FILE *f);
#endif
#endif /* RETARGET_H_ */
