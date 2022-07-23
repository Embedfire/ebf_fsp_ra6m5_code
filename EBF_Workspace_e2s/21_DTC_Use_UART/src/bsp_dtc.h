#ifndef BSP_DTC_H_
#define BSP_DTC_H_
#include "hal_data.h"


void set_transfer_dst_src_address(transfer_cfg_t const * const p_config,
        void const * volatile p_src, void const * volatile p_dest );
#endif /* BSP_DTC_H_ */
