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

uint8_t c,FilterNumber;

/******************************************************************************
CAN滤波器结构体数组 - 数组参考can.h
成员:左边:ID 右边:MASK
数组:0.显示模块 1.WiFi模块 2.Zigbee模块 3.寻迹模块 4.导航模块 5.主机模块 6.anything-留着接收从车等
*******************************************************************************/
Can_Filter_Struct SFilterArry[]=
{

	{CAN_SID_HL(ID_DISP,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},	/*0*/
	{CAN_SID_HL(ID_WIFI,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},	/*1*/
	{CAN_SID_HL(ID_ZIGBEE,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},	/*2*/
	{CAN_SID_HL(ID_TRACK,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},	/*3*/
	{CAN_SID_HL(ID_NAVIG,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},	/*4*/
	{CAN_SID_HL(ID_HOST,	ID_ZERO),CAN_SID_HL(ID_ALL,ID_ZERO)},	/*5*/
	{0,0,}, /*6*/

};


static uint16_t CanDrv_Fiter_Create16bit(uint32_t s,uint32_t e,uint8_t RTR,uint8_t IDE)
{
    s &= 0x03ff;
    RTR = (RTR)? 0x10:0;
    IDE = (IDE)? 0x08:0;
    e = (IDE)? (e>>15)&0x03:0;
    return (uint16_t)(s<<5)|RTR|IDE|e;
}



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



void Filter_Init(void)
{
	#define FXR1_LOW	CAN_FilterInitStructure.FilterIdLow
	#define FXR1_HIG	CAN_FilterInitStructure.FilterIdHigh
	#define FXR2_LOW	CAN_FilterInitStructure.FilterMaskIdLow
	#define FXR2_HIG	CAN_FilterInitStructure.FilterMaskIdHigh

	uint8_t len;
	len = sizeof(SFilterArry)/sizeof(SFilterArry[0]);
    FilterNumber = 0;
    c = 0;

    CAN_FilterTypeDef  CAN_FilterInitStructure;

    CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	CAN_FilterInitStructure.FilterActivation = CAN_FILTER_ENABLE;


    while(1)
    {
		CAN_FilterInitStructure.FilterMode	= CAN_FILTERMODE_IDMASK;
		CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_16BIT;
		FXR1_LOW = CanDrv_Fiter_Create16bit(SFilterArry[c].sid_id,	 0,0,0);
		FXR2_LOW = CanDrv_Fiter_Create16bit(SFilterArry[c].sid_mask, 0,0,0);
		if(++c < len)
		{
			FXR1_HIG = CanDrv_Fiter_Create16bit(SFilterArry[c].sid_id,	0,0,0);
			FXR2_HIG = CanDrv_Fiter_Create16bit(SFilterArry[c].sid_mask,0,0,0);
		}

		CAN_FilterInitStructure.FilterBank = FilterNumber;
		HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterInitStructure);

		if(c < len)
			c++;
		else
			break;

		if(++FilterNumber >= 13)
			break;

    }
}








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
	uint32_t Timeout=1000;		/*重试次数*/


	while(HAL_CAN_AddTxMessage(&hcan1, TxMessage, Data, &Used_pTxMailbox) != HAL_OK)
	{
		Timeout--;
		if(Timeout == 0){return HAL_TIMEOUT;}

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

/**************************************************************************
函数功能：CAN-初始化(使能can通信中断,开启can通信)
入口参数：无
返回  值：无
**************************************************************************/
void MyCan_Init(void)
{
	  __HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
	  HAL_CAN_Start(&hcan1);

	  /*配置高速模式*/
	  Hard_Can_SpeedMode(0);

	  /**************************************************************************
	   *对命令CAN_CMD_Tx结构体进行初始化
	   **************************************************************************/
	  Can_CmdStruct_Init();

}

/* USER CODE END 1 */
