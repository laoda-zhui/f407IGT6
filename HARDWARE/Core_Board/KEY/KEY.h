#ifndef CORE_BOARD_KEY_H_
#define CORE_BOARD_KEY_H_


#include "main.h"
#include "tim.h"


void KeyNum_Tick(void);
uint8_t Key_GetNum(void);
void Key_Init(void);

#endif /* INC_KEY_H_ */
