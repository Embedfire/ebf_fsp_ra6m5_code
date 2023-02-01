#ifndef __BSP_WIFI_ESP8266_H
#define __BSP_WIFI_ESP8266_H
#include "hal_data.h"



/*�궨�������Ϣ*/      
#define ESP8266_DEBUG   1

#if (ESP8266_DEBUG == 1)
#define     ESP8266_DEBUG_MSG(fmt, ... )        printf ( fmt, ##__VA_ARGS__ )
#else
#define     ESP8266_DEBUG_MSG(fmt, ... )
#endif


#define   ID             "ESP8266"             //Ҫ���õ��ȵ������
#define   PASSWORD       "123456789"           //Ҫ���õ��ȵ����Կ
#define   CHANNEL        "1"                   //Ҫ���õ�ͨ����
#define   ENCRY_MODE     "3"                   //Ҫ���õļ��ܷ�ʽ
/*
���ܷ�ʽ�������£�
0��OPEN
1��WEP
2��WPA_PSK
3��WPA2_PSK
4��WPA_WPA2_PSK
*/

#define   SERVER_MODE    "1"                   //����/�ر� ��������1Ϊ������0Ϊ�ر�
#define   SERVER_PORT    "8080"                //���÷������˿ں�



#define ESP8266_MODULE_ENABLE     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_15, BSP_IO_LEVEL_HIGH);  //ʹ��ESP8266ģ��
#define ESP8266_MODULE_DISABLE    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_15, BSP_IO_LEVEL_LOW);   //�ر�ESP8266ģ��

/*�����˸*/
#define ESP8266_ERROR_Alarm()     R_PORT4->PODR ^= 1<<(BSP_IO_PORT_04_PIN_00 & 0xFF); \
                                  R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS); 


/*���UART9���ݻ���������*/
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
