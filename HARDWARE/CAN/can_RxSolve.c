#include "can_RxSolve.h"


/*10ms wifi空闲处理数据*/
uint32_t WifiWaitTime = 10,WifiStartTime=0;

/*Wifi_Traffic数据处理开启标志位 1-开启 0-关闭*/
uint8_t Wifi_TrafficFlag=0;
/*Wifi_Camera数据处理开启标志位 1-开启 0-关闭*/
uint8_t Wifi_CameraFlag=0;

/**************************************************接收数组初始化***************************************************************/

/*对应缓冲数组 -- 大小参考can_RxSolve.h宏定义*/
uint8_t 	FifoBuf_Info[Can_RxInfoZize] 		= 	{0}; 	/*0接收显示屏*/
uint8_t 	FifoBuf_WifiRx[Can_RxFIFOZize] 		= 	{0}; 	/*1接收wifi*/
uint8_t 	FifoBuf_ZigbRx[Can_RxZigbeeZize] 	= 	{0}; 	/*2接收zigbee*/
uint8_t 	FifoBuf_Track[Can_RxTrackZize] 	  	= 	{0};	/*3接收循迹*/
uint8_t 	FifoBuf_Navig[Can_RxNavigZize] 	 	= 	{0}; 	/*4接收navig-暂时不知道是啥*/
uint8_t 	FifoBuf_HOST[Can_RxHOSTZize] 	  	= 	{0};	/*5接收主机*/
uint8_t 	FifoBuf_Anything[Can_RxAnythingZize]= 	{0};	/*6接收任何信息*/

/*缓冲区结构体*/
Can_RXFIFOBUF Can_RxFiFoBuf[7]; /*0-显示屏 1-Wifi 2-Zigbee 3-循迹 4-naving 5-主机 6-no*/


uint8_t x1,x2; /*循迹拆分数据x1:循迹板后面8个灯状况 x2：循迹板后面7个灯状况(循迹板b档情况下x1有效)*/


uint16_t Current_Angle;	/*猜测为当前航向角度*/


/*接受的数据*/
int16_t CanHost_Mp=0;	/*码盘值左*/
int16_t CanHost_Mp1=0;	/*码盘值右*/
int16_t CanHost_Mp2=0;	/*码盘值不使用*/
int16_t CanHost_Mp3=0;	/*码盘值不使用*/

float pitch=0;	/*pitch是围绕X轴旋转，也叫做俯仰角(不使用)	  如无功能扩展板则不使用*/
float yaw=0;	/*yaw是围绕Y轴旋转，也叫偏航角	 (不使用)	  如无功能扩展板则不使用*/
float roll=0;	/*roll是围绕Z轴旋转，也叫翻滚角	 (不使用)		  如无功能扩展板则不使用*/


/*安卓摄像头数据处理标志位*/
CameraFlag AndroidFlag = {0};

/*安卓数据处理标志位*/
uint8_t AndroidGoFlag = 0;

/*车库层数状态标志位*/
CarPort CarPortFlag = {0};

/*道闸状态标志位*/
GateStates GateFlag = {0};


/*报警台救援坐标*/
uint8_t  RescueLocation=0;

/*公交站回传信息*/
SmartBus BusData= {0};


/*颜色图形数据结构体*/
ColorShape CameraData = {0};

/*从车数据结构体*/
SlaveCar SlaveCarData = {0};




/*******************************************************初始化函数*****************************************************************/

/**************************************************************************
函数功能：CAN-接收缓冲区结构体初始化
入口参数：无
返回  值：无
**************************************************************************/
void CanRxBuf_Init(void)
{
	for(uint8_t i=0;i<(sizeof(Can_RxFiFoBuf)/sizeof(Can_RXFIFOBUF));i++)
	{
		Can_RxFiFoBuf[i].Flag = 0;
	}
	Can_RxFiFoBuf[1].Data = FifoBuf_WifiRx;
	Can_RxFiFoBuf[1].rp = Can_RxFIFOZize-1;		/*读索引复位*/
	Can_RxFiFoBuf[1].wp = 0;					/*写索引复位*/
	Can_RxFiFoBuf[1].Flag = 0;					/*清空cmd结构体的标志位*/

}




