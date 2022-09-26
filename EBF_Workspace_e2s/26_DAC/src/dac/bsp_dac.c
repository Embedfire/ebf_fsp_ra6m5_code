#include "bsp_dac.h"


//正弦波变量
int var[] = {2048, 2460, 2856, 3218, 3532, 3786, 3969, 4072, 4093, 4031, 3887, 3668,
           3382, 3042, 2661, 2255, 1841, 1435, 1054, 714, 428, 209, 65, 3, 24, 127,
           310, 564, 878, 1240, 1636, 2048};


/**
 * @brief  初始化DAC
 * @param  无
 * @retval 无
 */
void dac_Init()
{
    R_DAC_Open(DAC.p_ctrl, DAC.p_cfg);
    R_DAC_Start(DAC.p_ctrl);
}


/**
 * @brief  设置当前的电压
 * @param  需要控制的电压
 * @retval 无
 */
void dac_Set(float voltage)
{
    uint16_t dac_date;
    dac_date = (uint16_t)4095*((voltage)/3.3f);
    R_DAC_Write(DAC.p_ctrl, dac_date);
}


/**
 * @brief  生成正弦波波形
 * @param  输入的值可以调节波形的宽度
 * @retval 无
 */
void dac_Circle(uint32_t timer)
{
    for(int i = 0 ; i < (sizeof(var)/sizeof(var[0])); i++)
    {
        R_DAC_Write(DAC.p_ctrl, var[i]);
        R_BSP_SoftwareDelay(timer, BSP_DELAY_UNITS_MILLISECONDS);
    }
}


