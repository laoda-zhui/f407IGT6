#include "TaskFun.h"



uint8_t LightInit=0; /*路灯初始档位*/
uint8_t VoiceNumber=0; /*公交语音号*/
float Result;

/*通用任务功能函数*/

/**************************************************************************
函数功能：命令-开启语音识别并保存语音编号
入口参数：无
返回  值：无
**************************************************************************/
void GetVoicNum(void)
{
	VoiceNumber = Voice_ASR();
}

/**************************************************************************
函数功能：命令-获取路灯初始档位
入口参数：Gare:档位
返回  值：无
**************************************************************************/
void Ti24_2SendGetLight(void)
{
	LightInit = Command_LightAuto((int)Result%4+1); //档位自己设置 1 2 3 4;
}

/**************************************************************************
函数功能：命令-获取路灯初始档位
入口参数：Gare:档位
返回  值：无
**************************************************************************/
void Ti24_4SendGetLight(void)
{
	int D;
	uint8_t Result=0;

	int base = (int)(Dis_Buf/60);

	D = (int)pow(base, base);
	D = base % 4+1;
	Result = (uint8_t)D;

	LightInit = Command_LightAuto(Result);
}


/**************************************************************************
函数功能：命令-发送给从车初始档位
入口参数：Gare:档位
返回  值：无
**************************************************************************/
void SendToSlaveLight(void)
{
	Command_SlaveCarLight(LightInit);
}


/**************************************************************************
函数功能：命令-发送给交通灯B红灯
入口参数：无
返回  值：无
**************************************************************************/
void SendTrafficBRed(void)
{
	Command_TrafficBSendC(0);
}
/**************************************************************************
函数功能：命令-发送给交通灯B红灯
入口参数：无
返回  值：无
**************************************************************************/
void SendTrafficARed(void)
{
	Command_TrafficASendC(0);
}
/**************************************************************************
函数功能：命令-发送语音编号给自动评分系统
入口参数：无
返回  值：无
**************************************************************************/
void SendVoiceNum(void)
{
	Command_Autosystem(VoiceNumber);
}

/**************************************************************************
函数功能：命令-到达指定车库
入口参数：无
返回  值：无
**************************************************************************/
void SendCPortb(void)
{
	Command_CarPortB(LightInit);
}

/**************************************************************************
函数功能：命令-保存超声波的DIS距离
入口参数：无
返回  值：无
**************************************************************************/
void Command_SaveDis(void)
{
	Dis_Buf = (uint16_t)(Distance*10.0); //保存测量的超声波距离(mm)
}

/**************************************************************************
函数功能：命令-立体显示距离模式
入口参数：无
返回  值：无
**************************************************************************/
void showDis3D(void)
{
	Command_3DShowDis2(0,4);
}








/*23年国赛样题1 - 解密*/

void Ti23_1SloveRf(void)
{
	char Num[40]={0};

	char buf[17]={0};

	uint8_t index=0;
	memcpy(buf,READ_RFID,16);
	buf[16] = '\0';





}









/*24年样题8 - 解密*/

/**************************************************************************
函数功能：命令-读取RFID1
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_8ReadRF1(void)
{
	RC522(14, RFID_Read);
}

/**************************************************************************
函数功能：命令-解码RFID1
入口参数：无
返回  值：无
**************************************************************************/
uint8_t Ti24_8ReadTure=0;
void Ti24_8SloveRF1(void)
{
	char RFBuf[17]={0};
	memcpy(RFBuf,READ_RFID,16);
	RFBuf[16] = '\0';

	char *pstr1 = strstr(RFBuf, "RFID01");
	if(pstr1 != NULL)
	{
		Ti24_8ReadTure = 1;
	}

	char txdata[70];
	sprintf(txdata, "%d\n",Ti24_8ReadTure);
	CAN_TxtoDisplay(txdata,strlen(txdata));
}

/**************************************************************************
函数功能：命令-读取RFID2
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_8ReadRF2(void)
{
	if(Ti24_8ReadTure == 1)
	{
		RC522(18, RFID_Read);
		Ti24_8ReadTure = 0;
	}
}
/**************************************************************************
函数功能：命令-解码RFID2
入口参数：无
返回  值：无
**************************************************************************/
uint8_t Ti24_8Battery[3];
void Ti24_8SloveRF2(void)
{
	char RFBuf[50];
	char Number_PaiXu[10];

	SloveTemplate1();
	int len=0;
	int pos=0;

	for(uint8_t index=0; index<E01_Count;index++)
	{
		len = strlen(E01_Results[index]);
		memcpy(&RFBuf[pos], &E01_Results[index], len);
		pos+=len;
	}

	SloveTemplate6_Sort(RFBuf,Number_PaiXu);

	Ti24_8Battery[0] = CharToHex(Number_PaiXu[0])<<4 | CharToHex(Number_PaiXu[1]);
	Ti24_8Battery[1] = CharToHex(Number_PaiXu[2])<<4 | CharToHex(Number_PaiXu[3]);
	Ti24_8Battery[2] = CharToHex(Number_PaiXu[4])<<4 | CharToHex(Number_PaiXu[5]);


	char txdata[150];
	sprintf(txdata, "%s\n",Number_PaiXu);
	CAN_TxtoDisplay(txdata,strlen(txdata));


}

