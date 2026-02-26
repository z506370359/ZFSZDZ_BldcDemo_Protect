/***********************************************************************
* @file           MathCalc.c
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    数学计算模块
***********************************************************************/
#include "../Include/MathCalc.h"

/***********************************************************************
* 函数名称  ：LowPassFilter
* 功能描述  ：一阶数字低通滤波器
* 形参      ：K：滤波系数 Xn：待滤波值 Yn-1：上一次滤波值
* 返回值    ：Yn：滤波后的值
* 要点备注  ：K越小，滤波效果越好，但可能有延迟  K越大，滤波效果差，实时性好
              K使用Q16定点化处理  K = Ts*2*π*fc
              Yn = K*Xn + (1 - K)*Yn-1
***********************************************************************/
unsigned short LowPassFilter(unsigned short K,unsigned short x,unsigned short y)
{
    unsigned short Temp;
    
    Temp = (unsigned short)(K * x >> 16) + (unsigned short)(((65536 - K) * y) >> 16);
    
    return (Temp);
}
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
