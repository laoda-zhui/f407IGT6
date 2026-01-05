#include "can_Tx.h"

/**************************************************接收初始化***************************************************************/



/*创建发送CAN结构体数组*/
CanP_Cmd_Struct CanP_Cmd_SBuf[10]={
		/*       StdId            				 ExtId		 	 索引   名称	 */
		{.sid =CAN_SID_HL(ID_WIFI,0), 		 .eid = 0x00000000},/*0   WiFi	 */
		{.sid =CAN_SID_HL(ID_ZIGBEE,0), 	 .eid = 0x00000000},/*1   Zigbee */
		{.sid =CAN_SID_HL(ID_DISP,0), 		 .eid = 0x00000000},/*2   Display*/
		{.sid =CAN_SID_HL(ID_MOTOR,0), 		 .eid = 0x00000000},/*3   Motor	 */
		{.sid =CAN_SID_HL(ID_HOST,0), 		 .eid = 0x00000000},/*4   CNT	 */
		{.sid =CAN_SID_HL(ID_NAVIG,ID_NAVIG),.eid = 0x00000000},/*5   NV	 */
		{.sid =CAN_SID_HL(ID_HOST,0), 		 .eid = 0x00000000},/*6   Power	 */
		{.sid =CAN_SID_HL(ID_TRACK,ID_HOST), .eid = 0x00000000},/*7   T0	 */
		{.sid =CAN_SID_HL(ID_TRACK,ID_HOST), .eid = 0x00000000},/*8   T1	 */
		{.sid =CAN_SID_HL(ID_TRACK,ID_HOST), .eid = 0x00000000},/*9   T2	 */
};


/*创建所有CAN命令发送结构体*/
CAN_TxHeaderTypeDef Can_Cmds[10];

void Can_CmdStruct_Init(void)
{
	for(uint8_t i=0;i < (sizeof(CanP_Cmd_SBuf)/sizeof(CanP_Cmd_Struct));i++)
	{
		memset(CanP_Cmd_SBuf[i].Data, 0, 8);		/*清空cmd结构体的缓冲数组数据*/
		CanP_Cmd_SBuf[i].Flag = 0;					/*清空cmd结构体的标志位*/
		CanP_Cmd_SBuf[i].rp = 0;					/*读索引复位*/
		CanP_Cmd_SBuf[i].wp = 0;					/*写索引复位*/

		Can_Cmds[i].StdId = CanP_Cmd_SBuf[i].sid;	/*标准id*/
		Can_Cmds[i].ExtId = CanP_Cmd_SBuf[i].eid;	/*扩展id*/
		Can_Cmds[i].IDE = CAN_ID_STD;				/*标准格式*/
		Can_Cmds[i].RTR = CAN_RTR_DATA;				/*数据帧*/
	}
}

/***************************************************以上为初始化************************************************************/





/**************************************************缓冲区操作***************************************************************/
void Can_BufWrite(CanP_Cmd_Struct *CanBuf, uint8_t *Data)
{
	if(CanBuf == NULL){return;}
	if(Data == NULL){return;}

   // 检查是否需要回绕
	if (fifo->w_idx + CAN_FRAME_SIZE > fifo->size) {
	uint16_t first_part = fifo->size - fifo->w_idx;
	uint16_t second_part = CAN_FRAME_SIZE - first_part;

	memcpy(&fifo->buf[fifo->w_idx], data, first_part);
	memcpy(fifo->buf, &data[first_part], second_part);
	fifo->w_idx = second_part;
	} else {
		memcpy(&fifo->buf[fifo->w_idx], data, CAN_FRAME_SIZE);
		fifo->w_idx += CAN_FRAME_SIZE;
	}

	memcpy(&CanBuf->Data[0], Data, 8);



}








/******************************************功能函数************************************************/

/*0.wifi*/

/**************************************************************************
函数功能：CAN-发送wifi数据
入口参数：Data:发送指令数组 len:指令长度
返回  值：无
**************************************************************************/
void CAN_TxtoWifi(uint8_t *Data, uint8_t len)
{
	if(len>8){return;}
	if(Data == NULL){return;}

	memcpy(CanP_Cmd_SBuf[0].Data, Data, len);
	Can_Cmds[0].DLC = len;

	CanP_Cmd_SBuf[0].Flag = 1;
}



/*1.Zigbee*/

/**************************************************************************
函数功能：CAN-发送Zigbee数据
入口参数：Data:发送指令数组 len:指令长度
返回  值：无
**************************************************************************/
void CAN_TxtoZigbee(uint8_t *Data, uint8_t len)
{
	if(len>8){return;}
	if(Data == NULL){return;}

	memcpy(CanP_Cmd_SBuf[1].Data, Data, len);
	Can_Cmds[1].DLC = len;

	CanP_Cmd_SBuf[1].Flag = 1;
}



/*2.Display*/

