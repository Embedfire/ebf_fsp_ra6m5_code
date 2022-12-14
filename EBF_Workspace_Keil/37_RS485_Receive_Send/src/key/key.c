#include "key.h"
#include "rs485.h"
#include "bsp_led.h"

void IRQ_Init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /*开启外部中断*/
    err = g_external_irq_on_icu.open(&sw2_external_irq_ctrl, &sw2_external_irq_cfg);
    assert(FSP_SUCCESS == err);
    /*使能外部中断*/
    err = g_external_irq_on_icu.enable(&sw2_external_irq_ctrl);
    assert(FSP_SUCCESS == err);
}

/*按键中断回调函数*/
void external_irq_callback(external_irq_callback_args_t *p_args)
{
    (void)(p_args);
    
    /*配置RS485_1为发送模式*/
    RS485_1_TX;
    
    /*LED灯指示状态*/
    LED1_ON;
    LED3_OFF;
    
    /*发送数据*/
    RS485_Send_Example();
    
    /*等待发送完毕*/
    R_BSP_SoftwareDelay(1,BSP_DELAY_UNITS_SECONDS);

}
