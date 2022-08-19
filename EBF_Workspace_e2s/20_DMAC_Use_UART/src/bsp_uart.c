/**
  ******************************************************************************
  * @file    bsp_uart.c
  * @author  fire
  * @version V1.0
  * @date    2022-07-xx
  * @brief   串口处理文件
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火_瑞萨启明6M5开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "bsp_uart.h"
#include "bsp_dmac.h"


/*
 * UART API 接口
 *
 * const uart_api_t g_uart_on_sci =
 * {
 *     .open               = R_SCI_UART_Open,
 *     .close              = R_SCI_UART_Close,
 *     .write              = R_SCI_UART_Write,
 *     .read               = R_SCI_UART_Read,
 *     .infoGet            = R_SCI_UART_InfoGet,
 *     .baudSet            = R_SCI_UART_BaudSet,
 *     .communicationAbort = R_SCI_UART_Abort,
 *     .callbackSet        = R_SCI_UART_CallbackSet,
 *     .readStop           = R_SCI_UART_ReadStop,
 * };
 */


/* 开启串口4 */
void bsp_uart4_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    //设置波特率
//    r_sci_uart_baud_example(115200, 5);

    err = g_uart_on_sci.open(g_uart4.p_ctrl, g_uart4.p_cfg); //开启串口4
    if(FSP_SUCCESS != err){
        __BKPT(1);
    }
}


//设置波特率
//#define SCI_UART_BAUDRATE_19200              (19200)
//#define SCI_UART_BAUDRATE_ERROR_PERCENT_5    (5000)
void r_sci_uart_baud_example(uint32_t _sci_uart_baudrate, uint32_t _error_percent)
{
    baud_setting_t baud_setting;
    uint32_t baud_rate = _sci_uart_baudrate;
    bool enable_bitrate_modulation = false;
    uint32_t error_rate_x_1000 = _error_percent * 1000;
    fsp_err_t err;

    err = R_SCI_UART_BaudCalculate(baud_rate, enable_bitrate_modulation, error_rate_x_1000, &baud_setting);
    assert(FSP_SUCCESS == err);
    err = R_SCI_UART_BaudSet(&g_uart4_ctrl, (void *) &baud_setting);
    assert(FSP_SUCCESS == err);
}



/* 串口4的回调函数 */
volatile uint8_t uart_cpu_tx_complete_flag = 0;
volatile uint8_t uart_cpu_rx_complete_flag = 0;
volatile uint32_t uart_cpu_rx_count = 0;
void g_uart4_callback(uart_callback_args_t *p_args)
{
    switch ((uint32_t)p_args->event) {
    case UART_EVENT_RX_COMPLETE:  // 接收完成事件
    {
        uart_cpu_rx_complete_flag = 1;
        break;
    }
    case UART_EVENT_TX_COMPLETE:  // 发送完成事件
    {
        uart_cpu_tx_complete_flag = 1;
        break;
    }
    case UART_EVENT_RX_CHAR:  ///< Character received 接收到的字符
    {
//        g_uart_on_sci.write(g_uart4.p_ctrl, (uint8_t *)&(p_args->data), 1);
        uart_cpu_rx_count++;
        break;
    }
    case UART_EVENT_ERR_PARITY:  ///< 奇偶校验错误事件
    {
        break;
    }
    case UART_EVENT_ERR_FRAMING:  ///< 模式故障错误事件
    {
        break;
    }
    case UART_EVENT_ERR_OVERFLOW:  ///< FIFO溢出错误事件
    {
        break;
    }
    case UART_EVENT_BREAK_DETECT:  ///< 中断检测错误事件
    {
        break;
    }
    case UART_EVENT_TX_DATA_EMPTY:  ///< Last byte is transmitting, ready for more data
    {
        break;
    }
    }
}


