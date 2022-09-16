/**
 ******************************************************************************
 * @file    bsp_SysTick.c
 * @author  fire
 * @version V1.0
 * @date    2022-7-13
 * @brief   SystemTick 系统滴答计时器的配置（可配置中断周期）和使用滴答计时器的延时函数
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

#include "bsp_SysTick.h"

static __IO uint32_t IT_Nums;  //延时需要触发中断的次数
static uint32_t IT_Period;  //中断周期(单位为时钟节拍数)
/**
 * @brief  启动系统滴答计时器 SysTick
 * @param  IT_frequency: 滴答计时器每秒的中断次数
 * @retval 无
 */
void SysTick_Init(uint32_t IT_frequency)
{
    /* SystemFrequency在这里默认为200M
     * SystemFrequency / 1000    1ms中断一次
     * SystemFrequency / 100000  10us中断一次
     * SystemFrequency / 1000000 1us中断一次
     */
    IT_Period = SystemCoreClock / IT_frequency;
    uint32_t err = SysTick_Config (IT_Period);
    assert(err == 0); //capture error
}

/**
 * @brief  延时程序
 * @param  delay: 延时的单位时间
 * @param  unit: 延时的单位
 * @retval 无
 */

void SysTick_Delay(uint32_t delay, sys_delay_units_t unit)
{
    uint32_t Sum_Time = delay * unit; //计算总延时时间(单位为时钟节拍数)
    IT_Nums = Sum_Time / IT_Period;     //计算进入中断的次数
    SysTick->VAL = 0UL;   //计数清零
    while (IT_Nums != 0)
        ;
}

/**
 * @brief  SysTick的中断服务函数
 * @param  无
 * @retval 无
 * @attention 需要先extern声明一下避免编译器警告
 */
extern void SysTick_Handler(void);
void SysTick_Handler(void)
{
    if (IT_Nums != 0x00)
    {
        IT_Nums--;
    }
}

/*********************************************END OF FILE**********************/

