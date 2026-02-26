/***********************************************************************
* @file           Initialize.c
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    张飞实战电子"扬帆起航"课程电机控制部分
***********************************************************************/
#include "../Include/Initialize.h"

/***********************************************************************
* 函数名称  ：Initialize
* 功能描述  ：系统初始化函数
* 形参      ：无
* 返回值    ：无
* 要点备注  ：无
***********************************************************************/
void Initialize(void)
{
    SysClockInitialize(); //系统时钟初始化设置
    GpioInitialize(); //普通数字IO端口初始化设置
    PwmInitialize(); //PWM模块初始化设置
    AdcInitialize(); //ADC模块初始化设置
    DmaInitialize(); //DMA及DMAMUX模块初始化设置
    Timer3Initialize(); //定时器3模块初始化设置
    Timer1617Initialize(); //定时器16 17模块初始化设置
    SysTickInitialize(); //滴答定时器模块初始化设置
    UsartInitialize(); //串口模块1初始化设置
    SpiInitialize(); //SPI模块初始化设置
    NVICInitialize(); //NVIC模块初始化设置
    
    MotorInit(); //电机模块初始化函数
    ProtectInit(); //故障处理初始化函数
    
    LCD_Init();
    LCD_Fill(0,0,240,135,BLACK); //填充黑色
}

/***********************************************************************
* 函数名称  ：SysClockInitialize
* 功能描述  ：系统时钟初始化配置
* 形参      ：无
* 返回值    ：无
* 要点备注  ：晶振频率：12MHz  主频：64MHz
* fVCO = fPLLIN × (N / M)   M = 3  N = 80
* N：fvco ：64MHz~344MHz  M：确保分频后时钟在2.66MHz~16MHz
* fPLLP = fVCO / P
* fPLLR = fVCO / R  R = 5
***********************************************************************/
static void SysClockInitialize(void)
{
    unsigned int Temp;
    
    //打开HSE 并等待HSE准备好
    RCC->CR |= 0x00010000; //HSEON = 1
    
    do
    {
        Temp = RCC->CR;
    }while((Temp & 0x00020000) == 0); //判断HSERDY是否为1
    //设置FLASH的访问周期
    FLASH->ACR |= 0x00000100;// PRFTEN = 1 使能指令预取功能
    FLASH->ACR |= 0x00000002;// LATENCY[2:0] = 0b010 读访问Flash 3HCLK时钟周期
    //设置PLL参数
    RCC->PLLCFGR = 0x90075023;//PLLR[2:0] = 0b100 (5) PLLREN = 1 PLLP[4:0] = 0b00011 (4) PLLPEN = 1 PLLN = 0b0101 0000 (80) 
                              //PLLM = 0b010 (3) PLLSRC = 0b11 HSE 
    //打开PLL
    RCC->CR |= 0x01000000;//PLLON = 1  
    while((RCC->CR & 0x02000000) == 0); //等待PLL锁定完成
    //进行时钟切换
    RCC->CFGR |= 0x00000002; //SW[2:0] = 0b010 PLLRCLK
    while((RCC->CFGR & 0x00000038) == 0); //等待时钟切换完成
    
    //RCC->CFGR |= 0x61000000; //MCOPRE[3:0] = 0b0000 (1) MCOSEL[3:0] = 0b0001 SYSCLK
    
    RCC->IOPENR |= 0x0000000f; //GPIOAEN = 1 GPIOBEN = 1 GPIOCEN = 1 GPIODEN = 1
    RCC->APBENR2 |= 0x0016d800; //ADCEN = 1 TIM17EN = 1 TIM16EN = 1 TIM14EN = 1 USART1EN = 1 SPI1EN = 1 TIM1EN = 1 
    RCC->AHBENR |= 0x00000001; //DMA1EN = 1
    RCC->APBENR1 |= 0x00000002; //TIM3EN = 1
}

