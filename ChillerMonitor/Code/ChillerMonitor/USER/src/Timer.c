/**
  *****************************************************************************
  * @file  Time_test.c
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	定时器配置函数	  .
  *****************************************************************************
  */ 
/* Includes ------------------------------------------------------------------*/

#include "Timer.h"
/**
  * @brief	 NVIC中断优先级配置 
  * @param None
  * @retval : None
  */
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef TIM2_NVIC_InitStructure,DMA1_NVIC_InitStructure; 
	/*
	*@TIM2中断配置
	*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    TIM2_NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    TIM2_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    TIM2_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    TIM2_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&TIM2_NVIC_InitStructure);
	/*
	*@DMA1中断配置
	*/
	DMA1_NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; 
    DMA1_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    DMA1_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
    DMA1_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&DMA1_NVIC_InitStructure);          // Enable the DMA Interrupt
}
	

/**
  * @brief	TIM2中断周期为100ms
  * @param None
  * @retval : None
  */
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=TIM2_ARR;	//自动重装载寄存器周期的值(计数值) 
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= TIM2_PSC;	//时钟预分频数1000   
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);	// 清除溢出中断标志 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);	// 开启时钟    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);	//先关闭等待使用  
}
/**
  * @brief	TIM1中断周期为10ms
  * @param 配置TIM1输出的PWM信号的模式，如周期、极性、占空比
  * @retval : None
  */
void TIM1_Mode_Config(void)
{
	vu16 ccr1=5000;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
/* -----------------------------------------------------------------------
    TIM1 Configuration: generate 2 PWM signals with 2 different duty cycles:
    TIM1CLK = 72 MHz, Prescaler = 0x0, TIM1 counter clock = 36 MHz
    TIM3 ARR Register = 50000 => TIM1 Frequency = TIM1 counter clock/(ARR + 1)
    TIM3 Frequency = 1440Hz.
    TIM1 Channel1 duty cycle = (TIM1_CCR1/ TIM1_ARR)* 100 
    TIM1 Channel2 duty cycle = (TIM1_CCR2/ TIM1_ARR)* 100 
    
  ----------------------------------------------------------------------- */
 /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 10000;//72主频，每到0.5S产生一次中断触发ADC
  TIM_TimeBaseStructure.TIM_Prescaler = (3600-1);
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* PWM2 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			//PWM2工作模式
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ccr1;						//占空比50%
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;//开反向通道
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM1, ENABLE);
  /* TIM1 enable counter */
  TIM_Cmd(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);	//输出使能
}
