#include "hal_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


/* 用户头文件包含 */
#include "led/bsp_led.h"
#include "debug_uart/bsp_debug_uart.h"
#include "dtc/bsp_dtc.h"


extern const uint32_t aSRC_Buffer[BUFFER_SIZE];
extern uint32_t aDST_Buffer[BUFFER_SIZE];
volatile bool dtc_start_transmission_sign = false;

uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);


/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    /* TODO: add your own code here */

    uint8_t TransferStatus;

    LED_Init();         // LED 初始化
    Debug_UART4_Init(); // SCI4 UART 调试串口初始化

    printf("\r\n实验3：DTC外部中断触发传输\r\n");
    printf("- DTC数据传输失败，则 LED1 亮（红色）\r\n");
    printf("- DTC数据传输成功，则 LED2 亮（蓝色）\r\n");

    /* 开启外部中断 */
    R_ICU_ExternalIrqOpen(&g_external_irq10_ctrl, &g_external_irq10_cfg);
    R_ICU_ExternalIrqEnable(&g_external_irq10_ctrl);

    /* 初始化DTC模块 */
    DTC_Init();

    /* 使能DTC模块 */
    R_DTC_Enable(&g_transfer_dtc_ctrl);

    /* 等待按键按下，激活DTC传输 */
    while (dtc_start_transmission_sign == false);
    dtc_start_transmission_sign = false;
    //R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);

    /* 比较源数据与传输后数据 */
    TransferStatus = Buffercmp(aSRC_Buffer, aDST_Buffer, BUFFER_SIZE);

    /* 判断源数据与传输后数据比较结果 */
    if(TransferStatus == 0)
    {
        /* 源数据与传输后数据不相等时，LED1 亮（红色），表示传输失败 */
        LED1_ON;
    }
    else
    {
        /* 源数据与传输后数据相等时，LED1 亮（蓝色），表示传输成功 */
        LED2_ON;
    }


    while(1)
    {

    }


#if BSP_TZ_SECURE_BUILD
    /* Enter non-secure code */
    R_BSP_NonSecureEnter();
#endif
}


/**
  * 判断指定长度的两个数据源是否完全相等，
  * 如果完全相等返回1，只要其中一对数据不相等返回0
  */
uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
    /* 数据长度递减 */
    while(BufferLength--)
    {
        /* 判断两个数据源是否对应相等 */
        if(*pBuffer != *pBuffer1)
        {
            /* 对应数据源不相等马上退出函数，并返回0 */
            return 0;
        }
        /* 递增两个数据源的地址指针 */
        pBuffer++;
        pBuffer1++;
    }
    /* 完成判断并且对应数据相对 */
    return 1;
}


/* 按键引脚外部中断的回调函数 */
void key_sw3_irq10_callback(external_irq_callback_args_t *p_args)
{
    (void)(p_args);  //FSP_PARAMETER_NOT_USED

    /* DTC传输被激活，同时设置标志位 */
    dtc_start_transmission_sign = true;
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