/**************************************************************************
函数功能：CAN-发送Display数据
入口参数：Data:发送指令数组 len:指令长度
返回  值：无
**************************************************************************/
void CAN_TxtoDisplay(char *Data, uint8_t len)
{
	if(len>8){return;}
	if(Data == NULL){return;}

	memcpy(CanP_Cmd_SBuf[2].Data, Data, len);
	Can_Cmds[2].DLC = len;

	CanP_Cmd_SBuf[2].Flag = 1;
}


/*3.Motor*/

/**************************************************************************
函数功能：CAN-控制电机转速
入口参数：x1 左侧2个电机速度  x2 右侧2个电机转速
返回  值：无
**************************************************************************/
void CAN_TxtoMotor(int x1, int x2)
{
	uint8_t txbuf[4]={0};
	txbuf[0] = x1;
	txbuf[1] = x1;
	txbuf[2] = x2;
	txbuf[3] = x2;

	memcpy(CanP_Cmd_SBuf[3].Data, txbuf, 4);
	Can_Cmds[3].DLC = 4;

	CanP_Cmd_SBuf[3].Flag = 1;
}


/*4.CNT(未使用-移植百科荣创功能函数)*/

/**************************************************************************
函数功能：CAN-发送不知道(未使用-移植百科荣创功能函数)
入口参数：不知道
返回  值：无
**************************************************************************/
void CAN_TxtoCNT(void)
{
	uint8_t txbuf[5];
	txbuf[0] =0x02;
	//U16ToU8(txbuf+1,CodedDisc_Get(0));
	//U16ToU8(txbuf+3,CodedDisc_Get(1));

	memcpy(CanP_Cmd_SBuf[4].Data, txbuf, 3);
	Can_Cmds[4].DLC = 3;

	CanP_Cmd_SBuf[4].Flag = 1;
}


/*5.NV(未使用-移植百科荣创功能函数)*/

/**************************************************************************
函数功能：CAN-发送不知道(未使用-移植百科荣创功能函数)
入口参数：无
返回  值：无
**************************************************************************/
void CAN_TxtoNV(uint16_t c)
{
	uint8_t txbuf[8];
	txbuf[0] = (c>>8)&0x0ff;
	txbuf[1] = (c)&0x0ff;

	memcpy(CanP_Cmd_SBuf[5].Data, txbuf, 2);
	Can_Cmds[5].DLC = 2;

	CanP_Cmd_SBuf[5].Flag = 1;
}


/*6.Power*/

/**************************************************************************
函数功能：CAN-上传电量信息
入口参数：x1 左侧电量  x2右侧电量
返回  值：无
**************************************************************************/
void CAN_TxtoPower(uint8_t x1, uint8_t x2)
{
	uint8_t txbuf[3];
	txbuf[0] = 0x03;
	txbuf[1] = x1;
	txbuf[2] = x2;

	memcpy(CanP_Cmd_SBuf[6].Data, txbuf, 3);
	Can_Cmds[6].DLC = 3;

	CanP_Cmd_SBuf[6].Flag = 1;
}


/*7.T0 (暂未使用)*/

/**************************************************************************
函数功能：设置循迹板发射功率 (暂未使用)
入口参数：power 待设置循迹板功率
返回  值：无
**************************************************************************/
void CAN_TxtoT0(uint16_t power) // 设置寻迹板发射功率
{
	uint8_t txbuf[3];

	txbuf[0] = 0X03;  //命令关键字
	txbuf[1] = (power>> 8)&0xff;
	txbuf[2] = (power)&0xff;

	memcpy(CanP_Cmd_SBuf[7].Data, txbuf, 3);
	Can_Cmds[7].DLC = 3;

	CanP_Cmd_SBuf[7].Flag = 1;
}


/*8.T1 (暂未使用)*/

/**************************************************************************
函数功能：发送设置循迹板参数 (暂未使用)
入口参数：addr:?  ydata:?
返回  值：无
**************************************************************************/
void CAN_TxtoT1(uint8_t addr, uint16_t ydata)
{
	uint8_t txbuf[4];

	txbuf[0] = 0X04;  //命令关键字
	txbuf[1] = addr;
	txbuf[2] = (ydata>> 8)&0xff;
	txbuf[3] = (ydata)&0xff;

	memcpy(CanP_Cmd_SBuf[8].Data, txbuf, 4);
	Can_Cmds[8].DLC = 4;

	CanP_Cmd_SBuf[8].Flag = 1;
}


/*9.T2(暂未使用)*/
/**************************************************************************
函数功能：CAN-设置循迹板上传数据时间(暂未使用)
入口参数：TIME:上传时间间隔
返回  值：无
**************************************************************************/
void CAN_TxtoT2(uint8_t time)  // 设置循迹数据上传时间间隔
{
	uint8_t txbuf[2];
	txbuf[0] = 0X02;  //命令关键字
	txbuf[1] = time;

	memcpy(CanP_Cmd_SBuf[9].Data, txbuf, 2);
	Can_Cmds[9].DLC = 2;

	CanP_Cmd_SBuf[9].Flag = 1;
}


