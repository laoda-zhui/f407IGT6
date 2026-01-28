#include "Task.h"


/*常用速度定义*/
#define GoSpeed  		120
#define TrackSpeed  	77
#define TurnSpeed  		80
#define MPTurnSpeed 	30

static uint16_t  StepIdx = 0;    // 当前行号
static uint8_t  StepState = 0;  // 0:启动 1:等待
static uint32_t TimerStart = 0; // 计时器


uint8_t LightInit=0; /*路灯初始档位*/
uint8_t VoiceNumber=0; /*公交语音号*/

uint8_t Sector=0; //扇区地址 第几扇区
uint8_t Block=0;	//块地址
char Coordinate[10]; //位置字符串


uint8_t SendBattery[3]={0};

/*包装函数*/
void RC522Read(void) //卡1要读的扇区块地址
{
	RC522(18, RFID_Read);
}
void RC522Read2(void) //卡2要读的扇区块地址
{
	RC522((Sector*4+Block+1), RFID_Read);
}



/*卡1数据处理*/
void RFID_Slove()
{
	char RFIDBuf[17]={0}; //缓存RFid的接收
	memcpy(RFIDBuf, READ_RFID, sizeof(READ_RFID));
	RFIDBuf[16] = '\0';

	char *pStart  = strchr(RFIDBuf, '<');
	char *pEnd = strchr(RFIDBuf, '>');
	if(pStart  !=NULL && pEnd !=NULL)
	{
		int len = pEnd - pStart  - 1; // >位置减去- <位置,那就是<>内的数据+ '>'
		memcpy(Coordinate, pStart +1, len); //把<>内的数据拿出来
		Coordinate[len]='\0';
		memset(pStart , ' ', pEnd - pStart + 1); //把<>以及<>内的数据删去

		char *num1_ptr = strpbrk(RFIDBuf, "0123456789ABCDEF"); /*查找第一个数字的位置*/
		if(num1_ptr != NULL)
		{
			Sector =  CharToHex(*num1_ptr);/*把第一个字符串数字用十六进制的格式转换成数字*/
			char *num2_ptr = strpbrk(num1_ptr+1, "0123456789ABCDEF");/*查找第2个数字的位置*/
			if(num2_ptr != NULL)
			{
				Block =  CharToHex(*num2_ptr);/*把第2个字符串数字用十六进制的格式转换成数字*/
			}
		}
	}
}
/*卡2数据处理*/
void RFID_Slove2()
{
	char RFIDBuf[17]={0}; //缓存RFid的接收
	char buf1[9]={0},buf2[9]={0};
	uint32_t w1=0,w2=0,w0;

	memcpy(RFIDBuf, READ_RFID, sizeof(READ_RFID));
	RFIDBuf[16] = '\0';

	// 1. 【有效性自检】
	// 检查卡片数据是否为全 0（空卡或读卡失败）
	uint8_t all_zero = 1;
	for(uint8_t i = 0; i < 16; i++)
	{
		if(RFIDBuf[i] != 0)
		{
			all_zero = 0; // 只要有一个字节不是0，就说明有数据
			break;
		}
	}
	// 如果是全 0 数据，直接退出函数，不执行后面的计算
	// 这样 SendBattery 数组里的旧数据就不会被覆盖
	if(all_zero == 1) return;


	memcpy(buf1, RFIDBuf, 8);
	buf1[8] = '\0';

	w1 = strtoul(buf1, NULL, 16);

	memcpy(buf2, &RFIDBuf[8], 8);
	buf2[8] = '\0';
	w2 = strtoul(buf2, NULL, 16);

	w0 = w1^w2;

	SendBattery[0] = (w0 >> 24);
	SendBattery[1] = (w0 >> 16) &0xff;
	SendBattery[2] = (w0 >> 8) & 0xff;
}


uint8_t RFShanqu1;
uint8_t RFkuai1;

/*23真题卡1处理*/
void Ti23_RF1Read(void)
{
	RC522(22, RFID_Read);
}
void Ti23_RF1Slove(void)
{
	char RFBuf[7];
	RFBuf[6] = '\0';
	memcpy(RFBuf,READ_RFID,6);
	char *p1 = strpbrk(RFBuf, "0123456789");
	if(p1!=NULL)
	{
		RFShanqu1 = CharToHex(*p1);
		char *p2 = strpbrk(p1+1, "0123456789");;
		if(p2!=NULL)
		{
			RFkuai1 = CharToHex(*p2);
		}
	}

}
/*23真题卡2读和处理*/

uint8_t RFShanqu2;
uint8_t RFkuai2;
void Ti23_RF2Read(void)
{
	RC522(RFShanqu1*4+RFkuai1, RFID_Read);
}
void Ti23_RF2Slove(void)
{
	char RFBuf[7];
	RFBuf[6] = '\0';
	memcpy(RFBuf,READ_RFID,6);
	char *p1 = strpbrk(RFBuf, "0123456789");
	if(p1!=NULL)
	{
		RFShanqu2 = CharToHex(*p1);
		char *p2 = strpbrk(p1+1, "0123456789");;
		if(p2!=NULL)
		{
			RFkuai2 = CharToHex(*p2);
		}
	}

}

