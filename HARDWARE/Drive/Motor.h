#ifndef DRIVE_MOTOR_H_
#define DRIVE_MOTOR_H_

#include "main.h"
#include "can_RxSolve.h"
#include "math.h"
#include "tim.h"
#include "PID.h"
#include "Time_Interrupt.h"


/*测试记得删*/
extern PID_t Turn_PID;
extern PID_t PID_TurnTrack;

/*任务执行状态*/
typedef enum{
	Task_Start 	  = 	1,	/*小车启动*/
	Task_Complete = 	2,	/*小车完成*/
}Task_Flag;
extern Task_Flag Stop_Flag;


/*小车执行状态-行进标志位*/
typedef enum{
	Go_Flag 	  = 0,	/*小车前进*/
	Back_Flag,			/*小车后退*/
	wheel_L_Flag,		/*小车MP左转*/
	wheel_R_Flag,		/*小车MP右转*/
	Track_Flag,			/*小车循迹*/
	TrackTime_Flag,		/*小车时间循迹*/
	TrackMp_Flag,		/*小车码盘循迹*/
	TurnLeft_Flag,		/*新小车左转*/
	TurnRight_Flag,		/*新小车右转*/

}Car_Flag;
extern Car_Flag CarFlag;



/*电机控制函数*/
void Motor_Control(int L_Spend,int R_Spend);
void Car_Go(uint8_t speed, uint16_t temp);
void Car_Back(uint8_t speed, uint16_t temp);
void Car_MPLeft(uint8_t speed, double Angle) ;
void Car_MPRight(uint8_t speed, double Angle);
void Car_Track(uint8_t speed);
void Car_TrackTime(uint8_t speed, uint32_t Time);
void Car_TrackMp(uint8_t speed, uint16_t Temp);
void Car_Left(uint8_t SpeedAll);
void Car_Right(uint8_t SpeedAll);


/*电机初始化函数*/
void Motor_Init(void);


/*路况刷新*/
void Go_and_Back_Check(void);
void TurnAngle_Check(void);
void Track_Check(void);
void TurnAngle_NewCheck(void);


/*编码器功能函数*/
uint16_t Motor_calculate_pulses(double distance_cm);
uint16_t Motor_GetLDifcoder(void);
uint16_t Motor_GetRDifcoder(void);
uint16_t Motor_GetLRDifcoder(void);
void Motor_SyncLeftEncoder(void);
void Motor_SyncRightEncoder(void);


#endif /* DRIVE_MOTOR_H_ */
