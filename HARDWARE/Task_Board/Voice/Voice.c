#include "Voice.h"



uint8_t Voice_RxData[10]={0};		// VoiceUART 接收数据缓存
uint8_t Voice_Rx_Complete_Flag=0;

/**************************************************************************
函数功能：Voice-发送数组
入口参数：无
返回  值：无
**************************************************************************/
void Voice_Init(void)
{
	HAL_UART_Receive_DMA(&huart6, Voice_RxData, 3);
}



/**************************************************************************
函数功能：Voice-发送数组
入口参数：无
返回  值：无
**************************************************************************/
void Voice_SendData(uint8_t *TxData, uint8_t Size)
{
	HAL_UART_Transmit(&huart6, TxData, Size, 50);
}




/**************************************************************************
函数功能：Voice-接收中断回调函数
入口参数：无
返回  值：无
**************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart6)
	{
		Voice_Rx_Complete_Flag = 1;
		HAL_UART_Receive_DMA(huart, Voice_RxData, 3);
	}
}



/**************************************************************************
函数功能：Voice-执行语音识别（ASR）操作 识别到后给回传数据 (美好生活:55020100 秀丽山河:55020200 追逐梦想:55020300 扬帆启航:55020400 齐头并进:55020500)
		再根据回传的数据发送Voice_RxData[2]的数据，并且播报相应的语音
入口参数：无
返回  值：返回识别结果的标志
**************************************************************************/
uint8_t Voice_ASR(void)
{
	uint32_t TimeStart=0;
	uint32_t TimeOut=1000; /*1000ms超时检测*/

	uint8_t Frame[5];		// 保存发送命令的数组
	uint8_t Voice_Flag_2 = 1;
	uint8_t Voice_Flag   = 0;
	char Buf[50];

	/* 发送开启语音识别指令 */
	Frame[0] = 0xFA;
	Frame[1] = 0xFA;
	Frame[2] = 0xFA;
	Frame[3] = 0xFA;
	Frame[4] = 0xA1;
	Voice_SendData(Frame, 5);

	TimeStart = HAL_GetTick();

	while(Voice_Flag_2)
	{
		if(Voice_Rx_Complete_Flag == 1)
		{
			Voice_Rx_Complete_Flag = 0;
			if(Voice_RxData[0] == 0x55)
			{
				if (Voice_RxData[1] == 0x02)
				{
					Voice_Flag &= 0xF0;
					Voice_SendData(&Voice_RxData[2],1);
					Voice_Flag |= Voice_RxData[2];
					sprintf(Buf, "bkrc_voice_V1\nID: %d\n",Voice_Flag);
					CAN_TxtoDisplay(Buf, strlen(Buf));
					Voice_Flag_2 = 0;
				}
			}
		}

		if(HAL_GetTick() - TimeStart > TimeOut)/*超时检测*/
		{
			Voice_Flag_2 = 0;
		}

	}

	return Voice_Flag;
}



/**************************************************************************
函数功能：Voice-发送命令 根据命令播报语音
入口参数：Command: 1-美好生活 2-秀丽山河 3-追逐梦想 4-扬帆启航 5-齐头并进
返回  值：无
**************************************************************************/
void Voice_SendCommand(uint8_t Command)
{
	uint8_t TxData=0;
	switch(Command)
	{
		case 1:
			TxData = 0x01;
			HAL_UART_Transmit(&huart6, &TxData, 1, 50);
			break;
		case 2:
			TxData = 0x02;
			HAL_UART_Transmit(&huart6, &TxData, 1, 50);
			break;
		case 3:
			TxData = 0x03;
			HAL_UART_Transmit(&huart6, &TxData, 1, 50);
			break;
		case 4:
			TxData = 0x04;
			HAL_UART_Transmit(&huart6, &TxData, 1, 50);
			break;
		case 5:
			TxData = 0x05;
			HAL_UART_Transmit(&huart6, &TxData, 1, 50);
			break;
	}
}






