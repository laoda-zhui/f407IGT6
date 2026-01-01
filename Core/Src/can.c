/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "can.h"

/* USER CODE BEGIN 0 */

/*CAN接收结构体*/
CAN_RxHeaderTypeDef RxMsgArray;

/*CAN接收缓冲数组*/
uint8_t RxData[20]={0};

/*CAN接收成功标志位 0-无数据 1-接收到数据*/
uint8_t MyCAN_RxFlag = 0;

/******************************************************************************
CAN滤波器结构体数组 - 数组参考can.h
成员:左边:ID 右边:MASK
数组:1.显示模块 2.WiFi模块 3.Zigbee模块 4.寻迹模块 5.导航模块 6.主机模块 7.anything-留着接收从车等
*******************************************************************************/
static const Can_Filter_Struct SFilterArry[]={
		{0x7800, 	0x7C00},	 /*1*/
		{0x5000, 	0x7C00},	 /*2*/
		{0x5400, 	0x7C00},	 /*3*/
		{0x1C00, 	0x7C00},	 /*4*/
		{0x2000, 	0x7C00},	 /*5*/
		{0x3C00, 	0x7C00},	 /*6*/
		{0	   ,		 0},	 /*7*/
		{0	   ,		 0}		 /*补位，无作用,使总数为偶数*/
};


/**************************************************************************
函数功能：设置CAN模块的模式
入口参数：0-高速模式，1-待机只接受不发送
返回  值：无
**************************************************************************/
void Hard_Can_SpeedMode(uint8_t f)
{
      HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, (GPIO_PinState)f);
}


/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */


  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 3;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_10TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = ENABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */


  /**************************************************************************
   *  用户-配置和初始化滤波器
   *  注意:SFilterArry结构体数组要为偶数倍(奇数倍麻烦不写)
   **************************************************************************/
  for(uint8_t i=0;i < (sizeof(SFilterArry)/sizeof(Can_Filter_Struct));i+=2)
  {
	  CAN_FilterTypeDef SFilterConfig;
	  SFilterConfig.FilterBank = i/2;                     			/*使用过滤器组*/
	  SFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK; 			/*使用掩码模式*/
	  SFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT; 			/*16位宽*/

	  SFilterConfig.FilterIdLow = SFilterArry[i].sid_id;             /*验证码ID低16位*/
	  SFilterConfig.FilterMaskIdLow = SFilterArry[i].sid_mask;       /*屏蔽码低16位 (0表示不关心该位)*/

	  SFilterConfig.FilterIdHigh = SFilterArry[i+1].sid_id;          /*验证码ID高16位*/
	  SFilterConfig.FilterMaskIdHigh = SFilterArry[i+1].sid_mask;      /*屏蔽码高16位 (0表示不关心该位)*/

	  SFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0; 		 /*匹配的报文放入FIFO0*/
	  SFilterConfig.FilterActivation = CAN_FILTER_ENABLE;          	 /*使能该过滤器*/
	  if (HAL_CAN_ConfigFilter(&hcan1, &SFilterConfig) != HAL_OK)
	  {
	      Error_Handler();
	  }

  }

  /*配置高速模式*/
  Hard_Can_SpeedMode(0);


  /**************************************************************************
   *对命令CAN_CMD_Tx结构体进行初始化
   **************************************************************************/
  Can_CmdStruct_Init();


  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**************************************************接收FIFO功能函数***************************************************************/

/**************************************************************************
函数功能：CAN-底层发送函数
入口参数：TxMessage:发送结构体CAN_TxHeaderTypeDef Data:发送数据
返回 值：HAL_OK:发送成功 HAL_TIMEOUT:发送超时
注   意:这里使用超时判断为滴答定时器，注意中断优先级，在中断调用该函数需注意
**************************************************************************/
HAL_StatusTypeDef MyCAN_Transmit(CAN_TxHeaderTypeDef *TxMessage, uint8_t *Data)
{
	uint32_t Used_pTxMailbox;
	uint32_t CurTime,timeout_ms;

	CurTime = HAL_GetTick(); /*当前时间*/
	timeout_ms = 9;		 	 /*等待超时时间*/

	while(HAL_CAN_AddTxMessage(&hcan1, TxMessage, Data, &Used_pTxMailbox) != HAL_OK)
	{
		if( (HAL_GetTick() - CurTime) > timeout_ms)
		{
			return HAL_TIMEOUT;
		}
	}
	return HAL_OK;
}

/**************************************************************************
函数功能：CAN-底层查询接收标志
入口参数：无
返回  值：0-未接收到数据，1-接收到数据
**************************************************************************/
uint8_t MyCAN_ReceiveFlag()
{
	if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_FILTER_FIFO0))
	{
		return 1;
	}
	return 0;
}


/**************************************************************************
函数功能：CAN-底层接收函数
入口参数：RxMessage:CAN_RxHeaderTypeDef结构体 Data:接收数据
返回  值：无
**************************************************************************/
void MyCAN_Receive(CAN_RxHeaderTypeDef *RxMessage, uint8_t *Data)
{
	HAL_CAN_GetRxMessage(&hcan1, CAN_FILTER_FIFO0, RxMessage, Data);
}

/**************************************************************************
函数功能：CAN-FIFO0邮箱有数据-回调中断函数
入口参数：无
返回  值：无
**************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(HAL_CAN_GetRxMessage(&hcan1, CAN_FILTER_FIFO0, &RxMsgArray, RxData)  == HAL_OK)	/*中断中接收*/
	{
		MyCAN_RxFlag = 1;	/*接收后标志位置1*/
	}
}

/* USER CODE END 1 */
