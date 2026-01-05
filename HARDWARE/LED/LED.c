#include "LED.h"

/*引脚定义*/
#define 	LED1_GPIO_Port 			GPIOH
#define 	LED1_Pin 				GPIO_PIN_12

#define 	LED2_GPIO_Port 			GPIOH
#define 	LED2_Pin 				GPIO_PIN_13

#define 	LED3_GPIO_Port 			GPIOH
#define 	LED3_Pin 				GPIO_PIN_14

#define 	LED4_GPIO_Port 			GPIOH
#define 	LED4_Pin 				GPIO_PIN_15



/**************************************************************************
函数功能：LED1翻转
入口参数：Time:闪烁周期
返回  值：无
**************************************************************************/
void LED1Toogle(uint32_t Time)
{
	static uint32_t CurrentTime=0,LastTime=0;
	CurrentTime = HAL_GetTick();
	if((CurrentTime - LastTime)>Time)
	{
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		LastTime = CurrentTime;
	}
}

/**************************************************************************
函数功能：LED2翻转
入口参数：Time:闪烁周期
返回  值：无
**************************************************************************/
void LED2Toogle(uint32_t Time)
{
	static uint32_t CurrentTime=0,LastTime=0;
	CurrentTime = HAL_GetTick();
	if((CurrentTime - LastTime)>Time)
	{
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		LastTime = CurrentTime;
	}
}

/**************************************************************************
函数功能：LED3翻转
入口参数：Time:闪烁周期
返回  值：无
**************************************************************************/
void LED3Toogle(uint32_t Time)
{
	static uint32_t CurrentTime=0,LastTime=0;
	CurrentTime = HAL_GetTick();
	if((CurrentTime - LastTime)>Time)
	{
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		LastTime = CurrentTime;
	}
}

/**************************************************************************
函数功能：LED4翻转
入口参数：Time:闪烁周期
返回  值：无
**************************************************************************/
void LED4Toogle(uint32_t Time)
{
	static uint32_t CurrentTime=0,LastTime=0;
	CurrentTime = HAL_GetTick();
	if((CurrentTime - LastTime)>Time)
	{
		HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
		LastTime = CurrentTime;
	}
}
