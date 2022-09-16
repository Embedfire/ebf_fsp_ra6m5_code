/**
 ******************************************************************************
 * @file    bsp_RTC.c
 * @author  fire
 * @version V1.0
 * @date    2022-7-25
 * @brief   RTC初始化函数和中断回调函数
 ******************************************************************************
 * @attention
 *
 * 实验平台 : RA6M5
 * 公司     :http://www.embedfire.com
 * 论坛     :http://www.firebbs.cn
 * 淘宝     :http://firestm32.taobao.com
 *
 ******************************************************************************
 */




#include <bsp_RTC.h>
#include "bsp_debug_uart.h"
#include "bsp_pin.h"

/**
 * @brief 初始化RTC闹钟
 * @param 无
 * @retval 无
 */
static void RTC_Alarm_Init(void)
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
    R_RTC_CalendarAlarmSet(RTC.p_ctrl, &alarm_time);;
}

/**
 * @brief 初始化RTC
 * @retval 无
 * @param 无
 */
void RTC_Init(void)
{
    //初始化时设定的时间
    rtc_time_t set_time =
    {
      .tm_sec = RTC_SEC_SET,  //秒
      .tm_min = RTC_MIN_SET,  //分
      .tm_hour = RTC_HOUR_SET,  //小时
      .tm_mday = RTC_MDAY_SET,  //日（一个月中）
      .tm_wday = RTC_WDAY_SET,   //星期
      .tm_mon = RTC_MON_SET,   //月份
      .tm_year = RTC_YEAR_SET-1900, //年份（如今年是2022，则这里输入2022-1900=122）
    };
    /*打开RTC模块*/
    R_RTC_Open (RTC.p_ctrl, RTC.p_cfg);
    /*时钟源设置，如果在FSP Configuration设置"Set Source Clock in Open"为"enabled"，那这一步可以被跳过*/
    R_RTC_ClockSourceSet (RTC.p_ctrl);
    /*若RTC时钟已经使用纽扣电池工作了一段时间，则可以使用这个函数获取当前日历并设置当前时间*/
    //R_RTC_CalendarTimeGet(RTC.p_ctrl,&set_time);
    /*这个函数至少调用一次以启动RTC*/
    R_RTC_CalendarTimeSet (RTC.p_ctrl, &set_time); //设置当前时间
    RTC_Alarm_Init(); //初始化闹钟中断
}



/**
 * @brief 让蜂鸣器叫一声
 * @retval 无
 * @param 无
 */
static void Buzzer_sout(void)
{
    R_BSP_PinWrite(BUZZER, 1);
    R_BSP_SoftwareDelay(50, BSP_DELAY_UNITS_MILLISECONDS);
    R_BSP_PinWrite(BUZZER, 0);
}

/**
 * @brief RTC中断回调函数
 * @retval 无
 * @param p_args
 */
void RTC_Callback(rtc_callback_args_t *p_args)
{
    static rtc_time_t get_time;
    switch (p_args->event)
    {
        case RTC_EVENT_PERIODIC_IRQ:
            break;
        /*若是闹钟中断，则发送日期给串口并切换LED电平并让蜂鸣器叫一声*/
        case RTC_EVENT_ALARM_IRQ:
            /*获取当前时间*/
            R_RTC_CalendarTimeGet (RTC.p_ctrl, &get_time);
            /*打印当前时间*/
            printf ("\r\n%d-%d-%d-%d:%d:%d\r\n", get_time.tm_year + 1900, get_time.tm_mon, get_time.tm_mday,
                                get_time.tm_hour, get_time.tm_min, get_time.tm_sec);
            PIN_TOGGLE (LED_R); //反转LED
            Buzzer_sout(); //蜂鸣器叫一声
            break;
        default:
            break;
    }
}
