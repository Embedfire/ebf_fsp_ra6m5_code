#ifndef __BSP_DMAC_H
#define __BSP_DMAC_H
#include "hal_data.h"


#define DTC_TRANSFER_BLOCK_MODE    (1)   //块模式
#define DTC_TRANSFER_NORMAL_MODE   (2)   //正常模式
#define DTC_TRANSFER_REPEAT_MODE   (3)   //重复模式
#define DTC_TRANSFER_MODE          DTC_TRANSFER_REPEAT_MODE


// 要发送的数据大小
#define BUFFER_SIZE     32


void DMAC_Init(void);

void set_transfer_dst_src_address(transfer_cfg_t const * const p_config,
        void const * volatile p_src, void const * volatile p_dest );

#endif /* __BSP_DMAC_H */
