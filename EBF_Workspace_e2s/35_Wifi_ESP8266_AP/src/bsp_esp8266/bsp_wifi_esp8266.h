#ifndef BSP_WIFI_ESP8266_H_
#define BSP_WIFI_ESP8266_H_
#include "hal_data.h"



/*宏定义调试信息*/
#define ESP8266_DEBUG   1

#if (ESP8266_DEBUG == 1)
#define     ESP8266_DEBUG_MSG(fmt, ... )        printf ( fmt, ##__VA_ARGS__ )
#else
#define     ESP8266_DEBUG_MSG(fmt, ... )
#endif


#define   ID             "ESP8266"             //要设置的热点的名称
#define   PASSWORD       "123456789"           //要设置的热点的密钥
#define   CHANNEL        "1"                   //要设置的通道号
#define   ENCRY_MODE     "3"                   //要设置的加密方式
/*
加密方式设置如下：
0：OPEN
1：WEP
2：WPA_PSK
3：WPA2_PSK
4：WPA_WPA2_PSK
*/

#define   SERVER_MODE    "1"                   //建立/关闭 服务器，1为建立，0为关闭
#define   SERVER_PORT    "8080"                //设置服务器端口号



#define ESP8266_MODULE_ENABLE     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_15, BSP_IO_LEVEL_HIGH);  //使能ESP8266模块
#define ESP8266_MODULE_DISABLE    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_15, BSP_IO_LEVEL_LOW);   //关闭ESP8266模块

/*红灯闪烁*/
#define ESP8266_ERROR_Alarm()     R_PORT4->PODR ^= 1<<(BSP_IO_PORT_04_PIN_00 & 0xFF); \
                                  R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS);


/*清除UART9数据缓冲区函数*/
#define   Clear_Buff()   memset( At_Rx_Buff , 0 , sizeof(At_Rx_Buff) ); \
                         Uart9_Num = 0;


void ESP8266_AP_Test(void);
void ESP8266_UART9_Init(void);
void ESP8266_AT_Send(char * cmd );
void ESP8266_Rst(void);
void ESP8266_STA ( void );
void ESP8266_AP ( void );
void ESP8266_STA_AP ( void );
void ESP8266_AP_SetAP( char * id ,  char * password , char * channel_num , char * encry_mode );
void Link_Mode( uint8_t mode );
void Query_Address(void);
void ESP8266_AP_SetServer( char * way ,  char * port );


#endif
