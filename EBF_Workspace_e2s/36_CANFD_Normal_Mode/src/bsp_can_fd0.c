#include <bsp_can_fd0.h>



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

can_frame_t canfd0_tx_frame; //CAN transmit frame
can_frame_t canfd0_rx_frame;
/* Variable to store rx frame status info*/
can_info_t canfd0_rx_info;



/* 要在回调函数中设置的标志 */
volatile bool b_canfd0_tx_complete = false;
volatile bool b_canfd0_rx_complete = false;
volatile bool b_canfd0_err_status = false;
volatile canfd_error_t canfd0_err_status = (canfd_error_t) 0;




/* CANFD Channel 0 Acceptance Filter List (AFL) rule array */
const canfd_afl_entry_t p_canfd0_afl[CANFD_CFG_AFL_CH0_RULE_NUM] =
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
         .mask_frame_type = 0,            //仅接受具有配置的帧类型的帧 (frame_type)
         .mask_id_mode    = MASK_ID_MODE  //仅接受配置ID模式的帧 (id_mode)
     },

     .destination =
     {
         /* If DLC checking is enabled any messages shorter than the below setting will be rejected. */
         .minimum_dlc = CANFD_MINIMUM_DLC_0, //要接受的最小DLC值（启用DLC检查时有效）

         /* Optionally specify a Receive Message Buffer (RX MB) to store accepted frames. RX MBs do not have an
          * interrupt or overwrite protection and must be checked with R_CANFD_InfoGet and R_CANFD_Read. */
         .rx_buffer   = CANFD_RX_MB_0,  //接收此规则接受的消息的RX消息缓冲区

         /* Specify which FIFO(s) to send filtered messages to. Multiple FIFOs can be OR'd together. */
         .fifo_select_flags = CANFD_RX_FIFO_0  //接收此规则接受的消息的RX FIFO
     }
 },
};





void bsp_canfd0_init(void)
{
    fsp_err_t err = g_canfd_on_canfd.open(&g_canfd0_ctrl, &g_canfd0_cfg);
    assert(FSP_SUCCESS == err);
}

void canfd_fd_loopback_example(void)
{
    can_frame_t g_can_tx_frame;
    fsp_err_t err;

    /* 切换到外部环回模式 */ //CAN_TEST_MODE_LOOPBACK_EXTERNAL CAN_TEST_MODE_LOOPBACK_INTERNAL
    err = g_canfd_on_canfd.modeTransition(&g_canfd0_ctrl, CAN_OPERATION_MODE_NORMAL, CAN_TEST_MODE_LOOPBACK_EXTERNAL);
    assert(FSP_SUCCESS == err);

    /* 将帧配置为在启用比特率切换（BRS）的情况下写入64字节 */
    g_can_tx_frame.id               = CAN_ID;
    g_can_tx_frame.id_mode          = CAN_ID_MODE_EXTENDED;
    g_can_tx_frame.type             = CAN_FRAME_TYPE_DATA;
    g_can_tx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;
    g_can_tx_frame.options          = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;

    /* 将数据写入传输帧 */
    for (uint32_t i = 0; i < CAN_DATA_BUFFER_LENGTH; i++)
    {
        g_can_tx_frame.data[i] = (uint8_t) i;
    }

    /* 发送数据 */
    canfd0_write_data(g_can_tx_frame);

//    err = g_canfd_on_canfd.write(&g_canfd0_ctrl, CANFD_TX_MB_0, &g_can_tx_frame);
//    assert(FSP_SUCCESS == err);
    /* 等待 发送/接收 回调事件 */
}

void can0_operation(void)
{
    /* Update transmit frame parameters */
    canfd0_tx_frame.id = CAN_ID;
    canfd0_tx_frame.id_mode = CAN_ID_MODE_EXTENDED;
    canfd0_tx_frame.type = CAN_FRAME_TYPE_DATA;
    canfd0_tx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
    canfd0_tx_frame.options = 0;

    /* Update transmit frame data with message */
    memcpy((uint8_t*)&canfd0_tx_frame.data[0], (uint8_t*)&tx_data[0], CAN_FD_DATA_LENGTH_CODE);

    CANFD_PRINT("CANFD0 通过经典CAN帧传输数据\r\n");

    /* Transmission of data over classic CAN frame */
    canfd0_write_data(canfd0_tx_frame);
}

void canfd0_operation(void)
{
    /* Updating FD frame parameters */
    canfd0_tx_frame.id = CAN_ID;
    canfd0_tx_frame.id_mode = CAN_ID_MODE_EXTENDED;
    canfd0_tx_frame.type = CAN_FRAME_TYPE_DATA;
    canfd0_tx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;
    canfd0_tx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;

    /* Fill frame data that is to be sent in FD frame */
    for( uint16_t j = 0; j < 64; j++)
    {
        canfd0_tx_frame.data[j] = (uint8_t) (j + 1);
    }

    CANFD_PRINT("CANFD0 通过CAN-FD帧传输数据\r\n");

    /* Transmission of data as over FD frame */
    canfd0_write_data(canfd0_tx_frame);

}



void canfd0_write_data(can_frame_t can_transmit_frame)
{
    fsp_err_t err = FSP_SUCCESS;
    volatile uint32_t g_time_out = WAIT_TIME;

    /* Transmit the data from mail box #0 with tx_frame */
    err = g_canfd_on_canfd.write(&g_canfd0_ctrl, CAN_MAILBOX_NUMBER_0, &can_transmit_frame);
    assert(FSP_SUCCESS == err);

    /* 等待传输完成 */
    while ((true != b_canfd0_tx_complete) && (--g_time_out));
    b_canfd0_tx_complete = false;
    if (0 == g_time_out) {
        CANFD_PRINT("CANFD传输超时！！CANFD传输失败！！\r\n");
        return;
    }
    CANFD_PRINT("CANFD传输完成\r\n");
}



void canfd0_read_data(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* 获取CAN传输的状态信息 */
    err = g_canfd_on_canfd.infoGet(&g_canfd0_ctrl, &canfd0_rx_info);
    assert(FSP_SUCCESS == err);

    /* 检查FIFO中是否接收到数据 */
    if(canfd0_rx_info.rx_mb_status)
    {
        /* Read the input frame received */
        err = R_CANFD_Read(&g_canfd0_ctrl, 0, &canfd0_rx_frame);
        assert(FSP_SUCCESS == err);

        /*Check if the transmitted and received data are same and send ACK accordingly */
        //canfd_check_data();
    }
    else
    {
        /* Do Nothing */
    }
}



void canfd0_deinit(void)
{
    fsp_err_t err = g_canfd_on_canfd.close(&g_canfd0_ctrl);
    assert(FSP_SUCCESS == err);
}


void canfd0_callback(can_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        case CAN_EVENT_RX_COMPLETE:    //接收完成中断
        {
            b_canfd0_rx_complete = true;

            /* 读取接收帧 */
            memcpy(&canfd0_rx_frame, p_args->p_frame, sizeof(can_frame_t));

            //CANFD_PRINT("canfd0接收到数据\r\n");
            break;
        }
        case CAN_EVENT_TX_COMPLETE:    //传输完成中断
        {
            b_canfd0_tx_complete = true;        //set flag bit
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
            b_canfd0_err_status = true;  //设置标志位

            /* 获取错误状态 */
            canfd0_err_status = (canfd_error_t) p_args->error;

            break;
        }
        default:
        {
            break;
        }
    }
}

