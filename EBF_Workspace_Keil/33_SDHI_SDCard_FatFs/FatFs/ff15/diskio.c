/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
//#include "qspi_flash/bsp_qspi_flash.h"
#include "sdhi_sdcard/bsp_sdhi_sdcard.h"

//此FatFs底层IO驱动文件由野火修改

/* Definitions of physical drive number for each drive */
//#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
//#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#define DEV_FLASH       0
#define DEV_SDCARD      1


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;

	switch (pdrv) {
	case DEV_FLASH :
	    //QSPI_Flash_WaitForWriteEnd(); //等待Flash芯片内部操作完成

	    stat = RES_OK;
		return stat;

    case DEV_SDCARD :

	    stat = RES_OK;
		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
    sdmmc_status_t status;

	switch (pdrv) {
	case DEV_FLASH :
	    //QSPI_Flash_Init();

        stat = RES_OK;
		return stat;

    case DEV_SDCARD :
        /* 初始化SD卡：打开SDHI外设模块 */
        SDCard_Init();

        /* 检查SD卡是否插入 */
        R_SDHI_StatusGet(&g_sdmmc0_ctrl, &status);
        if (!status.card_inserted)
        {
            /* 等待卡插入中断 */
            while (!g_card_inserted)
            {
                printf("请插入SD卡\r\n");
                R_BSP_SoftwareDelay(1000, BSP_DELAY_UNITS_MILLISECONDS);
            }
            printf("\r\n检测到SD卡已插入\r\n");
        }

        /* 设备应在检测到VDD最小值后1ms内准备好接受第一个命令。参考SD物理层简化规范6.00版第6.4.1.1节“卡的通电时间”。 */
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        /* Initialize the SD card.  This should not be done until the card is plugged in for SD devices. */
        R_SDHI_MediaInit(&g_sdmmc0_ctrl, &my_sdmmc_device);

        stat = RES_OK;
		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;

	switch (pdrv) {
	case DEV_FLASH :
		// translate the arguments here

		//QSPI_Flash_BufferRead(buff, sector<<12, count<<12); //1 sector == 4096 bytes

		res = RES_OK;
		return res;

	case DEV_SDCARD :
        R_SDHI_Read(&g_sdmmc0_ctrl, buff, sector, count); //1 sector == 512 bytes
        while (g_transfer_complete == 0);
        g_transfer_complete = 0;

		res = RES_OK;
		return res;
	}
	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	uint32_t write_addr;

	switch (pdrv) {
	case DEV_FLASH :
	    write_addr = sector << 12;
		//QSPI_Flash_SectorErase(write_addr);
		//QSPI_Flash_BufferWrite(buff, write_addr, count<<12);

        res = RES_OK;
		return res;

	case DEV_SDCARD :
        R_SDHI_Write(&g_sdmmc0_ctrl, buff, sector, count);
        while (g_transfer_complete == 0);
        g_transfer_complete = 0;

        res = RES_OK;
		return res;
	}
	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	switch (pdrv) {
	case DEV_FLASH :
        switch (cmd) {
        case GET_SECTOR_COUNT:      /* 扇区数量：1024*4096/1024/1024 = 4(MB) */
            *(DWORD *)buff = 1024;
            break;
        case GET_SECTOR_SIZE:       /* 扇区大小  */
            *(WORD *)buff = 4096;
            break;
        case GET_BLOCK_SIZE:        /* 同时擦除扇区个数 */
            *(DWORD *)buff = 1;
            break;
        }

        res = RES_OK;
		return res;

	case DEV_SDCARD :
        switch (cmd)
        {
            // 扇区大小。仅当FF_MAX_SS > FF_MIN_SS时，才需要此命令。
            case GET_SECTOR_SIZE:       /* 扇区大小：512(Byte)  */
                *(WORD *)buff = (WORD)my_sdmmc_device.sector_size_bytes;
                break;
            // 擦除块大小（以扇区为单位）
            case GET_BLOCK_SIZE:        /* 同时擦除扇区个数 */
                *(DWORD *)buff = my_sdmmc_device.erase_sector_count;
                break;
            // 可用扇区数
            case GET_SECTOR_COUNT:      /* 扇区数量 */
                *(DWORD *)buff = my_sdmmc_device.sector_count;
                break;
        }

        res = RES_OK;
		return res;
	}

	return RES_PARERR;
}

