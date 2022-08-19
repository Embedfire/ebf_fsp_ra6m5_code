/**
  ******************************************************************************
  * @file    bsp_dtc.c
  * @author  fire
  * @version V1.0
  * @date    2022-07-xx
  * @brief
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火_瑞萨启明6M5开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "bsp_dtc.h"

/*
 * 传输 API 的 DTC 实现
 *
 * const transfer_api_t g_transfer_on_dtc =
 * {
 *     .open          = R_DTC_Open,
 *     .reconfigure   = R_DTC_Reconfigure,
 *     .reset         = R_DTC_Reset,
 *     .infoGet       = R_DTC_InfoGet,
 *     .softwareStart = R_DTC_SoftwareStart,
 *     .softwareStop  = R_DTC_SoftwareStop,
 *     .enable        = R_DTC_Enable,
 *     .disable       = R_DTC_Disable,
 *     .close         = R_DTC_Close,
 * };
 */

/* 定义aSRC_Const_Buffer数组作为DMA传输数据源
 * const关键字将aSRC_Const_Buffer数组变量定义为常量类型
 * 表示数据存储在内部的FLASH中
 */
const uint32_t aSRC_Const_Buffer[BUFFER_SIZE]= {
                                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};
/* 定义DMA传输目标存储器
 * 存储在内部的SRAM中
 */
uint32_t aDST_Buffer[BUFFER_SIZE] = {0};

transfer_info_t my_transfer_info = {
  .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,  //每次传输后，目的地址指针都会增加
  .transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_SOURCE,        //源区域重复 (正常模式下无效)
  .transfer_settings_word_b.irq = TRANSFER_IRQ_END,                           //传输完成后中断
  .transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,        //不使能
  .transfer_settings_word_b.src_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,   //每次传输后，源地址指针都会增加
  .transfer_settings_word_b.size = TRANSFER_SIZE_4_BYTE,                      //每次传输4字节
  .transfer_settings_word_b.mode = TRANSFER_MODE_BLOCK,                       //块模式
  .p_dest = (void*) aDST_Buffer,                //目标地址
  .p_src = (void const*) aSRC_Const_Buffer,     //源地址
  .num_blocks = 1,                              //使用时要传输的块数 (仅在 Repeat, Block, Repeat-Block 模式下有效)
  .length = BUFFER_SIZE,                        //每次传输的长度
};


void bsp_dtc_init(void)
{
    fsp_err_t err;

    /* 方法一：定义结构体全部重新配置 */
    err = g_transfer_on_dtc.open(&g_transfer_dtc_ctrl, &g_transfer_dtc_cfg);
    assert(FSP_SUCCESS == err);
    err = g_transfer_on_dtc.reconfigure(&g_transfer_dtc_ctrl, &my_transfer_info);
    assert(FSP_SUCCESS == err);

    /* 方法二：主要内容在 configuration.xml 中配置，目标地址和源地址在这里配置 */
//    set_transfer_dst_src_address(&g_transfer_dtc_cfg, my_transfer_info.p_src, my_transfer_info.p_dest);
//    err = g_transfer_on_dtc.open(&g_transfer_dtc_ctrl, &g_transfer_dtc_cfg);
//    assert(FSP_SUCCESS == err);
}


void set_transfer_dst_src_address(transfer_cfg_t const * const p_config,
        void const * volatile p_src, void const * volatile p_dest )
{
    p_config->p_info->p_src = (void *) p_src;
    p_config->p_info->p_dest = (void *) p_dest;
}






