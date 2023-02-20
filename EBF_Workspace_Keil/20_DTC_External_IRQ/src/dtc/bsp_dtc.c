#include "bsp_dtc.h"

/**
 * 传输 API 的 DTC 实现
 *
 * const transfer_api_t g_transfer_on_dtc =
 * {
 *     .open          = R_DTC_Open,
 *     .reconfigure   = R_DTC_Reconfigure,
 *     .reset         = R_DTC_Reset,
 *     .infoGet       = R_DTC_InfoGet,
 *     .softwareStart = R_DTC_SoftwareStart,    //空函数（DTC不支持软件激活）
 *     .softwareStop  = R_DTC_SoftwareStop,
 *     .enable        = R_DTC_Enable,
 *     .disable       = R_DTC_Disable,
 *     .close         = R_DTC_Close,
 * };
 */


#pragma pack(4)

/**
 * 定义 SRC_Buffer 数组作为 DTC 传输数据源
 * const 关键字将 SRC_Buffer 数组变量定义为常量类型，数据存储在内部的FLASH中
 */
const uint32_t SRC_Buffer[BUFFER_SIZE] = {
                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80 };
/**
 * 定义 DTC 传输目标存储器
 * 存储在内部的SRAM中
 */
uint32_t DST_Buffer[BUFFER_SIZE] = {0};

#pragma pack()


#ifndef USE_MY_TRANSFER_INFOR_CONFIG

/* FSP配置界面的传输信息配置（正常传输模式）等效于下面 fsp_transfer_info_normal 里的配置（除了源地址和目标地址）
   源地址和目标地址在FSP配置界面设置的话不太方便，我们会在 DTC_Init 函数里设置。
   关于偏移值（Offset value）和源缓冲大小（Source Buffer Size），在 transfer_info_t 里没有这两项设置，建议在FSP配置界面设置
   - 偏移值只有在地址模式是偏移模式的情况下才会用到；
   - 而源缓冲大小与之相关的功能本例程不会涉及到，所以暂不考虑。
   下面的 fsp_transfer_info_normal 仅作为对比参考，在本例程中是没有用到的。
 */
//transfer_info_t fsp_transfer_info_normal =
//{
//    .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,        //每次传输后，目标地址指针固定不变
//    .transfer_settings_word_b.repeat_area    = TRANSFER_REPEAT_AREA_SOURCE,     //源区域重复 (正常模式下无效)
//    .transfer_settings_word_b.irq            = TRANSFER_IRQ_END,                //传输完成后中断
//    .transfer_settings_word_b.chain_mode     = TRANSFER_CHAIN_MODE_DISABLED,    //不使能（DMAC没有该功能，仅DTC有）
//    .transfer_settings_word_b.src_addr_mode  = TRANSFER_ADDR_MODE_FIXED,        //每次传输后，源地址指针固定不变
//    .transfer_settings_word_b.size           = TRANSFER_SIZE_2_BYTE,            //每次传输2字节
//    .transfer_settings_word_b.mode           = TRANSFER_MODE_NORMAL,            //正常传输模式
//    .p_dest                                  = (void *) DST_Buffer,             //目标地址
//    .p_src                                   = (void const *) SRC_Buffer,       //源地址
//    .num_blocks = 0,            //指定传输的块数（正常模式和重复模式下均无效，仅块模式下有效）
//    .length     = 1,            //指定传输的长度（即正常的传输次数或重复模式下重复大小 或 块模式下传输的块大小）
//};
// 按照上述传输信息配置，期待的正确传输结果为：
uint32_t Expected_DST_Buffer[BUFFER_SIZE] = {
                    0x00000304,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000 };


#else   /* 下面的这些是我们自定义的使用各种模式的传输配置信息：*/

#if (DTC_TRANSFER_MODE == DTC_TRANSFER_NORMAL_MODE)           //正常模式（相当于重复次数为1的重复模式）