/*23真题卡3读和处理*/
char BtNum[4];
void Ti23_RF3Read(void)
{
	RC522(RFShanqu2*4+RFkuai2, RFID_Read);
}
void Ti23_RF3Slove(void)
{
	char RFBuf[17];
	RFBuf[16] = '\0';
	BtNum[3]  =  '\0';
	memcpy(RFBuf,READ_RFID,16);

	char *pStart = strstr(RFBuf, "-%");
	if(pStart != NULL)
	{
		char *pEnd = strstr(pStart+2, "-%");
		if(pEnd != NULL)
		{
			int len = pEnd - pStart -2;

			if(len == 3)
			{
				BtNum[0] = CharToHex(*(pStart+2));
				BtNum[1] = CharToHex(*(pStart+3));
				BtNum[2] = CharToHex(*(pStart+4));
			}
		}

	}
}
/*23年真题二维码1*/
/*二维码解密后的数组*/
char QR1Num[50]; //二维码1
char QR2Num[50]; //二维码2
void Ti2_RQSlove(void)
{
	char* pStart1 = strstr(RQData.RQ1Buf, "/*");
	char* pEnd1   = strstr(RQData.RQ1Buf, "*/");

	if(pStart1 != NULL && pEnd1 != NULL)
	{
		int len = pEnd1 - pStart1 - 2; //真实数据长度

		memcpy(QR1Num, pStart1+2, len);
		QR1Num[len] = '\0';
	}

	/*23年真题二维码2*/
	char* pStart2 = strstr(RQData.RQ2Buf, "-/");
	if(pStart2 != NULL)
	{
		char* pEnd2   = strstr(pStart2+2, "-/");
		if(pEnd2 != NULL)
		{
			int len = pEnd2 - pStart2 - 2; //真实数据长度

			memcpy(QR2Num, pStart2+2, len);
			QR2Num[len] = '\0';
		}
	}
}


//从车
void SendLight(void) //发送路灯初始值
{
	Command_SlaveCarLight(LightInit);
}

void SendBat(void) //给从车发送无线充电开启码
{
	Command_SlaveCarSendBt(SendBattery[0], SendBattery[1], SendBattery[2]);
}
void Sendyanxing(void)
{
	Command_TFTBShowHex((CameraData.juxing<<4|CameraData.circle), (CameraData.star<<4|CameraData.red), (CameraData.green<<4|CameraData.yellow));
}


/*ETC闸门前的徘徊任务*/
uint8_t Action_WaitGateOpen(void)
{
	static uint8_t States=0;

	switch(States)
	{
	case 0:
		Car_TrackMp(17,11.5);	/*前进*/
		States = 1;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			Car_Back(0,11.5);	/*后退*/
			States = 2;
			if(GateFlag == GateOpen)
			{
				return 1;
			}
		}
		break;
	case 2:
		if(Stop_Flag == Task_Complete)
		{
			Car_TrackMp(17,12);	/*前进*/
			States = 1;
			if(GateFlag == GateOpen)
			{
				return 1;
			}
		}

		break;
	}
	return 0;
}



void CommandLight(void) //获取路灯初始值 并调节路灯档位到车库初始值
{
	LightInit = Command_LightAuto(CarPortFlag);
}
void Ti2_CommandLight(void) //获取路灯初始值 并调节路灯档位到计算值
{
	LightInit = Command_LightAuto(3);
}
void Command_sendshap(void) //发送给tft显示形状
{
	Command_TFTBShowHex(0xA0|(CameraData.changfan+CameraData.juxing), 0xD0|(CameraData.lingxing), 0xE0|(CameraData.star));
}
void Command_sendcolor(void)//发送给led显示颜色
{
	Command_LEDShowDown(0xF0|(CameraData.red), 0xF0|(CameraData.green), 0xF0|(CameraData.blue));
}
void GetVoicNum(void)
{
	VoiceNumber = Voice_ASR(); /*开启语音识别,并获取编号*/
}
void SendAutoVoice(void)/*发送语音编号给评分系统*/
{
	Command_Autosystem(VoiceNumber);
}
void CommandCarPtB1(void) // 下降立体车库到1层
{
	Command_CarPortB(1);
}


void CommandGateF(void)
{
	Command_SetGateTop(0x41, 0x41, 0x41);
}
void CommandGateL(void)
{
	Command_SetGateLast(0x41, 0x41, 0x41);
}




