#include "bsp_agt_pulse_period_measurement.h"
#include "stdio.h"


/*  当前选用输入捕获引脚  AGTIO3  P600 */


timer_info_t info;  //用于获取定时器参数信息
uint32_t period;    //用于保存计数器的计数周期

uint32_t pulse_period_time;         //测量的脉冲周期时间
volatile uint8_t print_enble_flag;  //允许打印测量结果标志


/* AGT初始化函数 */
void AGT_Pulse_Period_Measurement_Init(void)
{
    /* 初始化 AGT 模块 */
    R_AGT_Open(&g_timer_agt3_ctrl, &g_timer_agt3_cfg);
    
    /* 获取当前参数 */
    (void) R_AGT_InfoGet(&g_timer_agt3_ctrl, &info);
    /* 获取计数周期：AGT的一个周期的计数次数 */
    period = info.period_counts;
    
    /* 使能 AGT 定时器 */
	R_AGT_Enable(&g_timer_agt3_ctrl);   //注：对于脉冲宽度/周期测量模式下，使用 R_AGT_Enable 或 R_AGT_Start 函数效果是一致的
    
    /* 启动 AGT 定时器 */
    //R_AGT_Start(&g_timer_agt3_ctrl);
}


/* AGT 脉冲周期测量中断回调函数 */
void agt3_pulse_period_measurement_callback(timer_callback_args_t * p_args)
{
    static uint32_t overflow_times = 0; //计数器溢出次数

    /* 测量完成事件 */
    if (TIMER_EVENT_CAPTURE_A == p_args->event)
    {
        pulse_period_time = p_args->capture + overflow_times * period; //记录时间 B
        overflow_times = 0;
        
        print_enble_flag = 1;   //测量完成后 允许打印测量结果
    }
    /* 定时器计数溢出事件 */
    else if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        /* 输入捕获期间计数器溢出，则记录溢出次数+1 */
        overflow_times++;
    }
}
