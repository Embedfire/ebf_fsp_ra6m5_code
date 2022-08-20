#ifndef BSP_CAN_FD0_H_
#define BSP_CAN_FD0_H_
#include "hal_data.h"
#include "stdio.h"



#define CAN_MAILBOX_NUMBER_0            (CANFD_TX_MB_0)    //邮箱号码
#define CAN_CLASSIC_FRAME_DATA_BYTES    (8U)               //用于经典帧的数据长度代码
#define CAN_FD_DATA_LENGTH_CODE         (64)               //FD帧的数据长度编码
#define CAN_ID                          (0x1100)           //要在传输帧中更新的ID

/* 验收过滤器数组参数 */
#define CANFD_FILTER_ID                 (0x00001000)
#define MASK_ID                         (0x1FFFF000)
#define MASK_ID_MODE                    (1)


/*信息输出*/
#define CANFD_DEBUG_ON            1   //DEBUG开关

#define CANFD_PRINT(fmt, arg...)  \
    do                            \
    {                             \
        if (CANFD_DEBUG_ON)       \
            printf(fmt, ##arg);   \
    } while (0)




extern can_frame_t canfd0_tx_frame; //CAN transmit frame
extern can_frame_t canfd0_rx_frame;

extern volatile bool b_canfd0_tx_complete;
extern volatile bool b_canfd0_rx_complete;
extern volatile bool b_canfd0_err_status;
extern volatile canfd_error_t canfd0_err_status;


void bsp_canfd0_init(void);
void canfd0_deinit(void);

void canfd_test_mode_switch(can_test_mode_t test_mode);
void canfd0_operation(void);
void canfd0_write_data(can_frame_t can_transmit_frame);
void canfd0_read_data(void);

void canfd_fd_loopback_example(void);

#endif /* BSP_CAN_FD0_H_ */
