#ifndef TASK_BOARD_ULTRASONIC_ULTRASONIC_H_
#define TASK_BOARD_ULTRASONIC_ULTRASONIC_H_


#include "main.h"
#include "Delay.h"
#include "TIM.h"

/*测量数据*/
extern float Distance;
extern uint16_t Dis_Buf;

/*初始化*/
void Ultrasonic_Init();

/*启动函数*/
void Ultrasonic_Start();



#endif /* TASK_BOARD_ULTRASONIC_ULTRASONIC_H_ */
