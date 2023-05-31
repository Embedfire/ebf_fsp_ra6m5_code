#include "bsp_dtc.h"
#include "bsp_led.h"
#include "bsp_debug_uart.h"

void RTC_Alarm_Init(void)
{
    rtc_alarm_time_t alarm_time =
    {
        .time = {
                .tm_sec = RTC_ALARM_SEC_SET,  //��
                .tm_min = RTC_ALARM_MIN_SET,  //��
                .tm_hour = RTC_ALARM_HOUR_SET,  //Сʱ
                .tm_mday = RTC_ALARM_MDAY_SET,  //�գ�һ�����У�
                .tm_wday = RTC_ALARM_WDAY_SET,   //����
                .tm_mon = RTC_ALARM_MON_SET,   //�·�
                .tm_year = RTC_ALARM_YEAR_SET-1900, //���
        },
        .dayofweek_match = RTC_ALARM_WDAY_ENABLE,
        .hour_match = RTC_ALARM_HOUR_ENABLE,
        .mday_match = RTC_ALARM_MDAY_ENABLE,
        .min_match = RTC_ALARM_MIN_ENABLE,
        .mon_match = RTC_ALARM_MON_ENABLE,
        .sec_match = RTC_ALARM_SEC_ENABLE,
        .year_match = RTC_ALARM_YEAR_ENABLE,
    };
    R_RTC_CalendarAlarmSet(rtc.p_ctrl, &alarm_time);
}

void RTC_Init(void)
{
   //��ʼ��ʱ�趨��ʱ��
   rtc_time_t set_time =
   { .tm_sec = RTC_SEC_SET,  //��
     .tm_min = RTC_MIN_SET,  //��
     .tm_hour = RTC_HOUR_SET,  //Сʱ
     .tm_mday = RTC_MDAY_SET,  //�գ�һ�����У�
     .tm_wday = RTC_WDAY_SET,   //����
     .tm_mon = RTC_MON_SET - 1,   //�·�
     .tm_year = RTC_YEAR_SET-1900, //��ݣ��������2008������������2008-1900=108��
   };
   /*��RTCģ��*/
   R_RTC_Open (rtc.p_ctrl, rtc.p_cfg);
   /*ʱ��Դ���ã������FSP Configuration����"Set Source Clock in Open"Ϊ"enabled"������һ�����Ա�����*/
   R_RTC_ClockSourceSet (rtc.p_ctrl);
   /*��RTCʱ���Ѿ�ʹ��Ŧ�۵�ع�����һ��ʱ�䣬�����ʹ�����������ȡ��ǰ���������õ�ǰʱ��*/
   //R_RTC_CalendarTimeGet(rtc.p_ctrl,&set_time);
   /*����������ٵ���һ��������RTC*/
   R_RTC_CalendarTimeSet (rtc.p_ctrl, &set_time); //���õ�ǰʱ��
   /*���������жϵ�����Ϊ1��*/
   R_RTC_PeriodicIrqRateSet (rtc.p_ctrl, RTC_PERIODIC_IRQ_SELECT_1_SECOND);
}


void rtc_callback(rtc_callback_args_t *p_args)
{
    static rtc_time_t get_time;
    switch (p_args->event)
    {
        case RTC_EVENT_PERIODIC_IRQ:
			R_RTC_CalendarTimeGet (rtc.p_ctrl, &get_time);
            break;
        /*���������жϣ��������ڸ����ڲ��л�LED��ƽ���÷�������һ��*/
        case RTC_EVENT_ALARM_IRQ:
            /*��ȡ��ǰʱ��*/
            R_RTC_CalendarTimeGet (rtc.p_ctrl, &get_time);
            /*��ӡ��ǰʱ��*/
            printf ("\r\n%d-%d-%d-%d:%d:%d\r\n", get_time.tm_year + 1900, get_time.tm_mon + 1, get_time.tm_mday,
                                get_time.tm_hour, get_time.tm_min, get_time.tm_sec);
        
            LED1_TOGGLE; //��תLED
        
            Buzzer_sout(); //��������һ��
        
            break;
        default:
            break;
    }
}