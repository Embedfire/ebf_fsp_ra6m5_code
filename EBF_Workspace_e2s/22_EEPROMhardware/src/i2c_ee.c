#include "hal_data.h"
#include "debug_uart.h"
#include "i2c_ee.h"

unsigned int timeout_ms = 500;

i2c_master_event_t g_i2c_callback_event;
static volatile bool iic_complete = false;

void i2c_callback (i2c_master_callback_args_t * p_args)
{
    g_i2c_callback_event = p_args->event;
}



void I2C_EE_Init()
{
    R_IIC_MASTER_Open(&g_i2c_master0_ctrl, &g_i2c_master0_cfg);
}

void I2C_EE_ByteWrite(unsigned char address, unsigned char byte)
{
    iic_complete = false;
    unsigned char send_buffer[2] = {};

    send_buffer[0] = address;
    send_buffer[1] = byte;
    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], 2, false); //每当写完数据 false 总线拉高

    while (false == iic_complete)
    {
        for(int i=0;i<timeout_ms;i++)
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MICROSECONDS);
    }
}




//八个字节的页写入
void I2C_EE_Writepage(unsigned char* ptr_write , unsigned char WriteAddr,unsigned char len)      //页写入   page 0~31
{

    unsigned char send_buffer[9] = {};
    send_buffer[0] = WriteAddr;

    for(unsigned char i = 0;i<len;i++)
    {
        send_buffer[1+i] = *(ptr_write+i);
    }

    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], len+1 , false);

    while ((I2C_MASTER_EVENT_TX_COMPLETE != g_i2c_callback_event) && timeout_ms)
    {
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        timeout_ms--;
    }
    timeout_ms = 500;

    I2C_EE_WaitState();
}


/**
* @brief   将缓冲区中的数据写到I2C EEPROM中
* @param
*   @arg pBuffer:缓冲区指针
*   @arg WriteAddr:写地址
*     @arg NumByteToWrite:写的字节数
* @retval  无
*/
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite)
{
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

    Addr = WriteAddr % EEPROM_PAGESIZE;
    count = EEPROM_PAGESIZE - Addr;
    NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;
    NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;

    /* If WriteAddr is I2C_PageSize aligned  */
    if (Addr == 0) {
        /* If NumByteToWrite < I2C_PageSize */
        if (NumOfPage == 0) {
            I2C_EE_Writepage(pBuffer, WriteAddr, NumOfSingle);
        }
        /* If NumByteToWrite > I2C_PageSize */
        else {
            while (NumOfPage--) {
                I2C_EE_Writepage(pBuffer, WriteAddr, EEPROM_PAGESIZE);
                WriteAddr +=  EEPROM_PAGESIZE;
                pBuffer += EEPROM_PAGESIZE;
            }

            if (NumOfSingle!=0) {
                I2C_EE_Writepage(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
    /* If WriteAddr is not I2C_PageSize aligned  */
    else {
        /* If NumByteToWrite < I2C_PageSize */
        if (NumOfPage== 0) {
            I2C_EE_Writepage(pBuffer, WriteAddr, NumOfSingle);
        }
        /* If NumByteToWrite > I2C_PageSize */
        else {
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / EEPROM_PAGESIZE;
            NumOfSingle = NumByteToWrite % EEPROM_PAGESIZE;

            if (count != 0) {
                I2C_EE_Writepage(pBuffer, WriteAddr, count);
                WriteAddr += count;
                pBuffer += count;
            }

            while (NumOfPage--) {
                I2C_EE_Writepage(pBuffer, WriteAddr, EEPROM_PAGESIZE);
                WriteAddr +=  EEPROM_PAGESIZE;
                pBuffer += EEPROM_PAGESIZE;
            }
            if (NumOfSingle != 0) {
                I2C_EE_Writepage(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

void I2C_EE_BufferRead(unsigned char* ptr_read,unsigned char address,unsigned char byte)
{

    unsigned char send_buffer[2] = {};
    unsigned char read_buffer[1] = {};

    for(unsigned char i = 0;i<byte;i++)
    {
        send_buffer[0] = address + i;
        R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], 1, true);
        while ((I2C_MASTER_EVENT_TX_COMPLETE != g_i2c_callback_event) && timeout_ms)
        {
            R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
            timeout_ms--;
        }
        timeout_ms = 500;

        R_IIC_MASTER_Read(&g_i2c_master0_ctrl, &read_buffer[0], 1, false);
        while ((I2C_MASTER_EVENT_RX_COMPLETE != g_i2c_callback_event) && timeout_ms)
        {
            R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
            timeout_ms--;
        }
        timeout_ms = 500;

        *(ptr_read+i) = read_buffer[0];
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        //printf("read str = 0x%X",read_buffer[0]);
    }

}


void I2C_EE_Writedrase()
{
    unsigned char I2c_Buf_Write[256] = {};
    for(int i = 0 ; i<256 ; i++)
    {
        I2c_Buf_Write[i] = 0xff;
    }
    I2C_EE_BufferWrite(I2c_Buf_Write, 0x00 , 256);
}

void I2C_EE_WaitState(void)
{
    while (timeout_ms)
    {
        R_BSP_SoftwareDelay(15U, BSP_DELAY_UNITS_MICROSECONDS);
        timeout_ms--;
    }
    timeout_ms = 500;

}

/**
* @brief  I2C(AT24C02)读写测试
* @param  无
* @retval 正常返回1 ，不正常返回0
*/
uint8_t I2C_Test(void)
{
    uint16_t i;
    unsigned char DATA_Size = 45;
    unsigned char I2c_Buf_Write[50] = {};
    unsigned char I2c_Buf_Read[50] = {};

    printf("写入的数据\n");

    for ( i=0; i<DATA_Size; i++ ) { //填充缓冲
        I2c_Buf_Write[i] =i;
        printf("0x%02X \n", I2c_Buf_Write[i]);
        if (i%16 == 15)
            printf("\n\r");
    }

    //将I2c_Buf_Write中顺序递增的数据写入EERPOM中
    I2C_EE_BufferWrite( I2c_Buf_Write, 0x00 , DATA_Size);

    printf("读出的数据\n");
    //将EEPROM读出数据顺序保持到I2c_Buf_Read中
    I2C_EE_BufferRead(I2c_Buf_Read, 0x00 , DATA_Size);
    //将I2c_Buf_Read中的数据通过串口打印
    for (i=0; i<DATA_Size; i++) {
        if (I2c_Buf_Read[i] != I2c_Buf_Write[i]) {
            printf("0x%02X \n", I2c_Buf_Read[i]);
            printf("错误:I2C EEPROM写入与读出的数据不一致\n");
            printf("%d\n",i);
            return 0;
        }
        printf("0x%02X \n", I2c_Buf_Read[i]);
        if (i%16 == 15)
            printf("\n\r");

    }
    printf("I2C(AT24C02)读写测试成功\n");
    return 1;
}