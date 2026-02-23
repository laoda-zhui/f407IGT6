#include "Task.h"


/*常用速度定义*/
#define GoSpeed  		120
#define TrackSpeed  	77
#define TurnSpeed  		77
#define MPTurnSpeed 	30

static uint16_t  StepIdx = 0;    // 当前行号
static uint8_t  StepState = 0;  // 0:启动 1:等待
static uint32_t TimerStart = 0; // 计时器
static uint32_t TimerStart2 = 0; // 计时器


/*以下为任务表模板*/

/**********************************************************************************************
 * 1.循迹白卡 读白卡 解密白卡 (十字路口 黑线路上都合适)
{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
{0, 	ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适

{0, 	ACT_DELAY,       	 800,     	0,     NULL},
{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_4ReadRF1},

//后退再来一遍
{0, 		ACT_BACK,       	0, 	 	  15,     NULL},	//后退
{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
{0, 		ACT_DELAY,       	 800,     	0,     NULL},
{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_4ReadRF1},

//解密1.....
{0, 		ACT_DELAY,       	 800,     	0,     NULL},
{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_4SloveRF1},


**********************************************************************************************/






/**********************************************************************************************
 * 2.颜色和形状识别
{0, 		ACT_DELAY,       	 2000,     	0,     NULL}, //等摄像稳定
{0, 		ACT_CMD,         	   0,        0,     (void*)Command_AndroidColor},   // 颜色识别
{0, 		ACT_DELAY,       	 800,     	0,     NULL},
{0, 		ACT_CMD,         	   0,        0,     (void*)Command_AndroidColor},   // 颜色识别
{0, 		ACT_DELAY,       	 800,     	0,     NULL},
{0, 		ACT_CMD,         	   0,        0,     (void*)Command_Androidshape},   // 形状识别
{0, 		ACT_DELAY,       	 800,     	0,     NULL},
{0, 		ACT_CMD,         	   0,        0,     (void*)Command_Androidshape},   // 形状识别
{0, 		ACT_DELAY,       	 1000,     	0,     NULL},
**********************************************************************************************/

/**********************************************************************************************
 * 3.特殊地形 有白卡(黑线上 非十字路口)
//有白卡版
{0, 		ACT_GO,          	GoSpeed, 	77,     NULL},
{0, 		ACT_TRACKRFID,       TrackSpeed, 	0,     NULL}, //如出地形就出现白卡就用白卡循迹

//无白卡版
{0, 		ACT_GO,          	GoSpeed, 	77,     NULL},
{0, 		ACT_TRACKMP,      	TrackSpeed, 	12,     NULL},   //码盘循迹
{0, 		ACT_BACK,          	GoSpeed, 	 	11,     NULL},	//后退
{0, 		ACT_TRACKMP,      	TrackSpeed, 	12,     NULL},   //码盘循迹
{0, 		ACT_BACK,          	GoSpeed, 	 	7,     NULL},	//后退
{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
**********************************************************************************************/



/**********************************************************************************************
 * 4.红绿灯识别 (交通灯B)
{0, 		ACT_DELAY,       		1000,     		0,     NULL}, //等停稳
{0,   		ACT_RESET_TIMER,   		0,    			0,      NULL}, //记录当前时刻
{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficBInMode}, // 开启识别
{0, 		ACT_DELAY,       		50,     		 0,     NULL},
{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficBInMode}, // 开启识别

{55555, 	ACT_JUMP_IF,        	55556, 			1,     (void*)&AndroidFlag}, //满足红灯 则跳转到55556
{0, 		ACT_JUMP_IF,        	55556, 			2,     (void*)&AndroidFlag}, //满足黄灯 则跳转到55556
{0, 		ACT_JUMP_IF,        	55556, 			3,     (void*)&AndroidFlag}, //满足绿灯 则跳转到55556
{0, 		ACT_DELAY,       		600,     		0,     NULL}, //等600ms
{0, 		ACT_CMD,        		  0,        	0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
{0,   		ACT_CHECK_TIMEOUT, 		55557,  		8000,     NULL},  //检查是否超时 如果超时10s则跳转到55557
{0, 		ACT_JUMP,        	  	55555, 	  		0,     	NULL},	//不满足跳转55555无条件跳转)



{55556, 	ACT_DELAY,       	  50,     	 	0,     NULL},
{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficBSend},   // 发送结果
{0, 		ACT_DELAY,       	  50,     	 	0,     NULL},
{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficBSend},   // 发送结果

{55557, 	ACT_DELAY,       	  50,     	 0,     NULL}, //10s内没识别到 发送红色
{0, 		ACT_CMD,         	   0,        0,     (void*)SendTrafficBRed},   // 发送结果

**********************************************************************************************/



/*********************************************************************************************
 * 5.开始计时-并且放下车库到第一层 接着等22s后获取车库层数 如果收到层数为1就启动
{0, 		ACT_DELAY,       	100,     	0,     NULL},
{0, 		ACT_CMD,         	0,       	0,     (void*)Command_StartTim},  	   // 开始计时
{0, 		ACT_CMD,         	0,       	0,     (void*)Command_StartTim},  	   // 开始计时
{0, 		ACT_DELAY,       	100,     	0,     NULL},
{0, 		ACT_CMD,         	0,        	0,     (void*)CommandCarPtB1},  	   // 降车库
{0, 		ACT_DELAY,       	200,     	0,     NULL},
{0, 		ACT_CMD,         	0,        	0,     (void*)CommandCarPtB1},  	   // 降车库
{0, 		ACT_DELAY,       	22000,     	0,     NULL}, //等22s下降
{0, 		ACT_CMD,         	0,        	0,     (void*)Command_GetPortBFloor},  	   // 获取车库层数
{0, 		ACT_DELAY,       	100,     	0,     NULL},
{0, 		ACT_CMD,         	0,        	0,     (void*)Command_GetPortBFloor},  	   // 获取车库层数
{0, 		ACT_DELAY,       	100,     	0,     NULL},
{0, 		ACT_CMD,         	0,        	0,     (void*)Command_GetPortBFloor},  	   // 获取车库层数
{0, 		ACT_WAIT_VAL_EQ,   	99999,    	1,     (void*)&CarPortFlag}, 	//超时99s跳过(保险),只有接收到车库层数为1才走
**********************************************************************************************/



/*********************************************************************************************
 * 6.超声波回传以及二维码扫描
//测距静态标志位物:..........
{0, 		ACT_DELAY,       	2000,     	0,     NULL},	//等停稳
{0, 		ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上 并缓存一次Dis到DisBuf
{0, 		ACT_DELAY,       	50,     	0,     NULL},
{0, 		ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上
{0, 		ACT_GO,          	GoSpeed, 	 	7,     NULL},	//前进

//二维码扫描静态标志物:..........
{0, 		ACT_DELAY,       	2000,     	0,     NULL},
{0, 		ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
{0, 		ACT_DELAY,       	500,     	0,     NULL},
{0, 		ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描


//第二版
//测距静态标志位物:..........
{0, 		ACT_DELAY,       	2000,     	0,     NULL},	//等停稳
{0, 		ACT_CMD,         	0,        0,     (void*)Command_SaveDis}, // 缓存一次Dis到DisBuf
{0, 		ACT_DELAY,       	50,     	0,     NULL},
{0, 		ACT_CMD,         	0,        0,     (void*)Command_SaveDis}, // 缓存一次Dis到DisBuf
{0, 		ACT_DELAY,       	50,     	0,     NULL},
{0, 		ACT_CMD,         	0,        0,     (void*)Command_SaveDis}, // 缓存一次Dis到DisBuf

{0, 		ACT_GO,          	GoSpeed, 	 	7,     NULL},	//前进

//二维码扫描静态标志物:..........
{0, 		ACT_DELAY,       	2000,     	0,     NULL},
{0, 		ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
{0, 		ACT_DELAY,       	500,     	0,     NULL},
{0, 		ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
{0, 		ACT_BACK,          	GoSpeed, 	 	25,     NULL},	//后退
{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
**********************************************************************************************/


/*********************************************************************************************
 * 7.倒车入库
{0, 		ACT_TRACKMP,       TrackSpeed, 		24,     NULL},	//循迹
{0, 		ACT_BACK,          GoSpeed, 	 	59,     NULL},	//后退

{0, 		ACT_TRACKMP,      	TrackSpeed, 	25,     NULL},   //码盘循迹
{0, 		ACT_BACK,          	GoSpeed, 	 	70,     NULL},	//后退

**********************************************************************************************/


/*********************************************************************************************
 * 8.跳转模板
{201, ACT_JUMP_IF,        300, 	'1',     (void*)&Coordinate[1]}, //满足在d3跳转300  Coordinate[1]='1'跳转300
{203, ACT_JUMP,        	  110, 	  0,     NULL},	//不满足跳转110(无条件跳转)
**********************************************************************************************/



/*********************************************************************************************
 * 9.等从车给我发指定车库-微循环跳转模板
{0,   		ACT_RESET_TIMER,   		0,    		0,      NULL}, //记录当前时刻
{600, 		ACT_JUMP_IF,        	300, 		1,     (void*)&SlaveCarData.Location}, //满足在d3跳转300
{700, 		ACT_JUMP_IF,        	350, 		2,     (void*)&SlaveCarData.Location}, //满足在d3跳转350
{800, 		ACT_JUMP_IF,        	400, 		3,     (void*)&SlaveCarData.Location}, //满足在d3跳转400
{600, 		ACT_JUMP_IF,        	450, 		4,     (void*)&SlaveCarData.Location}, //满足在d3跳转450
{0,   		ACT_CHECK_TIMEOUT, 		400,  	 60000,   NULL},  //检查是否超时 如果超时60s则跳转到400
{0,   		ACT_JUMP,          		600,  		0,      NULL}, //无条件跳转600
**********************************************************************************************/



/*********************************************************************************************
 * 10.ETC闸门前后徘徊-循环模板
 *
{9998, 		ACT_TRACKMP,       			35, 		  	 11.6,     		NULL},	//循迹
{0, 		ACT_BACK,          		 	0, 	 	    	10.4,     		NULL},	//后退
{0, 		ACT_JUMP_IF,        		9999, 			GateOpen,     	(void*)&GateFlag}, //满足GateFlag == GateOpen,跳转9999
{0,   		ACT_JUMP,          			9998,  			0,      		NULL}, //无条件跳转9998
{9999, 		ACT_TRACK,       			TrackSpeed, 	0,     			NULL},
{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
**********************************************************************************************/




/*********************************************************************************************
 * 11.公交站任务模板

{0, 		ACT_GO,          	GoSpeed, 	 	4,     NULL},	//前进
{0, 		ACT_TRACKMP,      	TrackSpeed, 	11,     NULL},   //码盘循迹


{0, 	ACT_DELAY,       	50,     	0,     NULL},
{0, 	ACT_CMD,         	0,        	0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度

{0, 	ACT_DELAY,       50,     0,     NULL},
{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
{0, 	ACT_DELAY,       50,     0,     NULL},
{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
{0, 	ACT_DELAY,       50,     0,     NULL},
{0, 	ACT_CMD,         0,        0,     (void*)GetVoicNum}, // 识别公交语音以及获取编号
{0, 	ACT_DELAY,       100,     0,     NULL},
{0, 	ACT_CMD,         0,        0,     (void*)SendVoiceNum}, // 发送语音编号
{0, 	ACT_DELAY,       100,     0,     NULL},
{0, 	ACT_CMD,         0,        0,     (void*)SendVoiceNum}, // 发送语音编号
**********************************************************************************************/



/*********************************************************************************************
 * 12.路灯调节任务模板
 *
{0, 	ACT_DELAY,       	100,     	0,     NULL},
{0, 	ACT_CMD,         	0,        0,     (void*)SendToSlaveLight}, // 路灯调档位

{0, 	ACT_DELAY,       	100,     	0,     NULL},
{0, 	ACT_CMD,         	0,        0,     (void*)SendToSlaveLight}, // 发送档位给从车
{0, 	ACT_DELAY,       	50,     	0,     NULL},
{0, 	ACT_CMD,         	0,        0,     (void*)SendToSlaveLight}, // 发送档位给从车
**********************************************************************************************/


/*********************************************************************************************
 * 13.发送从车启动
{0, 	ACT_DELAY,       	50,     	0,     NULL},
{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
{0, 	ACT_DELAY,       	50,     	0,     NULL},
{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
{0, 	ACT_DELAY,       	50,     	0,     NULL},
{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
{0, 	ACT_DELAY,       	50,     	0,     NULL},
{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
**********************************************************************************************/




/*********************************************************************************************
 * 14.发送LED开始计时
{0, 	ACT_DELAY,       	50,     	0,     NULL},
{0, 	ACT_CMD,         	0,       		0,     (void*)Command_StartTim},  	   // 开始计时
{0, 	ACT_DELAY,       	50,     	0,     NULL},
{0, 	ACT_CMD,         	0,       		0,     (void*)Command_EndTim},  	   // 关闭计时
**********************************************************************************************/




/*********************************************************************************************
 * 15.车牌识别
{0, 	ACT_DELAY,       	1000,     	0,     NULL}, 		//delay
{0, 	ACT_CMD,         	0,       		0,     (void*)Command_AndroidPlatNum}, //车牌识别
**********************************************************************************************/




