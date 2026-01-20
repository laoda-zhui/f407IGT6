#include "can_Tx.h"

/*
 * Wifi zigbee	display 采用80字节环形缓冲区 读写同时进行
 * 剩下采用命令采用 写入即发(buf=8字节)
 * */


/**************************************************接收初始化***************************************************************/
#define	Can_TxMaxBuf	80	/*发送缓冲区最大空间 wifi zigbee display*/
#define Can_CmdMaxBuf	10	/*剩下命令缓冲区最大空间*/

/*创建发送CAN结构体buf缓冲区数组*/
uint8_t Can_WifiBuf[Can_TxMaxBuf] = {0};
uint8_t Can_ZigbeeBuf[Can_TxMaxBuf] = {0};
uint8_t Can_DisplayBuf[Can_TxMaxBuf] = {0};

uint8_t Can_MotorBuf[Can_CmdMaxBuf] = {0};
uint8_t Can_CNTBuf[Can_CmdMaxBuf] = {0};
uint8_t Can_NVBuf[Can_CmdMaxBuf] = {0};
uint8_t Can_PowerBuf[Can_CmdMaxBuf] = {0};
uint8_t Can_T0Buf[Can_CmdMaxBuf] = {0};
uint8_t Can_T1Buf[Can_CmdMaxBuf] = {0};
uint8_t Can_T2Buf[Can_CmdMaxBuf] = {0};


/*创建发送CAN结构体数组*/
CanP_Cmd_Struct CanP_Cmd_SBuf[10]={
		/*       StdId            				    ExtId		 	 	Data			 索引   名称	 */
		{.sid =CAN_SID_HL(ID_WIFI,0), 		 .eid = 0x00000000,	.Data = Can_WifiBuf}   ,/*0   WiFi	 */
		{.sid =CAN_SID_HL(ID_ZIGBEE,0), 	 .eid = 0x00000000,	.Data = Can_ZigbeeBuf} ,/*1   Zigbee */
		{.sid =CAN_SID_HL(ID_DISP,0), 		 .eid = 0x00000000,	.Data = Can_DisplayBuf},/*2   Display*/
		{.sid =CAN_SID_HL(ID_MOTOR,0), 		 .eid = 0x00000000,	.Data = Can_MotorBuf}  ,/*3   Motor	 */
		{.sid =CAN_SID_HL(ID_HOST,0), 		 .eid = 0x00000000,	.Data = Can_CNTBuf}	   ,/*4   CNT	 */
		{.sid =CAN_SID_HL(ID_NAVIG,ID_NAVIG),.eid = 0x00000000,	.Data = Can_NVBuf}	   ,/*5   NV	 */
		{.sid =CAN_SID_HL(ID_HOST,0), 		 .eid = 0x00000000,	.Data = Can_PowerBuf}  ,/*6   Power	 */
		{.sid =CAN_SID_HL(ID_TRACK,ID_HOST), .eid = 0x00000000,	.Data = Can_T0Buf}	   ,/*7   T0	 */
		{.sid =CAN_SID_HL(ID_TRACK,ID_HOST), .eid = 0x00000000,	.Data = Can_T1Buf}	   ,/*8   T1	 */
		{.sid =CAN_SID_HL(ID_TRACK,ID_HOST), .eid = 0x00000000,	.Data = Can_T2Buf}	   ,/*9   T2	 */
};


/*创建所有CAN命令发送结构体*/
CAN_TxHeaderTypeDef Can_Cmds[10];



/**************************************************************************
函数功能：CAN-结构体初始化
入口参数：无
返回  值：无
**************************************************************************/
void Can_CmdStruct_Init(void)
{
	for(uint8_t i=0;i < (sizeof(CanP_Cmd_SBuf)/sizeof(CanP_Cmd_Struct));i++)
	{
		CanP_Cmd_SBuf[i].Flag = 0;					/*清空cmd结构体的标志位*/
		CanP_Cmd_SBuf[i].rp = Can_TxMaxBuf-1;					/*读索引复位*/
		CanP_Cmd_SBuf[i].wp = 0;					/*写索引复位*/

		Can_Cmds[i].StdId = CanP_Cmd_SBuf[i].sid;	/*标准id*/
		Can_Cmds[i].ExtId = CanP_Cmd_SBuf[i].eid;	/*扩展id*/
		Can_Cmds[i].IDE = CAN_ID_STD;				/*标准格式*/
		Can_Cmds[i].RTR = CAN_RTR_DATA;				/*数据帧*/
	}
}

