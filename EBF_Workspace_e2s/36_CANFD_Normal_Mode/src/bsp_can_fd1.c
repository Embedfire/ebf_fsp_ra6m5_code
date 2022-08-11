#include "bsp_can_fd1.h"



/*
 * CAN function pointers
 * const can_api_t g_canfd_on_canfd =
 * {
 *     .open           = R_CANFD_Open,
 *     .close          = R_CANFD_Close,
 *     .write          = g_canfd_on_canfd.write,
 *     .read           = R_CANFD_Read,
 *     .modeTransition = R_CANFD_ModeTransition,
 *     .infoGet        = R_CANFD_InfoGet,
 *     .callbackSet    = R_CANFD_CallbackSet,
 * };
 */

can_frame_t canfd1_tx_frame; //CAN transmit frame
can_frame_t canfd1_rx_frame;
/* Variable to store rx frame status info*/
//can_info_t can1_rx_info =
//{
//    .error_code  = 0,
//    .error_count_receive = 0,
//    .error_count_transmit = 0,
//    .rx_fifo_status = 0,
//    .rx_mb_status = 0,
//    .status = 0,
//};



/* 要在回调函数中设置的标志 */
volatile bool b_canfd1_tx_complete = false;
volatile bool b_canfd1_rx_complete = false;
volatile bool b_canfd1_err_status = false;
volatile canfd_error_t canfd1_err_status = (canfd_error_t) 0;



/* CANFD Channel 1 Acceptance Filter List (AFL) rule array */
const canfd_afl_entry_t p_canfd1_afl[CANFD_CFG_AFL_CH1_RULE_NUM] =
{
    {
        .id =
        {
            /* Specify the ID, ID type and frame type to accept. */
            .id         = CANFD_FILTER_ID,
            .frame_type = CAN_FRAME_TYPE_DATA,
            .id_mode    = CAN_ID_MODE_EXTENDED
        },

        .mask =
        {
            /* These values mask which ID/mode bits to compare when filtering messages. */
            .mask_id         = MASK_ID,
            .mask_frame_type = 0,
            .mask_id_mode    = MASK_ID_MODE
        },

        .destination =
        {
            /* If DLC checking is enabled any messages shorter than the below setting will be rejected. */
            .minimum_dlc = CANFD_MINIMUM_DLC_0,

            /* Optionally specify a Receive Message Buffer (RX MB) to store accepted frames. RX MBs do not have an
             * interrupt or overwrite protection and must be checked with R_CANFD_InfoGet and R_CANFD_Read. */
            .rx_buffer   = CANFD_RX_MB_0,

            /* Specify which FIFO(s) to send filtered messages to. Multiple FIFOs can be OR'd together. */
            .fifo_select_flags = CANFD_RX_FIFO_0
        }
    },
};


void bsp_canfd1_init(void)
{
    fsp_err_t err = g_canfd_on_canfd.open(&g_canfd1_ctrl, &g_canfd1_cfg);
    assert(FSP_SUCCESS == err);
}

void canfd1_operation(void)
{
    /* Update transmit frame parameters */
    canfd1_tx_frame.id = CAN_ID;
    canfd1_tx_frame.id_mode = CAN_ID_MODE_EXTENDED;
    canfd1_tx_frame.type = CAN_FRAME_TYPE_DATA;
    canfd1_tx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
    canfd1_tx_frame.options = 0;

    /* Update transmit frame data with message */
    memcpy((uint8_t*)&canfd1_tx_frame.data[0], (uint8_t*)&tx_data[0], CAN_FD_DATA_LENGTH_CODE);





    /* Updating FD frame parameters */
    canfd1_tx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;
    canfd1_tx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;

    /* Fill frame data that is to be sent in FD frame */
    for( uint16_t j = 0; j < 64; j++)
    {
        canfd1_tx_frame.data[j] = (uint8_t) (j + 1);
    }

    CANFD_PRINT("通过CAN-FD帧传输数据\r\n");

    /* Transmission of data as over FD frame */
    canfd1_write_data(canfd1_tx_frame);





//    CANFD_PRINT("通过经典CAN帧传输数据\r\n");
//
//    /* Transmission of data over classic CAN frame */
//    canfd1_write_data(canfd1_tx_frame);
}

void canfd1_write_data(can_frame_t can_transmit_frame)
{
    fsp_err_t err = FSP_SUCCESS;
    volatile uint32_t g_time_out = 1000;

    /* Transmit the data from mail box #0 with tx_frame */
    err = g_canfd_on_canfd.write(&g_canfd1_ctrl, CAN_MAILBOX_NUMBER_0, &can_transmit_frame);
    assert(FSP_SUCCESS == err);

    /* 等待传输完成 */
    while ((true != b_canfd1_tx_complete) && (g_time_out--));
    if (0 == g_time_out) {
        CANFD_PRINT("can传输超时！！can传输失败！！\r\n");
        return;
    }
    CANFD_PRINT("can传输完成\r\n");

    /* 重置标志位 */
    b_canfd1_tx_complete = false;
}


void canfd1_deinit(void)
{
    fsp_err_t err = g_canfd_on_canfd.close(&g_canfd1_ctrl);
    assert(FSP_SUCCESS == err);
}


void canfd1_callback(can_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        case CAN_EVENT_RX_COMPLETE:    //接收完成中断
        {
            b_canfd1_rx_complete = true;

            /* 读取接收帧 */
            memcpy(&canfd1_rx_frame, p_args->p_frame, sizeof(can_frame_t));

            //CANFD_PRINT("canfd1接收到数据\r\n");
            break;
        }
        case CAN_EVENT_TX_COMPLETE:    //传输完成中断
        {
            b_canfd1_tx_complete = true;        //set flag bit
            break;
        }
        case CAN_EVENT_ERR_WARNING:             //error warning event
        case CAN_EVENT_ERR_PASSIVE:             //error passive event
        case CAN_EVENT_ERR_BUS_OFF:             //error Bus Off event
        case CAN_EVENT_BUS_RECOVERY:            //Bus recovery error event
        case CAN_EVENT_MAILBOX_MESSAGE_LOST:    //overwrite/overrun error event
        case CAN_EVENT_ERR_BUS_LOCK:            //Bus lock detected (32 consecutive dominant bits).
        case CAN_EVENT_ERR_CHANNEL:             //Channel error has occurred.
        case CAN_EVENT_TX_ABORTED:              //Transmit abort event.
        case CAN_EVENT_ERR_GLOBAL:              //Global error has occurred.
        case CAN_EVENT_TX_FIFO_EMPTY:           //Transmit FIFO is empty.
        {
            b_canfd1_err_status = true;  //设置标志位

            /* 获取错误状态 */
            canfd1_err_status = (canfd_error_t) p_args->error;

            break;
        }
        default:
        {
            break;
        }
    }
}



