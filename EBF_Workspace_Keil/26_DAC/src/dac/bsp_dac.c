#include "bsp_dac.h"


//正弦波数据数组变量
uint16_t var[] = {
    2048, 2460, 2856, 3218, 3532, 3786, 3969, 4072, 4093, 4031, 3887, 3668,
    3382, 3042, 2661, 2255, 1841, 1435, 1054, 714, 428, 209, 65, 3, 24, 127,
    310, 564, 878, 1240, 1636, 2048
};


/**
 * @brief  初始化DAC
 * @param  无
 * @retval 无
 */
void DAC_Init()
{
    R_DAC_Open(&g_dac0_ctrl, &g_dac0_cfg);
    R_DAC_Start(&g_dac0_ctrl);
}


/**
 * @brief  设置当前的电压
 * @param  需要控制的电压
 * @retval 无
 */
void DAC_SetVoltage(float voltage)
{
    uint16_t dac_data;
    dac_data = (uint16_t)(4095*((voltage)/3.3f));
    R_DAC_Write(&g_dac0_ctrl, dac_data);
}


/**
 * @brief  生成正弦波波形
 * @param  输入的值可以调节波形的周期
 * @retval 无
 */
void DAC_SinWave_Cycle(uint32_t time_interval)
{
    for(uint32_t i = 0 ; i < (sizeof(var)/sizeof(var[0])); i++)
    {
        R_DAC_Write(&g_dac0_ctrl, var[i]);
        R_BSP_SoftwareDelay(time_interval, BSP_DELAY_UNITS_MILLISECONDS);
    }
}


