/*
 * bsp_uart.h
 *
 *  Created on: 2022年7月18日
 *      Author: embedfire-野火 www.embedfire.com
 */

#ifndef BSP_UART_H_
#define BSP_UART_H_
#include "hal_data.h"
#include <stdio.h>



void bsp_uart4_init(void);
void bsp_uart_dtc_init(void);
void r_sci_uart_baud_example(uint32_t _sci_uart_baudrate, uint32_t _error_percent);
#endif /* BSP_UART_H_ */
