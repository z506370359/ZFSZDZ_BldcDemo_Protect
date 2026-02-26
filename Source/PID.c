/***********************************************************************
* @file           PID.c
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    PID逻辑模块
***********************************************************************/
#include "../Include/PID.h"

/***********************************************************************
* 函数名称  : PIInit
* 功能描述  ：PI控制器参数初始化
* 形参      ：tPI_Type *PIStruct
              Kp ：比例系数  Ki ：积分系数  Kc ：抗积分饱和系数 
              Max ：输出限幅最大值  Min ：输出限幅最小值
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
void PIInit(tPI_Type *PIStruct,signed short Kp,signed short Ki,signed short Kc,signed short Max,signed short Min)
{
    PIStruct->qKp = Kp;
    PIStruct->qKi = Ki;
    PIStruct->qKc = Kc;
    PIStruct->qOutMax = Max;
    PIStruct->qOutMin = Min;
    PIStruct->qOut = 0;
    PIStruct->qSum = 0;
}
/***********************************************************************
* 函数名称  : PIControl
* 功能描述  ：抗积分饱和位置式PI控制器
* 形参      ：tPI_Type *PIStruct
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
void PIControl(tPI_Type *PIStruct)
{
    signed short Current_Error; //当前误差
    signed int U;
    signed short Out_Temp;
    
    Current_Error = PIStruct->qInRef - PIStruct->qInMeas;
    U = PIStruct->qKp * Current_Error; //Q30
    U = U + PIStruct->qSum; //PI控制器的输出
    Out_Temp = (signed short)(U >> 15); //Q15
    
    if(Out_Temp > PIStruct->qOutMax) //PI控制器限幅处理
        PIStruct->qOut = PIStruct->qOutMax;
    else if(Out_Temp < PIStruct->qOutMin)
        PIStruct->qOut = PIStruct->qOutMin;
    else
        PIStruct->qOut = Out_Temp;
    
    U = Current_Error * PIStruct->qKi;
    Current_Error = Out_Temp - PIStruct->qOut;
    U -= Current_Error * PIStruct->qKc;
    PIStruct->qSum = PIStruct->qSum + U; //抗积分饱和处理之后的积分项
}

/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
