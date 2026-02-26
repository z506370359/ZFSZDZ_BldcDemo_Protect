/***********************************************************************
* @file           LCD.h
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    LCD显示支持文件
***********************************************************************/
#include "../Include/stm32g030xx.h"
#include "../Include/LCDfont.h"

#define LCD_RES_Clr()  {GPIOD->BSRR |= 0x00010000;}//RES
#define LCD_RES_Set()  {GPIOD->BSRR |= 0x00000001;}

#define LCD_DC_Clr()   {GPIOD->BSRR |= 0x00020000;}//DC
#define LCD_DC_Set()   {GPIOD->BSRR |= 0x00000002;}
 		     
#define LCD_CS_Clr()   {GPIOA->BSRR |= 0x80000000;}//CS
#define LCD_CS_Set()   {GPIOA->BSRR |= 0x00008000;}

#define LCD_BLK_Clr()  {GPIOD->BSRR |= 0x00040000;}//BLK
#define LCD_BLK_Set()  {GPIOD->BSRR |= 0x00000004;}

#define USE_HORIZONTAL 3  // 设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 135
#define LCD_H 240

#else
#define LCD_W 240
#define LCD_H 135
#endif

void Delay_ms(unsigned short timer);
void LCD_Init(void); 				    //LCD初始化
void LCD_Writ_Bus(unsigned char dat);	// SPI时序
void LCD_WR_DATA8(unsigned char dat);	// 写入一个字节
void LCD_WR_DATA(unsigned short dat);	// 写入两个字节
void LCD_WR_REG(unsigned char dat);		// 写入一个指令
void LCD_Address_Set(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2); // 设置坐标函数

void LCD_Fill(unsigned short xsta,unsigned short ysta,unsigned short xend,unsigned short yend,unsigned short color); // 指定区域填充颜色
void LCD_DrawPoint(unsigned short x,unsigned short y,unsigned short color); // 在指定位置画一个点
void LCD_DrawLine(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2,unsigned short color); // 在指定位置画一条线
void LCD_DrawRectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,unsigned short color); // 在指定位置画一个矩形
void Draw_Circle(unsigned short x0,unsigned short y0,unsigned char r,unsigned short color); // 在指定位置画一个圆

void LCD_ShowChinese(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode); 			// 显示汉字串
void LCD_ShowChinese12x12(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode); // 显示单个12x12汉字
void LCD_ShowChinese16x16(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode); // 显示单个16x16汉字
void LCD_ShowChinese24x24(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode); // 显示单个24x24汉字
void LCD_ShowChinese32x32(unsigned short x,unsigned short y,unsigned char *s,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode); // 显示单个32x32汉字

void LCD_ShowChar(unsigned short x,unsigned short y,unsigned char num,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode);				// 显示一个字符
void LCD_ShowString(unsigned short x,unsigned short y,const unsigned char *p,unsigned short fc,unsigned short bc,unsigned char sizey,unsigned char mode); // 显示字符串
unsigned int mypow(unsigned char m,unsigned char n); // 求幂
void LCD_ShowIntNum(unsigned short x,unsigned short y,unsigned short num,unsigned char len,unsigned short fc,unsigned short bc,unsigned char sizey); // 显示整数变量
void LCD_ShowFloatNum(unsigned short x, unsigned short y, float num, unsigned char len, unsigned char decimal, unsigned short fc, unsigned short bc, unsigned char sizey);	// 显示带符号的浮点数
void LCD_ShowFloatNum1(unsigned short x, unsigned short y, float num, unsigned char len, unsigned char decimal, unsigned short fc, unsigned short bc, unsigned char sizey);	// 显示正的浮点数

void LCD_ShowPicture(unsigned short x,unsigned short y,unsigned short length,unsigned short width,const unsigned char pic[]); //显示图片

/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
