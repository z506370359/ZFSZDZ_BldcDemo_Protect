/***********************************************************************
* @file           Initialize.h
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    张飞实战电子"扬帆起航"课程电机控制部分
***********************************************************************/
#include "../Include/stm32g030xx.h"
#include "../Include/Define.h"

void Initialize(void);
static void SysClockInitialize(void);
static void GpioInitialize(void);
static void PwmInitialize(void);
static void AdcInitialize(void);
static void DmaInitialize(void);
static void Timer3Initialize(void);
static void Timer1617Initialize(void);
static void UsartInitialize(void);
static void SpiInitialize(void);
static void NVICInitialize(void);
static void SysTickInitialize(void);

extern void LCD_Init(void);
extern void LCD_Fill(unsigned short xsta,unsigned short ysta,unsigned short xend,unsigned short yend,unsigned short color);

extern void MotorInit(void);
extern void ProtectInit(void);

tDMA_Type tDMA;
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
