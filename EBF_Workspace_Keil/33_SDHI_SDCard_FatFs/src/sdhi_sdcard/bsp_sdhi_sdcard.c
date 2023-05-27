#include "bsp_sdhi_sdcard.h"


__IO uint32_t g_transfer_complete = 0;
__IO uint32_t g_card_erase_complete = 0;
__IO bool g_card_inserted = false;

sdmmc_device_t my_sdmmc_device = {0};

/* SDHI SD卡初始化函数 */
void SDCard_Init(void)
{
    fsp_err_t err;
    
    /* 打开SDHI */
    err = R_SDHI_Open(&g_sdmmc0_ctrl, &g_sdmmc0_cfg);
    assert(FSP_SUCCESS == err);
}

void SDCard_DeInit(void)
{
    fsp_err_t err;
    
    /* 关闭SDHI */
    err = R_SDHI_Close(&g_sdmmc0_ctrl);
    assert(FSP_SUCCESS == err);
}


/* 如果启用了卡检测中断，则在发生卡检测事件时调用回调。 */
void g_sdmmc0_callback(sdmmc_callback_args_t *p_args)
{
    if (SDMMC_EVENT_TRANSFER_COMPLETE == p_args->event)  //读取或写入完成
    {
        g_transfer_complete = 1;
    }
    if (SDMMC_EVENT_CARD_INSERTED == p_args->event)  //卡插入中断
    {
        g_card_inserted = true;
    }
    if (SDMMC_EVENT_CARD_REMOVED == p_args->event)   //卡拔出中断
    {
        g_card_inserted = false;
    }
    if (SDMMC_EVENT_ERASE_COMPLETE == p_args->event)  //擦除完成
    {
        g_card_erase_complete = 1;
    }
    if (SDMMC_EVENT_ERASE_BUSY == p_args->event)  //擦除超时
    {
        g_card_erase_complete = 2;
    }
}