/**************************************************接收FIFO功能函数***************************************************************/


/**************************************************************************
函数功能：CAN接收-识别指定设备数据，将数据存储到指定数组中
入口参数：无
返回  值：无
**************************************************************************/
void CanRx_Loop(void)
{
	if(MyCAN_RxFlag == 1) /*中断接收到CAN数据标志位MyCAN_RxFlag*/
	{
		MyCAN_RxFlag = 0; /*清除中断接收到CAN数据标志位MyCAN_RxFlag*/

		switch(RxMsgArray.FilterMatchIndex)		//判断消息邮箱索引
		{
			case 2:			/*zigbee rx*/
				memcpy(FifoBuf_ZigbRx, RxData, RxMsgArray.DLC);
				Can_RxFiFoBuf[2].Flag = 1;
				break;
			case 0:			/*disp*/
				memcpy(FifoBuf_Info, RxData, RxMsgArray.DLC);
				Can_RxFiFoBuf[0].Flag = 1;
				break;
			case 1:			/*wifi rx*/
				Can_RxBufWrite(&Can_RxFiFoBuf[1], RxData, RxMsgArray.DLC);
				Can_RxFiFoBuf[1].Flag = 1;
				WifiStartTime = HAL_GetTick();
				break;
			case 3:			/*Track*/
				memcpy(FifoBuf_Track, RxData, RxMsgArray.DLC);
				Can_RxFiFoBuf[3].Flag = 1;
				break;
			case 4:			/*Navig*/
				memcpy(FifoBuf_Navig, RxData, RxMsgArray.DLC);
				Can_RxFiFoBuf[4].Flag = 1;
				break;
			case 5:			/*HOST*/
				memcpy(FifoBuf_HOST, RxData, RxMsgArray.DLC);
				Can_RxFiFoBuf[5].Flag = 1;
				break;
			case 6:			/*Anything*/
				//memcpy(FifoBuf_Anything, RxData, RxMsgArray.DLC);
				//Can_RxFiFoBuf[6].Flag = 1;
				break;
		}
	}
}