/**********************************************************************************************
 * 16.升车库
{0, 	ACT_DELAY,       	100,     	0,     NULL},
{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_3SendCPortb},
{0, 	ACT_DELAY,       	100,     	0,     NULL},
{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_3SendCPortb},
**********************************************************************************************/





/**********************************************************************************************
 * 17.刚好车屁股前进到黑线
{0, 		ACT_GO,          	GoSpeed, 	 	4,     NULL},	//前进
{0, 		ACT_TRACKMP,      	TrackSpeed, 	17,     NULL},   //码盘循迹
**********************************************************************************************/




/**********************************************************************************************
 * 18.立体显示
{0, 		ACT_DELAY,       		800,     		 0,     NULL},
{0, 		ACT_MPLEFT,         MPTurnSpeed, 	70,     NULL},	//码盘左转(45°)

{0, 		ACT_DELAY,       	100,     	0,     NULL},
{0, 		ACT_CMD,         	0,        	0,     (void*)showDis3D},
{0, 		ACT_DELAY,       	100,     	0,     NULL},
{0, 		ACT_CMD,         	0,        	0,     (void*)showDis3D},

{0, 		ACT_DELAY,       		800,     		 0,     NULL},
{0, 		ACT_MPRIGHT,        MPTurnSpeed, 	74,     NULL},	//码盘右转(45°)
**********************************************************************************************/




/**********************************************************************************************
 * 19.开启无线充电
{0, 	ACT_DELAY,       	50,     	0,     NULL},
{0, 	ACT_CMD,         	0,       		0,     (void*)Command_WireOpen},  	   // 开启无线充电
{0, 	ACT_DELAY,       	50,     	0,     NULL},
{0, 	ACT_CMD,         	0,       		0,     (void*)Command_WireOpen},  	   // 开启无线充电
**********************************************************************************************/




/*********************************************************************************************
 * 20.等待从车给我车牌号，发送车牌号开启道闸
{0,   		ACT_RESET_TIMER,   		0,    		0,      NULL}, //记录当前时刻
{1110, 		ACT_JUMP_IF_NOT,        1111, 		0,     (void*)&SlaveCarData.chepai[0]}, //满足不等于0跳转1111
{0,   		ACT_CHECK_TIMEOUT, 		1112,  	 15000,   NULL},  //检查是否超时 如果超时15s则跳转到1112
{0,   		ACT_JUMP,          		1110,  		0,      NULL}, //无条件跳转600


{1111, 	ACT_DELAY,       	800,     	0,     NULL}, 		//delay
{0, 	ACT_CMD,         	0,       		0,     (void*)Command_SetGateTop2}, //车牌前三位
{0, 	ACT_DELAY,       	800,     	0,     NULL}, 		//delay
{0, 	ACT_CMD,         	0,       		0,     (void*)Command_SetGateLast2}, //车牌后三位

{1112, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
**********************************************************************************************/



/*********************************************************************************************
 * 21.放立体车库(根据车库层数动态等待)
{0, 		ACT_DELAY,       	100,     	0,     NULL},
{0, 		ACT_CMD,         	0,        0,     (void*)Command_GetPortBFloor}, //获取车库层数
{0, 		ACT_DELAY,       	200,     	0,     NULL},
{0, 		ACT_CMD,         	0,        0,     (void*)Command_GetPortBFloor},//获取车库层数
{0, 		ACT_DELAY,       	200,     	0,     NULL},
{0, 		ACT_CMD,         	0,        0,     (void*)Command_GetPortBFloor},//获取车库层数
{0, 		ACT_DELAY,       	500,     	0,     NULL},

//降车库
{0, 		ACT_DELAY,       	100,     	0,     NULL},
{0, 		ACT_CMD,         	0,        0,     (void*)Command_CarPortBTo1}, //降车库
{0, 		ACT_DELAY,       	100,     	0,     NULL},
{0, 		ACT_CMD,         	0,        0,     (void*)Command_CarPortBTo1},//降车库


//动态响应
{0,   		ACT_RESET_TIMER,   		0,    		0,      NULL}, //记录当前时刻
{33333, 	ACT_JUMP_IF,        	33337, 		CarportB1,     (void*)&CarPortFlag}, //满足跳转33334
{0, 		ACT_JUMP_IF,        	33334, 		CarportB2,     (void*)&CarPortFlag}, //满足跳转33334
{0, 		ACT_JUMP_IF,        	33335, 		CarportB3,     (void*)&CarPortFlag}, //满足跳转33335
{0, 		ACT_JUMP_IF,        	33336, 		CarportB4,     (void*)&CarPortFlag}, //满足跳转33336
{0,   		ACT_CHECK_TIMEOUT, 		33337,  	 22000,   NULL},  //检查是否超时 如果超时22s则跳转到33337
{0,   		ACT_JUMP,          		33333,  		0,      NULL}, //无条件跳转33333


//2层
{33334, 	ACT_DELAY,       	8000,     	0,     NULL},
{0,   		ACT_JUMP,           33337,  		0,      NULL}, //无条件跳转33333

//3层
{33335, 	ACT_DELAY,       	15000,     	0,     NULL},
{0,   		ACT_JUMP,           33337,  		0,      NULL}, //无条件跳转33333

//4层
{33336, 	ACT_DELAY,       	22000,     	0,     NULL},
{0,   		ACT_JUMP,           33337,  		0,      NULL}, //无条件跳转33333

{33337, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
{0, 		ACT_TRACKMP,      	TrackSpeed, 	25,     NULL},   //码盘循迹
{0, 		ACT_BACK,          	GoSpeed, 	 	70,     NULL},	//后退

// --- 15.升车库到指定层数
{0, 	ACT_DELAY,       	100,     	0,     NULL},
{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_7SendCPortb2},
**********************************************************************************************/



/*********************************************************************************************
 * 30.基本任务 前进 循迹 寻卡循迹 后退 左转 右转
{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
{0, 		ACT_TRACKMP,      	TrackSpeed, 	10,     NULL},   //码盘循迹
{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
{0, 		ACT_BACK,          	GoSpeed, 	 	8,     NULL},	//后退
{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转

{0, 		ACT_DELAY,       		800,     		 0,     NULL},
{0, 		ACT_MPRIGHT,       	MPTurnSpeed, 	92,     NULL},	//码盘右转(90°)
{0, 		ACT_DELAY,       		800,     		 0,     NULL},
{0, 		ACT_MPLEFT,        	MPTurnSpeed, 	92,     NULL},	//码盘左转(90°)

{0, 		ACT_DELAY,       		800,     		 0,     NULL},
{0, 		ACT_MPRIGHT,        MPTurnSpeed, 	63,     NULL},	//码盘右转(45°)
{0, 		ACT_DELAY,       		800,     		 0,     NULL},
{0, 		ACT_MPLEFT,         MPTurnSpeed, 	66,     NULL},	//码盘左转(45°)

{0, 		ACT_DELAY,       	50,     	0,     NULL}, 		//delay
{0, 		ACT_CMD,         	0,       		0,     (void*)Command_StartTim}, //命令
**********************************************************************************************/


/*以上为任务表模板*/



/*执行任务表8 24年样题8*/
const TaskStep_t MyMission[] = {

		// --- 1.计时出库
		{0, 		ACT_CMD,         	0,       		0,     (void*)Command_StartTim},  	   // 开始计时
		{0, 		ACT_DELAY,       	50,     	0,     NULL},
		{0, 		ACT_CMD,         	0,       		0,     (void*)Command_StartTim},  	   // 开始计时
		{0, 		ACT_DELAY,       	100,     	0,     NULL},
		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转

		// --- 2.交通灯b识别
		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	2,     NULL},	//前进


		{0, 		ACT_DELAY,       		1000,     		0,     NULL}, //等停稳
		{0,   		ACT_RESET_TIMER,   		0,    			0,      NULL}, //记录当前时刻
		{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficBInMode}, // 开启识别
		{0, 		ACT_DELAY,       		50,     		 0,     NULL},
		{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficBInMode}, // 开启识别

		{55555, 	ACT_JUMP_IF,        	55556, 			1,     (void*)&AndroidFlag}, //满足红灯 则跳转到55556
		{0, 		ACT_JUMP_IF,        	55556, 			2,     (void*)&AndroidFlag}, //满足黄灯 则跳转到55556
		{0, 		ACT_JUMP_IF,        	55556, 			3,     (void*)&AndroidFlag}, //满足绿灯 则跳转到55556
		{0, 		ACT_DELAY,       		600,     		0,     NULL}, //等600ms
		{0, 		ACT_CMD,        		  0,        	0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
		{0,   		ACT_CHECK_TIMEOUT, 		55557,  		8000,     NULL},  //检查是否超时 如果超时10s则跳转到55557
		{0, 		ACT_JUMP,        	  	55555, 	  		0,     	NULL},	//不满足跳转55555无条件跳转)



		{55556, 	ACT_DELAY,       	  50,     	 	0,     NULL},
		{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficBSend},   // 发送结果
		{0, 		ACT_DELAY,       	  50,     	 	0,     NULL},
		{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficBSend},   // 发送结果

		{55557, 	ACT_DELAY,       	  50,     	 0,     NULL}, //10s内没识别到 发送红色
		{0, 		ACT_CMD,         	   0,        0,     (void*)SendTrafficBRed},   // 发送结果


		// ---3.二维码和测距
		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
		{0, 		ACT_BACK,          	GoSpeed, 	 	18,     NULL},	//后退
		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //码盘循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	2,     NULL},	//前进
		{0, 		ACT_TRACKMP,      	TrackSpeed, 	9,     NULL},   //码盘循迹
		//识别
		//测距静态标志位物:..........
		{0, 		ACT_DELAY,       	2000,     	0,     NULL},	//等停稳
		{0, 		ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上 并缓存一次Dis到DisBuf
		{0, 		ACT_DELAY,       	50,     	0,     NULL},
		{0, 		ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上

		//二维码扫描静态标志物:..........
		{0, 		ACT_TRACKMP,          	GoSpeed, 	 	9,     NULL},	//前进
		{0, 		ACT_DELAY,       	2000,     	0,     NULL},
		{0, 		ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
		{0, 		ACT_DELAY,       	500,     	0,     NULL},
		{0, 		ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描


		// ---4.路灯

		{0, 		ACT_BACK,          	GoSpeed, 	 	30,     NULL},	//后退
		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
		{0, 		ACT_GO,          	GoSpeed, 	 	4,     NULL},	//前进
		{0, 		ACT_TRACKMP,      	TrackSpeed, 	12,     NULL},   //码盘循迹

		//调节
		{0, 	ACT_DELAY,       	500,     	0,     NULL},
		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_8SendGetLight}, // 路灯调档位


		{0, 		ACT_BACK,          	GoSpeed, 	 	35,     NULL},	//后退
		{0, 		ACT_DELAY,       	50,     	0,     NULL},
		{0, 		ACT_CMD,         	0,        0,     (void*)SendToSlaveLight},
		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_DELAY,       	50,     	0,     NULL},
		{0, 		ACT_CMD,         	0,        0,     (void*)SendToSlaveLight},
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_DELAY,       	50,     	0,     NULL},
		{0, 		ACT_CMD,         	0,        0,     (void*)SendToSlaveLight},
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转

		// ---5.读卡1
		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
		{0, 	ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适

		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_8ReadRF1},

		//后退再来一遍
		{0, 		ACT_BACK,       	0, 	 	  22,     NULL},	//后退
		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_8ReadRF1},

		//解密1.....
		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_8SloveRF1},

		//后退再来一遍
		{0, 		ACT_BACK,       	0, 	 	  22,     NULL},	//后退
		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_8ReadRF2},



		//后退再来一遍
		{0, 		ACT_BACK,       	0, 	 	  22,     NULL},	//后退
		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_8ReadRF2},

		//解密1.....
		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_8SloveRF2},


		// --- 6.过特殊地形
		{0, 		ACT_GO,          	GoSpeed, 	70,     NULL},


		// --- 7.读卡2
		{0, 		ACT_TRACK,       TrackSpeed, 	0,     NULL},
		{0, 		ACT_GO,          	GoSpeed, 	3,     NULL},
		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转

		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
		{0, 	ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_8ReadRF1},

		//后退再来一遍
		{0, 		ACT_BACK,       	0, 	 	  22,     NULL},	//后退
		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_8ReadRF1},

		//解密1.....
		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_8SloveRF1},

		//后退再来一遍
		{0, 		ACT_BACK,       	0, 	 	  22,     NULL},	//后退
		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_8ReadRF2},

		//后退再来一遍
		{0, 		ACT_BACK,       	0, 	 	  22,     NULL},	//后退
		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_8ReadRF2},

		//解密2.....
		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_8SloveRF2},


		{0, 		ACT_BACK,       	0, 	 	  25,     NULL},	//后退
		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转


		// --- 8.智能交通语音

		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度

		{0, 	ACT_DELAY,       50,     0,     NULL},
		{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
		{0, 	ACT_DELAY,       50,     0,     NULL},
		{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
		{0, 	ACT_DELAY,       50,     0,     NULL},
		{0, 	ACT_CMD,         0,        0,     (void*)GetVoicNum}, // 识别公交语音以及获取编号
		{0, 	ACT_DELAY,       100,     0,     NULL},
		{0, 	ACT_CMD,         0,        0,     (void*)SendVoiceNum}, // 发送语音编号
		{0, 	ACT_DELAY,       100,     0,     NULL},
		{0, 	ACT_CMD,         0,        0,     (void*)SendVoiceNum}, // 发送语音编号


		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转

		// --- 9.ETC
		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转

		{9998, 		ACT_TRACKMP,       			35, 		  	 11.6,     		NULL},	//循迹
		{0, 		ACT_BACK,          		 	0, 	 	    	10.4,     		NULL},	//后退
		{0, 		ACT_JUMP_IF,        		9999, 			GateOpen,     	(void*)&GateFlag}, //满足GateFlag == GateOpen,跳转9999
		{0,   		ACT_JUMP,          			9998,  			0,      		NULL}, //无条件跳转9998
		{9999, 		ACT_TRACK,       			TrackSpeed, 	0,     			NULL},
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转

		// --- 10.发送从车启动
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},

		// --- 11.等从车给我发停车库地址
		{0,   		ACT_RESET_TIMER,   		0,    		0,      NULL}, //记录当前时刻
		{5000, 		ACT_JUMP_IF,        	3000, 		1,     (void*)&SlaveCarData.Location}, //满足在d7跳转300
		{0, 		ACT_JUMP_IF,        	3500, 		2,     (void*)&SlaveCarData.Location}, //满足在f7跳转350
		{0, 		ACT_JUMP_IF,        	4000, 		3,     (void*)&SlaveCarData.Location}, //满足在g6跳转400
		{0, 		ACT_JUMP_IF,        	4500, 		4,     (void*)&SlaveCarData.Location}, //满足在g4跳转450
		{0,   		ACT_CHECK_TIMEOUT, 		4000,  	 40000,   NULL},  //检查是否超时 如果超时60s则跳转到4000
		{0,   		ACT_JUMP,          		5000,  		0,      NULL}, //无条件跳转5000

		//D7
		{3000, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
		{0, 		ACT_TRACKMP,      	TrackSpeed, 	25,     NULL},   //码盘循迹
		{0, 		ACT_BACK,          	GoSpeed, 	 	59,     NULL},	//后退
		{0,   		ACT_JUMP,          		6000,  		0,      NULL}, //无条件跳转600

		//F7
		{3500, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
		{0, 		ACT_TRACKMP,      	TrackSpeed, 	25,     NULL},   //码盘循迹
		{0, 		ACT_BACK,          	GoSpeed, 	 	59,     NULL},	//后退
		{0,   		ACT_JUMP,          		6000,  		0,      NULL}, //无条件跳转600
		{0,   		ACT_JUMP,          		6000,  		0,      NULL}, //无条件跳转600

		//G6
		{4000, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
		{0, 		ACT_TRACKMP,      	TrackSpeed, 	25,     NULL},   //码盘循迹
		{0, 		ACT_BACK,          	GoSpeed, 	 	59,     NULL},	//后退
		{0,   		ACT_JUMP,          		6000,  		0,      NULL}, //无条件跳转600

		//G4
		{4500, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
		{0, 		ACT_TRACKMP,      	TrackSpeed, 	25,     NULL},   //码盘循迹
		{0, 		ACT_BACK,          	GoSpeed, 	 	59,     NULL},	//后退
		{0,   		ACT_JUMP,          		6000,  		0,      NULL}, //无条件跳转600

		// --- 12.停止计时
		{6000, 	ACT_DELAY,       	100,     	0,     NULL},
		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_EndTim},  	   // 关闭计时
		{0, 	ACT_DELAY,       	100,     	0,     NULL},
		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_EndTim},  	   // 关闭计时
		{0, 	ACT_DELAY,       	100,     	0,     NULL},
		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_WireOpen},  	   // 关闭计时
		{0, 	ACT_DELAY,       	100,     	0,     NULL},
		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_WireOpen},  	   // 关闭计时
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         	0,       		0,     (void*)Ti24_8SendWife},  	   // 关闭计时
		{0, 	ACT_DELAY,       	50,     	0,     NULL},
		{0, 	ACT_CMD,         	0,       		0,     (void*)Ti24_8SendWife},  	   // 关闭计时

};

