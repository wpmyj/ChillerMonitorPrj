/**
  *****************************************************************************
  * @file  J12864.c
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	J12864Һ����������.
  *****************************************************************************
  */

#include "Display.h"
#include "J12864.h"
#include "key.h"
#include "Tempra_AD.h"
#include "IO_Process.h"
#include "STM32Flash.h"
#include "stm32f10x_it.h"


/* private------- ------------------------------------------------------------*/
char OldMenu=10;	//���岢��ʼ����һ����
char AntifreezeFlag;//������־λ,һ������Ч
char AlarmSaveflag=0;//�����쳣ʱ��ʾ���������������״̬


char *pnull[1]={"                "};//����һ���հ��ַ���ָ������

char *pctrl[3]={"�ֶ�","�Զ�","���"};//����һ���ַ���ָ������
char *pWarning[]={"��Դ�����쳣","ѹ��������","Һλ����","����ˮ�����쳣","����ˮ�����쳣",\
					"����ˮ�ù���","����ˮ�ù���","�����¶��쳣","ˮ�¸�������","ˮ�µ�������"};//�����쳣������Ϣ

/**
  * @brief	��ʾ����ṹ��,����ʾ������������ɽṹ��,�ܹ���ʾ6������,���Ƕ���ƽ�й�ϵ
  * @param None
  * @retval : None
  */

Menu_Typedef CH_SetupMenuItems[MenuNum] = 
{
	{"                ","�ſ�������豸  ","    Ϊ������    ","                ",NULL},
	{"���ڽ��з�������","��ȴ� 		 ","����ˮ: 	 2��","�����¶ȣ�	2��",NULL},
	{"    ������Ϊ:   ","  �ֶ�����ģʽ  ","  �ȴ���������  ","                ",NULL},
	{"    ������Ϊ:   ","  �Զ�����ģʽ  ","    30���      ","�Զ�������ˮ��  ",NULL},
	{"    ������Ϊ:   ","  ��ز���ģʽ  ","  �ȴ�����ź�  ","                ",NULL},
	{"�����¶�:   24��","����ˮ:     22��","����ˮ:     28��","�ֶ�ģʽ��������",EnterNormalDis},
	{"  ����ģʽ����  ","ѡ��:     ģʽ  ","��  ������ģʽ  ","                ",EnterModelSetting},       
	{"����ˮ  �¶�����","Ԥ���¶ȣ�    ��","��  �������¶�  ","                ",EnterTemperaSetting},
	{"����ˮ  �¶�����","Ԥ���¶ȣ�    ��","��  �������¶�  ","                ",EnterTemperaSetting},
	{"���ޱ����¶�����","Ԥ���¶ȣ�    ��","��  �������¶�  ","                ",EnterTemperaSetting},
	{"���ޱ����¶�����","Ԥ���¶ȣ�    ��","��  �������¶�  ","                ",EnterTemperaSetting},
	{"                ","�¶Ȳ���У׼����","����ˮϵ��   0��","                ",NULL},
	{"                ","�¶Ȳ���У׼����","����ˮϵ��   0��","                ",NULL},
	{"                ","�¶Ȳ���У׼����","����ϵ��     0��","                ",NULL}
};

/** 
  * @brief  ƽʱ��ʾ����
  * @param None
  * @retval : None		Dis_Menu(Key.MenuSelect);
  */
