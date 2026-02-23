#include "RC522.h"




#define MAXRLEN 18

static uint8_t RC522_LinkFlag=0;

uint8_t CT[2];		// 卡类型
uint8_t SN[4];		// 卡号
uint8_t READ_RFID[16];		// 存放RFID
uint8_t WRITE_RFID[16]={"0123456789ABCDE"};
uint8_t KEY_A[6]={0xff,0xff,0xff,0xff,0xff,0xff};   // A密钥
uint8_t KEY_B[6]={0xff,0xff,0xff,0xff,0xff,0xff};	// B密钥
uint8_t ADDR_Str[14]={"RFID_ADDR:01\n"};








/***************************************读写操作*********************************************/

/**************************************************************************
函数功能：RF读卡器-发送数据
入口参数：Data:发送的数据 len:数据长度(字节)
返回  值：无
**************************************************************************/
void RC522_SendData(uint8_t* Data, uint8_t len)
{
	HAL_UART_Transmit(&huart1, Data, len, 10);
}



/**************************************************************************
函数功能：RF读卡器-接收一字节数据
入口参数：RxData:接收的缓冲区
返回  值：无
**************************************************************************/
int8_t RC522_ReceiveData(uint8_t* RxData)
{
	int8_t States;
	States = HAL_UART_Receive(&huart1, RxData, 1, 10);

	switch(States)
	{
		case HAL_OK:
			States = STATUS_SUCCESS;
			break;
		case HAL_ERROR:
			States = MI_ERR;
			break;
		case HAL_BUSY:
			States = MI_ERR;
			break;
		case HAL_TIMEOUT:
			States = STATUS_IO_TIMEOUT;
			break;
	}

	return States;
}



/**************************************************************************
函数功能：RF读卡器-写寄存器
入口参数：Address:地址 value:值
返回  值：无
**************************************************************************/
int8_t RC522_WriteReg(uint8_t Address, uint8_t value)
{
    uint8_t EchoByte;
    int8_t status;
	uint8_t e = 3;

    Address &= 0x3f;   //code the first byte
	for(e = 0; e<3; e++)
	{
		RC522_SendData(&Address, 1);
		status = RC522_ReceiveData(&EchoByte);

		if(status == STATUS_SUCCESS)
		{
			if(Address == EchoByte)
			{
				RC522_SendData(&value, 1);
				break;
			}
			else
			{
				status = STARUS_ADDR_RERR;
			}
		}
	}
	return status;
}


/**************************************************************************
函数功能：RF读卡器-读RC632寄存器
入口参数：Address[IN]:寄存器地址
返回  值：无
**************************************************************************/
uint8_t RC522_ReadReg(uint8_t Address)
{
	uint8_t RegVal;
    int8_t status;
    Address = (Address & 0x3f) | 0x80;   //code the first byte
    RC522_SendData(&Address, 1);
    status = RC522_ReceiveData(&RegVal);
    if(status != STATUS_SUCCESS)
        return 0xff;
    return RegVal;
}




/**************************************************************************
函数功能：RF读卡器-清RC522寄存器位
入口参数：reg[IN]:寄存器地址 mask[IN]:清位值
返回  值：无
**************************************************************************/
int8_t RC522_ClearBitMask(uint8_t reg,uint8_t mask)
{
	int8_t tmp = 0x0;
    tmp = RC522_ReadReg(reg);
    RC522_WriteReg(reg, tmp & ~mask);  // clear bit mask
	return MI_OK;
}



/**************************************************************************
函数功能：RF读卡器-置RC522寄存器位
入口参数：reg[IN]:寄存器地址 mask[IN]:清位值
返回  值：无
**************************************************************************/
int8_t RC522_SetBitMask(uint8_t reg,uint8_t mask)
{
	int8_t tmp = 0x0;
    tmp = RC522_ReadReg(reg);
    RC522_WriteReg(reg,tmp | mask);  // set bit mask
	return MI_OK;
}






