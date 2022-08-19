#include "RTC.h"
#include "stdio.h"

static rtc_time_t set_time =
{
    .tm_sec  = 10,
    .tm_min  = 20,
    .tm_hour = 15,
    .tm_mday = 26,
    .tm_wday = 3,
    .tm_mon  = 7,
    .tm_year = 144,
};

void RTC_Init(void)
{
	R_RTC_Open(RTC.p_ctrl,RTC.p_cfg);
	R_RTC_ClockSourceSet(RTC.p_ctrl);
	R_RTC_CalendarTimeGet(RTC.p_ctrl,&set_time);
	R_RTC_CalendarTimeSet(RTC.p_ctrl, &set_time);
	R_RTC_PeriodicIrqRateSet(RTC.p_ctrl, RTC_PERIODIC_IRQ_SELECT_1_SECOND);
}



static void LED_Task(void)
{
	static bool LED_Flag = true;
	if(LED_Flag)
	{
		R_BSP_PinWrite(LED_G,1);
		LED_Flag = !LED_Flag;
	}
	else
	{
		R_BSP_PinWrite(LED_G,0);
		LED_Flag = !LED_Flag;
	}
}

void RTC_Callback(rtc_callback_args_t *p_args)
{
	static rtc_time_t get_time;
	switch(p_args->event)
	{
		case RTC_EVENT_PERIODIC_IRQ:  //流水灯+发送数据给调试串口
			LED_Task();
			R_RTC_CalendarTimeGet(RTC.p_ctrl,&get_time);
			printf("%d-%d-%d-%d:%d:%d\r\n", get_time.tm_year+1900, get_time.tm_mon, get_time.tm_mday, get_time.tm_hour, get_time.tm_min, get_time.tm_sec);
			break;
		default:
			break;
	}
}