#include "bh1750.h"


#define 	I2C_ADDR_GPIO_Port 			GPIOG
#define 	I2C_ADDR_Pin 				GPIO_PIN_15


#define	 	BH1750Address	 			0x46  //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改



/**************************************************************************
函数功能：BH1750-写命令
入口参数：Command - 写入命令
返回  值：HAL_StatusTypeDef
**************************************************************************/
uint8_t BH1750_WriteCommand(uint8_t Command)
{
	uint8_t Status;
	Status = HAL_I2C_Master_Transmit(&hi2c1, BH1750Address, &Command, 1, 100);
	return Status;
}



/**************************************************************************
函数功能：BH1750-读取光照度
入口参数：无
返回  值：0-65535 Lux
**************************************************************************/
uint16_t BH1750_GetLux(void)
{
	uint8_t Buf[2];
	uint16_t Lux=0;

	if(HAL_I2C_Master_Receive(&hi2c1, BH1750Address, Buf, 2, 50) == HAL_OK)
	{
		Lux = (Buf[0]<<8)+Buf[1];
		Lux =	(uint16_t)(Lux/1.2f);
		return Lux;
	}
	return 0;
}



/**************************************************************************
函数功能：BH1750-初始化
入口参数：无
返回  值：无
**************************************************************************/
void BH1750_Init(void)
{
	HAL_Delay(50);

	uint8_t Status;
	uint16_t TimeOut=0;


	HAL_GPIO_WritePin(I2C_ADDR_GPIO_Port, I2C_ADDR_Pin, GPIO_PIN_RESET);	/*ADD - 0*/
	Status = BH1750_WriteCommand(0x01);

	while(Status != HAL_OK)
	{
		TimeOut++;
		if(TimeOut >10){break;}
		Status = BH1750_WriteCommand(0x01);
		HAL_Delay(10);
	}
	TimeOut = 0;

	Status = BH1750_WriteCommand(0x10);
	while(Status != HAL_OK)
	{
		TimeOut++;
		if(TimeOut >10){break;}
		Status = BH1750_WriteCommand(0x10);
		HAL_Delay(10);
	}

}


