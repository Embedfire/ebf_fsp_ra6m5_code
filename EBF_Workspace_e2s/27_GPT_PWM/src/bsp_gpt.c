/**
 ******************************************************************************
 * @file    bsp_gpt.c
 * @author  fire
 * @version V1.0
 * @date    2022-7-25
 * @brief   GPT初始化和PWM占空比设置
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
    /*打开GPT，初始化寄存器配置*/
    R_GPT_Open (GPT_PWM.p_ctrl, GPT_PWM.p_cfg);
    /*启动GPT计时*/
    R_GPT_Start (GPT_PWM.p_ctrl);
}


/**
 * @brief 设置PWM的占空比
 * @param GPT_p_ctrl GPT控制句柄
 * @param duty_cycle PWM占空比
 * @return NONE
 */
void GPT_PWM_Duty_Cycle_Set(timer_ctrl_t * const GPT_p_ctrl, uint32_t duty_cycle)
{
    timer_info_t info;
    /*获得GPT的信息*/
    R_GPT_InfoGet (GPT_p_ctrl, &info);
    /*获得计时器一个周期需要的计数次数*/
    uint32_t current_period_counts = info.period_counts;
    /*根据占空比和一个周期的计数次数计算GTCCR的值*/
    uint32_t duty_cycle_counts = (uint32_t)(((uint64_t) current_period_counts * duty_cycle) / 100);
    /*设置GTCCR的值*/
    R_GPT_DutyCycleSet(GPT_p_ctrl, duty_cycle_counts, LED_R_PWM);
}
