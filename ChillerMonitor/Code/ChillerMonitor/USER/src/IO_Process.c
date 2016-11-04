#include "IO_Process.h"
#include "Display.h"
#include "Tempra_AD.h"
#include "Delay_Timer.h"

	
/* private------- ------------------------------------------------------------*/
Alarm_Typedef Alarm={0,0,0,0,0,0,0,0,1,0,0,0};//������ʶλ
Perif_Typedef Chiller={0,0,0,0,0,0,0,0,0,0,0,0,0,0};//��ˮ�����蹤��ʹ�ܱ�ʶ
Perif_Typedef SaveChillerNormalPara={0,0,0,0,0,0,0,0,0,0,0,0,0,0};//������ˮ�������쳣ǰ����ʹ�ܱ�ʶ

/**
  * @brief	��ͨ���������������
  * @param None
  * @retval : None
  */
/***************  ������������õ���I/O�� *******************/
void IOputConfig(void)	
{
	/*���������*/
	GPIO_InitTypeDef InPort_L,InPort_H,OutPort;
	RCC_APB2PeriphClockCmd(InPinCtrl_Clk ,ENABLE); //ʱ��ʹ��
	InPort_L.GPIO_Pin  = InPin_L;//ѡ�������
	InPort_L.GPIO_Speed= GPIO_Speed_50MHz;
	InPort_L.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(InPinCtrl_L,&InPort_L);
	
	InPort_H.GPIO_Pin  = InPin_H|ExCtr_IN;//ѡ�������
	InPort_H.GPIO_Speed= GPIO_Speed_50MHz;
	InPort_H.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_Init(InPinCtrl_H,&InPort_H);
	
	/* ���������*/
	RCC_APB2PeriphClockCmd(OutPinCtrl_Clk ,ENABLE); //ʱ��ʹ��
	OutPort.GPIO_Pin  = OutPin;//ѡ�������
	OutPort.GPIO_Speed= GPIO_Speed_50MHz;
	//	LCD_port.GPIO_Mode = GPIO_Mode_Out_OD;//OD��·���������������赽5V
	OutPort.GPIO_Mode = GPIO_Mode_Out_PP;//PP�������
	GPIO_Init(OutPinCtrl,&OutPort);
}
/**
  * @brief	ֹͣ���蹤��
  * @param 	
  * @retval : None
  */
void PerifStop(void)	
{
	OutPinCtrl->BSRR = OutPin;//����ߵ�ƽ,��������ֹͣ����
}
/**
  * @brief	�������蹤��
  * @param 	
  * @retval : None
  */
void PerifStart(void)	
{
	OutPinCtrl->BRR = OutPin;//����ߵ�ƽ,��������ֹͣ����
}

/**
  * @brief	������
  * @param ������������״̬Ȼ��ó���Ӧ�ı�־λ
  * @retval : None
  */
