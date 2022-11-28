#include "bsp_debug_wifi.h"
#include "debug_uart/bsp_debug_uart.h"


/*Wifi (SPI9 UART) 初始化*/
void Debug_WIFI_Init(void)
{
    R_SCI_UART_Open(g_wifi.p_ctrl, g_wifi.p_cfg);
}


/*Wifi串口回调函数*/
void debug_wifi_callback(uart_callback_args_t * p_args)
{
       switch(p_args->event)
       {
           case UART_EVENT_RX_CHAR:  //收到AT的数指令后打印回显信息
               R_SCI_UART_Write(&g_uart4_ctrl, (uint8_t *)&(p_args->data), 1);
               break;
           default:
               break;
       }
}