void EnterNormalDis(void)
{
	LCD_Setpos_DispString(4,1,pctrl[Key.Model_count-1]);
	LCD_Setpos_DispDecimal(2,6,TMonitorDis[0],1);//��ʾ����ˮ
	if(TMonitorDis[0]<10)//�����ʾʱ��С��10
	{
		LCD_WriteByte(0x20);//С������һλ��ʾ�հ�
		LCD_WriteByte(0x20);//С�����ڶ�λ��ʾ�հ�
	}
	else if(TMonitorDis[0]<100)//�����ʾʱ��С��100
		LCD_WriteByte(0x20);//С�����ڶ�λ��ʾ�հ�
	LCD_Setpos_DispDecimal(3,6,TMonitorDis[1],1);//��ʾ����ˮ
	if(TMonitorDis[1]<10)//�����ʾʱ��С��10
	{
		LCD_WriteByte(0x20);//С������һλ��ʾ�հ�
		LCD_WriteByte(0x20);//С�����ڶ�λ��ʾ�հ�
	}
	else if(TMonitorDis[1]<100)//�����ʾʱ��С��100
		LCD_WriteByte(0x20);//С�����ڶ�λ��ʾ�հ�
	LCD_Setpos_DispDecimal(1,6,TMonitorDis[2],1);//��ʾ�����¶�
	if(TMonitorDis[2]<10)//�����ʾʱ��С��10
	{
		LCD_WriteByte(0x20);//С������һλ��ʾ�հ�
		LCD_WriteByte(0x20);//С�����ڶ�λ��ʾ�հ�
	}
	else if(TMonitorDis[2]<100)//�����ʾʱ��С��100
		LCD_WriteByte(0x20);//С�����ڶ�λ��ʾ�հ�

}
/**
  * @brief  ����ģʽ��ʾ����
  * @param None
  * @retval : None
  */
void EnterModelSetting(void)
{
	LCD_Setpos_DispString(2,4,pctrl[Key.Model_count-1]);
	LCD_Setpos(3,2);
	LCD_WriteByte(0x1e);
	LCD_WriteByte(0x1f);
}

/**
  * @brief  �����¶����� ��ʾ����
  * @param None
  * @retval : None
  */
void EnterTemperaSetting(void)
{
	if(Key.MenuSelect>=7)
		LCD_Setpos_DispDecimal(3,6,TemperaSet[Key.MenuSelect-3],1);//��ʾ�¶�
	else
		LCD_Setpos_DispDecimal(2,6,TemperaSet[Key.MenuSelect-3],1);//��ʾ�¶�
	LCD_Setpos(3,2);
	LCD_WriteByte(0x1e);//�ϼ�ͼ��
	LCD_WriteByte(0x1f);//�¼�ͼ��
}
/**
  * @brief  ������ʾ����
  * @param None
  * @retval : None
  */
void AlarmDis(void){
	if(Alarm.flag>0)
	{
		LCD_Clear();										//����
		Chiller.AlarmOUT_ON=1;								//�б����ź����
		Chiller.AlarmOUT_OFF=0;								//�б����ź����
		if(Alarm.Powerphase==1)								//��Դ���������쳣
			LCD_Setpos_DispString(2,1,*pWarning);
		if(Alarm.Compressor==1)								//ѹ���������쳣
			LCD_Setpos_DispString(2,1,*(pWarning+1));
		if(Alarm.Level==1)									//Һλ�����쳣
			LCD_Setpos_DispString(2,1,*(pWarning+2));
		if(Alarm.HighWaterFlow==1)							//����ˮ�����쳣
			LCD_Setpos_DispString(2,1,*(pWarning+3));
		if(Alarm.LowWaterFlow==1)							//����ˮ�����쳣
			LCD_Setpos_DispString(2,1,*(pWarning+4));
		if(Alarm.Pumphigh==1)								//����ˮ�ù����쳣
			LCD_Setpos_DispString(2,1,*(pWarning+5));
		if(Alarm.Pumplow==1)								//����ˮ�ù����쳣
			LCD_Setpos_DispString(2,1,*(pWarning+6));
		/*
		*@��ʾ������������е�һЩ����
		**/
		if(Alarm.Environ==1)								//�����¶��쳣
			LCD_Setpos_DispString(2,1,*(pWarning+7));
		if(Alarm.UpperWaterSet==1)							//ˮ�¸��������쳣
			LCD_Setpos_DispString(2,1,*(pWarning+8));
		if(Alarm.LowerWaterSet==1)							//ˮ�µ��������쳣 
			LCD_Setpos_DispString(2,1,*(pWarning+9));
	}
}

/**
  * @brief  ��˾LOGO��ʾ����
  * @param None
  * @retval : None
  */

void DisLogo(void)
{
	Menu_Typedef *pMenu;//����һ���ṹ��ָ��
	pMenu=CH_SetupMenuItems;
	LCD_Setpos_DispString(1,1,pMenu->pText1);
	LCD_Setpos_DispString(2,1,pMenu->pText2);
	LCD_Setpos_DispString(3,1,pMenu->pText3);
	LCD_Setpos_DispString(4,1,pMenu->pText4);
}
/**
  * @brief  ����ģʽ�ȴ�����
  * @param None
  * @retval : None
  */

