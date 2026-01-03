#include "PID.h"



/**************************************************************************
函数功能：PID-清空
入口参数：结构体PID
返回  值：无
**************************************************************************/
void PID_Clear(PID_t *p)
{
	p->Target = 0;
	p->Actual = 0;
	p->Out = 0;
	p->Error0 = 0;
	p->Error1 = 0;
	p->ErrorInt = 0;
}



/**************************************************************************
函数功能：PID速度环-位置式
入口参数：结构体PID
返回  值：无
**************************************************************************/
void PIDSpeed_Update(PID_t *PID)
{
	PID->Error1 = PID->Error0;
	PID->Error0 = PID->Target - PID->Actual;

	if(PID->Ki !=0)
	{
		PID->ErrorInt += PID->Error0;

	}
	else
	{
		PID->ErrorInt = 0;
	}

    if (PID->ErrorInt > PID->ErrorIntMax)
    {
        PID->ErrorInt = PID->ErrorIntMax;
    }

    else if(PID->ErrorInt < PID->ErrorIntMin)
    {
        PID->ErrorInt = PID->ErrorIntMin;
    }

	PID->Out = PID->Kp * PID->Error0 + PID->Ki * PID->ErrorInt + PID->Kd * (PID->Error0 - PID->Error1);


	if(PID->Out > PID->OutMax){PID->Out = PID-> OutMax;}
	if(PID->Out < PID->OutMin){PID->Out = PID-> OutMin;}
}





/**************************************************************************
函数功能：PD转向环-增量(限制角度，陀螺仪用)
入口参数：结构体PID
返回  值：无
**************************************************************************/
void PIDTurn_Update(PID_t *PID)
{
	PID->Error2 = PID->Error1;
	PID->Error1 = PID->Error0;
	PID->Error0 = PID->Target - PID->Actual;
    
	// 规范误差到 [-180°, 180°]
	if (PID->Error0 > 180) 
    {
        PID->Error0 = PID->Error0 - 360; 		 
    }// 正大角度 → 转换为负角度

	else if (PID->Error0 < -180) 
	{
        PID->Error0 = PID->Error0 + 360;
    }  // 负大角度 → 转换为正角度

    if (PID->ErrorInt > PID->ErrorIntMax)
    {
        PID->ErrorInt = PID->ErrorIntMax;
    }

    else if(PID->ErrorInt < PID->ErrorIntMin)
    {
        PID->ErrorInt = PID->ErrorIntMin;
    }

	PID->Out = PID->Kp * (PID->Error0 - PID->Error1) + PID->Ki * PID->Error0 + PID->Kd * (PID->Error0 - 2*PID->Error1 + PID->Error2);


	if(PID->Out > PID->OutMax){PID->Out=PID->OutMax;}
	if(PID->Out < PID->OutMin){PID->Out=PID->OutMin;}

}




/**************************************************************************
函数功能：PD转向环-增量(无限制角度)
入口参数：结构体PID
返回  值：无
**************************************************************************/
void PID_Update(PID_t *PID)
{
	PID->Error2 = PID->Error1;
	PID->Error1 = PID->Error0;
	PID->Error0 = PID->Target - PID->Actual;
    

    if (PID->ErrorInt > PID->ErrorIntMax)
    {
        PID->ErrorInt = PID->ErrorIntMax;
    }

    else if(PID->ErrorInt < PID->ErrorIntMin)
    {
        PID->ErrorInt = PID->ErrorIntMin;
    }

	PID->Out = PID->Kp * (PID->Error0 - PID->Error1) + PID->Ki * PID->Error0 + PID->Kd * (PID->Error0 - 2*PID->Error1 + PID->Error2);


	if(PID->Out > PID->OutMax){PID->Out=PID->OutMax;}
	if(PID->Out < PID->OutMin){PID->Out=PID->OutMin;}

}


