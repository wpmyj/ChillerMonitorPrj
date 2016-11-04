/**
  *****************************************************************************
  * @file  main.c
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	������.
  *****************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "led.h"
#include "key.h"  
#include "J12864.h"
#include "Delay_Timer.h"
#include "Display.h"
#include "Timer.h"
#include "stm32f10x_it.h"
#include "IO_Process.h"
#include "Tempra_AD.h"
#include "STM32Flash.h"


/**
  * @brief  ������
  * @param None
  * @retval : 0
  */

int main(void)
{ 
	/*��ʼ������*/
	SystemInit();					// ����ϵͳʱ��Ϊ72M 
	DelayInit(72000000);
	NVIC_Configuration();			//TIM2 ��ʱ���� 
	TIM2_Configuration(); 	
	START_TIME;	 					//TIM2 ��ʱ��ʼ��ʱ	
	Key_GPIO_Config();				//�����˿ڳ�ʼ��
	IOputConfig();					//��ʼ���������IO
	PerifStop();					//��ֹͣ���蹤��
	LCD_PORT_Init();
	LCD_Init();						//��ʼ��Һ��
	ADC1_Init();					//��ʼ��ADC
	PerifStop();					//��ʼ���������״̬�ر�
	LogoTime=0;
	while(LogoTime<LogoFreshTime)
		DisLogo();					//��ʾ��˾LOGO
	//StoreSysPrama();
	GetSysPrama();					//���ϵͳ����
	AntifreezePro();				//��������
	while(1){
		Key_Process();				//��������
		if(MenuTime>=MenuFreshTime)	//1����ˢ��һ��
		{
			ADC1_Tempera();			//ADC����
			TemperaPro();			//�¶ȴ���
			Dis_Menu(Key.MenuSelect);	//��ʾ����
			MenuTime=0;				//��ռ�����
		}
		OutPro();					//������ƴ���
	}
	return 0;
}