/**************************************************************************
函数功能：命令-开启无线充电
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_8SendWife(void)
{
	Command_WireStart(Ti24_8Battery[0], Ti24_8Battery[1], Ti24_8Battery[2]);

}






/*24年样题7 - 解密*/

/**************************************************************************
函数功能：命令-读取RFID1
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_7ReadRF1(void)
{
	RC522(6, RFID_Read);

}

/**************************************************************************
函数功能：命令-解码RFID1
入口参数：无
返回  值：无
**************************************************************************/
int Ti24_7=0;
void Ti24_7SloveRF1(void)
{
	SloveTemplate_OnlyNumbers();

	for(uint8_t i=0;i < E04_Count;i++)
	{
		Ti24_7 = Ti24_7*10 + CharToHex(Number_Results[i]);

	}


	char txdata[70];
	sprintf(txdata, "%d\n",Ti24_7);
	CAN_TxtoDisplay(txdata,strlen(txdata));
}

/**************************************************************************
函数功能：命令-降车库
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_7SendCPortb(void)
{
	Command_CarPortB(1);
}

/**************************************************************************
函数功能：命令-降车库
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_7SendCPortb2(void)
{
	int R;
	R = Ti24_7 % LightInit + 1;
	Command_CarPortB(R);
}





/*24年样题6 - 解密*/

/**************************************************************************
函数功能：命令-读取RFID1
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_6ReadRF1(void)
{
	RC522(22, RFID_Read);

}

/**************************************************************************
函数功能：命令-解码RFID1
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_6SloveRF1(void)
{
	SloveTemplate_OnlyNumbers();

	uint8_t Shanqu=0;
	uint8_t kuai=0;
	Shanqu = CharToHex(Number_Results[0]);
	kuai = CharToHex(Number_Results[1]);

	char txdata[50];
	sprintf(txdata, "%d,%d\n",Shanqu,kuai);
	CAN_TxtoDisplay(txdata,strlen(txdata));
}

/**************************************************************************
函数功能：命令-读取RFID2
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_6ReadRF2(void)
{
	uint8_t Shanqu=0;
	uint8_t kuai=0;
	Shanqu = CharToHex(Number_Results[0]);
	kuai = CharToHex(Number_Results[1]);
	RC522(Shanqu*4+kuai,RFID_Read);
}


/**************************************************************************
函数功能：命令-解码RFID2 提取公式
入口参数：无
返回  值：无
**************************************************************************/
uint8_t Ti24_6M=0;
void Ti24_6SloveRF2(void)
{
	char RFBuf[17]={0};
	memcpy(RFBuf,READ_RFID,16);
	RFBuf[16] = '\0';
	float R = 0;
	R = Solve_Math_String(RFBuf);
	Ti24_6M = (uint8_t)R;

	 char txdata[50];
	 sprintf(txdata, "%d\n",Ti24_6M);
	 CAN_TxtoDisplay(txdata,strlen(txdata));

}

/**************************************************************************
函数功能：命令-从车发送白卡解密信息
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_6SendRFID(void)
{
	Command_SlaveCarSendRFID(93);
}


/*24年样题5 - 解密跟样题4一样*/


/*24年样题4 - 解密*/

