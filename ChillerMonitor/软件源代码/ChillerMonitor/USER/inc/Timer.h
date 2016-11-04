#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"
/** 
  * @72Mhzʱ��Դ����ʱ��2,100ms��ʱ���ж�һ��,��Ƶϵ���Լ��Զ����������Ϊ65536
  */
#define TIM2_ARR			( 7200 - 1 )//�Զ�����Ԥ��ֵ
#define TIM2_PSC			( ( ( 72000000 / 10 ) / ( TIM2_ARR + 1 ) ) - 1 )//��Ƶϵ��1000

#define START_TIME RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);TIM_Cmd(TIM2, ENABLE)
#define STOP_TIME  TIM_Cmd(TIM2, DISABLE);RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE)

extern void NVIC_Configuration(void);//�ж�����
extern void TIM2_Configuration(void);
extern void TIM1_Mode_Config(void);

#endif	/* TIME_TEST_H */
