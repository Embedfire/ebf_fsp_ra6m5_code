#include "bsp_sdhi_sdcard.h"
#include "stdio.h"


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


__IO uint32_t g_transfer_complete = 0;
__IO uint32_t g_card_erase_complete = 0;
__IO bool g_card_inserted = false;


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



uint8_t g_dest[SDHI_MAX_BLOCK_SIZE] BSP_ALIGN_VARIABLE(4);  //4字节对齐
uint8_t g_src[SDHI_MAX_BLOCK_SIZE]  BSP_ALIGN_VARIABLE(4);


void SDCard_Operation(void)
{
    fsp_err_t err;
    sdmmc_status_t status;
    sdmmc_device_t my_sdmmc_device = {0};
    uint32_t i;


    /* 初始化要传输到SD卡内的源数组 */
    for (i = 0; i < SDHI_MAX_BLOCK_SIZE; i++)
    {
        g_src[i] = (uint8_t)('A' + (uint8_t)(i % 26));
    }
    
    /* 检查卡是否插入 */
    err = R_SDHI_StatusGet(&g_sdmmc0_ctrl, &status);
    assert(FSP_SUCCESS == err);
    if (!status.card_inserted)
    {
        /* 等待卡插入中断 */
        while (!g_card_inserted)
        {
            printf("\r\n请插入SD卡\r\n");
            R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
        }
        printf("\r\n检测到卡已插入\r\n");
    }
    
    /* 设备应在检测到VDD最小值后1ms内准备好接受第一个命令。
       参考SD物理层简化规范6.00版第6.4.1.1节“卡的通电时间”。
    */
    R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
    
    /* 初始化SD卡。在为SD设备插入卡之前，不应执行此操作。 */
    err = R_SDHI_MediaInit(&g_sdmmc0_ctrl, &my_sdmmc_device);
    assert(FSP_SUCCESS == err);

    /* 写入数据 */
    err = R_SDHI_Write(&g_sdmmc0_ctrl, g_src, 1, 1);
    assert(FSP_SUCCESS == err);
    while (g_transfer_complete == 0);
    g_transfer_complete = 0;

    /* 读出数据 */
    err = R_SDHI_Read(&g_sdmmc0_ctrl, g_dest, 1, 1);
    assert(FSP_SUCCESS == err);
    while (g_transfer_complete == 0);
    g_transfer_complete = 0;

    /* 打印数据 */
    for (i = 0; i < SDHI_MAX_BLOCK_SIZE; i++)
    {
        if (i % 26 == 0)
            printf(" ");
        printf("%c", g_dest[i]);
    }

    /* 对比数据 */
    if (strncmp((char*)&g_dest[0], (char*)&g_src[0], SDHI_MAX_BLOCK_SIZE) == 0)
    {
        printf("\r\nSD卡读写数据成功!\r\n");
    }
    else
    {
        printf("\r\nSD卡读写数据错误!\r\n");
    }
}


