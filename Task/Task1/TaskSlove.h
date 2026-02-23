#ifndef TASK1_TASKSLOVE_H_
#define TASK1_TASKSLOVE_H_


#include "main.h"
#include "main.h"
#include "Motor.h"
#include "can_Tx.h"
#include "RC522.h"
#include "Ultrasonic.h"
#include "bh1750.h"
#include "infrared.h"
#include "Delay.h"
#include "can_cmd.h"
#include "Photoresistance.h"
#include "Command.h"
#include "Task_Beep.h"
#include "Voice.h"
#include <ctype.h>
#include "string.h"
#include "Lib.h"


/*通用解题模板*/
void SloveTemplate1(void);
void SloveTemplate2(void);
void SloveTemplate3(void);
void SloveTemplate4(char *pResult);
void SloveTemplate5(char *pInput,char *pResult);
void SloveTemplate6_Sort(char *pInput, char *pResult);

extern char E01_Results[8][20];
extern  uint8_t E01_Count;

extern char E02_Results[8][20]; // 结果存放
extern uint8_t E02_Count;

extern char E03_Results[8][20]; // 结果存放
extern uint8_t E03_Count;

extern char Number_Results[32]; // 存放提取出的纯数字字符串
extern uint8_t E04_Count;

extern char Number_PaiXu[100];

#endif /* TASK1_TASKSLOVE_H_ */