/***************************************************以上为功能函数************************************************************/



/**************************************************************************
函数功能：CAN-检测缓冲区数据是否为空
入口参数：data:缓冲区数组
返回  值：0-空 1-存在数据
**************************************************************************/
uint8_t CAN_TxDataCheck(uint8_t *Data)
{

	return(!!(Data[0] | Data[1] | Data[2] | Data[3] | Data[4] | Data[5] | Data[6] | Data[7]));
}


/**************************************************************************
函数功能：CAN-检测缓冲区数据并上传(if的顺序控制优先级)
入口参数：无
返回  值：无
**************************************************************************/
void CAN_TxLoop(void)
{
	uint8_t Status;
	if(CanP_Cmd_SBuf[1].Flag == 1) /*Zigbee 优先级:0*/
	{
		Status = MyCAN_Transmit(&Can_Cmds[1], CanP_Cmd_SBuf[1].Data);
		if(Status == HAL_OK)
		{
			memset(CanP_Cmd_SBuf[1].Data, 0, 8);
			CanP_Cmd_SBuf[1].Flag = 0;
		}
	}


	if(CanP_Cmd_SBuf[0].Flag == 1) /*Wifi 优先级:1*/
	{
		Status = MyCAN_Transmit(&Can_Cmds[0], CanP_Cmd_SBuf[0].Data);
		if(Status == HAL_OK)
		{
			memset(CanP_Cmd_SBuf[0].Data, 0, 8);
			CanP_Cmd_SBuf[0].Flag = 0;
		}
	}

	if(CanP_Cmd_SBuf[3].Flag == 1) /*电机 优先级:2*/
	{
		Status = MyCAN_Transmit(&Can_Cmds[3], CanP_Cmd_SBuf[3].Data);
		if(Status == HAL_OK)
		{
			memset(CanP_Cmd_SBuf[3].Data, 0, 8);
			CanP_Cmd_SBuf[3].Flag = 0;
		}
	}

	if(CanP_Cmd_SBuf[9].Flag == 1) /*T2 优先级:3*/
	{
		Status = MyCAN_Transmit(&Can_Cmds[9], CanP_Cmd_SBuf[9].Data);
		if(Status == HAL_OK)
		{
			memset(CanP_Cmd_SBuf[9].Data, 0, 8);
			CanP_Cmd_SBuf[9].Flag = 0;
		}
	}


	if(CanP_Cmd_SBuf[7].Flag == 1) /*T0 优先级:4*/
	{
		Status = MyCAN_Transmit(&Can_Cmds[7], CanP_Cmd_SBuf[7].Data);
		if(Status == HAL_OK)
		{
			memset(CanP_Cmd_SBuf[7].Data, 0, 8);
			CanP_Cmd_SBuf[7].Flag = 0;
		}
	}


	if(CanP_Cmd_SBuf[8].Flag == 1) /*T1 优先级:5*/
	{
		Status = MyCAN_Transmit(&Can_Cmds[8], CanP_Cmd_SBuf[8].Data);
		if(Status == HAL_OK)
		{
			memset(CanP_Cmd_SBuf[8].Data, 0, 8);
			CanP_Cmd_SBuf[8].Flag = 0;
		}
	}


	if(CanP_Cmd_SBuf[5].Flag == 1) /*NV 优先级:6*/
	{
		Status = MyCAN_Transmit(&Can_Cmds[5], CanP_Cmd_SBuf[5].Data);
		if(Status == HAL_OK)
		{
			memset(CanP_Cmd_SBuf[5].Data, 0, 8);
			CanP_Cmd_SBuf[7].Flag = 0;
		}

	}


	if(CanP_Cmd_SBuf[4].Flag == 1) /*CNT 优先级:7*/
	{
		Status = MyCAN_Transmit(&Can_Cmds[4], CanP_Cmd_SBuf[4].Data);
		if(Status == HAL_OK)
		{
			memset(CanP_Cmd_SBuf[4].Data, 0, 8);
		}
	}


	if(CanP_Cmd_SBuf[2].Flag == 1) /*显示 优先级:8*/
	{
		Status = MyCAN_Transmit(&Can_Cmds[2], CanP_Cmd_SBuf[2].Data);
		if(Status == HAL_OK)
		{
			memset(CanP_Cmd_SBuf[2].Data, 0, 8);
			CanP_Cmd_SBuf[2].Flag = 0;
		}
	}


	if(CanP_Cmd_SBuf[6].Flag == 1) /*Power 优先级:9*/
	{
		Status = MyCAN_Transmit(&Can_Cmds[6], CanP_Cmd_SBuf[6].Data);
		if(Status == HAL_OK)
		{
			memset(CanP_Cmd_SBuf[6].Data, 0, 8);
			CanP_Cmd_SBuf[6].Flag = 0;
		}
	}
}








