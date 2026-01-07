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

#define WHEEL_BASE_CM 17.0      // 车轮间距（两轮中心距离，单位cm）


int16_t Pre_LEncoder;		/*同步一次的码盘值*/
int16_t Pre_REncoder;		/*同步一次的码盘值*/

uint16_t Taget_Pulses=0; 	/*目标脉冲数*/
int16_t Taget_AnglePulse=0; /*目标转向脉冲数*/

int Car_Spend=0;			/*小车全局速度*/
double Car_Spend_ing =50; 	/*小车实际行进速度*/

/*小车标志位*/
uint8_t wheel_L_Flag=0, wheel_R_Flag=0;
uint8_t Go_Flag=0, Back_Flag=0;
uint8_t Track_Flag=0;
uint16_t Stop_Flag=0;


/*转向pid结构体*/
PID_t Turn_PID={
		.Kp = 0.4,
		.Ki = 0.0,
		.Kd = 0.1,
		.OutMin = -120,
		.OutMax = 120

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
	Motor_Control(0,0);
}






/**************************************************************************
函数功能：电机控制函数
入口参数：L_Spend 左侧电机速度  R_Spend 右侧电机转速
返回  值：无
**************************************************************************/
void Motor_Control(int L_Spend,int R_Spend)
{
	if(L_Spend > 120){L_Spend=120;}
	if(L_Spend > 0 && L_Spend < 15){L_Spend = 15;}
	if(L_Spend < -120){L_Spend=-120;}
	if(L_Spend < 0 && L_Spend > -15){L_Spend = -15;}

	if(R_Spend > 120){R_Spend=120;}
	if(R_Spend > 0 && R_Spend < 15){R_Spend = 15;}
	if(R_Spend < -120){R_Spend=-120;}
	if(R_Spend < 0 && R_Spend > -15){R_Spend = -15;}


	CAN_TxtoMotor(L_Spend, R_Spend);
}


/**************************************************************************
函数功能：同步一次当前左轮编码器值(左轮)
入口参数：无
返回  值：无
**************************************************************************/
void Motor_SyncLeftEncoder(void)
{
	Pre_LEncoder = CanHost_Mp;
}



/**************************************************************************
函数功能：同步一次当前右轮编码器值(右轮)
入口参数：无
返回  值：无
**************************************************************************/
void Motor_SyncRightEncoder(void)
{
	Pre_REncoder = CanHost_Mp1;
}



/**************************************************************************
函数功能：获取左编码器差值 - 最短差值
入口参数：无
返回  值：LDifcoder-左编码器差值 (脉冲数 0-65535)
**************************************************************************/
uint16_t Motor_GetLDifcoder(void)
{
    uint32_t LDifcoder;
    if(CanHost_Mp > Pre_LEncoder){LDifcoder = CanHost_Mp - Pre_LEncoder;}
    else{LDifcoder = Pre_LEncoder - CanHost_Mp;}
    if(LDifcoder > 0x8000){LDifcoder = 0xffff - LDifcoder;}

    return LDifcoder;
}


/**************************************************************************
函数功能：获取右编码器差值 - 最短差值
入口参数：无
返回  值：RDifcoder-编码器差值 (脉冲数 0-65535)
**************************************************************************/
uint16_t Motor_GetRDifcoder(void)
{
    uint32_t RDifcoder;
    if(CanHost_Mp1 > Pre_REncoder){RDifcoder = CanHost_Mp1 - Pre_REncoder;}
    else{RDifcoder = Pre_REncoder - CanHost_Mp1;}
    if(RDifcoder > 0x8000){RDifcoder = 0xffff - RDifcoder;}

    return RDifcoder;
}