void InputMonitor(void){
	if(((GPIO_ReadInputData(InPinCtrl_H)&(InPin_H))==0)&& \
	((GPIO_ReadInputData(InPinCtrl_L)&(InPin_L))==0)&& \
	((Alarm.Environ+Alarm.LowerWaterSet+Alarm.UpperWaterSet)==0))
	{
		/*��ʱ����*/
		Delayms(20);
		if(((GPIO_ReadInputData(InPinCtrl_H)&(InPin_H))==0)&& \
		((GPIO_ReadInputData(InPinCtrl_L)&(InPin_L))==0)&& \
		((Alarm.Environ+Alarm.LowerWaterSet+Alarm.UpperWaterSet)==0))
		{
			Alarm.flag=0;
			/*������еľ���*/
			Alarm.Powerphase=0;
			Alarm.Compressor=0;
			Alarm.Level=0;
			Alarm.HighWaterFlow=0;
			Alarm.LowWaterFlow=0;
			Alarm.Pumphigh=0;
			Alarm.Pumplow=0;
			if(Alarm.Recover==0)
			{
				Alarm.Recover=1;//������������
				OldMenu=10;//�ָ�����ʼֵ�ǣ�����ǰһ�ε���������
			}
			Chiller.Pump_ON=1;//û�б�������ˮ��
			Chiller.Pump_OFF=0;
		}
		
	}
	else//һ��ʱ��ֻ��ʾһ������
	{
		/*��ʱ����*/
		Delayms(20);
		if(((GPIO_ReadInputData(InPinCtrl_H)&(InPin_H))!=0)|| \
		((GPIO_ReadInputData(InPinCtrl_L)&(InPin_L))!=0)|| \
		((Alarm.Environ+Alarm.LowerWaterSet+Alarm.UpperWaterSet)!=0))
		{
			Alarm.flag=1;//�б���
			Alarm.Recover=0;//�����쳣����
			if(AlarmSaveflag==0)//�ж�ǰһ����ʾ״̬�Ƿ�Ϊ����״̬
				SaveChillerNormalPara=Chiller;//��������ֵ
			if(GPIO_ReadInputDataBit(InPinCtrl_L,Powerphase_IN) == Bit_SET){//�����쳣
				/*�쳣��ʾ��ʶ����*/
				Alarm.Powerphase=1;
				/*��ֹ�������蹤��*/
				Chiller.Pump_OFF=1;
				Chiller.Pump_ON=0;
				Chiller.Valve_OFF=1;
				Chiller.Valve_ON=0;
				Chiller.Compressor_OFF=1;
				Chiller.Compressor_ON=0;
				Chiller.ElBow_OFF=1;
				Chiller.ElBow_ON=0;
			}
			else
				Alarm.Powerphase=0;
			if(GPIO_ReadInputDataBit(InPinCtrl_L,Compressor_IN) == Bit_SET){//ѹ��������
				Alarm.Compressor=1;
				/*�ر�ѹ����*/
				Chiller.Compressor_OFF=1;
				Chiller.Compressor_ON=0;
			}
			else
				Alarm.Compressor=0;
			if(GPIO_ReadInputDataBit(InPinCtrl_L,Level_IN) == Bit_SET){//Һλ�����쳣
				Alarm.Level=1;
				/*�ر�ˮ��,ѹ����*/
				Chiller.Pump_OFF=1;
				Chiller.Pump_ON=0;
				Chiller.Compressor_OFF=1;
				Chiller.Compressor_ON=0;
			}
			else
				Alarm.Level=0;
			if(GPIO_ReadInputDataBit(InPinCtrl_L,HighWater_IN) == Bit_SET){//����ˮ�����쳣
				Alarm.HighWaterFlow=1;
				/*�ر�ˮ��,ѹ����*/
				Chiller.Pump_OFF=1;
				Chiller.Pump_ON=0;
				Chiller.Compressor_OFF=1;
				Chiller.Compressor_ON=0;
			}
			else
				Alarm.HighWaterFlow=0;
			if(GPIO_ReadInputDataBit(InPinCtrl_H,LowWater_IN) == Bit_SET){//����ˮ�����쳣
				Alarm.LowWaterFlow=1;
				/*�ر�ˮ��,ѹ����*/
				Chiller.Pump_OFF=1;
				Chiller.Pump_ON=0;
				Chiller.Compressor_OFF=1;
				Chiller.Compressor_ON=0;
			}
			else
				Alarm.LowWaterFlow=0;
			if(GPIO_ReadInputDataBit(InPinCtrl_H,Pumphigh_IN) == Bit_SET){//����ˮ�ù���
				Alarm.Pumphigh=1;
				/*�ر�ˮ��*/
				Chiller.Pump_OFF=1;
				Chiller.Pump_ON=0;
			}
			else
				Alarm.Pumphigh=0;
			if(GPIO_ReadInputDataBit(InPinCtrl_H,Pumplow_IN) == Bit_SET){//����ˮ�ù���
				Alarm.Pumplow=1;
				/*�ر�ˮ��,ѹ����*/
				Chiller.Pump_OFF=1;
				Chiller.Pump_ON=0;
				Chiller.Compressor_OFF=1;
				Chiller.Compressor_ON=0;
			}
			else
				Alarm.Pumplow=0;
		}	
	}
}
/**
  * @brief	�������
  * @param ���ݱ�־λ������Ӧ�������ƽ
  * @retval : None
  */
void OutPro(void){//�����������
	InputMonitor();//�������״̬
	OutPinCtrl->BRR=((Chiller.Pump_ON<<4)|(Chiller.Valve_ON<<5)|(Chiller.Compressor_ON<<6)|(Chiller.ElBow_ON<<7)| \
					(Chiller.AlarmOUT_OFF<<8)|(Chiller.Opticalcoupler1_ON<<9)|(Chiller.Opticalcoupler2_ON<<10));
	OutPinCtrl->BSRR=((Chiller.Pump_OFF<<4)|(Chiller.Valve_OFF<<5)|(Chiller.Compressor_OFF<<6)|(Chiller.ElBow_OFF<<7)| \
					(Chiller.AlarmOUT_ON<<8)|(Chiller.Opticalcoupler1_OFF<<9)|(Chiller.Opticalcoupler2_OFF<<10));
}


