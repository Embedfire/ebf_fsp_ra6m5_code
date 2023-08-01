#include "hal_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


/* 用户头文件包含 */
#include "led/bsp_led.h"
#include "debug_uart/bsp_debug_uart.h"

//FatFs
#include "FatFs/ff15/ff.h"


FATFS fs;           /* FatFs文件系统对象 */
FIL fnew;           /* 文件对象 */
UINT fnum;          /* 文件成功读写数量 */
FRESULT res_flash;  /* 文件操作结果 */
BYTE  fileReadBuffer[1024]; /* 读缓冲区 */
BYTE  fileWriteBuffer[] =   /* 写缓冲区 */
        "感谢您选用野火启明瑞萨RA开发板 [FatFs读写测试文件.txt]";

BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */


/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    /* TODO: add your own code here */

    LED_Init();         // LED 初始化
    Debug_UART4_Init(); // SCI4 UART 调试串口初始化

    printf("这是一个串行FLASH的FatFs使用演示例程\r\n");
    printf("打开串口助手查看打印的信息\r\n\r\n");

    /* 尝试挂载外部FLASH FAT文件系统 */
    res_flash = f_mount(&fs, "0:", 1);
    if (res_flash == FR_NO_FILESYSTEM)
    {
        printf(">>> FLASH还没有文件系统，即将进行格式化...\r\n");
        /* 格式化 */
        res_flash = f_mkfs("0:", NULL, work, sizeof(work));
        if (res_flash == FR_OK)
        {
            printf(">>> FLASH已成功格式化文件系统。\r\n");
            /* 格式化后，先取消挂载 */
            res_flash = f_mount(NULL,"0:",1);
            /* 重新挂载 */
            res_flash = f_mount(&fs,"0:",1);
        }
        else
        {
            printf(">>> 格式化失败！！\r\n");
            while (1);
        }
    }
    else if (res_flash == FR_OK)
    {
        printf(">>> 文件系统挂载成功，可以进行读写测试。\r\n");
    }
    else
    {
        printf("！！外部Flash挂载文件系统失败。(%d)\r\n", res_flash);
        printf("！！可能原因：Flash初始化不成功。\r\n");
        while (1);
    }
    

    /*----------------------- 文件系统测试：写测试 -----------------------------*/
    printf("\r\n****** 即将进行文件写入测试 ******\r\n");

    /* 打开文件，如果文件不存在则创建它 */
    res_flash = f_open(&fnew, "0:FatFs读写测试文件.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if ( res_flash == FR_OK )
    {
        printf(">>> 打开/创建“FatFs读写测试文件.txt”文件成功，向文件写入数据。\r\n");
        /* 将指定存储区内容写入到文件内 */
        res_flash = f_write(&fnew, fileWriteBuffer, sizeof(fileWriteBuffer), &fnum);
        if (res_flash==FR_OK) {
            printf(">>> 文件写入成功，写入字节数据：%d\r\n", fnum);
            printf(">>> 向文件写入的数据为：%s\r\n", fileWriteBuffer);
        } else {
            printf("！！文件写入失败：(%d)\n",res_flash);
        }
        /* 不再读写，关闭文件 */
        f_close(&fnew);
    }
    else
    {
        printf("！！打开/创建文件失败。\r\n");
    }


    /*----------------------- 文件系统测试：读测试 -----------------------------*/
    printf("****** 即将进行文件读取测试 ******\r\n");

    /* 打开文件，该文件前面已创建 */
    res_flash = f_open(&fnew, "0:FatFs读写测试文件.txt", FA_OPEN_EXISTING | FA_READ);
    if (res_flash == FR_OK)
    {
        printf(">>> 打开文件成功。\r\n");
        res_flash = f_read(&fnew, fileReadBuffer, sizeof(fileReadBuffer), &fnum);
        if (res_flash==FR_OK) {
            printf(">>> 文件读取成功，读到字节数据：%d\r\n", fnum);
            printf(">>> 读取得的文件数据为：%s\r\n", fileReadBuffer);
        } else {
            printf("！！文件读取失败：(%d)\n",res_flash);
        }
        /* 不再读写，关闭文件 */
        f_close(&fnew);
    }
    else
    {
        printf("！！打开文件失败。\r\n");
    }

    /* 不再使用文件系统，取消挂载文件系统 */
    f_mount(NULL,"0:",1);

    printf("****** 测试结束 ******\r\n");
    while(1);


#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
    if (BSP_WARM_START_RESET == event)
    {
#if BSP_FEATURE_FLASH_LP_VERSION != 0

        /* Enable reading from data flash. */
        R_FACI_LP->DFLCTL = 1U;

        /* Would normally have to wait tDSTOP(6us) for data flash recovery. Placing the enable here, before clock and
         * C runtime initialization, should negate the need for a delay since the initialization will typically take more than 6us. */
#endif
    }

    if (BSP_WARM_START_POST_C == event)
    {
        /* C runtime environment and system clocks are setup. */

        /* Configure pins. */
        R_IOPORT_Open (&g_ioport_ctrl, g_ioport.p_cfg);
    }
}

#if BSP_TZ_SECURE_BUILD

BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ();

/* Trustzone Secure Projects require at least one nonsecure callable function in order to build (Remove this if it is not required to build). */
BSP_CMSE_NONSECURE_ENTRY void template_nonsecure_callable ()
{

}
#endif