#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"
/** 
  * @72Mhz时钟源给定时器2,100ms定时器中断一次,分频系数以及自动加载数最大为65536
  */
#define TIM2_ARR			( 7200 - 1 )//自动加载预设值
#define TIM2_PSC			( ( ( 72000000 / 10 ) / ( TIM2_ARR + 1 ) ) - 1 )//分频系数1000

#define START_TIME RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);TIM_Cmd(TIM2, ENABLE)
#define STOP_TIME  TIM_Cmd(TIM2, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE)

extern void NVIC_Configuration(void);//中断配置
extern void TIM2_Configuration(void);
extern void TIM1_Mode_Config(void);

#endif	/* TIME_TEST_H */