/**************************************************************************
函数功能：命令-读取RFID1
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_4ReadRF1(void)
{
	RC522(22, RFID_Read);
}

uint16_t Ti24_4Num=0;
/**************************************************************************
函数功能：命令-解码RFID1
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_4SloveRF1(void)
{
	char buf[17]={0};
	buf[16] = '\0';
	char Num[6]={0};

	uint8_t Index=0;

	memcpy(buf,READ_RFID,16);

	char *pstr1 = strpbrk(buf, "0123456789");

	while(pstr1 != NULL)
	{
		Num[Index]= (*pstr1);
		Index++;
		pstr1 = strpbrk(pstr1+1, "0123456789");
	}
	Num[Index] = '\0';

    for(uint8_t i = 0; i < Index; i++)
    {
        Ti24_4Num = Ti24_4Num * 10 + CharToHex(Num[i]);  // 简单可靠
    }

    char txdata[100];
    sprintf(txdata, "%d",Ti24_4Num);
    CAN_TxtoDisplay(txdata, strlen(txdata));

}



/**************************************************************************
函数功能：命令-到达指定车库
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_4SendCPortb(void)
{
	uint8_t CarP=0;
	CarP = Ti24_4Num % LightInit+1;

	Command_CarPortB(CarP);
}







/*24年样题3 - 解密*/
/**************************************************************************
函数功能：命令-读取RFID1
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_3ReadRF1(void)
{
	RC522(28, RFID_Read);
}

/**************************************************************************
函数功能：命令-解码RFID1
入口参数：无
返回  值：无
**************************************************************************/
uint8_t Ti24_3Shanqu=0;
uint8_t Ti24_3Kuai=0;
void Ti24_3SloveRF1(void)
{
	char buf[17]={0};
	memcpy(buf,READ_RFID,16);
	buf[16] = '\0';

	char *pstr = strpbrk(buf, "0123456789");
	if(pstr != NULL)
	{
		Ti24_3Shanqu = CharToHex(*pstr);
		pstr = strpbrk(pstr+1, "0123456789");
		if(pstr != NULL)
		{
			Ti24_3Kuai = CharToHex(*pstr);
		}

	}

	char Txdata[50];
	sprintf(Txdata, "%d %d\n",Ti24_3Shanqu,Ti24_3Kuai);
	CAN_TxtoDisplay(Txdata, strlen(Txdata));

}

/**************************************************************************
函数功能：命令-读取RFID2
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_3ReadRF2(void)
{
	RC522(Ti24_3Shanqu*4+Ti24_3Kuai, RFID_Read);
}




/**************************************************************************
函数功能：命令-解码RFID2 提取公式
入口参数：无
返回  值：无
**************************************************************************/
int Port=0;
void Ti24_3SloveRF2(void)
{
	char Txdata[50];
	char buf[17]={0};
	float MResult=0;
	memcpy(buf,READ_RFID,16);
	buf[16] = '\0';

	MResult = Solve_Math_String(buf);

	sprintf(Txdata, "%.1f\n",MResult);
	CAN_TxtoDisplay(Txdata, strlen(Txdata));

	float R = fmod((MResult*MResult), 4.0) + 1;
	R = fmod(R, 4.0);
	Port =  (int)round(R);



	sprintf(Txdata, "%d\n",Port);
	CAN_TxtoDisplay(Txdata, strlen(Txdata));

}


/**************************************************************************
函数功能：命令-到达解密出的车库层数
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_3SendCPortb(void)
{
	Command_CarPortB(Port);
}
/**************************************************************************
函数功能：命令-获取路灯初始档位
入口参数：Gare:档位
返回  值：无
**************************************************************************/
void Ti24_3SendGetLight(void)
{
	LightInit = Command_LightAuto(1); //档位自己设置 1 2 3 4;
}
/**************************************************************************
函数功能：命令-获取路灯初始档位
入口参数：Gare:档位
返回  值：无
**************************************************************************/
void Ti24_6SendGetLight(void)
{
	int R=0,base;
	base = Dis_Buf/60;
	R = (int)pow(base,base)%4+1;


	LightInit = Command_LightAuto(R); //档位自己设置 1 2 3 4;
}
/**************************************************************************
函数功能：命令-获取路灯初始档位
入口参数：Gare:档位
返回  值：无
**************************************************************************/
void Ti24_8SendGetLight(void)
{
	int Gare=0;
	float R=0;
	R = Dis_Buf /60.0;
	R = exp(3.0)*R;
	Gare = (int)sqrt(R);
	Gare = Gare%4+1;


	LightInit = Command_LightAuto(Gare); //档位自己设置 1 2 3 4;
}












/*24年样题2 - 解密*/

/**************************************************************************
函数功能：命令-读取RFID1
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_2ReadRF1(void)
{
	RC522(13, RFID_Read);
}
/**************************************************************************
函数功能：命令-读取RFID2
入口参数：无
返回  值：无
**************************************************************************/
void Ti24_2ReadRF2(void)
{
	RC522(30, RFID_Read);
}
/**************************************************************************
函数功能：命令-解码RFID1
入口参数：无
返回  值：无
**************************************************************************/

