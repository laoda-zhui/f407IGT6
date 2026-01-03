#include "Motor.h"


/**************************************************************************
  不同电机修改不同的参数，否则控制精度下降
**************************************************************************/
double wheelDiameter = 6.5;			// 车轮直径，单位cm
int gearRatio = 80; 				// 减速比，行星齿轮电机25，普通齿轮减速电机45，最老版本行星齿轮电机（Namiki）80  或根据电机减速比进行调整
int pulsesPerRevolution = 2; 		// 电机减速前每转一圈的脉冲数/PPR（现阶段全部使用11），Namiki电机为2，
int restore = 4;					// 分频还原，不一定所有电机都需要，暂不调整
uint16_t pulsesNeeded; 				// 单次运行的脉冲数总值，可与电机实际旋转的脉冲数量进行对比
uint16_t oneCircle; 				// 单次转1圈的脉冲数总值


uint16_t Pre_Encoder;		/*同步一次的码盘值*/
uint16_t Pre_Angle;			/*同步一次的角度值*/

uint16_t Taget_Pulses=0; 	/*目标脉冲数*/

int Car_Spend=0;			/*小车全局速度*/
double Car_Spend_ing =50; 	/*小车实际行进速度*/

uint8_t wheel_L_Flag=0, wheel_R_Flag=0;	/*小车标志位*/
uint8_t Go_Flag=0, Back_Flag=0;
uint8_t Track_Flag=0;
uint8_t Stop_Flag=0;

/*转向pid结构体*/
PID_t Turn_PID={
		.Kp = 0.0,
		.Ki = 0.0,
		.Kp = 0.0,
		.OutMin = -100,
		.OutMax = 100

};



/********************************************* 获取传感器等计算 ***********************************************/


/**************************************************************************
函数功能：电机驱动初始化
入口参数：无
返回  值：无
**************************************************************************/
void Motor_Init(void)
{
	HAL_TIM_Base_Start_IT(&htim9);	/*开启tim9中断*/
}




/**************************************************************************
函数功能：同步一次当前的角度值
入口参数：无
返回  值：无
**************************************************************************/
void Motor_GetAngle(void)
{
    Pre_Angle = Current_Angle;
}

/**************************************************************************
函数功能：获取角度差值
入口参数：无
返回  值：DifAngle-角度差值
**************************************************************************/
uint16_t Motor_GetDifAngle(void)
{
    uint16_t DifAngle;

    if(Current_Angle > Pre_Angle){DifAngle = Current_Angle - Pre_Angle;}
    else{DifAngle = Pre_Angle - Current_Angle;}

    while(DifAngle >= 36000){DifAngle -= 36000;}

    return DifAngle;
}


/**************************************************************************
函数功能：电机控制函数
入口参数：L_Spend 左侧电机速度  R_Spend 右侧电机转速
返回  值：无
**************************************************************************/
void Motor_Control(int L_Spend,int R_Spend)
{
    if(L_Spend>=0) 	/*限制速度参数*/
    {
        if(L_Spend>100)L_Spend=100;
        if(L_Spend<5)L_Spend=5;
    }
    else
    {
        if(L_Spend<-100)L_Spend= -100;
        if(L_Spend>-5)L_Spend= -5;
    }
    if(R_Spend>=0)
    {
        if(R_Spend>100)R_Spend=100;
        if(R_Spend<5)R_Spend=5;
    }
    else
    {
        if(R_Spend<-100)R_Spend= -100;
        if(R_Spend>-5)R_Spend= -5;
    }


	CAN_TxtoMotor(L_Spend, R_Spend);
}


/**************************************************************************
函数功能：同步一次当前的编码器值
入口参数：无
返回  值：无
**************************************************************************/
void Motor_GetEncoder(void)
{
	Pre_Encoder = CanHost_Mp;
}

/**************************************************************************
函数功能：获取编码器差值 - 最短差值
入口参数：无
返回  值：Difcoder-编码器差值 (脉冲数 0-65535)
**************************************************************************/
uint16_t Motor_GetDifcoder(void)
{
    uint32_t Difcoder;
    if(CanHost_Mp > Pre_Encoder){Difcoder = CanHost_Mp - Pre_Encoder;}
    else{Difcoder = Pre_Encoder - CanHost_Mp;}
    if(Difcoder > 0x8000){Difcoder = 0xffff - Difcoder;}

    return Difcoder;
}


/**************************************************************************
函数功能：计算行驶指定距离所需脉冲数的函数，将厘米转换为码盘值/编码器值，可以辅助实现车辆精确控制
入口参数：无
返回  值：编码器值
**************************************************************************/
uint16_t Motor_calculate_pulses(double distance_cm)
{
	double wheelCircumference = M_PI * wheelDiameter; 					// 计算车轮周长，使用M_PI常量，它是π的近似值
	double revolutionsNeeded = distance_cm / wheelCircumference;		// 计算车轮需要转的圈数
	double motorRevolutions;											// 计算电机需要转的圈数
	if(gearRatio == 80)
	{
		motorRevolutions = revolutionsNeeded * gearRatio * restore;		// 最老小车的Namiki电机需要还原分频 * 4
		pulsesPerRevolution = 2;										// 修改为此电机对应的PPR（单圈脉冲）2
	}
	else
	{
	  motorRevolutions = revolutionsNeeded * gearRatio;					// 计算现阶段电机需要转的圈数
	}
	pulsesNeeded = (uint16_t)ceil(motorRevolutions * pulsesPerRevolution);	// 计算单次启动行走距离的总脉冲
	oneCircle = gearRatio * pulsesPerRevolution;							// 计算单圈脉冲
	return pulsesNeeded;
}




/************************************************** 运行控制指令 ********************************************************************/

