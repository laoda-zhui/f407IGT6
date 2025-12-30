#include "can_Tx.h"



/**************************************************接收FIFO初始化***************************************************************/
/*定义发送CAN命令结构体*/
typedef struct _CanP_Cmd_Struct
{
	uint8_t l;	 /*数据长度*/
	uint8_t d[8];/*发送数据数组*/
	uint32_t sid;/*标准Id*/
	uint32_t eid;/*扩展Id*/
}CanP_Cmd_Struct;

/*创建发送CAN结构体数组*/
CanP_Cmd_Struct CanP_Cmd_SBuf[CANP_CMD_ID_SIZE];

/**************************************************************************
函数功能：CAN发送结构体初始化 - 将结构体有效长度l值0
入口参数：无
返回  值：无
**************************************************************************/
void CanP_Cmd_Init(void)
{
	for(uint8_t i = 0;i < CANP_CMD_ID_SIZE;i++)
		CanP_Cmd_SBuf[i].l = 0;
}
