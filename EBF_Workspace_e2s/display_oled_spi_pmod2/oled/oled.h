#ifndef __OLED_H
#define __OLED_H

#define XLevelL			0x00
#define XLevelH			0x10
#define XLevel	        ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	    128
#define Max_Row			64
#define	Brightness	    0xCF
#define X_WIDTH 		128
#define Y_WIDTH 		64

#define OLED_DC(a)  R_IOPORT_PinWrite(&g_ioport_ctrl, dateORcmd, a)
#define OLED_CS(a)  R_IOPORT_PinWrite(&g_ioport_ctrl, CSspi, a)

//#define OLED_CLK(a)  R_IOPORT_PinWrite(&g_ioport_ctrl, CLK, a)
//#define OLED_MOSI(a)  R_IOPORT_PinWrite(&g_ioport_ctrl, MOSI, a)


void OLED_WrDat(unsigned char dat);
void OLED_WrCmd(unsigned char cmd);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char bmp_dat);
void OLED_CLS(void);
void OLED_Init(void);
void OLED_6x8Str(unsigned char x, unsigned char y, unsigned char ch[]);
void OLED_8x16Str(unsigned char x, unsigned char y, unsigned char ch[]);
void OLED_16x16CN(unsigned char x, unsigned char y, unsigned char N);
void OLED_BMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[]);


#endif
