#ifndef RS485_RS485_H_
#define RS485_RS485_H_

#include "hal_data.h"

#define RS485_1_TX     R_IOPORT_PinWrite(&g_ioport_ctrl,BSP_IO_PORT_05_PIN_03,BSP_IO_LEVEL_HIGH)
#define RS485_1_RX     R_IOPORT_PinWrite(&g_ioport_ctrl,BSP_IO_PORT_05_PIN_03,BSP_IO_LEVEL_LOW)

void RS485_1_Init(void);
void rs485_send_example(void);

#endif
