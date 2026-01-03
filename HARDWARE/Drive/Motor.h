#ifndef DRIVE_MOTOR_H_
#define DRIVE_MOTOR_H_

#include "main.h"
#include "can_RxSolve.h"
#include "math.h"
#include "tim.h"
#include "PID.h"

/*任务状态*/
enum Task_Flag{
	Task_Start 	  = 	0x00,	/*小车启动*/
	Task_Complete = 	0x01,	/*小车完成*/

};

/*任务状态标志位*/
extern uint8_t Stop_Flag;	/*停止标志位*/
extern uint8_t Go_Flag;		/*前进标志位*/
extern uint8_t Back_Flag;	/*后退标志位*/
extern uint8_t wheel_L_Flag, wheel_R_Flag;



/*电机控制函数*/
void Motor_Control(int L_Spend,int R_Spend);
void Car_Go(uint8_t speed, uint16_t temp);
void Car_Back(uint8_t speed, uint16_t temp);
void Car_Left(uint8_t speed, int16_t Angle);
void Car_Right(uint8_t speed, int16_t Angle);



/*电机初始化函数*/
void Motor_Init(void);

/*路况刷新*/
void Go_and_Back_Check(void);
void TurnAngle_Check(void);





#endif /* DRIVE_MOTOR_H_ */
