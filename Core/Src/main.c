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
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can_Tx.h"
#include "can_RxSolve.h"
#include "Motor.h"
#include "PID.h"
#include "Beep.h"
#include "Delay.h"
#include "KEY.h"
#include "LED.h"
#include "infrared.h"
#include "Ultrasonic.h"
#include "bh1750.h"
#include "WheelLED.h"
#include "Task_Beep.h"
#include "can_cmd.h"
#include "Voice.h"
#include "Photoresistance.h"
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

uint8_t TestData[8]={0x56,0x03,0x9f,0x01,0x02,0x03,0x04,0xBB};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/**************************************************************************
函数功能：硬件初始化
入口参数：无
返回  值：无
**************************************************************************/
void Hardware_Init(void)
{
	/*获取键值对函数初始化*/
	Key_Init();

	/*启动CAN1*/
	MyCan_Init();
	Filter_Init();

	/*超声波初始化*/
	Ultrasonic_Init();

	/*红外管初始化*/
	Infrared_Init();

	/*Delay初始化*/
	My_Delay_Init();


	/*电机初始化*/
	Motor_Init();

	/*BH1750初始化*/
	BH1750_Init();	/*没电会初始化失败,瞬间上电也会初始化失败(需要复位)*/

	/*小创语音助手初始化*/
	Voice_Init();
}



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
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_ADC1_Init();
  MX_TIM9_Init();
  MX_TIM10_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_USART6_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /*硬件初始化*/
  Hardware_Init();

  /*键值对变量*/
  uint8_t KeyNum=0;

  /*任务切片时间-ms*/
  uint32_t ADC_TaskTime=100,ADC_LastTime=0;/*ADC切片*/

  uint32_t BH1750_TaskTime=180,BH1750_LastTime=0;/*BH1750任务时间*/
  uint8_t BH1750_StartFlag=0;	/*BH1750开启标志位 0-关闭 1-开启*/

  uint32_t Ultrasonic_TaskTime=100,Ultrasonic_LastTime=0;/*超声波任务时间*/
  uint8_t Ultrasonic_StartFlag=0;	/*超声波开启标志位 0-关闭 1-开启*/


  /*光照值Lux 0-65535*/
  uint16_t Lux=0,LuxTemp=0;

  /*测试变量-记得删*/
  float data;
  uint8_t Command = 1;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  /*按键*/
	  KeyNum = Key_GetNum();
	  if(KeyNum)
	  {
		  if(KeyNum == 1)
		  {
			  Beep_Set(1);
			  HAL_Delay(100);
			  Beep_Set(0);
			  HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_12);

			  char Txdata1[100];
			  sprintf(Txdata1,"%d\n\r",Lux);
			  CAN_TxtoDisplay(Txdata1, strlen(Txdata1));
		  }
		  if(KeyNum == 2)
		  {
			  Beep_Set(1);
			  HAL_Delay(100);
			  Beep_Set(0);
			  HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_13);


			  char Txdata2[50];
			  data = Distance;
			  sprintf(Txdata2,"%.2f\n\r",data);
			  CAN_TxtoDisplay(Txdata2, strlen(Txdata2));

		  }
		  if(KeyNum == 3)
		  {
			  Beep_Set(1);
			  HAL_Delay(100);
			  Beep_Set(0);
			  HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_14);


			  Voice_SendCommand(Command);
			  Command++;
			  if(Command>=6){Command=1;}
		  }
		  if(KeyNum == 4)
		  {
			  Beep_Set(1);
			  HAL_Delay(100);
			  Beep_Set(0);
			  HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_15);

			  uint8_t i;
			  i = Phsis_GetValue();
			  char Txdata2[50];
			  sprintf(Txdata2,"%d\n\r",i);
			  CAN_TxtoDisplay(Txdata2, strlen(Txdata2));
		  }
	  }


	  /*刷新*/
	  Go_and_Back_Check();
	  TurnAngle_Check();
	  Track_Check();

	  CanRx_Loop();
	  CAN_TxLoop();


	  /******************************任务切片******************************/
	  /*ADC_TaskTime-100ms 触发十次后 上传电量数据*/
	  if( (HAL_GetTick() - ADC_LastTime) > ADC_TaskTime)
	  {
		  Power_TxandStart();
		  ADC_LastTime = HAL_GetTick();
	  }
	  /*BH1750-180ms,获取光照值lux*/
	  if( ((HAL_GetTick() - BH1750_LastTime) > BH1750_TaskTime) && BH1750_StartFlag == 1)
	  {
		  LuxTemp = BH1750_GetLux();
		  if(LuxTemp)
		  {
			  Lux = LuxTemp;
		  }
		  BH1750_LastTime = HAL_GetTick();
	  }
	  /*BH1750-180ms,获取光照值lux*/
	  if( ((HAL_GetTick() - Ultrasonic_LastTime) > Ultrasonic_TaskTime) && Ultrasonic_StartFlag == 1)
	  {
		  Ultrasonic_Start();
		  Ultrasonic_LastTime = HAL_GetTick();
	  }




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
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
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
