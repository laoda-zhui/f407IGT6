/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include "can_Tx.h"
#include "can_RxSolve.h"


/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan1;

/* USER CODE BEGIN Private defines */

/*CAN接收结构体*/
extern CAN_RxHeaderTypeDef RxMsgArray;

/*滤波器id和掩码mask结构体*/
typedef struct _Can_Filter_Struct
{
	uint16_t sid_id;	/*ID*/
	uint16_t sid_mask;	/*MASK*/
}Can_Filter_Struct;

/*CAN接收缓冲数组*/
extern uint8_t RxData[20];

/*CAN接收成功标志位 0-无数据 1-接收到数据*/
extern uint8_t MyCAN_RxFlag;

/* USER CODE END Private defines */

void MX_CAN1_Init(void);

/* USER CODE BEGIN Prototypes */




/*CAN-底层发送函数*/
HAL_StatusTypeDef MyCAN_Transmit(CAN_TxHeaderTypeDef *TxMessage, uint8_t *Data);

/*CAN-底层接收函数*/
void MyCAN_Receive(CAN_RxHeaderTypeDef *RxMessage, uint8_t *Data);
uint8_t MyCAN_ReceiveFlag();

/*GPIO配置外部CAN硬件的模式*/
void Hard_Can_SpeedMode(uint8_t f);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

