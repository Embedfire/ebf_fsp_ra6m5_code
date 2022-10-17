#ifndef _BSP_QSPI_FLASH_H_
#define _BSP_QSPI_FLASH_H_

#include "hal_data.h"
#include "bsp_uart.h"
#include <stdio.h>

//#define  sFLASH_ID              0xEF3015   //W25X16
//#define  sFLASH_ID              0xEF4015   //W25Q16
//#define  sFLASH_ID              0XEF4018   //W25Q128
//#define  sFLASH_ID              0XEF4017   //W25Q64
#define  sFLASH_ID              0xEF4016	//W25Q32
//#define  sFLASH_ID              0x1F8701     // AT25SF321B
//#define  sFLASH_ID              0xEF6016



#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256
#define RESET_VALUE                    (0x00)


/*命令定义-开头*******************************/
#define WriteEnable                 0x06
#define WriteDisable                0x04
#define ReadStatusReg               0x05
#define WriteStatusReg              0x01
#define ReadData                    0x03
#define FastReadData                0x0B
#define FastReadDual                0x3B
#define PageProgram                 0x02
#define BlockErase                  0xD8
#define SectorErase                 0x20
#define ChipErase                   0xC7
#define PowerDown                   0xB9
#define ReleasePowerDown            0xAB
#define DeviceID                    0xAB
#define ManufactDeviceID            0x90
#define JedecDeviceID               0x9F

/* WIP(busy)标志，FLASH内部正在写入 */
#define WIP_Flag                    0x01
#define Dummy_Byte                  0xFF
/*命令定义-结尾*******************************/



//static void qspi_d0_byte_write_standard (uint8_t byte);
void QSPI_FLASH_Init(void);
void QSPI_FLASH_SectorErase(uint32_t adress);
void QSPI_FLASH_PageWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite); //页写入
void QSPI_FLASH_BufferWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void QSPI_FLASH_BufferRead(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t QSPI_FLASH_ReadID(void);
uint32_t SPI_FLASH_ReadDeviceID(void);
void QSPI_Flash_PowerDown(void);
void QSPI_Flash_WAKEUP(void);


void QSPI_FLASH_WriteEnable(void);
void get_flash_status(void);
fsp_err_t R_QSPI_Read(spi_flash_ctrl_t    *p_ctrl, uint8_t    *p_src, uint8_t *const  p_dest, uint32_t  byte_count);
fsp_err_t QSPI_FLASH_WaitForWriteEnd(void);



#endif
