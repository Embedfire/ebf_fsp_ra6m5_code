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
can_info_t canfd0_rx_info =
{
    .error_code  = 0,
    .error_count_receive = 0,
    .error_count_transmit = 0,
    .rx_fifo_status = 0,
    .rx_mb_status = 0,
    .status = 0,
};



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





void bsp_canfd0_init(void)
{
    fsp_err_t err = g_canfd_on_canfd.open(&g_canfd0_ctrl, &g_canfd0_cfg);
    assert(FSP_SUCCESS == err);
}


void canfd0_operation(void)
{
    /* Update transmit frame parameters */
    canfd0_tx_frame.id = CAN_ID;
    canfd0_tx_frame.id_mode = CAN_ID_MODE_EXTENDED;
    canfd0_tx_frame.type = CAN_FRAME_TYPE_DATA;
    canfd0_tx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
    canfd0_tx_frame.options = 0;

    /* Update transmit frame data with message */
    memcpy((uint8_t*)&canfd0_tx_frame.data[0], (uint8_t*)&tx_data[0], CAN_FD_DATA_LENGTH_CODE);





    /* Updating FD frame parameters */
    canfd0_tx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;
    canfd0_tx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;

    /* Fill frame data that is to be sent in FD frame */
    for( uint16_t j = 0; j < 64; j++)
    {
        canfd0_tx_frame.data[j] = (uint8_t) (j + 1);
    }

    CANFD_PRINT("通过CAN-FD帧传输数据\r\n");

    /* Transmission of data as over FD frame */
    canfd0_write_data(canfd0_tx_frame);






//    CANFD_PRINT("通过经典CAN帧传输数据\r\n");
//
//    /* Transmission of data over classic CAN frame */
//    canfd0_write_data(canfd0_tx_frame);
}



void canfd0_write_data(can_frame_t can_transmit_frame)
{
    fsp_err_t err = FSP_SUCCESS;
    volatile uint32_t g_time_out = 1000;

    /* Transmit the data from mail box #0 with tx_frame */
    err = g_canfd_on_canfd.write(&g_canfd0_ctrl, CAN_MAILBOX_NUMBER_0, &can_transmit_frame);
    assert(FSP_SUCCESS == err);

    /* 等待传输完成 */
    while ((true != b_canfd0_tx_complete) && (g_time_out--));
    if (0 == g_time_out) {
        CANFD_PRINT("can传输超时！！can传输失败！！\r\n");
        return;
    }
    CANFD_PRINT("can传输完成\r\n");

    /* 重置标志位 */
    b_canfd0_tx_complete = false;
}

//
//void canfd_check_data(void)
//{
//    /*Update data to be compared with data transmitted/received over FD frame */
//    can_fd_data_update();
//
//    if(0 == strncmp((char*)&g_canfd_rx_frame.data[0], (char*)&tx_data[0], CAN_CLASSIC_FRAME_DATA_BYTES))
//    {
//        CANFD_PRINT("\nReceived 'TX__MESG' on classic frame, responding with 'RX__MESG' using classic CAN frame\n");
//        /* 更新接收帧参数 */
//        g_canfd_rx_frame.id = CAN_ID;
//        g_canfd_rx_frame.type = CAN_FRAME_TYPE_DATA;
//        g_canfd_rx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
//        g_canfd_rx_frame.options = 0;
//
//        /* Update receive frame data with message */
//        memcpy(&g_canfd_rx_frame.data, &rx_data, CAN_CLASSIC_FRAME_DATA_BYTES);
//
//        /* Transmission of data as acknowledgement */
//        canfd_write_data(g_canfd_rx_frame);
//
//        CANFD_PRINT("\r\n接收成功后进行CAN传输。使用经典CAN帧发回ACK\r\n");
//
//
//    }
//    else if(0 == strncmp((char*)&g_canfd_rx_frame.data[0], (char*)&rx_data[0], CAN_CLASSIC_FRAME_DATA_BYTES))
//    {
//        CANFD_PRINT("\r\n收到经典CAN帧传输的确认。\r\nCAN操作成功长度 = %d\r\n", g_canfd_rx_frame.data_length_code);
//        CANFD_PRINT("\r\nFD帧上的数据传输\r\n");
//        /* Updating FD frame parameters */
//        g_canfd_rx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;
//        g_canfd_rx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;
//
//        /* 填充数据到 can fd */
//        for( uint16_t j = 0; j < 64; j++)
//        {
//            g_canfd_rx_frame.data[j] = (uint8_t) (j + 1);
//        }
//
//        /* canfd 发送 */
//        canfd_write_data(g_canfd_rx_frame);
//
//        CANFD_PRINT("\r\n成功接收经典帧ACK后，在FD帧上进行CAN传输\r\n");
//
//    }
//    else if(0 == strncmp((char*)&g_canfd_rx_frame.data[0], (char*)&tx_fd_data[0], CAN_FD_DATA_LENGTH_CODE)) // acknowledging for second transmission
//    {
//        CANFD_PRINT("\r\n通过FD帧接收数据。\r\nCAN操作成功，数据长度 = %d\r\n", g_canfd_rx_frame.data_length_code);
//
//        CANFD_PRINT("\r\n现在通过FD帧发送修改后的数据，作为对接收到的FD数据的确认。\r\n");
//
//        g_canfd_rx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;
//        g_canfd_rx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;
//
//        /* Fill frame data that is to be sent in FD frame */
//        for( uint16_t j = 0; j < 64; j++)
//        {
//            g_canfd_rx_frame.data[j] = (uint8_t) (j + 5);
//        }
//
//        /* Transmission of data as acknowledgement */
//        canfd_write_data(g_canfd_rx_frame);
//
//        CANFD_PRINT("\r\n当确认成功时，在FD帧上进行CAN传输\r\n");
//
//    }
//    else if(0 == strncmp((char*)&g_canfd_rx_frame.data[0], (char*)&rx_fd_data[0], CAN_FD_DATA_LENGTH_CODE)) // acknowledgement for second transmission
//    {
//        CANFD_PRINT("\r\n接收到FD帧的确认。\r\nCAN操作成功，数据长度 =  %d\r\n", g_canfd_rx_frame.data_length_code);
//
//        CANFD_PRINT("\nPlease enter any key on RTT Viewer to initiate CAN transmission.\n");
//    }
//    else /* Wrong MSG Received */
//    {
//        CANFD_PRINT("\r\nCAN数据不匹配！！\r\nCAN操作失败！！\r\n");
//    }
//}
//



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

