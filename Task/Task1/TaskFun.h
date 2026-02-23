#ifndef TASK1_TASKFUN_H_
#define TASK1_TASKFUN_H_


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
#include "TaskSlove.h"

extern float SR_Dis;

/*通用模版*/
void SendTrafficRed(void);
void GetVoicNum(void);	//开启语音识别并且获得语音编号
void SendToSlaveLight(void);

void SendVoiceNum(void);

void SendCPortb(void);

void SendTrafficBRed(void);
void SendTrafficARed(void);

void Command_SaveDis(void);//保存一次超声波的DIS距离
void showDis3D(void); //立体显示标志物显示指定距离

/*24样题2*/
void Ti24_2ReadRF1(void);
void Ti24_2ReadRF2(void);
void Ti24_2SloveRF1(void);
void Ti24_2SloveRF2(void);
void Ti24_2SendGetLight(void);

/*24样题3*/
void Ti24_3ReadRF1(void);
void Ti24_3SloveRF1(void);
void Ti24_3ReadRF2(void);
void Ti24_3SloveRF2(void);
void Ti24_3SendCPortb(void);
void Ti24_3SendGetLight(void);



/*24样题4*/
void Ti24_4ReadRF1(void);
void Ti24_4SloveRF1(void);
void Ti24_4SendCPortb(void);
void Ti24_4SendGetLight(void);



/*24样题6*/
void Ti24_6SendGetLight(void);
void Ti24_6SloveRF2(void);
void Ti24_6ReadRF2(void);
void Ti24_6SloveRF1(void);
void Ti24_6ReadRF1(void);
void Ti24_6SendRFID(void);

/*24样题7*/
void Ti24_7SendCPortb(void);
void Ti24_7SendCPortb2(void);
void Ti24_7ReadRF1(void);
void Ti24_7SloveRF1(void);

/*24样题8*/
void Ti24_8ReadRF1(void);
void Ti24_8SloveRF1(void);
void Ti24_8ReadRF2(void);
void Ti24_8SloveRF2(void);
void Ti24_8SendWife(void);
void Ti24_8SendGetLight(void);

#endif /* TASK1_TASKFUN_H_ */
