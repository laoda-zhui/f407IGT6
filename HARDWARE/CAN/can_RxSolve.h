#ifndef CAN_CAN_RXSOLVE_H_
#define CAN_CAN_RXSOLVE_H_


#include "main.h"
#include "can.h"
#include <stdint.h>
#include <string.h>
#include "stdlib.h"

/*对应缓冲数组大小*/
#define	Can_RxFIFOZize		500	/*接收缓冲区最大空间 wifi zigbee display*/
#define Can_RxZigbeeZize  	20
#define Can_RxInfoZize 		20
#define Can_RxTrackZize 	20
#define Can_RxNavigZize 	20
#define Can_RxHOSTZize  	20
#define Can_RxAnythingZize 	20



/*对应缓冲数组*/
extern uint8_t FifoBuf_Info[Can_RxInfoZize]; 	/*0接收显示屏*/
extern uint8_t FifoBuf_WifiRx[Can_RxFIFOZize]; 	/*1接收wifi*/
extern uint8_t FifoBuf_ZigbRx[Can_RxZigbeeZize]; 	/*2接收zigbee*/
extern uint8_t FifoBuf_Track[Can_RxTrackZize];	/*3接收循迹*/
extern uint8_t FifoBuf_Navig[Can_RxNavigZize]; 	/*4接收navig-暂时不知道是啥*/
extern uint8_t FifoBuf_HOST[Can_RxHOSTZize];	/*5接收主机*/
extern uint8_t FifoBuf_Anything[Can_RxAnythingZize];	/*6接收任何信息*/


extern uint8_t AndroidGoFlag;


/*定义接收CAN通信缓冲区数组*/
typedef struct{
	uint8_t Flag;	/*标志位*/
	uint8_t  *Data;	/*发送数据缓冲区*/
	uint16_t rp; 	/*读索引*/
	uint16_t wp; 	/*写索引*/
}Can_RXFIFOBUF;



/*缓冲区操作*/
void Can_RxBufWrite(Can_RXFIFOBUF *CanBuf, uint8_t *Data, uint8_t len);
uint8_t Can_RxReadBit(Can_RXFIFOBUF *CanBuf, uint8_t *data);
uint8_t RingBuf_PeekByte(Can_RXFIFOBUF *buf, uint8_t *val);
uint8_t Can_RxCheckReadEn(Can_RXFIFOBUF *p);

/*初始化函数*/
void CanRxBuf_Init(void);


/*刷新-接收到指定数组函数,并进行数据处理*/
void CanRx_Loop(void);
void Slove_ALL(void);

extern int16_t CanHost_Mp; /*码盘值A*/
extern int16_t CanHost_Mp1;	/*码盘值B*/
extern int16_t CanHost_Mp2;	/*码盘值C*/
extern int16_t CanHost_Mp3;	/*码盘值D*/

extern uint16_t Current_Angle;	/*猜测为当前航向角度*/

extern float pitch;	/*pitch是围绕X轴旋转，也叫做俯仰角*/
extern float yaw;	/*yaw是围绕Y轴旋转，也叫偏航角*/
extern float roll;	/*roll是围绕Z轴旋转，也叫翻滚角*/

extern uint8_t x1,x2; /*循迹拆分数据x1:循迹板后面8个灯状况 x2：循迹板后面7个灯状况(循迹板b档情况下x1有效)*/

/*二维码解析数据*/
typedef struct{
	char RQ1Buf[100];
	char RQ2Buf[100];
	char RQ3Buf[100];
} RQStruct;
extern RQStruct RQData;

extern char QR1Num[50]; //二维码1







/*安卓摄像头状态*/
typedef enum{
	TrafficNull= 0,
	TrafficRed_Flag,
	TrafficYellow_Flag,
	TrafficGreen_Flag,

}CameraFlag;

extern CameraFlag AndroidFlag;

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
	GateOpen 	 /*ETC道闸开启*/

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


/*公交站回传数据*/
typedef struct{
	uint8_t ruijiao;	/*锐角*/
	uint8_t dunjiao;	/*钝角*/
	uint8_t zhijiao;	/*直角*/
	uint8_t lingxing;	/*菱形*/
	uint8_t changfan;	/*长方形*/
	uint8_t juxing;		/*矩形*/
	uint8_t star;		/*五角星形*/
	uint8_t circle;		/*圆形*/

	uint8_t red;		/*红色*/
	uint8_t green;		/*绿色*/
	uint8_t blue;		/*蓝色*/
	uint8_t yellow;		/*黄色*/
	uint8_t qingse;		/*青色*/
	uint8_t orange;		/*橙色*/
	uint8_t purple;		/*紫色*/
	uint8_t black;		/*黑色*/

}ColorShape;

extern ColorShape CameraData;

/*从车回传数据*/
typedef struct{
	uint8_t chepai[7];	/*车牌*/

}SlaveCar;

extern SlaveCar SlaveCarData;



#endif /* CAN_CAN_RXSOLVE_H_ */