/**************************************************************************
函数功能：获取左右 编码器差值 - 最短差值
入口参数：无
返回  值：LRDifcoder-编码器差值 (脉冲数 0-65535)
**************************************************************************/
uint16_t Motor_GetLRDifcoder(void)
{
    uint32_t LRDifcoder,RDifcoder,LDifcoder;


    RDifcoder = Motor_GetRDifcoder();
    LDifcoder = Motor_GetLDifcoder();

    if(RDifcoder > LDifcoder)
    {
    	LRDifcoder = RDifcoder - LDifcoder;
    }
    else
    {
    	LRDifcoder = LDifcoder - RDifcoder;
    }


    return LRDifcoder;
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
入口参数：speed:速度(max=120)  temp:前进距离cm
返回  值：无
**************************************************************************/
void Car_Go(uint8_t speed, uint16_t temp)   // 主车前进 参数：速度/距离（厘米）
{
	Motor_SyncLeftEncoder();      /*编码器同步一次*/
	if(Go_Flag == 1){return;}



    Stop_Flag = Task_Start;          // 运行状态标志位
    Go_Flag = 1;            // 前进标志位

    Taget_Pulses = Motor_calculate_pulses(temp);         // 距离转换为码盘值
    Car_Spend = speed;      // 速度值

}



/**************************************************************************
函数功能：控制小车后退
入口参数：speed:速度(max=120)  temp:后退距离cm
返回  值：无
**************************************************************************/
void Car_Back(uint8_t speed, uint16_t temp) // 主车后退 参数：速度/距离（厘米）
{
	Motor_SyncLeftEncoder();     /*编码器同步一次*/
	if(Back_Flag == 1){return;}



    Stop_Flag = Task_Start;          // 运行状态标志位
    Back_Flag = 1;          // 后退标志位

    Taget_Pulses = Motor_calculate_pulses(temp);         // 距离转换为码盘值
    Car_Spend = speed;      // 速度值

}



/**************************************************************************
函数功能：控制小车左转
入口参数：speed:速度(max=120) Angle:转弯角度
返回  值：无
**************************************************************************/
void Car_Left(uint8_t speed, double Angle)       // 主车左转 参数：角度参考值
{
	Motor_SyncLeftEncoder();     /*左编码器同步一次*/							/*参考： 左转角度			真实值
																				90°				56.25


	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	*/
	Motor_SyncRightEncoder();    /*右编码器同步一次*/
	if(wheel_L_Flag == 1){return;}

	PID_Clear(&Turn_PID);	/*清空一次转向pid*/
	Taget_AnglePulse = Motor_calculate_pulses(Angle*(M_PI/180.0)*WHEEL_BASE_CM);
	Turn_PID.Target = (float)Taget_AnglePulse;
    Stop_Flag = Task_Start;          // 运行状态标志位
    wheel_L_Flag = 1;       // 左转标志位
    Car_Spend = speed;      // 速度值

}



/**************************************************************************
函数功能：控制小车右转
入口参数：speed:速度(max=120) Angle:转弯角度
返回  值：无
**************************************************************************/
void Car_Right(uint8_t speed, double Angle)       // 主车右转 参数：角度参考值
{
	Motor_SyncLeftEncoder();     /*左编码器同步一次*/
	Motor_SyncRightEncoder();    /*右编码器同步一次*/						/*参考： 右转角度			真实值
																			90°				62.07


	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	*/
	if(wheel_R_Flag == 1){return;}

	PID_Clear(&Turn_PID);	/*清空一次转向pid*/
	Taget_AnglePulse = Motor_calculate_pulses(Angle*(M_PI/180.0)*WHEEL_BASE_CM);
	Turn_PID.Target = (float)Taget_AnglePulse;
    Stop_Flag = Task_Start;          // 运行状态标志位
    wheel_R_Flag = 1;       // 右转标志位

    Car_Spend = speed;      // 速度值
}

/**************************************************************************
函数功能：控制小车开始循迹
入口参数：speed:速度(max=120)
返回  值：无
**************************************************************************/
void Car_Track(uint8_t speed)   // 主车循迹 参数：速度
{
	if(Track_Flag == 1){return;}

    Stop_Flag = Task_Start;          // 运行状态标志位
    Track_Flag = 1;         // 循迹标志位
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
	if((Go_Flag == 1 || Back_Flag == 1) &&  (Stop_Flag == Task_Start) && (Go_and_Back_CheckFlag == 1)) // 前进状态
	{
		Go_and_Back_CheckFlag = 0;

		if(Car_Spend_ing < Car_Spend && Motor_GetLDifcoder() <= 0.2 * Taget_Pulses)
		{ // 起步阶段，缓慢加速，保护电机同时也能增强视觉效果
			Car_Spend_ing += 0.2; // 通过循环，缓慢加速
		}
		else if(Car_Spend_ing > Car_Spend)
		{
			Car_Spend_ing -= 0.2; // 当多次控制速度不一致时，用于调节速度
		}

		// 达到行驶路程的一定阶段，开始减速，防止车辆因惯性导致距离产生误差，加入车轮单圈转动距离以及车辆速度与的动态计算，同时增加最低限速，防止电机抖动
		if(Motor_GetLDifcoder() >= 0.6 * Taget_Pulses && 0.5 * Taget_Pulses < oneCircle * 2 * (Car_Spend / 100.00))
		{
			Car_Spend_ing = Car_Spend_ing <= 15 ? 15 : Car_Spend_ing - 1;
		}
		else if(Motor_GetLDifcoder() >= 0.6 * Taget_Pulses && (Taget_Pulses - Motor_GetLDifcoder()) <= oneCircle * 2 * (Car_Spend / 100.00))
		{
			Car_Spend_ing = Car_Spend_ing <= 15 ? 15 : Car_Spend_ing - 1;
		}
		if(Taget_Pulses <= Motor_GetLDifcoder()) // 行驶距离大于等于需要行驶的码盘值/距离时，停车
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
	if((wheel_L_Flag == 1 || wheel_R_Flag == 1) &&  (Stop_Flag == Task_Start) && (TurnCheckFlag==1))
	{
		TurnCheckFlag = 0;

		Turn_PID.Actual = Motor_GetLRDifcoder();

		PIDSpeed_Update(&Turn_PID);

		if(Motor_GetLRDifcoder() > Taget_AnglePulse)	/*达到目标*/
		{
			Stop_Flag = Task_Complete;
			Car_Spend_ing = 50;
			Motor_Control(0,0);		// 停止
		}
		else
		{
			if(wheel_L_Flag == 1)
			{
				Motor_Control((Car_Spend - (int)Turn_PID.Out),(Car_Spend + (int)Turn_PID.Out)); // 没有前进到指定距离时，继续行驶
			}
			else if(wheel_R_Flag == 1)
			{
				Motor_Control((Car_Spend + (int)Turn_PID.Out),(Car_Spend - (int)Turn_PID.Out)); // 没有后退到指定距离时，继续行驶
			}
		}


	}

}





/****************************PID结构体**********************************/
PID_t PID_TurnTrack={
		.Kp = 1.0,
		.Ki = 0.0,
		.Kd = 0.0,
		.ErrorIntMax = 1000,
		.ErrorIntMin = -1000,
		.OutMax = 120,
		.OutMin = -120,
		.Target = 0
};



/**************************************************************************
函数功能：循迹刷新
入口参数：无
返回  值：无
**************************************************************************/
void Track_Check(void) 	/*这里选择取循迹的后面八个-x1，右边到左右依次增大，第n个等于2的(n-1)次方*/
{
	static int8_t err=0; /*误差*/

	if((Track_Flag == 1) && (Stop_Flag == Task_Start) && (Track_CheckFlag == 1))
	{
		Track_CheckFlag = 0;

		switch(x1)
		{
			case 0x3C:	/*十字路口全灭停止 0011 1100,  0000 0000*/
			case 0x00:
			{
				err = 0;
				Stop_Flag = Task_Complete;
				Motor_Control(0,0);
				break;
			}

			case 0xE7: err = 0; break;		/*直线 1110 0111*/

			case 0xEF: err = -1; break;		/*左转1 1110 1111*/
			case 0xCF: err = -2; break;		/*左转2  1100 1111*/
			case 0xDF: err = -3; break;		/*左转2  1101 1111*/
			case 0x9F: err = -5; break;		/*左转3  1001 1111*/
			case 0xBF: err = -6; break;		/*左转3  1011 1111*/
			case 0x3F: err = -8; break;		/*左转3  0011 1111*/
			case 0x7F: err = -10; break;	/*左转4  0111 1111*/

			case 0xF7: err = 1; break;		/*右转1  1111 0111*/
			case 0xF3: err = 2; break;		/*右转2  1111 0011*/
			case 0xFB: err = 3; break;		/*右转2  1111 1011*/
			case 0xF9: err = 5; break;		/*右转3  1111 1001*/
			case 0xFD: err = 6; break;		/*右转2  1111 1101*/
			case 0xFC: err = 8; break;		/*右转2  1111 1100*/
			case 0xFE: err = 10; break;		/*右转4  1111 1110*/

			case 0xFF:	/*脱线了全亮*/
			{
				err = 0;
				uint8_t count;
				if(count > 2000)
				{
					count=0;
					Motor_Control(0,0);
					Stop_Flag = Task_Complete;
				}
				else
				{
					count++;
				}
				break;
			}
			default:
				break;
		}


		PID_TurnTrack.Actual = err;
		PIDSpeed_Update(&PID_TurnTrack);
		Motor_Control( (Car_Spend + PID_TurnTrack.Out),  (Car_Spend - PID_TurnTrack.Out) );

	}



//
//    if(gd == 0x00)	// 循迹灯全灭 停止
//    {
//        Track_Flag = 0;
//        Stop_Flag = 1;
//        Send_UpMotor(0,0);
//
//    }
//    else if(gd==0xE7)	/*直线*/
//    {
//        LSpeed=Car_Spend;
//        RSpeed=Car_Spend;
//    }
//    else if((gd==0xF7) || (gd==0XF3))  /*右转1*/
//    {
//        LSpeed=Car_Spend+20;
//        RSpeed=Car_Spend-40;
//    }
//    else if((gd==0XFB) || (gd==0XF9))  /*右转2*/
//    {
//        LSpeed=Car_Spend+40;
//        RSpeed=Car_Spend-60;
//    }
//    else if((gd==0XFD) || (gd==0XFC))  /*右转3*/
//    {
//        LSpeed=Car_Spend+60;
//        RSpeed=Car_Spend-90;
//    }
//    else if(gd==0XFE)	/*右转4*/
//    {
//        LSpeed=Car_Spend+80;
//        RSpeed=Car_Spend-120;
//    }
//    else if((gd==0XEF) || (gd==0XCF))	 /*左转1*/
//    {
//        RSpeed = Car_Spend+20;
//        LSpeed = Car_Spend-40;
//    }
//    else if((gd==0XDF) || (gd==0X9F))  /*左转2*/
//    {
//        RSpeed = Car_Spend+40;
//        LSpeed = Car_Spend-60;
//    }
//    else if((gd==0XBF) || (gd==0X3F))  /*左转3*/
//    {
//        RSpeed = Car_Spend+60;
//        LSpeed = Car_Spend-90;
//    }
//    else if(gd==0X7F)		/*左转4*/
//    {
//        RSpeed = Car_Spend+80;
//        LSpeed = Car_Spend-120;
//    }
//    else		/*其他情况*/
//    {
//        LSpeed = Car_Spend;
//        RSpeed = Car_Spend;
//    }
//
//    if(gd==0xFF)   //循迹灯全亮
//    {
//        LSpeed = Car_Spend;
//        RSpeed = Car_Spend;
//        if(count > 1200)
//        {
//            count=0;
//            Send_UpMotor(0,0);
//            Track_Flag=0;
//            Stop_Flag = 4;
//        }
//        else {
//            count++;
//        }
//    }
//    else
//    {
//        count=0;
//    }
//
//    if(Track_Flag != 0)
//    {
//        Control(LSpeed,RSpeed);
//    }
}



