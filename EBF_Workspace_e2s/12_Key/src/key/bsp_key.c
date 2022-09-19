#include "bsp_key.h"
#include "led/bsp_led.h"

void KEY_Init(void)
{
    /* 初始化配置引脚（这里重复初始化了，可以注释掉） */
    //R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
}

uint32_t Key_read(bsp_io_port_pin_t key)
{
    bsp_io_level_t state;
    R_IOPORT_PinRead(&g_ioport_ctrl, key, &state);
    return state;
}


void Key_SW2_test(void)
{
    if(Key_read(Key_1) == 0)
    {
        LED1_ON;
    }else{
        LED1_OFF;
    }
}

void Key_SW3_test(void)
{
    if(Key_read(Key_2) == 0)
    {
        LED2_ON;
    }else{
        LED2_OFF;
    }
}