/**************************************************************************
函数功能：控制小车前进
入口参数：speed:速度(max=100)  temp:前进距离cm
返回  值：无
**************************************************************************/
void Car_Go(uint8_t speed, uint16_t temp)   // 主车前进 参数：速度/距离（厘米）
{
	if(Go_Flag == 1){return;}

	Motor_GetDifcoder();      /*编码器同步一次*/

    Stop_Flag = Task_Start;          // 运行状态标志位
    Go_Flag = 1;            // 前进标志位

    Taget_Pulses = Motor_calculate_pulses(temp);         // 距离转换为码盘值
    Car_Spend = speed;      // 速度值

}



/**************************************************************************
函数功能：控制小车后退
入口参数：speed:速度(max=100)  temp:后退距离cm
返回  值：无
**************************************************************************/
void Car_Back(uint8_t speed, uint16_t temp) // 主车后退 参数：速度/距离（厘米）
{
	if(Back_Flag == 1){return;}


	Motor_GetDifcoder();     /*编码器同步一次*/

    Stop_Flag = Task_Start;          // 运行状态标志位
    Back_Flag = 1;          // 后退标志位

    Taget_Pulses = Motor_calculate_pulses(temp);         // 距离转换为码盘值
    Car_Spend = speed;      // 速度值

}



/**************************************************************************
函数功能：控制小车左转
入口参数：speed:速度(max=100) Angle:转弯角度
返回  值：无
**************************************************************************/
void Car_Left(uint8_t speed, int16_t Angle)       // 主车左转 参数：速度
{
	if(wheel_L_Flag == 1){return;}

	PID_Clear(&Turn_PID);	/*清空一次转向pid*/

	Turn_PID.Target = Angle;
	Motor_GetAngle();	/*角度同步一次*/

    Stop_Flag = Task_Start;          // 运行状态标志位
    wheel_L_Flag = 1;       // 左转标志位

    Car_Spend = speed;      // 速度值

}



/**************************************************************************
函数功能：控制小车右转
入口参数：speed:速度(max=100) Angle:转弯角度
返回  值：无
**************************************************************************/
void Car_Right(uint8_t speed, int16_t Angle)       // 主车右转 参数：速度
{
	if(wheel_R_Flag == 1){return;}

	PID_Clear(&Turn_PID);	/*清空一次转向pid*/

	Turn_PID.Target = -Angle;
	Motor_GetAngle();	/*角度同步一次*/

    Stop_Flag = Task_Start;          // 运行状态标志位
    wheel_R_Flag = 1;       // 右转标志位

    Car_Spend = speed;      // 速度值
}


/********************************************* 路况执行 ***********************************************/


/**************************************************************************
函数功能：前进后退路况
入口参数：无
返回  值：无
**************************************************************************/
void Go_and_Back_Check(void)
{
	if((Go_Flag == 1 || Back_Flag == 1) &&  (Stop_Flag == Task_Start)) // 前进状态
	{
		if(Car_Spend_ing < Car_Spend && Motor_GetDifcoder() <= 0.2 * Taget_Pulses)
		{ // 起步阶段，缓慢加速，保护电机同时也能增强视觉效果
			Car_Spend_ing += 0.2; // 通过循环，缓慢加速
		}
		else if(Car_Spend_ing > Car_Spend)
		{
			Car_Spend_ing -= 0.2; // 当多次控制速度不一致时，用于调节速度
		}

		// 达到行驶路程的一定阶段，开始减速，防止车辆因惯性导致距离产生误差，加入车轮单圈转动距离以及车辆速度与的动态计算，同时增加最低限速，防止电机抖动
		if(Motor_GetDifcoder() >= 0.6 * Taget_Pulses && 0.5 * Taget_Pulses < oneCircle * 2 * (Car_Spend / 100.00))
		{
			Car_Spend_ing = Car_Spend_ing <= 15 ? 15 : Car_Spend_ing - 1;
		}
		else if(Motor_GetDifcoder() >= 0.6 * Taget_Pulses && (Taget_Pulses - Motor_GetDifcoder()) <= oneCircle * 2 * (Car_Spend / 100.00))
		{
			Car_Spend_ing = Car_Spend_ing <= 15 ? 15 : Car_Spend_ing - 1;
		}
		if(Taget_Pulses <= Motor_GetDifcoder()) // 行驶距离大于等于需要行驶的码盘值/距离时，停车
		{
			Stop_Flag = Task_Complete;
			Car_Spend_ing = 50;
			Motor_Control(0,0);		// 停止
		}
		else
		{
			if(Go_Flag == 1)
			{
				Motor_Control((int)Car_Spend_ing,(int)Car_Spend_ing); // 没有前进到指定距离时，继续行驶
			}
			else if(Back_Flag == 1)
			{
				Motor_Control(-(int)Car_Spend_ing,-(int)Car_Spend_ing); // 没有后退到指定距离时，继续行驶
			}
		}
	}
}



/**************************************************************************
函数功能：转弯角度检查
入口参数：无
返回  值：无
**************************************************************************/
void TurnAngle_Check(void)
{
	if((wheel_L_Flag == 1 || wheel_R_Flag == 1) &&  (Stop_Flag == Task_Start))
	{
		PID_Update(&Turn_PID);

		if(Pre_Angle <= Motor_GetDifAngle()) // 行驶距离大于等于需要行驶的码盘值/距离时，停车
		{
			Stop_Flag = Task_Complete;
			Car_Spend_ing = 50;
			Motor_Control(0,0);		// 停止
		}
		else
		{
			Motor_Control((int)(Car_Spend+Turn_PID.Out), (int)(Car_Spend-Turn_PID.Out));	/*我也不想写屎山*/
		}
	}

}




