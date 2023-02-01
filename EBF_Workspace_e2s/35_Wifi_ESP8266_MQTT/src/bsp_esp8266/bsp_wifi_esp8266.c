#include <bsp_esp8266/bsp_wifi_esp8266.h>
#include "debug_uart/bsp_debug_uart.h"


_Bool               Uart9_Receive_Flag = false; //用来判断UART9接收以及发送数据是否完成
_Bool               Uart9_Show_Flag = false;    //控制UART9收发数据显示标志

/*用来接收UART9数据的缓冲区*/
char                At_Rx_Buff[256];
uint8_t             Uart9_Num = 0;

/*自动配置ESP8266函数*/
void ESP8266_MQTT_Test(void)
{

    ESP8266_DEBUG_MSG("\r\n正在初始化ESP8266...\r\n");
    ESP8266_UART9_Init();

    ESP8266_DEBUG_MSG("\r\n设置STA模式中...\r\n");
    ESP8266_STA();
    ESP8266_Rst();

    ESP8266_DEBUG_MSG("\r\n正在连接WIFI中...\r\n");
    ESP8266_STA_JoinAP( ID , PASSWORD , 20 );

    ESP8266_DEBUG_MSG("\r\n正在配置MQTT用户信息...\r\n");
    MQTT_SetUserProperty( CLIENT_ID , USER_NAME, USER_PASSWORD );

    ESP8266_DEBUG_MSG("\r\n正在连接MQTT服务器...\r\n");
    Connect_MQTT( MQTT_IP , MQTT_Port , 10 );

    ESP8266_DEBUG_MSG("\r\n正在订阅主题...\r\n");
    Subscribes_Topics( MQTT_TOPICS );

    ESP8266_DEBUG_MSG("\r\n正在发布消息...\r\n");
    Send_Data( MQTT_TOPICS , TOPICS_DATA );

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

/*重启ESP8266函数*/
void ESP8266_Rst(void)
{
    ESP8266_AT_Send ( "AT+RST\r\n" );

    /*判断是否设置成功*/
    while ( !Uart9_Receive_Flag )
    {
         if (strstr( At_Rx_Buff , "ready\r\n" ))
         {
         R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);  //等待重启完成
         ESP8266_DEBUG_MSG("\r\nESP8266已重启\r\n");
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

/*设置 MQTT 用户属性*/
void MQTT_SetUserProperty( char * client_id , char * user_name, char * user_password )
{
    char  SetUserProperty_AT[256];

    sprintf( SetUserProperty_AT , "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n" , client_id , user_name , user_password);

    ESP8266_AT_Send ( SetUserProperty_AT );

    /*等待设置完成*/
    while ( !Uart9_Receive_Flag )
    {
         if (strstr( At_Rx_Buff , "OK\r\n" ))
         {
         ESP8266_DEBUG_MSG("\r\nMQTT用户属性已设置完成\r\n");
         Clear_Buff();      //清除缓冲区数据
         }
    }

}

/*连接MQTT服务器函数*/
void Connect_MQTT( char * mqtt_ip , char * mqtt_port , uint8_t timeout )
{
    char  Connect_MQTT_AT[256];

    uint8_t i;

    sprintf( Connect_MQTT_AT , "AT+MQTTCONN=0,\"%s\",%s,0\r\n" , mqtt_ip , mqtt_port);

    ESP8266_AT_Send( Connect_MQTT_AT );

       /*判断连接是否设置成功，失败则打印错误信息*/
    while ( !Uart9_Receive_Flag )
     {
    for(i = 0; i <= timeout; i++)
       {
          if ( strstr( At_Rx_Buff , "OK\r\n" ) )
           {
               ESP8266_DEBUG_MSG("\r\nMQTT服务器连接成功\r\n");
               Clear_Buff();      //清除缓冲区数据
               break;
           }
          if ( strstr( At_Rx_Buff , "ERROR\r\n" ) )
           {
               ESP8266_DEBUG_MSG("\r\nMQTT服务器连接失败，请检查各项配置是否正确\r\n");
               while(1)
               {
                ESP8266_ERROR_Alarm();
               }      //LED灯警告错误，红灯闪烁
           }
           if ( i == timeout )
           {
               ESP8266_DEBUG_MSG("\r\nMQTT服务器连接超出期望时间，请检查各项配置是否正确\r\n");
               while(1)
               {
                ESP8266_ERROR_Alarm();
               }      //LED灯警告错误，红灯闪烁
           }
           R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
        }
      }


}

/*订阅主题函数*/
void Subscribes_Topics( char * topics )
{
    char  Sub_Topics_AT[256];

    sprintf( Sub_Topics_AT , "AT+MQTTSUB=0,\"%s\",1\r\n" , topics);

    ESP8266_AT_Send( Sub_Topics_AT );

    while ( !Uart9_Receive_Flag )
      {
          R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);  //等待订阅时间

          if ( strstr( At_Rx_Buff , "OK" ) )
           {
               ESP8266_DEBUG_MSG("\r\n主题订阅成功\r\n");
               Clear_Buff();      //清除缓冲区数据
               break;
           }
          if ( strstr( At_Rx_Buff , "ALREADY\r\n" ) )
           {
               ESP8266_DEBUG_MSG("\r\n已经订阅过该主题\r\n");
               Clear_Buff();      //清除缓冲区数据
               break;
           }
       }
}

/*发布MQTT消息函数*/
void Send_Data( char * topics , char * data )
{
    char  Send_Data[256];

    sprintf( Send_Data , "AT+MQTTPUB=0,\"%s\",\"%s\",1,0\r\n" , topics , data );

    ESP8266_AT_Send( Send_Data );

    while ( !Uart9_Receive_Flag )
      {
          R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);  //等待发布时间

          if ( strstr( At_Rx_Buff , "OK\r\n" ) )
           {
               ESP8266_DEBUG_MSG("\r\n消息发布成功\r\n");
               Uart9_Show_Flag = true;  //打开串口回显
               Clear_Buff();      //清除缓冲区数据
               break;
           }
          if ( strstr( At_Rx_Buff , "ALREADY" ) )
           {
               ESP8266_DEBUG_MSG("\r\n消息发布失败，请检查消息格式等信息是否正确\r\n");
               Clear_Buff();      //清除缓冲区数据
               break;
           }
       }
}

/*Wifi串口回调函数*/
void esp8266_uart9_callback(uart_callback_args_t * p_args)
{
       switch(p_args->event)
       {
           case UART_EVENT_RX_CHAR:

                At_Rx_Buff[Uart9_Num++] = ( char ) p_args->data;  //将UART9收到的数据放到Buff缓冲区中

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
