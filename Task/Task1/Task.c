#include "Task.h"


uint16_t TaskFlag=0; /*任务状态*/
uint8_t GoSpeed=100;  /*行进基础速度*/
uint8_t TrackSpeed = 100; /*循迹基础速度*/

uint32_t Stop_Delay_Tick=0;
uint16_t LoopCount =0;	/*循环次数*/

void Task1_Start(void)
{
	switch(TaskFlag)
	{
		case 0:	/*循迹开始*/
			if(LoopCount==0){Command_StartTim();Command_StartTim();}
			Car_Track(TrackSpeed);
			TaskFlag = 1;
			break;
		case 1: /*循迹结束*/
			if(Stop_Flag == Task_Complete)
			{
				TaskFlag = 2;
				if(LoopCount == 1){Command_CloseGate();Command_CloseGate();} /*第二次右转关闸门*/
			}
			break;
		case 2: /*前进开始*/
			Car_Go(GoSpeed, 6);
			TaskFlag = 3;
			break;
		case 3: /*前进结束*/
			if(Stop_Flag == Task_Complete)
			{
				TaskFlag = 4;
				if(LoopCount == 0){Command_OpenGate();Command_OpenGate();} /*第一次右转开闸门*/
			}
			break;
		case 4: /*右转开始*/
			Car_Right(78, -78);
			TaskFlag = 5;
			break;
		case 5: /*右转结束*/
			if(Stop_Flag == Task_Complete)
			{
				if(LoopCount == 0){Command_OpenGate();Command_OpenGate();} /*第一次右转开闸门*/
				TaskFlag = 0;
				LoopCount++;
				if(LoopCount == 4)	/*第四次 回到原地*/
				{
					TaskFlag=6;
					LoopCount=0;
					Car_TrackTime(47, 700);
					Command_EndTim();
				}
			}
			break;
		case 6:
			if(Stop_Flag == Task_Complete)
			{
				TaskBeep_Set(GPIO_PIN_SET);
				Car_Back(44, 57);
				TaskFlag = 7;
			}
			break;
		case 7:
			if(Stop_Flag == Task_Complete)
			{
				TaskBeep_Set(GPIO_PIN_RESET);
				Command_SlaveCarStart();
				Command_SlaveCarStart();
				Command_SlaveCarStart();
				Start_Flag = 0;
			}
			break;
	}
}