/********************************************初始化操作************************************************/


/**************************************************************************
函数功能：RF读卡器-软件复位
入口参数：无
返回  值：无
**************************************************************************/
int8_t RC522_SoftReset(void)
{
	int8_t Rt = MI_ERR;
	uint8_t Rd=0, TxData=0x55;
	uint16_t i=0;

	for(i=0; i<100; i++)
	{
		RC522_SendData(&TxData, 1);
		if(RC522_ReceiveData(&Rd) == STATUS_SUCCESS)
		{
			RC522_SendData(&TxData, 1);
			HAL_Delay(100);
			Rt = MI_OK;
			break;
		}
	}
	return Rt;
}




/**************************************************************************
函数功能：RF读卡器-关闭天线
入口参数：无
返回  值：无
**************************************************************************/
void RC522_PcdAntennaOff(void)
{
	RC522_ClearBitMask(TxControlReg, 0x03);
}



/**************************************************************************
函数功能：RF读卡器-开启天线 每次启动或关闭天险发射之间应至少有1ms的间隔
入口参数：无
返回  值：无
**************************************************************************/
void RC522_PcdAntennaOn()
{
	uint8_t i;
    i = RC522_ReadReg(TxControlReg);
    if (!(i & 0x03))
    {
    	RC522_SetBitMask(TxControlReg, 0x03);
    }
}



