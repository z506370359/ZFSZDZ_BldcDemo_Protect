/***********************************************************************
* @file           PID.h
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    PID逻辑模块
***********************************************************************/
#include "../Include/Define.h"

void PIInit(tPI_Type *PIStruct,signed short Kp,signed short Ki,signed short Kc,signed short Max,signed short Min);
void PIControl(tPI_Type *PIStruct);

/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
