/***********************************************************************
* @file           Usart.c
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    串口通讯模块
***********************************************************************/
#include "../Include/Usart.h"

/***********************************************************************
* 函数名称  : UsartApp
* 功能描述  ：串口通讯应用程序
* 形参      ：无
* 返回值    ：无
* 要点备注  ：在主循环中调用
***********************************************************************/
void UsartApp(void)
{
    if(tSystem.UsartCount == 0)
    {
        tSystem.UsartCount = 1000;
        tUsart.Flag.TxStart = 1;
    }
    SendDataSetup(); //发送数据准备
    ReceDataProcess(); //接收数据解析
}
/***********************************************************************
* 函数名称  : SendDataSetup
* 功能描述  ：发送数据准备
* 形参      ：无
* 返回值    ：无
* 要点备注  ：在串口应用程序里面调用
***********************************************************************/
static void SendDataSetup(void)
{
    if(tUsart.Flag.TxStart) //判断是否可以发送一帧数据
    {
        if(!tUsart.Flag.SendReq) //判断上一个字节数据是否发送
        {
            if(tUsart.TxNum < 14)
            {
                switch(tUsart.TxNum)
                {
                    case 0:
                        tUsart.TxData = 0x002a; //帧头
                        break;
                    case 1:
                        tUsart.TxData = tUsart.TxNo ++; //数据包计数
                        break;
                    case 2:
                        tUsart.TxData = tAdc.VbusReal >> 8; //Vbus电压高8位
                        break;
                    case 3:
                        tUsart.TxData = tAdc.VbusReal & 0x00ff; //Vbus电压低8位
                        break;
                    case 4:
                        tUsart.TxData = tAdc.NtcReal; //温度真实值
                        break;
                    case 5:
                        tUsart.TxData = tMotor.AimSpeed >> 8; //目标转速高8位
                        break;
                    case 6:
                        tUsart.TxData = tMotor.AimSpeed & 0x00ff; //目标转速低8位
                        break;
                    case 7:
                        tUsart.TxData = tMotor.ActSpeed >> 8; //实际转速高8位
                        break;
                    case 8:
                        tUsart.TxData = tMotor.ActSpeed & 0x00ff; //实际转速低8位
                        break;
                    case 9:
                        tUsart.TxData = tBldc.State; //状态机状态
                        break;
                    case 10:
                        tUsart.TxData = tProtect.Code; //故障代码
                        break;
                    case 11:
                        tUsart.TxData = tUsart.FmVersion >> 8; //软件版本号高8位
                        break;
                    case 12:
                        tUsart.TxData = tUsart.FmVersion & 0x00ff; //软件版本号低8位
                        break;
                    case 13:
                        tUsart.TxData = 0x0023; //帧尾
                        break;
                    default:
                        break;
                }
                tUsart.TxNum ++; //发送数据计数
                tUsart.TxSum += tUsart.TxData; //求取数据累加和
                tUsart.Flag.SendReq = 1; //数据发送请求标志置位
            }
            else
            {
                switch(tUsart.TxNum)
                {
                    case 14:
                        tUsart.TxNum ++;
                        tUsart.TxSum = ~tUsart.TxSum + 1;
                        tUsart.TxData = tUsart.TxSum;
                        tUsart.Flag.SendReq = 1;
                        break;
                    case 15:
                        tUsart.TxNum = 0;
                        tUsart.TxSum = 0;
                        tUsart.Flag.TxStart = 0;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
/***********************************************************************
* 函数名称  : ReceDataProcess
* 功能描述  ：串口通讯接收数据处理
* 形参      ：无
* 返回值    ：无
* 要点备注  ：对上位机下发的数据进行解析
***********************************************************************/
static void ReceDataProcess(void)
{
    unsigned short Temp16;
    
    if(tUsart.Flag.ReceReq)
    {
        if(tUsart.RxDataBuf[0] == USART_SET_MODE) //电机运行模式设定
        {
            if(tMotor.RunStopState == 0)
            {
                tMotor.RunMode = tUsart.RxDataBuf[1];
            }
        }
        
        if(tUsart.RxDataBuf[0] == USART_SET_DIRECTION) //电机运行方向设置
        {
            if(tMotor.RunStopState == 0)
            {
                tMotor.Direction = tUsart.RxDataBuf[1];
            }
        }
        
        if(tUsart.RxDataBuf[0] == USART_SET_SPEEDMODE) //速度调节模式设置
        {
            tMotor.SpeedMode = tUsart.RxDataBuf[1];
        }
        
        if(tUsart.RxDataBuf[0] == USART_SET_DUTY) //开环占空比设置
        {
            Temp16 = tUsart.RxDataBuf[1];
            Temp16 |= tUsart.RxDataBuf[2] << 8;
            tUsart.Duty = Temp16;
        }
        
        if(tUsart.RxDataBuf[0] == USART_SET_SPEED) //闭环目标速度设置
        {
            Temp16 = tUsart.RxDataBuf[1];
            Temp16 |= tUsart.RxDataBuf[2] << 8;
            tUsart.SetSpeed = Temp16;
        }
        
        tUsart.Flag.ReceReq = 0;
    }
}
/***********************************************************************
* 函数名称  : SentReceData
* 功能描述  ：发送接收数据状态查询
* 形参      ：无
* 返回值    ：无
* 要点备注  ：在DMA中断中调用 62.5us
***********************************************************************/
void SentReceData(void)
{
    unsigned short Temp16;
    
    //发送数据逻辑代码
    if(tUsart.Flag.SendReq)
    {
        if(USART1->ISR & 0x00000080) //查询TXE是否为1，判断发送数据寄存器不满
        {
            USART1->TDR = tUsart.TxData;
            tUsart.Flag.SendReq = 0;
        }
    }
    //接收数据逻辑代码
    if(USART1->ISR & 0x00000020) //查询RXNE是否为1，判断有没有数据接收到
    {
        if(tUsart.Flag.ReceReq == 0) //数据解析完成后，再次接收下一帧数据
        {
            Temp16 = USART1->RDR;
            Temp16 &= 0x00ff;
            
            if(tUsart.Flag.RxStart == 0)
            {
                if(Temp16 == 0x002a) //判断帧头是否正确？
                {
                    tUsart.Flag.RxStart = 1;
                    tUsart.RxSum = Temp16;
                    tUsart.RxNum = 0;
                }
            }
            else
            {
                if(tUsart.RxNum < 3)
                {
                    tUsart.RxDataBuf[tUsart.RxNum++] = Temp16;
                    tUsart.RxSum += Temp16;
                }
                else
                {
                    if(tUsart.RxNum == 3)
                    {
                        if(Temp16 == 0x0023) //检测帧尾是否正确？
                        {
                            tUsart.RxNum ++;
                            tUsart.RxSum += Temp16;
                        }
                        else
                        {
                            tUsart.Flag.Unusual = 1; //该帧数据无效
                            tUsart.Flag.RxStart = 0; //数据重新接收
                        }
                    }
                    else
                    {
                        if(tUsart.RxNum == 4)
                        {
                            tUsart.RxSum += Temp16;
                            tUsart.RxSum &= 0x00ff;
                            if(tUsart.RxSum == 0) //数据包数据是正确
                            {
                                tUsart.Flag.ReceReq = 1;//数据正确，可以处理（解析）
                                tUsart.Flag.Unusual = 0;
                            }
                            else
                            {
                                tUsart.Flag.Unusual = 1;
                            }
                            tUsart.Flag.RxStart = 0;
                        }
                    }    
                }
            }        
        }
    }
}
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
