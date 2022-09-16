#include "hal_data.h"
#include "oled.h"
#include "oledfont.h"
#include "debug_uart.h"
#include "stdio.h"
#include "mpu6050.h"
#include<math.h>

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


float aRes = 4.0/32768.0;



int cifang(int num,int a)
{
    int b = 1;
    for(int i = 0; i<a ;i++)
    {
       b = b*num;
    }
    return b;
}


void doubletochar(double val,char *C_num,int num)
{
    char char_num[13] = {'0','1','2','3','4','5','6','7','8','9',' ',};
    int str = 0;
    if(val < 0)
    {
        C_num[str] = '-';
        str ++;
    }

    //判断整数有多少个位
    int int_num = 1;
    int int_len = 0;
    while(fabs(val)>int_num)
    {
        int_num = int_num*10;
        int_len++;
    }

//    //判断小数有多少个位
    int double_len = 2; //最终保存的小数长度
//    double n1=fabs(val);
//    double y1;
//    int x1=(int)n1;
//    y1=n1-(double)x1;
//    while(y1 != 0) //0.858<0.1
//    {
//        n1 = n1*10;
//        x1=(int)n1;
//        y1=n1-(double)x1;
//        double_len++;
//    }



    //判断完整数赋值
    int now_ten_num;
    int int_c = fabs(val);
    if(int_len == 0) //如果小于1
    {
        C_num[str] = char_num[0];
        str ++;
    }else{                  //如果大与1
        while(int_len!=0)   //确定循环了3次
        {
            now_ten_num = cifang(10,int_len-1);
            int b = int_c/now_ten_num;
            int_c = int_c-(b*now_ten_num);

            C_num[str] = char_num[b];
            str ++;
            int_len --;
        }
    }

    //赋值小数点
    C_num[str] = '.';
    str ++;


    float double_num = fabs(val);
    int double_c = (int)double_num;
    double double_y_num =double_num-(double)double_c;
    //赋值小数部分
    if(double_len == 0) //如果小于1
    {
        str --;
        C_num[str] = char_num[10];
    }else{
        while(double_len != 0)   //小数赋值   先减去整数部分
        {
            double_len = double_len - 1;
            int rongqi;
            double_y_num = double_y_num*10;
            rongqi = double_y_num/1;
            double_y_num = double_y_num - rongqi;


            C_num[str] = char_num[rongqi];
            str ++;
        }
    }

    C_num[str] = '\0';
//    printf("Read test num = %s \r\n",C_num);

}

void hal_entry(void)
{
    Debug_UART_Init();
    MPU6050_Init();
    if (MPU6050ReadID() == 0)
    {
      printf("\r\n没有检测到MPU6050传感器！\r\n");
          while(1);
      }
    printf("\r\n检测到MPU6050传感器！\r\n");
    R_SPI_Open(&g_spi0_ctrl, &g_spi0_cfg);
    R_BSP_SoftwareDelay(200,BSP_DELAY_UNITS_MICROSECONDS);
    OLED_Init();
    OLED_Fill(0x00);
    OLED_8x16Str(0u,0u,(unsigned char *)"Read MPU6050");
    OLED_8x16Str(0,2, "X=");
    while(1)
    {
        short Acel[3];
        short Gyro[3];
        uint16_t    temp;
        short Temp;
        MPU6050ReadAcc(Acel);
        printf("加速度：%8d%8d%8d",Acel[0],Acel[1],Acel[2]);
        MPU6050ReadGyro(Gyro);
        printf("陀螺仪：%8d%8d%8d",Gyro[0],Gyro[1],Gyro[2]);
        MPU6050_ReturnTemp(&Temp);
        int temperature = ((float) Temp) / 340. + 36.53;
        printf("温度：%d\r\n",temperature);
        double a = Acel[2];
        char C[50] = {};
        doubletochar(a,C,2);
        OLED_8x16Str(16,2, (unsigned char *)C);
        printf("Read num = %s \r\n",C);
        R_BSP_SoftwareDelay(200,BSP_DELAY_UNITS_MILLISECONDS);

    }

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
