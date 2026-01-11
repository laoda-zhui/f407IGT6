#include "WheelLED.h"

#define 	WheelLEDL_GPIO_Port 			GPIOH
#define 	WheelLEDL_Pin 				GPIO_PIN_10

#define 	WheelLEDR_GPIO_Port 			GPIOH
#define 	WheelLEDR_Pin 				GPIO_PIN_11


/**************************************************************************
函数功能： 任务板转向灯控制
入口参数：LorR  L_LED--左侧   R_LED--右侧
		swch  GPIO_PIN_SET -- 开     RESET--关
返回  值： 无
**************************************************************************/
void Set_tba_WheelLED(uint8_t LorR,uint8_t sw)
{
	uint8_t output = (sw == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET;

	switch(LorR)
	{
		case L_LED:

				HAL_GPIO_WritePin(WheelLEDL_GPIO_Port, WheelLEDL_Pin, output);
			break;
		case R_LED:

				HAL_GPIO_WritePin(WheelLEDR_GPIO_Port, WheelLEDR_Pin, output);
			break;
	}
}




