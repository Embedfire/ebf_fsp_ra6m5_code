#include "bsp_qspi_flash.h"

static void qspi_d0_byte_write_standard(uint8_t byte)
{
    R_QSPI->SFMCOM = byte;
}

void QSPI_FLASH_Init(void)
{
    R_QSPI_Open(Flash.p_ctrl, Flash.p_cfg);
		R_BSP_SoftwareDelay(10u,BSP_DELAY_UNITS_MILLISECONDS);
}

/**
* @brief  读取FLASH ID
* @param  无
* @retval FLASH ID
*/
uint32_t QSPI_FLASH_ReadID(void)
{
    unsigned char data[6] = {};
    uint32_t back;
    data[0] = JedecDeviceID;

    R_QSPI_DirectWrite(Flash.p_ctrl, &data[0], 1, true);     //false: close the spi  true: go go go
    R_QSPI_DirectRead(Flash.p_ctrl, &data[0], 3);

    /*把数据组合起来，作为函数的返回值*/
    back = (data[0] << 16) | (data[1] << 8) | (data[2]);

    return back;
}

uint32_t QSPI_FLASH_ReadDeviceID(void)
{
    unsigned char data[6] = {};
    uint32_t back;
    data[0] = DeviceID;
    data[1] = 0xff;
    data[2] = 0xff;
    data[3] = 0xff;
    R_QSPI_DirectWrite(Flash.p_ctrl, &data[0], 4, true);     //false: close the spi  true: go go go
    R_QSPI_DirectRead(Flash.p_ctrl, &data[0], 3);

    /*把数据组合起来，作为函数的返回值*/
    back = (data[0] << 16) | (data[1] << 8) | (data[2]);

    return back;
}



//进入掉电模式
void QSPI_Flash_PowerDown(void)
{
    unsigned char data[6] = {};
    data[0] = PowerDown;
    R_QSPI_DirectWrite(Flash.p_ctrl, &data[0], 1, false);
}

//唤醒
void QSPI_Flash_WAKEUP(void)
{
    unsigned char data[6] = {};
    data[0] = ReleasePowerDown;
    R_QSPI_DirectWrite(Flash.p_ctrl, &data[0], 1, false);
}

/**
 * @brief  向FLASH发送 写使能 命令
 * @param  none
 * @retval none
 */
void QSPI_FLASH_WriteEnable(void)
{
    unsigned char data[6] = {};
    data[0] = WriteEnable;
    R_QSPI_DirectWrite(Flash.p_ctrl, &data[0], 1, false);
}

void get_flash_status(void)
{
	QSPI_FLASH_WaitForWriteEnd();
//	    spi_flash_status_t  status;
//    while (false != status.write_in_progress)
//    {
//        R_QSPI_StatusGet(Flash.p_ctrl, &status);
//    }
}


fsp_err_t QSPI_FLASH_WaitForWriteEnd(void)
{
    spi_flash_status_t status = {.write_in_progress = true};
    int32_t time_out          = (INT32_MAX);
    fsp_err_t err             = FSP_SUCCESS;

    do
    {
        /* Get status from QSPI flash device */
        err = R_QSPI_StatusGet(Flash.p_ctrl, &status);
        if (FSP_SUCCESS != err)
        {
            printf("R_QSPI_StatusGet Failed\r\n");
            return err;
        }

        /* Decrement time out to avoid infinite loop in case of consistent failure */
        --time_out;

        if (RESET_VALUE >= time_out)
        {
            printf("\r\n ** Timeout : No result from QSPI flash status register ** \r\n");
            return FSP_ERR_TIMEOUT;
        }

    }
    while (false != status.write_in_progress);

    return err;
}



fsp_err_t R_QSPI_Read(spi_flash_ctrl_t     *p_ctrl,
                      uint8_t              *p_src,
                      uint8_t *const       p_dest,
                      uint32_t              byte_count)
{
    qspi_instance_ctrl_t *p_instance_ctrl = (qspi_instance_ctrl_t *) p_ctrl;


    uint32_t chip_address = (uint32_t) p_dest - (uint32_t) QSPI_DEVICE_START_ADDRESS + R_QSPI->SFMCNT1;

    bool restore_spi_mode = false;
    void (* write_command)(uint8_t byte) = qspi_d0_byte_write_standard;
    void (* write_address)(uint8_t byte) = qspi_d0_byte_write_standard;

#if QSPI_CFG_SUPPORT_EXTENDED_SPI_MULTI_LINE_PROGRAM

    /* If the peripheral is in extended SPI mode, and the configuration provided in the BSP allows for programming on
     * multiple data lines, and a unique command is provided for the required mode, update the SPI protocol to send
     * data on multiple lines. */
    if ((SPI_FLASH_DATA_LINES_1 != p_instance_ctrl->data_lines) &&
            (SPI_FLASH_PROTOCOL_EXTENDED_SPI == R_QSPI->SFMSPC_b.SFMSPI))
    {
        R_QSPI->SFMSPC_b.SFMSPI = p_instance_ctrl->data_lines;

        restore_spi_mode = true;

        /* Write command in extended SPI mode on one line. */
        write_command = gp_qspi_prv_byte_write[p_instance_ctrl->data_lines];

        if (SPI_FLASH_DATA_LINES_1 == p_instance_ctrl->p_cfg->page_program_address_lines)
        {
            /* Write address in extended SPI mode on one line. */
            write_address = gp_qspi_prv_byte_write[p_instance_ctrl->data_lines];
        }
    }
#endif

    /* Enter Direct Communication mode */
    R_QSPI->SFMCMD = 1;

    /* Send command to enable writing */
    write_command(0x03);

    /* Write the address. */
    if ((p_instance_ctrl->p_cfg->address_bytes & R_QSPI_SFMSAC_SFMAS_Msk) == SPI_FLASH_ADDRESS_BYTES_4)
    {
        /* Send the most significant byte of the address */
        write_address((uint8_t)(chip_address >> 24));
    }

    /* Send the remaining bytes of the address */
    write_address((uint8_t)(chip_address >> 16));
    write_address((uint8_t)(chip_address >> 8));
    write_address((uint8_t)(chip_address));

    /* Write the data. */
    uint32_t index = 0;
    while (index < byte_count)
    {
        /* Read the device memory into the passed in buffer */
        *(p_src + index) = (uint8_t) R_QSPI->SFMCOM;
        index++;
    }

    /* Close the SPI bus cycle. Reference section 39.10.3 "Generating the SPI Bus Cycle during Direct Communication"
     * in the RA6M3 manual R01UH0886EJ0100. */
    R_QSPI->SFMCMD = 1;


    /* Return to ROM access mode */
    R_QSPI->SFMCMD = 0;

    return FSP_SUCCESS;
}


