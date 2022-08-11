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


/* 为每个设备定义一个物理编号 */
#define ATA          0  // SD 卡
#define QSPI_FLASH   1  // 外部 QSPI Flash

// SD 卡块大小
#define SD_BLOCKSIZE 512


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = 0;
	fsp_err_t err;

	switch (pdrv) {
	case ATA :  /* SD CARD */

        /* 设备应在检测到VDD最小值后1ms内准备好接受第一个命令。参考SD物理层简化规范6.00版第6.4.1.1节“卡的通电时间”。 */
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        /* Initialize the SD card.  This should not be done until the card is plugged in for SD devices. */
        err = g_sdmmc_on_sdhi.mediaInit(&g_sdmmc0_ctrl, &my_sdmmc_device);
        assert(FSP_SUCCESS == err);

	    stat = RES_OK;
		return stat;

	case QSPI_FLASH :  /* QSPI FLASH */
		break;
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
    fsp_err_t err;

    switch (pdrv) {
    case ATA :  /* SD CARD */

        /* Open the SDHI driver. */
        err = g_sdmmc_on_sdhi.open(&g_sdmmc0_ctrl, &g_sdmmc0_cfg);
        assert(FSP_SUCCESS == err);

        stat = RES_OK;
        return stat;

    case QSPI_FLASH :  /* QSPI FLASH */
        break;
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
    DRESULT res = RES_PARERR;
    fsp_err_t err;

    switch (pdrv) {
    case ATA :  /* SD CARD */
        err = g_sdmmc_on_sdhi.read(&g_sdmmc0_ctrl, buff, sector, count);
        assert(FSP_SUCCESS == err);
        while (g_transfer_complete == 0);
        g_transfer_complete = 0;

        res = RES_OK;
        return res;

    case QSPI_FLASH :  /* QSPI FLASH */
        break;
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
	DRESULT res = RES_PARERR;
    fsp_err_t err;

    switch (pdrv) {
    case ATA :  /* SD CARD */
        err = g_sdmmc_on_sdhi.write(&g_sdmmc0_ctrl, buff, sector, count);
        assert(FSP_SUCCESS == err);
        while (g_transfer_complete == 0);
        g_transfer_complete = 0;

        res = RES_OK;
        return res;

    case QSPI_FLASH :  /* QSPI FLASH */
        break;
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
    case ATA :  /* SD CARD */
        switch (cmd)
        {
            // 扇区大小。仅当FF_MAX_SS > FF_MIN_SS时，才需要此命令。
            case GET_SECTOR_SIZE :
                *(WORD * )buff = (WORD)my_sdmmc_device.sector_size_bytes;
            break;
            // 擦除块大小（以扇区为单位）
            case GET_BLOCK_SIZE :
                *(DWORD * )buff = my_sdmmc_device.erase_sector_count;
            break;
            //可用扇区数
            case GET_SECTOR_COUNT:
                *(DWORD * )buff = my_sdmmc_device.sector_count;
                break;
            case CTRL_SYNC :
            break;
        }

        res = RES_OK;
        return res;

    case QSPI_FLASH :  /* QSPI FLASH */
        break;
    }

	return RES_PARERR;
}


//DWORD get_fattime(void) {
//    /* 返回当前时间戳 */
//
//    return (DWORD)(2022 - 80) << 25 |  /* Year */
//           (DWORD)(1 + 1) << 21 |      /* Month */
//           (DWORD)1 << 16 |            /* Mday */
//           (DWORD)1 << 11 |            /* Hour */
//           (DWORD)1 << 5 |             /* Min */
//           (DWORD)1 >> 1;              /* Sec */
//}

