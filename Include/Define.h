/***********************************************************************
* @file           Define.h
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    张飞实战电子"扬帆起航"课程电机控制部分
***********************************************************************/

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE           	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)
/*==================================================================================*/
typedef struct
{
    unsigned short ADDATA0;
    unsigned short ADDATA1;
    unsigned short ADDATA2;
    unsigned short ADDATA3;
    unsigned short ADDATA4;
}tDMA_Type;
/*==================================================================================*/
typedef struct
{
    unsigned short ValueInstant; //瞬时值
    unsigned short ValueAverage; //平均值
    unsigned short ValueFilter; //滤波值
    unsigned short Num; //计数值
    unsigned int Sum; //求和值
}tAnx;
/*==================================================================================*/
typedef struct
{
    tAnx AN0,AN3,AN4,AN5,AN9;
    unsigned short PotValue; //旋钮采样转换值
    unsigned short VbusReal; //母线电压真实值
    unsigned short SetSpeed; //旋转设定转速值
    signed char NtcReal; //温度真实值
}tAdc_Type;
/*==================================================================================*/
typedef struct
{
    unsigned short StartDelayCount; //上电延时计数变量
    unsigned short LcdCount; //LCD刷新计数变量
    unsigned short UsartCount; //串口发送一帧数据计数变量
    unsigned short DutyCount; //占空比变化的计数变量
}tSystem_Type;
/*==================================================================================*/
typedef struct
{
    unsigned char State; //BLDC运行状态
    unsigned char PhaseTest;
    unsigned char HallData; //霍尔组合值
    unsigned char StartCount; //启动换相计数
    unsigned char StartState; //启动状态判断
    unsigned char StartFailCount; //占空比加大到最大值后，还是不能正常转动计数
    unsigned char PhaseTimeNum; //60°时间存储计数变量
    unsigned char SpeedUpDate; //速度更新标志
    unsigned char SpeedEnable; //速度环使能标志
    unsigned short MaxNoSwitchTime;//最大不换相时间
    unsigned short PwmTick; //PWM周期计数器
    unsigned short Duty;
    unsigned short PhaseTime; //60°对应的计数值/时间
    unsigned short PhaseTimeArray[6]; //60度计数值存储
    unsigned short SetNoPhaseTime; //设置不换相时间
}tBldc_Type;
/*==================================================================================*/
typedef struct
{
    unsigned char RunStopState; //电机启停状态
    unsigned char RunMode; //电机运行模式  开环 or 闭环
    unsigned char Direction; //电机运行方向
    unsigned char SpeedMode; //速度模式设置 旋钮 or 串口
    unsigned short ActSpeed; //电机实际转速
    unsigned short AimSpeed; //电机目标转速
    unsigned short VarDelta; //斜坡增量值
    unsigned short AimDuty; //目标占空比
    unsigned short InstantSpeed; //电机瞬时速度
    unsigned short DisplaySpeed; //LCD显示速度
}tMotor_Type;
/*==================================================================================*/
typedef struct
{
    unsigned short SendReq:1; //数据发送请求标志
    unsigned short RxStart:1; //开始接收数据标志
    unsigned short Unusual:1; //数据无效标志
    unsigned short ReceReq:1; //接收数据处理请求
    unsigned short TxStart:1; //开始发送一帧数据标志
    unsigned short RESERVED:11; //保留位
}STRUCT_USART_FLAG;
/*==================================================================================*/
typedef struct
{
    unsigned char TxNo; //发送数据包计数变量
    
    STRUCT_USART_FLAG Flag; //串口标志
    unsigned short TxData; //发送数据变量
    unsigned short TxSum; //校验和求取
    unsigned short TxNum; //发送数据个数
    unsigned short RxSum; //校验和求取
    unsigned short RxNum; //接收数据个数
    unsigned short RxDataBuf[3]; //接收有效数据BUF
    unsigned short FmVersion; //固件版本号
    unsigned short Duty; //串口通讯 开环占空比设定
    unsigned short SetSpeed; //串口通讯 闭环给定速度设定
}tUsart_Type;
/*==================================================================================*/
typedef struct
{
    unsigned char Code; //故障代码
    unsigned char CountHOverCurrent; //硬件过流计数
    unsigned char CountSOverCurrent; //软件过流计数
    unsigned char CountOverVoltage; //Vbus过压计数
    unsigned char CountUnderVoltage; //Vbus欠压计数
    unsigned char CountNtc; //温度保护计数
    unsigned short CountLockRotorPhase;//堵转相位保护计数
    unsigned short CountLockRotorCurrent;//堵转电流保护计数
}tProtect_Type;
/*==================================================================================*/
typedef union
{
    unsigned char byte;
    struct
    {
        unsigned char B0 : 1;
        unsigned char B1 : 1;
        unsigned char B2 : 1;
        unsigned char B3 : 1;
        unsigned char B4 : 1;
        unsigned char B5 : 1;
        unsigned char B6 : 1;
        unsigned char B7 : 1;
    }Bits;
}UNION_U8;
/*==================================================================================*/
typedef struct
{
    signed short qKp; //比例项系数
    signed short qKi; //积分项系数
    signed short qKc; //抗积分饱和系数
    signed short qOutMax; //输出最大值
    signed short qOutMin; //输出最小值
    signed short qOut; //PI控制器输出值
    signed short qInRef; //目标给定值
    signed short qInMeas; //实际测量值
    signed int qSum; //积分项
}tPI_Type;
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