/**************************************************************************
函数功能：RF读卡器-设置RC522的工作方式
入口参数：无
返回  值：无
**************************************************************************/
int8_t RC522_PcdConfigISOType(uint8_t type)
{
	if(type == 'A')                     //ISO14443_A
	{
		RC522_ClearBitMask(Status2Reg,0x08);
		RC522_WriteReg(ModeReg,0x3D);//3F
		RC522_WriteReg(RxSelReg,0x86);//84
		RC522_WriteReg(RFCfgReg,0x70);   //4F
		RC522_WriteReg(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
		RC522_WriteReg(TReloadRegH,0);
		RC522_WriteReg(TModeReg,0x8D);
		RC522_WriteReg(TPrescalerReg,0x3E);
		HAL_Delay(10);
		RC522_PcdAntennaOn();
	}
	else
	{
		return ((int8_t)-1);
	}
	return MI_OK;
}


/**************************************************************************
函数功能：RF读卡器-上电复位初始化寄存器
入口参数：无
返回  值：无
**************************************************************************/
int8_t RC522_PcdReset(void)
{
	if(RC522_SoftReset()!=MI_OK)
		return MI_ERR;
	RC522_WriteReg(CommandReg,PCD_RESETPHASE);
	HAL_Delay(50);
	RC522_WriteReg(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
	RC522_WriteReg(TReloadRegL,30);
    RC522_WriteReg(TReloadRegH,0);
    RC522_WriteReg(TModeReg,0x8D);
    RC522_WriteReg(TPrescalerReg,0x3E);
    RC522_WriteReg(TxAutoReg,0x40);

    return MI_OK;
}



/**************************************************************************
函数功能：RF读卡器-获取初始化成功标志位
入口参数：无
返回  值：RC522_LinkFlag 0-还未初始化 1-舒适化成功
**************************************************************************/
uint8_t RC522_GetLinkFlag(void)
{
	return RC522_LinkFlag;
}



/**************************************************************************
函数功能：RF读卡器-初始化
入口参数：无
返回  值：MI_OK or MI_ERR
**************************************************************************/
int8_t InitRC522(void)
{
	if(RC522_PcdReset() != MI_OK)
		return MI_ERR;
	RC522_PcdAntennaOff();
	HAL_Delay(2);
	RC522_PcdAntennaOn();
	if(RC522_PcdConfigISOType( 'A' ) != MI_OK)
		return MI_ERR;
	RC522_LinkFlag = 1;
	return MI_OK;
}










/****************************************************以下为功能函数***********************************************/




/**************************************************************************
函数功能：RF读卡器-测试
入口参数：无
返回  值：无
**************************************************************************/
void RC522_LinkTest(void)
{
	RC522_ReadReg(TxControlReg);
}





/**************************************************************************
函数功能：RF读卡器-通过RC522和ISO14443卡通讯
入口参数：Command[IN]:RC522命令字
	   pInData[IN]:通过RC522发送到卡片的数据
	   InLenByte[IN]:发送数据的字节长度
	   pOutData[OUT]:接收到的卡片返回数据
	   *pOutLenBit[OUT]:返回数据的位长度
返回  值：无
**************************************************************************/
int8_t RC522_PcdComMF522(uint8_t Command,
                 uint8_t *pInData,
                 uint8_t InLenByte,
                 uint8_t *pOutData,
                 uint32_t  *pOutLenBit)
{
    int8_t status = MI_ERR;
    uint8_t irqEn   = 0x00;
    uint8_t waitFor = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint32_t i;

    switch(Command)
    {
       case PCD_AUTHENT:
          irqEn   = 0x12;
          waitFor = 0x10;
          break;

       case PCD_TRANSCEIVE:
          irqEn   = 0x77;
          waitFor = 0x30;
          break;

       default:
         break;
    }
    RC522_WriteReg(ComIEnReg,irqEn|0x80);
    RC522_ClearBitMask(ComIrqReg,0x80);
    RC522_WriteReg(CommandReg,PCD_IDLE);
    RC522_SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<InLenByte; i++)
    {
    	RC522_WriteReg(FIFODataReg, pInData[i]);
	}
    RC522_WriteReg(CommandReg, Command);
    if (Command == PCD_TRANSCEIVE)
    {
    	RC522_SetBitMask(BitFramingReg,0x80);
	}
    i = 1;//根据时钟频率调整，操作M1卡最大等待时间25ms
    uint32_t tickstart = HAL_GetTick(); /*超时标记*/
    do
    {
		n = RC522_ReadReg(ComIrqReg);
		if(HAL_GetTick() - tickstart >50)
		{
			i = 0; /*错误*/
			break;
		}
    }
    while (!(n&0x01) && !(n&waitFor));
    RC522_ClearBitMask(BitFramingReg,0x80);
    if(i != 0)
    {
		if(!(RC522_ReadReg(ErrorReg)&0x1B))
		{
			status = MI_OK;
			if (n & irqEn & 0x01)
			{
				status = MI_NOTAGERR;
			}
			if (Command == PCD_TRANSCEIVE)
			{
				n = RC522_ReadReg(FIFOLevelReg);
				lastBits = RC522_ReadReg(ControlReg) & 0x07;
				if(lastBits)
				{
					*pOutLenBit = (n-1)*8 + lastBits;
				}
				else
				{
					*pOutLenBit = n*8;
				}
				if (n == 0)
				{
					n = 1;
				}
				if(n > MAXRLEN)
				{
					n = MAXRLEN;
				}
				for (i=0; i<n; i++)
				{
					pOutData[i] = RC522_ReadReg(FIFODataReg);
				}
			}
		}
		else
		{
			status = MI_ERR;
		}
   }
    RC522_SetBitMask(ControlReg,0x80);           // stop timer now
    RC522_WriteReg(CommandReg,PCD_IDLE);
   if(status)
   {
   }
   return status;
}




/**************************************************************************
函数功能：RF读卡器-寻卡
入口参数：req_code[IN]:寻卡方式  0x52 = 寻感应区内所有符合14443A标准的卡
						   0x26 = 寻未进入休眠状态的卡
						   pTagType[OUT]：卡片类型代码
						   0x4400 = Mifare_UltraLight
						   0x0400 = Mifare_One(S50)
						   0x0200 = Mifare_One(S70)
						   0x0800 = Mifare_Pro(X)
						   0x4403 = Mifare_DESFire
返回  值：成功返回MI_OK
**************************************************************************/
int8_t RC522_PcdRequest(uint8_t req_code,uint8_t *pTagType)
{
	int8_t status;
	uint32_t  unLen;
	uint8_t ucComMF522Buf[MAXRLEN];

	RC522_ClearBitMask(Status2Reg,0x08);
	RC522_WriteReg(BitFramingReg,0x07);
	RC522_SetBitMask(TxControlReg,0x03);
	ucComMF522Buf[0] = req_code;

	status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
	if((status == MI_OK) && (unLen == 0x10))
	{
		*pTagType = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	{
		status = MI_ERR;
	}
	return status;
}


/**************************************************************************
函数功能：RF读卡器-防冲撞
入口参数：pSnr[OUT]:卡片序列号，4字节
返回  值：成功返回MI_OK
**************************************************************************/
int8_t RC522_PcdAnticoll(uint8_t *pSnr)
{
    int8_t status;
    uint8_t i,snr_check=0;
    uint32_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN];

    RC522_ClearBitMask(Status2Reg,0x08);
    RC522_WriteReg(BitFramingReg,0x00);
    RC522_ClearBitMask(CollReg,0x80);
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;
    status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if(status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {
             *(pSnr+i) = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {
			 status = MI_ERR;
		 }
    }
    RC522_SetBitMask(CollReg,0x80);
    return status;
}



/////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////
/********************************************************
函数功能：RC522-用MF522计算CRC16函数
入口参数： pIndata  len  *pOutData
返回  值：成功返回MI_OK
********************************************************/
int8_t RC522_CalulateCRC(uint8_t *pIndata,uint8_t len,uint8_t *pOutData)
{
    uint8_t i,n;
    RC522_ClearBitMask(DivIrqReg,0x04);
    RC522_WriteReg(CommandReg,PCD_IDLE);
    RC522_SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {
    	RC522_WriteReg(FIFODataReg, *(pIndata+i));
	}
    RC522_WriteReg(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do
    {
        n = RC522_ReadReg(DivIrqReg);
        i--;
    }
    while((i!=0) && !(n&0x04));
    pOutData[0] = RC522_ReadReg(CRCResultRegL);
    pOutData[1] = RC522_ReadReg(CRCResultRegM);
	return MI_OK;
}


/********************************************************
函数功能：RC522	选定卡片
入口参数：pSnr[IN]:卡片序列号，4字节
返回  值：成功返回MI_OK
********************************************************/
int8_t RC522_PcdSelect(uint8_t *pSnr)
{
    int8_t status;
    uint8_t i;
    uint32_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6] ^= *(pSnr+i);
    }
    RC522_CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
    RC522_ClearBitMask(Status2Reg,0x08);
    status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x18))
    {
		status = MI_OK;
	}
    else
    {
		status = MI_ERR;
	}
    return status;
}



