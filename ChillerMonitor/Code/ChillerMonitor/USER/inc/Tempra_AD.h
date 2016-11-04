#ifndef __TEMPERA_AD_H
#define	__TEMPERA_AD_H
/* Define to prevent recursive inclusion -------------------------------------------*/
#include "stm32f10x.h"

/* Exported constants ----------------------------------------------------------*/
#define ADC1_Count			100					//每一个通道的ADC采样次数
#define ADC_ChannelCount	6					//采样通道数
#define SmoothNum			20					//定义平滑值得次数

/* Exported macro -------------------------------------------------------------*/
#define ADC_CONTROL  	GPIOC					//默认输出口的控制口在PC口
#define ADC_ClK			RCC_APB2Periph_GPIOC	//默认ADC时钟
/** 
  * @采样引脚宏定义
  */
#define H_Tempra1 			GPIO_Pin_0  	//高温水温度1
#define H_Tempra2 			GPIO_Pin_1  	//高温水温度2
#define L_Tempra1 			GPIO_Pin_2  	//低温水温度1
#define L_Tempra2 			GPIO_Pin_3  	//低温水温度2
#define ENV_Tempra1 		GPIO_Pin_4 		//环境温度1
#define ENV_Tempra2 		GPIO_Pin_5		//环境温度2

#define ADC_Pin H_Tempra1|H_Tempra2|L_Tempra1|L_Tempra2|ENV_Tempra1|ENV_Tempra2//定义ADC通道引脚
/* Exported functions ------------------------------------------------------- ---*/ 
extern vu16 ADGetValue[ADC1_Count][ADC_ChannelCount];   		//AD采样值,ADC_ChannelCount个通道,每个通道采样ADC1_Count次
extern vu16 ADC_Filter[ADC_ChannelCount];   					//AD滤波后,计算一个通道ADC1_Count次数据的均值
extern u32 TMonitorDis[3];//用来显示存放检测所得的温度值，依次为低温,高温,环境温度

void ADC1_GPIO_Config(void);			//ADC用到的GPIO配置
void ADC1_Mode_Config(void);		//ADC工作模式配置
void DMA_Config(void);					//配置DMA
void ADC1_Init(void);						//ADC初始化
void ADC1_Tempera(void);				//ADC采样数据处理 
void AntifreezePro(void);				//防冻处理函数
void TemperaPro(void);					//ADC采样数据处理

#endif /*__TEMPERA_AD_H */

