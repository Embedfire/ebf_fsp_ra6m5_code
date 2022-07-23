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




void set_transfer_dst_src_address(transfer_cfg_t const * const p_config,
        void const * volatile p_src, void const * volatile p_dest )
{
    p_config->p_info->p_src = (void *) p_src;
    p_config->p_info->p_dest = (void *) p_dest;
}

