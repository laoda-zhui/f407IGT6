#include "KEY.h"

 uint8_t KeyNum=0;

 /**************************************************************************
 函数功能：获取键值对初始化
 入口参数：无
 返回  值：键码值
 **************************************************************************/
 void Key_Init(void)
 {
	 HAL_TIM_Base_Start_IT(&htim10);

 }


/**************************************************************************
函数功能：主函数获取键码值
入口参数：无
返回  值：键码值
**************************************************************************/
uint8_t Key_GetNum(void)
{
	uint8_t Temp;
	if (KeyNum)
	{
		Temp = KeyNum;
		KeyNum = 0;
		return Temp;
	}
	return 0;
}

uint8_t KeyNum_GetState(void)
{
	if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_4) == GPIO_PIN_RESET)
	{
		return 1;
	}
	if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_5) == GPIO_PIN_RESET)
	{
		return 2;
	}
	if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_6) == GPIO_PIN_RESET)
	{
		return 3;
	}
	if(HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_7) == GPIO_PIN_RESET)
	{
		return 4;
	}
	return 0;
}

/**************************************************************************
函数功能：通过时钟中断不断刷新获取键码值
入口参数：无
返回  值：无
**************************************************************************/
void KeyNum_Tick(void)
{
	static uint8_t CurrState=0,PrevState=0;
	PrevState = CurrState;
	CurrState = KeyNum_GetState();
	if (CurrState == 0 && PrevState != 0)
	{
		KeyNum = PrevState;
	}

}




