/*
 * bsp_led.h
 *
 *  Created on: 2022年7月15日
 *      Author: admin
 */

#ifndef BSP_LED_H_
#define BSP_LED_H_

#include "hal_data.h"

#define LED_R(a) R_BSP_PinWrite(LED_R__,a)
#define LED_G(a) R_BSP_PinWrite(LED_G__,a)
#define LED_B(a) R_BSP_PinWrite(LED_B__,a)

#define LED_RED     LED_R(0);LED_G(1);LED_B(1)
#define LED_GREEN   LED_R(1);LED_G(0);LED_B(1)
#define LED_BLUE    LED_R(1);LED_G(1);LED_B(0)
#define LED_YELLOW  LED_R(0);LED_G(0);LED_B(1)  //红+绿=黄
#define LED_PURPLE  LED_R(0);LED_G(1);LED_B(0)  //红+蓝=紫
#define LED_CYAN    LED_R(1);LED_G(0);LED_B(0)  //绿+蓝=青
#define LED_WHITE   LED_R(0);LED_G(0);LED_B(0)  //红+绿+蓝=白
#define LED_RGBOFF  LED_R(1);LED_G(1);LED_B(1)  //灯光全灭

#endif /* BSP_LED_H_ */
