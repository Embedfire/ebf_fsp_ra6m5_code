#include "bsp_key.h"
#include "led/bsp_led.h"


/* 按键初始化函数 */
void KEY_Init(void)
{
    /* 初始化配置引脚（这里重复初始化了，可以注释掉） */
}


/*
 * 按键读取函数 输入引脚 返回当前引脚的状态
 * */
uint32_t R_key_read(bsp_io_port_pin_t key)
{
    bsp_io_level_t state;
    R_IOPORT_PinRead(&g_ioport_ctrl, key, &state);
    return state;
}


/*
 * 按键扫描函数
 * 当你的手松开返回KEY_ON
 */

uint32_t Key_Scan(bsp_io_port_pin_t key)
{
    if(R_key_read(key) == BSP_IO_LEVEL_HIGH)
    {
        return KEY_OFF;
    }
    else if(R_key_read(key) == BSP_IO_LEVEL_LOW)
    {
        do  //等待按键释放
        {
            R_key_read(key);
        } while (BSP_IO_LEVEL_LOW == R_key_read(key));
    }
    return KEY_ON;
}

