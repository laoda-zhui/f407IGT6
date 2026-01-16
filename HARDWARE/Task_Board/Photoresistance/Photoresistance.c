#include "Photoresistance.h"


#define 	Photoresistance_GPIO_Port 			GPIOG
#define 	Photoresistance_Pin 				GPIO_PIN_8




/**************************************************************************
函数功能：光敏电阻-获取值
入口参数：无
返回  值：光线强时：输出低电平(0) 光线弱时：输出高电平(1)。
**************************************************************************/
uint8_t Phsis_GetValue(void)
{
	return HAL_GPIO_ReadPin(Photoresistance_GPIO_Port, Photoresistance_Pin);
}












