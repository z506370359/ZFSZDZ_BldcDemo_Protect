/***********************************************************************
* @file           UserParams.h
* @author         ZhaoYun(ZFSZDZ)
* @date           2024.10.12
* @Version        V1.0.0
* @description    用户设定参数头文件
***********************************************************************/
#define SYSCLOCKFREQ            (64000000) //单片机主频为64MHz
#define PWMFRQE                 (16000) //PWM频率
#define PWMARRVALUE             (SYSCLOCKFREQ / PWMFRQE) //PWM重载值

#define SPEEDMODE_POT           (0x00)
#define SPEEDMODE_USART         (0x01)
#define DEFAULT_SPEEDMODE       (SPEEDMODE_POT) //默认使用电位器启动机调速

#define RUNMODE_OPENLOOP        (0x00)
#define RUNMODE_CLOSELOOP       (0x01)
#define DEFAULT_RUNMODE         (RUNMODE_CLOSELOOP) //默认开环工作

#define DIR_CW                  (0x00)
#define DIR_CCW                 (0x01)
#define DEFAULT_DIR             (DIR_CW) //默认顺时针转动

#define START_MIN_DUTY          (100) //设置启动最小占空比10%
#define START_MAX_DUTY          (600) //设置启动最大占空比30% 最大允许到35%
#define START_STEP_DUTY         (100) //启动占空比步进值

#define RUN_MIN_DUTY            (100) //设置运行最小占空比5%
#define RUN_MAX_DUTY            (2000) //设置运行最大占空比100%

#define RUN_MIN_SPEED           (100) //设置运行最小转速
#define RUN_MAX_SPEED           (3000) //设置运行最大转速

#define MOTOR_POLEPAIRS         (2) //电机极对数
#define LOWPASS_FILTER_FACTOR   (20578) //Ts*2*pi*fc  = 0.001 * 2 * 3.14 * 50Hz * 65536 = 4116

#define VBUSFACTOR              (5280) //20K / 2K * 3.3V / 4096 = 0.008056640625 * 65536 * 10 = 5280
/*******************Copyright (c) ZFSZDZ ******END OF FILE************/
