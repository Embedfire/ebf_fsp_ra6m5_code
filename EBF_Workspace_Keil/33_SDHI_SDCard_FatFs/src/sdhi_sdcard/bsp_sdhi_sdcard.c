#include "bsp_sdhi_sdcard.h"
#include "fatfs/ff.h"
#include "led/bsp_led.h"


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


MKFS_PARM f_opt = {
    .fmt = FM_FAT32,       //格式选项
    .n_fat = 0,     //FATs大小
    .align = 0,     //数据区域对齐（扇区）
    .n_root = 0,    //根目录条目数
    .au_size = 0,   //群集大小（字节）
};
FATFS fs;                         /* FatFs文件系统对象 */
FIL fnew;                         /* 文件对象 */
FRESULT res_sd;                   /* 文件操作结果 */
UINT fnum;                        /* 文件成功读写数量 */
BYTE ReadBuffer[1024]={0};        /* 读缓冲区 */
BYTE WriteBuffer[] =              /* 写缓冲区*/
        "欢迎使用野火_瑞萨启明6M5开发板，这是一个文件系统测试文件\r\n";
BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */


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

void SDCard_FatFs(void)
{
    fsp_err_t err;
    sdmmc_status_t status;
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
            printf("请插入SD卡\r\n");
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

        res_sd = f_mount(&fs, "0:", 1);

        /*----------------------- 格式化测试 ---------------------------*/
        /* 如果没有文件系统就格式化创建创建文件系统 */
        if(res_sd == FR_NO_FILESYSTEM)
        {
            printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
            /* 格式化 */
            res_sd = f_mkfs("0:", NULL, work, sizeof(work));

            if(res_sd == FR_OK)
            {
                printf("》SD卡已成功格式化文件系统。\r\n");
                /* 格式化后，先取消挂载 */
                res_sd = f_mount(NULL, "0:", 1);
                /* 重新挂载   */
                res_sd = f_mount(&fs, "0:", 1);
            }
            else
            {
                LED1_ON;  //红灯亮
                printf("《《格式化失败。》》\r\n");
                while(1);
            }

            res_sd = f_setlabel("0:FATFS TEST");  //设置盘符的名称为“FATFS TEST”
            if(res_sd == FR_OK)
            {
                printf("》设置盘符的名称为“FATFS TEST”。\r\n");
            }
            else
            {
                LED1_ON;  //红灯亮
                printf("《《设置盘符的名称失败。》》\r\n");
                while(1);
            }
        }
        else if(res_sd != FR_OK)
        {
            LED1_ON;      //红灯亮
            printf("！！SD卡挂载文件系统失败。(%d)\r\n", res_sd);
            printf("！！可能原因：SD卡初始化不成功。\r\n");
            while(1);
        }
        else
        {
            printf("》文件系统挂载成功，可以进行读写测试\r\n");
        }

        /*----------------------- 文件系统测试：写测试 -----------------------------*/
        /* 打开文件，如果文件不存在则创建它 */
        printf("\r\n****** 即将进行文件写入测试... ******\r\n");
        // 创建新文件。如果该文件存在，则覆盖。  |
        // 可以写文件
        res_sd = f_open(&fnew, "0:FatFs读写测试文件.txt", FA_CREATE_ALWAYS | FA_WRITE);  //FatFs读写测试文件 FatFs_files
        if ( res_sd == FR_OK )
        {
            printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
            /* 将指定存储区内容写入到文件内 */
            res_sd = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);
            if(res_sd == FR_OK)
            {
                printf("》文件写入成功，写入字节数据：%d\n", fnum);
                printf("》向文件写入的数据为：\r\n%s\r\n", WriteBuffer);
            }
            else
            {
                LED1_ON;  //红灯亮
                printf("！！文件写入失败：(%d)\n", res_sd);
            }
            /* 不再写，关闭文件 */
            f_close(&fnew);
        }
        else
        {
            LED1_ON;  //红灯亮
            printf("！！打开/创建文件失败。\r\n");
            while(1);
        }

        /*------------------- 文件系统测试：读测试 ------------------------------------*/
        printf("****** 即将进行文件读取测试... ******\r\n");
        res_sd = f_open(&fnew, "0:FatFs读写测试文件.txt", FA_OPEN_EXISTING | FA_READ);
        if(res_sd == FR_OK)
        {
            printf("》打开文件成功。\r\n");
            res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
            if(res_sd == FR_OK)
            {
                printf("》文件读取成功,读到字节数据：%d\r\n", fnum);
                printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);
            }
            else
            {
                LED1_ON;  //红灯亮
                printf("！！文件读取失败：(%d)\n", res_sd);
            }
        }
        else
        {
            LED1_ON;  //红灯亮
            printf("！！打开文件失败。\r\n");
            while(1);
        }
        /* 不再读，关闭文件 */
        f_close(&fnew);

        /* 不再使用文件系统，取消挂载文件系统 */
        f_mount(NULL, "0:", 1);

}


