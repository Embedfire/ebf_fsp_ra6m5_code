#ifndef BSP_DTC_H
#define BSP_DTC_H

#include "hal_data.h"

void RTC_Init(void);
void RTC_Alarm_Init (void);


/**************RTC����ʱ��궨��**************/

/**********���ں궨��**********/
#define RTC_YEAR_SET 2008       //��
#define RTC_MON_SET 8           //��
#define RTC_MDAY_SET 8          //��
/*ͨ�����չ�ʽ��������*/
#define RTC_WDAY_SET (RTC_YEAR_SET-2000 \
                    + ((RTC_YEAR_SET-2000)/4) \
                    - 35 + (26*(RTC_MON_SET+1))/10 \
                    + RTC_MDAY_SET -1 )%7

/**********ʱ��궨��**********/
#define RTC_SEC_SET 0           //��
#define RTC_MIN_SET 0           //��
#define RTC_HOUR_SET 0          //ʱ

/**************���Ӻ궨��**************/

/**********���ں궨��**********/
#define RTC_ALARM_YEAR_SET 2008       //��
#define RTC_ALARM_MON_SET 8           //��
#define RTC_ALARM_MDAY_SET 8          //��
/*ͨ�����չ�ʽ��������*/
#define RTC_ALARM_WDAY_SET (RTC_YEAR_SET-2000 \
                    + ((RTC_YEAR_SET-2000)/4) \
                    - 35 + (26*(RTC_MON_SET+1))/10 \
                    + RTC_MDAY_SET -1 )%7

/**********ʱ��궨��**********/
#define RTC_ALARM_SEC_SET 15          //��
#define RTC_ALARM_MIN_SET 0           //��
#define RTC_ALARM_HOUR_SET 0          //ʱ

/***ʹ�����ӱȽ�***/
#define RTC_ALARM_YEAR_ENABLE 0  //��
#define RTC_ALARM_MON_ENABLE 0   //��
#define RTC_ALARM_MDAY_ENABLE 0  //��
#define RTC_ALARM_WDAY_ENABLE 0     //����

#define RTC_ALARM_SEC_ENABLE 1          //��
#define RTC_ALARM_MIN_ENABLE 0           //��
#define RTC_ALARM_HOUR_ENABLE 0          //ʱ

/*��������һ��*/
#define Buzzer_sout()  R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_05, BSP_IO_LEVEL_HIGH); \
											 R_BSP_SoftwareDelay (500, BSP_DELAY_UNITS_MILLISECONDS); \
											 R_IOPORT_PinWrite(&g_ioport_ctrl, BSP_IO_PORT_06_PIN_05, BSP_IO_LEVEL_LOW); 


#endif