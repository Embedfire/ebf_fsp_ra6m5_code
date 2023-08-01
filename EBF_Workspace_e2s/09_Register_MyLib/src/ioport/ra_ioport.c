/* 自己写库——构建库函数雏形 */

#include "ra_ioport.h"


/* 读引脚电平 */

uint32_t IOPORT_PinRead(IO_Port_t port, IO_Pin_t pin)
{
    /* Read pin level. */
    return R_PFS->PORT[port >> 8].PIN[pin].PmnPFS_b.PIDR;
}


/* 写引脚电平 */

void IOPORT_PinWrite(IO_Port_t port, IO_Pin_t pin, IO_Level_t level)
{
    uint32_t pfs_bits = R_PFS->PORT[port >> 8].PIN[pin].PmnPFS; //读寄存器 PmnPFS
    
    pfs_bits &= ~(uint32_t)0x1; //清零 PODR 位

    R_PFS->PORT[port >> 8].PIN[pin].PmnPFS = (pfs_bits | level);
}


/* 翻转引脚电平 */

void IOPORT_PinToggle(IO_Port_t port, IO_Pin_t pin)
{
    uint32_t pfs_bits = R_PFS->PORT[port >> 8].PIN[pin].PmnPFS; //读寄存器 PmnPFS
    
    pfs_bits ^= (uint32_t)0x1; //取反 PODR 位

    R_PFS->PORT[port >> 8].PIN[pin].PmnPFS = pfs_bits;
}


/* 引脚访问使能 */

void IOPORT_PinAccessEnable(void)
{
    R_PMISC->PWPR = 0;        ///< Clear BOWI bit - writing to PFSWE bit enabled
    R_PMISC->PWPR = 1U << 6U; ///< Set PFSWE bit - writing to PFS register enabled
}


/* 引脚访问禁止 */

void IOPORT_PinAccessDisable(void)
{
    R_PMISC->PWPR = 0;        ///< Clear PFSWE bit - writing to PFS register disabled
    R_PMISC->PWPR = 1U << 7U; ///< Set BOWI bit - writing to PFSWE bit disabled
}


/* IOPORT 初始化函数 */

void IOPORT_Init(IOPORT_Init_t *ioport_init)
{
    uint32_t pfs_bits = 0; //不读取寄存器 PmnPFS
    
    if(ioport_init->Mode == IO_MODE_GPIO) //如果引脚用作普通GPIO功能
    {        
        if(ioport_init->Dir == IO_DIR_INPUT)        /* 用作输入模式 */
        {
            pfs_bits |= (ioport_init->Pull) << 4;   //设置输入上拉
        }
        else if(ioport_init->Dir == IO_DIR_OUTPUT)  /* 用作输出模式 */
        {
            pfs_bits |= (ioport_init->Dir) << 2;    //设置为输出
            pfs_bits |= (ioport_init->Level) << 0;  //设置输出电平
            pfs_bits |= (ioport_init->OType) << 6;  //设置输出模式
            pfs_bits |= (ioport_init->Drive) << 10; //设置输出驱动能力
        }
    }
    else
    {
        //我们不考虑引脚用作复用模式和模拟输入输出模式的情况
        //也不考虑中断的情况，仅考虑普通的GPIO输入输出功能
    }
    
    /* 写入配置到寄存器 PmnPFS */
    R_PFS->PORT[ioport_init->Port >> 8].PIN[ioport_init->Pin].PmnPFS = pfs_bits;
}



//End of file
