/**
  *****************************************************************************
  * @file  Time_test.c
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	��ʱ�����ú���	  .
  *****************************************************************************
  */ 
/* Includes ------------------------------------------------------------------*/

#include "Timer.h"
/**
  * @brief	 NVIC�ж����ȼ����� 
  * @param None
  * @retval : None
  */
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef TIM2_NVIC_InitStructure,DMA1_NVIC_InitStructure; 
	/*
	*@TIM2�ж�����
	*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
    TIM2_NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	  
    TIM2_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    TIM2_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    TIM2_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&TIM2_NVIC_InitStructure);
	/*
	*@DMA1�ж�����
	*/
	DMA1_NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; 
    DMA1_NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    DMA1_NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
    DMA1_NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&DMA1_NVIC_InitStructure);          // Enable the DMA Interrupt
}
	

/**
  * @brief	TIM2�ж�����Ϊ100ms
  * @param None
  * @retval : None
  */
void TIM2_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
    TIM_DeInit(TIM2);
    TIM_TimeBaseStructure.TIM_Period=TIM2_ARR;	//�Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) 
    /* �ۼ� TIM_Period��Ƶ�ʺ����һ�����»����ж� */
    TIM_TimeBaseStructure.TIM_Prescaler= TIM2_PSC;	//ʱ��Ԥ��Ƶ��1000   
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ 
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);	// �������жϱ�־ 
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);	// ����ʱ��    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);	//�ȹرյȴ�ʹ��  
}
/**
  * @brief	TIM1�ж�����Ϊ10ms
  * @param ����TIM1�����PWM�źŵ�ģʽ�������ڡ����ԡ�ռ�ձ�
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
  TIM_TimeBaseStructure.TIM_Period = 10000;//72��Ƶ��ÿ��0.5S����һ���жϴ���ADC
  TIM_TimeBaseStructure.TIM_Prescaler = (3600-1);
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* PWM2 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;			//PWM2����ģʽ
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = ccr1;						//ռ�ձ�50%
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;//������ͨ��
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM1, ENABLE);
  /* TIM1 enable counter */
  TIM_Cmd(TIM1, ENABLE);
  TIM_CtrlPWMOutputs(TIM1, ENABLE);	//���ʹ��
}
