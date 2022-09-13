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
    /*打开输入捕获GPT时钟*/
	R_GPT_Open(GPT_Input_Capture.p_ctrl,GPT_Input_Capture.p_cfg);
	/*使能输入捕获*/
	R_GPT_Enable(GPT_Input_Capture.p_ctrl);
	/*启动输入捕获GPT时钟*/
    R_GPT_Start(GPT_Input_Capture.p_ctrl);
	/*打开PWM的GPT时钟*/
	R_GPT_Open(GPT_PWM.p_ctrl, GPT_PWM.p_cfg);
	/*启动PWM的GPT时钟*/
	R_GPT_Start(GPT_PWM.p_ctrl);
}



/**
 * @brief GPT输入捕获中断回调函数
 * @param p_args
 * @return NONE
 */
void GPT_Input_Capture_Callback(timer_callback_args_t * p_args)
{
	static uint32_t a_time,a_time_; //下降沿捕获的时间
	static uint32_t b_time;  //上升沿捕获的时间
	static uint32_t overflow_times; //计时器溢出次数
	static uint32_t period;  //输入捕获计数器的周期
	static uint32_t pwm_period; //PWM的周期
	static uint32_t pwm_high_level_time; //PWN高电平的时间
	static timer_info_t info; //用于获取GPT参数
	static float pwm_fre, pwm_duty; //PWM频率和占空比
	switch(p_args->event)
	{
	    /*捕获下降沿*/
		case TIMER_EVENT_CAPTURE_A:
		    /*获取GPT当前参数*/
            (void) R_GPT_InfoGet(GPT_Input_Capture.p_ctrl, &info);
            /*获取GPT的一个周期的计数次数*/
            period = info.period_counts;
		    /*保存上一次下降沿捕获的时间，用来计算PWM周期*/
            a_time_ = a_time;
            /*获取下降沿捕获的时间*/
		    a_time = p_args->capture + overflow_times*period;
		    /*计算PWM周期*/
		    pwm_period = a_time - a_time_;
		    /*计算PWM高电平的时间，用于计算占空比*/
		    pwm_high_level_time = a_time - b_time;
		    /*清除溢出次数*/
		    overflow_times = 0;
			break;
		/*捕获上升沿*/
		case TIMER_EVENT_CAPTURE_B:
		    /*获取GPT当前参数*/
		    (void) R_GPT_InfoGet(GPT_Input_Capture.p_ctrl, &info);
		    /*获取GPT的一个周期的计数次数*/
		    period = info.period_counts;
		    /*计算上升沿捕获的时间*/
		    b_time = p_args->capture + overflow_times*period;
			/*清除溢出次数*/
			overflow_times = 0;
			break;
		case TIMER_EVENT_CYCLE_END:
		    /*再输入捕获期间计数器溢出，则反转绿色led电平并使溢出次数加一*/
		    PIN_TOGGLE(LED_G);
		    /*计算PWM的频率*/
		    pwm_fre = (float)info.clock_frequency/(float)pwm_period;
		    /*计算PWM的占空比*/
		    pwm_duty = (float)pwm_high_level_time/(float)pwm_period;
		    /*打印结果*/
		    APP_PRINT("Friquency = %f, Duty_Cycle = %f\r\n", pwm_fre, pwm_duty);
			overflow_times++;
			break;
		default:
			break;
	}
}
