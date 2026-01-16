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

/**************************************************************************
函数功能：BH1750-解除i2c断电再上电死锁 再I2C初始化之前调用(已经放在i2c.c初始化中)
入口参数：无
返回  值：无
**************************************************************************/
void I2C_ForceReset(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1. 修改这里：请根据您的原理图确认引脚！
    // 假设是 I2C1，通常是 PB6(SCL) 和 PB7(SDA)
    // 如果您的板子不是这两个引脚，请务必修改！！！
    GPIO_TypeDef* I2C_PORT = GPIOB;
    uint16_t PIN_SCL = GPIO_PIN_6;
    uint16_t PIN_SDA = GPIO_PIN_7;

    // 开启 GPIOB 时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 2. 将 SCL 和 SDA 配置为推挽输出模式 (Output Push-Pull)
    // 目的是让 MCU 能够强行控制引脚电平
    GPIO_InitStruct.Pin = PIN_SCL | PIN_SDA;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C_PORT, &GPIO_InitStruct);

    // 3. 开始模拟时钟信号 (这也是最关键的一步)

    // 先拉高 SDA 和 SCL
    HAL_GPIO_WritePin(I2C_PORT, PIN_SDA, GPIO_PIN_SET);
    HAL_GPIO_WritePin(I2C_PORT, PIN_SCL, GPIO_PIN_SET);
    HAL_Delay(1);

    // 产生 9 个时钟脉冲
    for(int i=0; i<9; i++)
    {
        // SCL 拉低
        HAL_GPIO_WritePin(I2C_PORT, PIN_SCL, GPIO_PIN_RESET);
        HAL_Delay(1);

        // SCL 拉高 (此时传感器会认为是一个时钟周期，释放一位数据)
        HAL_GPIO_WritePin(I2C_PORT, PIN_SCL, GPIO_PIN_SET);
        HAL_Delay(1);
    }

    // 4. 发送 STOP 信号 (停止位)
    // STOP 信号的时序是：在 SCL 为高电平时，SDA 由低变高
    HAL_GPIO_WritePin(I2C_PORT, PIN_SCL, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(I2C_PORT, PIN_SDA, GPIO_PIN_RESET); // SDA 先拉低
    HAL_Delay(1);

    HAL_GPIO_WritePin(I2C_PORT, PIN_SCL, GPIO_PIN_SET);   // SCL 拉高
    HAL_Delay(1);

    HAL_GPIO_WritePin(I2C_PORT, PIN_SDA, GPIO_PIN_SET);   // SDA 拉高 -> 产生 STOP
    HAL_Delay(1);

    // 解锁完成！引脚状态不用管，后面的 MX_I2C1_Init 会重新接管它们。
}
