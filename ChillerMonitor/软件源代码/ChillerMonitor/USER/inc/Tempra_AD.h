#ifndef __TEMPERA_AD_H
#define	__TEMPERA_AD_H
/* Define to prevent recursive inclusion -------------------------------------------*/
#include "stm32f10x.h"

/* Exported constants ----------------------------------------------------------*/
#define ADC1_Count			100					//ÿһ��ͨ����ADC��������
#define ADC_ChannelCount	6					//����ͨ����
#define SmoothNum			20					//����ƽ��ֵ�ô���

/* Exported macro -------------------------------------------------------------*/
#define ADC_CONTROL  	GPIOC					//Ĭ������ڵĿ��ƿ���PC��
#define ADC_ClK			RCC_APB2Periph_GPIOC	//Ĭ��ADCʱ��
/** 
  * @�������ź궨��
  */
#define H_Tempra1 			GPIO_Pin_0  	//����ˮ�¶�1
#define H_Tempra2 			GPIO_Pin_1  	//����ˮ�¶�2
#define L_Tempra1 			GPIO_Pin_2  	//����ˮ�¶�1
#define L_Tempra2 			GPIO_Pin_3  	//����ˮ�¶�2
#define ENV_Tempra1 		GPIO_Pin_4 		//�����¶�1
#define ENV_Tempra2 		GPIO_Pin_5		//�����¶�2

#define ADC_Pin H_Tempra1|H_Tempra2|L_Tempra1|L_Tempra2|ENV_Tempra1|ENV_Tempra2//����ADCͨ������
/* Exported functions ------------------------------------------------------- ---*/ 
extern vu16 ADGetValue[ADC1_Count][ADC_ChannelCount];   		//AD����ֵ,ADC_ChannelCount��ͨ��,ÿ��ͨ������ADC1_Count��
extern vu16 ADC_Filter[ADC_ChannelCount];   					//AD�˲���,����һ��ͨ��ADC1_Count�����ݵľ�ֵ
extern u32 TMonitorDis[3];//������ʾ��ż�����õ��¶�ֵ������Ϊ����,����,�����¶�

void ADC1_GPIO_Config(void);			//ADC�õ���GPIO����
void ADC1_Mode_Config(void);		//ADC����ģʽ����
void DMA_Config(void);					//����DMA
void ADC1_Init(void);						//ADC��ʼ��
void ADC1_Tempera(void);				//ADC�������ݴ��� 
void AntifreezePro(void);				//����������
void TemperaPro(void);					//ADC�������ݴ���

#endif /*__TEMPERA_AD_H */

