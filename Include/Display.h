/***********************************************************************
* @file           DisPlay.h
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    LCD显示支持文件
***********************************************************************/
#include "../Include/Define.h"

void Display(void);

extern void LCD_ShowChinese(unsigned short x, unsigned short y, unsigned char *s, unsigned short fc, unsigned short bc, unsigned char sizey, unsigned char mode);
extern void LCD_ShowString(unsigned short x, unsigned short y, const unsigned char *p, unsigned short fc, unsigned short bc, unsigned char sizey, unsigned char mode);
extern void LCD_ShowIntNum(unsigned short x, unsigned short y, unsigned short num, unsigned char len, unsigned short fc, unsigned short bc, unsigned char sizey);

extern tMotor_Type tMotor;
extern tBldc_Type tBldc;
extern tProtect_Type tProtect;
extern tAdc_Type tAdc;
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
