#include "Task.h"


uint16_t TaskFlag=0;
uint8_t GoSpeed=120;/*基础速度*/
uint8_t TrackSpeed = 120;

uint32_t Stop_Delay_Tick=0;

uint8_t x=0;

void Task1_Start(void)
{
	switch(TaskFlag)
	{
		case 0:	/*循迹开始*/
			if(x==0){Command_StartTim();}
			Car_Track(TrackSpeed);
			TaskFlag = 1;
			break;
		case 1: /*循迹结束*/
			if(Stop_Flag == Task_Complete)
			{
				TaskFlag = 2;
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
				if(x == 0){Command_OpenGate();Command_OpenGate();} /*第一次右转开闸门*/
			}
			break;
		case 4: /*右转开始*/
			Car_Right(77, -77);
			TaskFlag = 5;
			break;
		case 5: /*右转结束*/
			if(Stop_Flag == Task_Complete)
			{
				if(x == 0){Command_OpenGate();Command_OpenGate();} /*第一次右转开闸门*/
				TaskFlag = 0;
				x++;
				if(x == 4)	/*第四次 回到原地*/
				{
					TaskFlag=6;
					x=0;
					Command_EndTim();
					Car_TrackTime(50, 600);
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
				Start_Flag = 0;
			}

	}
}











