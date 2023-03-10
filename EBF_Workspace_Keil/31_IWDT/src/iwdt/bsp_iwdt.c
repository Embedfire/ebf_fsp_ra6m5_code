#include "bsp_iwdt.h"
#include "led/bsp_led.h"


void IWDT_Init(void)
{
    /*�����J-Link�������Ļ���Ҫ������仰*/
    R_DEBUG->DBGSTOPCR_b.DBGSTOP_IWDT = 0;

    /*�������Ź���ʼ��*/
    R_IWDT_Open(&g_iwdt0_ctrl, &g_iwdt0_cfg);
}


void IWDT_Feed(void)
{
    /*ˢ�¿��Ź�������*/
    (void) R_IWDT_Refresh(&g_iwdt0_ctrl);
}


/* �����Ź�NMI����ʱ�жϻص� */
void iwdt_callback (wdt_callback_args_t * p_args)
{
      /*��ֹ�������������ں�����û��ʹ���βεľ���*/
      (void) p_args;

      /*��ɫLED������ע�⣬������LED�ƺ���Ϊ��ָ��
       *ʵ��Ӧ���У�����Ӧ�÷�����Ҫ�ĺ��������籣����Ҫ���ݵ�*/
      LED1_OFF;
      LED2_ON;
      R_BSP_SoftwareDelay(3, BSP_DELAY_UNITS_SECONDS);

      /* ͨ�������λMCU*/
      __NVIC_SystemReset();
}


