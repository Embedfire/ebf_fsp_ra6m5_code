#ifndef _I2C_EE_H_
#define _I2C_EE_H_

void i2c_callback (i2c_master_callback_args_t * p_args);
void write_num(unsigned char address, unsigned char num);
int read_num(unsigned char address);
void write_continuity(unsigned char* ptr_write,unsigned char address,unsigned char byte);
void write_drase();
void read_continuity(unsigned char* ptr_read,unsigned char address,unsigned char byte);
void write_page(unsigned char* ptr_write , unsigned char page);
void BufferWrite(unsigned char* ptr_write,unsigned char address,unsigned char byte);

#endif
