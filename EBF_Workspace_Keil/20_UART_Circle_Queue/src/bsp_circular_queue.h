/*
 * bsp_circular_queue.h
 *
 *  Created on: 2022年7月18日
 *      Author: admin
 */

#ifndef BSP_CIRCULAR_QUEUE_H_
#define BSP_CIRCULAR_QUEUE_H_

#include "hal_data.h"

#define DATA_LEN 300 //队列缓存大小

typedef struct
{
    uint16_t head;   //头指针
    uint16_t tail;   //尾指针
    uint8_t data[DATA_LEN];  //队列数据
} Circular_queue_t;

bool Queue_Init(volatile Circular_queue_t *circular_queue);  //初始化队列

bool Queue_isEmpty(volatile Circular_queue_t *circular_queue);  //判断队列是否为空

bool Queue_isFull(volatile Circular_queue_t *circular_queue);   //判断队列是否已满

bool Queue_Wirte(volatile Circular_queue_t *circular_queue, uint8_t *string, uint16_t len); //写数据

bool Queue_Read(volatile Circular_queue_t *circular_queue, uint8_t *string, uint16_t len);  //读数据

uint16_t Queue_HadUse(volatile Circular_queue_t *circular_queue);   //返回队列中数据的长度

uint16_t Queue_NoUse(volatile Circular_queue_t *circular_queue);    //返回未使用数据的长度


extern volatile Circular_queue_t Circular_queue; //环形队列全局变量

#endif /* BSP_CIRCULAR_QUEUE_H_ */
