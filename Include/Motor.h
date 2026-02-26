/***********************************************************************
* @file           Motor.h
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    电机应用模块
***********************************************************************/
#include "../Include/stm32g030xx.h"
#include "../Include/Define.h"
#include "../Include/UserParams.h"

void MotorApp(void);
void MotorStop(void);
void MotorInit(void);
unsigned short GetAimSet(unsigned short Var,unsigned short Aim,unsigned short MaxDelta,unsigned short MinValue);
void MotorSetDuty(unsigned short Var);
void MotorSetSpeed(unsigned short Var);
void CalcMotorSpeed(void);
void MotorSpeedControl(void);

static void MotorRunStopControl(void);

extern unsigned short LowPassFilter(unsigned short K,unsigned short x,unsigned short y);
extern void PIControl(tPI_Type *PIStruct);
extern void PIInit(tPI_Type *PIStruct,signed short Kp,signed short Ki,signed short Kc,signed short Max,signed short Min);

tMotor_Type tMotor;
tPI_Type tPI_Speed;


extern tAdc_Type tAdc;
extern tBldc_Type tBldc;
extern tUsart_Type tUsart;
extern tProtect_Type tProtect;
extern tSystem_Type tSystem;
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
