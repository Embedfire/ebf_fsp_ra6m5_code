#ifndef BSP_DTC_H
#define BSP_DTC_H

#include "hal_data.h"

/**********日期宏定义**********/
#define RTC_YEAR_SET 2008       //年
#define RTC_MON_SET 8           //月
#define RTC_MDAY_SET 8          //日
/*通过蔡勒公式计算星期*/
#define RTC_WDAY_SET (RTC_YEAR_SET-2000 \
                  + ((RTC_YEAR_SET-2000)/4) \
                  - 35 + (26*(RTC_MON_SET+1))/10 \
                  + RTC_MDAY_SET -1 )%7

/**********时间宏定义**********/
#define RTC_HOUR_SET 0          //时
#define RTC_SEC_SET 0           //秒
#define RTC_MIN_SET 0           //分

void RTC_Init(void);




#endif