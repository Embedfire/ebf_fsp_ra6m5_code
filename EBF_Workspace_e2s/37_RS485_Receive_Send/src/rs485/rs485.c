#include "rs485.h"
#include "debug_uart/bsp_debug_uart.h"

void RS485_1_Init(void)
{
    R_SCI_UART_Open(rs485_1.p_ctrl, rs485_1.p_cfg);

}

void RS485_Send_Example( uint8_t ch )
{
    /*串口写入函数*/
    R_SCI_UART_Write (&rs485_1_ctrl, (uint8_t*) &ch, 1);
}

void rs485_1_callback(uart_callback_args_t * p_args)
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
