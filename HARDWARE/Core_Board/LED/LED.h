#ifndef CORE_BOARD_LED_LED_H_
#define CORE_BOARD_LED_LED_H_

#include "main.h"

/*LED操作*/
#define 	LED1(x)			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, (GPIO_PinState)(x));
#define 	LED2(x)			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, (GPIO_PinState)(x));
#define 	LED3(x)			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, (GPIO_PinState)(x));
#define 	LED4(x)			HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, (GPIO_PinState)(x));

void LED1Toogle(uint32_t Time);
void LED2Toogle(uint32_t Time);
void LED3Toogle(uint32_t Time);
void LED4Toogle(uint32_t Time);

#endif /* CORE_BOARD_LED_LED_H_ */
