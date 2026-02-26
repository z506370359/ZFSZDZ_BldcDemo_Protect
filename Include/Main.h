/***********************************************************************
* @file           Main.h
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    张飞实战电子"扬帆起航"课程电机控制部分 
***********************************************************************/
#include "../Include/stm32g030xx.h"
#include "../Include/Define.h"

extern void Initialize(void);

extern void Display(void);

extern void AdcSample(void);
extern void AdcFilter(void);
extern void AdcApp(void);

extern void MotorApp(void);

extern void BLDC_StateMachine(void);

extern void SentReceData(void);
extern void UsartApp(void);
    
extern void BLDC_PWM_ON_OFF(unsigned char Phase);
extern unsigned char BLDC_ReadHallValue(void);
extern unsigned char BLDC_HallToPhase(unsigned char Hall,unsigned char Dir);

extern void HardOverCurrentMonitor(void);

extern void ProtectApp(void);

unsigned int TIM3_CCR1;

tSystem_Type tSystem;
extern tBldc_Type tBldc;
extern tMotor_Type tMotor;
extern tProtect_Type tProtect;
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
