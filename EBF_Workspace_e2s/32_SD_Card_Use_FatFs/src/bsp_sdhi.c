#include "bsp_sdhi.h"



/*
 * const sdmmc_api_t g_sdmmc_on_sdhi =
 * {
 *     .open        = R_SDHI_Open,
 *     .mediaInit   = R_SDHI_MediaInit,
 *     .read        = R_SDHI_Read,
 *     .write       = R_SDHI_Write,
 *     .readIo      = R_SDHI_ReadIo,
 *     .writeIo     = R_SDHI_WriteIo,
 *     .readIoExt   = R_SDHI_ReadIoExt,
 *     .writeIoExt  = R_SDHI_WriteIoExt,
 *     .ioIntEnable = R_SDHI_IoIntEnable,
 *     .statusGet   = R_SDHI_StatusGet,
 *     .erase       = R_SDHI_Erase,
 *     .callbackSet = R_SDHI_CallbackSet,
 *     .close       = R_SDHI_Close,
 * };
 */

uint8_t g_dest[SDHI_MAX_BLOCK_SIZE] BSP_ALIGN_VARIABLE(4);  //4字节对齐
uint8_t g_src[SDHI_MAX_BLOCK_SIZE]  BSP_ALIGN_VARIABLE(4);
__IO uint32_t g_transfer_complete = 0;

__IO bool g_card_inserted = false;



sdmmc_device_t my_sdmmc_device = {0};


void r_sdhi_basic_example (void)
{
    /* Initialize g_src to known data */
    for (uint32_t i = 0; i < SDHI_MAX_BLOCK_SIZE; i++)
    {
        g_src[i] = (uint8_t) (1 + (i % 26));
    }
    /* Open the SDHI driver. */
    fsp_err_t err = g_sdmmc_on_sdhi.open(&g_sdmmc0_ctrl, &g_sdmmc0_cfg);
    assert(FSP_SUCCESS == err);

    /* 设备应在检测到VDD最小值后1ms内准备好接受第一个命令。参考SD物理层简化规范6.00版第6.4.1.1节“卡的通电时间”。 */
    R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
    /* Initialize the SD card.  This should not be done until the card is plugged in for SD devices. */
    err = g_sdmmc_on_sdhi.mediaInit(&g_sdmmc0_ctrl, &my_sdmmc_device);
    assert(FSP_SUCCESS == err);

    err = g_sdmmc_on_sdhi.write(&g_sdmmc0_ctrl, g_src, 1, 1);
    assert(FSP_SUCCESS == err);
    while (g_transfer_complete == 0);
    g_transfer_complete = 0;

    err = g_sdmmc_on_sdhi.read(&g_sdmmc0_ctrl, g_dest, 1, 1);
    assert(FSP_SUCCESS == err);
    while (g_transfer_complete == 0);
    g_transfer_complete = 0;
}


void r_sdhi_card_detect_example (void)
{
    /* Open the SDHI driver. This enables the card detection interrupt. */
    fsp_err_t err = R_SDHI_Open(&g_sdmmc0_ctrl, &g_sdmmc0_cfg);
    /* Handle any errors. This function should be defined by the user. */
    assert(FSP_SUCCESS == err);
    /* Check if card is inserted. */
    sdmmc_status_t status;
    err = R_SDHI_StatusGet(&g_sdmmc0_ctrl, &status);
    assert(FSP_SUCCESS == err);
    if (!status.card_inserted)
    {
        while (!g_card_inserted)
        {
            /* Wait for a card insertion interrupt. */
        }
    }
    /* A device shall be ready to accept the first command within 1ms from detecting VDD min. Reference section 6.4.1.1
     * "Power Up Time of Card" in the SD Physical Layer Simplified Specification Version 6.00. */
    R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
    /* Initialize the SD card after card insertion is detected. */
    err = R_SDHI_MediaInit(&g_sdmmc0_ctrl, NULL);
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
}

