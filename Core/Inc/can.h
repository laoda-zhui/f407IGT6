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
#include "tim.h"

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan1;

/* USER CODE BEGIN Private defines */

/*复制自商家的定义*/
#define ID_MOTOR	0x01
#define ID_TRACK	0x07
#define ID_NAVIG	0x08
#define ID_HOST		0x0f
#define ID_WIFI		0x14
#define ID_ZIGBEE	0x15
#define ID_DISP		0x1e

#define ID_ALL		0x3f
#define ID_ZERO		0x00

#define CAN_SID_HL(s,d)	((((uint16_t)s&0x3f)<<5)|(d&0x1f))



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

/*MyCan初始化*/
void MyCan_Init(void);

void Filter_Init(void);


/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

