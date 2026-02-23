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

int Car_Speed=0;			/*小车全局速度*/
int Car_LSpeed=0,Car_RSpeed=0;
double Car_Speed_ing =20; 	/*小车实际行进速度*/

uint32_t Turn_StartTime=0;  /*转弯检测时间*/

uint32_t TrackTimeOut=0,TrackStartTime=0;

/*小车任务状态-行进标志位*/
Car_Flag CarFlag;
/*任务执行状态*/
Task_Flag Stop_Flag;







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
void Motor_Control(int L_Spend, int R_Spend)
{
    // --- 设定最小启动阈值 (根据你的电机实测，通常在 15~20 之间) ---
    const int MIN_SPEED = 15;

    // 1. 处理左轮死区
    if(L_Spend > 0 && L_Spend < MIN_SPEED) L_Spend = MIN_SPEED;
    if(L_Spend < 0 && L_Spend > -MIN_SPEED) L_Spend = -MIN_SPEED;

    // 2. 处理右轮死区
    if(R_Spend > 0 && R_Spend < MIN_SPEED) R_Spend = MIN_SPEED;
    if(R_Spend < 0 && R_Spend > -MIN_SPEED) R_Spend = -MIN_SPEED;

    // 3. 原有的限幅逻辑 (最大120)
    if(L_Spend > 120) L_Spend = 120;
    if(L_Spend < -120) L_Spend = -120;
    if(R_Spend > 120) R_Spend = 120;
    if(R_Spend < -120) R_Spend = -120;

    // 4. 执行发送
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
	Motor_SyncRightEncoder();

    Taget_Pulses = Motor_calculate_pulses(temp);         // 距离转换为码盘值

    Car_Speed = speed;      // 速度值

    Stop_Flag = Task_Start;          // 运行状态标志位
    CarFlag = Go_Flag;            // 前进标志位

}



/**************************************************************************
函数功能：控制小车后退
入口参数：speed:速度(max=120)  temp:后退距离cm
返回  值：无
**************************************************************************/
void Car_Back(uint8_t speed, uint16_t temp) // 主车后退 参数：速度/距离（厘米）
{
	Motor_SyncLeftEncoder();     /*编码器同步一次*/
	Motor_SyncRightEncoder();


    Taget_Pulses = Motor_calculate_pulses(temp);         // 距离转换为码盘值
    Car_Speed = speed;      // 速度值

    Stop_Flag = Task_Start;          // 运行状态标志位
    CarFlag = Back_Flag;          // 后退标志位

}



/**************************************************************************
函数功能：根据码盘差值控制小车左转 -  (推荐30)
入口参数：speed:速度(max=120) Angle:转弯角度
返回  值：无
**************************************************************************/
void Car_MPLeft(uint8_t speed, double Angle)       // 主车左转 参数：角度参考值
{
	Motor_SyncLeftEncoder();     /*左编码器同步一次*/							/*参考： 左转角度			真实值  速度(30情况下)
																				90°				87
																				45°				48

	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 		  */
	Motor_SyncRightEncoder();    /*右编码器同步一次*/

	PID_Clear(&Turn_PID);	/*清空一次转向pid*/
	Taget_AnglePulse = Motor_calculate_pulses(Angle*(M_PI/180.0)*WHEEL_BASE_CM/3.0);
	Turn_PID.Target = (float)Taget_AnglePulse;
    Car_Speed = speed;      // 速度值

    Stop_Flag = Task_Start;          // 运行状态标志位
    CarFlag = wheel_L_Flag;       // 左转标志位


}


/**************************************************************************
函数功能：根据码盘差控制小车右转 - (推荐30)
入口参数：speed:速度(max=120) Angle:转弯角度
返回  值：无
**************************************************************************/
void Car_MPRight(uint8_t speed, double Angle)       // 主车右转 参数：角度参考值
{
	Motor_SyncLeftEncoder();     /*左编码器同步一次*/
	Motor_SyncRightEncoder();    /*右编码器同步一次*/						/*参考： 右转角度			真实值	速度(30情况下)
																			90°				85
																			45°				45

	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	*/
	PID_Clear(&Turn_PID);	/*清空一次转向pid*/
	Taget_AnglePulse = Motor_calculate_pulses(Angle*(M_PI/180.0)*WHEEL_BASE_CM/3.0);
	Turn_PID.Target = (float)Taget_AnglePulse;

    Car_Speed = speed;      // 速度值

    Stop_Flag = Task_Start;          // 运行状态标志位
    CarFlag = wheel_R_Flag;       // 右转标志位


}


