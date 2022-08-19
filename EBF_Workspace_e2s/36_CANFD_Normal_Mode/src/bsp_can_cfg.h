#ifndef BSP_CAN_CFG_H_
#define BSP_CAN_CFG_H_


#define CAN_MAILBOX_NUMBER_0            (0U)               //邮箱号码
#define CAN_CLASSIC_FRAME_DATA_BYTES    (8U)               //用于经典帧的数据长度代码
#define CAN_FD_DATA_LENGTH_CODE         (64)               //FD帧的数据长度编码
#define CAN_ID                          (0x1100)           //要在传输帧中更新的ID

/* 验收过滤器数组参数 */
#define CANFD_FILTER_ID                 (0x00001000)
#define MASK_ID                         (0x1FFFF000)
#define MASK_ID_MODE                    (1)

#define ZERO                            (0U)               //Array Index value
#define NULL_CHAR                       ('\0')
#define WAIT_TIME                       (500U)             //Wait time value


/*信息输出*/
#define CANFD_DEBUG_ON            1   //DEBUG开关

#define CANFD_PRINT(fmt, arg...)  \
    do                            \
    {                             \
        if (CANFD_DEBUG_ON)       \
            printf(fmt, ##arg);   \
    } while (0)


/* Data to be loaded in Classic CAN and FD frames for transmission and acknowledgement */
extern volatile uint8_t tx_data[64];
extern volatile uint8_t rx_data[64];
extern volatile uint8_t tx_fd_data[64];
extern volatile uint8_t rx_fd_data[64];

#endif /* BSP_CAN_CFG_H_ */
