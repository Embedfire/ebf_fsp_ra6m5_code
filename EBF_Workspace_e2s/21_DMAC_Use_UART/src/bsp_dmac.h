#ifndef BSP_DMAC_H_
#define BSP_DMAC_H_
#include "hal_data.h"


void bsp_dmac_init(void);
void set_transfer_length(transfer_cfg_t const * p_config, volatile uint16_t _length);
void set_transfer_dst_src_address(transfer_cfg_t const * const p_config,
        const volatile uint8_t * _p_src, const volatile uint8_t * _p_dest);
#endif /* BSP_DMAC_H_ */
