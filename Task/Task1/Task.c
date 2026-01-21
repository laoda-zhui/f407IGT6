#include "Task.h"


uint16_t TaskFlag=0; /*任务状态*/
uint8_t GoSpeed=100;  /*行进基础速度*/
uint8_t TrackSpeed = 100; /*循迹基础速度*/

uint32_t TestTimeOut=3000,TestTimeStart=0;




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
			Car_Go(TrackSpeed, 7);

			TaskFlag = 2;
		}
		break;

	case 2:
		if(Stop_Flag == Task_Complete)
		{
			Car_Left(78);	/*左转*/
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
			TestTimeOut = 700;
		}

		break;

	case 4:
		if((HAL_GetTick() - TestTimeStart) > TestTimeOut)/*等900ms开始安卓识别*/
		{
			Command_AndroidTraffic();
			TestTimeStart = HAL_GetTick();
			TaskFlag = 5;
			TestTimeOut = 1500;
		}
		break;

	case 5:
		Command_TrafficASend();
		Command_AndroidTraffic();

		if((HAL_GetTick() - TestTimeStart) > TestTimeOut)
		{
			TaskFlag = 6;
		}

		break;
	case 6:

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








