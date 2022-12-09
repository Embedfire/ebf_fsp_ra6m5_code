#include "key.h"
#include "bsp_led.h"

void IRQ_Init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /*开启外部中断*/
    err = g_external_irq_on_icu.open(&sw2_external_irq_ctrl, &sw2_external_irq_cfg);
    assert(FSP_SUCCESS == err);
    err = g_external_irq_on_icu.enable(&sw2_external_irq_ctrl);
    assert(FSP_SUCCESS == err);
}

/*按键中断回调函数*/
void Key1_IRQ_Callback(external_irq_callback_args_t *p_args)
{
    (void)(p_args);
}
