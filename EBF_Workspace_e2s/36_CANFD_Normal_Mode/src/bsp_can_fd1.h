#ifndef BSP_CAN_FD1_H_
#define BSP_CAN_FD1_H_
#include "hal_data.h"
#include "stdio.h"
#include "bsp_can_cfg.h"



extern can_frame_t canfd1_tx_frame; //CAN transmit frame
extern can_frame_t canfd1_rx_frame;

extern volatile bool b_canfd1_tx_complete;
extern volatile bool b_canfd1_rx_complete;
extern volatile bool b_canfd1_err_status;
extern volatile canfd_error_t canfd1_err_status;


void bsp_canfd1_init(void);
void canfd1_deinit(void);

void canfd1_operation(void);
void canfd1_write_data(can_frame_t can_transmit_frame);

#endif /* BSP_CAN_FD0_H_ */
