/**
 ******************************************************************************
 * @file    RS485_1.c
 * @author  fire
 * @version V1.0
 * @date    2022-7-25
 * @brief   RS485接口1的初始化函数和中断回调函数
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


#include "RS485_1.h"


/**
 * @brief  RS485_1初始化
 * @retval 无
 * @param  无
 */
void RS485_1_Init(void)
{
    R_SCI_UART_Open(RS485_1.p_ctrl, RS485_1.p_cfg);
}


/**
 * @brief  RS485_1回调函数
 * @retval 无
 * @param  uart_callback_args_t *p_args
 */
void RS485_1_Callback(uart_callback_args_t * p_args)
{
    switch(p_args->event)
    {
        case UART_EVENT_RX_CHAR:  //收到RS485_2的数据后发到Debug串口
            R_SCI_UART_Write(Debug_UART.p_ctrl, (uint8_t *)&p_args->data, 1);
            break;
        default:
            break;
    }
}
