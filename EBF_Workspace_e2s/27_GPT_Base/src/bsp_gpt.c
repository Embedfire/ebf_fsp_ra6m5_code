 /******************************************************************************
 * @file    bsp_gpt.c
 * @author  fire
 * @version V1.0
 * @date    2022-7-25
 * @brief   GPT初始化和中断回调函数
 ******************************************************************************
 * @attention
 *
 * 实验平台 : RA6M5
 * 公司     :http://www.embedfire.com
 * 论坛     :http://www.firebbs.cn
 * 淘宝     :http://firestm32.taobao.com
 *
 ******************************************************************************/

#include "bsp_gpt.h"
#include "bsp_debug_uart.h"
#include "bsp_pin.h"


/**
 * @brief 初始化
 * @param NONE
 * @return NONE
 */
void GPT_Init(void)
{
    /*打开GPT，初始化寄存器配置*/
	R_GPT_Open(GPT.p_ctrl,GPT.p_cfg);
	/*启动GPT计时*/
	R_GPT_Start(GPT.p_ctrl);
}



/**
 * @brief GPT中断回调函数
 * @param p_args
 * @return NONE
 */
void GPT_Callback(timer_callback_args_t * p_args)
{
	switch(p_args->event)
	{
		case TIMER_EVENT_CYCLE_END:
		    PIN_TOGGLE(LED_G);
			break;
		default:
			break;
	}
}
