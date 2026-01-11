#include "Beep.h"


#define 	BEEP_GPIO_Port 			GPIOH
#define 	BEEP_Pin 				GPIO_PIN_5


/**************************************************************************
函数功能：Beep-蜂鸣器周期性响
入口参数：Time-周期
返回  值：无
**************************************************************************/
void Beep_StartTask(uint32_t Time)
{
	static uint32_t CurrentTime=0,LastTime=0;
	CurrentTime = HAL_GetTick();

	if((CurrentTime - LastTime) > Time)
	{
		HAL_GPIO_TogglePin(BEEP_GPIO_Port, BEEP_Pin);
		LastTime = CurrentTime;
	}
}



/**************************************************************************
函数功能：Beep-蜂鸣器设置开启/关闭
入口参数：sw：1-开启 0-关闭
返回  值：无
**************************************************************************/
void Beep_Set(uint8_t sw)
{
	HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, (GPIO_PinState)sw);
}





















