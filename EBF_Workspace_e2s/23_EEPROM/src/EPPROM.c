#include "hal_data.h"
#include "EPPROM.h"

bsp_io_level_t  pin_test;
bsp_io_level_t  pin_SCL;
bsp_io_level_t  pin_SDA;

unsigned char ACK_flag = 0;

int IIC_SCL_Read()
{
    R_IOPORT_PinRead(&g_ioport_ctrl, IICSCL, &pin_SCL);
    int a = (int)pin_SCL;
    return a;
}


int IIC_SDA_Read()
{
    R_IOPORT_PinRead(&g_ioport_ctrl, IICSDA, &pin_SDA);
    int a = (int)pin_SDA;
    return a;
}


int IIC_test_Read()
{
    R_IOPORT_PinRead(&g_ioport_ctrl, test, &pin_test);
    int a = (int)pin_test;
    return a;
}




void I2C_init()
{
    IIC_SCL_W(1);            //拉高SDA和SCL
    delay_5us();
    IIC_SDA_W(1);
    delay_5us();
}

void I2C_Start()
{
    IIC_SDA_W(1);
    IIC_SCL_W(1);             //打开时钟
    delay_5us();
    IIC_SDA_W(0);             //产生SDA下降沿，触发开始信号
    delay_5us();
}

void I2C_Stop()
{
    IIC_SCL_W(0);
    IIC_SDA_W(0);
    delay_1us();
    IIC_SCL_W(1);            //打开时钟
    delay_5us();
    IIC_SDA_W(1);           //产生SDA上升沿，触发结束信号
    delay_5us();
}



void I2C_Send(unsigned char byte)
{
    unsigned char i,temp;
    temp = byte;
    for(i = 0;i<8;i++)
    {
        IIC_SCL_W(0);        //关闭时钟准备数据变化
        if((temp & 0x80)==128) //从最高位发送 1000 0000
        {
            IIC_SDA_W(1);
        }
        else
        {
            IIC_SDA_W(0);
        }
        delay_5us();
        IIC_SCL_W(1);        //打开时钟发送数据
        delay_5us();
        temp <<= 1;
    }
}

/*  I2C数据接收 */
uint8_t I2C_Get()
{
    uint8_t i = 0,byte = 0;
    for(i = 0;i<8;i++)
    {
        IIC_SCL_W(0);       //关闭时钟准备数据变化
        delay_1us();
        IIC_SCL_W(1);        //打开时钟接收数据
        delay_5us();
        if(IIC_SDA_Read()) byte++; //从最高位接收
        printf("read num = %d\r\n",IIC_SDA_Read());
        IIC_SCL_W(0);        //接收完毕关闭时钟
        if(i == 7) return byte;
        byte <<= 1;
    }
    return 0x50;
}


/*  I2C主机应答 */
void I2C_ACK_Send(char A)
{
    IIC_SCL_W(0);
    delay_5us();
    if(A)               //如果i = 1那么拉低数据总线，表示主机应答。
    {
        IIC_SDA_W(0);
    }                   //如果i = 0发送非应答
    else
    {
        IIC_SDA_W(1);
    }
    delay_5us();
    IIC_SCL_W(1);
    delay_5us();
    IIC_SCL_W(0);
    delay_5us();
    IIC_SDA_W(1);
    delay_5us();
}

/*  I2C从机应答*/
int  I2C_ACK_Get()
{
    int flag;
    IIC_SCL_W(0);
    IIC_SDA_W(1);

    delay_2us();
    IIC_SCL_W(1);
    delay_2us();
    flag = IIC_SDA_Read();
    IIC_SCL_W(0);
    return flag;
}

/*  写入段 */
void I2C_Write(uint8_t add,uint8_t byte)
{
    I2C_Start();
    R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MICROSECONDS);
    I2C_Send(0xa0);
    printf("IIC back = %d\r\n",I2C_ACK_Get());
    I2C_Send(add);
    printf("IIC back = %d\r\n",I2C_ACK_Get());
    I2C_Send(byte);
    printf("IIC back = %d\r\n",I2C_ACK_Get());
    I2C_Stop();               //主机停止
}

/*  读取段 */
unsigned char I2C_Read(unsigned char add_7,unsigned char add)
{
    unsigned char message;
    I2C_Start();                    //开始
    I2C_Send(add_7+0);              //写eeprom
    if(I2C_ACK_Get()) ACK_flag = 1;         //接收从机ACK
    I2C_Send(add);                  //选择内存地址
    if(I2C_ACK_Get()) ACK_flag = 1;         //接收从机ACK

    I2C_Start();                    //重开始
    I2C_Send(add_7+1);              //读eeprom
    if(I2C_ACK_Get()) ACK_flag = 1;         //接收从机ACK
    message = I2C_Get();                    //接收从机数据
    I2C_ACK_Send(0);                //主机发送ACK
    I2C_Stop();                     //主机停止
    return message;
}


//#define IIC_SCL_W(a) R_IOPORT_PinWrite(&g_ioport_ctrl, IICSCL, a)
//#define IIC_SDA_W(a) R_IOPORT_PinWrite(&g_ioport_ctrl, IICSDA, a)
//#define test_Write(a) R_IOPORT_PinWrite(&g_ioport_ctrl, test, a)

