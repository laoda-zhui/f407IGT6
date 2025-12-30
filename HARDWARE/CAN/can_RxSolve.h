#ifndef CAN_CAN_RXSOLVE_H_
#define CAN_CAN_RXSOLVE_H_


#include "main.h"
#include "can.h"
#include <stdint.h>
#include <string.h>


/*定义接收缓冲FIFO结构体*/
typedef struct _Fifo_Drv_Struct
{
	uint32_t ml; /*FIFO大小*/
	uint32_t rp; /*读指针*/
	uint32_t wp; /*写指针*/
	uint8_t *buf;/*缓存数组*/
}Fifo_Drv_Struct;


/*全部FIFO初始化函数*/
void CanP_FifoInit(void);



#endif /* CAN_CAN_RXSOLVE_H_ */
