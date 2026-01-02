#include "can_Tx.h"

/**************************************************接收初始化***************************************************************/

/*创建发送CAN结构体数组*/
const CanP_Cmd_Struct CanP_Cmd_SBuf[10]={
		/*  StdId            ExtId		   索引   名称		*/
		{.sid =0x280 , .eid = 0x00000000},/*0   WiFi	*/
		{.sid =0x2A0 , .eid = 0x00000000},/*1   Zigbee	*/
		{.sid =0x3C0 , .eid = 0x00000000},/*2   Display	*/
		{.sid =0x20  , .eid = 0x00000000},/*3   Motor	*/
		{.sid =0x1E0 , .eid = 0x00000000},/*4   CNT		*/
		{.sid =0x108 , .eid = 0x00000000},/*5   NV		*/
		{.sid =0x1E0 , .eid = 0x00000000},/*6   Power	*/
		{.sid =0xEF  , .eid = 0x00000000},/*7   T0		*/
		{.sid =0xEF  , .eid = 0x00000000},/*8   T1		*/
		{.sid =0xEF  , .eid = 0x00000000},/*9   T2		*/
};


/*创建所有CAN命令发送结构体*/
CAN_TxHeaderTypeDef Can_Cmds[10];

void Can_CmdStruct_Init(void)
{
	for(uint8_t i=0;i < (sizeof(CanP_Cmd_SBuf)/sizeof(CanP_Cmd_Struct));i++)
	{
		Can_Cmds[i].StdId = CanP_Cmd_SBuf[i].sid;	/*标准id*/
		Can_Cmds[i].ExtId = CanP_Cmd_SBuf[i].eid;	/*扩展id*/
		Can_Cmds[i].IDE = CAN_ID_STD;				/*标准格式*/
		Can_Cmds[i].RTR = CAN_RTR_DATA;				/*数据帧*/
	}
}

/***************************************************以上为初始化************************************************************/




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

	Can_Cmds[0].DLC = len;
	MyCAN_Transmit(&Can_Cmds[0], Data);
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

	Can_Cmds[1].DLC = len;
	MyCAN_Transmit(&Can_Cmds[1], Data);
}



/*2.Display*/

/**************************************************************************
函数功能：CAN-发送Display数据
入口参数：Data:发送指令数组 len:指令长度
返回  值：无
**************************************************************************/
void CAN_TxtoDisplay(uint8_t *Data, uint8_t len)
{
	if(len>8){return;}
	if(Data == NULL){return;}

	Can_Cmds[2].DLC = len;
	MyCAN_Transmit(&Can_Cmds[2], Data);
}


/*3.Motor*/

/**************************************************************************
函数功能：CAN-发送电机转速
入口参数：x1 左侧电机速度  x2 右侧电机转速
返回  值：无
**************************************************************************/
void CAN_TxtoMotor(int x1, int x2)
{
	uint8_t txbuf[4];
	txbuf[0] = (x1>>8)&&0xff;	/*不确定要不要移位*/
	txbuf[1] = x1&&0xff;
	txbuf[2] = (x2>>8)&&0xff;;
	txbuf[3] = x2&&0xff;

	Can_Cmds[3].DLC = 4;
	MyCAN_Transmit(&Can_Cmds[3], txbuf);
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

	Can_Cmds[4].DLC = 3;

	MyCAN_Transmit(&Can_Cmds[4], txbuf);
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

	Can_Cmds[5].DLC = 2;

	MyCAN_Transmit(&Can_Cmds[5], txbuf);
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

	Can_Cmds[6].DLC = 3;

	MyCAN_Transmit(&Can_Cmds[6], txbuf);
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

	Can_Cmds[7].DLC = 3;
	MyCAN_Transmit(&Can_Cmds[7], txbuf);
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

	Can_Cmds[8].DLC = 4;
	MyCAN_Transmit(&Can_Cmds[8], txbuf);
}


/*9.T2*/
/**************************************************************************
函数功能：CAN-设置循迹板上传数据时间
入口参数：TIME:上传时间间隔
返回  值：无
**************************************************************************/
void CAN_TxtoT2(uint8_t time)  // 设置循迹数据上传时间间隔
{
	uint8_t txbuf[2];
	txbuf[0] = 0X02;  //命令关键字
	txbuf[1] = time;

	Can_Cmds[9].DLC = 2;
	MyCAN_Transmit(&Can_Cmds[9], txbuf);
}


/***************************************************以上为功能函数************************************************************/






