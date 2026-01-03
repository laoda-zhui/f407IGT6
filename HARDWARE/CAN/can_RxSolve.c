#include "can_RxSolve.h"



/**************************************************接收数组初始化***************************************************************/

/*对应缓冲数组 -- 大小参考can_RxSolve.h宏定义*/
uint8_t FifoBuf_Info[FIFOSIZE_INFO] 		= {0}; 	/*0接收显示屏*/
uint8_t FifoBuf_WifiRx[FIFOSIZE_WIFIRX] 	= {0}; 	/*1接收wifi*/
uint8_t FifoBuf_ZigbRx[FIFOSIZE_ZIGBRX] 	= {0}; 	/*2接收zigbee*/
uint8_t FifoBuf_Track[FIFOSIZE_ZIGBTX] 	  	= {0};	/*3接收循迹*/
uint8_t FifoBuf_Navig[FIFOSIZE_ZIGBTX] 	 	= {0}; 	/*4接收navig-暂时不知道是啥*/
uint8_t FifoBuf_HOST[FIFOSIZE_ZIGBTX] 	  	= {0};	/*5接收主机*/
uint8_t FifoBuf_Anything[FIFOSIZE_ZIGBTX] 	= {0};	/*6接收任何信息*/


uint8_t x1,x2; /*循迹拆分数据x1:循迹板后面8个灯状况 x2：循迹板后面7个灯状况(循迹板b档情况下x1有效)*/


uint16_t Current_Angle;	/*猜测为当前航向角度*/


/*接受的数据*/
int16_t CanHost_Mp=0;	/*码盘值A*/
int16_t CanHost_Mp1=0;	/*码盘值B*/
int16_t CanHost_Mp2=0;	/*码盘值C*/
int16_t CanHost_Mp3=0;	/*码盘值D*/

float pitch=0;	/*pitch是围绕X轴旋转，也叫做俯仰角*/
float yaw=0;	/*yaw是围绕Y轴旋转，也叫偏航角*/
float roll=0;	/*roll是围绕Z轴旋转，也叫翻滚角*/


/*******************************************************以上为初始化*****************************************************************/



/**************************************************接收FIFO功能函数***************************************************************/


/**************************************************************************
函数功能：CAN接收-识别指定设备数据，将数据存储到指定数组中,再进行数据处理
入口参数：无
返回  值：无
**************************************************************************/
void CanRx_Loop(void)
{
	if(MyCAN_RxFlag == 1) /*中断接收到CAN数据标志位MyCAN_RxFlag*/
	{
		MyCAN_RxFlag = 0; /*清除中断接收到CAN数据标志位MyCAN_RxFlag*/

		switch(RxMsgArray.FilterMatchIndex)		//判断消息邮箱索引
		{
			case 0:			/*disp*/
				memcpy(FifoBuf_Info, RxData, RxMsgArray.DLC);
				break;

			case 1:			/*wifi rx*/
				memcpy(FifoBuf_WifiRx, RxData, RxMsgArray.DLC);
				break;

			case 2:			/*zigbee rx*/
				memcpy(FifoBuf_ZigbRx, RxData, RxMsgArray.DLC);
				break;

			case 3:			/*Track*/
				memcpy(FifoBuf_Track, RxData, RxMsgArray.DLC);

				x1 = FifoBuf_Track[0];
				x2 = FifoBuf_Track[1];

				break;

			case 4:			/*Navig*/
				memcpy(FifoBuf_Navig, RxData, RxMsgArray.DLC);

				Current_Angle = (FifoBuf_Navig[0]<<8)|FifoBuf_Navig[1];	/*高位在前，低位在后*/
				while(Current_Angle >= 3600){Current_Angle -= 3600;}

				break;

			case 5:			/*HOST*/
				memcpy(FifoBuf_HOST, RxData, RxMsgArray.DLC);
				if(FifoBuf_HOST[0] == 0x02)
				{
					CanHost_Mp  =  (FifoBuf_HOST[2]<<8) | FifoBuf_HOST[1];/*码盘值A*/
					CanHost_Mp1 = (FifoBuf_HOST[4]<<8) | FifoBuf_HOST[3]; /*码盘值B*/

				}
				if(FifoBuf_HOST[0] == 0x04)
				{
					CanHost_Mp2 = (FifoBuf_HOST[2]<<8) | FifoBuf_HOST[1]; /*码盘值C*/
					CanHost_Mp3 = (FifoBuf_HOST[4]<<8) | FifoBuf_HOST[3]; /*码盘值D*/
				}

				/*陀螺仪数据*/
				if(FifoBuf_HOST[0] == 0x21)
				{
					/*pitch是围绕X轴旋转，也叫做俯仰角*/
					pitch = (float)((FifoBuf_HOST[1] << 24) | (FifoBuf_HOST[2] << 16) | (FifoBuf_HOST[3] << 8) | FifoBuf_HOST[4]);
				}
				if(FifoBuf_HOST[0] == 0x22)
				{
					/*yaw是围绕Y轴旋转，也叫偏航角*/
					yaw = (float)((FifoBuf_HOST[1] << 24) | (FifoBuf_HOST[2] << 16) | (FifoBuf_HOST[3] << 8) | FifoBuf_HOST[4]);
				}
				if(FifoBuf_HOST[0] == 0x23)
				{
					/*roll是围绕Z轴旋转，也叫翻滚角*/
					roll = (float)((FifoBuf_HOST[1] << 24) | (FifoBuf_HOST[2] << 16) | (FifoBuf_HOST[3] << 8) | FifoBuf_HOST[4]);
				}

				break;

			case 6:			/*Anything*/
				memcpy(FifoBuf_Anything, RxData, RxMsgArray.DLC);
				break;
		}

	}
}


















