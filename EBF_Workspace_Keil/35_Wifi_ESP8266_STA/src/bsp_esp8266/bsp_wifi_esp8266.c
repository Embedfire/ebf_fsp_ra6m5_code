#include "bsp_debug_uart.h"
#include "bsp_wifi_esp8266.h"

_Bool               Uart9_Receive_Flag = false; //用来判断UART9接收以及发送数据是否完成
_Bool               Uart9_Show_Flag = false;    //控制UART9收发数据显示标志

/*用来接收UART9数据的缓冲区*/
char                At_Rx_Buff[256];      
uint8_t             Uart9_Num = 0;

/*自动配置ESP8266函数*/
void ESP8266_STA_Test(void)
{
        
    ESP8266_DEBUG_MSG("\r\n正在初始化ESP8266...\r\n");
    ESP8266_UART9_Init();
    
    ESP8266_DEBUG_MSG("\r\n设置STA模式中...\r\n");
    ESP8266_STA();
   
    ESP8266_DEBUG_MSG("\r\n正在连接WIFI中...\r\n");
    ESP8266_STA_JoinAP( ID , PASSWORD , 20 );
    Link_Mode( 0 );
     
    ESP8266_DEBUG_MSG("\r\n正在连接服务器中...\r\n");
    ESP8266_STA_JoinServer( SeverIP , SeverPort , 20 );
    
    ESP8266_DEBUG_MSG("\r\n正在配置为透传发送模式...\r\n");
    ESP8266_STA_Transmission();
    ESP8266_Send_Data();
    
}


/*ESP8266 (SPI9 UART) 初始化函数*/
void ESP8266_UART9_Init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    
    err = R_SCI_UART_Open(g_uart9_esp8266.p_ctrl, g_uart9_esp8266.p_cfg);
    assert(FSP_SUCCESS == err);
}


/*向ESP8266发送AT指令函数*/
void ESP8266_AT_Send(char * cmd )  
{
    /*向ESP8266(UART9)发送指令*/
    R_SCI_UART_Write(&g_uart9_esp8266_ctrl, (uint8_t *)cmd, strlen(cmd)); 
    
    /*AT指令发送完成标志*/
    Uart9_Receive_Flag = false;

}


/*设置ESP8266为 STA 模式*/
void ESP8266_STA ( void )
{
    ESP8266_AT_Send ( "AT+CWMODE=1\r\n" );
     
    /*等待设置完成*/
    while ( !Uart9_Receive_Flag )
    {
         if (strstr( At_Rx_Buff , "OK\r\n" ))
         {
         ESP8266_DEBUG_MSG("\r\nESP8266已切换为STA模式\r\n");
         Clear_Buff();      //清除缓冲区数据
         }       
    }
}


/*设置ESP8266为 AP 模式*/
void ESP8266_AP ( void )
{
      ESP8266_AT_Send ( "AT+CWMODE=2\r\n" );

      /*等待设置完成*/
      while ( !Uart9_Receive_Flag )
      {
           if (strstr( At_Rx_Buff , "OK\r\n" ))
           {
           ESP8266_DEBUG_MSG("\r\nESP8266已切换为AP模式\r\n");
           Clear_Buff();      //清除缓冲区数据
           }
      }
}


/*设置ESP8266为 STA + AP 模式*/
void ESP8266_STA_AP ( void )
{
      ESP8266_AT_Send ( "AT+CWMODE=3\r\n" );

      /*等待设置完成*/
      while ( !Uart9_Receive_Flag )
      {
           if (strstr( At_Rx_Buff , "OK\r\n" ))
           {
           ESP8266_DEBUG_MSG("\r\nESP8266已切换为STA+AP模式\r\n");
           Clear_Buff();      //清除缓冲区数据
           }
      }
}


/*ESP8266连接WiFi函数，timeout：期望最大连接时间*/
void ESP8266_STA_JoinAP( char * id ,  char * password , uint8_t timeout )
{
    char  JoinAP_AT[256];
    
    uint8_t i;
    
    sprintf( JoinAP_AT , "AT+CWJAP=\"%s\",\"%s\"\r\n" , id , password);
    
    ESP8266_AT_Send( JoinAP_AT );

   /*判断连接是否设置成功，失败则打印错误信息*/
   while ( !Uart9_Receive_Flag )
     {
    for(i = 0; i <= timeout; i++)
       {
          if ( strstr( At_Rx_Buff , "OK\r\n" ) ) 
           {
               ESP8266_DEBUG_MSG("\r\nWifi连接成功\r\n");
               Clear_Buff();      //清除缓冲区数据
               break;
           }
          if ( strstr( At_Rx_Buff , "ERROR\r\n" ) ) 
           {
               if( strstr( At_Rx_Buff , "+CWJAP:1\r\n" ))
               ESP8266_DEBUG_MSG("\r\nWifi连接超时，请检查各项配置是否正确\r\n");
               
               if( strstr( At_Rx_Buff , "+CWJAP:2\r\n" ))
               ESP8266_DEBUG_MSG("\r\nWifi密码错误，请检查Wifi密码是否正确\r\n");
               
               if( strstr( At_Rx_Buff , "+CWJAP:3\r\n" ))
                   ESP8266_DEBUG_MSG("\r\n无法找到目标Wifi，请检查Wifi是否打开或Wifi名称是否正确\r\n");
               
               if( strstr( At_Rx_Buff , "+CWJAP:4\r\n" ))
               ESP8266_DEBUG_MSG("\r\nWifi连接失败，请检查各项配置是否正确\r\n");
               
               while(1)
               {
                ESP8266_ERROR_Alarm();
               }      //LED灯警告错误，红灯闪烁
           }
           if ( i == timeout )
           {
               ESP8266_DEBUG_MSG("\r\nWifi连接超出期望时间，请检查各项配置是否正确\r\n");
               while(1)
               {
                ESP8266_ERROR_Alarm();
               }      //LED灯警告错误，红灯闪烁
           }
           R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
        }
      }
}

