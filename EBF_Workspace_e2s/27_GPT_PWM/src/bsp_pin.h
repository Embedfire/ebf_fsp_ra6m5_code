/*
 * bsp_pin.h
 *
 *  Created on: 2022年8月26日
 *      Author: admin
 */

#ifndef BSP_PIN_H_
#define BSP_PIN_H_

#include "hal_data.h"

/*使用前先使用R_BSP_PinAccessEnable*/
#define PIN_TOGGLE(pin) R_PFS->PORT[pin>>8].PIN[pin & BSP_IO_PRV_8BIT_MASK].PmnPFS_b.PODR++

#endif /* BSP_PIN_H_ */