/**
 * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
 * @param  pBuffer，要写入数据的指针
 * @param  WriteAddr，写入地址
 * @param  NumByteToWrite，写入数据长度
 * @retval 无
 */
void QSPI_FLASH_BufferWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    /*mod运算求余，若writeAddr是SPI_FLASH_PageSize整数倍，运算结果Addr值为0*/
    Addr = WriteAddr % SPI_FLASH_PageSize;

    /*差count个数据值，刚好可以对齐到页地址*/
    count = SPI_FLASH_PageSize - Addr;
    /*计算出要写多少整数页*/
    NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
    /*mod运算求余，计算出剩余不满一页的字节数*/
    NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

    /* Addr=0,则WriteAddr 刚好按页对齐 aligned  */
    if (Addr == 0)
    {
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if (NumOfPage == 0)
        {
            R_QSPI_Write(&Flash_ctrl, pBuffer, WriteAddr, NumByteToWrite);
            get_flash_status();

        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            /*先把整数页都写了*/
            while (NumOfPage--)
            {
                R_QSPI_Write(&Flash_ctrl, pBuffer, WriteAddr, SPI_FLASH_PageSize);
                get_flash_status();

                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }
            /*若有多余的不满一页的数据，把它写完*/
            R_QSPI_Write(&Flash_ctrl, pBuffer, WriteAddr, NumOfSingle);
            get_flash_status();

        }
    }
    /* 若地址与 SPI_FLASH_PageSize 不对齐  */
    else
    {
        /* NumByteToWrite < SPI_FLASH_PageSize */
        if (NumOfPage == 0)
        {
            /*当前页剩余的count个位置比NumOfSingle小，一页写不完*/
            if (NumOfSingle > count)
            {
                temp = NumOfSingle - count;
                /*先写满当前页*/
                R_QSPI_Write(&Flash_ctrl, pBuffer, WriteAddr, count);
                get_flash_status();


                WriteAddr +=  count;
                pBuffer += count;
                /*再写剩余的数据*/
                R_QSPI_Write(&Flash_ctrl, pBuffer, WriteAddr, temp);
                get_flash_status();

            }
            else /*当前页剩余的count个位置能写完NumOfSingle个数据*/
            {
                R_QSPI_Write(&Flash_ctrl, pBuffer, WriteAddr, NumByteToWrite);
                get_flash_status();

            }
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            /*地址不对齐多出的count分开处理，不加入这个运算*/
            NumByteToWrite -= count;
            NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
            NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

            /* 先写完count个数据，为的是让下一次要写的地址对齐 */
            R_QSPI_Write(&Flash_ctrl, pBuffer, WriteAddr, count);
            get_flash_status();

            /* 接下来就重复地址对齐的情况 */
            WriteAddr +=  count;
            pBuffer += count;
            /*把整数页都写了*/
            while (NumOfPage--)
            {
                R_QSPI_Write(&Flash_ctrl, pBuffer, WriteAddr, SPI_FLASH_PageSize);
                get_flash_status();

                WriteAddr +=  SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }
            /*若有多余的不满一页的数据，把它写完*/
            if (NumOfSingle != 0)
            {
                R_QSPI_Write(&Flash_ctrl, pBuffer, WriteAddr, NumOfSingle);
                get_flash_status();

            }
        }
    }
}


/**
* @brief  擦除FLASH扇区
* @param  SectorAddr：要擦除的扇区地址
* @retval 无
*/
void QSPI_FLASH_SectorErase(uint32_t adress)
{
    unsigned char data[6] = {};

    data[0] = 0x06;     //write_enable_command
    data[1] = 0x20;     //erase_command
    data[2] = (uint8_t)(adress >> 16);
    data[3] = (uint8_t)(adress >> 8);
    data[4] = (uint8_t)(adress);
    R_QSPI->SFMCMD = 1U;
    R_QSPI->SFMCOM = data[0];
    R_QSPI_DirectWrite(Flash.p_ctrl, &data[1], 4, false);

    QSPI_FLASH_WaitForWriteEnd();
}


/**
* @brief  对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
* @param  pBuffer，要写入数据的指针
* @param WriteAddr，写入地址
* @param  NumByteToWrite，写入数据长度，必须小于等于页大小
* @retval 无
*/
void QSPI_FLASH_PageWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    R_QSPI_Write(&Flash_ctrl, pBuffer, WriteAddr, NumByteToWrite);
    get_flash_status();
}

/**
* @brief  读取FLASH数据
* @param  pBuffer，存储读出数据的指针
* @param   ReadAddr，读取地址
* @param   NumByteToRead，读取数据长度
* @retval 无
*/
void QSPI_FLASH_BufferRead(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    R_QSPI_Read(&Flash_ctrl, pBuffer, ReadAddr, NumByteToRead);
}
