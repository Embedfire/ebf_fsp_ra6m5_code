#ifndef _EPPROM_H_
#define _EPPROM_H_

#include "hal_data.h"

#define IIC_SCL_W(a) R_IOPORT_PinWrite(&g_ioport_ctrl, IICSCL, a)
#define IIC_SDA_W(a) R_IOPORT_PinWrite(&g_ioport_ctrl, IICSDA, a)
#define test_Write(a) R_IOPORT_PinWrite(&g_ioport_ctrl, test, a)


#define delay_5us()  R_BSP_SoftwareDelay(5, BSP_DELAY_UNITS_MICROSECONDS)
#define delay_2us()  R_BSP_SoftwareDelay(2, BSP_DELAY_UNITS_MICROSECONDS)
#define delay_1us()  R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MICROSECONDS)

void I2C_init();
void I2C_Start();
void I2C_Stop();
void I2C_Send(unsigned char byte);
unsigned char I2C_Get();
void I2C_ACK_Send(char A);
int I2C_ACK_Get();

void I2C_Write(uint8_t add,uint8_t byte);
unsigned char I2C_Read(unsigned char add_7,unsigned char add);
void I2C_test();

#endif