/*设置连接模式为单连接或者多连接*/
void Link_Mode( uint8_t mode )
{
      switch ( mode ){
          case 0 :
            ESP8266_AT_Send("AT+CIPMUX=0\r\n"); //设置为单连接模式，透传只有在单连接模式下才能进行
            break;
          case 1 :
            ESP8266_AT_Send("AT+CIPMUX=1\r\n"); //设置为多连接模式，服务器只有在多连接模式下才能打开
            break;
      }
    
       /*等待设置完成*/
      while ( !Uart9_Receive_Flag )
      {
           if (strstr( At_Rx_Buff , "OK\r\n" ))
           {
               if( mode )
                ESP8266_DEBUG_MSG("\r\nESP8266已切换为多连接模式\r\n");
               else
                ESP8266_DEBUG_MSG("\r\nESP8266已切换为单连接模式\r\n");
                Clear_Buff();      //清除缓冲区数据
           }
      }
}


/*ESP8266连接服务器函数，timeout：期望最大连接时间*/
void ESP8266_STA_JoinServer( char * server_id ,  char * port , uint8_t timeout )
{
    char  JoinServer_AT[256];
    
    uint8_t i;
    
    sprintf( JoinServer_AT , "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n" , server_id , port );
    
    ESP8266_AT_Send( JoinServer_AT );
    
      /*判断连接是否设置成功，失败则打印错误信息*/
      while ( !Uart9_Receive_Flag )
      {
           for(i = 0; i <= timeout; i++)
       {
           if (strstr( At_Rx_Buff , "OK\r\n" ))
           {
               ESP8266_DEBUG_MSG("\r\n服务器连接成功\r\n");
               Clear_Buff();      //清除缓冲区数据
               break;
           }
           if (strstr( At_Rx_Buff , "ERROR\r\n" ))
           {
               ESP8266_DEBUG_MSG("\r\n服务器连接失败，请检查服务器是否打开以及参数是否正确\r\n");
               while(1)
               {
                ESP8266_ERROR_Alarm();
               }      //LED灯警告错误，红灯闪烁
           }
           if ( i == timeout )
           {
               ESP8266_DEBUG_MSG("\r\n服务器连接超出期望时间，请检查各项配置是否正确\r\n");
               while(1)
               {
                ESP8266_ERROR_Alarm();
               }      //LED灯警告错误，红灯循环闪烁
           }
           R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
       }
      }
}


/*设置ESP8266为透传模式*/
void ESP8266_STA_Transmission( void )
{
      ESP8266_AT_Send ( "AT+CIPMODE=1\r\n" );

      /*等待设置完成*/
      while ( !Uart9_Receive_Flag )
      {
           if (strstr( At_Rx_Buff , "OK\r\n" ))
           {
           ESP8266_DEBUG_MSG("\r\nESP8266已切换为透传模式\r\n");
           Clear_Buff();      //清除缓冲区数据
           }
      }
}


/*设置ESP8266为发送数据模式*/
void ESP8266_Send_Data( void )
{
    ESP8266_AT_Send ( "AT+CIPSEND\r\n" );
    
    /*等待设置完成*/
    while ( !Uart9_Receive_Flag )
    {
         if (strstr( At_Rx_Buff , "OK\r\n" ))
         {
            ESP8266_DEBUG_MSG("\r\nESP8266已进入透传发送模式\r\n\r\n>");
            Uart9_Show_Flag = true;            
            Clear_Buff();      //清除缓冲区数据
         }
    }    
}


/*Wifi串口回调函数*/
void esp8266_uart9_callback(uart_callback_args_t * p_args)
{
       switch(p_args->event)
       {
           case UART_EVENT_RX_CHAR:  
               
                At_Rx_Buff[Uart9_Num++] = (uint8_t ) p_args->data;  //将UART9收到的数据放到Buff缓冲区中
                
                /*进入透传模式后打开串口调试助手收发数据显示*/
                if( Uart9_Show_Flag )
                R_SCI_UART_Write(&g_uart4_ctrl, (uint8_t *)&(p_args->data), 1); 
                
                break;
           
           case UART_EVENT_TX_COMPLETE:
           {
                Uart9_Receive_Flag = true;      //ESP8266回应完成标志
               
                break;
           }
           default:
               break;
       }
}