/***********************************************************************
* 函数名称  ：GpioInitialize
* 功能描述  ：IO端口初始化
* 形参      ：无
* 返回值    ：无
* 要点备注  ：PC14-->DR (O)   PC15-->KEY (I)  PC6--> LED2 (O)  PC7-->LED1 (O)
*             PD0-->RES (O)   PD1-->DC (O)    PD2-->BLK (O)    PD3-->TP (O)
*             PB4-->LED3(O)
***********************************************************************/
static void GpioInitialize(void)
{
    GPIOB->MODER &= 0xfffffcff; //MODE4[1:0] = 0b00
    GPIOB->MODER |= 0x00000100; //MODE4[1:0] = 0b01 General purpose output mode
    
    GPIOB->OSPEEDR |= 0x00000200; //OSPEED4[1:0] = 0b10 High speed
    GPIOB->BSRR |= 0x00100000; //BR4 = 1 PB4输出低电平
    
    GPIOD->MODER &= 0xffffff00; //MODE3[1:0] = 0b00  MODE2[1:0] = 0b00 
                                //MODE1[1:0] = 0b00  MODE0[1:0] = 0b00
    GPIOD->MODER |= 0x00000055; //MODE3[1:0] = 0b01  MODE2[1:0] = 0b01 
                                //MODE1[1:0] = 0b01  MODE0[1:0] = 0b01  General purpose output mode
    
    GPIOD->OSPEEDR |= 0x000000aa; //OSPEED3[1:0] = 0b10  OSPEED2[1:0] = 0b10
                                  //OSPEED1[1:0] = 0b10  OSPEED0[1:0] = 0b10 High speed
    GPIOD->BSRR |= 0x000f0000; //BR3 = 1 BR2 = 1  BR1 = 1  BR0 = 1 PD0/1/2/3输出低电平
    
    GPIOC->MODER &= 0x0fff0fff; //MODE15[1:0] = 0b00  MODE14[1:0] = 0b00 
                                //MODE6[1:0] = 0b00  MODE7[1:0] = 0b00
    
    GPIOC->MODER |= 0x10005000; //MODE15[1:0] = 0b00 INPUT  MODE14[1:0] = 0b01 OUTPUT
                                //MODE6[1:0] = 0b01         MODE7[1:0] = 0b01  OUTPUT
    GPIOC->OSPEEDR |= 0x2000a000; //OSPEED14[1:0] = 0b10  OSPEED7[1:0] = 0b10 OSPEED6[1:0] = 0b10  High speed
    GPIOC->BSRR |= 0x40c00000;//BR14 = 1 BR6 = 1  BR7 = 1 PC14/6/7 输出低电平
}