/**************************************************************************
函数功能：根据巡线控制控制小车左转 - 推荐 -80 80
入口参数：speed:速度(max=120)
返回  值：无
**************************************************************************/
void Car_Left(uint8_t SpeedAll)       // 主车左转 参数：角度参考值
{
	Turn_StartTime = HAL_GetTick();

    Car_LSpeed = -SpeedAll*1.1;      // 速度值
    Car_RSpeed = SpeedAll;

    Stop_Flag = Task_Start;          // 运行状态标志位
    CarFlag = TurnLeft_Flag;       // 左转标志位


}



/**************************************************************************
函数功能：根据巡线控制小车右转  推荐 80 -80
入口参数：speed:速度(max=120)
返回  值：无
**************************************************************************/
void Car_Right(uint8_t SpeedAll)       // 主车右转 参数：角度参考值
{
	Turn_StartTime = HAL_GetTick();
    Car_LSpeed = SpeedAll;      // 速度值
    Car_RSpeed = -SpeedAll*1.1;

    Stop_Flag = Task_Start;          // 运行状态标志位
    CarFlag = TurnRight_Flag;       // 右转标志位
}




/**************************************************************************
函数功能：控制小车开始循迹
入口参数：speed:速度(max=120)
返回  值：无
**************************************************************************/
void Car_Track(uint8_t speed)   // 主车循迹 参数：速度
{

    Car_Speed = speed;      // 速度值
    Stop_Flag = Task_Start;          // 运行状态标志位
    CarFlag = Track_Flag;         // 循迹标志位
}


/**************************************************************************
函数功能：控制小车规定时间内循迹
入口参数：speed:速度(max=120) Time:循迹时间
返回  值：无
**************************************************************************/
void Car_TrackTime(uint8_t speed, uint32_t Time)   // 主车循迹 参数：速度
{
	TrackStartTime = HAL_GetTick();
	TrackTimeOut = Time;
    Car_Speed = speed;      // 速度值

    Stop_Flag = Task_Start;          // 运行状态标志位
    CarFlag = TrackTime_Flag;         // 循迹标志位

}


/**************************************************************************
函数功能：控制小车规定距离内循迹
入口参数：speed:速度(max=120) Mp:循迹距离
返回  值：无
**************************************************************************/
void Car_TrackMp(uint8_t speed, uint16_t Temp)   // 主车循迹 参数：速度
{
	Motor_SyncLeftEncoder();     /*左编码器同步一次*/
	Motor_SyncRightEncoder();    /*右编码器同步一次*/

	Taget_Pulses = Motor_calculate_pulses(Temp);         // 距离转换为码盘值

    Car_Speed = speed;      // 速度值

    Stop_Flag = Task_Start;          // 运行状态标志位
    CarFlag = TrackMp_Flag;         // 循迹标志位
}

/**************************************************************************
函数功能：控制小车规定距离内后退循迹 (不行别用)
入口参数：speed:速度(max=120) Mp:循迹距离
返回  值：无
**************************************************************************/
void Car_TrackBackMp(uint8_t speed, uint16_t Temp)   // 主车循迹 参数：速度
{
	Motor_SyncLeftEncoder();     /*左编码器同步一次*/
	Motor_SyncRightEncoder();    /*右编码器同步一次*/

	Taget_Pulses = Motor_calculate_pulses(Temp);         // 距离转换为码盘值

	Car_Speed = -speed;      // 速度值

    Stop_Flag = Task_Start;          // 运行状态标志位
    CarFlag = TrackMpBack_Flag;         // 循迹标志位


}


