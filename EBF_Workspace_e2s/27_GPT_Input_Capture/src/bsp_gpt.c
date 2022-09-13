/**
 ******************************************************************************
 * @file    bsp_agt.c
 * @author  fire
 * @version V1.0
 * @date    2022-7-25
 * @brief   agt初始化和中断回调函数
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

#include "bsp_gpt.h"
#include "bsp_debug_uart.h"
#include "bsp_pin.h"


/**
 * @brief 初始化AGT时钟
 * @param NONE
 * @return NONE
 */
void GPT_Init(void)
{
    /*打开GPT时钟*/
	R_GPT_Open(GPT_Input_Capture.p_ctrl,GPT_Input_Capture.p_cfg);
	/*使能输入捕获*/
	R_GPT_Enable(GPT_Input_Capture.p_ctrl);
	/*打开GPT时钟*/
	R_GPT_Open(GPT_Base.p_ctrl,GPT_Base.p_cfg);
	/*启动GPT时钟*/
	R_GPT_Start(GPT_Base.p_ctrl);
}



/**
 * @brief GPT输入捕获中断回调函数
 * @param p_args
 * @return NONE
 */
void GPT_Input_Capture_Callback(timer_callback_args_t * p_args)
{
	static uint32_t first_time,seconed_time, overflow_times;
	static timer_info_t info;
	switch(p_args->event)
	{
	    /*捕获下降沿*/
		case TIMER_EVENT_CAPTURE_A:
		    /*获取下降沿捕获的时间*/
		    first_time = p_args->capture;
			break;
		/*捕获上升沿*/
		case TIMER_EVENT_CAPTURE_B:
		    /*获取GPT当前参数*/
		    (void) R_GPT_InfoGet(GPT_Input_Capture.p_ctrl, &info);
		    /*获取GPT的一个周期的计数次数*/
		    uint32_t period = info.period_counts;
		    /*计算上升沿捕获的时间*/
		    seconed_time = p_args->capture + overflow_times*period;
		    /*通过上升沿捕获的时间减去下降沿捕获的时间获取脉宽长度，再除计数器的时钟频率将单位转换为秒打印出来*/
			APP_PRINT("Low level pulse width: %f S\r\n",(float)(seconed_time - first_time)/(float)info.clock_frequency);
			/*清除溢出次数*/
			overflow_times = 0;
			break;
		case TIMER_EVENT_CYCLE_END:
		    /*再输入捕获期间计数器溢出，则反转绿色led电平并使溢出次数加一*/
		    PIN_TOGGLE(LED_G);
			overflow_times++;
			break;
		default:
			break;
	}
}


/**
 * @brief GPT中断回调函数
 * @param p_args
 * @return NONE
 */
void GPT_Base_Callback(timer_callback_args_t * p_args)
{
    switch(p_args->event)
    {
        case TIMER_EVENT_CYCLE_END:
            /*反转SIGNAL引脚电平*/
            PIN_TOGGLE(SIGNAL);
            break;
        default:
            break;
    }
}
