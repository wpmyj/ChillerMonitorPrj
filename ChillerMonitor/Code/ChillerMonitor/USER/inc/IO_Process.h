#ifndef __IO_Process_H
#define	__IO_Process_H
/* Define to prevent recursive inclusion -------------------------------------------*/
#include "stm32f10x.h"
/* Exported constants ----------------------------------------------------------*/
/** 
  * @���뱨��״̬��ʶ
  */
typedef struct
{
	char Powerphase;//�����쳣��ʶλ
	char Compressor;//ѹ�����쳣��ʶλ
	char Level;//Һλ�쳣��ʶλ
	char HighWaterFlow;//����ˮˮ�����쳣��ʶλ
	char LowWaterFlow;//����ˮˮ�����쳣��ʶλ
	char Pumphigh;//����ˮˮ���쳣��ʶλ
	char Pumplow;//����ˮˮ���쳣��ʶλ
	char flag;//�쳣����
	char Recover;//�ָ�������ʾ
	
	/*��MCU�������¶����ݱ�־λ*/
	char LowerWaterSet;//ˮ�����ޱ�����־
	char UpperWaterSet;//ˮ�����ޱ�����־
	char Environ;//�����¶��쳣��־
}Alarm_Typedef;

/** 
  * @�������ʹ�ܱ�־
  */

typedef struct
{
	char Pump_ON; 				//ˮ��ʹ�ܽ�
	char Valve_ON; 				//ˮ��ʹ�ܽ�
	char Compressor_ON; 		//ѹ����ʹ�ܽ�
	char ElBow_ON;				//�������ʹ�ܽ�
	char AlarmOUT_ON; 			//�����ź�ʹ�ܽ�
	char Opticalcoupler1_ON; 	//�������2ʹ�ܽ�
	char Opticalcoupler2_ON; 	//�������2ʹ�ܽ�

	char Pump_OFF; 				//ˮ�ý��ܽ�
	char Valve_OFF; 			//ˮ�����ܽ�
	char Compressor_OFF; 		//ѹ�������ܽ�
	char ElBow_OFF;				//����������ܽ�
	char AlarmOUT_OFF; 			//�����źŽ��ܽ�
	char Opticalcoupler1_OFF; 	//�������2���ܽ�
	char Opticalcoupler2_OFF; 	//�������2���ܽ�
}Perif_Typedef;


/* Exported macro -------------------------------------------------------------*/
#define OutPinCtrl  GPIOA						//Ĭ������ڵĿ��ƿ���PA��
#define OutPinCtrl_Clk  RCC_APB2Periph_GPIOA	//Ĭ��OUT���ƿ�ʱ��

#define InPinCtrl_L  GPIOC						//Ĭ�������ǰ�벿��PC��
#define InPinCtrl_H  GPIOA						//Ĭ������ں�벿��PA��
#define InPinCtrl_Clk  RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA//Ĭ��IN���ƿ�ʱ��

/** 
  * @���Ŷ����������
  */
#define Pump_OUT 				GPIO_Pin_4  	//ˮ�������
#define Valve_OUT 				GPIO_Pin_5  	//ˮ�������
#define Compressor_OUT 			GPIO_Pin_6  	//ѹ���������
#define ElBow_OUT 				GPIO_Pin_7 		//������������
#define Alarm_OUT 				GPIO_Pin_8		//�����ź������
#define Opticalcoupler1_OUT 	GPIO_Pin_9		//�������1�����
#define Opticalcoupler2_OUT 	GPIO_Pin_10		//�������2�����


#define OutPin Pump_OUT|Valve_OUT|Compressor_OUT|ElBow_OUT|Alarm_OUT|Opticalcoupler1_OUT|Opticalcoupler2_OUT//�������

/** 
  * @���벿��
  */
/*GPIOC*/
#define Powerphase_IN 	GPIO_Pin_6		//��Դ��������
#define Compressor_IN 	GPIO_Pin_7  	//ѹ��������
#define Level_IN 		GPIO_Pin_8		//Һλ����
#define HighWater_IN 	GPIO_Pin_9		//����ˮ��������
/*GPIOA*/
#define LowWater_IN 	GPIO_Pin_0		//����ˮ��������
#define Pumphigh_IN 	GPIO_Pin_1  	//����ˮˮ������
#define Pumplow_IN 		GPIO_Pin_2		//����ˮˮ������
/*�������*/
#define ExCtr_IN 		GPIO_Pin_3		//�������PIN_IN14

#define InPin_L Powerphase_IN|Compressor_IN|Level_IN|HighWater_IN	//����ǰ�벿������ʹ��
#define InPin_H	LowWater_IN|Pumphigh_IN|Pumplow_IN	//�����벿������ʹ��

/* private------- ------------------------------------------------------------*/
extern Alarm_Typedef Alarm;		//������־
extern Perif_Typedef Chiller;	//��ȴװ��
extern Perif_Typedef SaveChillerNormalPara;//������ȴװ���쳣ǰ����

/* Exported functions ------------------------------------------------------- ---*/
void IOputConfig(void);//�����������
void PerifStop(void);//ֹͣ���蹤��
void PerifStart(void);//�������蹤��
void InputMonitor(void);//������
void OutPro(void);//��������

#endif

