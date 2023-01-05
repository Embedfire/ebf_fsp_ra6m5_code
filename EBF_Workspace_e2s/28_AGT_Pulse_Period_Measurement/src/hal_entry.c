#include "hal_data.h"

FSP_CPP_HEADER
void R_BSP_WarmStart(bsp_warm_start_event_t event);
FSP_CPP_FOOTER


/* 用户头文件包含 */
#include "led/bsp_led.h"
#include "debug_uart/bsp_debug_uart.h"
#include "agt/bsp_agt_pwm_output.h"
#include "agt/bsp_agt_pulse_period_measurement.h"


// 外部变量声明
extern timer_info_t info;  //用于获取定时器参数信息
extern uint32_t pulse_period_time;  //PWM周期的时间计数
extern volatile uint8_t print_enble_flag;


/*******************************************************************************************************************//**
 * main() is generated by the RA Configuration editor and is used to generate threads if an RTOS is used.  This function
 * is called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
    /* TODO: add your own code here */

    LED_Init();         // LED 初始化
    Debug_UART4_Init(); // SCI4 UART 调试串口初始化

    AGT_PWM_Init();                     // AGT PWM输出初始化
    AGT_Pulse_Period_Measurement_Init(); // AGT 脉冲周期测量初始化

    printf("这是一个 AGT 的PWM输出 + 脉冲周期测量功能实验\r\n");
    printf("使用杜邦线连接 P500 和 P600 引脚，然后打开串口助手查看串口的打印信息\r\n");


    while(1)
    {
        float pulse_period_time_us;

        if (print_enble_flag)
        {
            // 打印PWM脉冲周期测量的计数
            printf("High=%d, ", pulse_period_time);
            /* 计算PWM脉冲周期测量的时间 */
            pulse_period_time_us = (float)pulse_period_time / ((float)info.clock_frequency / (float)1000000);
            printf("Time=%f us\r\n", pulse_period_time_us);

            pulse_period_time = 0;  //测量结果打印完后旧数据清零
            print_enble_flag = 0;   //允许打印测量结果标志位清零
        }

        // LED1 闪烁指示程序正在运行...
        LED1_TOGGLE;
        // 间隔1s
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_SECONDS);
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
