/**
 ******************************************************************************
 * @file    bsp_uart.c
 * @author  fire
 * @version V1.0
 * @date    2022-7-15
 * @brief   串口初始化函数，在串口回调函数中控制LED
 ******************************************************************************
 * @attention
 *
 * 实验平台 : RA6M5
 * 公司     :http://www.embedfire.com
 * 论坛     :http://www.firebbs.cn
 * 淘宝     :http://firestm32.taobao.com
 *
 ******************************************************************************
 */

#include "bsp_uart.h"
#include "stdio.h"
#include "bsp_led.h"


static volatile bool uart_send_complete_flag = false; //用于判断printf是否发送完毕，为了防止编译器优化，建议加上volatile，其他flag同理

volatile uint8_t Show_Message_flag = 0;

/**
 * @brief  打印指令输入提示信息
 * @param  无
 * @retval 无
 */
void Show_Message(void)
{
    printf ("\r\n  This is an experiment that control LED by serial communication");
    printf ("\r\n  The development board control RGB after receive the instruction, the instruction is as follows:");
    printf ("\r\n  Instruction   ------  Color ");
    printf ("\r\n       1        ------  Red ");
    printf ("\r\n       2        ------  Green ");
    printf ("\r\n       3        ------  Blue ");
    printf ("\r\n       4        ------  Yellow ");
    printf ("\r\n       5        ------  Purple ");
    printf ("\r\n       6        ------  Cyan ");
    printf ("\r\n       7        ------  White ");
    printf ("\r\n       8        ------  OFF \r\n");
}

/**
 * @brief   初始化串口
 * @param   无
 * @retval  无
 */
void UART_Init(void)
{
    R_SCI_UART_Open (&g_uart0_ctrl, &g_uart0_cfg); //打开串口
    Show_Message();  //打印提示
}

/**
 * @brief  串口中断回调函数，接收到指定数字后调整LED颜色
 * @param  p_args
 * @retval 无
 */
void UART_Callback(uart_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case UART_EVENT_RX_CHAR:  //若接收到数据
            /* 根据字符指令控制RGB彩灯颜色 */
            switch (p_args->data)
            {
                case '1':
                    LED_RED;
                    break;
                case '2':
                    LED_GREEN;
                    break;
                case '3':
                    LED_BLUE;
                    break;
                case '4':
                    LED_YELLOW;
                    break;
                case '5':
                    LED_PURPLE;
                    break;
                case '6':
                    LED_CYAN;
                    break;
                case '7':
                    LED_WHITE;
                    break;
                case '8':
                    LED_RGBOFF;
                    break;
                default:
                    /* 如果不是指定指令字符，打印提示信息
                     * (由于需要使用发送完成中断来进行printf打印，故不能直接在回调函数中使用printf) */
                    Show_Message_flag = true;
                    break;
            }
            break;
        case UART_EVENT_TX_COMPLETE:
            uart_send_complete_flag = true;
            break;
    default:
        break;
    }
}

//重定向printf输出
 #if defined __GNUC__ && !defined __clang__
 int _write(int fd, char *pBuffer, int size)
 {
    (void)fd;
    R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *)pBuffer, (uint32_t)size);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;
    return size;
 }
 #else
 int fputc(int ch, FILE *f)
 {
    (void)f;
    R_SCI_UART_Write(&g_uart0_ctrl, (uint8_t *)&ch, 1);
    while(uart_send_complete_flag == false);
    uart_send_complete_flag = false;
    return ch;
 }
 #endif




