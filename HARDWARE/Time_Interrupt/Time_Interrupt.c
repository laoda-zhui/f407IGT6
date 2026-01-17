#include "Time_Interrupt.h"


/**********************************************************
 *
 * 介于hal的中断回调机制,将回调函数放在一个文件处理
 *
 **********************************************************/

/*标志位*/
uint8_t TurnCheckFlag = 0;			/*转弯刷新标志位*/
uint8_t Go_and_Back_CheckFlag = 0;	/*前进后退刷新标志位*/
uint8_t Track_CheckFlag = 0;		/*循迹刷新标志位*/




/**************************************************************************
 HAL_TIM定时器中断回调函数
**************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint8_t Count1=0,Count2=0,Count3=0;
	if(htim == &htim9)	/*Motor-TIM9中断 psc:168-1 arr:=1000-1 中断时间:1ms*/
	{
		Count1++;
		Count2++;
		Count3++;

		if(Count1 > 15)
		{
			TurnCheckFlag = 1;
			Count1 = 0;
		}
		if(Count2 > 15)
		{
			Go_and_Back_CheckFlag = 1;
			Count2 = 0;
		}
		if(Count3 > 5)
		{
			Track_CheckFlag = 1;
			Count3=0;
		}




	}
	if(htim == &htim10) /*KEY-TIM10中断 psc:168-1 arr:=20000-1 中断时间:20ms*/
	{
		KeyNum_Tick();
	}

}


























