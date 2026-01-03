#ifndef INC_PID_H_
#define INC_PID_H_


#include "main.h"


/*定义PID结构体*/
typedef struct{
	float Actual;
	float Target;
	float Error0;
	float Error1;
	float Error2;
	float ErrorInt;
    float ErrorIntMax;
    float ErrorIntMin;
	float Kp;
	float Ki;
	float Kd;
	float Out;
	float OutMax;
	float OutMin;
}PID_t;

void PIDSpeed_Update(PID_t *PID);
void PIDTurn_Update(PID_t *PID);
void PID_Update(PID_t *PID);
void PID_Clear(PID_t *p);


#endif /* INC_PID_H_ */
