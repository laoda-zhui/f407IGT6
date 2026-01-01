/* USER CODE BEGIN Header */
/*******************************************************************************
  *致吾辈好儿郎们
  *不要问我改的程序有没有问题;
  *这种东西就像问你女朋友爱不爱你一样
  *毫无意义;
  *程序有没有问题要长时间跑过才知道;
  *爱情亦是如此;
  *晚上记得早点睡;
  *加油!
  *电子界吴彦祖;
  *******************************************************************************/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled_spi.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  /*测试发送结构体*/
//  CAN_TxHeaderTypeDef TxMsgArray[] = {
//  /*  StdId   ExtId        IDE          RTR        DLC*/
//  	{0x3C0, 0x00000000, CAN_ID_STD, CAN_RTR_DATA,   8,},
//  	{0x280, 0x00000000, CAN_ID_STD, CAN_RTR_DATA,   8,},
//  	{0x2A0, 0x00000000, CAN_ID_STD, CAN_RTR_DATA, 	8,},
//  	{0x0E0, 0x00000000, CAN_ID_STD, CAN_RTR_DATA,  	8,},
//	{0x100, 0x00000000, CAN_ID_STD, CAN_RTR_DATA,   8,},
//	{0x1E0, 0x00000000, CAN_ID_STD, CAN_RTR_DATA,   8,},
//	{0x1B3, 0x00000000, CAN_ID_STD, CAN_RTR_DATA,   8,}
//  };

  uint8_t index=0;/*发送数组的索引*/
  uint8_t TxData[][8]={ /*发送can数据数组 -测试*/
		  {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
		  {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22},
		  {0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33},
		  {0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44},
		  {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55},
		  {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66},
		  {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77},
		  {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88},
		  {0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99},
		  {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA}
  };


  /*键值*/
  uint8_t KeyNum=0;

  /*启动CAN1*/
  __HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
  uint8_t error = HAL_CAN_Start(&hcan1);

  /*OLED初始化*/
  OLED_Init();
  OLED_ShowNum(0, 0, error, 2, OLED_6X8);

  OLED_ShowString(0, 0, "Info:", OLED_6X8);
  OLED_ShowString(0, 9, "WifiRx:", OLED_6X8);
  OLED_ShowString(0, 18, "ZigbRx:", OLED_6X8);
  OLED_ShowString(0, 27, "Track:", OLED_6X8);
  OLED_ShowString(0, 36, "Navig:", OLED_6X8);
  OLED_ShowString(0, 45, "HOST", OLED_6X8);
  OLED_ShowString(0, 54, "Anything", OLED_6X8);

  OLED_UpDate();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET)
	  {
		 HAL_Delay(10);
		 while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET);
		 HAL_Delay(10);
		 KeyNum = 1;
	  }

	  if(KeyNum == 1)
	  {
		  OLED_ShowNum(0, 10, index, 2, OLED_6X8);
		  OLED_UpDate();
		  KeyNum=0;
		  switch(index)
		  {
		  	  case 0:
		  		CAN_TxtoWifi(TxData[index], 8);
		  		break;
		  	  case 1:
		  		CAN_TxtoZigbee(TxData[index], 8);
		  		break;
		  	  case 2:
		  		CAN_TxtoDisplay(TxData[index], 8);
		  		break;
		  	  case 3:
		  		CAN_TxtoMotor(12, 34);
		  		break;

		  	  case 5:
		  		CAN_TxtoNV(2);
		  		break;
		  	  case 6:
		  		CAN_TxtoPower(45, 67);
		  		break;
		  	  case 7:
		  		CAN_TxtoT0(8);
		  		break;
		  	  case 8:
		  		CAN_TxtoT1(11, 11);
		  		break;
		  	  case 9:
		  		CAN_TxtoT2(100);
		  		break;
		  }
		  index++;
		  if(index >9){index = 0;}
	  }
	  CanRx_Loop();

	  OLED_ShowHexNum(40, 0,  FifoBuf_Info[0], 2, OLED_6X8); //1
	  OLED_ShowHexNum(52, 0,  FifoBuf_Info[1], 2,  OLED_6X8);
	  OLED_ShowHexNum(64, 0,  FifoBuf_Info[2], 2,  OLED_6X8);
	  OLED_ShowHexNum(76, 0,  FifoBuf_Info[3], 2, OLED_6X8);
	  OLED_ShowHexNum(88, 0,  FifoBuf_Info[4], 2, OLED_6X8); //1
	  OLED_ShowHexNum(100, 0,  FifoBuf_Info[5], 2,  OLED_6X8);
	  OLED_ShowHexNum(112, 0,  FifoBuf_Info[6], 2,  OLED_6X8);
	  OLED_ShowHexNum(124, 0,  FifoBuf_Info[7], 2, OLED_6X8);


	  OLED_ShowHexNum(40, 9,  FifoBuf_WifiRx[0], 2, OLED_6X8); //2
	  OLED_ShowHexNum(52, 9,  FifoBuf_WifiRx[1], 2,  OLED_6X8);
	  OLED_ShowHexNum(64, 9,  FifoBuf_WifiRx[2], 2,  OLED_6X8);
	  OLED_ShowHexNum(76, 9,  FifoBuf_WifiRx[3], 2, OLED_6X8);
	  OLED_ShowHexNum(88, 9,  FifoBuf_WifiRx[4], 2, OLED_6X8); //2
	  OLED_ShowHexNum(100, 9,  FifoBuf_WifiRx[5], 2,  OLED_6X8);
	  OLED_ShowHexNum(112, 9,  FifoBuf_WifiRx[6], 2,  OLED_6X8);
	  OLED_ShowHexNum(124, 9,  FifoBuf_WifiRx[7], 2, OLED_6X8);


	  OLED_ShowHexNum(40, 18,  FifoBuf_ZigbRx[0], 2, OLED_6X8); //3
	  OLED_ShowHexNum(52, 18,  FifoBuf_ZigbRx[1], 2,  OLED_6X8);
	  OLED_ShowHexNum(64, 18,  FifoBuf_ZigbRx[2], 2,  OLED_6X8);
	  OLED_ShowHexNum(76, 18,  FifoBuf_ZigbRx[3], 2, OLED_6X8);
	  OLED_ShowHexNum(88, 18,  FifoBuf_ZigbRx[4], 2, OLED_6X8); //3
	  OLED_ShowHexNum(100, 18,  FifoBuf_ZigbRx[5], 2,  OLED_6X8);
	  OLED_ShowHexNum(112, 18,  FifoBuf_ZigbRx[6], 2,  OLED_6X8);
	  OLED_ShowHexNum(124, 18,  FifoBuf_ZigbRx[7], 2, OLED_6X8);


	  OLED_ShowHexNum(40, 27,  FifoBuf_Track[0], 2, OLED_6X8);//4
	  OLED_ShowHexNum(52, 27,  FifoBuf_Track[1], 2,  OLED_6X8);
	  OLED_ShowHexNum(64, 27,  FifoBuf_Track[2], 2,  OLED_6X8);
	  OLED_ShowHexNum(76, 27,  FifoBuf_Track[3], 2, OLED_6X8);
	  OLED_ShowHexNum(88, 27,  FifoBuf_Track[4], 2, OLED_6X8);//4
	  OLED_ShowHexNum(100, 27,  FifoBuf_Track[5], 2,  OLED_6X8);
	  OLED_ShowHexNum(112, 27,  FifoBuf_Track[6], 2,  OLED_6X8);
	  OLED_ShowHexNum(124, 27,  FifoBuf_Track[7], 2, OLED_6X8);


	  OLED_ShowHexNum(40, 36,  FifoBuf_Navig[0], 2, OLED_6X8);//5
	  OLED_ShowHexNum(52, 36,  FifoBuf_Navig[1], 2,  OLED_6X8);
	  OLED_ShowHexNum(64, 36,  FifoBuf_Navig[2], 2,  OLED_6X8);
	  OLED_ShowHexNum(76, 36,  FifoBuf_Navig[3], 2, OLED_6X8);
	  OLED_ShowHexNum(88, 36,  FifoBuf_Navig[4], 2, OLED_6X8);//5
	  OLED_ShowHexNum(100, 36,  FifoBuf_Navig[5], 2,  OLED_6X8);
	  OLED_ShowHexNum(112, 36,  FifoBuf_Navig[6], 2,  OLED_6X8);
	  OLED_ShowHexNum(124, 36,  FifoBuf_Navig[7], 2, OLED_6X8);


	  OLED_ShowHexNum(40, 45,  FifoBuf_HOST[0], 2, OLED_6X8);//6
	  OLED_ShowHexNum(52, 45,  FifoBuf_HOST[1], 2,  OLED_6X8);
	  OLED_ShowHexNum(64, 45,  FifoBuf_HOST[2], 2,  OLED_6X8);
	  OLED_ShowHexNum(76, 45,  FifoBuf_HOST[3], 2, OLED_6X8);
	  OLED_ShowHexNum(88, 45,  FifoBuf_HOST[4], 2, OLED_6X8);//6
	  OLED_ShowHexNum(100, 45,  FifoBuf_HOST[5], 2,  OLED_6X8);
	  OLED_ShowHexNum(112, 45,  FifoBuf_HOST[6], 2,  OLED_6X8);
	  OLED_ShowHexNum(124, 45,  FifoBuf_HOST[7], 2, OLED_6X8);


	  OLED_ShowHexNum(40, 54,  FifoBuf_Anything[0], 2, OLED_6X8);//7
	  OLED_ShowHexNum(52, 54,  FifoBuf_Anything[1], 2,  OLED_6X8);
	  OLED_ShowHexNum(64, 54,  FifoBuf_Anything[2], 2,  OLED_6X8);
	  OLED_ShowHexNum(76, 54,  FifoBuf_Anything[3], 2, OLED_6X8);
	  OLED_ShowHexNum(88, 54,  FifoBuf_Anything[4], 2, OLED_6X8);//7
	  OLED_ShowHexNum(100, 54,  FifoBuf_Anything[5], 2,  OLED_6X8);
	  OLED_ShowHexNum(112, 54,  FifoBuf_Anything[6], 2,  OLED_6X8);
	  OLED_ShowHexNum(124, 54,  FifoBuf_Anything[7], 2, OLED_6X8);



	  OLED_UpDate();





    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
