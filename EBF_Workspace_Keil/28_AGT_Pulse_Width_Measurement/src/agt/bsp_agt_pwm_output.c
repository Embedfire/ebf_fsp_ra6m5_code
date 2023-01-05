#include "bsp_agt_pwm_output.h"


/*  当前选用PWM输出引脚 AGTOA0  P500 */


/* AGT初始化函数 */
void AGT_PWM_Init(void)
{
    /* 初始化 AGT 模块 */
    R_AGT_Open(&g_timer_agt0_ctrl, &g_timer_agt0_cfg);
    
    /* 启动 AGT 定时器 */
    R_AGT_Start(&g_timer_agt0_ctrl);
    
    /* 重新设置占空比为 80% */
    AGT_PWM_SetDuty(80);
}


/** 自定义函数：设置PWM占空比
    @param duty 占空比范围：0~100 %
*/
void AGT_PWM_SetDuty(uint8_t duty)
{
    timer_info_t info;
    uint32_t current_period_counts;
    uint32_t duty_cycle_counts;

    if (duty > 100)
        duty = 100; //限制占空比范围：0~100

    /* 获得AGT的信息 */
    R_AGT_InfoGet(&g_timer_agt0_ctrl, &info);
    
    /* 获得计时器一个周期需要的计数次数 */
    current_period_counts = info.period_counts;
    
    /* 根据占空比和一个周期的计数次数计算比较匹配寄存器的值 */
    duty_cycle_counts = (uint32_t)(((uint64_t) current_period_counts * duty) / 100);

    /* 最后调用FSP库函数设置占空比 */
    R_AGT_DutyCycleSet(&g_timer_agt0_ctrl, duty_cycle_counts, AGT_OUTPUT_PIN_AGTOA);
}
