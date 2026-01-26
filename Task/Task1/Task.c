#include "Task.h"


/*常用速度定义*/
#define GoSpeed  		120
#define TrackSpeed  	80
#define TurnSpeed  		80

static int16_t  StepIdx = 0;    // 当前行号
static uint8_t  StepState = 0;  // 0:启动 1:等待
static uint32_t TimerStart = 0; // 计时器


uint8_t LightInit=0; /*路灯初始档位*/
uint8_t VoiceNumber=0; /*公交语音号*/

uint8_t Sector; //扇区地址 第几扇区
uint8_t Block;	//块地址
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

//从车
void SendLight(void) //发送路灯初始值
{
	Command_SlaveCarLight(LightInit);
}

void SendBat(void) //给从车发送无线充电开启码
{
	Command_SlaveCarSendBt(SendBattery[0], SendBattery[1], SendBattery[2]);
}


/*ETC闸门前的徘徊任务*/
uint8_t Action_WaitGateOpen(void)
{
	static uint8_t States=0;

	switch(States)
	{
	case 0:
		Car_TrackMp(10,10);	/*前进*/
		States = 1;
	case 1:
		if(Stop_Flag == Task_Complete)
		{
			Car_Back(0,10.5);	/*后退*/
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
			Car_TrackMp(10,11.5);	/*前进*/
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
void CommandLight(void) //获取路灯初始值
{
	LightInit = Command_LightAuto(CarPortFlag);
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
	VoiceNumber = Voice_ASR(); /*开启语音识别*/
}
void CommandCarPtB1(void)
{
	Command_CarPortB(1);
}


/*执行任务表*/
const TaskStep_t MyMission[] = {
    // 动作类型,        参数1,    参数2,   指针(函数或变量)

    // --- 1. 起步 ---
    {ACT_CMD,         0,        0,     (void*)Command_StartTim}, //开启计时
    {ACT_TRACK,       TrackSpeed, 0,     NULL}, /*循迹*/
	{ACT_GO,          GoSpeed, 8,     NULL},	/*前进*/
	{ACT_LEFT,        TurnSpeed, 0,     NULL},  /*左转面向交通灯*/
	{ACT_BACK,        100, 6,     NULL},  /*后退一点好识别*/

    // --- 2. 交通灯识别流程 ---
    {ACT_CMD,         0,        0,     (void*)Command_TrafficAInMode}, // 开启识别
    {ACT_DELAY,       1500,     0,     NULL},                          // 等1.5秒让摄像头稳定
    {ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
	{ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令2
    {ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令3

    // 【关键】等待 AndroidFlag 变动，超时 2000ms
    {ACT_WAIT_NEQ0,   2000,     0,     (void*)&AndroidFlag}, /*超时2s跳过，如果接收到AndroidFlag颜色则直接发送结果*/

    {ACT_CMD,         0,        0,     (void*)Command_TrafficASend},   // 发送结果
	{ACT_CMD,         0,        0,     (void*)Command_TrafficASend},   // 发送结果

    // --- 3. 继续跑路 ---
    {ACT_TRACK,     TrackSpeed, 0,     NULL},
    {ACT_CMD,         0,        0,     (void*)Command_GetPortBFloor}, // 发送 - 获取车库层数
    {ACT_GO,        GoSpeed, 	7,     NULL},
    {ACT_RIGHT,     TurnSpeed, 	0,     NULL},
    {ACT_CMD,         0,        0,     (void*)Command_GetPortBFloor}, // 发送 - 获取车库层数
    {ACT_TRACK,     TrackSpeed, 0,     NULL},
    {ACT_GO,        GoSpeed, 	8,     NULL},

	// --- 4. 读卡-左边是路灯 ---
	{ACT_DELAY,       100,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)RC522Read},   // 读取卡片结果
	{ACT_DELAY,       100,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)RFID_Slove},   // 处理卡片结果
	{ACT_GO,        GoSpeed, 	1,     NULL},

	// --- 5. 面向路灯 ---
	{ACT_LEFT,        TurnSpeed, 0,     NULL},  /*左转面向路灯*/
	{ACT_CMD,         0,         0,     (void*)CommandLight},   // 调节路灯
	{ACT_DELAY,       50,     	0,     NULL},

	// --- 6. 转到etc前 ---
	{ACT_RIGHT,     TurnSpeed, 	0,     NULL},
	{ACT_CMD,         0,         0,     (void*)SendLight},   // 发送给从车路灯初始档位

	{ACT_RIGHT,     TurnSpeed, 	0,     NULL},
	{ACT_DELAY,       50,     	0,     NULL},

	{ACT_CMD,         0,         0,     (void*)SendLight},   // 发送给从车路灯初始档位
	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,         0,     (void*)SendLight},   // 发送给从车路灯初始档位


	// --- 7. 过etc闸门 ---
	{ACT_CUSTOM_LOOP, 		0, 	0, 		(void*)Action_WaitGateOpen}, /*ETC前进行后退直到开启闸门*/
    {ACT_TRACK,     TrackSpeed, 0,     NULL},
	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,         0,     (void*)CommandCarPtB1},   // 下降车库
    {ACT_GO,        GoSpeed, 	7,     NULL},
	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,         0,     (void*)CommandCarPtB1},   // 下降车库
	{ACT_DELAY,       100,     0,     NULL},


	// --- 8. 读卡-前面是特殊地形 ---
	{ACT_CMD,         0,        0,     (void*)RC522Read},   // 读取卡片结果
	{ACT_DELAY,       300,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)RFID_Slove},   // 处理卡片结果

	// --- 9. 过特殊地形 ---
	{ACT_BACK,      GoSpeed, 	10,     NULL},  /*后退一点好冲刺*/
    {ACT_TRACKMP, TrackSpeed, 	13,     NULL},	/*到白卡 用码盘循迹防止过白卡过头*/
	{ACT_GO,        GoSpeed, 	58,     NULL},  /*过特殊路段*/
    {ACT_TRACK,     TrackSpeed, 0,     	NULL},
    {ACT_GO,        GoSpeed, 	7,     NULL},

	// --- 10. 读卡 ---
	{ACT_DELAY,       200,     	0,     NULL},	/*等停稳*/
	{ACT_CMD,         0,        0,     (void*)RC522Read},   // 读取卡片结果
	{ACT_DELAY,       100,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)RFID_Slove},   // 处理卡片结果
	{ACT_DELAY,       100,     	0,     NULL},
    {ACT_GO,        GoSpeed, 	2,     NULL},


	// --- 11. TFT颜色形状识别 ---
	{ACT_DELAY,       1500,     	0,     NULL}, /*等1.5s摄像头稳定*/
	{ACT_CMD,         0,        0,     (void*)Command_Androidshape},   // 识别形状
	{ACT_DELAY,       300,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)Command_Androidshape},   // 识别形状
	{ACT_DELAY,       300,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)Command_AndroidColor},   // 识别颜色
	{ACT_DELAY,       300,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)Command_AndroidColor},   // 识别颜色
	{ACT_DELAY,       500,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)Command_sendshap},   	   // 发送识别的形状
	{ACT_DELAY,       500,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)Command_sendcolor},  	   // 发送识别的颜色
	{ACT_DELAY,       500,     	0,     NULL},


	// --- 12. 到公交站请求回传温度和进行语音识别 ---
	{ACT_CMD,         0,        0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
	{ACT_LEFT,        TurnSpeed, 0,     NULL},

	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
	{ACT_DELAY,       100,     	0,     NULL},

	{ACT_TRACK,     TrackSpeed, 0,     	NULL},
	{ACT_GO,        GoSpeed, 	16,     NULL},

	{ACT_CMD,         0,        0,     (void*)Command_BusReportRandom},  	/*开启交站语音*/
	{ACT_CMD,         0,        0,     (void*)Command_BusReportRandom},  	/*开启交站语音*/
	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)GetVoicNum},  	/*开启语音识别*/

	// --- 13. 识别完成，来到D2 ---
	{ACT_DELAY,       1000,     	0,  NULL},

	{ACT_CMD,         0,        0,     (void*)Command_SlaveCarSTep},  	/*发送给从车公交站温度信息*/
	{ACT_LEFT,        TurnSpeed, 0,     NULL},

	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)Command_SlaveCarSTep},  	/*发送从车公交站温度信息*/
	{ACT_TRACK,     TrackSpeed, 0,     	NULL},

	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)Command_SlaveCarSTep},  	/*发送从车公交站温度信息*/
	{ACT_GO,        GoSpeed, 	8,     	NULL},

	// --- 14. 识别完成，来到D2 ---
	{ACT_LEFT,        TurnSpeed,  0,     NULL},
	{ACT_TRACKMP,     TrackSpeed, 28,     NULL},


	// --- 15. 前后读卡 ---
	{ACT_DELAY,       100,     	0,     NULL},	/*等停稳*/
	{ACT_CMD,         0,        0,     (void*)RC522Read2},   // 读取卡片结果
	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)RC522Read2},   // 读取卡片结果
	{ACT_DELAY,       100,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)RFID_Slove2},   //处理卡片数据
	{ACT_DELAY,       500,     	0,     NULL},

	{ACT_BACK,        GoSpeed, 	42,    NULL},
	{ACT_DELAY,       200,     	0,     NULL},	/*等停稳*/
	{ACT_CMD,         0,        0,     (void*)RC522Read2},   // 读取卡片结果
	{ACT_DELAY,       100,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)RFID_Slove2},   //处理卡片数据
	{ACT_DELAY,       400,     	0,     NULL},

	// --- 16. 面向B2 ---
	{ACT_TRACK,     TrackSpeed, 0,     	NULL},
	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)Command_SlaveCarStart},  	/*发送给从车启动信息*/
	{ACT_GO,        GoSpeed, 	5,     	NULL},
	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)Command_SlaveCarStart},  	/*发送给从车启动信息*/
	{ACT_RIGHT,        TurnSpeed, 0,     NULL},

	// --- 17. 读卡-到b2 ---
	{ACT_TRACK,     TrackSpeed, 0,     	NULL},
	{ACT_GO,        GoSpeed, 	8,     	NULL},
	{ACT_DELAY,       200,     	0,     NULL},	/*等停稳*/
	{ACT_CMD,         0,        0,     (void*)RC522Read2},   // 读取卡片结果
	{ACT_DELAY,       100,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)RFID_Slove2},   //处理卡片数据

	// --- 18. 左转走到b5-道闸开不了 ---
	{ACT_LEFT,        TurnSpeed,  0,     NULL},
	{ACT_TRACK,       TrackSpeed, 0,     	NULL},
	{ACT_GO,          GoSpeed, 	  8,     	NULL},
	{ACT_TRACKMP,     TrackSpeed, 27,     	NULL},

	// --- 19. 读卡-b5 ---
	{ACT_DELAY,       200,     	0,     NULL},	/*等停稳*/
	{ACT_CMD,         0,        0,     (void*)RC522Read2},   // 读取卡片结果
	{ACT_DELAY,       100,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)RFID_Slove2},   //处理卡片数据
	{ACT_DELAY,       100,     	0,     NULL},


	// --- 19. 倒车入库 ---
	{ACT_TRACK,       TrackSpeed, 0,     	NULL},
	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)SendBat},   //处理卡片数据

	{ACT_GO,          GoSpeed, 	  8,     	NULL},
	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)SendBat},   //处理卡片数据
	{ACT_LEFT,        TurnSpeed,  0,     NULL},
	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)SendBat},   //处理卡片数据
	{ACT_TRACKMP,     TrackSpeed, 8,     	NULL},
	{ACT_DELAY,       50,     	0,     NULL},
	{ACT_CMD,         0,        0,     (void*)SendBat},   //处理卡片数据
	{ACT_BACK,        TurnSpeed,  30,     NULL},



};







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
                    TimerStart = HAL_GetTick(); // 记录开始时间
                    StepState = 1;
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





