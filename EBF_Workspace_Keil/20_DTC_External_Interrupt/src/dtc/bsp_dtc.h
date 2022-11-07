#ifndef __BSP_DTC_H
#define __BSP_DTC_H
#include "hal_data.h"


// 要发送的数据大小
#define BUFFER_SIZE     32


void DTC_Init(void);

void set_transfer_dst_src_address(transfer_cfg_t const * const p_config,
        void const * volatile p_src, void const * volatile p_dest );
#endif /* __BSP_DTC_H */
