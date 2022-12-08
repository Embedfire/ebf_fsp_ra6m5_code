#include "bsp_gpt_timing.h"
#include "led/bsp_led.h"


/* GPT 初始化函数 */
void GPT_Timing_Init(void)
{
    /* 初始化 GPT0 模块 */
    R_GPT_Open(&g_timer_gpt0_ctrl, &g_timer_gpt0_cfg);
    
    /* 启动 GPT0 定时器 */
    R_GPT_Start(&g_timer_gpt0_ctrl);
}


/* GPT 中断回调函数 */
void gpt0_timing_callback(timer_callback_args_t * p_args)
{
    /* 定时器溢出事件 */
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        /* 翻转 LED1 */
        LED1_TOGGLE;    //每秒翻转一次
    }
}

