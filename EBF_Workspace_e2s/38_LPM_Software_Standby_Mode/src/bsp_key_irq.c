/**
 ******************************************************************************
 * @file    bsp_key_irq.c
 * @author  fire
 * @version V1.0
 * @date    2022-7-25
 * @brief   外部中断初始化，外部中断回调函数
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
#include "bsp_key_irq.h"


/**
 * @brief 外部中断初始化
 * @param NONE
 * @return NONE
 */
void Key_Init(void)
{
    R_ICU_ExternalIrqOpen(Key1_IRQ.p_ctrl, Key1_IRQ.p_cfg);
    R_ICU_ExternalIrqOpen(Key2_IRQ.p_ctrl, Key2_IRQ.p_cfg);
    R_ICU_ExternalIrqEnable(Key1_IRQ.p_ctrl);
    R_ICU_ExternalIrqEnable(Key2_IRQ.p_ctrl);
}

/**
 * @brief 只是用于唤醒MCU，不需要写下代码
 * @param external_irq_callback_args_t *p_args
 * @return NONE
 */
void Key1_IRQ_Callback(external_irq_callback_args_t *p_args)
{
    (void)p_args; //防止编译器警告
}

/**
 * @brief 只是用于唤醒MCU，不需要写下代码
 * @param external_irq_callback_args_t *p_args
 * @return NONE
 */
void Key2_IRQ_Callback(external_irq_callback_args_t *p_args)
{
    (void)p_args; //防止编译器警告
}
