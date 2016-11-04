#ifndef __KEY_H
#define	__KEY_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @�������±���
  * @KEY_ON 0, KEY_OFF 1
  */

#define KEY_ON	0
#define KEY_OFF	1

/** 
  * @�������������Ľӿ�
  */
#define Key_set		GPIO_Pin_12	//���ü�
#define Key_up 		GPIO_Pin_11	//��ֵ�Ӽ�
#define Key_down 	GPIO_Pin_10	//��ֵ����
#define Key_OnOf	GPIO_Pin_2	//�������ģʽ

//#define Key_down GPIO_Pin_10//��ֵ����
typedef struct
{
	char Model_flag;
	char Set_flag;
	char Up_flag;
	char Down_flag;
	char MenuSelect;//ѡ����ʾҳ��
	char Model_count;//ģʽ����
	char MenuSetFlag;//���빤�������־
}Key_Typedef;
/** 
  * @������ȴ����������¸��µ���ˮ�趨ֵ��ʵ��ֵ�ò�ֵ�߽�
  */

typedef struct
{
	char LowW;
	char HighW;
}DValue_Typedef;

/* private------- ------------------------------------------------------------*/
extern Key_Typedef Key;//���尴�����ݽṹ
extern DValue_Typedef DValue;
extern u16 TemperaSet[7];//�����¶�����
extern u16* ptemper;//����һЩ����



void Key_GPIO_Config(void);
u8 Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);//����ɨ��
void Set_Pro(void);//�������ô���
void Key_Process(void);//��������

#endif /* __LED_H */

