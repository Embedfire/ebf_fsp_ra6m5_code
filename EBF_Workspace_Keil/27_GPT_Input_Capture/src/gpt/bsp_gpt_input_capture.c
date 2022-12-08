#include "bsp_gpt_input_capture.h"
#include "stdio.h"


/*  当前选用输入捕获引脚 GTIOC7A P603 */


timer_info_t info;  //用于获取定时器参数信息
uint32_t period;    //输入捕获计数器的计数周期

/* 保存所测量的PWM信号的信息 */
uint32_t pwm_period;            //PWM周期
uint32_t pwm_high_level_time;   //PWM高电平的时间
uint32_t pwm_freq;              //PWM频率
uint32_t pwm_duty;              //PWM占空比


/* GPT初始化函数 */
void GPT_InputCapture_Init(void)
{
    /* 初始化 GPT 模块 */
    R_GPT_Open(&g_timer_gpt7_ctrl, &g_timer_gpt7_cfg);
    
    /* 获取当前参数 */
    (void) R_GPT_InfoGet(&g_timer_gpt7_ctrl, &info);
    /* 获取计数周期：GPT计数器一个周期的计数次数 */
    period = info.period_counts;
    
    /* 使能输入捕获 */
	R_GPT_Enable(&g_timer_gpt7_ctrl);
    
    /* 启动 GPT 定时器 */
    R_GPT_Start(&g_timer_gpt7_ctrl);
}


/* GPT 输入捕获中断回调函数 */
void gpt7_input_capture_callback(timer_callback_args_t * p_args)
{
    static uint32_t a_time; // A 上升沿捕获的时间
    static uint32_t b_time; // B 下降沿捕获的时间
    static uint32_t c_time; // C 上升沿捕获的时间（其实也就是 A 可以用A'表示）
    static uint32_t overflow_times;     //计数器溢出次数
    static uint8_t  one_period_flag=0;  //用于表示是否完成对一个完整周期的测量


    switch(p_args->event)
    {
        /* 捕获到上升沿 -- 有可能是 A 或者 C (A') 位置 */
        case TIMER_EVENT_CAPTURE_A:
            /* A 开始对某个周期进行测量 */
            if (0 == one_period_flag)
            {
                a_time = p_args->capture;   //记录捕获的时间 A
                overflow_times = 0;         //初始化计数器溢出次数
                one_period_flag ++;         //表示即将完成对某个周期的测量
            }
            /* C (A') 如果测量完了一个周期，则计算PWM信号周期和高电平的时间 */
            else if (1 == one_period_flag)
            {
                c_time = p_args->capture + overflow_times * period; //记录捕获的时间 C
                
                //计算 PWM 周期
                pwm_period = c_time - a_time;
                
                //清零所有标志位
                overflow_times = 0;
                one_period_flag = 0;         //标志位清0,重新进入下一轮的测量
            }
            break;
        
        /* 捕获到下降沿 -- 是 B 位置 */
        case TIMER_EVENT_CAPTURE_B:
            //如果是在测量周期内检测到下降沿
            if (1 == one_period_flag)
            {
                b_time = p_args->capture + overflow_times * period; //记录捕获的时间 B
                
                pwm_high_level_time = b_time - a_time;  //计算高电平时间
            }
            break;
        
        /* 定时器计数溢出事件 */
        case TIMER_EVENT_CYCLE_END:
            /* 输入捕获期间计数器溢出，则记录溢出次数+1 */
            overflow_times++;
            break;
        
        default:
            break;
    }
}
