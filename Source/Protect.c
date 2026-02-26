/***********************************************************************
* @file           Protect.c
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    故障处理模块
***********************************************************************/
#include "../Include/Protect.h"

/***********************************************************************
* 函数名称  : ProtectInit
* 功能描述  ：故障监测初始化
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
void ProtectInit(void)
{
    tProtect.CountHOverCurrent = 0;
    tProtect.CountSOverCurrent = 100; //100ms
    tProtect.CountOverVoltage = 100; //100ms
    tProtect.CountUnderVoltage = 100; //100ms
}
/***********************************************************************
* 函数名称  : ProtectApp
* 功能描述  ：故障保护应用程序
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
void ProtectApp(void)
{
    if(tProtect.CountSOverCurrent)
        tProtect.CountSOverCurrent --;
    
    if(tProtect.CountOverVoltage)
        tProtect.CountOverVoltage --;
    
    if(tProtect.CountUnderVoltage)
        tProtect.CountUnderVoltage --;
    
    if(tProtect.CountNtc)
        tProtect.CountNtc --;
    
    //软件过流保护 4A
    if(tAdc.AN4.ValueAverage > SOCTHRESHOLD)
    {
        if(tProtect.CountSOverCurrent == 0)
        {
            if((tProtect.Code == 0) || (tProtect.Code > 3))
            {
                tProtect.Code = 4;
            }
        }
    }
    else
    {
        tProtect.CountSOverCurrent = 100;
    }
    //Vbus 过欠压保护
    if(tAdc.VbusReal > OVTHRESHOLD)  //过压
    {
        if(tProtect.CountOverVoltage == 0)
        {
            if((tProtect.Code == 0) || (tProtect.Code > 4))
            {
                tProtect.Code = 5;
            }
        }
    }
    else if(tAdc.VbusReal < LVTHRESHOLD) //欠压
    {
        if(tProtect.CountUnderVoltage == 0)
        {
            if((tProtect.Code == 0) || (tProtect.Code > 5))
            {
                tProtect.Code = 6;
            }
        }
    }
    else if((tAdc.VbusReal >= LVRETHRESHOLD) && (tAdc.VbusReal <= OVRETHRESHOLD)) //过欠压恢复
    {
        if((tProtect.Code == 5) || (tProtect.Code == 6))
        {
            tProtect.Code = 0;
        }
        tProtect.CountUnderVoltage = 100;
        tProtect.CountOverVoltage = 100;
    }
    else //没有电压故障时，计数变量保持100
    {
        tProtect.CountUnderVoltage = 100;
        tProtect.CountOverVoltage = 100;
    }
    //温度故障监测
    if(tAdc.NtcReal > OTTHRESHOLD) //温度大于60度，报过温故障
    {
        if(tProtect.CountNtc == 0)
        {
            if((tProtect.Code == 0) || (tProtect.Code > 6))
            {
                tProtect.Code = 7;
            }
        }
    }
    else if(tAdc.NtcReal <= OTRETHRESHOLD) //温度在58度及以下，温度故障恢复
    {
        if(tProtect.Code == 7)
        {
            tProtect.Code = 0;
        }
        tProtect.CountNtc = 100;
    }
    else //没有过温情况下，计数变量保持100
    {
        tProtect.CountNtc = 100;
    }
}
/***********************************************************************
* 函数名称  : HardOverCurrentMonitor
* 功能描述  ：硬件过流监测
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
void HardOverCurrentMonitor(void)
{
    if(TIM1->SR & 0x00000080) //查询BIF是否为1，若为1，则有Break故障发生
    {
        TIM1->SR &= 0xffffffef; //BIF = 0 清除故障
        tProtect.CountHOverCurrent ++;
        if(tProtect.CountHOverCurrent >= 5) //连续满足5次Break
        {
            tProtect.CountHOverCurrent = 0;
            tProtect.Code = 1; //设置故障代码
        }
    }
    else
    {
        tProtect.CountHOverCurrent = 0;
    }
}
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