/*执行任务表7 24年样题7*/
//const TaskStep_t MyMission[] = {
//
////		// --- 1.计时出库
//		{0, 		ACT_DELAY,       	50,     	0,     NULL}, 		//delay
//		{0, 		ACT_CMD,         	0,       	0,     (void*)Command_StartTim},  	   // 开始计时
//		{0, 		ACT_DELAY,       	50,     	0,     NULL}, 		//delay
//		{0, 		ACT_CMD,         	0,       	0,     (void*)Command_StartTim},  	   // 开始计时
//		{0, 		ACT_DELAY,       	50,     	0,     NULL}, 		//delay
//		// --- 2.开启无线充电标志物
//		{0, 		ACT_CMD,         	0,       	0,     (void*)Command_WireOpen},  	   // 开启
//		{0, 		ACT_DELAY,       	50,     	0,     NULL}, 		//delay
//		// --- 3.交通灯A识别
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	20,     NULL},   //码盘循迹
//		{0, 		ACT_BACK,          	GoSpeed, 	 	27,     NULL},	//后退
//
//		{0, 		ACT_DELAY,       		1000,     		0,     NULL}, //等停稳
//		{0,   		ACT_RESET_TIMER,   		0,    			0,      NULL}, //记录当前时刻
//		{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficAInMode}, // 开启识别
//		{0, 		ACT_DELAY,       		50,     		 0,     NULL},
//		{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficAInMode}, // 开启识别
//
//		{55555, 	ACT_JUMP_IF,        	55556, 			1,     (void*)&AndroidFlag}, //满足红灯 则跳转到55556
//		{0, 		ACT_JUMP_IF,        	55556, 			2,     (void*)&AndroidFlag}, //满足黄灯 则跳转到55556
//		{0, 		ACT_JUMP_IF,        	55556, 			3,     (void*)&AndroidFlag}, //满足绿灯 则跳转到55556
//		{0, 		ACT_DELAY,       		600,     		0,     NULL}, //等600ms
//		{0, 		ACT_CMD,        		  0,        	0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
//		{0,   		ACT_CHECK_TIMEOUT, 		55557,  		8000,     NULL},  //检查是否超时 如果超时10s则跳转到55557
//		{0, 		ACT_JUMP,        	  	55555, 	  		0,     	NULL},	//不满足跳转55555无条件跳转)
//
//
//
//		{55556, 	ACT_DELAY,       	  50,     	 	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficASend},   // 发送结果
//		{0, 		ACT_DELAY,       	  50,     	 	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficASend},   // 发送结果
//
//		{55557, 	ACT_DELAY,       	  50,     	 0,     NULL}, //10s内没识别到 发送红色
//		{0, 		ACT_CMD,         	   0,        0,     (void*)SendTrafficARed},   // 发送结果
//
//		// --- 4.测距和二维码
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	7,     NULL},	//前进
//		//测距静态标志位物:..........
//		{0, 		ACT_DELAY,       	2000,     	0,     NULL},	//等停稳
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_SaveDis}, // 把超声波距离显示到led屏幕上 并缓存一次Dis到DisBuf
//		{0, 		ACT_DELAY,       	50,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_SaveDis}, // 把超声波距离显示到led屏幕上
//		{0, 		ACT_DELAY,       	50,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_SaveDis}, // 把超声波距离显示到led屏幕上
//
//		{0, 		ACT_GO,          	GoSpeed, 	 	7,     NULL},	//前进
//
//		//二维码扫描静态标志物:..........
//		{0, 		ACT_DELAY,       	2000,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//		{0, 		ACT_DELAY,       	500,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//		{0, 		ACT_BACK,          	GoSpeed, 	 	25,     NULL},	//后退
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//
//
//		// ---5.ETC
//
//		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//
//		// 门前
//		{0, 		ACT_BACK,          		 	0, 	 	    	4,     		NULL},	//后退
//		{9998, 		ACT_TRACKMP,       			35, 		  	 11.6,     		NULL},	//循迹
//		{0, 		ACT_BACK,          		 	0, 	 	    	10.4,     		NULL},	//后退
//		{0, 		ACT_JUMP_IF,        		9999, 			GateOpen,     	(void*)&GateFlag}, //满足GateFlag == GateOpen,跳转9999
//		{0,   		ACT_JUMP,          			9998,  			0,      		NULL}, //无条件跳转9998
//		{9999, 		ACT_TRACK,       			TrackSpeed, 	0,     			NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	 	2,     NULL},	//前进
//
//		// ---6.特殊地形
//		{0, 		ACT_GO,          	GoSpeed, 	77,     NULL},
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	12,     NULL},   //码盘循迹
//		{0, 		ACT_BACK,          	GoSpeed, 	 	11,     NULL},	//后退
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	12,     NULL},   //码盘循迹
//
//
//
//		// ---7.智能路灯
//		{0, 		ACT_DELAY,       	100,     		0,     NULL},
//		{0, 		ACT_CMD,         	0,        		0,     (void*)Ti24_4SendGetLight}, // 路灯调档位
//		{0, 		ACT_BACK,          	GoSpeed, 	 	5,     NULL},	//后退
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		// ---8.公交站语音识别
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	4,     NULL},	//前进
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	13,     NULL},   //码盘循迹
//
//		{0, 		ACT_DELAY,       50,     0,     NULL},
//		{0, 		ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
//		{0, 		ACT_DELAY,       50,     0,     NULL},
//		{0, 		ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
//		{0, 		ACT_DELAY,       100,     0,     NULL},
//		{0, 		ACT_CMD,         0,        0,     (void*)GetVoicNum}, // 识别公交语音以及获取编号
//
//		{0, 		ACT_BACK,          	GoSpeed, 	 	25,     NULL},	//后退
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		// --- 9.读卡1
//		//读卡1
//		{0, 		ACT_GO,          GoSpeed, 	 	18,     NULL}, 	//9cm比较合适
//
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_7ReadRF1},
//
//		//后退再来一遍
//		{0, 		ACT_BACK,       		0, 	 	15,     NULL},	//后退
//		{0, 		ACT_GO,          GoSpeed, 	 	16,     NULL}, 	//9cm比较合适
//		{0, 		ACT_DELAY,       	 800,     	 0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_7ReadRF1},
//
//		//解密1.....
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_7SloveRF1},
//
//		// --- 10.读卡2
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//
//		//读卡2
//		{0, 		ACT_GO,          GoSpeed, 	 	28,     NULL}, 	//9cm比较合适
//
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_7ReadRF1},
//
//		//后退再来一遍
//		{0, 		ACT_BACK,       	0, 	 	  15,     NULL},	//后退
//		{0, 		ACT_GO,          GoSpeed, 	 	17,     NULL}, 	//9cm比较合适
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_7ReadRF1},
//
//		//解密2.....
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_7SloveRF1},
//
//		// --- 11.发送从车出库
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},	//从车启动
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},	//从车启动
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},	//从车启动
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},	//从车启动
//
//		// --- 12.立体显示标志物
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//
//		{0, 		ACT_DELAY,       		800,     		 0,     NULL},
//		{0, 		ACT_MPLEFT,         MPTurnSpeed, 	67,     NULL},	//码盘左转(45°)
//
//		{0, 		ACT_DELAY,       	100,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)showDis3D},
//		{0, 		ACT_DELAY,       	100,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)showDis3D},
//
//		{0, 		ACT_DELAY,       		800,     		 0,     NULL},
//		{0, 		ACT_MPRIGHT,        MPTurnSpeed, 	69,     NULL},	//码盘右转(45°)
//
//		// --- 13.等待从车给我发车牌信息
//
//		//车牌
//		{0,   		ACT_RESET_TIMER,   		0,    		0,      NULL}, //记录当前时刻
//		{1110, 		ACT_JUMP_IF_NOT,        1111, 		0,     (void*)&SlaveCarData.chepai[0]}, //满足在d3跳转1111
//		{0,   		ACT_CHECK_TIMEOUT, 		1112,  	 15000,   NULL},  //检查是否超时 如果超时15s则跳转到1112
//		{0,   		ACT_JUMP,          		1110,  		0,      NULL}, //无条件跳转600
//
//
//		{1111, 		ACT_DELAY,       	800,     	0,     NULL}, 		//delay
//		{0, 		ACT_CMD,         	0,       		0,     (void*)Command_SetGateTop2}, //车牌前三位
//		{0, 		ACT_DELAY,       	800,     	0,     NULL}, 		//delay
//		{0, 		ACT_CMD,         	0,       		0,     (void*)Command_SetGateLast2}, //车牌后三位
//
//		//走喽
//		{1112, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//
//
//
//		// --- 14.放立体车库(根据车库层数动态等待)
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_GetPortBFloor}, //获取车库层数
//		{0, 	ACT_DELAY,       	200,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_GetPortBFloor},//获取车库层数
//		{0, 	ACT_DELAY,       	200,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_GetPortBFloor},//获取车库层数
//		{0, 	ACT_DELAY,       	500,     	0,     NULL},
//
//		//降车库
//		{0, 		ACT_DELAY,       	100,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_CarPortBTo1}, //降车库
//		{0, 		ACT_DELAY,       	100,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_CarPortBTo1},//降车库
//
//
//		//动态响应
//		{0,   		ACT_RESET_TIMER,   		0,    		0,      NULL}, //记录当前时刻
//		{33333, 	ACT_JUMP_IF,        	33337, 		CarportB1,     (void*)&CarPortFlag}, //满足跳转33334
//		{0, 		ACT_JUMP_IF,        	33334, 		CarportB2,     (void*)&CarPortFlag}, //满足跳转33334
//		{0, 		ACT_JUMP_IF,        	33335, 		CarportB3,     (void*)&CarPortFlag}, //满足跳转33335
//		{0, 		ACT_JUMP_IF,        	33336, 		CarportB4,     (void*)&CarPortFlag}, //满足跳转33336
//		{0,   		ACT_CHECK_TIMEOUT, 		33337,  	 22000,   NULL},  //检查是否超时 如果超时22s则跳转到33337
//		{0,   		ACT_JUMP,          		33333,  		0,      NULL}, //无条件跳转33333
//
//
//		//2层
//		{33334, 		ACT_DELAY,       	8000,     	0,     NULL},
//		{0,   		ACT_JUMP,           33337,  		0,      NULL}, //无条件跳转33333
//
//		//3层
//		{33335, 		ACT_DELAY,       	15000,     	0,     NULL},
//		{0,   		ACT_JUMP,           33337,  		0,      NULL}, //无条件跳转33333
//
//		//4层
//		{33336, 		ACT_DELAY,       	22000,     	0,     NULL},
//		{0,   		ACT_JUMP,           33337,  		0,      NULL}, //无条件跳转33333
//
//
//		{33337, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	25,     NULL},   //码盘循迹
//		{0, 		ACT_BACK,          	GoSpeed, 	 	70,     NULL},	//后退
//
//		// --- 15.升车库到指定层数
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_7SendCPortb2},
//
//
//		// --- 16.关闭计时
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_EndTim},  	   // 关闭计时
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_EndTim},  	   // 关闭计时
//};


