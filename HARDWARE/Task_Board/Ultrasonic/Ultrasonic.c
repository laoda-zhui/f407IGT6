#include "Ultrasonic.h"


/*超声波距离-cm			1us中断计次， 65535*1us*340m/s /2.0 = 11m 极限11.14m  */
float Distance=0;

/**************************************************************************
函数功能：超声波初始化
入口参数：无
返回  值：无
**************************************************************************/
void Ultrasonic_Init()
{
    HAL_TIM_Base_Stop(&htim3);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
}


/**************************************************************************
函数功能：启动一次超声波
入口参数：无
返回  值：无
**************************************************************************/
void Ultrasonic_Start()
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
	My_Delayus(5);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);

    // 1. 手动把计数器清零 (这是测量起点)
    __HAL_TIM_SET_COUNTER(&htim3, 0);

    // 2. 清除之前的标志位
    __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_CC2);

    // 3. 开启定时器和通道1捕获 (捕获上升沿)
    HAL_TIM_Base_Start(&htim3);
    HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);


}


/**************************************************************************
函数功能：超声波输入捕获中断
入口参数：无
返回  值：无
**************************************************************************/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim3)
	{
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			uint32_t DownEdge,UpEdge;


			UpEdge = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			DownEdge = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			if(DownEdge > UpEdge)
			{
				Distance = (DownEdge - UpEdge)* 0.017;
			}


			HAL_TIM_IC_Stop(htim, TIM_CHANNEL_1);
	        HAL_TIM_IC_Stop_IT(htim, TIM_CHANNEL_2);
	        HAL_TIM_Base_Stop(htim);
		}
	}
}




