/***************************************************以上为初始化************************************************************/





/**************************************************缓冲区操作***************************************************************/

/**************************************************************************
函数功能：CAN-写入缓冲区
入口参数：CanBuf:对应缓冲区结构体  Data:发送指令数组 len:写入字节大小(别超过Can_TxMaxBuf最大值)
返回  值：无
**************************************************************************/
void Can_BufWrite(CanP_Cmd_Struct *CanBuf, uint8_t *Data, uint8_t len)
{
	if(CanBuf == _NULL){return;}
	if(Data == NULL){return;}

    if(CanBuf->wp >= Can_TxMaxBuf){CanBuf->wp = 0;}


   // 检查是否需要回绕
	if (CanBuf->wp + len > Can_TxMaxBuf)
	{
		/*回绕处理*/
		uint16_t first_part = Can_TxMaxBuf - CanBuf->wp;
		uint16_t second_part = len - first_part;

		memcpy(&CanBuf->Data[CanBuf->wp], &Data[0], first_part);
		memcpy(&CanBuf->Data[0], &Data[first_part], second_part);
		CanBuf->wp = second_part;
	}
	else
	{
		memcpy(&CanBuf->Data[CanBuf->wp], Data, len);
		CanBuf->wp += len;
	}

}

/**************************************************************************
函数功能：CAN-检查是否能读取缓冲区
入口参数：CanBuf:对应缓冲区结构体
返回  值：0-不可读 1-可读
**************************************************************************/
uint8_t Can_CheckReadEn(CanP_Cmd_Struct *p)
{
	if(p == _NULL){return 0;}

	uint8_t EN = 0;

	if(p->rp == Can_TxMaxBuf-1)
	{
		if(p->wp != 0)
			EN = 1;
	}
	else if(p->wp != (p->rp+1))
		EN = 1;

	return EN;
}



/**************************************************************************
函数功能：CAN-读取指定字节的缓冲区
入口参数：CanBuf:对应缓冲区结构体
返回  值：RT:返回为真正可读取的字节数
**************************************************************************/
uint8_t Can_BufRead(CanP_Cmd_Struct *CanBuf,uint8_t *Data)
{
	uint8_t Rt = 0;

	// 循环最多8次（固定读8字节）
	for (uint8_t i = 0; i < 8; i++)
	{
		// 检查是否还有数据（使用您提供的检查函数）
		if (Can_CheckReadEn(CanBuf) == 0)
		{
			break;  // 无数据可读，退出循环
		}

		if(++CanBuf->rp >= Can_TxMaxBuf)
		{
			CanBuf->rp = 0;
		}
		Data[i] = CanBuf->Data[CanBuf->rp];

		Rt++;  // 计数
	}

	return Rt;  // 返回实际读取字节数

}

/***************************************************以上为缓冲区操作************************************************************/




/******************************************功能函数************************************************/

/*0.wifi*/

/**************************************************************************
函数功能：CAN-发送wifi数据
入口参数：Data:发送指令数组 len:指令长度
返回  值：无
**************************************************************************/
void CAN_TxtoWifi(uint8_t *Data, uint8_t len)
{
	if(len>Can_TxMaxBuf){return;}
	if(Data == NULL){return;}

	Can_BufWrite(&CanP_Cmd_SBuf[0], Data, len);

}



/*1.Zigbee*/

/**************************************************************************
函数功能：CAN-发送Zigbee数据
入口参数：Data:发送指令数组 len:指令长度
返回  值：无
**************************************************************************/
void CAN_TxtoZigbee(uint8_t *Data, uint8_t len)
{
	if(len>Can_TxMaxBuf){return;}
	if(Data == NULL){return;}

	Can_BufWrite(&CanP_Cmd_SBuf[1], Data, len);

}



/*2.Display*/