/**************************************************************************
函数功能：控制小车白卡循迹(白卡长与黑线平行)
入口参数：speed:速度(max=120) Mp:循迹距离
返回  值：无
**************************************************************************/
void Car_TrackRFID(uint8_t speed)   // 主车循迹 参数：速度
{
    Car_Speed = speed;      // 速度值

    Stop_Flag = Task_Start;          // 运行状态标志位
    CarFlag = TrackRFID_Flag;         // 循迹标志位
}




/********************************************* 路况执行 ***********************************************/




/**************************************************************************
函数功能：前进后退路况
入口参数：无
返回  值：无
**************************************************************************/
void Go_and_Back_Check(void)
{
	if((CarFlag == Go_Flag|| CarFlag == Back_Flag) &&  (Stop_Flag == Task_Start) && (Go_and_Back_CheckFlag == 1)) // 前进状态
	{
		Go_and_Back_CheckFlag = 0;

		if(Car_Speed_ing < Car_Speed && Motor_GetLDifcoder() <= 0.2 * Taget_Pulses)
		{ // 起步阶段，缓慢加速，保护电机同时也能增强视觉效果
			Car_Speed_ing += 0.2; // 通过循环，缓慢加速
		}
		else if(Car_Speed_ing > Car_Speed)
		{
			Car_Speed_ing -= 0.2; // 当多次控制速度不一致时，用于调节速度
		}

		// 达到行驶路程的一定阶段，开始减速，防止车辆因惯性导致距离产生误差，加入车轮单圈转动距离以及车辆速度与的动态计算，同时增加最低限速，防止电机抖动
		if(Motor_GetLDifcoder() >= 0.6 * Taget_Pulses && 0.5 * Taget_Pulses < oneCircle * 2 * (Car_Speed / 100.00))
		{
			Car_Speed_ing = Car_Speed_ing <= 15 ? 15 : Car_Speed_ing - 1;
		}
		else if(Motor_GetLDifcoder() >= 0.6 * Taget_Pulses && (Taget_Pulses - Motor_GetLDifcoder()) <= oneCircle * 2 * (Car_Speed / 100.00))
		{
			Car_Speed_ing = Car_Speed_ing <= 15 ? 15 : Car_Speed_ing - 1;
		}
		if(Taget_Pulses <= Motor_GetLDifcoder()) // 行驶距离大于等于需要行驶的码盘值/距离时，停车
		{
			Stop_Flag = Task_Complete;
			Car_Speed_ing = 45;
			Motor_Control(0,0);		// 停止
			return;
		}
		else
		{
			if(CarFlag == Go_Flag)
			{
				Motor_Control((int)Car_Speed_ing,(int)Car_Speed_ing); // 没有前进到指定距离时，继续行驶
			}
			else if(CarFlag == Back_Flag)
			{
				Motor_Control(-(int)Car_Speed_ing,-(int)Car_Speed_ing); // 没有后退到指定距离时，继续行驶
			}
		}
	}
}


/*转向pid结构体*/
PID_t Turn_PID={
		.Kp = 0.6,
		.Ki = 0.0,
		.Kd = 0.1,
		.OutMin = -120,
		.OutMax = 120

};
/**************************************************************************
函数功能：转弯角度刷新执行
入口参数：无
返回  值：无
**************************************************************************/
void TurnAngle_Check(void)
{
	if((CarFlag == wheel_L_Flag||CarFlag == wheel_R_Flag) &&  (Stop_Flag == Task_Start) && (TurnCheckFlag==1))
	{
		TurnCheckFlag = 0;

		Turn_PID.Actual = Motor_GetLRDifcoder();

		PIDSpeed_Update(&Turn_PID);

		if(Motor_GetLRDifcoder() > Taget_AnglePulse)	/*达到目标*/
		{
			Stop_Flag = Task_Complete;
			Car_Speed_ing = 50;
			Motor_Control(0,0);		// 停止
			return;
		}
		else
		{
			if(CarFlag == wheel_L_Flag)
			{
				Motor_Control((Car_Speed - (int)Turn_PID.Out),(Car_Speed + (int)Turn_PID.Out)); // 没有前进到指定距离时，继续行驶
			}
			else if(CarFlag == wheel_R_Flag)
			{
				Motor_Control((Car_Speed + (int)Turn_PID.Out),(Car_Speed - (int)Turn_PID.Out)); // 没有后退到指定距离时，继续行驶
			}
		}


	}

}