/********************************************************
函数功能：RC522-验证卡片密码
入口参数：auth_mode[IN]: 密码验证模式
 	 	 	 	 0x60 = 验证A密钥
 	 	 	 	 0x61 = 验证B密钥
 	 	 addr[IN]：块地址
		pKey[IN]：密码
		pSnr[IN]：卡片序列号，4字节
返回  值：成功返回MI_OK
********************************************************/
int8_t RC522_PcdAuthState(uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr)
{
    int8_t status;
    uint32_t  unLen;
    uint8_t i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    for (i=0; i<6; i++)
    {
		ucComMF522Buf[i+2] = *(pKey+i);
	}
    for (i=0; i<6; i++)
    {
		ucComMF522Buf[i+8] = *(pSnr+i);
	}
    status = RC522_PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if((status != MI_OK) || (!(RC522_ReadReg(Status2Reg) & 0x08)))
    {
		status = MI_ERR;
	}
    return status;
}



/********************************************************
函数功能：RC522-写数据到M1卡一块
入口参数：addr[IN]：块地址
       pData[IN]：写入的数据，16字节
返回  值：成功返回MI_OK
********************************************************/
int8_t RC522_PcdWrite(uint8_t addr,uint8_t *pData)
{
    int8_t status;
    uint32_t  unLen;
    uint8_t i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    RC522_CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
    status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
		status = MI_ERR;
	}

    if(status == MI_OK)
    {
        for (i=0; i<16; i++)
        {
			ucComMF522Buf[i] = *(pData+i);
		}
        RC522_CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);
        status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {
			status = MI_ERR;
		}
    }
    return status;
}