/*执行任务表6 24年样题6*/
//const TaskStep_t MyMission[] = {
//
//		// --- 1.计时出库
//		{0, 		ACT_CMD,         	0,       	0,     (void*)Command_StartTim},  	   // 开始计时
//
//
//		// --- 2.交通灯A识别
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	20,     NULL},   //码盘循迹
//		{0, 		ACT_BACK,          	GoSpeed, 	 	30,     NULL},	//后退
//
//		{0, 		ACT_DELAY,       		1000,     		0,     NULL}, //等停稳
//		{0,   		ACT_RESET_TIMER,   		0,    			0,      NULL}, //记录当前时刻
//		{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficAInMode}, // 开启识别
//		{0, 		ACT_DELAY,       		50,     		 0,     NULL},
//		{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficAInMode}, // 开启识别
//
//		{55555, 	ACT_JUMP_IF,        	55556, 			1,     (void*)&AndroidFlag}, //满足红灯 则跳转到55556
//		{0, 		ACT_JUMP_IF,        	55556, 			2,     (void*)&AndroidFlag}, //满足黄灯 则跳转到55556
//		{0, 		ACT_JUMP_IF,        	55556, 			3,     (void*)&AndroidFlag}, //满足绿灯 则跳转到55556
//		{0, 		ACT_DELAY,       		600,     		0,     NULL}, //等600ms
//		{0, 		ACT_CMD,        		  0,        	0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
//		{0,   		ACT_CHECK_TIMEOUT, 		55557,  		8000,     NULL},  //检查是否超时 如果超时10s则跳转到55557
//		{0, 		ACT_JUMP,        	  	55555, 	  		0,     	NULL},	//不满足跳转55555无条件跳转)
//
//
//
//		{55556, 	ACT_DELAY,       	  50,     	 	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficASend},   // 发送结果
//		{0, 		ACT_DELAY,       	  50,     	 	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficASend},   // 发送结果
//
//		{55557, 	ACT_DELAY,       	  50,     	 0,     NULL}, //10s内没识别到 发送红色
//		{0, 		ACT_CMD,         	   0,        0,     (void*)SendTrafficARed},   // 发送结果
//
//
//		// --- 4.测距和二维码
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	7,     NULL},	//前进
//		//测距静态标志位物:..........
//		{0, 		ACT_DELAY,       	2000,     	0,     NULL},	//等停稳
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上 并缓存一次Dis到DisBuf
//		{0, 		ACT_DELAY,       	50,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上
//
//		{0, 		ACT_GO,          	GoSpeed, 	 	7,     NULL},	//前进
//
//		//二维码扫描静态标志物:..........
//		{0, 		ACT_DELAY,       	2000,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//		{0, 		ACT_DELAY,       	500,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//		{0, 		ACT_BACK,          	GoSpeed, 	 	25,     NULL},	//后退
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		//二维码处理
//
//
//		// --- 5.ETC
//		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		// 门前
//		{9998, 		ACT_TRACKMP,       			35, 		  	 11.6,     		NULL},	//循迹
//		{0, 		ACT_BACK,          		 	0, 	 	    	10.7,     		NULL},	//后退
//		{0, 		ACT_JUMP_IF,        		9999, 			GateOpen,     	(void*)&GateFlag}, //满足GateFlag == GateOpen,跳转9999
//		{0,   		ACT_JUMP,          			9998,  			0,      		NULL}, //无条件跳转9998
//		{9999, 		ACT_TRACK,       			TrackSpeed, 	0,     			NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	 	2,     NULL},	//前进
//
//		// --- 6.特殊地形
//		{0, 		ACT_GO,          	GoSpeed, 	77,     NULL},
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	12,     NULL},   //码盘循迹
//		{0, 		ACT_BACK,          	GoSpeed, 	 	11,     NULL},	//后退
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	12,     NULL},   //码盘循迹
//		{0, 		ACT_BACK,          	GoSpeed, 	 	7,     NULL},	//后退
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		// --- 7.交通语音识别
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	4,     NULL},	//前进
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	11,     NULL},   //码盘循迹
//
//		//识别
//		{0, 	ACT_DELAY,       50,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
//		{0, 	ACT_DELAY,       50,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
//		{0, 	ACT_DELAY,       100,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)GetVoicNum}, // 识别公交语音以及获取编号
//
//
//
//
//		// --- 8.白卡1
//		{0, 		ACT_BACK,          	GoSpeed, 	 	27,     NULL},	//后退
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		//读卡1
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
//		{0, 	ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
//
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_6ReadRF1},
//
//		//后退再来一遍
//		{0, 		ACT_BACK,       	0, 	 	  15,     NULL},	//后退
//		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
//		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_6ReadRF1},
//
//		//解密1.....
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_6SloveRF1},
//
//		// --- 9.读卡2
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//
//		//读卡2
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
//		{0, 	ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
//
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_6ReadRF2},
//
//		//后退再来一遍
//		{0, 		ACT_BACK,       	0, 	 	  15,     NULL},	//后退
//		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
//		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_6ReadRF2},
//
//		//解密2.....
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_6SloveRF2},
//
//		// --- 10.路灯
//		{0, 		ACT_TRACK,      TrackSpeed, 	0,     NULL},	//循迹
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_6SendRFID}, //发送给从车白卡解密信息
//		{0, 		ACT_GO,          	GoSpeed, 	 	4,     NULL},	//前进
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_6SendRFID}, //发送给从车白卡解密信息
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	17,     NULL},   //码盘循迹
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_6SendRFID}, //发送给从车白卡解密信息
//
//		//调节档位
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_6SendGetLight}, // 路灯调档位
//
//		// --从车发送启动
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//
//
//		// --- 11.立体显示标志物
//		{0, 		ACT_BACK,          	GoSpeed, 	 	27,     NULL},	//后退
//		{0, 		ACT_DELAY,       	50,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Ti24_6SendRFID}, //发送给从车白卡解密信息
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},	//循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 		ACT_DELAY,       	50,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Ti24_6SendRFID}, //发送给从车白卡解密信息
//
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	12,     NULL},   //码盘循迹
//		{0, 		ACT_BACK,          	GoSpeed, 	 	10,     NULL},	//后退
//
//
//		{0, 		ACT_DELAY,       		800,     		 0,     NULL},
//		{0, 		ACT_MPLEFT,         MPTurnSpeed, 	70,     NULL},	//码盘左转(45°)
//
//		{0, 		ACT_DELAY,       	100,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)showDis3D},
//		{0, 		ACT_DELAY,       	100,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)showDis3D},
//
//		{0, 		ACT_DELAY,       		800,     		 0,     NULL},
//		{0, 		ACT_MPRIGHT,        MPTurnSpeed, 	74,     NULL},	//码盘右转(45°)
//
//
//
//		{0, 		ACT_TRACK,      TrackSpeed, 	0,     NULL},	//循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	12,     NULL},	//前进
//		// ---12.等待从车给我发停车入库指令
//		{0,   		ACT_RESET_TIMER,   		0,    		0,      NULL}, //记录当前时刻
//		{600, 		ACT_JUMP_IF,        	300, 		1,     (void*)&SlaveCarData.GoFlag}, //满足在d3跳转300
//		{0,   		ACT_CHECK_TIMEOUT, 		400,  	 80000,   NULL},  //检查是否超时 如果超时60s则跳转到400
//		{0,   		ACT_JUMP,          		600,  		0,      NULL}, //无条件跳转600
//
//
//
//
//		// --- 13.倒车入库
//
//		{300, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	7,     NULL},   //码盘循迹
//		{0, 		ACT_BACK,          	GoSpeed, 	 	34,     NULL},	//后退
//
//
//		// -- 14.开启无线充电
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_WireOpen},  	   // 开启无线充电
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_WireOpen},  	   // 开启无线充电
//
//		// --- 15.停止计时
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_EndTim},  	   // 停止计时
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_EndTim},  	   // 停止计时
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_EndTim},  	   // 停止计时
//};
























