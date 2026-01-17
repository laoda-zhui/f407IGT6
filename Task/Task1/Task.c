#include "Task.h"


uint16_t TaskFlag=0;
uint8_t GoSpeed=90;/*基础速度*/
uint8_t TrackSpeed = 90;

uint32_t Stop_Delay_Tick=0;
void Task1_Start(void)
{
	switch(TaskFlag)
	{
		case 0:
			Car_Track(TrackSpeed);
			TaskFlag = 1;
			break;
		case 1:
			if(Stop_Flag == Task_Complete)
			{
				TaskFlag = 2;
			}
			break;
		case 2:
			Car_Go(GoSpeed, 6);
			TaskFlag = 3;
			break;
		case 3:
			if(Stop_Flag == Task_Complete)
			{
				TaskFlag = 4;
			}
			break;
		case 4:
			Car_Right(77, -77);
			TaskFlag = 5;
			break;
		case 5:
			if(Stop_Flag == Task_Complete)
			{
				TaskFlag = 0;
			}
			break;
	}


}











