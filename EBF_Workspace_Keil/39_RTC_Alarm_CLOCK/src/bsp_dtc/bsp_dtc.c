#include "bsp_dtc.h"
#include "bsp_led.h"
#include "bsp_debug_uart.h"

void RTC_Alarm_Init(void)
{
    rtc_alarm_time_t alarm_time =
    {
        .time = {
                .tm_sec = RTC_ALARM_SEC_SET,  //秒
                .tm_min = RTC_ALARM_MIN_SET,  //分
                .tm_hour = RTC_ALARM_HOUR_SET,  //小时
                .tm_mday = RTC_ALARM_MDAY_SET,  //日（一个月中）
                .tm_wday = RTC_ALARM_WDAY_SET,   //星期
                .tm_mon = RTC_ALARM_MON_SET,   //月份
                .tm_year = RTC_ALARM_YEAR_SET-1900, //年份
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
   //初始化时设定的时间
   rtc_time_t set_time =
   { .tm_sec = RTC_SEC_SET,  //秒
     .tm_min = RTC_MIN_SET,  //分
     .tm_hour = RTC_HOUR_SET,  //小时
     .tm_mday = RTC_MDAY_SET,  //日（一个月中）
     .tm_wday = RTC_WDAY_SET,   //星期
     .tm_mon = RTC_MON_SET - 1,   //月份
     .tm_year = RTC_YEAR_SET-1900, //年份（如今年是2008，则这里输入2008-1900=108）
   };
   /*打开RTC模块*/
   R_RTC_Open (rtc.p_ctrl, rtc.p_cfg);
   /*时钟源设置，如果在FSP Configuration设置"Set Source Clock in Open"为"enabled"，那这一步可以被跳过*/
   R_RTC_ClockSourceSet (rtc.p_ctrl);
   /*若RTC时钟已经使用纽扣电池工作了一段时间，则可以使用这个函数获取当前日历并设置当前时间*/
   //R_RTC_CalendarTimeGet(rtc.p_ctrl,&set_time);
   /*这个函数至少调用一次以启动RTC*/
   R_RTC_CalendarTimeSet (rtc.p_ctrl, &set_time); //设置当前时间
   /*设置周期中断的周期为1秒*/
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
        /*若是闹钟中断，则发送日期给串口并切换LED电平并让蜂鸣器叫一声*/
        case RTC_EVENT_ALARM_IRQ:
            /*获取当前时间*/
            R_RTC_CalendarTimeGet (rtc.p_ctrl, &get_time);
            /*打印当前时间*/
            printf ("\r\n%d-%d-%d-%d:%d:%d\r\n", get_time.tm_year + 1900, get_time.tm_mon + 1, get_time.tm_mday,
                                get_time.tm_hour, get_time.tm_min, get_time.tm_sec);
        
            LED1_TOGGLE; //反转LED
        
            Buzzer_sout(); //蜂鸣器叫一声
        
            break;
        default:
            break;
    }
}