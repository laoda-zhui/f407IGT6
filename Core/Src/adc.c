/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */

#define PWR_MAX  12000.0    // 12V
#define PWR_MIN   9000.0    // 9V
#define PWR_DV   (PWR_MAX - PWR_MIN)

uint16_t ADC_Data=0,ADC_DataPre=0,ADC_DataValue=0;

uint8_t ADC_BeginFlag=0, Power=0, i=0;

float Pa = 0, Pb = 0;

void Parameter_Init(void) // 电量计算参数初始化
{
	 Pb =(float) (PWR_MIN / PWR_DV);
	 Pb *= 100;

	 Pa = (float)(3300*11)/4096 ;
	 Pa = (float)((Pa *100) /PWR_DV);
}


/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  Parameter_Init();




  /* USER CODE END ADC1_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA6     ------> ADC1_IN6
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC1 interrupt Init */
    HAL_NVIC_SetPriority(ADC_IRQn, 13, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA6     ------> ADC1_IN6
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);

    /* ADC1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(ADC_IRQn);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/*---------------------------------------------------
函数名称: Filter()
函数功能: 平滑滤波
参数    : 带滤波样本
返回值  ：滤波结果
-----------------------------------------------------*/
#define FILTER_N 10		     //定义数组长度为八位

uint16_t filter_buf[FILTER_N+1];  //定义数组


uint16_t Smoothing_Filtering(uint16_t value)
{
	int i;
	uint16_t filter_sum=0;
	filter_buf[FILTER_N] = value;		  //AD转换的值赋给数组的最后一个值
	for(i=0;i<FILTER_N;i++)
	{
		filter_buf[i]=filter_buf[i+1];	  //所有的数据左移，数组第一个元素摒弃
		filter_sum+=filter_buf[i];
	}
	return(uint16_t)(filter_sum/FILTER_N);		  //返回对数组里的元素求得的平均值
}

uint32_t MLib_GetSub(uint32_t a,uint32_t b)
{
	return (a > b)? a-b:b-a;
}


/**************************************************************************
函数功能：启动检测电量并上传数据
入口参数：无
返回  值：无
**************************************************************************/
void Power_TxandStart(void)
{
	if(ADC_BeginFlag == 1)
	{
		ADC_BeginFlag = 0;

		ADC_Data = (uint16_t)(ADC_Data/10.0);
		ADC_Data = Smoothing_Filtering(ADC_Data);
		ADC_DataValue = MLib_GetSub(ADC_Data,ADC_DataPre);

		if(ADC_DataPre == 0)
		{
			ADC_Data = (Pa * ADC_Data); // 电量计算方法
			if(ADC_Data < Pb){Power =0;}
			else
			{
			   Power = (uint8_t) ( ADC_Data - Pb);
			   if( Power >100){Power =100;}
			}

			CAN_TxtoPower(1, Power);
		}

		ADC_DataPre = ADC_Data;
		if(ADC_DataValue > 10)
		{
			ADC_Data = (Pa * ADC_Data); // 电量计算方法
			if( ADC_Data < Pb ){Power =0;}
			else
			{
				Power = (uint8_t)(ADC_Data - Pb);
				if( Power >100){Power =100;}
			}
			CAN_TxtoPower(1, Power);
		}
		ADC_Data = 0;
	}
	HAL_ADC_Start_IT(&hadc1);
}





/**************************************************************************
函数功能：ADC-转换完成中断
入口参数：无
返回  值：无
**************************************************************************/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if((hadc == &hadc1) && ADC_BeginFlag==0)
	{
		ADC_Data += HAL_ADC_GetValue(hadc);
		i++;
		if(i >= 10)
		{
			i = 0;
			ADC_BeginFlag = 1;
		}


	}

}


/* USER CODE END 1 */