/**************************************************************************
函数功能：转弯角度刷新执行-新
入口参数：无
返回  值：无
**************************************************************************/
void TurnAngle_NewCheck(void)
{
	if((CarFlag == TurnLeft_Flag || CarFlag == TurnRight_Flag) &&  (Stop_Flag == Task_Start) && (TurnCheckFlag==1))
	{
		TurnCheckFlag = 0;
		Motor_Control(Car_LSpeed, Car_RSpeed);

		uint32_t runTime = HAL_GetTick() - Turn_StartTime;

		if(runTime < 500){return;} /*小于400ms检测黑线*/
		if (runTime > 4000)/*大于4s说明脱线了*/
		{
			Motor_Control(0,0);
			Stop_Flag = Task_Complete;
			return;
		}

		if( (x1 & 0x08) == 0x00 && CarFlag == TurnLeft_Flag)
		{
			if (x1 != 0x00)
			{
				Stop_Flag = Task_Complete;
				Motor_Control(0,0);
			}
		}
		if((x1 & 0x18) != 0x18&& CarFlag == TurnRight_Flag)
		{
			if (x1 != 0x00)
			{
				Stop_Flag = Task_Complete;
				Motor_Control(0,0);
			}
		}

	}
}



/****************************PID结构体**********************************/
PID_t PID_TurnTrack = {
    .Kp = 4.3,    // 原 9.5 -> 降为 5.5 (因为下面的 error 值变大了，总输出依然够大)
    .Ki = 0.0,    // 保持 0，循迹不需要积分
    .Kd = 65.0,   // 原 2.5 -> 猛增到 45.0 (核心！重车防抖专用阻尼)
    .OutMax = 250, // 稍微放宽限幅，保证大弯能转过来
    .OutMin = -250,
    .Target = 0
};



