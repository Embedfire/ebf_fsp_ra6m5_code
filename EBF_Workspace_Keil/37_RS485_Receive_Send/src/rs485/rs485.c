#include "rs485.h"
#include "bsp_debug_uart.h"

void RS485_1_Init(void)
{
    R_SCI_UART_Open(RS485_1.p_ctrl, RS485_1.p_cfg);

}

void RS485_Send_Example(void)
{
    /*定义要发送的变量*/
    uint8_t data = 0xff;
    /*串口写入函数*/
    R_SCI_UART_Write (&RS485_1_ctrl, (uint8_t*) &data, 1);
}

void RS485_1_Callback(uart_callback_args_t * p_args)
{
    switch(p_args->event)
    {
        case UART_EVENT_RX_CHAR:
            R_SCI_UART_Write (&g_uart4_ctrl, (uint8_t*) &p_args->data, 1);
            break;
        default:
            break;
    }
}
