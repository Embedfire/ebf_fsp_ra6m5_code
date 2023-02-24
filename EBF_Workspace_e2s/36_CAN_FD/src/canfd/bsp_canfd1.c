#include "bsp_canfd1.h"


/* CAN 初始化函数 */
void CANFD1_Init(void)
{
    fsp_err_t err = R_CANFD_Open(&g_canfd1_ctrl, &g_canfd1_cfg);
    assert(FSP_SUCCESS == err);
}


/* CANFD Channel 1 Acceptance Filter List (AFL) rule array */
const canfd_afl_entry_t p_canfd1_afl[CANFD_CFG_AFL_CH1_RULE_NUM] =
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


/* CAN 帧 */
can_frame_t canfd1_tx_frame; //CAN transmit frame
can_frame_t canfd1_rx_frame;

/* 保存帧接收状态信息 */
can_info_t canfd1_rx_info;


/* 要在回调函数中设置的标志 */
volatile bool canfd1_tx_complete_flag = false;
volatile bool canfd1_rx_complete_flag = false;
volatile bool canfd1_err_status_flag = false;
volatile canfd_error_t canfd1_err_status = (canfd_error_t) 0;


/* CANFD1 中断回调函数 */
void canfd1_callback(can_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        case CAN_EVENT_RX_COMPLETE:     //接收完成中断
        {
            canfd1_rx_complete_flag = true; //canfd1接收到数据

            /* 读取接收帧 */
            memcpy(&canfd1_rx_frame, &(p_args->frame), sizeof(can_frame_t));

            break;
        }
        case CAN_EVENT_TX_COMPLETE:     //传输完成中断
        {
            canfd1_tx_complete_flag = true; //canfd0数据发送完成
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
            canfd1_err_status_flag = true;  //设置标志位

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


/* CAN1 操作函数（不使用 FD） */
void CAN1_Operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t time_out = WAIT_TIME;
    uint8_t tx_data[64] = "TX_MESG";

    /* 更新传输帧的参数 */
    canfd1_tx_frame.id = CAN_ID;
    canfd1_tx_frame.id_mode = CAN_ID_MODE_EXTENDED;
    canfd1_tx_frame.type = CAN_FRAME_TYPE_DATA;
    canfd1_tx_frame.data_length_code = CAN_CLASSIC_FRAME_DATA_BYTES;
    canfd1_tx_frame.options = 0;

    /* 填充将要传输的帧数据 */
    memcpy((uint8_t*)&canfd1_tx_frame.data[0], (uint8_t*)&tx_data[0], CAN_CLASSIC_FRAME_DATA_BYTES);


    CANFD1_MSG_PRINTF("CANFD1 正在使用传统 CAN 帧传输数据");

    /* 通过 mail box #0 传输数据 */
    err = g_canfd_on_canfd.write(&g_canfd1_ctrl, CAN_MAILBOX_NUMBER_0, &canfd1_tx_frame);
    assert(FSP_SUCCESS == err);

    /* 等待传输完成 */
    while ((true != canfd1_tx_complete_flag) && (--time_out));
    canfd1_tx_complete_flag = false;
    if (0 == time_out) {
        CANFD1_MSG_PRINTF("传输超时！！传输失败！！");
        return;
    }
    CANFD1_MSG_PRINTF("传输完成");
}


/* CANFD1 操作函数（使用 FD） */
void CANFD1_Operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint32_t time_out = WAIT_TIME;

    /* 更新 FD 帧的参数 */
    canfd1_tx_frame.id = CAN_ID;
    canfd1_tx_frame.id_mode = CAN_ID_MODE_EXTENDED;
    canfd1_tx_frame.type = CAN_FRAME_TYPE_DATA;
    canfd1_tx_frame.data_length_code = CAN_FD_DATA_LENGTH_CODE;
    canfd1_tx_frame.options = CANFD_FRAME_OPTION_FD | CANFD_FRAME_OPTION_BRS;

    /* 填充将要在 FD 帧中发送出去的帧数据 */
    for( uint16_t j = 0; j < CAN_FD_DATA_LENGTH_CODE; j++)
    {
        canfd1_tx_frame.data[j] = (uint8_t) (CAN_FD_DATA_LENGTH_CODE - j);
    }


    CANFD1_MSG_PRINTF("CANFD1 正在使用 CAN FD 帧传输数据");

    /* 通过 mail box #0 传输数据 */
    err = R_CANFD_Write(&g_canfd1_ctrl, CAN_MAILBOX_NUMBER_0, &canfd1_tx_frame);
    assert(FSP_SUCCESS == err);

    /* 等待传输完成 */
    while ((true != canfd1_tx_complete_flag) && (--time_out));
    canfd1_tx_complete_flag = false;
    if (0 == time_out)
    {
        CANFD1_MSG_PRINTF("传输超时！！传输失败！！");
        return;
    }
    CANFD1_MSG_PRINTF("传输完成");
}

