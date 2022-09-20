#include "bsp_debug_uart.h"
#include "led/bsp_led.h"


/* 调试串口 UART4 初始化 */
void Debug_UART4_Init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    
    err = R_SCI_UART_Open (&g_uart4_ctrl, &g_uart4_cfg);
    assert(FSP_SUCCESS == err);
}



/* 发送完成标志 */
volatile bool uart_send_complete_flag = false;


/* 串口中断回调 */
void debug_uart4_callback (uart_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        case UART_EVENT_RX_CHAR:
        {
            /* 根据字符指令控制RGB彩灯颜色 */
            switch (p_args->data)
            {
                case '1':
                    LED1_ON;
                    break;
                case '2':
                    LED2_ON;
                    break;
                case '3':
                    LED3_ON;
                    break;
                case '4':
                    LED1_OFF;
                    break;
                case '5':
                    LED2_OFF;
                    break;
                case '6':
                    LED3_OFF;
                    break;
                case '7':
                    LED1_ON; LED2_ON; LED3_ON;
                    break;
                case '8':
                    LED1_OFF; LED2_OFF; LED3_OFF;
                    break;
                default:
                    break;
            }
            break;
        }
        case UART_EVENT_TX_DATA_EMPTY:
        {
            uart_send_complete_flag = true;
            break;
        }
    }
}


/* 重定向 printf 输出 */
#if defined __GNUC__ && !defined __clang__
int _write(int fd, char *pBuffer, int size)
{
    R_SCI_UART_Write(&g_uart4_ctrl, (uint8_t *)pBuffer, (uint32_t)size);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;

    return size;
}
#else
int fputc(int ch, FILE *f)
{
    R_SCI_UART_Write(&g_uart4_ctrl, (uint8_t *)&ch, 1);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;

    return ch;
}
#endif





