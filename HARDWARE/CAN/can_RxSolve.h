#ifndef CAN_CAN_RXSOLVE_H_
#define CAN_CAN_RXSOLVE_H_


#include "main.h"
#include "can.h"
#include <stdint.h>
#include <string.h>

#define FIFOSIZE_INFO		10	/*对应缓冲数组大小*/
#define FIFOSIZE_WIFIRX		10
#define FIFOSIZE_WIFITX		10
#define FIFOSIZE_ZIGBRX		10
#define FIFOSIZE_ZIGBTX		10

extern uint8_t FifoBuf_Info[FIFOSIZE_INFO];	/*对应缓冲数组*/
extern uint8_t FifoBuf_WifiRx[FIFOSIZE_WIFIRX];
extern uint8_t FifoBuf_WifiTx[FIFOSIZE_WIFITX];
extern uint8_t FifoBuf_ZigbRx[FIFOSIZE_ZIGBRX];
extern uint8_t FifoBuf_ZigbTx[FIFOSIZE_ZIGBTX];

extern uint8_t FifoBuf_Track[FIFOSIZE_ZIGBTX];		/*接收循迹*/
extern uint8_t FifoBuf_Navig[FIFOSIZE_ZIGBTX]; 	/*接收navig-暂时不知道是啥*/
extern uint8_t FifoBuf_HOST[FIFOSIZE_ZIGBTX];		/*接收主机*/
extern uint8_t FifoBuf_Anything[FIFOSIZE_ZIGBTX];	/*接收任何信息*/

/*刷新-接收到指定数组函数,并进行数据处理*/
void CanRx_Loop(void);

extern int16_t CanHost_Mp; /*码盘值A*/
extern uint16_t Current_Angle;	/*猜测为当前航向角度*/

#endif /* CAN_CAN_RXSOLVE_H_ */
