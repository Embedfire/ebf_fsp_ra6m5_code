#include "mpu6050.h"



i2c_master_event_t state;
/* Callback function */
void sci_i2c_master_callback(i2c_master_callback_args_t *p_args)
{
    state = p_args->event;
}

/**
  * @brief   写数据到MPU6050寄存器
  * @param   
  * @retval  
  */
void MPU6050_WriteReg(uint8_t reg_add,uint8_t reg_dat)
{
    uint32_t  timeout_ms = 2;
    state = I2C_MASTER_EVENT_ABORTED;
    unsigned char send_buffer[2] = {};

    send_buffer[0] = reg_add;
    send_buffer[1] = reg_dat;
    R_SCI_I2C_Write(I2C6.p_ctrl, &send_buffer[0], 2, false); //每当写完数据 false 总线拉高


    /* Since there is nothing else to do, block until Callback triggers*/
    while ((I2C_MASTER_EVENT_TX_COMPLETE != state) && timeout_ms)
    {
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        timeout_ms--;;
    }
}


/**
  * @brief   从MPU6050寄存器读取数据
  * @param   
  * @retval  
  */
void MPU6050_ReadData(uint8_t reg_add,unsigned char*Read,uint8_t num)
{
    uint32_t  timeout_ms = 2;
    state = I2C_MASTER_EVENT_ABORTED;
    unsigned char send_buffer[2] = {};

    send_buffer[0] = reg_add;

    R_SCI_I2C_Write(I2C6.p_ctrl, &send_buffer[0], 1, true);
    R_BSP_SoftwareDelay(300,BSP_DELAY_UNITS_MICROSECONDS);        //这里必须增加个延时不然会出现错误

    R_SCI_I2C_Read(I2C6.p_ctrl, Read, num, false);
    R_BSP_SoftwareDelay(1000,BSP_DELAY_UNITS_MICROSECONDS);

    /* Since there is nothing else to do, block until Callback triggers*/
    while ((I2C_MASTER_EVENT_RX_COMPLETE != state) && timeout_ms)
    {
        R_BSP_SoftwareDelay(1U, BSP_DELAY_UNITS_MILLISECONDS);
        timeout_ms--;;
    }
}


/**
  * @brief   初始化MPU6050芯片
  * @param   
  * @retval  
  */
void MPU6050_Init(void)
{
  R_SCI_I2C_Open(&I2C6_ctrl, &I2C6_cfg);
  int i=0,j=0;
  //在初始化之前要延时一段时间，若没有延时，则断电后再上电数据可能会出错
  for(i=0;i<1000;i++)
  {
    for(j=0;j<1000;j++)
    {
      ;
    }
  }
	MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1, 0x00);	    //解除休眠状态
	MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);	    //陀螺仪采样率，1KHz
	MPU6050_WriteReg(MPU6050_RA_CONFIG , 0x06);	        //低通滤波器的设置，截止频率是1K，带宽是5K
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG , 0x00);	  //配置加速度传感器工作在2G模式，不自检
	MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);     //陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
}

/**
  * @brief   读取MPU6050的ID
  * @param   
  * @retval  
  */
uint8_t MPU6050ReadID(void)
{
	unsigned char Re = 0;
    MPU6050_ReadData(MPU6050_RA_WHO_AM_I,&Re,1);    //读器件地址
    if(Re != 0x68)
    {
        printf("MPU6050 dectected error!\r\n检测不到MPU6050模块，请检查模块与开发板的接线");
        return 0;
    }
    else
    {
        printf("MPU6050 ID = %d\r\n",Re);
        return 1;
    }
		
}
/**
  * @brief   读取MPU6050的加速度数据
  * @param   
  * @retval  
  */
void MPU6050ReadAcc(short *accData)
{
    uint8_t buf[6];
    MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6);
    accData[0] = (buf[0] << 8) | buf[1];
    accData[1] = (buf[2] << 8) | buf[3];
    accData[2] = (buf[4] << 8) | buf[5];
}

/**
  * @brief   读取MPU6050的角加速度数据
  * @param   
  * @retval  
  */
void MPU6050ReadGyro(short *gyroData)
{
    uint8_t buf[6];
    MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
    gyroData[0] = (buf[0] << 8) | buf[1];
    gyroData[1] = (buf[2] << 8) | buf[3];
    gyroData[2] = (buf[4] << 8) | buf[5];
}


/**
  * @brief   读取MPU6050的原始温度数据
  * @param   
  * @retval  
  */
void MPU6050ReadTemp(short *tempData)
{
	uint8_t buf[2];
    MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);     //读取温度值
    *tempData = (buf[0] << 8) | buf[1];
}


/**
  * @brief   读取MPU6050的温度数据，转化成摄氏度
  * @param   
  * @retval  
  */
void MPU6050_ReturnTemp(short *Temperature)
{
	uint8_t buf[2];
	
	MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);     //读取温度值
	*Temperature  = (buf[0] << 8) | buf[1];
	//*Temperature=( temp3/340.0)+36.53;
}
