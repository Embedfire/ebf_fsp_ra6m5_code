#include "bsp_agt_timing.h"
#include "led/bsp_led.h"


/* AGT初始化函数 */
void AGT_Timing_Init(void)
{
    /* 初始化 AGT 模块 */
    R_AGT_Open(&g_timer_agt0_ctrl, &g_timer_agt0_cfg);
    
    /* 启动 AGT 定时器 */
    R_AGT_Start(&g_timer_agt0_ctrl);
}


/* 定时器溢出 中断回调函数 */
void agt0_timing_callback(timer_callback_args_t * p_args)
{
    if (TIMER_EVENT_CYCLE_END == p_args->event)
    {
        /* 翻转 LED1 */
        LED1_TOGGLE;    //每秒翻转一次
    }
}

