#include "Task.h"


uint16_t TaskFlag=0; /*任务状态*/
uint8_t GoSpeed=120;  /*行进基础速度*/
uint8_t TurnSpeed = 80;
uint8_t TrackSpeed = 100; /*循迹基础速度*/

uint32_t WaitTimeStart=0,WaitTimeOut=0;

uint8_t LightInit=0; /*路灯初始档位*/
uint8_t VoiceNumber=0;


void Task1_Start(void)
{


	switch(TaskFlag)
	{
	case 0:
		Command_StartTim();	/*开始计时*/
		Car_Track(TrackSpeed); /*开始循迹*/
		TaskFlag = 1;
		break;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			Command_GetPortBFloor(); /*偷偷获取车库层数*/
			Command_GetPortBFloor(); /*偷偷获取车库层数*/
			Car_Go(GoSpeed, 5);
			TaskFlag = 2;
		}
		break;

	case 2:
		if(Stop_Flag == Task_Complete)
		{
			Command_GetPortBFloor(); /*偷偷获取车库层数*/
			Command_GetPortBFloor(); /*偷偷获取车库层数*/
			Car_Left(TurnSpeed);	/*左转*/
			TaskFlag = 100;
		}
		break;

	case 100:
		if(Stop_Flag == Task_Complete)
		{
			Car_TrackBackMp(100, 8);
			Command_GetPortBFloor(); /*偷偷获取车库层数*/
			TaskFlag = 3;
		}
		break;

	case 3:
		if(Stop_Flag == Task_Complete)
		{

			Command_TrafficAInMode();	/*开启A交通灯识别*/
			Command_TrafficAInMode();
			Command_TrafficAInMode();
			WaitTimeStart = HAL_GetTick();
			WaitTimeOut = 1500;

			TaskFlag = 4;
		}
		break;

	case 4:
		if((HAL_GetTick() - WaitTimeStart) > WaitTimeOut)/*等1.5s开始安卓识别*/
		{
			Command_AndroidTraffic();
			WaitTimeStart = HAL_GetTick();
			WaitTimeOut = 3000;
			TaskFlag = 5;
		}
		break;

	case 5:
		Command_AndroidTraffic();

		if(AndroidFlag)
		{
			Command_TrafficASend();
			TaskFlag = 6;
		}

		if((HAL_GetTick() - WaitTimeStart) > WaitTimeOut) /*等3s超时跳过*/
		{
			Command_TrafficASend();
			Command_TrafficASend();
			TaskFlag = 6;
		}

		break;
	case 6:
		Command_GetPortBFloor(); /*偷偷获取车库层数*/
		Car_Track(TrackSpeed); /*开始循迹*/
		TaskFlag = 7;
		break;
	case 7:
		if(Stop_Flag == Task_Complete)
		{
			Command_GetPortBFloor(); /*偷偷获取车库层数*/
			Car_Go(TrackSpeed, 5);
			TaskFlag = 8;
		}
		break;
	case 8:
		if(Stop_Flag == Task_Complete)
		{
			Car_Right(TurnSpeed);	/*右转*/
			TaskFlag = 150;
		}
		break;
	case 150:
		if(Stop_Flag == Task_Complete)
		{
			Command_GetPortBFloor();
			Command_GetPortBFloor();
			TaskFlag = 9;
		}
		break;

	case 9:
		if(Stop_Flag == Task_Complete)
		{
			Car_Track(TrackSpeed); /*开始循迹*/
			TaskFlag = 10;
		}
		break;
	case 10:
		if(Stop_Flag == Task_Complete)
		{
			Car_Go(TrackSpeed, 5); /*B4*/
			TaskFlag = 11;
		}
		break;
	case 11:
		if(Stop_Flag == Task_Complete)
		{
			Car_Left(76);	/*左转-路灯*/
			TaskFlag = 12;
		}
		break;
	case 12:
		if(Stop_Flag == Task_Complete)	/*调节路灯*/
		{
			LightInit = Command_LightAuto(CarPortFlag);
			TaskFlag = 13;
		}
		break;
	case 13:
		Command_SlaveCarLight(LightInit);
		Car_Right(76);	/*右转*/
		TaskFlag = 14;

		break;
	case 14:
		if(Stop_Flag == Task_Complete)/*面向etc闸门*/
		{
			Command_SlaveCarLight(LightInit);
			Car_Right(TurnSpeed);	/*右转*/
			TaskFlag = 200;
		}
		break;
	case 200:
		if(Stop_Flag == Task_Complete)/*面向etc闸门*/
		{
			Car_TrackMp(12,10);	/*前进*/
			TaskFlag = 201;
			if(GateFlag == GateOpen)
			{
				Command_CarPortB(1);
				Command_CarPortB(1);
				TaskFlag = 15;
			}
			Slove_ALL();
		}
		break;
	case 201:
		if(Stop_Flag == Task_Complete)/*面向etc闸门*/
		{
			Car_Back(1,10);	/*后退*/
			TaskFlag = 200;
			if(GateFlag == GateOpen)
			{
				Command_CarPortB(1);
				Command_CarPortB(1);
				TaskFlag = 15;
			}
			Slove_ALL();
		}
		break;
	case 15:
		if(Stop_Flag == Task_Complete)
		{
			WaitTimeStart = HAL_GetTick();
			WaitTimeOut = 100;
			TaskFlag = 16;
			Command_SlaveCarLight(LightInit);
		}
		break;
	case 16:
		if((HAL_GetTick() - WaitTimeStart) > WaitTimeOut) /*等1.5s通过ETC*/
		{
			Car_Track(TrackSpeed); /*循迹到特殊地形前面*/
			Command_SlaveCarLight(LightInit);
			TaskFlag = 17;
		}
		break;
	case 17:
		if(Stop_Flag == Task_Complete)/*特殊地形*/
		{
			Car_Go(GoSpeed, 55);
			Command_SlaveCarLight(LightInit);
			TaskFlag = 18;
		}
		break;
	case 18:
		if(Stop_Flag == Task_Complete)/*特殊地形*/
		{
			Car_Track(TrackSpeed);
			Command_CarPortB(1);
			TaskFlag = 19;
		}
		break;
	case 19:
		if(Stop_Flag == Task_Complete)
		{
			Car_Go(GoSpeed, 3);
			Command_CarPortB(1);
			TaskFlag = 20;
		}
		break;
	case 20:
		if(Stop_Flag == Task_Complete)/*面向TFTB屏幕*/
		{
			Car_TrackMp(TrackSpeed, 8);
			TaskFlag = 21;
		}
		break;

	case 21:
		WaitTimeStart = HAL_GetTick();
		WaitTimeOut = 2000;
		TaskFlag = 22;

		break;
	case 22:
		if((HAL_GetTick() - WaitTimeStart) > WaitTimeOut)/*等2s开始识别图形*/
		{
			Command_Androidshape();
			Command_Androidshape();
			WaitTimeStart = HAL_GetTick();
			WaitTimeOut = 2000;
			TaskFlag = 23;
		}
		break;
	case 23:
		if((HAL_GetTick() - WaitTimeStart) > WaitTimeOut)/*等2s开始识别颜色*/
		{
			Command_AndroidColor();
			Command_AndroidColor();
			WaitTimeStart = HAL_GetTick();
			WaitTimeOut = 2000;
			TaskFlag = 24;
		}
		break;
	case 24:
		if((HAL_GetTick() - WaitTimeStart) > WaitTimeOut)/*等2s开始发送图形信息*/
		{
			Command_TFTBShowHex(0xA0|(CameraData.changfan+CameraData.juxing), 0xD0|(CameraData.lingxing), 0xE0|(CameraData.star));
			Command_TFTBShowHex(0xA0|(CameraData.changfan+CameraData.juxing), 0xD0|(CameraData.lingxing), 0xE0|(CameraData.star));
			WaitTimeStart = HAL_GetTick();
			WaitTimeOut = 2000;
			TaskFlag = 25;
		}
		break;
	case 25:
		if((HAL_GetTick() - WaitTimeStart) > WaitTimeOut)/*等2s开始发送颜色信息*/
		{
			Command_LEDShowDown(0xF0|(CameraData.red), 0xF0|(CameraData.green), 0xF0|(CameraData.blue));
			Command_LEDShowDown(0xF0|(CameraData.red), 0xF0|(CameraData.green), 0xF0|(CameraData.blue));
			Car_Left(TurnSpeed);
			TaskFlag = 26;
		}
		break;
	case 26:
		if(Stop_Flag == Task_Complete)
		{
			Car_Track(70);
			Command_BusCheckTem(); /*交通识别回传温度*/
			TaskFlag = 27;
		}
		break;

	case 27:
		if(Stop_Flag == Task_Complete)
		{
			Car_Go(50, 5);
			Command_BusCheckTem(); /*交通识别回传温度*/
			TaskFlag = 28;
		}
		break;
	case 28:
		if(Stop_Flag == Task_Complete)/*面向TFTA屏幕*/
		{
			Car_Right(TurnSpeed);
			Command_BusCheckTem(); /*交通识别回传温度*/
			WaitTimeStart = HAL_GetTick();
			WaitTimeOut = 3000;
			TaskFlag = 29;


		}
		break;
	case 29:
		Command_BusCheckTem(); /*交通识别回传温度*/
		HAL_Delay(10);
		if((HAL_GetTick() - WaitTimeStart) > WaitTimeOut)/*等3s开始识别行人，没有直接跳过*/
		{
			Command_SlaveCarSTep();
			TaskFlag = 30;

		}

		break;
	case 30:
		if(Stop_Flag == Task_Complete)
		{
			Command_SlaveCarSTep();
			Car_Left(TurnSpeed);
			TaskFlag = 31;
		}
		break;
	case 31:
		if(Stop_Flag == Task_Complete) /*语音*/
		{
			Command_BusReportRandom(); /*开启交站*/
			Command_BusReportRandom(); /*开启交站*/
			WaitTimeStart = HAL_GetTick();
			WaitTimeOut = 50;
			TaskFlag = 32;
		}
		break;
	case 32:
		if((HAL_GetTick() - WaitTimeStart) > WaitTimeOut)
		{
			VoiceNumber = Voice_ASR(); /*开启语音识别*/
			TaskFlag = 33;
		}
		break;
	case 33:
		Command_SlaveCarSTep();
		Command_SlaveCarSTep();
		TaskFlag = 34;

		break;
	case 34:
		Car_Left(TurnSpeed);	/*语音左转*/
		Command_Autosystem(VoiceNumber);
		Command_Autosystem(VoiceNumber);
		TaskFlag = 35;
		break;
	case 35:
		if(Stop_Flag == Task_Complete)
		{
			Command_SlaveCarStart();	/*发送从车启动*/
			Car_Track(TrackSpeed);
			TaskFlag = 36;
		}
		break;
	case 36:
		if(Stop_Flag == Task_Complete)
		{
			Command_SlaveCarStart();	/*发送从车启动*/
			Car_Go(GoSpeed, 5);
			TaskFlag = 37;
		}
		break;
	case 37:
		Command_SlaveCarStart();
		if(Stop_Flag == Task_Complete)
		{
			Command_SlaveCarSTep();
			Car_Track(TrackSpeed);
			TaskFlag = 38;
		}
		break;
	case 38:
		if(Stop_Flag == Task_Complete) /*B2*/
		{
			Car_Go(GoSpeed, 5);
			Command_SlaveCarSTep();
			TaskFlag = 39;
		}
		break;
	case 39:
		Car_Left(TurnSpeed);
		Command_OpenGate();
		Command_SetGateLast(65, 65, 65); /*车牌前3位*/
		TaskFlag = 40;
		break;
	case 40:
		if(Stop_Flag == Task_Complete)  /*闸门*/
		{

			Command_OpenGate();
//			Command_SetGateTop(65, 65, 65);/*车牌后3位*/
			WaitTimeStart = HAL_GetTick();
			WaitTimeOut = 500;
			TaskFlag = 41;
		}
		break;
	case 41:
		if((HAL_GetTick() - WaitTimeStart) > WaitTimeOut) /*闸门*/
		{
			Command_OpenGate();
//			Command_SetGateTop(65, 65, 65);/*车牌后3位*/
			Car_Track(TrackSpeed);
			TaskFlag = 42;
		}
		break;
	case 42:
		if(Stop_Flag == Task_Complete)
		{
			Car_Go(GoSpeed, 5);
			TaskFlag = 43;
		}
		break;
	case 43:
		if(Stop_Flag == Task_Complete)
		{
			Car_Track(TrackSpeed);
			TaskFlag = 44;
		}
		break;
	case 44:
		if(Stop_Flag == Task_Complete)
		{
			Car_Go(GoSpeed, 5);
			TaskFlag = 45;
		}
		break;
	case 45:
		if(Stop_Flag == Task_Complete)
		{
			Car_Left(TurnSpeed);
			TaskFlag = 46;
		}
		break;
	case 46:
		if(Stop_Flag == Task_Complete)
		{
			Car_TrackMp(60, 13);
			TaskFlag = 47;
		}
		break;
	case 47:
		if(Stop_Flag == Task_Complete)
		{
			Car_Back(GoSpeed, 40);
			TaskFlag = 0;
			Start_Flag = 0;
		}
		break;












	}
}

