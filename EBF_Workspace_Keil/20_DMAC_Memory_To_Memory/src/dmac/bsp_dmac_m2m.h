#ifndef __BSP_DMAC_M2M_H
#define __BSP_DMAC_M2M_H
#include "hal_data.h"


/* 宏 USE_MY_TRANSFER_INFOR_CONFIG
   定义该宏选择使用我们自定义的传输信息，需调用 R_DMAC_Reconfigure 函数进行重新配置
   若使用在 FSP 配置界面配置的传输信息，则注释掉该宏
*/
#define USE_MY_TRANSFER_INFOR_CONFIG

#ifdef USE_MY_TRANSFER_INFOR_CONFIG
  // 设置使用的模式：
  #define DMAC_TRANSFER_NORMAL_MODE         (1)   //正常模式
  #define DMAC_TRANSFER_REPEAT_MODE         (2)   //重复模式
  #define DMAC_TRANSFER_BLOCK_MODE          (3)   //块模式
  #define DMAC_TRANSFER_REPEAT_BLOCK_MODE   (4)   //重复-块模式
  #define DMAC_TRANSFER_MODE                DMAC_TRANSFER_REPEAT_BLOCK_MODE
#endif


// 用户要发送的数据大小
#define BUFFER_SIZE         32


void set_transfer_dst_src_address( transfer_cfg_t const * const p_config,
                                   void const * volatile p_src,
                                   void const * volatile p_dest );
void DMAC_Init(void);

#endif /* __BSP_DMAC_M2M_H */
