#include "key.h"
#include "bsp_led.h"

/*设置标志位*/
volatile bool key_flag = false;

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

/*按键1中断回调函数，用于进入深度睡眠或唤醒*/
void Key1_IRQ_Callback(external_irq_callback_args_t *p_args)
{
    (void)(p_args);
		key_flag = true;
}

/*按键2中断回调函数，用于唤醒*/
void Key2_IRQ_Callback(external_irq_callback_args_t *p_args)
{
    (void)(p_args);
}
