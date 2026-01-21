#include "Task.h"


uint16_t TaskFlag=0; /*任务状态*/
uint8_t GoSpeed=100;  /*行进基础速度*/
uint8_t TurnSpeed = 79;
uint8_t TrackSpeed = 120; /*循迹基础速度*/

uint32_t TestTimeOut=3000,TestTimeStart=0;


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








void Task1_Start(void)
{

	switch(TaskFlag)
	{
	case 0:
		Command_StartTim();	/*开始计时*/
		Command_StartTim();
		Car_Track(TrackSpeed); /*开始循迹*/
		TaskFlag = 1;
		break;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			Command_GetPortBFloor(); /*偷偷获取车库层数*/
			Car_Go(TrackSpeed, 7);
			TaskFlag = 2;
		}
		break;

	case 2:
		if(Stop_Flag == Task_Complete)
		{
			Command_GetPortBFloor(); /*偷偷获取车库层数*/
			Car_Left(TurnSpeed);	/*左转*/
			TaskFlag = 3;
		}
		break;

	case 3:
		if(Stop_Flag == Task_Complete)
		{
			Command_TrafficAInMode();	/*开启A交通灯识别*/
			Command_TrafficAInMode();
			TaskFlag = 4;
			TestTimeStart = HAL_GetTick();
			TestTimeOut = 900;
		}

		break;

	case 4:
		if((HAL_GetTick() - TestTimeStart) > TestTimeOut)/*等900ms开始安卓识别*/
		{
			Command_AndroidTraffic();
			TestTimeStart = HAL_GetTick();
			TaskFlag = 5;
			TestTimeOut = 2000;
		}
		break;

	case 5:
		Command_TrafficASend();
		Command_AndroidTraffic();

		if((HAL_GetTick() - TestTimeStart) > TestTimeOut)
		{
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
			Car_Go(TrackSpeed, 7);
			TaskFlag = 8;
		}
		break;
	case 8:
		if(Stop_Flag == Task_Complete)
		{
			Car_Right(TurnSpeed);	/*右转*/
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
			Car_Go(TrackSpeed, 7); /*B4*/
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
			Command_LightAuto(CarPortFlag);
			TaskFlag = 13;
		}
		break;
	case 13:
		Car_Right(76);	/*右转*/
		TaskFlag = 14;

		break;
	case 14:
		if(Stop_Flag == Task_Complete)/*面向etc闸门*/
		{
			Car_Right(TurnSpeed);	/*右转*/
			TaskFlag = 15;

		}
		break;
	case 15:
		if(Stop_Flag == Task_Complete)
		{
			TestTimeStart = HAL_GetTick();
			TestTimeOut = 1500;
			TaskFlag = 16;

		}

		break;
	case 16:
		if((HAL_GetTick() - TestTimeStart) > TestTimeOut)
		{
			Car_Track(TrackSpeed); /*循迹到特殊地形前面*/
			TaskFlag = 17;
		}
		break;
	case 17:
		if(Stop_Flag == Task_Complete)/*特殊地形*/
		{
			Car_Go(GoSpeed, 55);
			TaskFlag = 18;
		}
		break;
	case 18:
		if(Stop_Flag == Task_Complete)/*特殊地形*/
		{
			Car_Track(TrackSpeed);
			TaskFlag = 19;
		}


		break;

	}
}





/*交通灯测试*/
void TestTask(void)
{
	switch(TaskFlag)
	{
	case 0:
		{
			Command_TrafficBInMode();

			TaskFlag = 1;
			TestTimeStart = HAL_GetTick();
			TestTimeOut = 800;
			break;
}
	case 1:
		{
			if((HAL_GetTick() - TestTimeStart) > TestTimeOut)
			{
				Command_AndroidTraffic();
				TestTimeStart = HAL_GetTick();
				TaskFlag = 2;
				TestTimeOut = 1500;
			}

			break;
		}
	case 2:
		{

			break;
		}
	}
}








