/***********************************************************************
* @file           Main.c
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    张飞实战电子"扬帆起航"课程电机控制部分  
***********************************************************************/
#include "../Include/Main.h"

/***********************************************************************
* 函数名称  ：MainLoop
* 功能描述  ：主函数
* 形参      ：无
* 返回值    ：无
* 要点备注  ：单片机上电复位向量执行后跳转到该函数
***********************************************************************/
void MainLoop(void)
{
    Initialize(); //外设及应用程序初始化
    
    tSystem.StartDelayCount = 500; //设置上电延时时间 500ms
    SysTick->CSR |= 0x00000001; //打开滴答定时器
    NVIC->ISER[0] = 0x00010200; //使能DMA及定时3捕捉中断
    
    while(tSystem.StartDelayCount != 0); //等待上电延时结束
    
    do
    {
        if(tSystem.LcdCount == 0) //500ms刷新一次
        {
            tSystem.LcdCount = 500;
            Display(); //LCD显示
        }
    }while(1);
}
/***********************************************************************
* 函数名称  : DMA1_Channel1_IRQHandler
* 功能描述  ：DMA通道一中断服务函数
* 形参      ：无
* 返回值    ：无
* 要点备注  ：DMA搬运ADC数据，搬运完成后，进入中断
***********************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
    unsigned int Temp;
    
    Temp = DMA1->ISR;
    if(Temp & 0x00000002)
    {
        DMA1->IFCR |= 0x00000002; //清除中断标志位
        
        AdcSample(); //读取ADC转换结果瞬时值
        BLDC_StateMachine(); // BLDC运行状态机
        HardOverCurrentMonitor(); //硬件过流监测
        SentReceData(); //串口发送接收数据
        AdcFilter(); //Vbus电压 Pot旋钮值  Ntc采样值平均滤波处理
    }
}
/***********************************************************************
* 函数名称  : TIM3_IRQHandler
* 功能描述  ：定时器3捕捉中断服务函数
* 形参      ：无
* 返回值    ：无
* 要点备注  ：有霍尔沿跳变，进入该中断
***********************************************************************/
void TIM3_IRQHandler(void)
{
    unsigned int Temp;
    
    Temp = TIM3->SR;
    if(Temp & 0x0002)
    {
        TIM3->SR &= 0xfffd; //CCIF = 0 清除捕捉中断标志位
        TIM3->CNT = 0; //清零定时器3计数器，为下一次捕捉做准备
        
        tBldc.SetNoPhaseTime = 16000; //设置不换相的时间
        tProtect.CountLockRotorCurrent = 1600; //堵转电流保护时间设置 100ms
        tProtect.CountLockRotorPhase = 2400; //堵转相位保护时间设置 150ms
        
        tBldc.HallData = BLDC_ReadHallValue(); //读取霍尔组合值
        if((tBldc.HallData >= 1) && (tBldc.HallData <= 6)) //判断霍尔值是否有效
        {
            BLDC_PWM_ON_OFF(BLDC_HallToPhase(tBldc.HallData,tMotor.Direction)); //根据霍尔进行换相
        }
        if(tBldc.StartState == 1)
        {
            tBldc.StartCount ++; 
            tBldc.MaxNoSwitchTime = 0;
        }
        
        tBldc.PhaseTime = TIM3->CCR1; //60度计数值
        tBldc.PhaseTimeArray[tBldc.PhaseTimeNum++] = tBldc.PhaseTime; //存储60度时间
        if(tBldc.PhaseTimeNum >= 6)
        {
            tBldc.PhaseTimeNum = 0;
            tBldc.SpeedUpDate = 1;
            tBldc.SpeedEnable = 1;
        }
    }
}
/***********************************************************************
* 函数名称  : SysTick_Handler
* 功能描述  ：滴答定时器中断服务函数
* 形参      ：无
* 返回值    ：无
* 要点备注  ：1ms进入该中断一次
***********************************************************************/
void SysTick_Handler(void)
{
    unsigned int Temp;
    
    Temp = SysTick->CSR;
    
    if(Temp & 0x00010000) //判断COUNTFLAG = 1
    {
        AdcApp(); //ADC模块应用程序
        MotorApp(); //电机模块应用程序
        UsartApp(); //串口通讯模块应用程序
        ProtectApp(); //故障保护应用程序
        
        if(tSystem.StartDelayCount != 0) //上电延时变量递减
            tSystem.StartDelayCount --;
        
        if(tSystem.LcdCount != 0)
            tSystem.LcdCount --;
        
        if(tSystem.UsartCount != 0)
            tSystem.UsartCount --;
        
        if(tSystem.DutyCount != 0)
            tSystem.DutyCount --;
    }
}
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
