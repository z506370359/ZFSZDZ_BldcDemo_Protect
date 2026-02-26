/***********************************************************************
* @file           Motor.c
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    电机应用模块
***********************************************************************/
#include "../Include/Motor.h"
/***********************************************************************
* 函数名称  : MotorInit
* 功能描述  ：电机模块初始化
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
void MotorInit(void)
{   
    tMotor.SpeedMode = DEFAULT_SPEEDMODE; //默认调速方式
    tMotor.RunMode = DEFAULT_RUNMODE; //开环运行
    tMotor.Direction = DEFAULT_DIR; //电机转动方向
    tMotor.VarDelta = 1; //设置斜坡增量初始值
    tMotor.AimDuty = RUN_MIN_DUTY; //设置目标占空比初始值
    PIInit(&tPI_Speed,16384,128,32440,32767,0); //PI控制器参数初始化
}
/***********************************************************************
* 函数名称  : MotorApp
* 功能描述  ：电机模块应用程序
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
void MotorApp(void)
{
    MotorRunStopControl();
    if(tMotor.SpeedMode == SPEEDMODE_POT) //电位器调速
    {
        if(tMotor.RunMode == RUNMODE_OPENLOOP)
        {
            MotorSetDuty(tAdc.PotValue);
        }
        else if(tMotor.RunMode == RUNMODE_CLOSELOOP)
        {
            MotorSetSpeed(tAdc.SetSpeed);
        }
    }
    else if(tMotor.SpeedMode == SPEEDMODE_USART) //串口调速
    {
        if(tMotor.RunMode == RUNMODE_OPENLOOP)
        {
            MotorSetDuty(tUsart.Duty);
        }
        else if(tMotor.RunMode == RUNMODE_CLOSELOOP)
        {
            MotorSetSpeed(tUsart.SetSpeed);
        }
    }
    CalcMotorSpeed(); //计算电机转速
    MotorSpeedControl(); //速度闭环控制
}
/***********************************************************************
* 函数名称  : MotorRunStopControl
* 功能描述  ：电机启停控制(状态机状态跳转启动)
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
static void MotorRunStopControl(void)
{
    if(tMotor.RunStopState == 0) //在电机停止运行状态下，判断是否要启动
    {
        if(tMotor.SpeedMode == SPEEDMODE_POT) //电位器调速及启动
        {
            if(tMotor.RunMode == RUNMODE_OPENLOOP)
            {
                if((tAdc.PotValue > 50) && (tProtect.Code == 0))
                {
                    tMotor.RunStopState = 1;
                    tBldc.State = 1;
                }
            }
            else if(tMotor.RunMode ==  RUNMODE_CLOSELOOP)
            {
                if((tAdc.SetSpeed > 80) && (tProtect.Code == 0))
                {
                    tMotor.RunStopState = 1;
                    tBldc.State = 1;
                }
            }
        }
        else if(tMotor.SpeedMode == SPEEDMODE_USART) //串口数据调速及启动
        {
            if(tMotor.RunMode == RUNMODE_OPENLOOP)
            {
                if((tUsart.Duty > 50) && (tProtect.Code == 0))
                {
                    tMotor.RunStopState = 1;
                    tBldc.State = 1;
                }
            }
            else if(tMotor.RunMode ==  RUNMODE_CLOSELOOP)
            {
                if((tUsart.SetSpeed > 0) && (tProtect.Code == 0))
                {
                    tMotor.RunStopState = 1;
                    tBldc.State = 1;
                }
            }
        }
    }
    else //在电机运行状态下，判断是否要停机
    {
        if(tMotor.SpeedMode == SPEEDMODE_POT) //通过电位器设定停机条件
        {
            if(tMotor.RunMode == RUNMODE_OPENLOOP)
            {
                if((tAdc.PotValue < 40) || (tProtect.Code != 0))
                {
                    tMotor.RunStopState = 0;
                    tBldc.State = 0;
                }
            }
            else if(tMotor.RunMode ==  RUNMODE_CLOSELOOP)
            {
                if((tAdc.SetSpeed < 80) || (tProtect.Code != 0))
                {
                    tMotor.RunStopState = 0;
                    tBldc.State = 0;
                }
            }
        }
        else if(tMotor.SpeedMode == SPEEDMODE_USART) //通过串口数据设定停机条件
        {
            if(tMotor.RunMode == RUNMODE_OPENLOOP)
            {
                if((tUsart.Duty < 40) || (tProtect.Code != 0))
                {
                    tMotor.RunStopState = 0;
                    tBldc.State = 0;
                }
            }
            else if(tMotor.RunMode ==  RUNMODE_CLOSELOOP)
            {
                if((tUsart.SetSpeed == 0) || (tProtect.Code != 0))
                {
                    tMotor.RunStopState = 0;
                    tBldc.State = 0;
                }
            }
        }
    }
}
/***********************************************************************
* 函数名称  : MotorStop
* 功能描述  ：电机目标设定值求取
* 形参      ：Var:目标值设定量 （旋钮、串口）
              Aim:斜坡处理后的目标设定值
              MaxDelta：斜坡增量最大值
              MinValue：目标值最小限幅值
* 返回值    ：处理之后的目标值
* 要点备注  ：设定值的斜坡处理
***********************************************************************/
unsigned short GetAimSet(unsigned short Var,unsigned short Aim,unsigned short MaxDelta,unsigned short MinValue)
{
    unsigned short Temp;
    
    if(Var < Aim) //目标值下坡处理
    {
        Temp = Aim - Var; //求取当前目标值与设定值差值
        if(Temp > tMotor.VarDelta) //如果差值大于设定的减量值
        {
            Temp = Aim - tMotor.VarDelta; //让目标值减去减量值
        }
        else
        {
            Temp = Var; //差值小于等于减量时，直接赋值
        }
        if(Temp < MinValue) //限幅
            Temp = MinValue;
        Aim = Temp;
    }
    else if(Var > Aim) //目标值上坡处理
    {
        Temp = Var - Aim; //求取当前目标值与设定值差值
        if(Temp > tMotor.VarDelta) //如果差值大于设定的增量值
        {
            Temp = Aim + tMotor.VarDelta; //让目标值加去增量值
        }
        else
        {
            Temp = Var; //差值小于等于增量时，直接赋值
        }
        Aim = Temp;    
    }
    if(tMotor.VarDelta < MaxDelta) //让增量/减量值 慢慢变化到最大值
        tMotor.VarDelta ++;
    return (Aim);
}
/***********************************************************************
* 函数名称  : MotorSetDuty
* 功能描述  ：设置PWM占空比
* 形参      ：Var:目标值设定量 （旋钮、串口）
* 返回值    ：处理之后的目标值
* 要点备注  ：设定值的斜坡处理
***********************************************************************/
void MotorSetDuty(unsigned short Var)
{
    if(tSystem.DutyCount == 0)
    {
        tSystem.DutyCount = 100; //100ms占空比变化一次
        tMotor.AimDuty = GetAimSet(Var,tMotor.AimDuty,20,RUN_MIN_DUTY);
    }
}
/***********************************************************************
* 函数名称  : MotorSetSpeed
* 功能描述  ：速度闭环控制目标转速获取
* 形参      ：Var:目标值设定量 （旋钮、串口）
* 返回值    ：处理之后的目标值
* 要点备注  ：设定值的斜坡处理
***********************************************************************/
void MotorSetSpeed(unsigned short Var)
{
    if(tSystem.DutyCount == 0)
    {
        tSystem.DutyCount = 100; //100ms占空比变化一次
        
        if(tBldc.SpeedEnable)
            tMotor.AimSpeed = GetAimSet(Var,tMotor.AimSpeed,50,RUN_MIN_SPEED);
        else
            tMotor.AimSpeed = 0;
    }
}
/***********************************************************************
* 函数名称  : CalcMotorSpeed
* 功能描述  ：计算电机实际转速
* 形参      ：无
* 返回值    ：无
* 要点备注  ：根据360度计数值，计算电机转速
* Ne = 60/T*Np = 60 / (t * Nn * Np) = 60 / (0.000002 * Mperiod)
***********************************************************************/
void CalcMotorSpeed(void)
{
    unsigned char i;
    unsigned int SumPhaseTime;
    unsigned int Eperiod;
    unsigned int Mperiod;
    
    if(tBldc.SpeedUpDate)
    {
        tBldc.SpeedUpDate = 0;
        i = 0;
        SumPhaseTime = 0;
        
        do
        {
            SumPhaseTime += tBldc.PhaseTimeArray[i++];
        }while(i < 6); //求取360度计数值
        Eperiod = SumPhaseTime;
        Mperiod = MOTOR_POLEPAIRS * Eperiod; //机械周期
        
        tMotor.InstantSpeed = (unsigned short)(30000000 / Mperiod); //计算出电机瞬时速度
        tMotor.ActSpeed = tMotor.InstantSpeed; //用于控制的转速，尽量不滤波，或者滤波要浅
        tMotor.DisplaySpeed = LowPassFilter(LOWPASS_FILTER_FACTOR,tMotor.InstantSpeed,tMotor.DisplaySpeed); //显示的滤波，可以深一些
    }
}
/***********************************************************************
* 函数名称  : MotorSpeedControl
* 功能描述  ：电机转速控制
* 形参      ：无
* 返回值    ：无
* 要点备注  ：使用PI控制器控制电机转速
***********************************************************************/
void MotorSpeedControl(void)
{
    signed short qTemp;
    
    if(tBldc.SpeedEnable)
    {
        tPI_Speed.qInRef = tMotor.AimSpeed * 10; //0~3000 处理为0~30000
        tPI_Speed.qInMeas = tMotor.ActSpeed * 10; //0~3000 处理为0~30000
        PIControl(&tPI_Speed); //控制器输出值在0~32767
        qTemp = (tPI_Speed.qOut * (PWMARRVALUE >> 1)) >> 15; // 0~2000
        if(qTemp < RUN_MIN_DUTY)
            qTemp = RUN_MIN_DUTY;
        tMotor.AimDuty = qTemp;
    }
    else
    {
        tMotor.AimDuty = RUN_MIN_DUTY;
        tPI_Speed.qOut = 0;
        tPI_Speed.qSum = 0;
    }
}
/***********************************************************************
* 函数名称  : MotorStop
* 功能描述  ：电机停止运行
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
void MotorStop(void)
{
    TIM1->CCMR1 = 0x00004848;//OC1/2CE = 0 OC1/2M[2:0] = 0b100 Force inactive level OC1/2PE = 1 OC1/2FE = 0 CC1/2S[1:0] = 0b00 OUT
    TIM1->CCMR2 = 0x00006848; //OC3/4CE = 0 OC3/4M[2:0] = 0b100 Force inactive level OC3/4PE = 1 OC3/4FE = 0 CC3/4S[1:0] = 0b00 OUT
    TIM1->CCER = 0x00001ddd; //CC1/2/3/4E = 1  CC1/2/3P = 0 CC1/2/3NE = 1 CC1/2/3NP = 1
    
    TIM1->CCR1 = 0; //比较值设置
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;
}

/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
