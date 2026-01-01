#ifndef CAN_CAN_TX_H_
#define CAN_CAN_TX_H_

#include "main.h"
#include "can.h"

/*定义发送CAN通信id结构体*/
typedef struct _CanP_Cmd_Struct
{
	uint32_t sid;/*标准Id*/
	uint32_t eid;/*扩展Id*/
}CanP_Cmd_Struct;


/*发送id结构体*/
extern const CanP_Cmd_Struct CanP_Cmd_SBuf[10];


/*全部命令结构体初始化函数*/
void Can_CmdStruct_Init(void);

/*发送函数*/
void CAN_TxtoWifi(uint8_t *Data, uint8_t len);
void CAN_TxtoZigbee(uint8_t *Data, uint8_t len);
void CAN_TxtoDisplay(uint8_t *Data, uint8_t len);
void CAN_TxtoMotor(int x1, int x2);
void CAN_TxtoCNT(void);
void CAN_TxtoNV(uint16_t c);
void CAN_TxtoPower(uint8_t x1, uint8_t x2);
void CAN_TxtoT0(uint16_t power);
void CAN_TxtoT1(uint8_t addr, uint16_t ydata);
void CAN_TxtoT2(uint8_t time);


#endif /* CAN_CAN_TX_H_ */
