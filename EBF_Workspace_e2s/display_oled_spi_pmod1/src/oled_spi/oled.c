#include "hal_data.h"
#include "oledpic.h"
#include "oled.h"


spi_event_t g_transfer_complete;
/* Callback function */
void spi_callback(spi_callback_args_t *p_args)
{
    if (SPI_EVENT_TRANSFER_COMPLETE == p_args->event)
    {
        g_transfer_complete = true;
    }
}


void OLED_WrCmd(unsigned char cmd)//写命令
{
    uint8_t send_buffer[2] = {};

    OLED_DC(0);
    send_buffer[0] = cmd;
    R_SPI_Write(&g_spi0_ctrl, &send_buffer[0], 1, SPI_BIT_WIDTH_8_BITS);
    R_BSP_SoftwareDelay(1,BSP_DELAY_UNITS_MICROSECONDS);
}


void OLED_WrDat(unsigned char dat)//写数据
{
    uint8_t send_buffer[2] = {};

    OLED_DC(1);
    send_buffer[0] = dat;
    R_SPI_Write(&g_spi0_ctrl, &send_buffer[0], 1, SPI_BIT_WIDTH_8_BITS);
    R_BSP_SoftwareDelay(1,BSP_DELAY_UNITS_MICROSECONDS);
}

void OLED_SetPos(unsigned char x, unsigned char y)//设置起始点坐标
{
	OLED_WrCmd(0xb0 + y);
	OLED_WrCmd(((x&0xf0)>>4)|0x10);
	OLED_WrCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char bmp_dat)//全屏填充
{
	unsigned char y,x;
	for(y=0;y<8;y++)
	{
		OLED_WrCmd(0xb0+y);
		OLED_WrCmd(0x00);
		OLED_WrCmd(0x10);
		for(x=0;x<X_WIDTH;x++)
		{
			OLED_WrDat(bmp_dat);
		}
	}
}

void OLED_CLS(void)//清屏
{
	OLED_Fill(0x00);
}

void OLED_Init(void)
{
    OLED_WrCmd(0xAE);//关闭显示
    OLED_WrCmd(0xD5);//设置时钟分频因子
    OLED_WrCmd(0x80);

    OLED_WrCmd(0xA8);//设置驱动路数
    OLED_WrCmd(0x3F);//路数默认0x3F（1/64）

    OLED_WrCmd(0xD3);//设置显示偏移
    OLED_WrCmd(0x00);//偏移默认为0

    OLED_WrCmd(0x40);//设置显示开始行[5:0]

    OLED_WrCmd(0x8D);//电荷泵设置
    OLED_WrCmd(0x14);//bit2，开启/关闭

    OLED_WrCmd(0x20);//设置内存地址模式
    OLED_WrCmd(0x02);//[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;

    //OLED_WrCmd(0xA1);
    OLED_WrCmd(0xA1);//段重定义设置,bit0:0,0->0;1,0->127;
    OLED_WrCmd(0xc8);//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
    OLED_WrCmd(0xDA);//设置COM硬件引脚配置
    OLED_WrCmd(0x12);//[5:4]配置

    OLED_WrCmd(0x81);//对比度设置
    OLED_WrCmd(0xEF);//默认0x7F（范围1~255，越大越亮）

    OLED_WrCmd(0xD9);//设置预充电周期
    OLED_WrCmd(0xF1);//[3:0],PHASE 1;[7:4],PHASE 2;

    OLED_WrCmd(0xDB);//设置VCOMH 电压倍率
    OLED_WrCmd(0x30);//[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

    OLED_WrCmd(0xA4);//全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
    OLED_WrCmd(0xA6);//设置显示方式;bit0:1,反相显示;0,正常显示

    OLED_WrCmd(0xAF);//开启显示


    OLED_CLS();
}

void OLED_6x8Str(unsigned char x, unsigned char y, unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c = ch[j]-32;
		if(x>126)
		{
			x=0;y++;
		}
		OLED_SetPos(x,y);
		for(i=0;i<6;i++)
		{
			OLED_WrDat(F6x8[c][i]);
		}
		x+=6;
		j++;
	}
}

void OLED_8x16Str(unsigned char x, unsigned char y, unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120)
		{
			x=0;y++;
		}
		OLED_SetPos(x,y);
		for(i=0;i<8;i++)
		{
			OLED_WrDat(F8X16[c*16+i]);
		}
		OLED_SetPos(x,y+1);
		for(i=0;i<8;i++)
		{
			OLED_WrDat(F8X16[c*16+i+8]);
		}
		x+=8;
		j++;
	}
}



void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
  unsigned char c = 0,i = 0,j = 0;
  switch(TextSize)
  {
      case 1:
      {
          while(ch[j] != '\0')
          {
              c = ch[j] - 32;
              if(x > 126)
              {
                  x = 0;
                  y++;
              }
              OLED_SetPos(x,y);
              for(i=0;i<6;i++)
                  OLED_WrDat(F6x8[c][i]);
              x += 6;
              j++;
          }
      }break;
      case 2:
      {
          while(ch[j] != '\0')
          {
              c = ch[j] - 32;
              if(x > 120)
              {
                  x = 0;
                  y++;
              }
              OLED_SetPos(x,y);
              for(i=0;i<8;i++)
                  OLED_WrDat(F8X16[c*16+i]);
              OLED_SetPos(x,y+1);
              for(i=0;i<8;i++)
                  OLED_WrDat(F8X16[c*16+i+8]);
              x += 8;
              j++;
          }
      }break;
  }
}

void OLED_16x16CN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	}
}

void OLED_BMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;
	
  if(y1%8==0)
	{
		y=y1/8;
	}
  else
		y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			OLED_WrDat(BMP[j++]);
		}
	}
}