/*执行任务表2*/
const TaskStep_t MyMission[] = {


		// --- 1. 起步开始计时and下降车库 ---
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         	0,       	0,     (void*)Command_StartTim},  	   // 开始计时
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         	0,        	0,     (void*)CommandCarPtB1},  	   // 降车库
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         	0,        	0,     (void*)CommandCarPtB1},  	   // 降车库
		{0, 	ACT_DELAY,       	22000,     	0,     NULL}, //等22s下降
		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_GetPortBFloor},  	   // 获取车库层数
		{0, 	ACT_DELAY,       	100,     	0,     NULL},
		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_GetPortBFloor},  	   // 获取车库层数
		{0, 	ACT_DELAY,       	100,     	0,     NULL},
		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_GetPortBFloor},  	   // 获取车库层数
		{0, 	ACT_WAIT_VAL_EQ,   	99999,    	1,     (void*)&CarPortFlag}, /*超时99s跳过(保险),只有接收到车库层数为1才走*/


		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
		{0, 	ACT_GO,          GoSpeed, 	 	8,     NULL},	/*前进*/
		{0, 	ACT_RIGHT,     	 TurnSpeed, 	0,     NULL},	/*右转*/



		// --- 2. ETC闸门前徘徊 ---
		{0, 	ACT_CUSTOM_LOOP,    0, 	0,     (void *)Action_WaitGateOpen},	/*前进后退直到etc开门*/
		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, ACT_CMD,         0,        0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
		{0, 	ACT_GO,          GoSpeed, 	 	8,     NULL},	/*前进*/
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, ACT_CMD,         0,        0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度


		// --- 3. 智能公交站前 ---
		{0, 	ACT_RIGHT,     	 TurnSpeed, 	0,     NULL},	/*右转*/
		{0, 	ACT_CMD,         0,        0,     (void*)Command_SlaveCarSTep}, // 给从车发送温度
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_GO,          GoSpeed, 	 	7,     NULL},	/*前进*/
			/*公交站功能:......*/
			{0, 	ACT_DELAY,       	50,     	0,     NULL},
			{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 命令公交站开语音
			{0, 	ACT_DELAY,       	50,     	0,     NULL},
			{0, 	ACT_CMD,         0,        0,     (void*)GetVoicNum}, // 开启语音识别


		{0, 	ACT_BACK,          GoSpeed, 	 	3,     NULL},	/*后退*/
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         0,        0,     (void*)Command_SlaveCarSTep}, // 给从车发送温度
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         0,        0,     (void*)Command_SlaveCarSTep}, // 给从车发送温度
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         0,        0,     (void*)Command_SlaveCarSTep}, // 给从车发送温度
		{0, 	ACT_LEFT,        TurnSpeed, 0,     NULL},  /*左转 背对交通灯*/
		{0, 	ACT_LEFT,        TurnSpeed, 0,     NULL},  /*左转 背对交通灯*/



		// --- 4. 到静态显示标志物-面向他 ---
		{0, 	ACT_TRACKMP,       TrackSpeed, 	32,     NULL}, /*循迹*/
		{0, 	ACT_DELAY,       	600,     	0,     NULL},
		{0, 	ACT_MPRIGHT,       MPTurnSpeed, 	92,     NULL},	/*码盘右转*/

			/*测距静态标志位物:..........*/
			{0, 	ACT_DELAY,       	1000,     	0,     NULL},	/*等停稳*/
			{0, 	ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上
			{0, 	ACT_DELAY,       	50,     	0,     NULL},
			{0, 	ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上

			/*二维码扫描静态标志物:..........*/
			{0, 	ACT_DELAY,       	1000,     	0,     NULL},
			{0, 	ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描(缺少数据处理)
			{0, 	ACT_DELAY,       	500,     	0,     NULL},
			{0, 	ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描(缺少数据处理)

		{0, 	ACT_LEFT,        TurnSpeed, 0,     NULL},  /*左转*/




		// --- 5. 读卡1 ---
		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
		{0, 	ACT_GO,          GoSpeed, 	 	7,     NULL},	/*前进*/
		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL}, /*循迹*/
		{0, 	ACT_GO,          GoSpeed, 	 	8,     NULL},	/*前进*/
		{0, 	ACT_DELAY,       	 800,     	0,     NULL}, /*等停稳*/
		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti23_RF1Read},   // 读取卡片结果
		{0, 	ACT_DELAY,       	 900,     	0,     NULL},
		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti23_RF1Slove},   // 读取卡片结果
			/*解密........*/


		// --- 6. 到多功能显示标志物A(B跳过) ---
		{0, 	ACT_RIGHT,     	 TurnSpeed, 	0,     NULL},	/*右转*/

			/*多功能任务:.............*/
			{0, 	ACT_DELAY,       	 2000,     	0,     NULL},
			{0, 	ACT_CMD,         	   0,        0,     (void*)Command_AndroidColor},   // 颜色识别
			{0, 	ACT_DELAY,       	 800,     	0,     NULL},
			{0, 	ACT_CMD,         	   0,        0,     (void*)Command_AndroidColor},   // 颜色识别
			{0, 	ACT_DELAY,       	 800,     	0,     NULL},
			{0, 	ACT_CMD,         	   0,        0,     (void*)Command_Androidshape},   // 颜色识别
			{0, 	ACT_DELAY,       	 800,     	0,     NULL},
			{0, 	ACT_CMD,         	   0,        0,     (void*)Command_Androidshape},   // 颜色识别
			{0, 	ACT_DELAY,       	 1000,     	0,     NULL},
			{0, 	ACT_CMD,         	   0,        0,     (void*)Sendyanxing},   // 颜色识别Command_TFTBShowHex
			{0, 	ACT_DELAY,       	  50,     	0,     NULL},
			{0, 	ACT_CMD,         	   0,        0,     (void*)Sendyanxing},


		// --- 7. 到特殊地形 ---
		{0, 	ACT_LEFT,        	TurnSpeed, 		0,     NULL},  /*左转*/
		{0, 	ACT_TRACKMP,      	TrackSpeed, 	15,     NULL}, /*循迹*/
		{0, 	ACT_BACK,          	GoSpeed, 	 	28,     NULL},	/*后退*/
		{0, 	ACT_TRACKRFID,      TrackSpeed, 	0,     NULL}, /*循迹*/
		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	/*前进*/


		{0, 	ACT_LEFT,        	TurnSpeed, 		0,     NULL},  /*左转*/
		{0, 	ACT_BACK,          	GoSpeed, 	 	5,     NULL},	/*后退*/

			/*过特殊地形*/
			{0, 	ACT_GO,          	GoSpeed, 	54,     NULL},	/*前进*/
			{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},  /*循迹*/


		// --- 8. 读卡2 ---
		{0, 	ACT_GO,          GoSpeed, 	 	7,     NULL},	/*前进*/
		{0, 	ACT_DELAY,       	 700,     	0,     NULL}, /*等停稳*/
		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti23_RF2Read},   // 读取卡片结果
		{0, 	ACT_DELAY,       	 900,     	0,     NULL}, /*等停稳*/
		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti23_RF2Slove},   // 读取卡片结果
			/*解密........*/



		// --- 9. 道闸(开启码为车牌识别，现跳过) ---
		{0, 	ACT_LEFT,        TurnSpeed, 0,     NULL},  /*左转*/
			/*道闸开启函数:..........*/


		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL}, /*循迹*/
		{0, 	ACT_GO,          GoSpeed, 	 	7,     NULL},	/*前进*/

		// --- 10. 读卡3 ---
		{0, 	ACT_DELAY,       	 900,     	0,     NULL}, /*等停稳*/
		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti23_RF3Read},   // 读取卡片结果
		{0, 	ACT_DELAY,       	 700,     	0,     NULL}, /*等停稳*/
			/*解密........*/
			{0, 	ACT_CMD,         	   0,        0,     (void*)Ti23_RF3Slove},   // 读取卡片结果


		// --- 11. 行驶到交通灯B前 ---
		{0, 	ACT_CMD,         	   0,        0,     (void*)Command_SlaveCarStart},   // 读取卡片结果
		{0, 	ACT_DELAY,       	 50,     	0,     NULL}, /*等停稳*/
		{0, 	ACT_CMD,         	   0,        0,     (void*)Command_SlaveCarStart},   // 读取卡片结果
		{0, 	ACT_DELAY,       	 50,     	0,     NULL}, /*等停稳*/
		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
		{0, 	ACT_GO,          GoSpeed, 	 	7,     NULL},	/*前进*/
		{0, 	ACT_RIGHT,     	 TurnSpeed, 	0,     NULL},	/*右转*/
		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
		{0, 	ACT_GO,          GoSpeed, 	 	6,     NULL},	/*前进*/
		{0, 	ACT_RIGHT,     	 TurnSpeed, 	0,     NULL},	/*右转*/
		{0, 	ACT_BACK,          GoSpeed, 	 	3,     NULL},	/*前进*/

		// --- 12. 交通灯B识别 ---
			//....
			{0, 	ACT_DELAY,       	 1000,     	0,     NULL}, /*等停稳*/
			{5, ACT_CMD,         0,        0,     (void*)Command_TrafficBInMode}, // 开启识别
			{6, ACT_DELAY,       1500,     0,     NULL},                          // 等1.5秒让摄像头稳定
			{7, ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
			{8, ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令2
			{9, ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令3
			// 【关键】等待 AndroidFlag 变动，超时 3000ms
			{10, ACT_WAIT_NEQ0,   3000,     0,     (void*)&AndroidFlag}, /*超时3s跳过，如果接收到AndroidFl发送结果*/
			{11, ACT_CMD,         0,        0,     (void*)Command_TrafficBSend},   // 发送结果
			{12, ACT_CMD,         0,        0,     (void*)Command_TrafficBSend},   // 发送结果



		// --- 13. 到智能路灯前面 ---
		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
		{0, 	ACT_GO,          GoSpeed, 	 	7,     NULL},	/*前进*/
		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
		{0, 	ACT_GO,          GoSpeed, 	 	8,     NULL},	/*前进*/
		{0, 	ACT_LEFT,        TurnSpeed, 	0,     NULL},  /*左转*/


		// --- 14. 调节智能路灯 ---
			//....
			{12, ACT_CMD,         0,        0,     (void*)Ti2_CommandLight},   // 发送结果

		// --- 15. 倒车入库 ---
		{0, 	ACT_LEFT,        TurnSpeed, 		0,     NULL},  /*左转*/
		{0, 	ACT_TRACKMP,       TrackSpeed, 		24,     NULL}, /*循迹*/
		{0, 	ACT_BACK,          GoSpeed, 	 	59,     NULL},	/*后退*/

};

/*执行任务表1*/
//const TaskStep_t MyMission[] = {
//    // ID(可重复，主要用于跳转)  动作类型,        参数1,    参数2,   指针(函数或变量)
//
//    // --- 1. 起步 ---
//	{6, ACT_DELAY,       10,     0,     NULL},
//    {0, ACT_CMD,         0,        0,     (void*)Command_StartTim}, //开启计时
//	{6, ACT_DELAY,       10,     0,     NULL},
//    {1, ACT_TRACK,       TrackSpeed, 0,     NULL}, /*循迹*/
//	{2, ACT_GO,          GoSpeed, 8,     NULL},	/*前进*/
//	{3, ACT_LEFT,        TurnSpeed, 0,     NULL},  /*左转面向交通灯*/
//	{4, ACT_BACK,        100, 6,     NULL},  /*后退一点好识别*/
//
//    // --- 2. 交通灯识别流程 ---
//    {5, ACT_CMD,         0,        0,     (void*)Command_TrafficAInMode}, // 开启识别
//    {6, ACT_DELAY,       1500,     0,     NULL},                          // 等1.5秒让摄像头稳定
//    {7, ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
//	{8, ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令2
//    {9, ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令3
//
//    // 【关键】等待 AndroidFlag 变动，超时 2000ms
//    {10, ACT_WAIT_NEQ0,   3000,     0,     (void*)&AndroidFlag}, /*超时3s跳过，如果接收到AndroidFlag颜色则直接发送结果*/
//
//    {11, ACT_CMD,         0,        0,     (void*)Command_TrafficASend},   // 发送结果
//	{12, ACT_CMD,         0,        0,     (void*)Command_TrafficASend},   // 发送结果
//
//    // --- 3. 继续跑路 ---
//    {13, ACT_TRACK,     TrackSpeed, 0,     NULL},
//    {14, ACT_CMD,         0,        0,     (void*)Command_GetPortBFloor}, // 发送 - 获取车库层数
//    {15, ACT_GO,        GoSpeed, 	7,     NULL},
//    {16, ACT_RIGHT,     TurnSpeed, 	0,     NULL},
//    {17, ACT_CMD,         0,        0,     (void*)Command_GetPortBFloor}, // 发送 - 获取车库层数
//    {18, ACT_TRACK,     TrackSpeed, 0,     NULL},
//    {19, ACT_GO,        GoSpeed, 	8,     NULL},
//
//	// --- 4. 读卡-左边是路灯 ---
//	{20, ACT_DELAY,       100,     	0,     NULL},
//	{21, ACT_CMD,         0,        0,     (void*)RC522Read},   // 读取卡片结果
//	{22, ACT_DELAY,       100,     	0,     NULL},
//	{23, ACT_CMD,         0,        0,     (void*)RFID_Slove},   // 处理卡片结果
//	{24, ACT_GO,        GoSpeed, 	1,     NULL},
//
//	// --- 5. 面向路灯 ---
//	{25, ACT_LEFT,        TurnSpeed, 0,     NULL},  /*左转面向路灯*/
//	{26, ACT_CMD,         0,         0,     (void*)CommandLight},   // 调节路灯
//	{27, ACT_DELAY,       50,     	0,     NULL},
//
//	// --- 6. 转到etc前 ---
//	{28, ACT_RIGHT,     TurnSpeed, 	0,     NULL},
//	{29, ACT_CMD,         0,         0,     (void*)SendLight},   // 发送给从车路灯初始档位
//
//	{30, ACT_RIGHT,     TurnSpeed, 	0,     NULL},
//	{31, ACT_DELAY,       50,     	0,     NULL},
//
//	{32, ACT_CMD,         0,         0,     (void*)SendLight},   // 发送给从车路灯初始档位
//	{33, ACT_DELAY,       50,     	0,     NULL},
//	{34, ACT_CMD,         0,         0,     (void*)SendLight},   // 发送给从车路灯初始档位
//
//
//	// --- 7. 过etc闸门 ---
//	{35, ACT_CUSTOM_LOOP, 		0, 	0, 		(void*)Action_WaitGateOpen}, /*ETC前进行后退直到开启闸门*/
//    {36, ACT_TRACK,     TrackSpeed, 0,     NULL},
//	{37, ACT_DELAY,       50,     	0,     NULL},
//	{38, ACT_CMD,         0,         0,     (void*)CommandCarPtB1},   // 下降车库
//    {39, ACT_GO,        GoSpeed, 	7,     NULL},
//	{40, ACT_DELAY,       50,     	0,     NULL},
//	{41, ACT_CMD,         0,         0,     (void*)CommandCarPtB1},   // 下降车库
//	{42, ACT_DELAY,       100,     0,     NULL},
//
//
//	// --- 8. 读卡-前面是特殊地形 ---
//	{43, ACT_CMD,         0,        0,     (void*)RC522Read},   // 读取卡片结果
//	{44, ACT_DELAY,       300,     	0,     NULL},
//	{45, ACT_CMD,         0,        0,     (void*)RFID_Slove},   // 处理卡片结果
//
//	// --- 9. 过特殊地形 ---
//	{46, ACT_BACK,      GoSpeed, 	10,     NULL},  /*后退一点好冲刺*/
//    {47, ACT_TRACKMP, 	TrackSpeed, 13,     NULL},	/*到白卡 用码盘循迹防止过白卡过头*/
//	{48, ACT_GO,        GoSpeed, 	58,     NULL},  /*过特殊路段*/
//    {49, ACT_TRACKMP,   TrackSpeed, 20,     	NULL},
//    {50, ACT_GO,        GoSpeed, 	7,     NULL},
//
//	// --- 10. 读卡 ---
//	{51, ACT_DELAY,       200,     	0,     NULL},	/*等停稳*/
//	{52, ACT_CMD,         0,        0,     (void*)RC522Read},   // 读取卡片结果
//	{53, ACT_DELAY,       100,     	0,     NULL},
//	{54, ACT_CMD,         0,        0,     (void*)RFID_Slove},   // 处理卡片结果
//	{55, ACT_DELAY,       100,     	0,     NULL},
//    {56, ACT_GO,        GoSpeed, 	2,     NULL},
//
//
//	// --- 11. TFT颜色形状识别 ---
//	{57, ACT_DELAY,       1500,     	0,     NULL}, /*等1.5s摄像头稳定*/
//	{58, ACT_CMD,         0,        0,     (void*)Command_Androidshape},   // 识别形状
//	{59, ACT_DELAY,       300,     	0,     NULL},
//	{60, ACT_CMD,         0,        0,     (void*)Command_Androidshape},   // 识别形状
//	{61, ACT_DELAY,       300,     	0,     NULL},
//	{62, ACT_CMD,         0,        0,     (void*)Command_AndroidColor},   // 识别颜色
//	{63, ACT_DELAY,       300,     	0,     NULL},
//	{64, ACT_CMD,         0,        0,     (void*)Command_AndroidColor},   // 识别颜色
//	{65, ACT_DELAY,       500,     	0,     NULL},
//	{66, ACT_CMD,         0,        0,     (void*)Command_sendshap},   	   // 发送识别的形状
//	{67, ACT_DELAY,       500,     	0,     NULL},
//	{68, ACT_CMD,         0,        0,     (void*)Command_sendcolor},  	   // 发送识别的颜色
//	{69, ACT_DELAY,       500,     	0,     NULL},
//
//
//	// --- 12. 到公交站请求回传温度和进行语音识别 ---
//	{70, ACT_CMD,         0,        0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
//	{71, ACT_LEFT,        TurnSpeed, 0,     NULL},
//
//	{72, ACT_DELAY,       50,     	0,     NULL},
//	{73, ACT_CMD,         0,        0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
//	{74, ACT_DELAY,       100,     	0,     NULL},
//
//	{75, ACT_TRACK,     TrackSpeed, 0,     	NULL},
//	{76, ACT_GO,        GoSpeed, 	16,     NULL},
//
//	{77, ACT_CMD,         0,        0,     (void*)Command_BusReportRandom},  	/*开启交站语音*/
//	{78, ACT_DELAY,       1,     	0,     NULL},
//	{79, ACT_CMD,         0,        0,     (void*)Command_BusReportRandom},  	/*开启交站语音*/
//	{80, ACT_DELAY,       50,     	0,     NULL},
//	{81, ACT_CMD,         0,        0,     (void*)GetVoicNum},  	/*开启语音识别*/
//
//	// --- 13. 识别完成，来到D2 ---
//	{82, ACT_DELAY,       1000,     	0,  NULL},
//
//	{83, ACT_CMD,         0,        0,     (void*)Command_SlaveCarSTep},  	/*发送给从车公交站温度信息*/
//	{82, ACT_DELAY,       100,     	0,  NULL},
//	{84, ACT_BACK,        GoSpeed,  5,     NULL},
//	{84, ACT_LEFT,        TurnSpeed, 0,     NULL},
//
//	{85, ACT_DELAY,       50,     	0,     NULL},
//	{86, ACT_CMD,         0,        0,     (void*)Command_SlaveCarSTep},  	/*发送从车公交站温度信息*/
//	{87, ACT_TRACK,     TrackSpeed, 0,     	NULL},
//
//	{88, ACT_DELAY,       50,     	0,     NULL},
//	{89, ACT_CMD,         0,        0,     (void*)Command_SlaveCarSTep},  	/*发送从车公交站温度信息*/
//	{90, ACT_GO,        GoSpeed, 	10,     	NULL},
//
//	// -- 14.解析卡一位置如果在d3 d1就执行以下读卡
//	{200, ACT_JUMP_IF,        91, 	'3',     (void*)&Coordinate[1]}, //满足在d1跳转91
//	{201, ACT_JUMP_IF,        300, 	'1',     (void*)&Coordinate[1]}, //满足在d3跳转300
//	{203, ACT_JUMP,        	  110, 	  0,     NULL},	//不满足跳转110
//
//		// -- 分支1 卡在D3
//	{91, ACT_LEFT,        TurnSpeed,  0,     NULL},
//	{92, ACT_TRACKRFID,   TrackSpeed, 0,     NULL}, //读卡循迹
//	{93, ACT_GO,          GoSpeed, 	8,     NULL},
//	{94, ACT_DELAY,       800,     	0,     NULL},	/*等停稳*/
//	{95, ACT_CMD,         0,        0,     (void*)RC522Read2},   // 读取卡片结果
//	{96, ACT_DELAY,       100,     	0,     NULL},
//	{97, ACT_CMD,         0,        0,     (void*)RFID_Slove2},   //处理卡片数据
//	{98, ACT_DELAY,       200,     	0,     NULL},
//	{99, ACT_BACK,        GoSpeed, 	20,     NULL},   /*后退到十字路口再右转到白卡*/
//	{100, ACT_RIGHT,      TurnSpeed,  0,     NULL},
//	{203, ACT_JUMP,        	  110, 	  0,     NULL},	// 解卡完成，跳转发送从车启动指令
//
//		// --- 分支2 卡在D1
//	{300, ACT_RIGHT,        TurnSpeed,  0,     NULL},
//	{301, ACT_TRACKRFID,   TrackSpeed, 0,     NULL}, //读卡循迹
//	{302, ACT_GO,          GoSpeed, 	7,     	NULL},
//	{303, ACT_DELAY,       300,     	0,     NULL},	/*等停稳*/
//	{304, ACT_CMD,         0,        0,     (void*)RC522Read2},   // 读取卡片结果
//	{305, ACT_DELAY,       100,     	0,     NULL},
//	{306, ACT_CMD,         0,        0,     (void*)RFID_Slove2},   //处理卡片数据
//	{307, ACT_DELAY,       200,     	0,     NULL},
//	{308, ACT_BACK,        GoSpeed, 	17,     NULL},   /*后退到十字路口再右转到白卡*/
//	{311, ACT_LEFT,       TurnSpeed,  0,     NULL},
//
//	// --- 16. 面向B2 ---
//	{110, ACT_DELAY,      50,     	0,     NULL},
//	{111, ACT_CMD,        0,        0,     (void*)Command_SlaveCarStart},  	/*发送给从车启动信息*/
//	{112, ACT_DELAY,      50,     	0,     NULL},
//	{113, ACT_CMD,        0,        0,     (void*)Command_SlaveCarStart},  	/*发送给从车启动信息*/
//
//	// --- 17. 读卡-到B2 ---
//	{114, ACT_TRACK,      TrackSpeed, 0,     	NULL},
//	{115, ACT_CMD,        0,        0,     (void*)Command_SlaveCarStart},  	/*发送给从车启动信息*/
//	{116, ACT_GO,         GoSpeed, 	8,     	NULL},
//	{117, ACT_DELAY,      200,     	0,     NULL},	/*等停稳*/
//	{118, ACT_CMD,        0,        0,     (void*)RC522Read2},   // 读取卡片结果
//	{119, ACT_DELAY,      100,     	0,     NULL},
//	{120, ACT_CMD,        0,        0,     (void*)RFID_Slove2},   //处理卡片数据
//
//	// --- 18. 左转走到B5-道闸开不了 ---
//	{121, ACT_LEFT,       TurnSpeed,  0,     NULL},
//	{122, ACT_TRACK,      TrackSpeed, 0,     	NULL},
//	{123, ACT_GO,         GoSpeed, 	8,     	NULL},
//
//
//	// --- 19. 解析卡1是否在B5读卡-B5 ---
//	{250, ACT_JUMP_IF,        220, 	'5',     (void*)&Coordinate[1]}, //满足在d1跳转220
//	{251, ACT_JUMP,        	  130, 	  0,     NULL},	//不满足跳转130
//
//		// --- 分支3 卡在B5
//	{220, ACT_TRACKRFID,  TrackSpeed, 0,     NULL}, //读卡循迹
//	{221, ACT_GO,         GoSpeed, 	6,     	NULL},
//	{125, ACT_DELAY,      200,     	0,     NULL},	/*等停稳*/
//	{126, ACT_CMD,        0,        0,     (void*)RC522Read2},   // 读取卡片结果
//	{127, ACT_DELAY,      100,     	0,     NULL},
//	{128, ACT_CMD,        0,        0,     (void*)RFID_Slove2},   //处理卡片数据
//	{129, ACT_DELAY,      100,     	0,     NULL},
//
//	// --- 20. 倒车入库 ---
//	{130, ACT_TRACK,      TrackSpeed, 0,     	NULL},
//	{131, ACT_DELAY,      50,     	0,     NULL},
//	{132, ACT_CMD,        0,        0,     (void*)SendBat},   //发给从车卡2解密信息
//	{133, ACT_GO,         GoSpeed, 	8,     	NULL},
//	{134, ACT_DELAY,      50,     	0,     NULL},
//	{135, ACT_CMD,        0,        0,     (void*)SendBat},   //发给从车卡2解密信息
//	{136, ACT_LEFT,       TurnSpeed,  0,     NULL},
//	{137, ACT_DELAY,      50,     	0,     NULL},
//	{138, ACT_CMD,        0,        0,     (void*)SendBat},   //发给从车卡2解密信息
//	{139, ACT_TRACKMP,    TrackSpeed, 8,     	NULL},
//	{140, ACT_DELAY,      50,     	0,     NULL},
//	{141, ACT_CMD,        0,        0,     (void*)SendBat},   //发给从车卡2解密信息
//	{142, ACT_BACK,       TurnSpeed,  30,     NULL},
//
//
//
//};







/**************************************************************************
函数功能：任务解析器-任务循环调度
入口参数：无
返回  值：无
**************************************************************************/
void Task_Engine_Run(void)
{
    const TaskStep_t *step = &MyMission[StepIdx];

    // 遇到结束标志，直接返回
    if (step->Type == ACT_END){Start_Flag = 0;return;}

    switch(StepState)
    {
        // ============================================
        // 阶段 0：启动动作
        // ============================================
        case 0:
            switch(step->Type)
            {
                // --- 运动类 ---
                case ACT_GO:    		Car_Go(step->Param1, step->Param2);   		StepState = 1; break;
                case ACT_BACK:  		Car_Back(step->Param1, step->Param2); 		StepState = 1; break;
                case ACT_LEFT:  		Car_Left(step->Param1);               		StepState = 1; break;
                case ACT_RIGHT: 		Car_Right(step->Param1);              		StepState = 1; break;
                case ACT_TRACK: 		Car_Track(step->Param1);              		StepState = 1; break;
                case ACT_MPLEFT: 		Car_MPLeft(step->Param1, step->Param2); 	StepState = 1; break;
                case ACT_MPRIGHT: 		Car_MPRight(step->Param1, step->Param2); 	StepState = 1; break;
                case ACT_TRACKTIME: 	Car_TrackTime(step->Param1, step->Param2); 	StepState = 1; break;
                case ACT_TRACKMP: 		Car_TrackMp(step->Param1, step->Param2); 	StepState = 1; break;
                case ACT_TRACKRFID: 	Car_TrackRFID(step->Param1);				StepState = 1; break;

                // --- 指令类 (无等待) ---
                case ACT_CMD:
                    if(step->Ptr) {
                        void (*func)(void) = (void (*)(void))step->Ptr;
                        func();
                    }
                    StepIdx++; // 直接下一条
                    break;

                // --- 计时/等待类 ---
                case ACT_DELAY:
                case ACT_WAIT_NEQ0:
                case ACT_WAIT_VAL_EQ:
                    TimerStart = HAL_GetTick(); // 记录开始时间
                    StepState = 1;
                    break;



				// --- 跳转类 (核心更新) ---
				case ACT_JUMP:		// 无条件跳转: Param1 = 目标ID
					StepIdx = Find_Index_By_ID(step->Param1);
					// StepState 保持 0，下次循环直接执行新的一行
					break;

                case ACT_JUMP_IF:	//  --- 条件跳转: Param1 = 目标ID, Param2 = 匹配值, ArgPtr = 变量地址
                	if(step->Ptr != NULL)
					{
						uint8_t val = *(uint8_t*)(step->Ptr);
						if(val == step->Param2)
						{
							// 条件满足，跳转！
							StepIdx = Find_Index_By_ID(step->Param1);
						}
						else
						{
							// 条件不满足，下一行
							StepIdx++;
						}
					}
					else StepIdx++;
					break;



                // --- 自定义函数
                case ACT_CUSTOM_LOOP:
                	 typedef uint8_t (*LoopFunc_t)(void);
					if ( ((LoopFunc_t)step->Ptr)() == 1 )
					{
						StepIdx++; // 函数返回1说明完成了，跳下一步
						StepState = 0;
					}
                    break;


                default:
					break;
            }
            break;

        // ============================================
        // 阶段 1：等待完成
        // ============================================
        case 1:
            switch(step->Type)
            {
                // --- 运动类：等 Stop_Flag ---
                case ACT_GO:
                case ACT_BACK:
                case ACT_LEFT:
                case ACT_RIGHT:
                case ACT_TRACK:
                case ACT_MPLEFT:
                case ACT_MPRIGHT:
                case ACT_TRACKTIME:
                case ACT_TRACKMP:
                case ACT_TRACKRFID:
                    if(Stop_Flag == Task_Complete)
                    {
                        StepState = 0;
                        StepIdx++;
                    }
                    break;


                // --- 延时类：等时间 ---
                case ACT_DELAY:
                    if(HAL_GetTick() - TimerStart >= step->Param1)
                    {
                        StepState = 0;
                        StepIdx++;
                    }
                    break;

                // --- 变量等待类：等值变 或 超时 ---
                case ACT_WAIT_NEQ0:
                {
                    uint8_t *pVal = (uint8_t*)(step->Ptr);
                    // 条件：(值不为0) 或者 (超时了)
                    if( (*pVal != 0) || (HAL_GetTick() - TimerStart >= step->Param1) )
                    {
                        StepState = 0;
                        StepIdx++;
                    }
                }
                break;

                //  --- 变量等待类等待变量为指定值 或超时 (参数: 超时ms, 指定值, 变量地址)
                case ACT_WAIT_VAL_EQ:
                {
                    uint8_t *pVal = (uint8_t*)(step->Ptr);
                    // 条件：(值不为指定值) 或者 (超时了)
                    if( (*pVal == step->Param2) || (HAL_GetTick() - TimerStart >= step->Param1) )
                    {
                        StepState = 0;
                        StepIdx++;
                    }
                }
                break;

                default:
					break;

            }
            break;
    }
}

// 初始化或重置任务
void Task_Engine_Init(void)
{
    StepIdx = 0;
    StepState = 0;
}

// 【核心功能】根据 ID 查找数组下标
uint16_t Find_Index_By_ID(uint16_t target_id)
{
    uint16_t i = 0;
    while(1)
    {
        // 找到目标ID，返回下标
        if(MyMission[i].StepID == target_id) return i;

        // 遇到结束符还没找到，说明ID写错了，返回0(重头开始)或报错
        if(MyMission[i].Type == ACT_END) return 0;

        i++;
    }
}