/**************************************************************************
函数功能：CAN-摄像头数据处理
入口参数：无
返回  值：无
**************************************************************************/
void Slove_AndroidData(void)
{
	if(Wifi_TrafficFlag == 0){return;}

	static uint8_t state = 0;
	uint8_t byte = 0;
	Can_RXFIFOBUF *pBuf = &Can_RxFiFoBuf[1];

	// 从环形缓冲读取一个字节
	while (Can_RxReadBit(pBuf, &byte) == 1)
	{
		switch (state)
		{
		// ----------------------------------------------------
		// 阶段 1：寻找公共包头 0x55
		// ----------------------------------------------------
		case 0:
			if (byte == 0x55) state = 1;
			break;

		// ----------------------------------------------------
		// 阶段 2：分支判断 (区分是张老师版还是商家版)
		// ----------------------------------------------------
		case 1:
			if(byte == 0x03)
			{
				// === 命中张老师版 (55 03) ===
				state = 10; // 跳转到张老师专用解析状态
			}
			else if(byte == 0x55)
			{
				// 容错：连续收到两个55，可能是重发包头
				state = 1;
			}
			else if(byte == 0x00)
			{
				state = 20; // 跳转到商家专用解析状态
			}
			break;

		// ----------------------------------------------------
		// 分支 A：张老师版解析 (55 03 [Data])
		// ----------------------------------------------------
		case 10:
			// 当前 byte 是数据位
			if (byte == 0x01) AndroidFlag = TrafficRed_Flag;
			else if (byte == 0x02) AndroidFlag = TrafficYellow_Flag;
			else if (byte == 0x03) AndroidFlag = TrafficGreen_Flag;
			state = 0; // 完成，复位
			Can_RxFiFoBuf[1].Flag = 0;
			break;
		// ----------------------------------------------------
		// 分支 B：商家版解析 (55 xx 02 [Data])
		// ----------------------------------------------------
		case 20:
			// 当前 byte 是第三字节，商家版要求必须是 0x02
			if (byte == 0x02)
			{
				state = 21; // 符合，去读下一字节数据
			}
			else if (byte == 0x55)
			{
				state = 1; // 容错
			}
			else
			{
				state = 0; // 格式错误
			}
			break;

		case 21: // 商家版数据位
			if (byte == 0x01) AndroidFlag = TrafficRed_Flag;
			else if (byte == 0x03) AndroidFlag = TrafficYellow_Flag;
			else if (byte == 0x02) AndroidFlag = TrafficGreen_Flag;
			state = 0; // 完成，复位
			Can_RxFiFoBuf[1].Flag = 0;
			break;
		default:
			state = 0;
			break;
		}
	}
}
void Slove_Camera(void)
{
	if(Wifi_CameraFlag == 0){return;}

	if(HAL_GetTick() - WifiStartTime > WifiWaitTime)
	{
	    // 定义一个足够大的临时数组 (比 FIFO Size 稍大一点安全)
	    char temp_buf[Can_RxFIFOZize + 1]={0};
	    uint16_t len = 0;
	    uint8_t byte;
	    Can_RXFIFOBUF *pBuf = &Can_RxFiFoBuf[1]; // Wifi 缓冲区

		// ---------------------------------------------------------
		// 第一步：从环形缓冲区提取数据并转为字符串
		// ---------------------------------------------------------
		// 只要有数据就读出来，拼接到 temp_buf
	    while (Can_RxReadBit(pBuf, &byte) == 1)
	    {
	        if (len < Can_RxFIFOZize)
	        {
	            temp_buf[len++] = (char)byte;
	        }
	    }
	    temp_buf[len] = '\0'; // 【关键】必须添加字符串结束符！

		if (len == 0) return; // 空数据直接返回

		// ---------------------------------------------------------
		// 第二步：使用 strstr 和 atoi 暴力解析
		// ---------------------------------------------------------
		char *p;

		// 1. 锐角 (sharp3=)
		if ((p = strstr(temp_buf, "sharp3=")) != NULL) {
			CameraData.ruijiao = atoi(p + 7); // 跳过 "sharp3=" 这7个字符
		}

		// 2. 钝角 (dun3=)
		if ((p = strstr(temp_buf, "dun3=")) != NULL) {
			CameraData.dunjiao = atoi(p + 5);
		}

		// 3. 直角 (right3=)
		if ((p = strstr(temp_buf, "right3=")) != NULL) {
			CameraData.zhijiao = atoi(p + 7);
		}

		// 4. 菱形 (lin4=)
		if ((p = strstr(temp_buf, "lin4=")) != NULL) {
			CameraData.lingxing = atoi(p + 5);
		}

		// 5. 长方形 (rectangle=)
		if ((p = strstr(temp_buf, "rectangle=")) != NULL) {
			CameraData.changfan = atoi(p + 10);
		}

		// 6. 正方形 (square=)
		if ((p = strstr(temp_buf, "square=")) != NULL) {
			CameraData.juxing = atoi(p + 7);
		}

		// 7. 五角星 (star=)
		if ((p = strstr(temp_buf, "star=")) != NULL) {
			CameraData.star = atoi(p + 5);
		}

		// 8. 圆形 (circle=)
		if ((p = strstr(temp_buf, "circle=")) != NULL) {
			CameraData.circle = atoi(p + 7);
		}

		// ----------------------------------------------------
		// 颜色解析区 (Color Analysis)
		// ----------------------------------------------------

		// 1. 红色 (red=)
		if ((p = strstr(temp_buf, "red=")) != NULL) {
			CameraData.red = atoi(p + 4); // "red=" 长度4
		}

		// 2. 绿色 (green=)
		if ((p = strstr(temp_buf, "green=")) != NULL) {
			CameraData.green = atoi(p + 6); // "green=" 长度6
		}

		// 3. 蓝色 (blue=)
		if ((p = strstr(temp_buf, "blue=")) != NULL) {
			CameraData.blue = atoi(p + 5); // "blue=" 长度5
		}

		// 4. 黄色 (yellow=)
		if ((p = strstr(temp_buf, "yellow=")) != NULL) {
			CameraData.yellow = atoi(p + 7); // "yellow=" 长度7
		}

		// 5. 青色 (cyan=)
		if ((p = strstr(temp_buf, "cyan=")) != NULL) {
			CameraData.qingse = atoi(p + 5); // "cyan=" 长度5
		}

		// 6. 橙色 (orange=)
		if ((p = strstr(temp_buf, "orange=")) != NULL) {
			CameraData.orange = atoi(p + 7); // "orange=" 长度7
		}

		// 7. 紫色 (purple=)
		if ((p = strstr(temp_buf, "purple=")) != NULL) {
			CameraData.purple = atoi(p + 7); // "purple=" 长度7
		}

		// 8. 黑色 (black=)
		if ((p = strstr(temp_buf, "black=")) != NULL) {
			CameraData.black = atoi(p + 6); // "black=" 长度6
		}



		Can_RxFiFoBuf[1].Flag = 0;

	}

}


