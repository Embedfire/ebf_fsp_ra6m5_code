#ifndef BSP_SDHI_H_
#define BSP_SDHI_H_
#include "hal_data.h"


extern __IO uint32_t g_transfer_complete;
extern __IO bool g_card_inserted;

extern sdmmc_device_t my_sdmmc_device;


void r_sdhi_basic_example (void);
void r_sdhi_card_detect_example (void);

#endif /* BSP_SDHI_H_ */