/**************************************************************************
函数功能：循迹刷新
入口参数：无
返回  值：无
**************************************************************************/
void Track_Check(void) 	/*这里选择取循迹的后面八个-x1，右边到左右依次增大，第n个等于2的(n-1)次方*/
{
	static int8_t err; /*误差*/
	static uint16_t count=0;
	if((CarFlag == Track_Flag || CarFlag == TrackTime_Flag || CarFlag == TrackMp_Flag || CarFlag == TrackMpBack_Flag ||
			CarFlag == TrackRFID_Flag ) && (Stop_Flag == Task_Start) && (Track_CheckFlag == 1))
	{
		Track_CheckFlag = 0;

		if(CarFlag == TrackTime_Flag)	/*循迹时间超时*/
		{
			if(HAL_GetTick() - TrackStartTime > TrackTimeOut)
			{
				err = 0;
				Stop_Flag = Task_Complete;
				Motor_Control(0,0);
				return;
			}
		}

		if(CarFlag == TrackMp_Flag || CarFlag == TrackMpBack_Flag)  /*MP循迹 过头了*/
		{
			if(Taget_Pulses <= Motor_GetLDifcoder()) // 行驶距离大于等于需要行驶的码盘值/距离时，停车
			{
				err = 0;
				Stop_Flag = Task_Complete;
				Motor_Control(0,0);
				return;
			}
		}

		if(CarFlag == TrackRFID_Flag) /*白卡循迹*/
		{
			if((x1&0x81) == 0x00 || (x1 & 0xC3)==0x00) /*普通十字路口白卡循迹补丁*/
			{
				err = 0;
				Stop_Flag = Task_Complete;
				Motor_Control(0,0);
				return;
			}
			switch(x1)
			{

			case 0xFF: /*1111 1111*/
					count = 0;
					err = 0;
					Motor_Control(0,0);
					Stop_Flag = Task_Complete;

			}
		}


		/*循迹补丁*/
		if((x1 & 0x3C) == 0x00)
		{
			Motor_Control(0, 0);
		    Stop_Flag = Task_Complete;
		    return;
		}


	    switch(x1)
	    {
	        case 0x00: // 全亮/全黑 (根据实际情况调整)
	        {
	            if(count > 1000) {
	                count = 0;
	                Motor_Control(0,0);
	                Stop_Flag = Task_Complete;
	                return;
	            } else {
	                count++;
	                // 保持上一次 err，利用惯性盲走
	            }
	            break;
	        }

	        // --- 直线 ---
	        case 0xE7: err = 0; count=0; break;       /* 1110 0111 */

	        // --- 左偏 (Error 为负) ---
	        // 小偏：err 设大一点，让 PID 算出的值能突破静摩擦
	        case 0xEF: err = -25; count=0; break;     /* 1110 1111 */
	        case 0xC7: err = -35; count=0; break;     /* 1100 0111 */

	        // 中偏：平滑过渡
	        case 0xCF: err = -40; count=0; break;     /* 1100 1111 */
	        case 0xDF: err = -42; count=0; break;     /* 1101 1111 */
	        case 0x9F: err = -47; count=0; break;     /* 1001 1111 */

	        // 大偏：边缘拉回
	        case 0x8F: err = -55; count=0; break;     /* 1000 1111 */
	        case 0xBF: err = -60; count=0; break;     /* 1011 1111 */
	        case 0x3F: err = -65; count=0; break;     /* 0011 1111 */
	        case 0x1F: err = -75; count=0; break;     /* 0001 1111 */
	        case 0x7F: err = -85; count=0; break;     /* 0111 1111 */

	        // --- 右偏 (Error 为正) ---
	        case 0xF7: err = 25; count=0; break;      /* 1111 0111 */
	        case 0xE3: err = 35; count=0; break;      /* 1110 0011 */

	        case 0xF3: err = 40; count=0; break;      /* 1111 0011 */
	        case 0xFB: err = 42; count=0; break;      /* 1111 1011 */
	        case 0xF9: err = 47; count=0; break;      /* 1111 1001 */

	        case 0xF1: err = 55; count=0; break;      /* 1111 0001 */
	        case 0xFD: err = 60; count=0; break;      /* 1111 1101 */
	        case 0xFC: err = 65; count=0; break;      /* 1111 1100 */
	        case 0xF8: err = 75; count=0; break;      /* 1111 1000 */
	        case 0xFE: err = 85; count=0; break;      /* 1111 1110 */

	        case 0xFF: // 没线
	        {
	             if(count > 1000) {
	                count = 0;
	                Motor_Control(0,0);
	                Stop_Flag = Task_Complete;
	             } else {
	                count++;
	             }
	             break;
	        }
	        default: break;
	    }

	    // -------------------------------------------------------------
	    // 3. 动态 PID 计算 (Gain Scheduling)
	    // -------------------------------------------------------------
	    // 逻辑：大弯降 Kp 防甩，小弯标准 Kp
	    float abs_err = fabs(err);
	    if(abs_err > 55)
	    {
	        PID_TurnTrack.Kp = 3.0; // 大弯柔和模式
	    }
	    else
	    {
	        PID_TurnTrack.Kp = 4.2; // 小弯标准模式
	    }


		PID_TurnTrack.Actual = err;
		PIDSpeed_Update(&PID_TurnTrack);
		float Turn_Compensation = fabs(PID_TurnTrack.Out) * 0.3f; // 补偿系数 0.15


		if(CarFlag == TrackMpBack_Flag)
		{
			Motor_Control( (Car_Speed + PID_TurnTrack.Out + Turn_Compensation),  (Car_Speed - PID_TurnTrack.Out + Turn_Compensation) );
		}
		else
		{
			Motor_Control( (Car_Speed - PID_TurnTrack.Out + Turn_Compensation),  (Car_Speed + PID_TurnTrack.Out + Turn_Compensation) );
		}

	}

}



