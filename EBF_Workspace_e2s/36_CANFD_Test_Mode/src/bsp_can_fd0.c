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
            /* 指定要接受的ID、ID类型和帧类型。 */
            .id         = CANFD_FILTER_ID,
            .frame_type = CAN_FRAME_TYPE_DATA,
            .id_mode    = CAN_ID_MODE_EXTENDED
        },

        .mask =
        {
            /* 这些值屏蔽了过滤邮件时要比较的ID/模式位。 */
            .mask_id         = MASK_ID,
            .mask_frame_type = 0,
            .mask_id_mode    = MASK_ID_MODE
        },

        .destination =
        {
            /* 如果启用了DLC检查，则任何短于以下设置的消息都将被拒绝。 */
            .minimum_dlc = CANFD_MINIMUM_DLC_0,

            /* 也可以指定接收消息缓冲区(RX MB)来存储接受的帧。
             * RX MB没有中断或重写保护，必须使用R_CANFD_INFO Get和R_CANFD_READ进行检查。 */
            .rx_buffer   = CANFD_RX_MB_0,

            /* 指定要将筛选的消息发送到的FIFO。多个FIFO可以一起进行或运算。 */
            .fifo_select_flags = CANFD_RX_FIFO_0
        }
    },
};





void bsp_canfd0_init(void)
{
    fsp_err_t err = g_canfd_on_canfd.open(&g_canfd0_ctrl, &g_canfd0_cfg);
    assert(FSP_SUCCESS == err);
}


void canfd_test_mode_switch(can_test_mode_t test_mode)
{
    fsp_err_t err = g_canfd_on_canfd.modeTransition(
            &g_canfd0_ctrl, CAN_OPERATION_MODE_NORMAL, test_mode);
    assert(FSP_SUCCESS == err);
}

void canfd_fd_loopback_example(void)
{
    can_frame_t g_can_tx_frame;
    fsp_err_t err;

    /* 切换到测试模式 */
//    err = g_canfd_on_canfd.modeTransition(  //内部回环
//            &g_canfd0_ctrl, CAN_OPERATION_MODE_NORMAL, CAN_TEST_MODE_LOOPBACK_INTERNAL);
    err = g_canfd_on_canfd.modeTransition(  //外部回环
            &g_canfd0_ctrl, CAN_OPERATION_MODE_NORMAL, CAN_TEST_MODE_LOOPBACK_EXTERNAL);
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

    /* 等待 发送/接收 回调事件 */
}


void canfd0_write_data(can_frame_t can_transmit_frame)
{
    fsp_err_t err = FSP_SUCCESS;
    volatile uint32_t g_time_out = 0xFFFF;

    /* Transmit the data from mail box #0 with tx_frame */
    err = g_canfd_on_canfd.write(&g_canfd0_ctrl, CAN_MAILBOX_NUMBER_0, &can_transmit_frame);
    assert(FSP_SUCCESS == err);

    /* 等待传输完成 */
    while ((false == b_canfd0_tx_complete) && (--g_time_out));
    if (0 == g_time_out) {
        CANFD_PRINT("\r\n\r\n》can0传输超时！！can传输失败！！\r\n\r\n");
        return;
    }
    CANFD_PRINT("》can0传输完成\r\n");

    /* 重置标志位 */
    b_canfd0_tx_complete = false;
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
        err = g_canfd_on_canfd.read(&g_canfd0_ctrl, 0, &canfd0_rx_frame);
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

