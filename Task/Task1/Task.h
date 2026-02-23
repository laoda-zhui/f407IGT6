#ifndef TASK1_TASK_H_
#define TASK1_TASK_H_


#include "main.h"
#include "Motor.h"
#include "can_Tx.h"
#include "RC522.h"
#include "Ultrasonic.h"
#include "bh1750.h"
#include "infrared.h"
#include "Delay.h"
#include "can_cmd.h"
#include "Photoresistance.h"
#include "Command.h"
#include "Task_Beep.h"
#include "Voice.h"
#include <ctype.h>
#include "string.h"
#include "Lib.h"
#include "TaskFun.h"

typedef enum {
    ACT_END = 0,    // 结束标志

    // 运动类 (会自动等待 Stop_Flag)
    ACT_GO,         // 直行 (参数: 速度, 距离)
    ACT_BACK,       // 后退 (参数: 速度, 距离)
    ACT_LEFT,       // 左转 (参数: 速度)
	ACT_MPLEFT,		// 码盘左转 (参数: 速度, 角度)
    ACT_RIGHT,      // 右转 (参数: 速度)
	ACT_MPRIGHT,		// 码盘左转 (参数: 速度, 角度)
    ACT_TRACK,      // 循迹 (参数: 速度)
	ACT_TRACKTIME,	// 时间循迹(参数: 速度, 时间)
	ACT_TRACKMP,	// 码盘循迹(参数: 速度, 距离)
	ACT_TRACKRFID,  // 白卡循迹(参数: 速度)

    // 指令类 (发完即走)
    ACT_CMD,        // 调用函数 (参数: 函数指针)

    // 逻辑类
    ACT_DELAY,      // 延时等待 (参数: 毫秒)
    ACT_WAIT_NEQ0,   // 等待变量不为0 (参数: 超时ms, 变量地址)
	ACT_WAIT_VAL_EQ,   // 等待变量为指定值 (参数: 超时ms, 指定值, 变量地址)
	ACT_RESET_TIMER,	//重置检查时间
    ACT_CHECK_TIMEOUT,  // 检查是否超时，若超时则跳转 (没超时下一行)

	// --- 跳转类 ---
	ACT_JUMP,			// 无条件跳转: Param1 = 目标ID
    ACT_JUMP_IF,        // 条件跳转: Param1 = 目标ID, Param2 = 匹配值, ArgPtr = 变量地址
	ACT_JUMP_IF_NOT		// 条件跳转: Param1 = 目标ID, 变量 != Param2，则跳转到 Param1,ArgPtr = 变量地址

} ActionType_t;

// --- 2. 任务步骤结构体 ---
typedef struct {
    uint16_t     StepID;  // 【新增】本步骤的唯一ID (标签)，用于被跳转
    ActionType_t Type;   // 动作类型
    int32_t      Param1; // 参数1 (速度 / 时间 / 超时)
    int32_t      Param2; // 参数2 (距离 / 角度)
    void*        Ptr;    // 指针 (函数指针 或 变量地址)

} TaskStep_t;



/*RF卡数据处理*/
void RFID_Slove();
void RFID_Slove2();


/*任务运动函数*/
void Task_Engine_Init(void);
void Task_Engine_Run(void);
uint16_t Find_Index_By_ID(uint16_t target_id);

void SendLight(void);
void SendBat(void);

extern uint8_t VoiceNumber;
extern uint8_t LightInit;
extern uint8_t Sector; //扇区地址 第几扇区
extern uint8_t Block;	//块地址
extern char Coordinate[10]; //位置字符串
extern uint8_t SendBattery[3];

extern char QR1Num[50];	//解密后的数组二维码1
extern char QR2Num[50]; //解密后的数组二维码2









#endif /* TASK1_TASK_H_ */











