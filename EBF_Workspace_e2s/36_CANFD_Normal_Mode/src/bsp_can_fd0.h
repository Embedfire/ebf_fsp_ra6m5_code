#ifndef BSP_CAN_FD0_H_
#define BSP_CAN_FD0_H_
#include "hal_data.h"
#include "stdio.h"
#include "bsp_can_cfg.h"


extern can_frame_t canfd0_tx_frame; //CAN transmit frame
extern can_frame_t canfd0_rx_frame;

extern volatile bool b_canfd0_tx_complete;
extern volatile bool b_canfd0_rx_complete;
extern volatile bool b_canfd0_err_status;
extern volatile canfd_error_t canfd0_err_status;
extern can_info_t canfd0_rx_info;

void bsp_canfd0_init(void);
void canfd0_deinit(void);

void canfd_fd_loopback_example(void);

void can0_operation(void);
void canfd0_operation(void);
void canfd0_write_data(can_frame_t can_transmit_frame);
void canfd0_read_data(void);

#endif /* BSP_CAN_FD0_H_ */