void WaitOperation(void)
{
	Menu_Typedef *pvMenu;//����һ���ṹ��ָ��
	static char workstart;//��̬����ֻ�ڱ������иı���ֵ
	workstart=0;
	if(Alarm.flag==0)//��û�о��������ģʽ��ʾ����һ��ʱ��
	{
		if(Config_Data[0]==1)//�ֶ�����ģʽ����
		{
			pvMenu=(CH_SetupMenuItems+2);
			workstart=1;
			if((Key.MenuSetFlag==2)||(Key.MenuSetFlag==0))
			{
				LCD_Setpos_DispString(1,1,pvMenu->pText1);
				LCD_Setpos_DispString(2,1,pvMenu->pText2);
				LCD_Setpos_DispString(3,1,pvMenu->pText3);
				LCD_Setpos_DispString(4,1,pvMenu->pText4);
			}
			while((Key.MenuSetFlag==2)||(Key.MenuSetFlag==0))//�ȴ���������
			{
				Key_Process();
				PerifStop();//ֹͣ�������蹤��
				/*
				*@����Ϊ��λ��ʾ������ֵ
				*/
				Key.Model_flag=0;
				Key.Set_flag=0;
				Key.Up_flag=0;
				Key.Down_flag=0;
				Key.MenuSelect=0;
				OldMenu=10;//���ֵĽ���ָ�����ʼ��״̬
			}
		}	
		else if(Config_Data[0]==2)//�Զ�����ģʽ����
		{
			pvMenu=(CH_SetupMenuItems+3);
			workstart=1;
			if((Key.MenuSetFlag==2)||(Key.MenuSetFlag==0))
			{
				LCD_Setpos_DispString(1,1,pvMenu->pText1);
				LCD_Setpos_DispString(2,1,pvMenu->pText2);
				LCD_Setpos_DispString(3,1,pvMenu->pText3);
				LCD_Setpos_DispString(4,1,pvMenu->pText4);
				AutoTime=0;
				while(AutoTime<100)//�ȴ�����ʱ��
				{
					LCD_Setpos_Dispnum(3,3,(10-(AutoTime/10)));//��ʾ�ȴ�ʱ��
					if(AutoTime>10)//�����ʾʱ��С��10S
						LCD_WriteByte(0x20);//��λ��ʾ�հ�
					PerifStop();//ֹͣ�������蹤��
					/*
					*@����Ϊ��λ��ʾ������ֵ
					*/
					Key.Model_flag=0;
					Key.Set_flag=0;
					Key.Up_flag=0;
					Key.Down_flag=0;
					Key.MenuSelect=0;
					OldMenu=10;//���ֵĽ���ָ�����ʼ��״̬
				}
			}
		}	
		else if(Config_Data[0]==3)//��ز���ģʽ����
		{
			pvMenu=(CH_SetupMenuItems+4);
			workstart=1;
			if((Key.MenuSetFlag==2)||(Key.MenuSetFlag==0))
			{
				LCD_Setpos_DispString(1,1,pvMenu->pText1);
				LCD_Setpos_DispString(2,1,pvMenu->pText2);
				LCD_Setpos_DispString(3,1,pvMenu->pText3);
				LCD_Setpos_DispString(4,1,pvMenu->pText4);
				PerifStop();//ֹͣ�������蹤��
				/*
				*@����Ϊ��λ��ʾ������ֵ
				*/
				Key.Model_flag=0;
				Key.Set_flag=0;
				Key.Up_flag=0;
				Key.Down_flag=0;
				Key.MenuSelect=0;
				OldMenu=10;//���ֵĽ���ָ�����ʼ��״̬
				while(GPIO_ReadInputDataBit(GPIOA,ExCtr_IN) == Bit_SET)//�ȴ�����ź�
				{
				}
			}
		}
		if(workstart==1)//�������������������־�����ʼ���������
		{
			workstart=0;
			Key.MenuSelect=1;//ʱ�䵽�˽���������ʾ����
			Key.MenuSetFlag=1;//����ģʽ
			/*
			*���˰���֮����ˮ�ã�����������ر�
			*/
			Chiller.Pump_OFF=0;
			Chiller.Pump_ON=1;//����ˮ��
			Chiller.Valve_ON=0;
			Chiller.Valve_OFF=1;
			Chiller.Compressor_ON=0;
			Chiller.Compressor_OFF=1;
			Chiller.ElBow_ON=0;
			Chiller.ElBow_OFF=1;
			Chiller.AlarmOUT_ON=0;
			Chiller.AlarmOUT_OFF=1;
			GetSysPrama();	//�������ֵ׼����������ģʽ
		}
	}
}
/**
  * @brief  �����������
  * @param None
  * @retval : None
  */