/*执行任务表5 24年样题5*/
//const TaskStep_t MyMission[] = {
//
//		// --- 1.计时出库
//		{0, 		ACT_CMD,         	0,       	0,     (void*)Command_StartTim},  	   // 开始计时
//		{0, 		ACT_CMD,         	0,       	0,     (void*)Command_StartTim},  	   // 开始计时
//		{0, 		ACT_DELAY,       	200,     	0,     NULL}, 		//delay
//
//		// --- 2.交通灯A识别
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	3,     NULL},	//前进
//
//		//识别
//		{0, 		ACT_DELAY,       		1000,     		0,     NULL}, //等停稳
//		{0,   		ACT_RESET_TIMER,   		0,    			0,      NULL}, //记录当前时刻
//		{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficAInMode}, // 开启识别
//		{0, 		ACT_DELAY,       		50,     		 0,     NULL},
//		{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficAInMode}, // 开启识别
//
//		{55555, 	ACT_JUMP_IF,        	55556, 			1,     (void*)&AndroidFlag}, //满足红灯 则跳转到55556
//		{0, 		ACT_JUMP_IF,        	55556, 			2,     (void*)&AndroidFlag}, //满足黄灯 则跳转到55556
//		{0, 		ACT_JUMP_IF,        	55556, 			3,     (void*)&AndroidFlag}, //满足绿灯 则跳转到55556
//		{0, 		ACT_DELAY,       		600,     		0,     NULL}, //等600ms
//		{0, 		ACT_CMD,        		  0,        	0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
//		{0,   		ACT_CHECK_TIMEOUT, 		55557,  		8000,     NULL},  //检查是否超时 如果超时10s则跳转到55557
//		{0, 		ACT_JUMP,        	  	55555, 	  		0,     	NULL},	//不满足跳转55555无条件跳转)
//
//
//
//		{55556, 	ACT_DELAY,       	  50,     	 	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficASend},   // 发送结果
//		{0, 		ACT_DELAY,       	  50,     	 	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficASend},   // 发送结果
//
//		{55557, 	ACT_DELAY,       	  50,     	 0,     NULL}, //10s内没识别到 发送红色
//		{0, 		ACT_CMD,         	   0,        0,     (void*)SendTrafficARed},   // 发送结果
//
//
//		// --- 4.特殊地形
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//
//		{0, 		ACT_GO,          	GoSpeed, 	54,     NULL},
//		{0, 		ACT_TRACK,       TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	3,     NULL},
//
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	14,     NULL},   //码盘循迹
//		{0, 		ACT_BACK,          	GoSpeed, 	 	17,     NULL},	//后退
//		{0, 		ACT_TRACK,       TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	6,     NULL},
//
//
//		// --- 5.超声波测距以及二维码检测
//
//		//测距静态标志位物:..........
//		{0, 		ACT_DELAY,       	2000,     	0,     NULL},	//等停稳
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上 并缓存一次Dis到DisBuf
//		{0, 		ACT_DELAY,       	50,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上
//		//二维码扫描静态标志物:..........
//		{0, 		ACT_DELAY,       	1000,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//		{0, 		ACT_DELAY,       	500,     	0,     NULL},
//		{0, 		ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//
//		// --- 6.道闸(车牌跳过)
//		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 		ACT_TRACK,       TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	6,     NULL},
//		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 		ACT_TRACK,       TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	6,     NULL},
//
//		// --- 7.ETC闸门
//		{9998, 		ACT_TRACKMP,       			35, 		  	 11.6,     		NULL},	//循迹
//		{0, 		ACT_BACK,          		 	0, 	 	    	10.7,     		NULL},	//后退
//		{0, 		ACT_JUMP_IF,        		9999, 			GateOpen,     	(void*)&GateFlag}, //满足GateFlag == GateOpen,跳转9999
//		{0,   		ACT_JUMP,          			9998,  			0,      		NULL}, //无条件跳转9998
//		{9999, 		ACT_TRACK,       			TrackSpeed, 	0,     			NULL},
//
//
//		// --- 8.路灯
//		{0, 		ACT_GO,          	GoSpeed, 	 	3,     NULL},	//前进
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	16,     NULL},   //码盘循迹
//		{0, 		ACT_DELAY,       	100,     		0,    		NULL},
//		{0, 		ACT_CMD,         	0,       	 	0,     		(void*)Ti24_3SendGetLight}, // 路灯调档位
//
//		{0, 		ACT_BACK,          	GoSpeed, 	 	21,     	NULL},	//后退
//		{0, 		ACT_TRACK,       TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	6,     NULL},
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		// --- 9.寻卡读卡1
//		{0, 		ACT_TRACK,       TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	6,     NULL},
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		//读卡1
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
//		{0, 	ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
//
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_4ReadRF1},
//
//		//后退再来一遍
//		{0, 		ACT_BACK,       	0, 	 	  15,     NULL},	//后退
//		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
//		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_4ReadRF1},
//
//		//解密1.....
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_4SloveRF1},
//
//
//		// --- 10.寻卡读卡2
//		{0, 		ACT_TRACK,       TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	6,     NULL},
//
//		//读卡2
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
//		{0, 	ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
//
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_4ReadRF1},
//
//		//后退再来一遍
//		{0, 		ACT_BACK,       	0, 	 	  15,     NULL},	//后退
//		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
//		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_4ReadRF1},
//
//		//解密2.....
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_4SloveRF1},
//
//		// --- 11.公交站识别语音
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	4,     NULL},	//前进
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	17,     NULL},   //码盘循迹
//
//		{0, 		ACT_DELAY,       50,     0,     NULL},
//		{0, 		ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
//		{0, 		ACT_DELAY,       50,     0,     NULL},
//		{0, 		ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
//		{0, 		ACT_DELAY,       50,     0,     NULL},
//		{0, 		ACT_CMD,         0,        0,     (void*)GetVoicNum}, // 识别公交语音以及获取编号
//		{0, 		ACT_DELAY,       100,     0,     NULL},
//		{0, 		ACT_CMD,         0,        0,     (void*)SendVoiceNum}, // 发送语音编号
//		{0, 		ACT_DELAY,       100,     0,     NULL},
//		{0, 		ACT_CMD,         0,        0,     (void*)SendVoiceNum}, // 发送语音编号
//
//		{0, 		ACT_BACK,          	GoSpeed, 	 	30,     NULL},	//后退
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//
//
//		// --- 12.发送从车启动
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//
//
//		// --- 13.倒车入库
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	25,     NULL},   //码盘循迹
//		{0, 		ACT_BACK,          	GoSpeed, 	 	70,     NULL},	//后退
//
//
//
//};








/*执行任务表4 24年样题4*/
//const TaskStep_t MyMission[] = {
//
//		// --- 1.计时出库
//		{0, 	ACT_CMD,         	0,       	0,     (void*)Command_StartTim},  	   // 开始计时
//		{0, 	ACT_CMD,         	0,       	0,     (void*)Command_StartTim},  	   // 开始计时
//		{0, 	ACT_DELAY,       	200,     	0,     NULL}, 		//delay
//
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//
//		// --- 2.过道闸无 二维码和超声波测距
//		{0, 	ACT_GO,          	GoSpeed, 	 	8,     NULL},	//前进
//
//
//		//测距静态标志位物:..........
//		{0, 	ACT_DELAY,       	2000,     	0,     NULL},	//等停稳
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上 并缓存一次Dis到DisBuf
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上
//		//二维码扫描静态标志物:..........
//		{0, 	ACT_DELAY,       	1000,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//		{0, 	ACT_DELAY,       	500,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//
//
//		// --- 3.交通灯A识别
//		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	35,     NULL},   //码盘循迹
//		{0, 		ACT_BACK,          	GoSpeed, 	 	60,     NULL},	//后退
//
//		{0, 		ACT_DELAY,       		1000,     		0,     NULL}, //等停稳
//		{0,   		ACT_RESET_TIMER,   		0,    			0,      NULL}, //记录当前时刻
//		{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficAInMode}, // 开启识别
//		{0, 		ACT_DELAY,       		50,     		 0,     NULL},
//		{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficAInMode}, // 开启识别
//
//		{55555, 	ACT_JUMP_IF,        	55556, 			1,     (void*)&AndroidFlag}, //满足红灯 则跳转到55556
//		{0, 		ACT_JUMP_IF,        	55556, 			2,     (void*)&AndroidFlag}, //满足黄灯 则跳转到55556
//		{0, 		ACT_JUMP_IF,        	55556, 			3,     (void*)&AndroidFlag}, //满足绿灯 则跳转到55556
//		{0, 		ACT_DELAY,       		400,     		0,     NULL}, //等400ms
//		{0, 		ACT_CMD,        		  0,        	0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
//		{0,   		ACT_CHECK_TIMEOUT, 		55557,  		10000,     NULL},  //检查是否超时 如果超时10s则跳转到55557
//		{0, 		ACT_JUMP,        	  	55555, 	  		0,     	NULL},	//不满足跳转55555无条件跳转)
//
//
//
//		{55556, 	ACT_DELAY,       	  50,     	 	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficASend},   // 发送结果
//		{0, 		ACT_DELAY,       	  50,     	 	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficASend},   // 发送结果
//
//		{55557, 	ACT_DELAY,       	  50,     	 0,     NULL}, //10s内没识别到 发送红色
//		{0, 		ACT_CMD,         	   0,        0,     (void*)SendTrafficBRed},   // 发送结果
//
//		{0, 		ACT_GO,          	GoSpeed, 	 	4,     NULL},	//前进
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//
//		// --- 4.ETC
//		{0, 		ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 		ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 		ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//
//
//		{9998, 		ACT_TRACKMP,       			77, 		  	 11.6,     		NULL},	//循迹
//		{0, 		ACT_BACK,          		 	0, 	 	    	11.6,     		NULL},	//后退
//		{0, 		ACT_JUMP_IF,        		9999, 			GateOpen,     	(void*)&GateFlag}, //满足GateFlag == GateOpen,跳转9999
//		{0,   		ACT_JUMP,          			9998,  			0,      		NULL}, //无条件跳转9998
//		{9999, 		ACT_TRACK,       			TrackSpeed, 	0,     			NULL},
//		{0, 		ACT_GO,          			GoSpeed, 	 	6,     			NULL},
//
//
//		// --- 5.过特殊地形
//		{0, 		ACT_TRACKMP,     TrackSpeed, 	5,     NULL},   //码盘循迹
//		{0, 		ACT_GO,          	GoSpeed, 	49,     NULL},
//		{0, 		ACT_TRACK,       TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	6,     NULL},
//
//		// --- 6.语音交通站
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 		ACT_TRACK,       TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	18,     NULL},
//
//
//		{0, 	ACT_DELAY,       50,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
//		{0, 	ACT_DELAY,       50,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
//		{0, 	ACT_DELAY,       50,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)GetVoicNum}, // 识别公交语音以及获取编号
//		{0, 	ACT_DELAY,       100,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)SendVoiceNum}, // 发送语音编号
//		{0, 	ACT_DELAY,       100,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)SendVoiceNum}, // 发送语音编号
//
//
//		// --- 7.读卡1
//		{0, 		ACT_BACK,       	0, 	 	  31,     NULL},	//后退
//		{0, 		ACT_TRACK,       	TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	6,     NULL},
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
//		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
//
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_4ReadRF1},
//
//
//		//后退再来一遍
//
//		{0, 		ACT_BACK,       	0, 	 	  15,     NULL},	//后退
//		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
//		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_4ReadRF1},
//
//		//解密1.....
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_4SloveRF1},
//
//
//
//		// --- 8.读卡2
//		{0, 		ACT_TRACK,       	TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	6,     NULL},
//
//		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
//		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
//
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_2ReadRF1},
//
//
//		{0, 		ACT_BACK,       	0, 	 	  15,     NULL},	//后退
//		{0, 		ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},	//白卡循迹
//		{0, 		ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//9cm比较合适
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_4ReadRF1},
//
//
//		//解密2....
//		{0, 		ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        0,     (void*)Ti24_4SloveRF1},
//
//
//		// --- 9.路灯
//		{0, 		ACT_TRACK,       	TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	18,     NULL},
//
//		//调节
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_4SendGetLight}, // 路灯调档位
//
//		{0, 		ACT_BACK,       			0, 	 	  32,     NULL},	//后退
//		{0, 		ACT_TRACK,       	TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 	6,     NULL},
//
//
//		// ---10.从车启动
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//
//		// --- 11.倒车入库
//		{0, 		ACT_TRACK,       	TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 		6,     NULL},
//		{0, 		ACT_TRACK,       	TrackSpeed, 	0,     NULL},
//		{0, 		ACT_GO,          	GoSpeed, 		6,     NULL},
//
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		{0, 		ACT_TRACKMP,      	TrackSpeed, 	35,     NULL},   //码盘循迹
//		{0, 		ACT_BACK,          	GoSpeed, 	 	75,     NULL},	//后退
//
//		// --- 12.升车库
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_4SendCPortb},
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_4SendCPortb},
//
//
//};