/**************************************************************************
函数功能：CAN-发送Display数据
入口参数：Data:发送指令数组 len:指令长度
返回  值：无
**************************************************************************/
void CAN_TxtoDisplay(char *Data, uint8_t len)
{
	if(len>Can_TxMaxBuf){return;}
	if(Data == NULL){return;}

	Can_BufWrite(&CanP_Cmd_SBuf[2], (uint8_t *)Data, len);

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





/***********************************刷新判断是否有数据*****************************************************/

static uint8_t retry_data_zigbee[8],retry_data_Wifi[8],retry_data_Dis[8];  // 重发数据缓冲区
static uint8_t retry_size_zigbee = 0, retry_size_Wifi = 0, retry_size_Dis = 0; // 重发数据大小
/**************************************************************************
函数功能：CAN-检测缓冲区数据并上传(if的顺序控制优先级)s
入口参数：无
返回  值：无
**************************************************************************/
void CAN_TxLoop(void)
{
	uint8_t Status,Txsizeof1,Txsizeof2,Txsizeof3;
	uint8_t TxDataBuf_Zigbee[8],TxDataBuf_Wifi[8],TxDataBuf_Dis[8];



//	/***********重新发送区***********/
//	if(retry_size_zigbee)
//	{
//		Can_Cmds[1].DLC = retry_size_zigbee;
//		Status = MyCAN_Transmit(&Can_Cmds[1], retry_data_zigbee);
//
//		if(Status == HAL_OK)
//		{
//			retry_size_zigbee = 0;
//			memset(retry_data_zigbee,0,8);
//		}
//	}
//	if(retry_size_Wifi)
//	{
//		Can_Cmds[0].DLC = retry_size_Wifi;
//		Status = MyCAN_Transmit(&Can_Cmds[0], retry_data_Wifi);
//
//		if(Status == HAL_OK)
//		{
//			retry_size_Wifi = 0;
//			memset(retry_data_Wifi,0,8);
//		}
//	}
//	if(retry_size_Dis)
//	{
//		Can_Cmds[2].DLC = retry_size_Dis;
//		Status = MyCAN_Transmit(&Can_Cmds[2], retry_data_Dis);
//
//		if(Status == HAL_OK)
//		{
//			retry_size_Dis = 0;
//			memset(retry_data_Dis,0,8);
//		}
//	}



	/*************正常发送区***********/
	Txsizeof1 = Can_BufRead(&CanP_Cmd_SBuf[1], TxDataBuf_Zigbee);
	if(Txsizeof1) /*Zigbee 优先级:0*/
	{
		Can_Cmds[1].DLC = Txsizeof1;
		Status = MyCAN_Transmit(&Can_Cmds[1], TxDataBuf_Zigbee);

		if(Status != HAL_OK)/*发送失败*/
		{
			retry_size_zigbee = Txsizeof1;
			memcpy(retry_data_zigbee, TxDataBuf_Zigbee, Txsizeof1);
		}
	}

	Txsizeof2 = Can_BufRead(&CanP_Cmd_SBuf[0], TxDataBuf_Wifi);
	if(Txsizeof2) /*Wifi 优先级:1*/
	{
		Can_Cmds[0].DLC = Txsizeof2;
		Status = MyCAN_Transmit(&Can_Cmds[0], TxDataBuf_Wifi);

		if(Status != HAL_OK)/*发送失败*/
		{
			retry_size_Wifi = Txsizeof2;
			memcpy(retry_data_Wifi, TxDataBuf_Wifi, Txsizeof2);
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
			CanP_Cmd_SBuf[5].Flag = 0;
		}

	}


	if(CanP_Cmd_SBuf[4].Flag == 1) /*CNT 优先级:7*/
	{
		Status = MyCAN_Transmit(&Can_Cmds[4], CanP_Cmd_SBuf[4].Data);

		if(Status == HAL_OK)
		{
			memset(CanP_Cmd_SBuf[4].Data, 0, 8);
			CanP_Cmd_SBuf[4].Flag = 0;
		}
	}

	Txsizeof3 = Can_BufRead(&CanP_Cmd_SBuf[2], TxDataBuf_Dis);
	if(Txsizeof3) /*显示 优先级:8*/
	{
		Can_Cmds[2].DLC = Txsizeof3;

		Status = MyCAN_Transmit(&Can_Cmds[2], TxDataBuf_Dis);

		if(Status != HAL_OK)/*发送失败*/
		{
			retry_size_Dis = Txsizeof3;
			memcpy(retry_data_Dis, TxDataBuf_Dis, Txsizeof3);
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








