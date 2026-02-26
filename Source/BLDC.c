/***********************************************************************
* @file           BLDC.c
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    电机运行逻辑模块
***********************************************************************/
#include "../Include/BLDC.h"

/***********************************************************************
* 函数名称  : BLDC_StateMachine
* 功能描述  ：BLDC电机运行状态机
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
void BLDC_StateMachine(void)
{
//    unsigned short Temp;
    
    if(tBldc.PwmTick != 0)
        tBldc.PwmTick --;
    
    switch(tBldc.State)
    {
        case STOP_STATE:  //停止状态
            MotorStop();
            tBldc.SpeedUpDate = 0;
            tBldc.SpeedEnable = 0;
            tProtect.CountLockRotorCurrent = 1600; //堵转电流保护时间设置 100ms
            tProtect.CountLockRotorPhase = 2400; //堵转相位保护时间设置 150ms
            
            break;
        case CHARGEREADY_STATE: //自举电容充电准备状态
            //打开三个下管 --> PWM控制  占空比 50%
            TIM1->CCMR1 = 0x00006868;//OC1/2CE = 0 OC1/2M[2:0] = 0b100 Force inactive level OC1/2PE = 1 OC1/2FE = 0 CC1/2S[1:0] = 0b00 OUT
            TIM1->CCMR2 = 0x00006868; //OC3/4CE = 0 OC3/4M[2:0] = 0b100 Force inactive level OC3/4PE = 1 OC3/4FE = 0 CC3/4S[1:0] = 0b00 OUT
            TIM1->CCER = 0x00001444; //CC1/2/3E = 0 CC4E = 1  CC1/2/3P = 0 CC1/2/3NE = 1 CC1/2/3NP = 0
            
            TIM1->CCR1 = 1000; //比较值设置
            TIM1->CCR2 = 1000;
            TIM1->CCR3 = 1000;
            //设置自举电容充电时间 --> 100ms
            tBldc.PwmTick = 1600;
            //状态跳转
            tBldc.State = CHARGEWAIT_STATE;
            break;
        case CHARGEWAIT_STATE: //等待自举电容充电完成状态
            if(tBldc.PwmTick == 0)
            {
                TIM1->CCMR1 = 0x00004848;//OC1/2CE = 0 OC1/2M[2:0] = 0b100 Force inactive level OC1/2PE = 1 OC1/2FE = 0 CC1/2S[1:0] = 0b00 OUT
                TIM1->CCMR2 = 0x00006848; //OC3/4CE = 0 OC3/4M[2:0] = 0b100 Force inactive level OC3/4PE = 1 OC3/4FE = 0 CC3/4S[1:0] = 0b00 OUT
                TIM1->CCER = 0x00001ddd; //CC1/2/3/4E = 1  CC1/2/3P = 0 CC1/2/3NE = 1 CC1/2/3NP = 1
                
                TIM1->CCR1 = 0; //比较值设置
                TIM1->CCR2 = 0;
                TIM1->CCR3 = 0;
                
                tBldc.PhaseTest = 0;
                tBldc.Duty = START_MIN_DUTY; //设置启动占空比
                
                //状态跳转
                tBldc.State = STARTINIT_STATE;
            }
            break;
        case STARTINIT_STATE: //启动初始化状态
            if(tBldc.StartState == 0)
            {
                tBldc.HallData = BLDC_ReadHallValue(); //读取霍尔组合值
                if((tBldc.HallData >= 1) && (tBldc.HallData <= 6)) //判断霍尔值是否有效
                {
                    BLDC_PWM_ON_OFF(BLDC_HallToPhase(tBldc.HallData,tMotor.Direction)); //根据霍尔进行换相
                    tBldc.StartState = 1;
                }
            }
            else
            {
                if(tBldc.StartCount >= 3)
                {
                    tBldc.State = MOTORRUN_STATE;
                    tBldc.StartCount = 0;
                    tBldc.MaxNoSwitchTime = 0;
                    tBldc.StartState = 0;
                }
                else
                {
                    tBldc.MaxNoSwitchTime ++; //62.5us
                    if(tBldc.MaxNoSwitchTime >= 4800) //300ms
                    {
                        tBldc.MaxNoSwitchTime = 0;
                        tBldc.Duty += START_STEP_DUTY; //满足最大不换相时间，就按照5%增加占空比
                        if(tBldc.Duty >= START_MAX_DUTY)
                        {
                            tBldc.StartFailCount ++;
                            if(tBldc.StartFailCount >= 3)
                            {
                                if((tProtect.Code == 0) || (tProtect.Code > 7))
                                {
                                    tProtect.Code = 8; //启动失败故障
                                }
                            }
                            tBldc.Duty = START_MAX_DUTY;
                        }
                            
                        TIM1->CCR1 = tBldc.Duty;
                        TIM1->CCR2 = tBldc.Duty;
                        TIM1->CCR3 = tBldc.Duty;
                    }
                }
            }
            break;
        case MOTORRUN_STATE: //电机运行状态
            TIM16->CCR1 = tMotor.ActSpeed >> 1;
            TIM17->CCR1 = tMotor.AimSpeed >> 1;
            
            tBldc.Duty = tMotor.AimDuty;
            
            tBldc.HallData = BLDC_ReadHallValue(); //读取霍尔组合值
            if((tBldc.HallData >= 1) && (tBldc.HallData <= 6)) //判断霍尔值是否有效
            {
                BLDC_PWM_ON_OFF(BLDC_HallToPhase(tBldc.HallData,tMotor.Direction)); //根据霍尔进行换相
            }
        
            //1s不进入定时器3捕捉中断，则把实际速度设置为0
            if(tBldc.SetNoPhaseTime)
            {
                tBldc.SetNoPhaseTime --;
            }
            else
            {
                tMotor.ActSpeed = 0;
            }
            
            //堵转电流保护
            if(tProtect.CountLockRotorCurrent) 
            {
                tProtect.CountLockRotorCurrent --;
            }
            else
            {
                if(tAdc.AN4.ValueAverage >= 136) //100ms不换相，且电流超过1.1A，报堵转电流故障
                {
                    if((tProtect.Code == 0) || (tProtect.Code > 1))
                    {
                        tProtect.Code = 2; //100ms不换相，报堵转相位故障
                    }
                }
            }
            //堵转相位保护
            if(tProtect.CountLockRotorPhase) 
            {
                tProtect.CountLockRotorPhase --;
            }
            else
            {
                if((tProtect.Code == 0) || (tProtect.Code > 2))
                {
                    tProtect.Code = 3; //150ms不换相，报堵转相位故障
                }
            }
            
            break;
        case TEST_STATE: //测试状态
            break;
        default:
            break;
    }
}
/***********************************************************************
* 函数名称  : BLDC_PWM_ON_OFF
* 功能描述  ：根据相位值给对应绕组进行通电
* 形参      ：Phase 相位值 
              Phase1：U+V-  Phase2：W+V-  Phase3：W+U-
              Phase4：V+U-  Phase5：V+W-  Phase6：U+W-
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
void BLDC_PWM_ON_OFF(unsigned char Phase)
{
    switch(Phase)
    {
        case 1:
            BLDC_UV();
            break;
        case 2:
            BLDC_WV();
            break;
        case 3:
            BLDC_WU();
            break;
        case 4:
            BLDC_VU();
            break;
        case 5:
            BLDC_VW();
            break;
        case 6:
            BLDC_UW();
            break;
        default:
            BLDC_PWMOFF();
            break;
    }
}
/***********************************************************************
* 函数名称  : BLDC_UV
* 功能描述  ：给U相绕组跟V相绕组通电  U相上管使用PWM控制，V相下管使用恒通控制
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
static void BLDC_UV(void)
{
    TIM1->CCER = 0x00001041; //CC1E = 1  CC2NE = 1 CC3E = 0 CC3NE = 0 CC3NP = 0  CC4E = 1
    TIM1->CCMR1 = 0x00005868;// CC1S[1:0] = 0b00 Out  OC1PE = 1 使能预装载 OC1M[2:0] = 0b110 PWM mode1
                             // CC2S[1:0] = 0b00 Out  OC2PE = 1 使能预装载 OC2M[2:0] = 0b101 Force active level - OC2REF is forced high
    TIM1->CCMR2 = 0x00006800; //CC3S[1:0] = 0b00 Out  OC3PE = 0 使能预装载 OC3M[2:0] = 0b000 Frozen
                             //CC4S[1:0] = 0b00 Out  OC4PE = 1 使能预装载 OC4M[2:0] = 0b110 PWM mode1
    
    TIM1->CCR1 = tBldc.Duty; //比较值设置
    TIM1->CCR2 = tBldc.Duty;
    TIM1->CCR3 = tBldc.Duty;
}
/***********************************************************************
* 函数名称  : BLDC_WV
* 功能描述  ：给W相绕组跟V相绕组通电  W相上管使用PWM控制，V相下管使用恒通控制
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
static void BLDC_WV(void)
{
    TIM1->CCER = 0x00001140; //CC3E = 1  CC2NE = 1 CC1E = 0 CC1NE = 0 CC1NP = 0  CC4E = 1
    
    TIM1->CCMR1 = 0x00005800;// CC1S[1:0] = 0b00 Out  OC1PE = 0 使能预装载 OC1M[2:0] = 0b000 Frozen
                             // CC2S[1:0] = 0b00 Out  OC2PE = 1 使能预装载 OC2M[2:0] = 0b101 Force active level - OC2REF is forced high
    TIM1->CCMR2 = 0x00006868; //CC3S[1:0] = 0b00 Out  OC3PE = 1 使能预装载 OC3M[2:0] = 0b100 PWM mode1
                             //CC4S[1:0] = 0b00 Out  OC4PE = 1 使能预装载 OC4M[2:0] = 0b110 
    
    TIM1->CCR1 = tBldc.Duty; //比较值设置
    TIM1->CCR2 = tBldc.Duty;
    TIM1->CCR3 = tBldc.Duty;
}
/***********************************************************************
* 函数名称  : BLDC_WU
* 功能描述  ：给W相绕组跟U相绕组通电  W相上管使用PWM控制，U相下管使用恒通控制
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
static void BLDC_WU(void)
{
    TIM1->CCER = 0x00001104; //CC3E = 1  CC1NE = 1 CC2E = 0 CC2NE = 0 CC2NP = 0  CC4E = 1
    
    TIM1->CCMR1 = 0x00000058;// CC1S[1:0] = 0b00 Out  OC1PE = 1 使能预装载 OC1M[2:0] = 0b101 Force active level - OC1REF is forced high 
                             // CC2S[1:0] = 0b00 Out  OC2PE = 0 使能预装载 OC2M[2:0] = 0b000 Frozen
    TIM1->CCMR2 = 0x00006868; //CC3S[1:0] = 0b00 Out  OC3PE = 1 使能预装载 OC3M[2:0] = 0b100 PWM mode1
                             //CC4S[1:0] = 0b00 Out  OC4PE = 1 使能预装载 OC4M[2:0] = 0b110 
   
    TIM1->CCR1 = tBldc.Duty; //比较值设置
    TIM1->CCR2 = tBldc.Duty;
    TIM1->CCR3 = tBldc.Duty;
}
/***********************************************************************
* 函数名称  : BLDC_VU
* 功能描述  ：给V相绕组跟U相绕组通电  V相上管使用PWM控制，U相下管使用恒通控制
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
static void BLDC_VU(void)
{
    TIM1->CCER = 0x00001014; //CC2E = 1  CC1NE = 1 CC3E = 0 CC3NE = 0 CC3NP = 0  CC4E = 1
    
    TIM1->CCMR1 = 0x00006858;// CC1S[1:0] = 0b00 Out  OC1PE = 1 使能预装载 OC1M[2:0] = 0b101 Force active level - OC1REF is forced high 
                             // CC2S[1:0] = 0b00 Out  OC2PE = 1 使能预装载 OC2M[2:0] = 0b110 PWM mode1
    TIM1->CCMR2 = 0x00006800; //CC3S[1:0] = 0b00 Out  OC3PE = 0 使能预装载 OC3M[2:0] = 0b000 Frozen
                             //CC4S[1:0] = 0b00 Out  OC4PE = 1 使能预装载 OC4M[2:0] = 0b110 
    
    
    TIM1->CCR1 = tBldc.Duty; //比较值设置
    TIM1->CCR2 = tBldc.Duty;
    TIM1->CCR3 = tBldc.Duty;
}
/***********************************************************************
* 函数名称  : BLDC_VW
* 功能描述  ：给V相绕组跟W相绕组通电  V相上管使用PWM控制，W相下管使用恒通控制
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
static void BLDC_VW(void)
{
    TIM1->CCER = 0x00001410; //CC2E = 1  CC3NE = 1 CC1E = 0 CC1NE = 0 CC1NP = 0  CC4E = 1
    
    TIM1->CCMR1 = 0x00006800;// CC1S[1:0] = 0b00 Out  OC1PE = 0 使能预装载 OC1M[2:0] = 0b000 Frozen
                             // CC2S[1:0] = 0b00 Out  OC2PE = 1 使能预装载 OC2M[2:0] = 0b110 PWM mode1
    TIM1->CCMR2 = 0x00006858; //CC3S[1:0] = 0b00 Out  OC3PE = 1 使能预装载 OC3M[2:0] = 0b101 Force active level - OC3REF is forced high 
                             //CC4S[1:0] = 0b00 Out  OC4PE = 1 使能预装载 OC4M[2:0] = 0b110 
    
    TIM1->CCR1 = tBldc.Duty; //比较值设置
    TIM1->CCR2 = tBldc.Duty;
    TIM1->CCR3 = tBldc.Duty;
}
/***********************************************************************
* 函数名称  : BLDC_UW
* 功能描述  ：给U相绕组跟W相绕组通电  U相上管使用PWM控制，W相下管使用恒通控制
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
static void BLDC_UW(void)
{
    TIM1->CCER = 0x00001401; //CC1E = 1  CC3NE = 1 CC2E = 0 CC2NE = 0 CC2NP = 0  CC4E = 1
    
    TIM1->CCMR1 = 0x00000068;// CC1S[1:0] = 0b00 Out  OC1PE = 1 使能预装载 OC1M[2:0] = 0b110 PWM mode1 
                             // CC2S[1:0] = 0b00 Out  OC2PE = 0 使能预装载 OC2M[2:0] = 0b000 Frozen
    TIM1->CCMR2 = 0x00006858; //CC3S[1:0] = 0b00 Out  OC3PE = 1 使能预装载 OC3M[2:0] = 0b101 Force active level - OC3REF is forced high 
                             //CC4S[1:0] = 0b00 Out  OC4PE = 1 使能预装载 OC4M[2:0] = 0b110 
    
    TIM1->CCR1 = tBldc.Duty; //比较值设置
    TIM1->CCR2 = tBldc.Duty;
    TIM1->CCR3 = tBldc.Duty;
}
/***********************************************************************
* 函数名称  : BLDC_PWMOFF
* 功能描述  ：关闭MOS管驱动信号，让MOS管关断
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
static void BLDC_PWMOFF(void)
{
    TIM1->CCER = 0x00001DDD; //CC1E = 1 CC1NE = 1 CC1NP = 1 CC2E = 1 CC2NE = 1 CC2NP = 1   CC3E = 1 CC3NE = 1 CC3NP = 1 CC4E = 1
    
    TIM1->CCMR1 = 0x00004848;// CC1S[1:0] = 0b00 Out  OC1PE = 1 使能预装载 OC1M[2:0] = 0b100 Force inactive level - OC1REF is forced low
                             // CC2S[1:0] = 0b00 Out  OC2PE = 1 使能预装载 OC2M[2:0] = 0b100 Force inactive level - OC1REF is forced low
    TIM1->CCMR2 = 0x00006848; //CC3S[1:0] = 0b00 Out  OC3PE = 1 使能预装载 OC3M[2:0] = 0b100 Force inactive level - OC1REF is forced low
                             //CC4S[1:0] = 0b00 Out  OC4PE = 1 使能预装载 OC4M[2:0] = 0b110 
    
    TIM1->CCR1 = 0; //比较值设置
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;
}
/***********************************************************************
* 函数名称  : BLDC_ReadHallValue
* 功能描述  ：读取霍尔引脚输入电平值
* 形参      ：无
* 返回值    ：霍尔组合值
* 要点备注  ：HA   HB   HC
***********************************************************************/
unsigned char BLDC_ReadHallValue(void)
{
    unsigned int Temp32A;
    unsigned int Temp32B;
    UNION_U8 TempHall;
    
    Temp32A = GPIOA->IDR;
    Temp32B = GPIOB->IDR;
    
    Temp32A &= 0x000000c0;
    Temp32B &= 0x00000001;
    
    TempHall.Bits.B0 = (unsigned char)(Temp32B & 0x00000001); //HC
    TempHall.Bits.B1 = (unsigned char)((Temp32A >> 7) & 0x00000001); //HB
    TempHall.Bits.B2 = (unsigned char)((Temp32A >> 6) & 0x00000001); //HA
    
    return ((TempHall.byte) & 0x07);
}
/***********************************************************************
* 函数名称  : BLDC_HallToPhase
* 功能描述  ：根据霍尔值确定相序
* 形参      ：Hall:霍尔组合值  Dir：电机旋转方向
* 返回值    ：相位值
* 要点备注  ：无
***********************************************************************/
unsigned char BLDC_HallToPhase(unsigned char Hall,unsigned char Dir)
{
    unsigned char Phase;
    
    if(Dir == 0) //顺时针相序
    {
        switch(Hall)
        {
            case 3:
                Phase = 3;
                break;
            case 2:
                Phase = 4;
                break;
            case 6:
                Phase = 5;
                break;
            case 4:
                Phase = 6;
                break;
            case 5:
                Phase = 1;
                break;
            case 1:
                Phase = 2;
                break;
            default:
                Phase = 0;
                break;
        }
    }
    else //逆时针相序
    {
        switch(Hall)
        {
            case 3:
                Phase = 6;
                break;
            case 1:
                Phase = 5;
                break;
            case 5:
                Phase = 4;
                break;
            case 4:
                Phase = 3;
                break;
            case 6:
                Phase = 2;
                break;
            case 2:
                Phase = 1;
                break;
            default:
                Phase = 0;
                break;
        }
    }
    return (Phase);
}
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
