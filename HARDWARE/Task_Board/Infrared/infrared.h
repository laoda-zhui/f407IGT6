#ifndef INFRARED_INFRARED_H_
#define INFRARED_INFRARED_H_


#include "main.h"
#include "Delay.h"


void Infrared_Init(void);
void Infrared_SendpData(uint8_t *pData, uint32_t len);


#endif /* INFRARED_INFRARED_H_ */