/********************************************************
函数功能：RC522-读取M1卡一块数据
入口参数：addr[IN]：块地址
       pData[OUT]：读出的数据，16字节
返回  值：成功返回MI_OK
********************************************************/
int8_t RC522_PcdRead(uint8_t addr,uint8_t *pData)
{
    int8_t status;
    uint32_t  unLen;
    uint8_t i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    RC522_CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
    status = RC522_PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if((status == MI_OK) && (unLen == 0x90))
    {
        for (i=0; i<16; i++)
        {
			*(pData+i) = ucComMF522Buf[i];
		}
    }
    else
    {
		status = MI_ERR;
	}
    return status;
}





/********************************************************
函数功能：RC522	RFID读写控制函数
入口参数：card_addr	块地址（范围：0~63）
	   mode:	RFID_Read -> RFID读操作
				RFID_Write -> RFID写操作
				RFID_Write_Read -> RFID先写操作后读操作
返回  值：无
********************************************************/
uint8_t RC522(uint8_t card_addr,uint8_t mode)
{
	uint8_t card_key = (card_addr/4)*4+3;
	LED1(0)LED2(0)LED3(0)LED4(0)
	uint8_t retry = 10; // 尝试 30 次

	RC522_WriteReg(RFCfgReg, 0x70);
	while(retry--)
	{
		if(RC522_PcdRequest(PICC_REQALL,CT) == MI_OK)		// 寻卡成功
		{
			LED1(1)
			if(RC522_PcdAnticoll(SN) == MI_OK)			// 防冲撞成功
			{
				LED2(1)
				if(RC522_PcdSelect(SN) == MI_OK)			// 选卡成功
				{
					LED3(1)
					if(RC522_PcdAuthState(KEYA,card_key,KEY_A,SN) == MI_OK)	// 验证密钥（A密钥）
					{
						LED4(1)
						ADDR_Str[10] = card_addr/10%10 + 0x30;
						ADDR_Str[11] = card_addr%10 + 0x30;
						CAN_TxtoDisplay((char *)ADDR_Str,14);		// 打印读写操作块地址
						if(card_addr == card_key)		// 读写块地址为密钥块
						{
							mode = 0;			// 密钥块禁止读写操作
							//CAN_TxtoDisplay("ADDR ERROR!\n",13);
						}

						if((mode == RFID_Write) || (mode == RFID_Write_Read))	// 允许写入数据
						{
							if((RC522_PcdWrite(card_addr,WRITE_RFID) == MI_OK))		// 写入数据
							{
								//CAN_TxtoDisplay("WRITE_RFID - OK\n",17);
							}
						}
						if((mode == RFID_Read) || (mode == RFID_Write_Read))	// 允许读出数据
						{
							if(RC522_PcdRead(card_addr,READ_RFID) == MI_OK)			// 读取数据
							{
								//CAN_TxtoDisplay("READ_RFID - OK\n",16);
								CAN_TxtoDisplay("DATA(ASCII):\n",14);
								CAN_TxtoDisplay((char *)READ_RFID,16);
								CAN_TxtoDisplay("\n",2);

								Beep_Set(1);
								My_Delayms(200);
								Beep_Set(0);

								return 1;
							}
						}
					}
				}
			}
		}
		My_Delayms(5);
	}

	return 0;
}