//void Task_testSpecialLoad(void)
//{
//	switch(TaskFlag)
//	{
//	case 0:
//		Car_Track(TrackSpeed);
//		TaskFlag=1;
//		break;
//	case 1:
//		if(Stop_Flag == Task_Complete)	/*循迹到白卡*/
//		{
//			Car_Go(GoSpeed, 10);
//			if(RC522(18, RFID_Read) == 1)
//			{
//				TaskFlag=2;
//			}
//
//		}
//		break;
//	case 2:
//		if(Stop_Flag == Task_Complete)	/*读卡*/
//		{
//			Car_TrackMp(TrackSpeed, 25);
//			TaskFlag=3;
//		}
//		break;
//	case 3:
//		if(Stop_Flag == Task_Complete)	/*过特殊地形前*/
//		{
//			Car_Go(GoSpeed, 45);
//			TaskFlag=4;
//		}
//		break;
//	case 4:
//		if(Stop_Flag == Task_Complete)	/*过特殊地形中*/
//		{
//			Car_Track(TrackSpeed);
//			TaskFlag=5;
//		}
//		break;
//	case 5:
//		if(Stop_Flag == Task_Complete)	/*过特殊地形出来到十字路口1*/
//		{
//			Car_Go(GoSpeed, 10);
//			TaskFlag=6;
//		}
//		break;
//	case 6:
//		if(Stop_Flag == Task_Complete)	/*过特殊地形出来到十字路口2*/
//		{
//			if(RC522(18, RFID_Read) == 1)
//			{
//				TaskFlag=7;
//			}
//		}
//		break;
//	case 7:
//		if(Stop_Flag == Task_Complete)	/*到十字路口读卡*/
//		{
//			TaskFlag=8;
//		}
//		break;
//
//	}
//}












