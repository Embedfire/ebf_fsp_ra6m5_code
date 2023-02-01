#ifndef __BSP_WIFI_ESP8266_H
#define __BSP_WIFI_ESP8266_H
#include "hal_data.h"



/*�궨�������Ϣ*/      
#define ESP8266_DEBUG   1

#if     (ESP8266_DEBUG == 1)
#define     ESP8266_DEBUG_MSG(fmt, ... )        printf ( fmt, ##__VA_ARGS__ )
#else
#define     ESP8266_DEBUG_MSG(fmt, ... )
#endif


#define   ID             "AP_DEVICE"              //Ҫ���ӵ��ȵ������
#define   PASSWORD       "123456789"           //Ҫ���ӵ��ȵ����Կ

#define   SeverIP        "192.168.103.166"     //Ҫ���ӵķ�����IP
#define   SeverPort      "8000"                //Ҫ���ӵķ������˿�

#define ESP8266_MODULE_ENABLE     R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_15, BSP_IO_LEVEL_HIGH);  //ʹ��ESP8266ģ��
#define ESP8266_MODULE_DISABLE    R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_01_PIN_15, BSP_IO_LEVEL_LOW);   //�ر�ESP8266ģ��

/*�����˸*/
#define ESP8266_ERROR_Alarm()     R_PORT4->PODR ^= 1<<(BSP_IO_PORT_04_PIN_00 & 0xFF); \
                                  R_BSP_SoftwareDelay(500, BSP_DELAY_UNITS_MILLISECONDS); 


/*���UART9���ݻ���������*/
#define   Clear_Buff()   memset( At_Rx_Buff , 0 , sizeof(At_Rx_Buff) ); \
                         Uart9_Num = 0; 


void ESP8266_STA_Test(void);
void ESP8266_UART9_Init(void);
void ESP8266_AT_Send(char * cmd );
void ESP8266_STA ( void );
void ESP8266_AP ( void );
void ESP8266_STA_AP ( void );
void ESP8266_STA_JoinAP( char * id ,  char * password , uint8_t timeout );
void Link_Mode( uint8_t mode );
void ESP8266_STA_JoinServer( char * server_id ,  char * port , uint8_t timeout );
void ESP8266_STA_Transmission( void );
void ESP8266_Send_Data( void );


#endif
