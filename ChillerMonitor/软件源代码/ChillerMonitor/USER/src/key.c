/**
  *****************************************************************************
  * @file  key.c
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	����Ӧ�ú�����        .
  *****************************************************************************
  */ 
/* Includes ------------------------------------------------------------------*/

#include "key.h" 
#include "Display.h"
#include "Delay_Timer.h"
#include "J12864.h"
#include "stm32f10x_it.h"
#include "IO_Process.h"
#include "STM32Flash.h"


/* private------- ------------------------------------------------------------*/
Key_Typedef Key={0,0,0,0,0,1,0};//��ʼ��������־
DValue_Typedef DValue={20,20};//�趨��ʼ��ֵ
u16 TemperaSet[7]={220,280,100,450,0,0,0};//��ʼ���¶�����,����Ϊ����,����,����,���ޱ�����Ԥ��ֵ
u16 *ptemper=&TemperaSet[0];//����һָ��ָ���¶�����
static char Set3SecFlag=0;//���ü�����3���־λ
/**
  * @brief	���ð����õ���I/O�� 
  * @param None
  * @retval : None
  */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure,OnOf;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //���������˿ڵ�ʱ��
	GPIO_InitStructure.GPIO_Pin = (Key_set|Key_up|Key_down); 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //�˿�����Ϊ��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	//��ʼ���˿�

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE); //���������˿ڵ�ʱ��
	OnOf.GPIO_Pin = Key_OnOf; 
	OnOf.GPIO_Speed = GPIO_Speed_10MHz; 
	OnOf.GPIO_Mode = GPIO_Mode_IPU; //�˿�����Ϊ��������
	GPIO_Init(GPIOD, &OnOf);	//��ʼ���˿�
}
 /**
   * @brief  ����Ƿ��а������£����ж����ĸ�����
   * @param None
   * @retval : None
   */
u8 Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin)
{			
	uint16_t Key_judge;
	/*����Ƿ��а������� */
	if((GPIO_ReadInputData(GPIOx)&(GPIO_Pin))!=(GPIO_Pin))
	{
		/*��ʱ����*/
		Delayms(20);
		if((GPIO_ReadInputData(GPIOx)&(GPIO_Pin))!=(GPIO_Pin))
		{
			Key_judge=(GPIO_ReadInputData(GPIOx)&(GPIO_Pin));
			switch(Key_judge)
			{
				/*�ж��Ƿ�Ϊ���ü�*/
				case (Key_up|Key_down)://�������UP����DOWN��ֻ�͵�ֵ��ô˵��set��������
					if(GPIO_ReadInputDataBit(GPIOx,Key_set) == KEY_ON)
					{
						Key.Set_flag=1;
						LongKeyTime=0;			//�������
						Set3SecFlag=1;
					}
					Key.Up_flag=0;			//�ڰ������ü���ʱ��������¼���־
					Key.Down_flag=0;		//�ڰ������ü���ʱ��������¼���־
					break;
				/*�ж��Ƿ�Ϊ�Ӽ�*/
				case (Key_set|Key_down):	//�������SET����DOWN��ֻ�͵�ֵ��ô˵��UP��������
						Key.Up_flag=1;
						Key.Set_flag=0;		//������ü�
						Set3SecFlag=0;//����ñ�־λ
					break;
				/*�ж��Ƿ�Ϊ����*/
				case (Key_set|Key_up):		//�������UP����SET��ֻ�͵�ֵ��ô˵��DOWN��������
						Key.Down_flag=1;
						Key.Set_flag=0;		//������ü�
						Set3SecFlag=0;//����ñ�־λ
					break;
				default:
					break;
			}
			/*�ȴ������ͷ� */
			while((GPIO_ReadInputData(GPIOx)&(GPIO_Pin))!=(GPIO_Pin))
			{

				if(Set3SecFlag==1)//�ж�SET���Ƿ�Ϊ����
				{
					if(LongKeyTime>=20)//�ж�SET���Ƿ�Ϊ����
					{
						Set3SecFlag=0;//����ñ�־λ
						Key.Model_flag++;
						if(Key.Model_flag>=2)
						{
							Key.Model_flag=0;
							Key.MenuSelect=1;
						}
						LongKeyTime=0;			//�������
					}
				}
				Set_Pro();
				Dis_Menu(Key.MenuSelect);	//��ʾ����
			}
			return 	KEY_ON; 				//�а�������
		}
		else
			return KEY_OFF;					//�ް�������
	}
	else
		return KEY_OFF;						//�ް�������
}
 /**
   * @brief  ��ʾ���ô���
   * @param None
   * @retval : None
   */
 void Set_Pro(void)
 {
	static char i=0;
	if(Key.Model_flag==1)//�Ƿ��������ģʽ
	{					
		if(Key.Set_flag==1)
		{
			Key.Set_flag=0;
			Key.MenuSelect++;
			if(Key.MenuSelect>=10)//�������ڵĽ�����9��
				Key.MenuSelect=2;
		}
		if((Key.Up_flag==1)||(Key.Down_flag==1))//�����Ӽ���������
		{
			if(Key.MenuSelect==2)
			{
				Key.Model_count += (Key.Up_flag-Key.Down_flag);
				if(Key.Model_count>=4)
					Key.Model_count=1;
				else if(Key.Model_count<=0) 
					Key.Model_count=3;
			}
			else if(Key.MenuSelect>2)
			{
				i=Key.MenuSelect-3;
				*(ptemper+i) += (Key.Up_flag-Key.Down_flag);
				/*
				*@ȷ�������¶ȵ��趨��Χ
				**/
				if(*ptemper>=350)
					*ptemper=350;
				if(*ptemper<=150)
					*ptemper=150;
				
				if(*(ptemper+1)>=350)
					*(ptemper+1)=350;
				if(*(ptemper+1)<=150)
					*(ptemper+1)=150;
				
				if(*(ptemper+2)>=140)
					*(ptemper+2)=140;
				if(*(ptemper+2)<=0)
					*(ptemper+2)=0;

				if(*(ptemper+3)>=500)
					*(ptemper+3)=500;
				if(*(ptemper+3)<=410)
					*(ptemper+3)=410;
			}
			Key.Up_flag=0;
			Key.Down_flag=0;
			StoreSysPrama();	//��������ֵ
		}
	}
 }
 /**
   * @brief  �������봦��
   * @param None
   * @retval : None
   */
 void Key_Process(void){
 	if(GPIO_ReadInputDataBit(GPIOD,Key_OnOf) == KEY_ON)
	{
		Delayms(20);
		if(GPIO_ReadInputDataBit(GPIOD,Key_OnOf) == KEY_ON)
		{
			Key.MenuSetFlag++;
			if(Key.MenuSetFlag>=3)//Key.MenuSetFlagֻ������ֵ1,2���ϳ�ʼ����0ֵ
				Key.MenuSetFlag=1;
		}
		while(GPIO_ReadInputDataBit(GPIOD,Key_OnOf) == KEY_ON);//�ȴ������ͷ�
	}
	 if(Key.MenuSetFlag==1)
	 {
		if( Key_Scan(GPIOC,(Key_set|Key_up |Key_down)) == KEY_ON  )//�ж��Ƿ��м�����
		{
			Set_Pro();
		}
	 }
}
 
