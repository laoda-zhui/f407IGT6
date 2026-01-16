#ifndef TASK_BOARD_VOICE_VOICE_H_
#define TASK_BOARD_VOICE_VOICE_H_


#include "main.h"
#include "usart.h"
#include "can_Tx.h"
#include "string.h"


uint8_t Voice_ASR(void);


void Voice_Init(void);

/*发送命令*/
void Voice_SendCommand(uint8_t Command);



#endif /* TASK_BOARD_VOICE_VOICE_H_ */
