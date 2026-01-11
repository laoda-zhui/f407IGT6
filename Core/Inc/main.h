/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */


/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Task_Beep_Pin GPIO_PIN_13
#define Task_Beep_GPIO_Port GPIOC
#define CAN_Select_Pin GPIO_PIN_10
#define CAN_Select_GPIO_Port GPIOF
#define WheelLED_L_Pin GPIO_PIN_10
#define WheelLED_L_GPIO_Port GPIOH
#define WheelLED_R_Pin GPIO_PIN_11
#define WheelLED_R_GPIO_Port GPIOH
#define LED1_Pin GPIO_PIN_12
#define LED1_GPIO_Port GPIOH
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOH
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOH
#define LED4_Pin GPIO_PIN_15
#define LED4_GPIO_Port GPIOH
#define Trig_Pin GPIO_PIN_15
#define Trig_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_4
#define KEY1_GPIO_Port GPIOI
#define KEY2_Pin GPIO_PIN_5
#define KEY2_GPIO_Port GPIOI
#define KEY3_Pin GPIO_PIN_6
#define KEY3_GPIO_Port GPIOI
#define KEY4_Pin GPIO_PIN_7
#define KEY4_GPIO_Port GPIOI

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
