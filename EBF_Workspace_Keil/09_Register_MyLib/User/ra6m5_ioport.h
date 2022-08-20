/* 自己写库——构建库函数雏形 */
#ifndef __RA6M5_IOPORT_H
#define __RA6M5_IOPORT_H

#include "hal_data.h"


/* IOPORT 端口定义 */
typedef enum io_port
{
    IO_PORT_00 = 0x0000,           ///< IO port 0
    IO_PORT_01 = 0x0100,           ///< IO port 1
    IO_PORT_02 = 0x0200,           ///< IO port 2
    IO_PORT_03 = 0x0300,           ///< IO port 3
    IO_PORT_04 = 0x0400,           ///< IO port 4
    IO_PORT_05 = 0x0500,           ///< IO port 5
    IO_PORT_06 = 0x0600,           ///< IO port 6
    IO_PORT_07 = 0x0700,           ///< IO port 7
    IO_PORT_08 = 0x0800,           ///< IO port 8
    IO_PORT_09 = 0x0900,           ///< IO port 9
    IO_PORT_10 = 0x0A00,           ///< IO port 10
    IO_PORT_11 = 0x0B00,           ///< IO port 11
    IO_PORT_12 = 0x0C00,           ///< IO port 12
    IO_PORT_13 = 0x0D00,           ///< IO port 13
    IO_PORT_14 = 0x0E00,           ///< IO port 14
} IO_Port_t;


/* IOPORT 引脚定义 */
typedef enum io_pin
{
    IO_PIN_00 = 0x0000,    ///< IO port 0 pin 0
    IO_PIN_01 = 0x0001,    ///< IO port 0 pin 1
    IO_PIN_02 = 0x0002,    ///< IO port 0 pin 2
    IO_PIN_03 = 0x0003,    ///< IO port 0 pin 3
    IO_PIN_04 = 0x0004,    ///< IO port 0 pin 4
    IO_PIN_05 = 0x0005,    ///< IO port 0 pin 5
    IO_PIN_06 = 0x0006,    ///< IO port 0 pin 6
    IO_PIN_07 = 0x0007,    ///< IO port 0 pin 7
    IO_PIN_08 = 0x0008,    ///< IO port 0 pin 8
    IO_PIN_09 = 0x0009,    ///< IO port 0 pin 9
    IO_PIN_10 = 0x000A,    ///< IO port 0 pin 10
    IO_PIN_11 = 0x000B,    ///< IO port 0 pin 11
    IO_PIN_12 = 0x000C,    ///< IO port 0 pin 12
    IO_PIN_13 = 0x000D,    ///< IO port 0 pin 13
    IO_PIN_14 = 0x000E,    ///< IO port 0 pin 14
    IO_PIN_15 = 0x000F,    ///< IO port 0 pin 15
} IO_Pin_t;



/* IO 引脚模式 */
typedef enum
{
    IO_MODE_GPIO = 0,   /* 普通 GPIO 功能 */
    IO_MODE_AF   = 1,   /* 复用功能 */
    IO_MODE_AN   = 2    /* 模拟输入输出功能 */
} IO_Mode_t; 


/* IO 引脚方向 */
typedef enum
{
    IO_DIR_INPUT  = 0,  // 引脚用作GPIO输入功能
    IO_DIR_OUTPUT = 1   // 引脚用作GPIO输出功能
} IO_Dir_t;


/* IO 引脚输出类型 */
typedef enum
{
    IO_OTYPE_PP = 0x00, /* 推挽模式 */
    IO_OTYPE_OD = 0x01  /* 开漏模式 */
} IO_OType_t;


/* IO 引脚输出能力 */
typedef enum
{
    IO_DRIVE_LOW    = 0,    // Low drive
    IO_DRIVE_MIDDLE = 1,    // Middle drive
    IO_DRIVE_HSHD   = 2,    // High-speed high-drive
    IO_DRIVE_HIGH   = 3     // High drive
} IO_DriveCapability_t;


/* IO 引脚输出电平 */
typedef enum io_level
{
    IO_LEVEL_LOW = 0,       // Low
    IO_LEVEL_HIGH           // High
} IO_Level_t;


/* IO 引脚输入上下拉 */
typedef enum
{
    IO_NO_PULL =   0x00u,   /* 浮空输入 */
    IO_PULL_UP =   0x01u,   /* 上拉输入 */
    //IO_PULL_DOWN = 0x02u  /* RA6M5 引脚没有下拉功能 */
} IO_Pull_t;




/* IOPORT 初始化结构体定义 */

typedef struct
{
    IO_Port_t               Port;
    IO_Pin_t                Pin;
    IO_Mode_t               Mode;
    IO_Dir_t                Dir;
    IO_OType_t              OType;
    IO_DriveCapability_t    Drive;
    IO_Pull_t               Pull;
    IO_Level_t              Level;
} IOPORT_Init_t;




/* IO 操作函数（调用一次只能操作一个IO引脚） */
uint32_t IOPORT_PinRead         (IO_Port_t port, IO_Pin_t pin);
void     IOPORT_PinWrite        (IO_Port_t port, IO_Pin_t pin, IO_Level_t level);
void     IOPORT_PinToggle       (IO_Port_t port, IO_Pin_t pin);
void     IOPORT_PinAccessEnable (void);
void     IOPORT_PinAccessDisable (void);

/* IO 初始化函数（调用一次只能初始化一个IO引脚） */
void     IOPORT_Init (IOPORT_Init_t *ioport_init);


#endif //__RA6M5_IOPORT_H
