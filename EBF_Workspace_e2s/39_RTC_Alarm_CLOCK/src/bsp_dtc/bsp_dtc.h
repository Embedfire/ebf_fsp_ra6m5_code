#ifndef BSP_DTC_H
#define BSP_DTC_H

#include "hal_data.h"

void RTC_Init(void);
void RTC_Alarm_Init (void);


/**************RTC设置时间宏定义**************/

/**********日期宏定义**********/
#define RTC_YEAR_SET 2022       //年
#define RTC_MON_SET 8           //月
#define RTC_MDAY_SET 3          //日
/*通过蔡勒公式计算星期*/
#define RTC_WDAY_SET (RTC_YEAR_SET-2000 \
                    + ((RTC_YEAR_SET-2000)/4) \
                    - 35 + (26*(RTC_MON_SET+1))/10 \
                    + RTC_MDAY_SET -1 )%7

/**********时间宏定义**********/
#define RTC_SEC_SET 0           //秒
#define RTC_MIN_SET 0           //分
#define RTC_HOUR_SET 0          //时

/**************闹钟宏定义**************/

/**********日期宏定义**********/
#define RTC_ALARM_YEAR_SET 2022       //年
#define RTC_ALARM_MON_SET 8           //月
#define RTC_ALARM_MDAY_SET 3          //日
/*通过蔡勒公式计算星期*/
#define RTC_ALARM_WDAY_SET (RTC_YEAR_SET-2000 \
                    + ((RTC_YEAR_SET-2000)/4) \
                    - 35 + (26*(RTC_MON_SET+1))/10 \
                    + RTC_MDAY_SET -1 )%7

/**********时间宏定义**********/
#define RTC_ALARM_SEC_SET 15          //秒
#define RTC_ALARM_MIN_SET 0           //分
#define RTC_ALARM_HOUR_SET 0          //时

/***使能闹钟比较***/
#define RTC_ALARM_YEAR_ENABLE 0  //年
#define RTC_ALARM_MON_ENABLE 0   //月
#define RTC_ALARM_MDAY_ENABLE 0  //日
#define RTC_ALARM_WDAY_ENABLE 0     //星期

#define RTC_ALARM_SEC_ENABLE 1          //秒
#define RTC_ALARM_MIN_ENABLE 0           //分
#define RTC_ALARM_HOUR_ENABLE 0          //时

/*蜂鸣器响一声*/
#define Buzzer_sout()  R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_05, BSP_IO_LEVEL_HIGH); \
                                             R_BSP_SoftwareDelay (500, BSP_DELAY_UNITS_MILLISECONDS); \
                                             R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_05, BSP_IO_LEVEL_LOW);


#endif