/* 正常传输模式 */
transfer_info_t my_transfer_info_normal =
{
    .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,  //每次传输后，目标地址指针都会增加
    .transfer_settings_word_b.repeat_area    = TRANSFER_REPEAT_AREA_SOURCE,     //源区域重复 (正常模式下无效)
    .transfer_settings_word_b.irq            = TRANSFER_IRQ_END,                //传输完成后中断
    .transfer_settings_word_b.chain_mode     = TRANSFER_CHAIN_MODE_DISABLED,    //不使能（DMAC没有该功能，仅DTC有）
    .transfer_settings_word_b.src_addr_mode  = TRANSFER_ADDR_MODE_INCREMENTED,  //每次传输后，源地址指针都会增加
    .transfer_settings_word_b.size           = TRANSFER_SIZE_4_BYTE,            //每次传输4字节
    .transfer_settings_word_b.mode           = TRANSFER_MODE_NORMAL,            //正常传输模式
    .p_dest                                  = (void *) DST_Buffer,             //目标地址
    .p_src                                   = (void const *) SRC_Buffer,       //源地址
    .num_blocks = 0,            //指定传输的块数（正常模式和重复模式下均无效，仅块模式下有效）
    .length     = 1,            //指定传输的长度（即正常的传输次数或重复模式下重复大小 或 块模式下传输的块大小）
};
// 按照上述传输信息配置，期待的正确传输结果为：
uint32_t Expected_DST_Buffer[BUFFER_SIZE] = {
                    0x01020304,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000 };

#elif (DTC_TRANSFER_MODE == DTC_TRANSFER_REPEAT_MODE)         //重复模式

/* 重复传输模式 */
transfer_info_t my_transfer_info_repeat =
{
    .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,  //每次传输后，目标地址指针都会增加
    .transfer_settings_word_b.repeat_area    = TRANSFER_REPEAT_AREA_SOURCE,     //源区域重复 (正常模式下无效)
    .transfer_settings_word_b.irq            = TRANSFER_IRQ_EACH,                //传输完成后中断
    .transfer_settings_word_b.chain_mode     = TRANSFER_CHAIN_MODE_DISABLED,    //不使能（DMAC没有该功能，仅DTC有）
    .transfer_settings_word_b.src_addr_mode  = TRANSFER_ADDR_MODE_INCREMENTED,  //每次传输后，源地址指针都会增加
    .transfer_settings_word_b.size           = TRANSFER_SIZE_4_BYTE,            //每次传输4字节
    .transfer_settings_word_b.mode           = TRANSFER_MODE_REPEAT,            //重复传输模式
    .p_dest                                  = (void *) DST_Buffer,             //目标地址
    .p_src                                   = (void const *) SRC_Buffer,       //源地址
    .num_blocks = 0,            //指定传输的块数（正常模式和重复模式下均无效，仅块模式下有效）
    .length     = 2,            //指定传输的长度（即正常的传输次数或重复模式下重复大小 或 块模式下传输的块大小）
};
// 按照上述传输信息配置，期待的正确传输结果为：
uint32_t Expected_DST_Buffer[BUFFER_SIZE] = {
                    0x01020304,0x05060708,0x01020304,0x05060708,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000,
                    0x00000000,0x00000000,0x00000000,0x00000000 };

#elif (DTC_TRANSFER_MODE == DTC_TRANSFER_BLOCK_MODE)          //块模式

/* 块传输模式 */
transfer_info_t my_transfer_info_block =
{
    .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,  //每次传输后，目标地址指针都会增加
    .transfer_settings_word_b.repeat_area    = TRANSFER_REPEAT_AREA_SOURCE,     //源区域重复 (正常模式下无效)
    .transfer_settings_word_b.irq            = TRANSFER_IRQ_END,                //传输完成后中断
    .transfer_settings_word_b.chain_mode     = TRANSFER_CHAIN_MODE_DISABLED,    //不使能（DMAC没有该功能，仅DTC有）
    .transfer_settings_word_b.src_addr_mode  = TRANSFER_ADDR_MODE_INCREMENTED,  //每次传输后，源地址指针都会增加
    .transfer_settings_word_b.size           = TRANSFER_SIZE_4_BYTE,            //每次传输4字节
    .transfer_settings_word_b.mode           = TRANSFER_MODE_BLOCK,             //块传输模式
    .p_dest                                  = (void *) DST_Buffer,             //目标地址
    .p_src                                   = (void const *) SRC_Buffer,       //源地址
    .num_blocks = 1,            //指定传输的块数（正常模式和重复模式下均无效，仅块模式下有效）
    .length     = BUFFER_SIZE,  //指定传输的长度（即正常的传输次数或重复模式下重复大小 或 块模式下传输的块大小）
};
// 按照上述传输信息配置，期待的正确传输结果为：
uint32_t Expected_DST_Buffer[BUFFER_SIZE] = {
                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80 };

