#ifndef CAN_CAN_RXSOLVE_H_
#define CAN_CAN_RXSOLVE_H_


#include "main.h"
#include "can.h"
#include <stdint.h>
#include <string.h>

/*对应缓冲数组大小*/
#define FIFOSIZE_INFO		20
#define FIFOSIZE_WIFIRX		20
#define FIFOSIZE_WIFITX		20
#define FIFOSIZE_ZIGBRX		20
#define FIFOSIZE_ZIGBTX		20

/*对应缓冲数组*/
extern uint8_t FifoBuf_Info[FIFOSIZE_INFO];
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

/*安卓摄像头状态*/
typedef enum{
	TrafficNull= 0,
	TrafficRed_Flag,
	TrafficYellow_Flag,
	TrafficGreen_Flag,

}CreamFlag;

extern CreamFlag AndroidFlag;

/*车库层数状态*/
typedef enum{
	CarPortNull=0,
	CarportA1 = 1,	/*车库A到达第1层*/
	CarportA2,	/*车库A到达第2层*/
	CarportA3,	/*车库A到达第3层*/
	CarportA4,	/*车库A到达第4层*/
	CarportB1 = 1,	/*车库B到达第1层*/
	CarportB2,	/*车库B到达第2层*/
	CarportB3,	/*车库B到达第3层*/
	CarportB4,	/*车库B到达第4层*/

}CarPort;

extern CarPort CarPortFlag;

/*ETC闸门状态*/
typedef enum{
	GatetNull = 0,
	GateOpen 	 /*道闸开启*/

}GateStates;

extern GateStates GateFlag;

/*报警台救援坐标*/
extern uint8_t  RescueLocation;

/*公交站回传数据*/
typedef struct{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t secs;
	uint8_t weather;
	uint8_t temperature;
}SmartBus;

extern SmartBus BusData;






#endif /* CAN_CAN_RXSOLVE_H_ */