void Slove_GoFlag(void)
{
	if(FifoBuf_WifiRx[0] == 0x55 && FifoBuf_WifiRx[1] == 0xAA)
	{
		if(FifoBuf_WifiRx[2] == 0xA0 && FifoBuf_WifiRx[6] == 0xA0)
		{
			AndroidGoFlag = 1;
		}
	}
}






/**************************************************************************
函数功能：CAN-Zigbee数据处理
入口参数：无
返回  值：无
**************************************************************************/
void Slove_ZigbeeData(void)
{

	/*车库回传层数状态*/
	if(FifoBuf_ZigbRx[0] == 0x55)
	{
		if(FifoBuf_ZigbRx[1] == 0x0D && FifoBuf_ZigbRx[2] == 0x03) /*车库A*/
		{
			if(FifoBuf_ZigbRx[3] == 0x01) /*普通层数回传*/
			{
				if(FifoBuf_ZigbRx[4] == 0x01) /*第一层*/
				{
					CarPortFlag = CarportA1;

				}
				if(FifoBuf_ZigbRx[4] == 0x02) /*第二层*/
				{
					CarPortFlag = CarportA2;

				}
				if(FifoBuf_ZigbRx[4] == 0x03) /*第三层*/
				{
					CarPortFlag = CarportA3;

				}
				if(FifoBuf_ZigbRx[4] == 0x04) /*第四层*/
				{
					CarPortFlag = CarportA4;

				}
			}
		}
		if(FifoBuf_ZigbRx[1] == 0x05 && FifoBuf_ZigbRx[2] == 0x03) /*车库B*/
		{
			if(FifoBuf_ZigbRx[3] == 0x01) /*普通层数回传*/
			{
				if(FifoBuf_ZigbRx[4] == 0x01) /*第一层*/
				{
					CarPortFlag = CarportB1;

				}
				if(FifoBuf_ZigbRx[4] == 0x02) /*第二层*/
				{
					CarPortFlag = CarportB2;

				}
				if(FifoBuf_ZigbRx[4] == 0x03) /*第三层*/
				{
					CarPortFlag = CarportB3;

				}
				if(FifoBuf_ZigbRx[4] == 0x04) /*第四层*/
				{
					CarPortFlag = CarportB4;

				}
			}
		}

	}


	/*ETC闸门回传开启状态*/
	if(FifoBuf_ZigbRx[0] == 0x55 && FifoBuf_ZigbRx[1]  == 0x0c)
	{
		if(FifoBuf_ZigbRx[2] == 0x01 && FifoBuf_ZigbRx[3] == 0x01 && FifoBuf_ZigbRx[4] == 0x06)
		{
			GateFlag = GateOpen;
		}

	}


	/*报警台回传救援坐标*/
	if(FifoBuf_ZigbRx[0] == 0x55 && FifoBuf_ZigbRx[1]  == 0x07)
	{
		if(FifoBuf_ZigbRx[2] == 0x01)
		{
			RescueLocation = FifoBuf_ZigbRx[3];
		}
	}

	/*公交站回传数据*/
	if(FifoBuf_ZigbRx[0] == 0x55 && FifoBuf_ZigbRx[1]  == 0x06)
	{
		if(FifoBuf_ZigbRx[2] == 0x02) /*日期*/
		{
			BusData.year  =   FifoBuf_ZigbRx[3]/16*10 + FifoBuf_ZigbRx[3]%16;
			BusData.month =   FifoBuf_ZigbRx[4]/16*10 + FifoBuf_ZigbRx[4]%16;
			BusData.day   =   FifoBuf_ZigbRx[5]/16*10 + FifoBuf_ZigbRx[5]%16;

		}

		if(FifoBuf_ZigbRx[2] == 0x03) /*时间*/
		{
			BusData.hour  =   FifoBuf_ZigbRx[3]/16*10 + FifoBuf_ZigbRx[3]%16;
			BusData.min   =   FifoBuf_ZigbRx[4]/16*10 + FifoBuf_ZigbRx[4]%16;
			BusData.secs  =   FifoBuf_ZigbRx[5]/16*10 + FifoBuf_ZigbRx[5]%16;
		}

		if(FifoBuf_ZigbRx[2] == 0x04) /*天气和温度*/
		{
			BusData.weather     = FifoBuf_ZigbRx[3];
			BusData.temperature = FifoBuf_ZigbRx[4];

		}
	}

	/*从车发来的车牌信息*/
	if(FifoBuf_ZigbRx[0] == 0x55 && FifoBuf_ZigbRx[1]  == 0x12)
	{
		if(FifoBuf_ZigbRx[2]  == 0x20)
		{
			SlaveCarData.chepai[0] = FifoBuf_ZigbRx[3];
			SlaveCarData.chepai[1] = FifoBuf_ZigbRx[4];
			SlaveCarData.chepai[2] = FifoBuf_ZigbRx[5];
		}

		if(FifoBuf_ZigbRx[2]  == 0x21)
		{
			SlaveCarData.chepai[3] = FifoBuf_ZigbRx[3];
			SlaveCarData.chepai[4] = FifoBuf_ZigbRx[4];
			SlaveCarData.chepai[5] = FifoBuf_ZigbRx[5];
		}

	}



}