/*执行任务表3 24年样题3*/
//const TaskStep_t MyMission[] = {
//
//	// --- 1.测距
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	9,     NULL},	//前进
//		//测距 发送距离
//		{0, 	ACT_DELAY,       	1000,     	0,     NULL},	//等停稳
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上 并缓存一次Dis到DisBuf
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上
//
//
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarSendDis}, // 把超声波距离显示到led屏幕上
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarSendDis}, // 把超声波距离显示到led屏幕上
//
//
//	// --- 2.交通灯B识别
//		{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarSendDis}, // 把超声波距离显示到led屏幕上
//		{0, 	ACT_TRACKMP,      	TrackSpeed, 	30,     NULL},  //码盘循迹
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarSendDis}, // 把超声波距离显示到led屏幕上
//		{0, 	ACT_BACK,          	GoSpeed, 	 	55,     NULL},	//后退
//
//		//识别
//		{0, 		ACT_DELAY,       		1000,     		0,     NULL}, //等停稳
//		{0,   		ACT_RESET_TIMER,   		0,    			0,      NULL}, //记录当前时刻
//		{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficBInMode}, // 开启识别
//		{0, 		ACT_DELAY,       		50,     		 0,     NULL},
//		{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficBInMode}, // 开启识别
//		{55555, 	ACT_JUMP_IF,        	55556, 			1,     (void*)&AndroidFlag}, //满足红灯 则跳转到55556
//		{0, 		ACT_JUMP_IF,        	55556, 			2,     (void*)&AndroidFlag}, //满足黄灯 则跳转到55556
//		{0, 		ACT_JUMP_IF,        	55556, 			3,     (void*)&AndroidFlag}, //满足绿灯 则跳转到55556
//		{0, 		ACT_DELAY,       		400,     		0,     NULL}, //等400ms
//		{0, 		ACT_CMD,        		  0,        	0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
//		{0,   		ACT_CHECK_TIMEOUT, 		55557,  		10000,     NULL},  //检查是否超时 如果超时10s则跳转到55557
//		{0, 		ACT_JUMP,        	  	55555, 	  		0,     	NULL},	//不满足跳转55555无条件跳转)
//		{55556, 	ACT_DELAY,       	  50,     	 	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficBSend},   // 发送结果
//		{0, 		ACT_DELAY,       	  50,     	 	0,     NULL},
//		{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficBSend},   // 发送结果
//		{55557, 	ACT_DELAY,       	  50,     	 0,     NULL}, //10s内没识别到 发送红色
//		{0, 		ACT_CMD,         	   0,        0,     (void*)SendTrafficBRed},   // 发送结果
//
//
//	// --- 3.特殊地形
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//
//		{0, 	ACT_DELAY,       	1000,     	0,     NULL},	//等停稳
//		{0, 	ACT_BACK,          	GoSpeed, 	 	14,     NULL},	//后退
//		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
//		{0, 	ACT_GO,          	GoSpeed, 	 	3,     NULL},	//前进
//		{0, 	ACT_TRACKMP,      	TrackSpeed, 	7,     NULL},  //码盘循迹
//
//		{0, 	ACT_DELAY,       	1000,     	0,     NULL},	//等停稳
//
//		{0, 	ACT_BACK,          	GoSpeed, 	 	15,     NULL},	//后退
//		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
//		{0, 	ACT_GO,          	GoSpeed, 	 	3,     NULL},	//前进
//		{0, 	ACT_TRACKMP,      	TrackSpeed, 	7,     NULL},  //码盘循迹
//
//
//		{0, 	ACT_GO,          	GoSpeed, 	52,     NULL},
//		{0, 	ACT_GO,          	GoSpeed, 	 	18,     NULL},	//前进
//
//	 //--- 4.寻白卡1
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
//		{0, 	ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//8cm比较合适
//
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_3ReadRF1},
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//
//
//		//后退再读一次
//		{0, 	ACT_BACK,          	GoSpeed, 	 	19,     NULL},	//后退
//
//
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
//		{0, 	ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//8cm比较合适
//
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_3ReadRF1},
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//
//		//解密1
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_3SloveRF1},
//
//	// --- 5.读卡2
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
//		{0, 	ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//8cm比较合适
//
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_3ReadRF2},
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//
//
//		{0, 	ACT_BACK,          	GoSpeed, 	 	19,     NULL},	//后退
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
//		{0, 	ACT_GO,          GoSpeed, 	 	9,     NULL}, 	//8cm比较合适
//
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_3ReadRF2},
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//
//		//解密2
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_3SloveRF2},
//
//
//	// --- 6.公交站语音
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	12,     NULL},	//前进
//		//交通语音识别
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
//
//		{0, 	ACT_DELAY,       50,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
//		{0, 	ACT_DELAY,       50,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
//		{0, 	ACT_DELAY,       50,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)GetVoicNum}, // 识别公交语音以及获取编号
//		{0, 	ACT_DELAY,       100,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)SendVoiceNum}, // 发送语音编号
//		{0, 	ACT_DELAY,       100,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)SendVoiceNum}, // 发送语音编号
//
//
//	// 7.etc闸门
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{9998, 		ACT_TRACKMP,       			20, 		  	 11.6,     		NULL},	//循迹
//		{0, 		ACT_BACK,          		 	0, 	 	    	11.6,     		NULL},	//后退
//		{0, 		ACT_JUMP_IF,        		9999, 			GateOpen,     	(void*)&GateFlag}, //满足GateFlag == GateOpen,跳转9999
//		{0,   		ACT_JUMP,          			9998,  			0,      		NULL}, //无条件跳转9998
//		{9999, 		ACT_TRACKRFID,       		TrackSpeed, 	0,     			NULL},
//		{0, 		ACT_GO,          			100, 	 	6,     			NULL},
//
//
//	// 8.调节路灯
//		{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_3SendGetLight}, // 路灯调档位
//
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)SendToSlaveLight}, // 发送档位给从车
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)SendToSlaveLight}, // 发送档位给从车
//
//	// 9. 启动从车
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	1000,     	0,     NULL},
//
//	// 10.道闸无
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 		ACT_TRACK,       		TrackSpeed, 	0,     			NULL},
//		{0, 		ACT_GO,          			GoSpeed, 	 	6,     			NULL},
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 		ACT_TRACK,       		TrackSpeed, 	0,     			NULL},
//		{0, 		ACT_GO,          			GoSpeed, 	 	6,     			NULL},
//		{0, 		ACT_TRACK,       		TrackSpeed, 	0,     			NULL},
//		{0, 		ACT_GO,          			GoSpeed, 	 	6,     			NULL},
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 		ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		{0, 	ACT_TRACKMP,       TrackSpeed, 		24,     NULL},	//循迹
//		{0, 	ACT_BACK,          GoSpeed, 	 	69	,     NULL},	//后退
//
//
//	// 11.上升车库
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_3SendCPortb},
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_3SendCPortb},
//
//
//
//};
/*执行任务表3 24年样题2*/
//const TaskStep_t MyMission[] = {
//
//	// --- 1.计时开始
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_StartTim},  	   // 开始计时
//
//
//	// --- 2.超声波测距和二维码识别
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	9,     NULL},	//前进
//		{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//
//		//测距静态标志位物:..........
//		{0, 	ACT_DELAY,       	1500,     	0,     NULL},	//等停稳
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上 并缓存一次Dis到DisBuf
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上
//		//二维码扫描静态标志物:..........
//		{0, 	ACT_DELAY,       	1000,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//		{0, 	ACT_DELAY,       	400,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//		{0, 	ACT_DELAY,       	400,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//		{0, 	ACT_DELAY,       	400,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//
//
//	// --- 3.走红绿灯 识别红绿灯A
//		{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 	ACT_TRACKMP,      	TrackSpeed, 	27,     NULL},  //码盘循迹
//		{0, 	ACT_BACK,          	GoSpeed, 	 	22,     NULL},	//后退
//
//			//识别
//			{0, 		ACT_DELAY,       		1000,     		0,     NULL}, //等停稳
//			{0,   		ACT_RESET_TIMER,   		0,    			0,      NULL}, //记录当前时刻
//			{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficAInMode}, // 开启识别
//			{0, 		ACT_DELAY,       		50,     		 0,     NULL},
//			{0, 		ACT_CMD,        		0,        		0,     (void*)Command_TrafficAInMode}, // 开启识别
//
//			{55555, 	ACT_JUMP_IF,        	55556, 			1,     (void*)&AndroidFlag}, //满足红灯 则跳转到55556
//			{0, 		ACT_JUMP_IF,        	55556, 			2,     (void*)&AndroidFlag}, //满足黄灯 则跳转到55556
//			{0, 		ACT_JUMP_IF,        	55556, 			3,     (void*)&AndroidFlag}, //满足绿灯 则跳转到55556
//			{0, 		ACT_DELAY,       		400,     		0,     NULL}, //等400ms
//			{0, 		ACT_CMD,        		  0,        	0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
//			{0,   		ACT_CHECK_TIMEOUT, 		55557,  		7000,     NULL},  //检查是否超时 如果超时10s则跳转到55557
//			{0, 		ACT_JUMP,        	  	55555, 	  		0,     	NULL},	//不满足跳转55555无条件跳转)
//
//
//
//			{55556, 	ACT_DELAY,       	  50,     	 	0,     NULL},
//			{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficASend},   // 发送结果
//			{0, 		ACT_DELAY,       	  50,     	 	0,     NULL},
//			{0, 		ACT_CMD,         	   0,        	0,     (void*)Command_TrafficASend},   // 发送结果
//
//			{55557, 	ACT_DELAY,       	  50,     	 0,     NULL}, //10s内没识别到 发送红色
//			{0, 		ACT_CMD,         	   0,        0,     (void*)SendTrafficBRed},   // 发送结果
//
//	// --- 4.识别车牌
//		{0, 	ACT_GO,          	GoSpeed, 	 	3,     NULL},	//前进
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	9,     NULL},	//前进
//		{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 	ACT_TRACKMP,      	TrackSpeed, 	16,     NULL},  //码盘循迹
//		{0, 	ACT_BACK,          	GoSpeed, 	 	12,     NULL},	//后退
//		 // 车牌识别
//		{0, 	ACT_DELAY,       	1000,     	0,     NULL}, 		//delay
//		{0, 	ACT_CMD,         	0,       		0,     (void*)Command_AndroidPlatNum}, //命令
//
//	// --- 5.交通站语音交互
//		{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//
//		//语音识别
//		{0, 	ACT_DELAY,       50,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
//		{0, 	ACT_DELAY,       50,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
//		{0, 	ACT_DELAY,       50,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)GetVoicNum}, // 识别公交语音以及获取编号
//		{0, 	ACT_DELAY,       100,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)SendVoiceNum}, // 识别公交语音以及获取编号
//		{0, 	ACT_DELAY,       100,     0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)SendVoiceNum}, // 识别公交语音以及获取编号
//
//
//	// --- 6.从车发车
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)Command_SlaveCarStart},
//
//	// --- 20.等待从车通过
//		{0,   		ACT_RESET_TIMER,   		0,    			0,      NULL}, //记录当前时刻
//		{100, 		ACT_JUMP_IF,        	101, 			1,     (void*)&SlaveCarData.GoFlag}, //满足红灯 则跳转到55556
//		{0,   		ACT_CHECK_TIMEOUT, 		101,  		60000,     NULL},  //检查是否超时 如果超时60ss则跳转到55557
//		{0, 		ACT_JUMP,        	  	100, 	  		0,     	NULL},	//不满足跳转55555无条件跳转)
//
//	// --- 7.读卡1
//		{101, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 	ACT_TRACKMP,      	TrackSpeed, 	22,     NULL},   //码盘循迹
//		{0, 	ACT_BACK,          	GoSpeed, 	 	38,     NULL},	//后退
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
//		{0, 	ACT_GO,          GoSpeed, 	 	8,     NULL}, 	//8cm比较合适
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_2ReadRF1},
//
//		{0, 	ACT_BACK,          	GoSpeed, 	 	20,     NULL},	//后退
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
//		{0, 	ACT_GO,          GoSpeed, 	 	8,     NULL}, 	//8cm比较合适
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_2ReadRF1},
//
//		//解密...
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_2SloveRF1},
//
//
//	// --- 8.过特殊地形
//		{0, 	ACT_GO,          	GoSpeed, 	54,     NULL},
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL}, //如出地形就出现白卡就用白卡循迹
//		{0, 	ACT_GO,          	GoSpeed, 	8,     NULL},
//	// --- 9.读卡2
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_2ReadRF2},
//
//		//不保险再回来读一次
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL}, //如出地形就出现白卡就用白卡循迹
//		{0, 	ACT_GO,          	GoSpeed, 	8,     NULL},
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_2ReadRF2},
//
//		//解密
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti24_2SloveRF2},
//
//	// --- 10.路灯
//		{0, 	ACT_BACK,          	GoSpeed, 	 	21,     NULL},	//后退
//		{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
//		//调节路灯
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)SendToSlaveLight}, // 路灯调档位
//
//	// --- 11.到立体车库A
//		{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},   //循迹
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	//前进
//		{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
//
//		{0, 	ACT_TRACKMP,      	TrackSpeed, 	20,     NULL},   //码盘循迹
//		{0, 	ACT_BACK,          	GoSpeed, 	 	57,     NULL},	//后退
//
//		// 升车库
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)SendCPortb}, // 路灯调档位
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        0,     (void*)SendCPortb}, // 路灯调档位
//
//};



