#include "bsp_dtc.h"
#include "bsp_led.h"
#include "bsp_debug_uart.h"

void RTC_Init(void)
{
   //��ʼ��ʱ�趨��ʱ��
   rtc_time_t set_time =
   { .tm_sec = RTC_SEC_SET,  //��
     .tm_min = RTC_MIN_SET,  //��
     .tm_hour = RTC_HOUR_SET,  //Сʱ
     .tm_mday = RTC_MDAY_SET,  //�գ�һ�����У�
     .tm_wday = RTC_WDAY_SET,   //����
     .tm_mon = RTC_MON_SET,   //�·�
     .tm_year = RTC_YEAR_SET-1900, //��ݣ��������2022������������2022-1900=122��
   };
   /*��RTCģ��*/
   R_RTC_Open (RTC.p_ctrl, RTC.p_cfg);
   
   /*ʱ��Դ���ã������FSP Configuration����"Set Source Clock in Open"Ϊ"enabled"������һ�����Ա�����*/
   R_RTC_ClockSourceSet (RTC.p_ctrl);
   
   /*��RTCʱ���Ѿ�ʹ��Ŧ�۵�ع�����һ��ʱ�䣬�����ʹ�����������ȡ��ǰ���������õ�ǰʱ��*/
   //R_RTC_CalendarTimeGet(RTC.p_ctrl,&set_time);
   
   /*����������ٵ���һ��������RTC*/
   R_RTC_CalendarTimeSet (RTC.p_ctrl, &set_time); //���õ�ǰʱ��
   
   /*���������жϵ�����Ϊ1��*/
   R_RTC_PeriodicIrqRateSet (RTC.p_ctrl, RTC_PERIODIC_IRQ_SELECT_1_SECOND);
}

void rtc_callback(rtc_callback_args_t *p_args)
{
      static rtc_time_t get_time;
      switch (p_args->event)
      {
         /*���������жϣ��������ڸ����ڲ��л�LED��ƽ*/
         case RTC_EVENT_PERIODIC_IRQ:
            LED1_TOGGLE; //��תLED
         
            /*��ȡ��ǰʱ��*/
            R_RTC_CalendarTimeGet (RTC.p_ctrl, &get_time);
         
            /*��ӡ��ǰʱ��*/
            printf("\r\n%d-%d-%d-%d:%d:%d\r\n", get_time.tm_year + 1900, get_time.tm_mon, get_time.tm_mday,
                     get_time.tm_hour, get_time.tm_min, get_time.tm_sec);
            break;
         default:
            break;
      }
}