/**************************************************************************
函数功能：CAN-Track数据处理
入口参数：无
返回  值：无
**************************************************************************/
void Slove_Track(void)
{
	x1 = FifoBuf_Track[0];
	x2 = FifoBuf_Track[1];
}


/**************************************************************************
函数功能：CAN-Naving数据处理
入口参数：无
返回  值：无
**************************************************************************/
void Slove_Naving(void)
{
	Current_Angle = (FifoBuf_Navig[0]<<8)|FifoBuf_Navig[1];	/*高位在前，低位在后*/
	while(Current_Angle >= 3600){Current_Angle -= 3600;}
}



/**************************************************************************
函数功能：CAN-Host数据处理
入口参数：无
返回  值：无
**************************************************************************/
void Slove_Host(void)
{
	if(FifoBuf_HOST[0] == 0x02)
	{
		CanHost_Mp  =  (FifoBuf_HOST[2]<<8) | FifoBuf_HOST[1];/*码盘值A*/
		CanHost_Mp1 = (FifoBuf_HOST[4]<<8) | FifoBuf_HOST[3]; /*码盘值B*/

	}
	if(FifoBuf_HOST[0] == 0x04)
	{
		CanHost_Mp2 = (FifoBuf_HOST[2]<<8) | FifoBuf_HOST[1]; /*码盘值C*/
		CanHost_Mp3 = (FifoBuf_HOST[4]<<8) | FifoBuf_HOST[3]; /*码盘值D*/
	}

	/*陀螺仪数据*/
	if(FifoBuf_HOST[0] == 0x21)
	{
		/*pitch是围绕X轴旋转，也叫做俯仰角*/
		pitch = (float)((FifoBuf_HOST[1] << 24) | (FifoBuf_HOST[2] << 16) | (FifoBuf_HOST[3] << 8) | FifoBuf_HOST[4]);
	}
	if(FifoBuf_HOST[0] == 0x22)
	{
		/*yaw是围绕Y轴旋转，也叫偏航角*/
		yaw = (float)((FifoBuf_HOST[1] << 24) | (FifoBuf_HOST[2] << 16) | (FifoBuf_HOST[3] << 8) | FifoBuf_HOST[4]);
	}
	if(FifoBuf_HOST[0] == 0x23)
	{
		/*roll是围绕Z轴旋转，也叫翻滚角*/
		roll = (float)((FifoBuf_HOST[1] << 24) | (FifoBuf_HOST[2] << 16) | (FifoBuf_HOST[3] << 8) | FifoBuf_HOST[4]);
	}
}



