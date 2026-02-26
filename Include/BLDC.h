/***********************************************************************
* @file           BLDC.h
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    电机运行逻辑模块
***********************************************************************/
#include "../Include/stm32g030xx.h"
#include "../Include/Define.h"
#include "../Include/UserParams.h"

void BLDC_StateMachine(void);
void BLDC_PWM_ON_OFF(unsigned char Phase);
unsigned char BLDC_ReadHallValue(void);
unsigned char BLDC_HallToPhase(unsigned char Hall,unsigned char Dir);

static void BLDC_UV(void);
static void BLDC_WV(void);
static void BLDC_WU(void);
static void BLDC_VU(void);
static void BLDC_VW(void);
static void BLDC_UW(void);
static void BLDC_PWMOFF(void);
    
    
extern void MotorStop(void);

tBldc_Type tBldc;
extern tMotor_Type tMotor;
extern tProtect_Type tProtect;
extern tAdc_Type tAdc;
extern tUsart_Type tUsart;

#define  STOP_STATE          0
#define  CHARGEREADY_STATE   1
#define  CHARGEWAIT_STATE    2
#define  STARTINIT_STATE     3
#define  MOTORRUN_STATE      4
#define  TEST_STATE          5
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
