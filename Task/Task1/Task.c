#include "Task.h"


uint16_t TaskFlag=0; /*任务状态*/
uint8_t GoSpeed=100;  /*行进基础速度*/
uint8_t TurnSpeed = 80;
uint8_t TrackSpeed = 120; /*循迹基础速度*/




void Task_CarGO(uint8_t Speed, uint16_t Temp);
void Task_CarBack(uint8_t Speed, uint16_t Temp);

void Task_CarLeft(uint8_t Speed);
void Task_CarRight(uint8_t Speed);
void Task_CarMPLeft(uint8_t Speed, double Angle);
void Task_CarMPRight(uint8_t Speed, double Angle);

void Task_CarTrack(uint8_t Speed);
void Task_CarTrackTime(uint8_t Speed, uint32_t Time);
void Task_CarMpTrack(uint8_t Speed, uint16_t Temp);

void Task_CarWait(uint32_t Time);



void Task1_Start(void)
{

	switch(TaskFlag)
	{
	case 0:


		Command_GetPortBFloor(); /*获取车库层数*/
//		Command_StartTim();	/*开始计时*/
		TaskFlag = 1;
		break;
	case 1:
		Task_CarTrack(TrackSpeed); /*开始循迹*/
		break;
	case 2:
		Task_CarGO(TrackSpeed, 7);
		break;
	case 3:
		Task_CarLeft(TurnSpeed);	/*左转*/
		break;
	case 4:
		Command_TrafficAInMode();	/*开启A交通灯识别*/
		Command_TrafficAInMode();	/*开启A交通灯识别*/
		TaskFlag = 5;
		break;
	case 5:
		Task_CarWait(1500);
		break;
	case 6:
		Command_AndroidTraffic();
		CanRx_Loop();
		TaskFlag =7;
		break;
	case 7:
		Task_CarWait(2500);
		Command_TrafficASend();
		Command_AndroidTraffic();

		break;
	case 8:
		Task_CarTrack(TrackSpeed); /*开始循迹*/
		Command_GetPortBFloor(); /*获取车库层数*/
		break;
	case 9:
		Task_CarGO(TrackSpeed, 7);
		Command_GetPortBFloor(); /*获取车库层数*/
		break;
	case 10:
		Task_CarRight(TurnSpeed);	/*右转*/
		Command_GetPortBFloor(); /*获取车库层数*/
		break;
	case 11:
		Command_GetPortBFloor(); /*获取车库层数*/
		Command_GetPortBFloor(); /*获取车库层数*/
		TaskFlag =12;
		break;
	case 12:
		Task_CarTrack(TrackSpeed); /*开始循迹*/
		break;
	case 13:
		Task_CarGO(TrackSpeed, 7); /*B4*/
		break;
	case 14:
		Task_CarLeft(TurnSpeed);	/*左转-路灯*/
		break;
	case 15:
		Command_LightAuto(CarPortFlag);
		TaskFlag = 16;
		break;
	case 16:
		Task_CarRight(TurnSpeed);	/*右转*/
		break;
	case 17:
		Task_CarRight(TurnSpeed);	/*右转*/
		break;
	case 18:
		Task_CarWait(1500);
		break;
	case 19:
		Task_CarTrack(TrackSpeed); /*循迹到特殊地形前面*/
		break;
	case 20:
		Task_CarGO(GoSpeed, 55); /*过特殊地形*/
		break;
	case 21:
		Task_CarTrack(TrackSpeed);
		break;

	case 22:
		Task_CarGO(GoSpeed, 2);
		break;
	case 23:
		Task_CarMpTrack(60, 8);	/*到多功能标志物前*/
		break;
	case 24:
		Task_CarWait(800);
		break;
	case 25:
		Command_Androidshape();
		Command_Androidshape();
		TaskFlag = 26;
		break;
	case 26:
		Command_AndroidColor();
		Command_AndroidColor();
		TaskFlag = 27;
		break;
	case 27:
		Task_CarWait(1500);
		break;
	case 28:
		Command_TFTBShowHex(0xA0|(CameraData.changfan+CameraData.juxing), 0xD0|(CameraData.lingxing), 0xE0|(CameraData.star));
		Task_CarWait(700);
		break;
	case 29:
		Command_LEDShowDown(0xF0|(CameraData.red), 0xF0|(CameraData.green), 0xF0|(CameraData.blue));
		Task_CarWait(700);
		break;
	case 30:
		Task_CarLeft(TurnSpeed);
		break;
	}
}








/**************************************************************************
函数功能：任务-车等待时间进入下一个状态(非阻塞)
入口参数：Time:等待时间
返回  值：无
**************************************************************************/
void Task_CarWait(uint32_t Time)
{
	static uint8_t TaskWaitflag=0;
	static uint32_t TimeOut,TimeStart;
	switch(TaskWaitflag)
	{
	case 0:
		TimeStart = HAL_GetTick();
		TimeOut = Time;
		TaskWaitflag = 1;
		break;
	case 1:
		if((HAL_GetTick() - TimeStart) > TimeOut)
		{
			TaskWaitflag = 0;
			TaskFlag++;
		}
		break;
	}
}





