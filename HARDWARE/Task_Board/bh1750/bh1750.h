#ifndef TASK_BOARD_BH1750_BH1750_H_
#define TASK_BOARD_BH1750_BH1750_H_


#include "main.h"
#include "i2c.h"


uint16_t BH1750_GetLux(void);
void BH1750_Init(void);

extern uint8_t BH1750_InitFlag;


#endif /* TASK_BOARD_BH1750_BH1750_H_ */