/**************************************************************************
函数功能：CAN-全部数据处理
入口参数：无
返回  值：无
**************************************************************************/
void Slove_ALL(void)
{
	if(Can_RxFiFoBuf[2].Flag == 1) /*Zigbee*/
	{
		Slove_ZigbeeData();
		Can_RxFiFoBuf[2].Flag = 0;
	}
	if(Can_RxFiFoBuf[1].Flag == 1) /*Wifi*/
	{
		Slove_AndroidData();
		Slove_Camera();
		Slove_GoFlag();

	}
	if(Can_RxFiFoBuf[3].Flag == 1) /*循迹*/
	{
		Slove_Track();
		Can_RxFiFoBuf[3].Flag = 0;
	}
	if(Can_RxFiFoBuf[4].Flag == 1) /*Naving*/
	{
		Slove_Naving();
		Can_RxFiFoBuf[4].Flag = 0;
	}
	if(Can_RxFiFoBuf[5].Flag == 1) /*Host*/
	{
		Slove_Host();
		Can_RxFiFoBuf[5].Flag = 0;
	}
}


/**************************************************缓冲区操作***************************************************************/

/**************************************************************************
函数功能：CAN-写入缓冲区
入口参数：CanBuf:对应缓冲区结构体  Data:写入数组 len:写入字节大小(别超过Can_RxFIFOZize最大值)
返回  值：无
**************************************************************************/
void Can_RxBufWrite(Can_RXFIFOBUF *CanBuf, uint8_t *Data, uint8_t len)
{
	if(CanBuf == NULL){return;}
	if(Data == NULL){return;}

    if(CanBuf->wp >= Can_RxFIFOZize){CanBuf->wp = 0;}

   // 检查是否需要回绕
	if (CanBuf->wp + len > Can_RxFIFOZize)
	{
		/*回绕处理*/
		uint16_t first_part = Can_RxFIFOZize - CanBuf->wp;
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
uint8_t Can_RxCheckReadEn(Can_RXFIFOBUF *p)
{
	if(p == _NULL){return 0;}

	uint8_t EN = 0;

	if(p->rp == Can_RxFIFOZize-1)
	{
		if(p->wp != 0)
			EN = 1;
	}
	else if(p->wp != (p->rp+1))
		EN = 1;

	return EN;
}



/**************************************************************************
函数功能：CAN-缓冲区读取一个字节
入口参数：CanBuf:对应缓冲区结构体 Data接收一字节变量
返回  值：1-可读 0-空
**************************************************************************/
uint8_t Can_RxReadBit(Can_RXFIFOBUF *CanBuf, uint8_t *data)
{
    // 1. 检查是否有数据 (复用您写的检查函数)
    if (Can_RxCheckReadEn(CanBuf) == 0)
    {
        return 0; // 空的
    }

    // 2. 移动读指针 (您的逻辑是先++再读取)
    if (++CanBuf->rp >= Can_RxFIFOZize)
    {
        CanBuf->rp = 0;
    }

    // 3. 取出数据
    *data = CanBuf->Data[CanBuf->rp];
    return 1;
}





