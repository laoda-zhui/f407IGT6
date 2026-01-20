#ifndef COMMAND_COMMAND_H_
#define COMMAND_COMMAND_H_



#include "main.h"
#include "can_Tx.h"
#include "can_cmd.h"
#include "Ultrasonic.h"
#include "infrared.h"

/*从车*/
void Command_SlaveCarStart(void);

/*道闸*/
void Command_OpenGate(void);
void Command_CloseGate(void);
void Command_GetGateStates(void);
void Command_SetGateTop(uint8_t First, uint8_t Second, uint8_t Third);
void Command_SetGateLast(uint8_t First, uint8_t Second, uint8_t Third);


/*LED计时显示*/
void Command_StartTim(void);
void Command_EndTim(void);
void Command_ClearTim(void);
void Command_LEDShowDis(void);
void Command_LEDShowUp(uint8_t First, uint8_t Second, uint8_t Third);
void Command_LEDShowDown(uint8_t First, uint8_t Second, uint8_t Third);



/*智能交通信号灯*/
void Command_TrafficAInMode(void);
void Command_TrafficBInMode(void);
void Command_TrafficCInMode(void);
void Command_TrafficDInMode(void);
void Command_TrafficASend(uint8_t Result);
void Command_TrafficBSend(uint8_t Result);
void Command_TrafficCSend(uint8_t Result);
void Command_TrafficDSend(uint8_t Result);



/*TFT智能显示*/
void Command_TFTAPageUp(void);
void Command_TFTAPageDown(void);
void Command_TFTBPageUp(void);
void Command_TFTBPageDown(void);


/*Android*/
void Command_AndroidTraffic(void);



/*立体车库*/
void Command_CarPortA(uint8_t Floor);
void Command_GetPortAFloor(void);
void Command_GetPortAInfr(void);
void Command_GetPortBFloor(void);
void Command_GetPortBInfr(void);
void Command_CarPortB(uint8_t Floor);



/*智能路灯*/
void Command_light(uint8_t Gear);














#endif /* COMMAND_COMMAND_H_ */
