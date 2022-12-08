#ifndef BSP_DTC_H
#define BSP_DTC_H

#include "hal_data.h"

/**********���ں궨��**********/
#define RTC_YEAR_SET 2022       //��
#define RTC_MON_SET 8           //��
#define RTC_MDAY_SET 3          //��
/*ͨ�����չ�ʽ��������*/
#define RTC_WDAY_SET (RTC_YEAR_SET-2000 \
                  + ((RTC_YEAR_SET-2000)/4) \
                  - 35 + (26*(RTC_MON_SET+1))/10 \
                  + RTC_MDAY_SET -1 )%7

/**********ʱ��궨��**********/
#define RTC_HOUR_SET 0          //ʱ
#define RTC_SEC_SET 0           //��
#define RTC_MIN_SET 0           //��

void RTC_Init(void);




#endif