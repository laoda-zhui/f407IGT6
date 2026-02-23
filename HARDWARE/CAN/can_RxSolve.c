#include "can_RxSolve.h"


/*10ms wifi空闲处理数据*/
uint32_t WifiWaitTime = 10,WifiStartTime=0;
uint32_t Zigbee_RxTime = 0;

RQStruct RQData;//二维码数据结构体




/**************************************************接收数组初始化***************************************************************/

/*对应缓冲数组 -- 大小参考can_RxSolve.h宏定义*/
uint8_t 	FifoBuf_Info[Can_RxInfoZize] 		= 	{0}; 	/*0接收显示屏*/
uint8_t 	FifoBuf_WifiRx[Can_RxFIFOZize] 		= 	{0}; 	/*1接收wifi*/
uint8_t 	FifoBuf_ZigbRx[Can_RxFIFOZize] 		= 	{0}; 	/*2接收zigbee*/
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

/*安卓数据车辆启动标志位*/
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
	//wifi
	Can_RxFiFoBuf[1].Data = FifoBuf_WifiRx;
	Can_RxFiFoBuf[1].rp = Can_RxFIFOZize-1;		/*读索引复位*/
	Can_RxFiFoBuf[1].wp = 0;					/*写索引复位*/
	Can_RxFiFoBuf[1].Flag = 0;					/*清空cmd结构体的标志位*/

	//zigbee
	Can_RxFiFoBuf[2].Data = FifoBuf_ZigbRx;
	Can_RxFiFoBuf[2].rp = Can_RxFIFOZize-1;		/*读索引复位*/
	Can_RxFiFoBuf[2].wp = 0;					/*写索引复位*/
	Can_RxFiFoBuf[2].Flag = 0;					/*清空cmd结构体的标志位*/

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
		case 1:			/*wifi rx*/
			Can_RxBufWrite(&Can_RxFiFoBuf[1], RxData, RxMsgArray.DLC);
			Can_RxFiFoBuf[1].Flag = 1;
			WifiStartTime = HAL_GetTick();
			break;
		case 2:			/*zigbee rx*/
			Can_RxBufWrite(&Can_RxFiFoBuf[2], RxData, RxMsgArray.DLC);
			Can_RxFiFoBuf[2].Flag = 1;
			Zigbee_RxTime = HAL_GetTick();
			break;
		case 0:			/*disp*/
			memcpy(FifoBuf_Info, RxData, RxMsgArray.DLC);
			Can_RxFiFoBuf[0].Flag = 1;
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

    // 1. 检查是否有数据
    Can_RXFIFOBUF *pBuf = &Can_RxFiFoBuf[1];
    if (Can_RxCheckReadEn(pBuf) == 0) return;

    uint8_t head=0;

    // 2. 【关键】先偷看第一个字节，不移动 rp 指针
    RingBuf_PeekByte(pBuf, &head);


	// --- 情况 A: 二进制指令 (0x55 开头) ---
    if (head == 0x55)
    {
		// 不需要等待超时，直接全部读出来处理
		// (代码与您之前的一样，省略部分细节)

		char temp_buf[Can_RxFIFOZize + 1];
		uint16_t len = 0;
		uint8_t byte;

		// 真正取出数据
		while (Can_RxReadBit(pBuf, &byte) == 1)
		{
			if (len < Can_RxFIFOZize)
			{
				temp_buf[len++] = (char)byte;
			}
		}



        // 这里把原来的 while 循环改成了 for 循环遍历 temp_buf
        // 逻辑完全复用您的状态机，只是数据源变了
        static uint8_t state = 0;

        for (uint16_t i = 0; i < len; i++)
        {
            byte = (uint8_t)temp_buf[i];

            switch (state)
            {
            case 0: // 找包头 第1字节
                if (byte == 0x55){state = 1;}
                break;

            case 1: // 分支判断 第2字节
                if(byte == 0x03){state = 10;}      // 张老师版
                else if(byte == 0x55){state = 1;}  // 重复包头
                else if(byte == 0x00){state = 20;} // 商家版
                else if(byte == 0xAA){state = 30;} // 自动驾驶启动数据
                else state = 0;
                break;

            case 10: // 张老师版数据 第3字节
                if(byte == 0x01){AndroidFlag = TrafficRed_Flag;}
                else if(byte == 0x02){AndroidFlag = TrafficYellow_Flag;}
                else if(byte == 0x03){AndroidFlag = TrafficGreen_Flag;}
                state = 0;
                break;


            case 20: // 商家版第3字节
                if(byte == 0x02){state = 21;}
                else if(byte == 0x55){state = 1;}
                else{state = 0;}
                break;
            case 21: // 商家版数据 第4字节
                if (byte == 0x01){AndroidFlag = TrafficRed_Flag;}
                else if (byte == 0x03){AndroidFlag = TrafficYellow_Flag;}
                else if (byte == 0x02){AndroidFlag = TrafficGreen_Flag;}
                state = 0;
                break;


            case 30: // 自动驾驶数据 第3字节
                if(byte == 0xA0){state = 31;}
                else{state = 0;}
            	break;
            case 31: // 自动驾驶数据 第4字节
            	state = 32;
            	break;
            case 32: // 自动驾驶数据 第5字节
            	state = 33;
            	break;
            case 33: // 自动驾驶数据 第6字节
              	state = 34;
            	break;
            case 34: // 自动驾驶数据 第7字节
            	if (byte == 0xA0){AndroidGoFlag = 1;}
              	state = 0;
            	break;


            default:
                state = 0;
                break;
            }
        }

        // 处理完清零 Flag
        Can_RxFiFoBuf[1].Flag = 0;
    }
    // === 分支 B：字符串协议 (非 0x55 开头) ===
    else
    {
        if (HAL_GetTick() - WifiStartTime >= WifiWaitTime)
        {
            // 时间到了！全部读出来
            char temp_buf[Can_RxFIFOZize + 1];
            uint16_t len = 0;
            uint8_t byte;

            while (Can_RxReadBit(pBuf, &byte) == 1)
            {
                if (len < Can_RxFIFOZize) temp_buf[len++] = (char)byte;
            }
            temp_buf[len] = '\0';

            // 只有当积累了足够长的数据或者超时后才解析字符串
			// 这里我们可以简单地假设：只要不是55开头，就尝试用strstr搜一遍
			char *p;
			// 形状解析
			if ((p = strstr(temp_buf, "sharp3=")) != NULL)    CameraData.ruijiao = atoi(p + 7);
			if ((p = strstr(temp_buf, "dun3=")) != NULL)      CameraData.dunjiao = atoi(p + 5);
			if ((p = strstr(temp_buf, "right3=")) != NULL)    CameraData.zhijiao = atoi(p + 7);
			if ((p = strstr(temp_buf, "lin4=")) != NULL)      CameraData.lingxing = atoi(p + 5);
			if ((p = strstr(temp_buf, "rectangle=")) != NULL) CameraData.changfan = atoi(p + 10);
			if ((p = strstr(temp_buf, "square=")) != NULL)    CameraData.juxing = atoi(p + 7);
			if ((p = strstr(temp_buf, "star=")) != NULL)      CameraData.star = atoi(p + 5);
			if ((p = strstr(temp_buf, "circle=")) != NULL)    CameraData.circle = atoi(p + 7);

			// 颜色解析
			if ((p = strstr(temp_buf, "red=")) != NULL)    CameraData.red = atoi(p + 4);
			if ((p = strstr(temp_buf, "green=")) != NULL)  CameraData.green = atoi(p + 6);
			if ((p = strstr(temp_buf, "blue=")) != NULL)   CameraData.blue = atoi(p + 5);
			if ((p = strstr(temp_buf, "yellow=")) != NULL) CameraData.yellow = atoi(p + 7);
			if ((p = strstr(temp_buf, "cyan=")) != NULL)   CameraData.qingse = atoi(p + 5);
			if ((p = strstr(temp_buf, "orange=")) != NULL) CameraData.orange = atoi(p + 7);
			if ((p = strstr(temp_buf, "purple=")) != NULL) CameraData.purple = atoi(p + 7);
			if ((p = strstr(temp_buf, "black=")) != NULL)  CameraData.black = atoi(p + 6);

			//二维码解析

			char *p1 = strstr(temp_buf, "-1-");
			char *p2 = strstr(temp_buf, "-2-");
			char *p3 = strstr(temp_buf, "-3-");
			char *total_end = temp_buf + strlen(temp_buf);

			// --- 处理 QR1 ---
			if (p1 != NULL) {
			    char *data_start = p1 + 3; // 直接指向数据开始
			    char *data_end = total_end; // 默认到底

			    if (p2 != NULL) data_end = p2; // 有2到2

			    int len = data_end - data_start;
			    if (len > 0 && len < 50)
			    {
			        memcpy(RQData.RQ1Buf, data_start, len);
			        RQData.RQ1Buf[len] = '\0';
			    }
			}

			// --- 处理 QR2 ---
			if (p2 != NULL) {
			    char *data_start = p2 + 3;
			    char *data_end = total_end;

			    if(p3 != NULL) data_end = p3;

			    int len = data_end - data_start;
			    if(len > 0 && len < 50)
			    {
			        memcpy(RQData.RQ2Buf, data_start, len);
			        RQData.RQ2Buf[len] = '\0';
			    }
			}

			// --- 处理 QR3 ---
			if(p3 != NULL) {
			    char *data_start = p3 + 3;
			    char *data_end = total_end;

			    int len = data_end - data_start;
			    if (len > 0 && len < 50)
			    {
			        memcpy(RQData.RQ3Buf, data_start, len);
			        RQData.RQ3Buf[len] = '\0';
			    }
			}

			// --- 处理车牌号 ---
			char *pSearch = strstr(temp_buf, "国");
			if(pSearch != NULL)
			{
				char *pStart = pSearch + 3; // 跳过“国”字的3个字节
				char *pEnd = strchr(temp_buf, '-');

				if(pStart != NULL && pEnd != NULL)
				{
					int len = pEnd - (pStart);
					memcpy(&SlaveCarData.chepai,pStart,len);
					SlaveCarData.chepai[len] = '\0';


					char *pcolorE = strchr(pEnd+1, '-');
					if(pcolorE != NULL)
					{
						int len2 = pcolorE - pEnd - 1;
						memcpy(&SlaveCarData.Color, pEnd+1, len2);
						SlaveCarData.Color[len2] = '\0';
					}
				}
			}








            Can_RxFiFoBuf[1].Flag = 0;
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
	uint8_t ZigbeeBuf[8]={0};

	 for(uint8_t i = 0; i < 8; i++)
	{
		// 尝试读取一个字节
		if(Can_RxReadBit(&Can_RxFiFoBuf[2], &ZigbeeBuf[i]) == 0)
		{
			// 如果读空了（比如不满8字节），这里补0，防止数组越界
			ZigbeeBuf[i] = 0x00;
		}
	}



	/*车库回传层数状态*/
	if(ZigbeeBuf[0] == 0x55)
	{
		if(ZigbeeBuf[1] == 0x0D && ZigbeeBuf[2] == 0x03) /*车库A*/
		{
			if(ZigbeeBuf[3] == 0x01) /*普通层数回传*/
			{
				if(ZigbeeBuf[4] == 0x01) /*第一层*/
				{
					CarPortFlag = CarportA1;

				}
				if(ZigbeeBuf[4] == 0x02) /*第二层*/
				{
					CarPortFlag = CarportA2;

				}
				if(ZigbeeBuf[4] == 0x03) /*第三层*/
				{
					CarPortFlag = CarportA3;

				}
				if(ZigbeeBuf[4] == 0x04) /*第四层*/
				{
					CarPortFlag = CarportA4;

				}
			}
		}
		if(ZigbeeBuf[1] == 0x05 && ZigbeeBuf[2] == 0x03) /*车库B*/
		{
			if(ZigbeeBuf[3] == 0x01) /*普通层数回传*/
			{
				if(ZigbeeBuf[4] == 0x01) /*第一层*/
				{
					CarPortFlag = CarportB1;

				}
				if(ZigbeeBuf[4] == 0x02) /*第二层*/
				{
					CarPortFlag = CarportB2;

				}
				if(ZigbeeBuf[4] == 0x03) /*第三层*/
				{
					CarPortFlag = CarportB3;

				}
				if(ZigbeeBuf[4] == 0x04) /*第四层*/
				{
					CarPortFlag = CarportB4;

				}
			}
		}

	}


	/*ETC闸门回传开启状态*/
	if(ZigbeeBuf[0] == 0x55 && ZigbeeBuf[1]  == 0x0c)
	{
		if(ZigbeeBuf[2] == 0x01 && ZigbeeBuf[3] == 0x01 && ZigbeeBuf[4] == 0x06)
		{
			GateFlag = GateOpen;
		}

	}


	/*报警台回传救援坐标*/
	if(ZigbeeBuf[0] == 0x55 && ZigbeeBuf[1]  == 0x07)
	{
		if(ZigbeeBuf[2] == 0x01)
		{
			RescueLocation = ZigbeeBuf[3];
		}
	}

	/*公交站回传数据*/
	if(ZigbeeBuf[0] == 0x55 && ZigbeeBuf[1]  == 0x06)
	{
		if(ZigbeeBuf[2] == 0x02) /*日期*/
		{
			BusData.year  =   ZigbeeBuf[3]/16*10 + ZigbeeBuf[3]%16;
			BusData.month =   ZigbeeBuf[4]/16*10 + ZigbeeBuf[4]%16;
			BusData.day   =   ZigbeeBuf[5]/16*10 + ZigbeeBuf[5]%16;

		}

		if(ZigbeeBuf[2] == 0x03) /*时间*/
		{
			BusData.hour  =   ZigbeeBuf[3]/16*10 + ZigbeeBuf[3]%16;
			BusData.min   =   ZigbeeBuf[4]/16*10 + ZigbeeBuf[4]%16;
			BusData.secs  =   ZigbeeBuf[5]/16*10 + ZigbeeBuf[5]%16;
		}

		if(ZigbeeBuf[2] == 0x04) /*天气和温度*/
		{
			BusData.weather     = ZigbeeBuf[3];
			BusData.temperature = ZigbeeBuf[4];

		}
	}

	/*从车发来的车牌信息*/
	if(ZigbeeBuf[0] == 0x55 && ZigbeeBuf[1]  == 0x01)
	{
		if(ZigbeeBuf[2]  == 0x20)
		{
			SlaveCarData.chepai[0] = ZigbeeBuf[3];
			SlaveCarData.chepai[1] = ZigbeeBuf[4];
			SlaveCarData.chepai[2] = ZigbeeBuf[5];
		}

		if(ZigbeeBuf[2]  == 0x21)
		{
			SlaveCarData.chepai[3] = ZigbeeBuf[3];
			SlaveCarData.chepai[4] = ZigbeeBuf[4];
			SlaveCarData.chepai[5] = ZigbeeBuf[5];
		}

	}




	/*从车来发的停车地点*/
	if(ZigbeeBuf[0] == 0x55 && ZigbeeBuf[1]  == 0x01 && ZigbeeBuf[2]  == 0x44)
	{

		if(ZigbeeBuf[3]  == 'D' && ZigbeeBuf[4]  == '7')
		{
			SlaveCarData.Location = 1;
		}
		if(ZigbeeBuf[3]  == 'F' && ZigbeeBuf[4]  == '7')
		{
			SlaveCarData.Location = 2;
		}
		if(ZigbeeBuf[3]  == 'G' && ZigbeeBuf[4]  == '6')
		{
			SlaveCarData.Location = 3;
		}
		if(ZigbeeBuf[3]  == 'G' && ZigbeeBuf[4]  == '4')
		{
			SlaveCarData.Location = 4;
		}
	}

	/*从车发来的允许启动*/
	if(ZigbeeBuf[0] == 0x55 && ZigbeeBuf[1]  == 0x01)
	{
		if(ZigbeeBuf[2]  == 0x66 && ZigbeeBuf[3]  == 0x66)
		{
			SlaveCarData.GoFlag = 1;
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
	// 在 CanRx_Loop 里记录一下最后接收时间 Zigbee_RxTime = HAL_GetTick();

    while (Can_RxGetLen(&Can_RxFiFoBuf[2]) >= 8)
    {
        Slove_ZigbeeData();
    }

    // 处理剩下的残余数据 (如果有，且超时)
    if (Can_RxGetLen(&Can_RxFiFoBuf[2]) > 0 && (HAL_GetTick() - Zigbee_RxTime > 50))
    {
        Slove_ZigbeeData(); // 这里会补 0
    }
	if(Can_RxFiFoBuf[1].Flag == 1) /*Wifi*/
	{
		Slove_AndroidData();
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
	if(p == NULL){return 0;}

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

/**************************************************************************
函数功能：CAN-缓冲区不移动指针读取第一字节的数据
入口参数：CanBuf:对应缓冲区结构体 val接收一字节变量
返回  值：1-可读 0-空
**************************************************************************/
uint8_t RingBuf_PeekByte(Can_RXFIFOBUF *buf, uint8_t *val)
{
    if (Can_RxCheckReadEn(buf) == 0) return 0;

    // 注意：根据您的 CheckReadEn 逻辑，rp 指向的是已读位置
    // 所以要看的数据在 rp + 1
    uint16_t next_rp = buf->rp + 1;
    if (next_rp >= Can_RxFIFOZize) next_rp = 0;

    *val = buf->Data[next_rp];
    return 1;
}

/**************************************************************************
函数功能：CAN-获取环形缓冲区当前数据长度
入口参数：CanBuf:对应缓冲区结构体
返回  值：数据长度
**************************************************************************/
uint16_t Can_RxGetLen(Can_RXFIFOBUF *p)
{
    if (p == NULL) return 0;

    // 逻辑核心：因为你的 rp 初始化为 Size-1，且读取时先 ++rp
    // 所以数据的起始位置逻辑上是 (rp + 1)
    // 长度 = 写指针 - 读指针的下一位

    int32_t len = p->wp - (p->rp + 1);

    // 如果结果是负数，说明发生了回绕，加上缓冲区总大小即可
    if (len < 0)
    {
        len += Can_RxFIFOZize;
    }

    return (uint16_t)len;
}

