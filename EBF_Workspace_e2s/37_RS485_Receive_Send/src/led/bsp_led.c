#include "bsp_led.h"

/* LED初始化函数 */
void IO_Init(void)
{
    R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
}