/*执行任务表2 24年样题1*/
//const TaskStep_t MyMission[] = {
//
//
//
//	//--- 1. 起步开始计时 ---
//	{0, 	ACT_CMD,         	0,       		0,     (void*)Command_StartTim},  	   // 开始计时
////	{0, 	ACT_DELAY,       	200,     		0,     NULL},
////	{0, 	ACT_TRACK,       	TrackSpeed, 	0,     NULL}, 			/*循迹*/
////	{0, 	ACT_GO,          	GoSpeed, 	 	8,     NULL},			/*前进*/
////	{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},		//右转
////	{0, 	ACT_DELAY,       	800,     		0,     NULL},
////
////		/*识别车牌*/
////		{0, 	ACT_MPRIGHT,        MPTurnSpeed, 	63,     NULL},	//码盘右转
////		{0, 	ACT_DELAY,       	1500,     		0,     NULL},
////		{0, 	ACT_CMD,         	0,        		0,     (void*)Command_AndroidPlatNum},  	   // 识别车牌
////		{0, 	ACT_CMD,         	0,        		0,     (void*)Command_AndroidPlatNum},  	   // 识别车牌
////		{0, 	ACT_MPLEFT,        MPTurnSpeed, 	62,     NULL},	//码盘右转
////
////
////
////	// --- 2. 交通灯B识别 ---
////	{0, 	ACT_TRACK,       	TrackSpeed, 	0,     NULL}, 			/*循迹*/
////	{0, 	ACT_BACK,          	GoSpeed, 	 	7,     NULL},
////
////		/*交通灯功能*/
////		{0, 	ACT_DELAY,       	 1000,     	0,     NULL}, //等停稳
////		{5, 	ACT_CMD,         0,        0,     (void*)Command_TrafficBInMode}, // 开启识别
////		{6, 	ACT_DELAY,       1500,     0,     NULL},
////		{7, 	ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
////		{6, 	ACT_DELAY,       50,     0,     NULL},
////		{8, 	ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令2
////		{6,	 	ACT_DELAY,       50,     0,     NULL},
////		{9, 	ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令3
////
////		{10, 	ACT_WAIT_NEQ0,   3000,     0,     (void*)&AndroidFlag}, //超时3s跳过，如果接收到AndroidFl发送结果
////		{11, 	ACT_CMD,         0,        0,     (void*)Command_TrafficBSend},   // 发送结果
////		{12, 	ACT_CMD,         0,        0,     (void*)Command_TrafficBSend},   // 发送结果
////
////	{0, 	ACT_GO,          	GoSpeed, 	 	15,     NULL},			/*前进*/
////	// --- 3. 公交站语音 ---
////	{0, 	ACT_TRACK,       	TrackSpeed, 	0,     NULL}, 			/*循迹*/
////	{0, 	ACT_GO,          	GoSpeed, 	 	4,     NULL},			/*前进*/
////	{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
////		/*公交站功能*/
////		{6, 	ACT_DELAY,       50,     0,     NULL},
////		{8, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
////		{6, 	ACT_DELAY,       50,     0,     NULL},
////		{8, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 公交站开启语音
////		{6, 	ACT_DELAY,       50,     0,     NULL},
////		{8, 	ACT_CMD,         0,        0,     (void*)GetVoicNum}, // 公交站开启语音
////
////	// --- 4. 二维码与测距 ---
////	{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
////	{0, 	ACT_TRACKMP,      	TrackSpeed, 	15,     NULL},
////	{0, 	ACT_BACK,          	GoSpeed, 	 	31,     NULL},	//前进
////	{0, 	ACT_TRACK,      	TrackSpeed, 	0,     NULL},
////	{0, 	ACT_GO,          	GoSpeed, 	 	8,     NULL},	//前进
////
////		//测距静态标志位物:..........
////		{0, 	ACT_DELAY,       	1500,     	0,     NULL},	//等停稳
////		{0, 	ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上
////		{0, 	ACT_DELAY,       	50,     	0,     NULL},
////		{0, 	ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上
////
////		//二维码扫描静态标志物:..........
////		{0, 	ACT_DELAY,       	1000,     	0,     NULL},
////		{0, 	ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
////		{0, 	ACT_DELAY,       	500,     	0,     NULL},
////		{0, 	ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
////		{0, 	ACT_DELAY,       	500,     	0,     NULL},
////		{0, 	ACT_CMD,         	0,        0,     (void*)Ti24_RQSlove}, // 二维码数据处理
////
////
////	{0, 	ACT_GO,          	 GoSpeed, 	 	1,     NULL},
////	{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
////
////	// --- 5. 寻白卡1 ---
////	{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},
////	{0, 	ACT_GO,          	 GoSpeed, 	 	8,     NULL}, //7cm比较合适
////
////		//读卡1
////		{0, 	ACT_DELAY,       	1000,     	0,     NULL},
////		{0, 	ACT_CMD,         	0,        	0,     (void*)Ti24_1ReadRFID1}, // 读卡1
////		{0, 	ACT_DELAY,       	500,     	0,     NULL},
////		{0, 	ACT_CMD,         	0,        	0,     (void*)Ti24_1SloveRFID1}, // 卡1数据处理 判断是否是我要的卡
////		{0, 	ACT_DELAY,       	800,     	0,     NULL},
////		{100, 	ACT_CMD,         	0,        	0,     (void*)Ti24_1ReadRFID2}, // 卡1数据处理 判断是否是我要的卡
////		{101, 	ACT_DELAY,       	500,     	0,     NULL},
////		{102, 	ACT_CMD,         	0,        	0,     (void*)Ti24_1SloveRFID2}, // 是我要的卡就处理
////
////
////
////
////	// --- 6. 过特殊地形 ---
////		//特殊地形
////		{120, 	ACT_BACK,          	GoSpeed, 	 	29,     NULL},	//后退
////		{0, 	ACT_TRACKRFID,      TrackSpeed, 	0,     NULL},
////		{0, 	ACT_GO,          	GoSpeed, 	 	60,     NULL}, //过特殊地形
////
////		//寻白卡
////		{0, 	ACT_TRACKRFID,       66, 	0,     NULL},
////		{0, 	ACT_GO,          	 80, 	 	8,     NULL}, //8cm比较合适
////			//白卡2读卡解密
////			{0, 	ACT_DELAY,       	1000,     	0,     NULL},
////			{0, 	ACT_CMD,         	0,        	0,     (void*)Ti24_1ReadRFID1}, // 读卡2
////			{0, 	ACT_DELAY,       	800,     	0,     NULL},
////			{0, 	ACT_CMD,         	0,        	0,     (void*)Ti24_1SloveRFID1}, // 卡1数据处理 判断是否是我要的卡
////			{0, 	ACT_DELAY,       	800,     	0,     NULL},
////			{150, 	ACT_CMD,         	0,        	0,     (void*)Ti24_1ReadRFID2}, // 卡1数据处理 判断是否是我要的卡
////			{151, 	ACT_DELAY,       	800,     	0,     NULL},
////			{152, 	ACT_CMD,         	0,        	0,     (void*)Ti24_1SloveRFID2}, // 是我要的卡就处理
////
////
////	 // --- 7. 调节路灯 ---
////		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
////		{0, 	ACT_GO,          	 GoSpeed, 	 	8,     NULL},
////
////		{0, 	ACT_DELAY,       	100,     	0,     NULL},
////		{0, 	ACT_CMD,         	0,        0,     (void*)SendLightAuto1}, // 路灯调档位
////		{0, 	ACT_DELAY,       	100,     	0,     NULL},
////		{0, 	ACT_CMD,         	0,        0,     (void*)SendLight}, // 路灯调档位
////		{0, 	ACT_DELAY,       	50,     	0,     NULL},
////		{0, 	ACT_CMD,         	0,        0,     (void*)SendLight}, // 路灯调档位
////
////	// --- 8. 到etc闸门
////		{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
////		{0, 	ACT_DELAY,       	50,     	0,     NULL},
////		{0, 	ACT_CMD,         	0,        0,     (void*)SendLight}, // 路灯调档位
////		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
////		{0, 	ACT_DELAY,       	50,     	0,     NULL},
////		{0, 	ACT_CMD,         	0,        0,     (void*)SendLight}, // 路灯调档位
////		{0, 	ACT_GO,          	 GoSpeed, 	 	8,     NULL},
////		{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},	//左转
////			//徘徊过门
////			{0, 	ACT_CUSTOM_LOOP,    0, 			0,     (void *)Action_WaitGateOpen},	//前进后退直到etc开门
////			{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
////			{0, 	ACT_GO,          	 GoSpeed, 	 	8,     NULL},
////
////			//发送从车开启
////			{0, 	ACT_DELAY,       	 50,     	0,     NULL}, //等摄像稳定
////			{0, 	ACT_CMD,         	   0,        0,     (void*)Command_SlaveCarStart},   //从车启动
////			{0, 	ACT_DELAY,       	 50,     	0,     NULL}, //等摄像稳定
////			{0, 	ACT_CMD,         	   0,        0,     (void*)Command_SlaveCarStart},   //从车启动
////
////	// --- 9. 识别颜色图形
////			{0, 	ACT_DELAY,       	800,     		0,     NULL},
////			{0, 	ACT_MPLEFT,        MPTurnSpeed, 	64,     NULL},	//码盘右转
////
////				// 颜色识别
////				{0, 	ACT_DELAY,       	 2000,     	0,     NULL}, //等摄像稳定
////				{0, 	ACT_CMD,         	   0,        0,     (void*)Command_AndroidColor},   // 颜色识别
////				{0, 	ACT_DELAY,       	 800,     	0,     NULL},
////				{0, 	ACT_CMD,         	   0,        0,     (void*)Command_AndroidColor},   // 颜色识别
////				{0, 	ACT_DELAY,       	 800,     	0,     NULL},
////				{0, 	ACT_CMD,         	   0,        0,     (void*)Command_Androidshape},   // 形状识别
////				{0, 	ACT_DELAY,       	 800,     	0,     NULL},
////				{0, 	ACT_CMD,         	   0,        0,     (void*)Command_Androidshape},   // 形状识别
////				{0, 	ACT_DELAY,       	 1000,     	0,     NULL},
////				{0, 	ACT_CMD,         	0,        		0,     (void*)Command_sendshap2},  	   // 识别车牌
////				{0, 	ACT_DELAY,       	 100,     	0,     NULL},
////				{0, 	ACT_CMD,         	0,        		0,     (void*)Command_sendcolor2},  	   // 识别车牌
////
////			{0, 	ACT_MPRIGHT,        MPTurnSpeed, 	64,     NULL},	//码盘右转
////			{0, 	ACT_DELAY,       	50,     		0,     NULL},
////			{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
////
////	// --- 15. 等从车给我发指定车库
////		    {0,   ACT_RESET_TIMER,   	0,    0,      NULL},
////			{600, 	ACT_JUMP_IF,        300, 		1,     (void*)&SlaveCarData.Location}, //满足在d3跳转300
////			{700, 	ACT_JUMP_IF,        350, 		2,     (void*)&SlaveCarData.Location}, //满足在d3跳转300  Coordinate[1]='1'跳转300
////			{800, 	ACT_JUMP_IF,        400, 		3,     (void*)&SlaveCarData.Location}, //满足在d3跳转300  Coordinate[1]='1'跳转300
////			{600, 	ACT_JUMP_IF,        450, 		4,     (void*)&SlaveCarData.Location}, //满足在d3跳转300  Coordinate[1]='1'跳转300
////			{0,   ACT_CHECK_TIMEOUT, 	400,  	 60000,   NULL},
////			{0,   ACT_JUMP,          	600,  		0,      NULL},
////
////	// --- 11. 右转倒车入库 -D7
////			{300, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
////			{0, 	ACT_GO,          	 GoSpeed, 	 	8,     NULL},
////			{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
////			{0, 	ACT_RIGHT,     	 	TurnSpeed, 		0,     NULL},	//右转
////			{0, 	ACT_TRACKMP,       TrackSpeed, 		24,     NULL},	//循迹
////			{0, 	ACT_BACK,          GoSpeed, 	 	59,     NULL},	//后退
////			{0, 	ACT_JUMP,        	  800, 	  	0,     NULL},	//不满足跳转110(无条件跳转)
////
////
////	// --- 12. 右转倒车入库 -D2
////			{350, 	ACT_DELAY,       	30000,     		0,     NULL},
////			{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
////			{0, 	ACT_GO,          	 GoSpeed, 	 	8,     NULL},
////			{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},
////			{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
////			{0, 	ACT_GO,          	 GoSpeed, 	 	8,     NULL},
////			{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},
////			{0, 	ACT_TRACKMP,       TrackSpeed, 		24,     NULL},	//循迹
////			{0, 	ACT_BACK,          GoSpeed, 	 	40,     NULL},	//后退
////			{0, 	ACT_JUMP,        	  800, 	  	0,     NULL},	//不满足跳转110(无条件跳转)
////
////	// --- 12. 右转倒车入库 -D3
////			{400, 	ACT_DELAY,       	30000,     		0,     NULL},
////			{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
////			{0, 	ACT_GO,          	 GoSpeed, 	 	8,     NULL},
////			{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},
////			{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
////			{0, 	ACT_GO,          	 GoSpeed, 	 	8,     NULL},
////			{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},
////			{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},
////			{0, 	ACT_TRACKMP,       TrackSpeed, 		24,     NULL},	//循迹
////			{0, 	ACT_BACK,          GoSpeed, 	 	40,     NULL},	//后退
////			{0, 	ACT_JUMP,        	  800, 	  	0,     NULL},	//不满足跳转110(无条件跳转)
////
////	// --- 13. 右转倒车入库 -D4
////			{450, 	ACT_DELAY,       	30000,     		0,     NULL},
////			{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
////			{0, 	ACT_GO,          	 GoSpeed, 	 	8,     NULL},
////			{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},
////			{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
////			{0, 	ACT_GO,          	 GoSpeed, 	 	8,     NULL},
////			{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},
////			{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL},
////			{0, 	ACT_GO,          	 GoSpeed, 	 	8,     NULL},
////			{0, 	ACT_LEFT,     	 	TurnSpeed, 		0,     NULL},
////			{0, 	ACT_TRACKMP,       TrackSpeed, 		24,     NULL},	//循迹
////			{0, 	ACT_BACK,          GoSpeed, 	 	40,     NULL},	//后退
////			{0, 	ACT_JUMP,        	  800, 	  	0,     NULL},	//不满足跳转110(无条件跳转)
////
////
////
////
////
////	// --- 19发送无线充电码
////			{800, 	ACT_DELAY,       	50,     		0,     NULL},
////			{0, 	ACT_CMD,         	   0,        0,     (void*)Command_sendWire},
////			{0, 	ACT_END,          0, 	 	0,     NULL},
//
//
//};







