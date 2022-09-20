#ifndef _I2C_EE_H_
#define _I2C_EE_H_

/* AT24C01/02每页有8个字节 */
#define EEPROM_PAGESIZE           8
#define I2C_Register_SR1                ((uint8_t)0x14)

void i2c_callback (i2c_master_callback_args_t * p_args);
void I2C_EE_Init();
void I2C_EE_ByteWrite(unsigned char address, unsigned char byte);
void I2C_EE_Writepage(unsigned char* ptr_write , unsigned char WriteAddr,unsigned char len);
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite);
void I2C_EE_BufferRead(unsigned char* ptr_read,unsigned char address,unsigned char byte);
void I2C_EE_WaitState(void);
void I2C_EE_Writedrase();
uint8_t I2C_Test(void);

#endif
