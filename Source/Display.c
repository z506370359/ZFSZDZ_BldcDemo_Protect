/***********************************************************************
* @file           DisPlay.c
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    LCD显示支持文件
***********************************************************************/
#include "../Include/Display.h"

void Display(void)
{
	LCD_ShowChinese(0,3,(unsigned char *)"张飞实战电子电机控制",GREEN,BLACK,24,0);
    
    LCD_ShowChinese(10,40,(unsigned char *)"电压",GREEN,BLACK,16,0);
    LCD_ShowString(45,40,(unsigned char *)":",WHITE,BLACK,16,0);
    LCD_ShowIntNum(55, 40,tAdc.VbusReal, 3, BRRED, BLACK, 16);
    LCD_ShowString(85, 40, (unsigned char *)"V", WHITE, BLACK, 16, 0);
    
    LCD_ShowChinese(125,40,(unsigned char *)"故障代码",GREEN,BLACK,16,0);
    LCD_ShowString(190,40,(unsigned char *)":",WHITE,BLACK,16,0);
    LCD_ShowIntNum(200,40,tProtect.Code,1, BRRED, BLACK, 16);
    
    LCD_ShowChinese(10,60,(unsigned char *)"温度",GREEN,BLACK,16,0);
    LCD_ShowString(45,60,(unsigned char *)":",WHITE,BLACK,16,0);
    LCD_ShowIntNum(55, 60, tAdc.NtcReal, 3, BRRED, BLACK, 16);
    LCD_ShowString(85, 60, (unsigned char *)"C", WHITE, BLACK, 16, 0);
    
    LCD_ShowChinese(125,60,(unsigned char *)"调试信息",GREEN,BLACK,16,0);
    LCD_ShowString(190,60,(unsigned char *)":",WHITE,BLACK,16,0);
    LCD_ShowIntNum(200,60,tBldc.State, 1, BRRED, BLACK, 16);
    
    LCD_ShowChinese(10,80,(unsigned char *)"设定转速",GREEN,BLACK,16,0);
    LCD_ShowString(75,80,(unsigned char *)":",WHITE,BLACK,16,0);
    LCD_ShowIntNum(85, 80, tMotor.AimSpeed, 4, BRRED, BLACK, 16);
    LCD_ShowString(125, 80, (unsigned char *)"RPM", WHITE, BLACK, 16, 0);
    
    LCD_ShowChinese(10,100,(unsigned char *)"实际转速",GREEN,BLACK,16,0);
    LCD_ShowString(75,100,(unsigned char *)":",WHITE,BLACK,16,0);
    LCD_ShowIntNum(85, 100, tMotor.DisplaySpeed, 4, BRRED, BLACK, 16);
    LCD_ShowString(125, 100, (unsigned char *)"RPM", WHITE, BLACK, 16, 0);
}
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
