#include "can_RxSolve.h"



/**************************************************接收FIFO初始化***************************************************************/
Fifo_Drv_Struct Fifo_Info;	/*对应FIFO缓冲结构体*/
Fifo_Drv_Struct Fifo_WifiRx;
Fifo_Drv_Struct Fifo_WifiTx;
Fifo_Drv_Struct Fifo_ZigbRx;
Fifo_Drv_Struct Fifo_ZigbTx;

#define FIFOSIZE_INFO	50	/*对应FIFO缓冲数组大小*/
#define FIFOSIZE_WIFIRX	50
#define FIFOSIZE_WIFITX	50
#define FIFOSIZE_ZIGBRX	50
#define FIFOSIZE_ZIGBTX	50

uint8_t FifoBuf_Info[FIFOSIZE_INFO];	/*对应FIFO缓冲数组*/
uint8_t FifoBuf_WifiRx[FIFOSIZE_WIFIRX];
uint8_t FifoBuf_WifiTx[FIFOSIZE_WIFITX];
uint8_t FifoBuf_ZigbRx[FIFOSIZE_ZIGBRX];
uint8_t FifoBuf_ZigbTx[FIFOSIZE_ZIGBTX];

/**************************************************************************
函数功能：接收缓冲FIFO结构体指针以及清零初始化
入口参数：p:缓冲结构体,c:指定清空的值
返回  值：无
**************************************************************************/
void FifoDrv_Init(Fifo_Drv_Struct *p)
{
	if(p != _NULL) /*有效指针判断*/
	{
		if(p->ml > 1)	/*大小有效判断*/
		{
			p->wp = 0;	/*读指针索引指向数组0*/
			p->rp =  p->ml-1;	/*读指针索引指向数组n-1*/
			memset(p->buf,0,p->ml);	/*将缓存数组置零*/
		}
	}
}

/**************************************************************************
函数功能：将一个结构体的大小以及缓存数组初始化
入口参数：p:缓冲结构体,buf:缓存冲数组
返回  值：无
**************************************************************************/
void CanP_FifoInitS(Fifo_Drv_Struct *p,uint32_t l,uint8_t *buf)
{
	p->ml = l;			/*绑定大小*/
	p->buf = buf;		/*绑定缓存*/
	FifoDrv_Init(p);	/*指针移动，缓存清0*/
}

/**************************************************************************
函数功能：将所有结构体初始化
入口参数：无
返回  值：无
**************************************************************************/
void CanP_FifoInit(void)
{
	CanP_FifoInitS(&Fifo_Info,FIFOSIZE_INFO,FifoBuf_Info);
	CanP_FifoInitS(&Fifo_WifiRx,FIFOSIZE_WIFIRX,FifoBuf_WifiRx);
	CanP_FifoInitS(&Fifo_WifiTx,FIFOSIZE_WIFITX,FifoBuf_WifiTx);
	CanP_FifoInitS(&Fifo_ZigbRx,FIFOSIZE_ZIGBRX,FifoBuf_ZigbRx);
	CanP_FifoInitS(&Fifo_ZigbTx,FIFOSIZE_ZIGBTX,FifoBuf_ZigbTx);
}

/*******************************************************以上为初始化*****************************************************************/



/**************************************************接收FIFO功能函数***************************************************************/



