/*执行任务表2 23年真题*/
//const TaskStep_t MyMission[] = {
//
//
//
//		// --- 1. 起步开始计时and下降车库 ---
//		{0, 	ACT_CMD,         	0,       	0,     (void*)Command_StartTim},  	   // 开始计时
//		{0, 	ACT_DELAY,       	200,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)CommandCarPtB1},  	   // 降车库
//		{0, 	ACT_DELAY,       	200,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)CommandCarPtB1},  	   // 降车库
//		{0, 	ACT_DELAY,       	22000,     	0,     NULL}, //等22s下降
//		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_GetPortBFloor},  	   // 获取车库层数
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_GetPortBFloor},  	   // 获取车库层数
//		{0, 	ACT_DELAY,       	100,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_GetPortBFloor},  	   // 获取车库层数
//		{0, 	ACT_WAIT_VAL_EQ,   	99999,    	1,     (void*)&CarPortFlag}, /*超时99s跳过(保险),只有接收到车库层数为1才走*/
//
//
//		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
//		{0, 	ACT_GO,          GoSpeed, 	 	8,     NULL},	/*前进*/
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
//		{0, 	ACT_RIGHT,     	 TurnSpeed, 	0,     NULL},	/*右转*/
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
//
//
//
//		// --- 2. ETC闸门前徘徊 ---
//		{0, 	ACT_CUSTOM_LOOP,    0, 			0,     (void *)Action_WaitGateOpen},	/*前进后退直到etc开门*/
//		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
//		{0, 	ACT_GO,          GoSpeed, 	 	8,     NULL},	/*前进*/
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
//
//
//		// --- 3. 智能公交站前 ---
//		{0, 	ACT_RIGHT,     	 TurnSpeed, 	0,     NULL},	/*右转*/
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
//		{0, 	ACT_GO,          GoSpeed, 	 	7,     NULL},	/*前进*/
//			/*公交站功能:......*/
//			{0, 	ACT_DELAY,       	50,     	0,     NULL},
//			{0, 	ACT_CMD,         0,        0,     (void*)Command_BusReportRandom}, // 命令公交站开语音
//			{0, 	ACT_DELAY,       	50,     	0,     NULL},
//			{0, 	ACT_CMD,         0,        0,     (void*)GetVoicNum}, // 开启语音识别
//
//
//		{0, 	ACT_BACK,          GoSpeed, 	 	3,     NULL},	/*后退*/
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
//		{0, 	ACT_LEFT,        TurnSpeed, 0,     NULL},  /*左转 背对交通通站*/
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_LEFT,        TurnSpeed, 0,     NULL},  /*左转 背对交通站*/
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         	0,        	0,     (void*)Command_BusCheckTem},  	   // 请求公交站回传温度
//
//
//
//
//		// --- 4. 到静态显示标志物-面向他 ---
//		{0, 	ACT_TRACKMP,       TrackSpeed, 	28,     NULL}, /*循迹*/
//		{0, 	ACT_CMD,         0,        0,     (void*)Command_SlaveCarSTep}, // 给从车发送温度
//		{0, 	ACT_DELAY,       	50,     	0,     NULL},
//		{0, 	ACT_CMD,         0,        0,     (void*)Command_SlaveCarSTep}, // 给从车发送温度
//		{0, 	ACT_DELAY,       	600,     	0,     NULL},
//		{0, 	ACT_MPRIGHT,       MPTurnSpeed, 	92,     NULL},	/*码盘右转*/
//
//			/*测距静态标志位物:..........*/
//			{0, 	ACT_DELAY,       	1000,     	0,     NULL},	/*等停稳*/
//			{0, 	ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上
//			{0, 	ACT_DELAY,       	50,     	0,     NULL},
//			{0, 	ACT_CMD,         	0,        0,     (void*)Command_LEDShowDis}, // 把超声波距离显示到led屏幕上
//
//			/*二维码扫描静态标志物:..........*/
//			{0, 	ACT_DELAY,       	1000,     	0,     NULL},
//			{0, 	ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//			{0, 	ACT_DELAY,       	500,     	0,     NULL},
//			{0, 	ACT_CMD,         	0,        0,     (void*)Command_AndroidQR}, // 开启二维码扫描
//
//		{0, 	ACT_LEFT,        TurnSpeed, 0,     NULL},  /*左转*/
//
//
//
//
//		// --- 5. 读卡1 ---
//		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
//		{0, 	ACT_GO,          GoSpeed, 	 	7,     NULL},	/*前进*/
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL}, /*循迹*/
//		{0, 	ACT_GO,          GoSpeed, 	 	8,     NULL},	/*前进*/
//		{0, 	ACT_DELAY,       	 800,     	0,     NULL}, /*等停稳*/
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti23_RF1Read},   // 读取卡片结果
//		{0, 	ACT_DELAY,       	 900,     	0,     NULL},
//
//			/*解密........*/
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti23_RF1Slove},   // 解密
//
//		// --- 6. 到多功能显示标志物A(B跳过) ---
//		{0, 	ACT_GO,          GoSpeed, 	 	1,     NULL},	/*前进*/
//		{0, 	ACT_RIGHT,     	 TurnSpeed, 	0,     NULL},	/*右转*/
//
//			/*多功能任务:.............*/
//			{0, 	ACT_DELAY,       	 2000,     	0,     NULL},
//			{0, 	ACT_CMD,         	   0,        0,     (void*)Command_AndroidColor},   // 颜色识别
//			{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//			{0, 	ACT_CMD,         	   0,        0,     (void*)Command_AndroidColor},   // 颜色识别
//			{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//			{0, 	ACT_CMD,         	   0,        0,     (void*)Command_Androidshape},   // 形状识别
//			{0, 	ACT_DELAY,       	 800,     	0,     NULL},
//			{0, 	ACT_CMD,         	   0,        0,     (void*)Command_Androidshape},   // 形状识别
//			{0, 	ACT_DELAY,       	 1000,     	0,     NULL},
//			{0, 	ACT_CMD,         	   0,        0,     (void*)Sendyanxing},   // 颜色识别Command_TFTBShowHex
//			{0, 	ACT_DELAY,       	  50,     	0,     NULL},
//			{0, 	ACT_CMD,         	   0,        0,     (void*)Sendyanxing},
//
//
//		// --- 7. 到特殊地形 ---
//		{0, 	ACT_LEFT,        	TurnSpeed, 		0,     NULL},  /*左转*/
//		{0, 	ACT_TRACKMP,      	TrackSpeed, 	15,     NULL}, /*循迹*/
//		{0, 	ACT_BACK,          	GoSpeed, 	 	33,     NULL},	/*后退*/
//		{0, 	ACT_TRACKRFID,      TrackSpeed, 	0,     NULL}, /*循迹*/
//		{0, 	ACT_GO,          	GoSpeed, 	 	6,     NULL},	/*前进*/
//
//
//		{0, 	ACT_LEFT,        	TurnSpeed, 		0,     NULL},  /*左转*/
//		{0, 	ACT_BACK,          	GoSpeed, 	 	5,     NULL},	/*后退*/
//
//			/*过特殊地形*/
//			{0, 	ACT_GO,          	GoSpeed, 	54,     NULL},	/*前进*/
//			{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL},  /*循迹*/
//
//
//		// --- 8. 读卡2 ---
//		{0, 	ACT_GO,          GoSpeed, 	 	8,     NULL},	/*前进*/
//		{0, 	ACT_DELAY,       	 700,     	0,     NULL}, /*等停稳*/
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti23_RF2Read},   // 读取卡片结果
//		{0, 	ACT_DELAY,       	 900,     	0,     NULL}, /*等停稳*/
//			/*解密........*/
//			{0, 	ACT_CMD,         	   0,        0,     (void*)Ti23_RF2Slove},   // 读取卡片结果
//
//
//		// --- 9. 道闸(开启码为车牌识别，现跳过) ---
//		{0, 	ACT_LEFT,        TurnSpeed, 0,     NULL},  /*左转*/
//			/*道闸开启函数:..........*/
//			{0, 	ACT_CMD,         	   0,        0,     (void*)CommandGateTop},   // 读取卡片结果
//			{0, 	ACT_DELAY,       	 500,     	0,     NULL}, /*等停稳*/
//			{0, 	ACT_CMD,         	   0,        0,     (void*)CommandGateLast},   // 读取卡片结果
//
//		{0, 	ACT_TRACKRFID,       TrackSpeed, 	0,     NULL}, /*循迹*/
//		{0, 	ACT_GO,          GoSpeed, 	 	8,     NULL},	/*前进*/
//		{0, 	ACT_DELAY,       	 100,     	0,     NULL},
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Command_SlaveCarStart},   // 发送从车启动标志
//
//		// --- 10. 读卡3 ---
//		{0, 	ACT_DELAY,       	 900,     	0,     NULL}, /*等停稳*/
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Ti23_RF3Read},   // 读取卡片结果
//		{0, 	ACT_DELAY,       	 700,     	0,     NULL}, /*等停稳*/
//			/*解密........*/
//			{0, 	ACT_CMD,         	   0,        0,     (void*)Ti23_RF3Slove},   // 读取卡片结果
//			{0, 	ACT_DELAY,       	 100,     	0,     NULL},
//			{0, 	ACT_CMD,         	   0,        0,     (void*)Command_SlaveCarStart},   // 发送从车启动标志
//
//		// --- 11. 行驶到交通灯B前 ---
//		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Command_SlaveCarStart},   // 发送从车启动标志
//		{0, 	ACT_DELAY,       	 50,     	0,     NULL},
//		{0, 	ACT_GO,          GoSpeed, 	 	7,     NULL},	/*前进*/
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Command_SlaveCarStart},   // 发送从车启动标志
//		{0, 	ACT_DELAY,       	 50,     	0,     NULL},
//		{0, 	ACT_RIGHT,     	 TurnSpeed, 	0,     NULL},	/*右转*/
//		{0, 	ACT_CMD,         	   0,        0,     (void*)Command_SlaveCarStart},   // 发送从车启动标志
//		{0, 	ACT_DELAY,       	 50,     	0,     NULL},
//		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
//		{0, 	ACT_GO,          GoSpeed, 	 	6,     NULL},	/*前进*/
//		{0, 	ACT_RIGHT,     	 TurnSpeed, 	0,     NULL},	/*右转*/
//		{0, 	ACT_TRACKMP,       TrackSpeed, 	15,     NULL}, /*循迹*/
//		{0, 	ACT_BACK,          GoSpeed, 	 	28,     NULL},	/*后退*/
//
//		// --- 12. 交通灯B识别 ---
//			//....
//			{0, 	ACT_DELAY,       	 1000,     	0,     NULL}, /*等停稳*/
//			{5, ACT_CMD,         0,        0,     (void*)Command_TrafficBInMode}, // 开启识别
//			{6, ACT_DELAY,       1500,     0,     NULL},                          // 等1.5秒让摄像头稳定
//			{7, ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令1
//			{8, ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令2
//			{9, ACT_CMD,         0,        0,     (void*)Command_AndroidTraffic}, // 发送识别指令3
//			// 【关键】等待 AndroidFlag 变动，超时 3000ms
//			{10, ACT_WAIT_NEQ0,   3000,     0,     (void*)&AndroidFlag}, /*超时3s跳过，如果接收到AndroidFl发送结果*/
//			{11, ACT_CMD,         0,        0,     (void*)Command_TrafficBSend},   // 发送结果
//			{12, ACT_CMD,         0,        0,     (void*)Command_TrafficBSend},   // 发送结果
//
//
//
//		// --- 13. 到智能路灯前面 ---
//		{0, 	ACT_GO,          GoSpeed, 	 	10,     NULL},	/*前进*/
//		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
//		{0, 	ACT_GO,          GoSpeed, 	 	7,     NULL},	/*前进*/
//		{0, 	ACT_TRACK,       TrackSpeed, 	0,     NULL}, /*循迹*/
//		{0, 	ACT_GO,          GoSpeed, 	 	8,     NULL},	/*前进*/
//		{0, 	ACT_LEFT,        TurnSpeed, 	0,     NULL},  /*左转*/
//
//
//		// --- 14. 调节智能路灯 ---
//			//....
//			{12, ACT_CMD,         0,        0,     (void*)Ti2_CommandLight},   // 发送结果
//
//		// --- 15. 倒车入库 ---
//		{0, 	ACT_LEFT,        TurnSpeed, 		0,     NULL},  /*左转*/
//		{0, 	ACT_TRACKMP,       TrackSpeed, 		24,     NULL}, /*循迹*/
//		{0, 	ACT_BACK,          GoSpeed, 	 	59,     NULL},	/*后退*/
//
//};

/*执行任务表1 25年样题1*/
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

                case ACT_RESET_TIMER:
                	TimerStart2 = HAL_GetTick();
                	StepIdx++;
                	StepState = 0;
                	break;

                case ACT_CHECK_TIMEOUT:
                    if(HAL_GetTick() - TimerStart2 > step->Param2)
                    {
                        StepIdx = Find_Index_By_ID(step->Param1); // 超时了！跳转到错误处理
                    }
                    else
                    {
                        StepIdx++; // 没超时，继续下一行
                    }
                    break;


				// --- 跳转类 (核心更新) ---
				case ACT_JUMP:		// 无条件跳转: Param1 = 目标ID
					StepIdx = Find_Index_By_ID(step->Param1);
					// StepState 保持 0，下次循环直接执行新的一行
					break;

                case ACT_JUMP_IF:	//  --- 条件跳转: Param1 = 目标ID, Param2 = 匹配值, ArgPtr = 变量地址
                    uint8_t current_val = *(uint8_t*)(step->Ptr);
                    if (current_val == (uint8_t)step->Param2)
                    {
                        StepIdx = Find_Index_By_ID(step->Param1); // 满足了，跳！
                        StepState = 0;
                        return;
                    }
                    else
                    {
                        StepIdx++; // 不满足，走下一行
                        StepState = 0;
                    }

					break;

                case ACT_JUMP_IF_NOT: // 如果 变量 != Param2，则跳转到 Param1
                    if(step->Ptr != NULL)
                    {
                        uint8_t val = *(uint8_t*)(step->Ptr);
                        if(val != (uint8_t)step->Param2)
                        { // 【注意这里是不等于】
                            StepIdx = Find_Index_By_ID(step->Param1);
                            return;
                        }
                    }
                    StepIdx++;
                    break;

                case ACT_END:
                	Start_Flag = 0;
                	StepState = 0;
                	StepIdx = 0;
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


