/***********************************************************************
* @file           Protect.h
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    故障处理模块
***********************************************************************/
#include "../Include/stm32g030xx.h"
#include "../Include/Define.h"

void ProtectInit(void);
void HardOverCurrentMonitor(void);
void ProtectApp(void);
    
tProtect_Type tProtect;
extern tAdc_Type tAdc;

#define SOCTHRESHOLD   496   //4A * 0.02 * 5 / 3.3V * 4096 = 496
#define OVTHRESHOLD 300  //30V过压
#define OVRETHRESHOLD 290  //29V过压恢复

#define LVTHRESHOLD 210  //21V欠压
#define LVRETHRESHOLD 220  //22V欠压恢复

#define OTTHRESHOLD  60 //60度过温故障
#define OTRETHRESHOLD  58 //58度过温故障恢复

/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
