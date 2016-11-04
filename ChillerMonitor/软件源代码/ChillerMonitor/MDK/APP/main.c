/**
  *****************************************************************************
  * @file  main.c
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	主函数.
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
  * @brief  主函数
  * @param None
  * @retval : 0
  */

int main(void)
{ 
	/*初始化外设*/
	SystemInit();					// 配置系统时钟为72M 
	DelayInit(72000000);
	NVIC_Configuration();			//TIM2 定时配置 
	TIM2_Configuration(); 	
	START_TIME;	 					//TIM2 定时开始计时	
	Key_GPIO_Config();				//按键端口初始化
	IOputConfig();					//初始化输入输出IO
	PerifStop();					//现停止外设工作
	LCD_PORT_Init();
	LCD_Init();						//初始化液晶
	ADC1_Init();					//初始化ADC
	PerifStop();					//初始化外设输出状态关闭
	LogoTime=0;
	while(LogoTime<LogoFreshTime)
		DisLogo();					//显示公司LOGO
	//StoreSysPrama();
	GetSysPrama();					//获得系统参数
	AntifreezePro();				//防冻处理
	while(1){
		Key_Process();				//按键处理
		if(MenuTime>=MenuFreshTime)	//1秒钟刷新一次
		{
			ADC1_Tempera();			//ADC采样
			TemperaPro();			//温度处理
			Dis_Menu(Key.MenuSelect);	//显示更新
			MenuTime=0;				//清空计数器
		}
		OutPro();					//输出控制处理
	}
	return 0;
}