/***********************************************************************
* 函数名称  ：PwmInitialize
* 功能描述  ：PWM模块初始化
* 形参      ：无
* 返回值    ：无
* 要点备注  ：PA8-->TIM1_CH1    PA9-->TIM1_CH2    PA10-->TIM1_CH3
              PB13-->TIM1_CH1N  PB14-->TIM1_CH2N  PB15-->TIM1_CH3N
              PC13-->TIM1_BKIN
***********************************************************************/
static void PwmInitialize(void)
{
    //IO端口复用功能配置
    GPIOA->MODER &= 0xffc0ffff; //MODE10[1:0] = 0b00 MODE9[1:0] = 0b00 MODE8[1:0] = 0b00
    GPIOA->MODER |= 0x002a0000; //MODE10[1:0] = 0b10 MODE9[1:0] = 0b10 MODE8[1:0] = 0b10
    
    GPIOA->OSPEEDR |= 0x002a0000; //OSPEED10[1:0] = 0b10  OSPEED9[1:0] = 0b10 OSPEED8[1:0] = 0b10  High speed
    GPIOA->AFR[1] |= 0x00000222;  //AFSEL10[3:0] = 0b0010 AFSEL9[3:0] = 0b0010 AFSEL8[3:0] = 0b0010  AF2 TIM1_CH1/2/3
    
    GPIOB->MODER &= 0x03ffffff; //MODE15[1:0] = 0b00 MODE14[1:0] = 0b00 MODE13[1:0] = 0b00
    GPIOB->MODER |= 0xa8000000; //MODE15[1:0] = 0b10 MODE14[1:0] = 0b10 MODE13[1:0] = 0b10
     
    GPIOB->OSPEEDR |= 0xa8000000; //OSPEED15[1:0] = 0b10 OSPEED14[1:0] = 0b10 OSPEED13[1:0] = 0b10 High speed
    GPIOB->AFR[1] |= 0x22200000; //AFSEL15[3:0] = 0b0010 AFSEL14[3:0] = 0b0010 AFSEL13[3:0] = 0b0010 AF2 TIM1_CH1N/2N/3N
    
    GPIOC->MODER &= 0xf3ffffff; //MODE13[1:0] = 0b00
    GPIOC->MODER |= 0x08000000; //MODE13[1:0] = 0b10
    GPIOC->AFR[1] |= 0x00200000; //AFSEL13[3:0] = 0b0010 AF2 TIM1_BKIN
    
    //PWM模块功能配置
    TIM1->CR1 = 0x00a0; //ARPE = 1  CMS[1:0] = 0b01 中心对齐模式
    TIM1->CR2 = 0x00700000; //MMS2[3:0] = 0b0111 OC4REFC signal is used as trigger output (TRGO2)
    TIM1->CNT = 0; //清零计数器
    TIM1->PSC = 0; //没有分频
    TIM1->ARR = 1999; //64MHz / 16KHz / 2 = 2000 - 1
    
    TIM1->CCMR1 = 0x00004848;//OC1/2CE = 0 OC1/2M[2:0] = 0b100 Force inactive level OC1/2PE = 1 OC1/2FE = 0 CC1/2S[1:0] = 0b00 OUT
    TIM1->CCMR2 = 0x00006848; //OC3/4CE = 0 OC3/4M[2:0] = 0b100 Force inactive level OC3/4PE = 1 OC3/4FE = 0 CC3/4S[1:0] = 0b00 OUT
    TIM1->CCER = 0x00001ddd; //CC1/2/3/4E = 1  CC1/2/3P = 0 CC1/2/3NE = 1 CC1/2/3NP = 1
    
    TIM1->CCR1 = 0; //比较值设置
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;        
    TIM1->CCR4 = 2;
    
    TIM1->BDTR = 0x0009d000; //MOE = 1 AOE = 1 BKE = 1  BKF[3:0] = 0b1001
    TIM1->AF2 = 0; //BK2INE = 0
    TIM1->CR1 |= 0x0001; //CNE = 1 使能定时器1
}
/***********************************************************************
* 函数名称  ：AdcInitialize
* 功能描述  ：ADC模块初始化
* 形参      ：无
* 返回值    ：无
* 要点备注  ：PA0-->A   PA1-->B  PA2-->C  PA3-->VBUS  PA4-->IBUS 
              PA5-->POT PB1-->NTC_IN9
***********************************************************************/
static void AdcInitialize(void)
{
    TIM14->ARR = 65535;
    TIM14->CNT = 0; //清零计数器
    TIM14->PSC = 0; //没有分频
    TIM14->CR1 = 0x0080; //ARPE = 1
    TIM14->CR1 |= 0x0001; //CEN = 1
    
    //打开电压调节器并等待它稳定
    ADC1->CR |= 0x10000000; //ADVREGEN = 1 打开电压调节器
    TIM14->CNT = 0;
    while(TIM14->CNT < 6400); //等待100us 确保ADC电压调节器稳定
    TIM14->CR1 &= 0xfffe; //CEN = 0
    
    //对ADC模块进行校准
    ADC1->CR |= 0x80000000; //ADCAL = 1 
    while((ADC1->CR & 0x80000000) != 0); //等待ADC校准结束
    
    //配置ADC模块功能
    ADC1->CFGR1 = 0x00201400; //CHSELRMOD = 1 序列 OVRMOD = 1 新结果覆盖老结果 EXTEN[1:0] = 0b01  上升沿触发
    ADC1->CFGR2 = 0x40000000; //CKMODE[1:0]= 0b01 PCLK/2 = 32MHz
    ADC1->SMPR = 0; //1.5 ADC clock cycles
    ADC1->CHSELR = 0x00f95430;  //设置通道
    while((ADC1->ISR & 0x00002000) == 0); //等待通道配置OK
    
    ADC1->CR |= 0x00000001;// ADEN = 1 使能ADC
    while((ADC1->ISR & 0x00000001) == 0); //等待ADC准备好
    ADC1->CFGR1 |= 0x00000003;// DMACFG = 1 DMA circular mode selected  DMAEN = 1 
    ADC1->CR |= 0x00000004;//ADSTART = 1 开始ADC转换
}
/***********************************************************************
* 函数名称  ：DmaInitialize
* 功能描述  ：DMA模块初始化及DMAMUX设置
* 形参      ：无
* 返回值    ：无
* 要点备注  ：通过DMA搬运ADC数据寄存器数据
***********************************************************************/
static void DmaInitialize(void)
{
    DMA1_Channel1->CPAR = (unsigned int)&ADC1->DR;//外设寄存器地址
    DMA1_Channel1->CMAR = (unsigned int)&tDMA; //内存基地址
    DMA1_Channel1->CNDTR = 5; //传输5个数据
    DMA1_Channel1->CCR = 0x000035a2; //MEM2MEM = 0 PL[1:0] = 0b11 MSIZE[1:0]=0b01 16bit PSIZE[1:0]=0b01 16bit
                                     //MINC = 1 内存地址自增 PINC = 0 外设地址不自增 CIRC = 1 循环搬运
                                     //DIR = 0 数据从外设到内存 TCIE = 1 传输完成中断
    DMA1_Channel1->CCR |= 0x00000001;//EN = 1 使能DMA通道
    
    DMAMUX1_Channel0->CCR = 5; //ADC 
}
/***********************************************************************
* 函数名称  ：Timer3Initialize
* 功能描述  ：定时器3模块初始化配置
* 形参      ：无
* 返回值    ：无
* 要点备注  ：PA6-->HA  PA7-->HB PB0-->HC
***********************************************************************/
static void Timer3Initialize(void)
{
    //引脚复用功能配置
    GPIOA->MODER &= 0xffff0fff; //MODE6[1:0] = 0b00 MODE7[1:0] = 0b00
    GPIOA->MODER |= 0x0000a000; //MODE6[1:0] = 0b10 MODE7[1:0] = 0b10  引脚复用功能
    
    GPIOA->AFR[0] |= 0x11000000; //AFSEL7[3:0] = 0b0001 AFSEL6[3:0] = 0b0001  AF1 TIM3_CH1/2
    
    GPIOB->MODER &= 0xfffffffc; //MODE0[1:0] = 0b00
    GPIOB->MODER |= 0x00000002; //MODE0[1:0] = 0b10 MODE0[1:0] = 0b00
    
    GPIOB->AFR[0] |= 0x00000001; //AFSEL0[3:0] = 0b0001 AF1 TIM3_CH3
    
    //定时器3功能配置
    TIM3->CR1 = 0x0080; //ARPE = 1  CMS[1:0] = 0b00 边沿对齐计数  DIR = 0 上升计数
    TIM3->CR2 = 0x0080; //TI1S = 1 The TIMx_CH1, CH2 and CH3 pins are connected to the TI1 input (XOR combination)
    TIM3->CNT = 0; //清零计数器
    TIM3->PSC = 127; //128分频  64MHz  / 128 = 0.5MHz  2us 
    TIM3->ARR = 65535;  //131.072ms  支持最低转速 38RPM 
    
    TIM3->CCMR1 = 0x00000091; //IC1F[3:0] = 0b1001  64MHz  / 8 N = 8 1us IC1PSC[1:0] = 0b00 不分频 CC1S[1:0] = 0b01 IC1-->TI1
    TIM3->CCER = 0x000b; //CC1NP = 1  CC1P = 1  CC1E = 1 双沿捕捉
    TIM3->DIER |= 0x0002; //CC1E = 1 使能CH1捕捉中断
    TIM3->CR1 |= 0x0001; //CEN = 1 使能定时3
}
/***********************************************************************
* 函数名称  ：Timer1617Initialize
* 功能描述  ：定时器16 17模块初始化配置
* 形参      ：无
* 返回值    ：无
* 要点备注  ：PB8-->TIM16_CH1  PB9-->TIM17_CH1
***********************************************************************/
static void Timer1617Initialize(void)
{
    //引脚复用功能配置
    GPIOB->MODER &= 0xfff0ffff; //MODE8[1:0] = 0b00 MODE9[1:0] = 0b00
    GPIOB->MODER |= 0x000a0000; //MODE8[1:0] = 0b10 MODE9[1:0] = 0b10
    
    GPIOB->OSPEEDR |= 0x000a0000; //OSPEED9[1:0] = 0b10 OSPEED8[1:0] = 0b10 
    GPIOB->AFR[1] |= 0x00000022; //AFSEL9[3:0] = 0b0010 AFSEL8[3:0] = 0b0010  AF2 TIM16/17_CH1
    
    //定时器功能配置
    TIM16->CR1 = 0x0080; //ARPE = 1 默认是上升计数
    TIM16->CNT = 0; //清零计数器
    TIM16->PSC = 0; //没有分频
    TIM16->ARR = 1999;//1279; //64MHz / 50KHz = 1280 
    
    TIM16->CCMR1 = 0x00000068;//OC1CE = 0 OC1M[2:0] = 0b110 pwm mode 1 OC1PE = 1 OC1FE = 0 CC1S[1:0] = 0b00 OUT
    TIM16->CCER = 0x0001; //CC1E = 1 
    
    TIM16->CCR1 = 1000; //比较值设置  2.578V
    
    TIM16->BDTR = 0x00008000; //MOE = 1 主输出使能
    TIM16->AF1 = 0x00000000; //BKINE = 0
    TIM16->CR1 |= 0x0001; //CNE = 1 使能定时器16
    
    TIM17->CR1 = 0x0080; //ARPE = 1 默认是上升计数
    TIM17->CNT = 0; //清零计数器
    TIM17->PSC = 0; //没有分频
    TIM17->ARR = 1999;//1279; //64MHz / 50KHz = 1280 
    
    TIM17->CCMR1 = 0x00000068;//OC1CE = 0 OC1M[2:0] = 0b110 pwm mode 1 OC1PE = 1 OC1FE = 0 CC1S[1:0] = 0b00 OUT
    TIM17->CCER = 0x0001; //CC1E = 1 
    
    TIM17->CCR1 = 800; //比较值设置  2.0625V
    
    TIM17->BDTR = 0x00008000; //MOE = 1 主输出使能
    TIM17->AF1 = 0x00000000; //BKINE = 0
    TIM17->CR1 |= 0x0001; //CNE = 1 使能定时器17
}
/***********************************************************************
* 函数名称  ：UsartInitialize
* 功能描述  ：串口通讯模块初始化配置
* 形参      ：无
* 返回值    ：无
* 要点备注  ：PB6-->USART1_TX  PB7-->USART1_RX 
***********************************************************************/
static void UsartInitialize(void)
{
    //引脚复用功能配置
    GPIOB->MODER &= 0xffff0fff; //MODE7[1:0] = 0b00 MODE6[1:0] = 0b00
    GPIOB->MODER |= 0x0000a000; //MODE7[1:0] = 0b10 MODE6[1:0] = 0b10 复用模式
    
    GPIOB->OSPEEDR |= 0x00002000; //OSPEED6[1:0] = 0b10 High Speed
    
    GPIOB->AFR[0] |= 0; //AFSEL7[3:0] = 0b0000  AFSEL6[3:0] = 0b0000 AF0  USART1_TX/RX
    
    //串口模块功能配置
    USART1->CR1 = 0x0000000c;// TE = 1 RE = 1 使能发送 使能接收
    USART1->BRR = 6666; //64MHz/9600bps = 6666
    USART1->CR1 |= 0x00000001;// UE = 1 使能串口模块1
}
/***********************************************************************
* 函数名称  ：SpiInitialize
* 功能描述  ：SPI串口模块初始化配置
* 形参      ：无
* 返回值    ：无
* 要点备注  ：PB3-->SCK  PB5-->MOSI  PA15-->CS 
***********************************************************************/
static void SpiInitialize(void)
{
    //引脚复用功能配置
    GPIOA->MODER &= 0x3fffffff;// MODE15[1:0] =0b00
    GPIOA->MODER |= 0x40000000;// MODE15[1:0] =0b01
    
    GPIOA->OSPEEDR |= 0x80000000; //OSPEED15[1:0] = 0b10 High Speed
    GPIOA->BSRR |= 0x00008000;// BS15 = 1 GPIOA15 输出高电平
    
    GPIOB->MODER &= 0xfffff33f; //MODE5[1:0] =0b00  MODE3[1:0] =0b00
    GPIOB->MODER |= 0x00000880; //MODE5[1:0] =0b10  MODE3[1:0] =0b10 复用功能
    
    GPIOB->OSPEEDR |= 0x00000880; //OSPEED5[1:0] = 0b10 OSPEED3[1:0] = 0b10 High Speed
    GPIOB->AFR[0] |= 0; //AFSEL5[3:0] = 0b0000  AFSEL3[3:0] = 0b0000  AF0 SPI1_SCK/MOSI
    //SPI模块配置
    SPI1->CR1 = 0x030c;//0x0324; //SSM = 1 SSI = 1 BR[2:0] = 0b100 fpclk / 32 = 2MHz MSTR = 1主机
    SPI1->CR2 = 0x0700; //DS[3:0] 0b0111 8bits
    SPI1->CR1 |= 0x0040; //SPE = 1 使能SPI1
}
/***********************************************************************
* 函数名称  ：NVICInitialize
* 功能描述  ：NVIC中断控制器模块初始化配置
* 形参      ：无
* 返回值    ：无
* 要点备注  ：定时器3换相中断   DMA通道一传输完成中断
*           中断优先级可设置四种 0x00 0x40 0x80 0xc0 数字越高，优先级越低
* NVIC_IPRn : PRI_(4n+3)  PRI_(4n+2)  PRI_(4n+1)  PRI_(4n)
* DMA通道一中断向量号：9   定时器3捕捉中断向量号：16
***********************************************************************/
static void NVICInitialize(void)
{
    //优先级设置
    NVIC->IP[2] = 0x00004000;  //DMA通道一中断 优先级第2
    NVIC->IP[4] = 0x00000000; //定时器3中断优先级第1
    SCB->SHP[1] = 0x80000000; //滴答定时器中断优先级第3
    //中断使能
    //NVIC->ISER[0] = 0x00010200; //使能DMA通道一中断，使能定时器3中断
}
/***********************************************************************
* 函数名称  ：SysTickInitialize
* 功能描述  ：内核滴答定时器模块初始化配置
* 形参      ：无
* 返回值    ：无
* 要点备注  ：滴答定时器中断优先级，设置为最低
***********************************************************************/
static void SysTickInitialize(void)
{
    SysTick->RVR = 63999;//64MHz / 1KHz = 64000 - 1   1ms
    SysTick->CVR = 0;
    SysTick->CSR = 0x00000006;//CLKSOURCE = 1 Processor clock  TICKINT = 1使能中断
    //SysTick->CSR |= 0x00000001; //ENABLE = 1 使能滴答定时
}
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