/**************************************************************************
函数功能：任务-车前进 任务完成后自动TaskFlag++
入口参数：speed:速度(max=120) temp:前进距离cm
返回  值：无
**************************************************************************/
void Task_CarGO(uint8_t Speed, uint16_t Temp)
{
	static uint8_t TaskGoflag=0;
	switch(TaskGoflag)
	{
	case 0:
		Car_Go(Speed, Temp);
		TaskGoflag = 1;
		break;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			TaskGoflag = 0;
			TaskFlag++;
		}
		break;
	}
}



/**************************************************************************
函数功能：任务-车后退 任务完成后自动TaskFlag++
入口参数：speed:速度(max=120) temp:前进距离cm
返回  值：无
**************************************************************************/
void Task_CarBack(uint8_t Speed, uint16_t Temp)
{
	static uint8_t TaskBackflag=0;
	switch(TaskBackflag)
	{
	case 0:
		Car_Back(Speed, Temp);
		TaskBackflag = 1;
		break;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			TaskBackflag = 0;
			TaskFlag++;
		}
		break;
	}
}




/**************************************************************************
函数功能：任务-车左转 任务完成后自动TaskFlag++
入口参数：speed:速度(max=120) 推荐80
返回  值：无
**************************************************************************/
void Task_CarLeft(uint8_t Speed)
{
	static uint8_t TaskLeftflag=0;
	switch(TaskLeftflag)
	{
	case 0:
		Car_Left(Speed);
		TaskLeftflag = 1;
		break;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			TaskLeftflag = 0;
			TaskFlag++;
		}
		break;
	}
}

/**************************************************************************
函数功能：任务-车右转 任务完成后自动TaskFlag++
入口参数：speed:速度(max=120) 推荐80
返回  值：无
**************************************************************************/
void Task_CarRight(uint8_t Speed)
{
	static uint8_t TaskRightflag=0;
	switch(TaskRightflag)
	{
	case 0:
		Car_Right(Speed);
		TaskRightflag = 1;
		break;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			TaskRightflag = 0;
			TaskFlag++;
		}
		break;
	}
}



/**************************************************************************
函数功能：任务-车循迹 任务完成后自动TaskFlag++
入口参数：speed:速度(max=120)
返回  值：无
**************************************************************************/
void Task_CarTrack(uint8_t Speed)
{
	static uint8_t TaskTrackflag=0;
	switch(TaskTrackflag)
	{
	case 0:
		Car_Track(Speed);
		TaskTrackflag = 1;
		break;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			TaskTrackflag = 0;
			TaskFlag++;
		}
		break;
	}
}



/**************************************************************************
函数功能：任务-车规定时间内循迹 任务完成后自动TaskFlag++
入口参数：speed:速度(max=120) Time:循迹时间
返回  值：无
**************************************************************************/
void Task_CarTrackTime(uint8_t Speed, uint32_t Time)
{
	static uint8_t TaskTrackflag=0;
	switch(TaskTrackflag)
	{
	case 0:
		Car_TrackTime(Speed, Time);
		TaskTrackflag = 1;
		break;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			TaskTrackflag = 0;
			TaskFlag++;
		}
		break;
	}
}


/**************************************************************************
函数功能：任务-车码盘循迹 任务完成后自动TaskFlag++
入口参数：speed:速度(max=120) Temp:距离cm
返回  值：无
**************************************************************************/
void Task_CarMpTrack(uint8_t Speed, uint16_t Temp)
{
	static uint8_t TaskTrackMpflag=0;
	switch(TaskTrackMpflag)
	{
	case 0:
		Car_TrackMp(Speed, Temp);
		TaskTrackMpflag = 1;
		break;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			TaskTrackMpflag = 0;
			TaskFlag++;
		}
		break;
	}
}









/**************************************************************************
函数功能：任务-车码盘左转 任务完成后自动TaskFlag++
入口参数：speed:速度(max=120) Angle:角度(有误差)
返回  值：无
**************************************************************************/
void Task_CarMPLeft(uint8_t Speed, double Angle)
{
	static uint8_t TaskMPLflag=0;
	switch(TaskMPLflag)
	{
	case 0:
		Car_MPLeft(Speed, Angle);
		TaskMPLflag = 1;
		break;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			TaskMPLflag = 0;
			TaskFlag++;
		}
		break;
	}
}


/**************************************************************************
函数功能：任务-车码盘右转 任务完成后自动TaskFlag++
入口参数：speed:速度(max=120)
返回  值：无
**************************************************************************/
void Task_CarMPRight(uint8_t Speed, double Angle)
{
	static uint8_t TaskMPRflag=0;
	switch(TaskMPRflag)
	{
	case 0:
		Car_MPRight(Speed, Angle);
		TaskMPRflag = 1;
		break;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			TaskMPRflag = 0;
			TaskFlag++;
		}
		break;
	}
}














