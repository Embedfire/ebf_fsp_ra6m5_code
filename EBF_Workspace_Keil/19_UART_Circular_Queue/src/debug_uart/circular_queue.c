#include <debug_uart/circular_queue.h>
#include "stdlib.h"

//定义环形队列结构体变量
Circular_queue_t Circular_queue;

/**
 * @brief 初始化队列
 * @param circular_queue: 要使用的队列
 * @retval 若初始化成功返回true,反之false
 */
bool Queue_Init(Circular_queue_t *circular_queue)
{
    if (!circular_queue)
        return false;
    memset (circular_queue, 0, sizeof(Circular_queue_t));
    return true;
}

/**
 * @brief 判断队列是否为空
 * @param circular_queue: 要使用的队列
 * @retval 若队列为空返回true,反之或指针非法则false
 */
bool Queue_isEmpty(Circular_queue_t *circular_queue)
{
    if (!circular_queue)
        return false;

    if (circular_queue->head == circular_queue->tail) //头指针等于尾指针则为空
        return true;
    else
        return false;
}

/**
 * @brief 判断队列是否已满
 * @param circular_queue: 要使用的队列
 * @return 若队列已满返回true,反之或指针非法则false
 */
bool Queue_isFull(Circular_queue_t *circular_queue)
{
    if (!circular_queue)
        return false;

    if ((circular_queue->tail + 1) % DATA_LEN == circular_queue->head)
        return true;
    else
        return false;
}

/**
 * @brief 返回队列中数据的长度
 * @param circular_queue
 * @return len_haduse
 */
uint16_t Queue_HadUse(Circular_queue_t *circular_queue)
{
    if (!circular_queue)
        return false;
    return (uint16_t)(circular_queue->tail - circular_queue->head + DATA_LEN) % DATA_LEN;
}

/**
 * @brief 返回队列未使用数据长度
 * @param circular_queue
 * @return len_NoUse
 */
uint16_t Queue_NoUse(Circular_queue_t *circular_queue)
{
    if (!circular_queue)
        return false;
    return (uint16_t)(DATA_LEN - (circular_queue->tail -
           circular_queue->head + DATA_LEN) % DATA_LEN -1); //保留一个Byte数据用来区分队列的满和空状态
}

/**
 * @brief 给队列写数据
 * @param circular_queue: 要使用的队列
 * @param string: 要写入的数据
 * @param len: 数据长度
 * @return 写入成功则返回true，反之或指针非法则false
 */
bool Queue_Wirte( Circular_queue_t *circular_queue, uint8_t *string, uint16_t len)
{
    if (!circular_queue)
        return false;

    if(Queue_NoUse(circular_queue)<len)
        return false;
    for(int i=0;i<len;i++)
    {
        circular_queue->data[circular_queue->tail]=string[i];
        circular_queue->tail = (uint16_t)(circular_queue->tail+1)%DATA_LEN; //防止越界
    }
    return true;
}

/**
 * @brief 读队列的数据
 * @param circular_queue
 * @param string
 * @param len
 * @return 读数据成功则返回true，反之或指针非法则false
 */
bool Queue_Read(Circular_queue_t *circular_queue, uint8_t *string, uint16_t len)
{
    if (!circular_queue)
            return false;

    if(Queue_HadUse(circular_queue)<len)
        return false;
    for(int i=0;i<len;i++)
    {
        string[i]=circular_queue->data[circular_queue->head];
        circular_queue->head = (uint16_t)(circular_queue->head+1)%DATA_LEN; //防止越界
    }
    return true;
}