void Ti24_2SloveRF1(void)
{
	uint8_t Ti24_2Num[3];

	uint8_t index=0;
	char buf[13]={0};
	char buf2[10]={0};
	memcpy(buf,READ_RFID,12);
	buf[12] = '\0';
	buf2[9] = '\0';

	char* pStart = strchr(buf, '[')+1;
	char* pEnd   = strchr(buf, ']');
	if(pStart != NULL && pStart != NULL)
	{
		int len = pEnd - pStart;

		memcpy(buf2, pStart,len);
		buf2[len] = '\0'; // 封口
		char* pstr1 = strpbrk(buf2, "0123456789");
		while(pstr1 != NULL)
		{
			Ti24_2Num[index++] = CharToHex(*pstr1);
			pstr1 = strpbrk(pstr1+1, "0123456789");
		}
	}


	for(uint8_t i=0;i < index-1;i++)
	{
		for(uint8_t j=0;j < index-1;j++)
		{
			if(Ti24_2Num[j]> Ti24_2Num[j+1])
			{
				char temp = Ti24_2Num[j+1];
				Ti24_2Num[j+1] = Ti24_2Num[j];
				Ti24_2Num[j] = temp;
			}
		}
	}

	char pbuf[100];
	sprintf(pbuf,"%d %d %d\n", Ti24_2Num[0],Ti24_2Num[1],Ti24_2Num[2]);
	CAN_TxtoDisplay(pbuf, strlen(pbuf));

	AssValue(Ti24_2Num[0], Ti24_2Num[1], Ti24_2Num[2]);


}
/**************************************************************************
函数功能：命令-解码RFID2 提取公式
入口参数：无
返回  值：无
**************************************************************************/
float Result=0;
void Ti24_2SloveRF2(void)
{
	char buf[17]={0};

	char Gongshi[20];

	memcpy(buf,READ_RFID,16);
	buf[16] = '\0';

	char* pStart = strchr(buf, '{')+1;
	char* pEnd   = strchr(buf, '}');
	if(pStart != NULL && pStart != NULL)
	{
		int len = pEnd - pStart;
		memcpy(Gongshi, pStart, len);

		CAN_TxtoDisplay(Gongshi, strlen(Gongshi));
	}

	//数学运算
	Result = Solve_Math_String(Gongshi);
	char Rs[10];
	sprintf(Rs,"\n%.2f",Result);
	CAN_TxtoDisplay(Rs, strlen(Rs));
}






















/*卡1数据处理 25样题1*/
char Coordinate[40];
char Sector;
char Block;

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
/*卡2数据处理 25样题1*/
uint8_t SendBattery[3];

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

/*23真题 卡1处理*/
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
/*23真题 卡2读和处理*/

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

/*23真题 卡3读和处理*/
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
/*23年真题 二维码1*/
char QR1Num[50]; //解密后的数组二维码1
char QR2Num[50]; //解密后的数组二维码2
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




/*24年样题1 二维码*/
char Ti24_1QRNum[3];
void Ti24_RQSlove(void)
{
	char *pstr1 = strpbrk(RQData.RQ1Buf, "0123456789");
	if(pstr1 != NULL)
	{
		Ti24_1QRNum[0] = CharToHex(*pstr1); //扇区
		char *pstr2 = strpbrk(pstr1+1, "0123456789");
		if(pstr2 != NULL)
		{
			Ti24_1QRNum[1] = CharToHex(*pstr2);	//块
			Ti24_1QRNum[2] = '\0';
		}
	}
}


/*24年样题1 RFID*/
uint8_t ReadTrue=0;
void Ti24_1ReadRFID1(void) //先看看哪个是有效的卡
{
	RC522(14, RFID_Read);
}
void Ti24_1ReadRFID2(void)
{
	if(ReadTrue == 1)
	{
		RC522((Ti24_1QRNum[0]+1)*4+Ti24_1QRNum[1], RFID_Read);
		ReadTrue = 0;
	}

}

void Ti24_1SloveRFID1(void)
{
	char RFBuf[17];
	RFBuf[16] = '\0';
	memcpy(RFBuf,READ_RFID,16);

	char*p1 = strstr(RFBuf,"CARD01");
	if(p1 != NULL)
	{
		ReadTrue = 1;
	}
}

uint8_t Ti24_1SendBat[3];
void Ti24_1SloveRFID2(void)
{
	uint8_t RFNum[6]={0};
	uint8_t Index=0;
	uint8_t Count[16]={0};

	char RFBuf[16];
	memcpy(RFBuf, READ_RFID, 15);
	RFBuf[15] = '\0';

	char *pStart = strchr(RFBuf, '{');
	char *pEnd = strchr(RFBuf, '}');

	if(pStart == NULL || pEnd == NULL){return;}

	for(char *p=pStart+1;p<pEnd;p++) /*记录0-F出现的频率*/
	{
		uint8_t Value = CharToHex(*p);
		if(*p >= '0' && *p <= 'F')
		{
			Count[Value]++;
		}
	}

	for(uint8_t i=0;i<16;i++) /*去掉重复字母数字重新排序*/
	{
		if(Count[i] == 1)
		{
			RFNum[Index++] = i;
		}
	}

	Ti24_1SendBat[0] = RFNum[0]<<4 | RFNum[1];
	Ti24_1SendBat[1] = RFNum[2]<<4 | RFNum[3];
	Ti24_1SendBat[2] = RFNum[4]<<4 | RFNum[5];

}