void AntifreezeDis(void)
{
	Menu_Typedef *pMenu;//����һ���ṹ��ָ��
	pMenu=(CH_SetupMenuItems+1);
	if(AntifreezeFlag==0)
	{
		AntifreezeFlag=1;
		LCD_Setpos_DispString(1,1,pMenu->pText1);
		LCD_Setpos_DispString(2,1,pMenu->pText2);
		LCD_Setpos_DispString(3,1,pMenu->pText3);
		LCD_Setpos_DispString(4,1,pMenu->pText4);
	}
	LCD_Setpos_DispDecimal(3,6,TMonitorDis[1],1);
	if(TMonitorDis[1]<10)//�����ʾʱ��С��10
	{
		LCD_WriteByte(0x20);//С�����ڶ�λ��ʾ�հ�
		LCD_WriteByte(0x20);//С�����ڶ�λ��ʾ�հ�
	}
	else if(TMonitorDis[1]<100)//�����ʾʱ��С��100
		LCD_WriteByte(0x20);//С�����ڶ�λ��ʾ�հ�
	LCD_Setpos_DispDecimal(3,6,TMonitorDis[2],1);
	if(TMonitorDis[2]<10)//�����ʾʱ��С��10
	{
		LCD_WriteByte(0x20);//С�����ڶ�λ��ʾ�հ�
		LCD_WriteByte(0x20);//С�����ڶ�λ��ʾ�հ�
	}
	else if(TMonitorDis[2]<100)//�����ʾʱ��С��100
		LCD_WriteByte(0x20);//С�����ڶ�λ��ʾ�հ�
}

/**
  * @brief  ��ʾ����
  * @param None
  * @retval : None
  */

void Dis_Menu(char MenuDis)
{
	Menu_Typedef *pCurrentMenu;//����һ���ṹ��ָ��
	if(Alarm.flag==0)//���ж��Ƿ񱨾��������Ľ������ȴ���
	{
		if(AlarmSaveflag==1)
		{
			AlarmSaveflag=0;
			Chiller=SaveChillerNormalPara;//��ȡ�쳣ǰһ�̱��汣����������״̬
			Chiller.AlarmOUT_ON=0;//�ޱ����ź����
			Chiller.AlarmOUT_OFF=1;//�ޱ����ź����
		} 
		if(Key.MenuSetFlag==1)
		{
			if(MenuDis!=OldMenu)//��ͬһ�������ⲿ�����ݲ���ˢ��
			{
				pCurrentMenu=(CH_SetupMenuItems+MenuDis+4);
				OldMenu=MenuDis;
				LCD_Setpos_DispString(1,1,pCurrentMenu->pText1);
				LCD_Setpos_DispString(2,1,pCurrentMenu->pText2);
				LCD_Setpos_DispString(3,1,pCurrentMenu->pText3);
				LCD_Setpos_DispString(4,1,pCurrentMenu->pText4);
			}
			(*pCurrentMenu->pMenuFunc)();//���Ĳ���ָ��
		}
		if((Key.MenuSetFlag==2)||(Key.MenuSetFlag==0))//�رչ���ģʽ
		{
			WaitOperation();//�ȴ���������
		}
	}
	if(Alarm.flag>0)//���ж��Ƿ񱨾��������Ľ������ȴ���
	{
		AlarmDis();//��ʾ��������
		AlarmSaveflag=1;
	}

}


