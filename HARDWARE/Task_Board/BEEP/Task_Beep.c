#include "Task_Beep.h"


#define 	TASKBEEP_GPIO_Port 			GPIOC
#define 	TASKBEEP_Pin 				GPIO_PIN_13


/**************************************************************************
函数功能：TaskBeep-蜂鸣器周期性响
入口参数：Time-周期
返回  值：无
**************************************************************************/
void TaskBeep_StartTask(uint32_t Time)
{
	static uint32_t CurrentTime=0,LastTime=0;
	CurrentTime = HAL_GetTick();

	if((CurrentTime - LastTime) > Time)
	{
		HAL_GPIO_TogglePin(TASKBEEP_GPIO_Port, TASKBEEP_Pin);
		LastTime = CurrentTime;
	}
}



/**************************************************************************
函数功能：TaskBeep-蜂鸣器设置开启/关闭
入口参数：sw：1-开启 0-关闭
返回  值：无
**************************************************************************/
void TaskBeep_Set(uint8_t sw)
{
	uint8_t output = (sw == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET;

	HAL_GPIO_WritePin(TASKBEEP_GPIO_Port, TASKBEEP_Pin, output);

}















