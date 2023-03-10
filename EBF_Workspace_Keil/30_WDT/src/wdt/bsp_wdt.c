#include "bsp_wdt.h"
#include "led/bsp_led.h"

/*初始化看门狗并启动计数器*/
void WDT_Init(void)
{
      //如果使用J-Link调试器进行调试的话需要加上这一句话
      R_DEBUG->DBGSTOPCR_b.DBGSTOP_WDT = 0;

      //初始化看门狗（WDT）模块
      R_WDT_Open(&g_wdt0_ctrl, &g_wdt0_cfg);

      /*刷新看门狗计数器，在这里的作用是初次启动寄存模式下的看门狗计数器
      *要注意，除非是在刷新允许的范围内，否则自启动模式下不应该使用该函数*/
      R_WDT_Refresh(&g_wdt0_ctrl);
}


/*喂狗*/
void WDT_Feed(void)
{
       /*喂狗，刷新递减计数器的值*/
       R_WDT_Refresh(&g_wdt0_ctrl);
}

/* 当看门狗NMI发生时中断回调 */
void wdt_callback (wdt_callback_args_t * p_args)
{
      /*防止编译器产生关于函数中没有使用形参的警告*/
      (void) p_args;

      /*蓝色LED亮3秒，请注意，在这里LED灯函数为代指，
       *实际应用中，这里应该放最重要的函数，比如保存重要数据等*/
      LED2_ON;
      R_BSP_SoftwareDelay(3, BSP_DELAY_UNITS_SECONDS);

      /* 通过软件复位MCU*/
      __NVIC_SystemReset();
}


