#ifndef COMMAND_COMMAND_H_
#define COMMAND_COMMAND_H_



#include "main.h"
#include "can_Tx.h"
#include "can_cmd.h"
#include "Ultrasonic.h"
#include "infrared.h"
#include "Delay.h"
#include "bh1750.h"
#include "can_RxSolve.h"


/*从车*/
void Command_SlaveCarStart(void);
void Command_SlaveCarSTep(void);
void Command_SlaveCarLight(uint8_t LightInit);


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
void Command_TrafficASend(void);
void Command_TrafficBSend(void);
void Command_TrafficCSend(void);
void Command_TrafficDSend(void);



/*TFT智能显示*/
void Command_TFTAPageUp(void);
void Command_TFTAPageDown(void);
void Command_TFTAShowHex(uint8_t First, uint8_t Second, uint8_t Third);
void Command_TFTBPageUp(void);
void Command_TFTBPageDown(void);
void Command_TFTBShowHex(uint8_t First, uint8_t Second, uint8_t Third);
void Command_TFTCPageUp(void);
void Command_TFTCPageDown(void);
void Command_TFTCShowHex(uint8_t First, uint8_t Second, uint8_t Third);


/*Android*/
void Command_AndroidTraffic(void);
void Command_Androidshape(void);
void Command_AndroidColor(void);
void Command_AndroidQR(void);



/*立体车库*/
void Command_CarPortA(uint8_t Floor);
void Command_GetPortAFloor(void);
void Command_GetPortAInfr(void);
void Command_GetPortBFloor(void);
void Command_GetPortBInfr(void);
void Command_CarPortB(uint8_t Floor);



/*智能路灯*/
uint8_t Command_LightAuto(uint8_t Loaction);


/*公交站*/
void Command_BusReportFixed(uint8_t Number);
void Command_BusReportRandom(void);
void Command_BusSetDate(uint8_t Year, uint8_t Month, uint8_t Day);
void Command_BusCheckDate(void);
void Command_BusSetTime(uint8_t Hour, uint8_t Minutes, uint8_t Seconds);
void Command_BusCheakTime(void);
void Command_BusSetTem(uint8_t Weather, uint8_t Temperature);
void Command_BusCheckTem(void);


/*无线充电站*/
void Command_WireCharStart(void);
void Command_WireCharEnd(void);


/*ETC闸门*/
void Command_ETCUp(void);
void Command_ETCDown(void);



/*自动评分系统*/
void Command_Autosystem(uint8_t Number);












#endif /* COMMAND_COMMAND_H_ */