/* 块传输模式 2 */
//transfer_info_t my_transfer_info_block =
//{
//    .transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,  //每次传输后，目标地址指针都会增加
//    .transfer_settings_word_b.repeat_area    = TRANSFER_REPEAT_AREA_SOURCE,     //源区域重复 (正常模式下无效)
//    .transfer_settings_word_b.irq            = TRANSFER_IRQ_END,                //传输完成后中断
//    .transfer_settings_word_b.chain_mode     = TRANSFER_CHAIN_MODE_DISABLED,    //不使能（DMAC没有该功能，仅DTC有）
//    .transfer_settings_word_b.src_addr_mode  = TRANSFER_ADDR_MODE_INCREMENTED,  //每次传输后，源地址指针都会增加
//    .transfer_settings_word_b.size           = TRANSFER_SIZE_4_BYTE,            //每次传输4字节
//    .transfer_settings_word_b.mode           = TRANSFER_MODE_BLOCK,             //块传输模式
//    .p_dest                                  = (void *) DST_Buffer,             //目标地址
//    .p_src                                   = (void const *) SRC_Buffer,       //源地址
//    .num_blocks = 2,              //指定传输的块数（正常模式和重复模式下均无效，仅块模式下有效）
//    .length     = BUFFER_SIZE/2,  //指定传输的长度（即正常的传输次数或重复模式下重复大小 或 块模式下传输的块大小）
//};
//// 按照上述传输信息配置，期待的正确传输结果为：
//uint32_t Expected_DST_Buffer[BUFFER_SIZE] = {
//                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
//                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
//                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
//                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
//                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
//                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
//                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
//                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40 };

#endif //DTC_TRANSFER_MODE

#endif //USE_MY_TRANSFER_INFOR_CONFIG




/* 设置传输的源地址和目的地址 */
void set_transfer_dst_src_address( transfer_cfg_t const * const p_config,
                                   void const * volatile p_src,
                                   void const * volatile p_dest )
{
    p_config->p_info->p_src = (void *) p_src;
    p_config->p_info->p_dest = (void *) p_dest;
}


/* DTC 初始化函数 */
void DTC_Init(void)
{
    fsp_err_t err;

    /* 使用 FSP 界面的配置：需要先重新设置传输的源地址和目的地址 */
#ifndef USE_MY_TRANSFER_INFOR_CONFIG
    set_transfer_dst_src_address(&g_transfer_dtc_cfg, SRC_Buffer, DST_Buffer);
#endif

    err = R_DTC_Open(&g_transfer_dtc_ctrl, &g_transfer_dtc_cfg);
    assert(FSP_SUCCESS == err);


    /* 使用我们新的自定义的传输信息：重新配置传输 */
#ifdef USE_MY_TRANSFER_INFOR_CONFIG

    // 根据我们要使用的传输模式进行选择配置：
#if (DTC_TRANSFER_MODE == DTC_TRANSFER_NORMAL_MODE)           //正常模式
    err = R_DTC_Reconfigure(&g_transfer_dtc_ctrl, &my_transfer_info_normal);
    assert(FSP_SUCCESS == err);

#elif (DTC_TRANSFER_MODE == DTC_TRANSFER_REPEAT_MODE)         //重复模式
    err = R_DTC_Reconfigure(&g_transfer_dtc_ctrl, &my_transfer_info_repeat);
    assert(FSP_SUCCESS == err);

#elif (DTC_TRANSFER_MODE == DTC_TRANSFER_BLOCK_MODE)          //块模式
    err = R_DTC_Reconfigure(&g_transfer_dtc_ctrl, &my_transfer_info_block);
    assert(FSP_SUCCESS == err);
#endif //DTC_TRANSFER_MODE

#endif //USE_MY_TRANSFER_INFOR_CONFIG
}

