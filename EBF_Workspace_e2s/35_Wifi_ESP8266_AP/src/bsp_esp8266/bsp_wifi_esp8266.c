#include <bsp_esp8266/bsp_wifi_esp8266.h>
#include "debug_uart/bsp_debug_uart.h"


_Bool               Uart9_Receive_Flag = false; //用来判断UART9接收以及发送数据是否完成
_Bool               Uart9_Show_Flag = false;    //控制UART9收发数据显示标志

/*用来当作接收UART9数据的缓冲区*/
char                At_Rx_Buff[256];
uint8_t             Uart9_Num = 0;

/*自动配置ESP8266函数*/
void ESP8266_AP_Test(void)
{

    ESP8266_DEBUG_MSG("\r\n正在初始化ESP8266...\r\n");
    ESP8266_UART9_Init();


    ESP8266_DEBUG_MSG("\r\n设置AP模式中...\r\n");
    ESP8266_AP();
    ESP8266_Rst();

    ESP8266_DEBUG_MSG("\r\n正在配置Wifi中...\r\n");
    ESP8266_AP_SetAP( ID , PASSWORD , CHANNEL , ENCRY_MODE );
    Link_Mode( 1 );

    ESP8266_DEBUG_MSG("\r\n正在建立服务器中...\r\n");
    ESP8266_AP_SetServer( SERVER_MODE , SERVER_PORT );

    ESP8266_DEBUG_MSG("\r\n等待连接...\r\n");
    Query_Address();  //提示配置信息

}


/*ESP8266 (SPI9 UART) 初始化*/
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

/*重启ESP8266函数*/
void ESP8266_Rst(void)
{
    ESP8266_AT_Send ( "AT+RST\r\n" );

    /*等待设置完成*/
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

/*ESP8266配置热点信息函数*/
void ESP8266_AP_SetAP( char * id ,  char * password , char * channel_num , char * encry_mode )
{
    char  SetAP_AT[256];

    sprintf( SetAP_AT , "AT+CWSAP=\"%s\",\"%s\",%s,%s\r\n" , id , password , channel_num , encry_mode );

    ESP8266_AT_Send( SetAP_AT );

    /*判断是否设置成功*/
    while ( !Uart9_Receive_Flag )
    {
         if (strstr( At_Rx_Buff , "OK\r\n" ))
         {
         ESP8266_DEBUG_MSG("\r\nESP8266已配置好Wifi\r\n");
         Clear_Buff();      //清除缓冲区数据
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

/*建立/关闭 TCP 或 SSL 服务器*/
void ESP8266_AP_SetServer( char * mode ,  char * port )
{
    char  SetServer_AT[256];

    sprintf( SetServer_AT , "AT+CIPSERVER=%s,%s\r\n" , mode , port );

    ESP8266_AT_Send( SetServer_AT );

    /*等待设置完成*/
    while ( !Uart9_Receive_Flag )
    {
         if (strstr( At_Rx_Buff , "OK\r\n" ))
         {
         ESP8266_DEBUG_MSG("\r\nESP8266已配置好服务器信息\r\n");
         Clear_Buff();      //清除缓冲区数据
         }
    }

}

/*获取打印ESP8266的AP配置信息函数*/
void Query_Address(void)
{
    /*获取IP/MAC地址*/
    ESP8266_AT_Send ( "AT+CIFSR\r\n" );

    const char s[1] = "\"";
    char * AP_ID = 0;

    while ( !Uart9_Receive_Flag )
    {
        if (strstr( At_Rx_Buff , "OK\r\n" ))
        {
        AP_ID = strtok( At_Rx_Buff , s ); //获取IP地址
        AP_ID = strtok( NULL , s );
        }
    }
    ESP8266_DEBUG_MSG("\nWifi名称为  '%s'\n",ID);
    ESP8266_DEBUG_MSG("Wifi密码为  '%s'\n",PASSWORD);

    ESP8266_DEBUG_MSG("\nIP地址为  '%s'\n",AP_ID);
    ESP8266_DEBUG_MSG("服务器端口号为  '%s'\n", SERVER_PORT);

    Clear_Buff();      //清除缓冲区数据

    Uart9_Show_Flag = true;
}

/*Wifi串口回调函数*/
void esp8266_uart9_callback(uart_callback_args_t * p_args)
{
       switch(p_args->event)
       {
           case UART_EVENT_RX_CHAR:

               At_Rx_Buff[Uart9_Num++] = ( char ) p_args->data;  //将UART9收到的数据放到Buff缓冲区中

                /*普通传输模式时打开串口调试助手收发数据显示*/
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

