#include "hal_data.h"
#include "debug_uart.h"
#include "i2c_ee.h"

#define timeout_ms 1000

i2c_master_event_t g_i2c_callback_event;
void i2c_callback (i2c_master_callback_args_t * p_args)
{
    g_i2c_callback_event = p_args->event;
}

void write_num(unsigned char address, unsigned char num)
{
    unsigned char send_buffer[2] = {};

    send_buffer[0] = address;
    send_buffer[1] = num;
    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], 2, false); //每当写完数据 false 总线拉高
    //R_BSP_SoftwareDelay(4000,BSP_DELAY_UNITS_MICROSECONDS);
    while ((I2C_MASTER_EVENT_TX_COMPLETE != g_i2c_callback_event) && timeout_ms)
    {
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        //timeout_ms--;
    }
    if (I2C_MASTER_EVENT_ABORTED == g_i2c_callback_event)
    {
        __BKPT(0);
    }

    printf("EPPROM send num %d in %d address\r\n",send_buffer[1],send_buffer[0]);
}

int read_num(unsigned char address)
{
    unsigned char send_buffer[2] = {};
    unsigned char read_buffer[2] = {};

    send_buffer[0] = address;
    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], 1, true);
    R_BSP_SoftwareDelay(300,BSP_DELAY_UNITS_MICROSECONDS);        //这里必须增加个延时不然会出现错误

    R_IIC_MASTER_Read(&g_i2c_master0_ctrl, &read_buffer[0], 1, false);
    R_BSP_SoftwareDelay(300,BSP_DELAY_UNITS_MICROSECONDS);

    printf("EPPROM read num = %d\r\n",read_buffer[0]);

    return read_buffer[0];
}


void write_continuity(unsigned char* ptr_write,unsigned char address,unsigned char byte)      //暴力写入
{
    unsigned char send_buffer[2] = {};

    for(int i = 0;i<byte;i++)
    {
        send_buffer[0] = address+i;
        send_buffer[1] = *(ptr_write+i);


        R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], 2, false);
        R_BSP_SoftwareDelay(3,BSP_DELAY_UNITS_MILLISECONDS);

    }

    R_BSP_SoftwareDelay(100,BSP_DELAY_UNITS_MILLISECONDS);       //延时ms
}

void write_drase()  //暴力删除数据
{
    unsigned char send_buffer[2] = {};

    for(int i = 0;i<256;i++)
    {
        send_buffer[0] = i;
        send_buffer[1] = 0xff;


        R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], 2, false);
        R_BSP_SoftwareDelay(1,BSP_DELAY_UNITS_MILLISECONDS);
        R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], 1, false);
        R_BSP_SoftwareDelay(1,BSP_DELAY_UNITS_MILLISECONDS);

    }
}


void read_continuity(unsigned char* ptr_read,unsigned char address,unsigned char byte)        //暴力读取
{
    unsigned char send_buffer[2] = {};
    unsigned char read_buffer[1] = {};

    for(unsigned char i = 0;i<byte;i++)
    {
        send_buffer[0] = address + i;
        R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], 1, true);
        R_BSP_SoftwareDelay(300,BSP_DELAY_UNITS_MICROSECONDS);        //这里必须增加个延时不然会出现错误


        R_IIC_MASTER_Read(&g_i2c_master0_ctrl, &read_buffer[0], 1, false);
        R_BSP_SoftwareDelay(1,BSP_DELAY_UNITS_MILLISECONDS);        //说实话100是最舒服的

        *(ptr_read+1) = read_buffer[0];
        printf("read ptr= %d\r\n",read_buffer[0]);                    //这里要对时间进行优化
    }

}

void write_page(unsigned char* ptr_write , unsigned char page)      //页写入   page 0~31
{
    unsigned char send_buffer[256] = {};
    send_buffer[0] = page*8;

    for(unsigned char i = 0;i<8;i++)
    {
        send_buffer[1+i] = *(ptr_write+i);
    }

    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], 9, false);
    R_BSP_SoftwareDelay(4,BSP_DELAY_UNITS_MILLISECONDS);

}


//void BufferWrite(unsigned char address,unsigned char byte)
//{
//    //首先8页的起始地址，中间地址的计算
//    //假如地址在20写入30个数据
//    unsigned char firstaddr_byte = 0,middle_byte = 0,then_addr_byte,middle_addr = 0,then_addr=0;
//    unsigned char send_buffer[256] = {};
//    //20%8余下4   放入到firstaddr_byte
//    firstaddr_byte = address%8;
//
//    send_buffer[0] = address;
//    send_buffer[1] = 10;
//    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], firstaddr_byte, false);
//
//    middle_addr = (address+firstaddr_byte)/8;
//    middle_byte = (byte-firstaddr_byte)/8;
//    for(int i = 0 ;i < middle_byte;i++)
//    {
//        write_page(&send_buffer[0],middle_addr+i);
//    }
//    //之后24/8余下3  （30+20）/8余下6  6-3余下3
//
//    then_addr_byte = (byte-firstaddr_byte)%8;
//    then_addr = (address+byte)/8;
//    send_buffer[0] = then_addr;
//    send_buffer[1] = 10;
//    R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], firstaddr_byte, false);
//    //最后50%8余下2
//
//    //首先起始地址数据烧录
//    //中间地址进行烧录
//    //小尾巴
//}




void BufferWrite(unsigned char* ptr_write,unsigned char address,unsigned char byte)
{
    unsigned char i,b;
    unsigned char firstaddr_byte = 0,middle_byte = 0,then_addr_byte,middle_addr = 0,then_addr=0 ,page_num =8;
    unsigned char send_buffer[256] = {};

    //起始
    firstaddr_byte = address%8;

    if(byte<=firstaddr_byte)
    {
        send_buffer[0] = address;
        for(i = 0; i<firstaddr_byte ;i++)   //发送的数据赋值
        {
            send_buffer[1+i] = *(ptr_write+i);  //指向数据地址的指针偏移
        }
        R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], firstaddr_byte+1, false);
        R_BSP_SoftwareDelay(4,BSP_DELAY_UNITS_MILLISECONDS);

    }else if(byte>firstaddr_byte)
    {
        send_buffer[0] = address;
        for(i = 0; i<firstaddr_byte ;i++)
        {
            send_buffer[1+i] = *(ptr_write+i);
        }
        R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], firstaddr_byte+1, false);
        R_BSP_SoftwareDelay(4,BSP_DELAY_UNITS_MILLISECONDS);

        //中间
        middle_addr = (address+firstaddr_byte)/page_num;   //起始地址
        middle_byte = (byte-firstaddr_byte)/page_num;      //中间有多少个页
        for(i = 0 ;i < middle_byte;i++)
        {
            for(b = 0; b<8 ;b++)
            {
                send_buffer[b] = *(ptr_write+firstaddr_byte+b+i*8);
            }
            write_page(&send_buffer[0],middle_addr+i);
        }

        //小尾巴
        then_addr_byte = (address+byte)%8;
        then_addr = (address+byte)/8;
        send_buffer[0] = then_addr*8;
        for(i = 0; i<then_addr_byte ;i++)
        {
            send_buffer[1+i] = *(ptr_write+firstaddr_byte+middle_byte*8+i);
        }
        R_IIC_MASTER_Write(&g_i2c_master0_ctrl, &send_buffer[0], then_addr_byte+1, false);
        R_BSP_SoftwareDelay(4,BSP_DELAY_UNITS_MILLISECONDS);
    }
}
