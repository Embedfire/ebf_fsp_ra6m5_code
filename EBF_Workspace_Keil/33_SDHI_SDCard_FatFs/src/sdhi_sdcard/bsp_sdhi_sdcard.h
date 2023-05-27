#ifndef __BSP_SDHI_SDCARD_H
#define	__BSP_SDHI_SDCARD_H

#include "hal_data.h"
#include "stdio.h"

/*ÉùÃ÷±äÁ¿*/
extern __IO uint32_t g_transfer_complete;
extern __IO bool g_card_inserted;

extern sdmmc_device_t my_sdmmc_device;

void SDCard_Init(void);
void SDCard_DeInit(void);


#endif
