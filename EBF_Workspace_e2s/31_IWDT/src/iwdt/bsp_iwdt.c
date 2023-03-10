#include "bsp_iwdt.h"
#include "led/bsp_led.h"


void IWDT_Init(void)
{
    /*如果是J-Link调试器的话需要加上这句话*/
    R_DEBUG->DBGSTOPCR_b.DBGSTOP_IWDT = 0;

    /*独立看门狗初始化*/
    R_IWDT_Open(&g_iwdt0_ctrl, &g_iwdt0_cfg);
}


void IWDT_Feed(void)
{
    /*刷新看门狗计数器*/
    (void) R_IWDT_Refresh(&g_iwdt0_ctrl);
}


/* 当看门狗NMI发生时中断回调 */
void iwdt_callback (wdt_callback_args_t * p_args)
{
      /*防止编译器产生关于函数中没有使用形参的警告*/
      (void) p_args;

      /*蓝色LED亮，请注意，在这里LED灯函数为代指，
       *实际应用中，这里应该放最重要的函数，比如保存重要数据等*/
      LED1_OFF;
      LED2_ON;
      R_BSP_SoftwareDelay(3, BSP_DELAY_UNITS_SECONDS);

      /* 通过软件复位MCU*/
      __NVIC_SystemReset();
}


