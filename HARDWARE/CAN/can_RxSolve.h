#ifndef CAN_CAN_RXSOLVE_H_
#define CAN_CAN_RXSOLVE_H_


#include "main.h"
#include "can.h"
#include <stdint.h>
#include <string.h>

#define FIFOSIZE_INFO		20	/*对应缓冲数组大小*/
#define FIFOSIZE_WIFIRX		20
#define FIFOSIZE_WIFITX		20
#define FIFOSIZE_ZIGBRX		20
#define FIFOSIZE_ZIGBTX		20

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
extern int16_t CanHost_Mp1;	/*码盘值B*/
extern int16_t CanHost_Mp2;	/*码盘值C*/
extern int16_t CanHost_Mp3;	/*码盘值D*/

extern uint16_t Current_Angle;	/*猜测为当前航向角度*/

extern float pitch;	/*pitch是围绕X轴旋转，也叫做俯仰角*/
extern float yaw;	/*yaw是围绕Y轴旋转，也叫偏航角*/
extern float roll;	/*roll是围绕Z轴旋转，也叫翻滚角*/


extern uint8_t x1,x2; /*循迹拆分数据x1:循迹板后面8个灯状况 x2：循迹板后面7个灯状况(循迹板b档情况下x1有效)*/


#endif /* CAN_CAN_RXSOLVE_H_ */
