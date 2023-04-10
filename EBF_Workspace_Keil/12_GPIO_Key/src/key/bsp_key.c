#include "bsp_key.h"

/* 按键初始化函数 */
void Key_Init(void)
{
    /* 初始化配置引脚（这里重复初始化了，可以注释掉） */
    //R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
}


/* 按键扫描函数（阻塞式）
 * key: KEY1_SW2_PIN 用户按键1（丝印SW2）的引脚
 *      KEY2_SW3_PIN 用户按键2（丝印SW3）的引脚
 */
uint32_t Key_Scan(bsp_io_port_pin_t key)
{
    bsp_io_level_t state;

    // 读取按键引脚电平
    R_IOPORT_PinRead(&g_ioport_ctrl, key, &state);
    if (BSP_IO_LEVEL_HIGH == state)
    {
        return KEY_OFF; //按键没有被按下
    }
    else
    {
        do  //等待按键释放
        {
            R_IOPORT_PinRead(&g_ioport_ctrl, key, &state);
        } while (BSP_IO_LEVEL_LOW == state);
    }

    return KEY_ON; //按键被按下了
}
