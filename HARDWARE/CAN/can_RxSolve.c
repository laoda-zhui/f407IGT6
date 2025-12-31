#include "can_RxSolve.h"



/**************************************************接收FIFO初始化***************************************************************/

/*对应缓冲数组 -- 大小参考can_RxSolve.h宏定义*/
uint8_t FifoBuf_Info[FIFOSIZE_INFO] 	= {0}; /*0*/
uint8_t FifoBuf_WifiRx[FIFOSIZE_WIFIRX] = {0}; /*1*/
uint8_t FifoBuf_WifiTx[FIFOSIZE_WIFITX] = {0};
uint8_t FifoBuf_ZigbRx[FIFOSIZE_ZIGBRX] = {0}; /*2*/
uint8_t FifoBuf_ZigbTx[FIFOSIZE_ZIGBTX] = {0};

uint8_t FifoBuf_Track[FIFOSIZE_ZIGBTX] 	  = {0};		/*3接收循迹*/
uint8_t FifoBuf_Navig[FIFOSIZE_ZIGBTX] 	  = {0}; 	/*4接收navig-暂时不知道是啥*/
uint8_t FifoBuf_HOST[FIFOSIZE_ZIGBTX] 	  = {0};		/*5接收主机*/
uint8_t FifoBuf_Anything[FIFOSIZE_ZIGBTX] = {0};	/*6接收任何信息*/



/*******************************************************以上为初始化*****************************************************************/



/**************************************************接收FIFO功能函数***************************************************************/


/**************************************************************************
函数功能：CAN接收-识别指定设备数据，将数据存储到指定结构体中
入口参数：无
返回  值：无
**************************************************************************/
void CanP_CanRx_Irq(void)
{
	if(MyCAN_RxFlag == 1) /*收到CAN数据*/
	{
		switch(RxMsgArray.FilterMatchIndex)										 //判断消息邮箱索引
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
				break;

			case 4:			/*Navig*/
				memcpy(FifoBuf_Navig, RxData, RxMsgArray.DLC);
				break;

			case 5:			/*HOST*/
				memcpy(FifoBuf_HOST, RxData, RxMsgArray.DLC);
				break;

			case 6:			/*Anything*/
				memcpy(FifoBuf_Anything, RxData, RxMsgArray.DLC);
				break;
		}
		MyCAN_RxFlag = 0;
	}
}


















