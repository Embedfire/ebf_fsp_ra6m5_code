#include "key.h"
#include "led/bsp_led.h"

void IRQ_Init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /*开启外部中断*/
    err = g_external_irq_on_icu.open(&sw2_external_irq_ctrl, &sw2_external_irq_cfg);
    assert(FSP_SUCCESS == err);
    err = g_external_irq_on_icu.enable(&sw2_external_irq_ctrl);
    assert(FSP_SUCCESS == err);
	
	err = g_external_irq_on_icu.open(&sw3_external_irq_ctrl, &sw3_external_irq_cfg);
    assert(FSP_SUCCESS == err);
    err = g_external_irq_on_icu.enable(&sw3_external_irq_ctrl);
    assert(FSP_SUCCESS == err);
}

/*按键中断回调函数，用于唤醒*/
void key1_irq_callback(external_irq_callback_args_t *p_args)
{
     /*防止编译器产生没有使用形参的警告*/
    FSP_PARAMETER_NOT_USED(p_args);
}

/*按键中断回调函数，用于睡眠请求*/
void key2_irq_callback(external_irq_callback_args_t *p_args)
{
     /*防止编译器产生没有使用形参的警告*/
    FSP_PARAMETER_NOT_USED(p_args);
}
