/**
  *****************************************************************************
  * @file  Tempera_ad.c
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	DMA��ʽ��ȡADCֵӦ�ú����� 
  *****************************************************************************
  */ 
#include "key.h" 
#include "Tempra_AD.h"
#include "IO_Process.h"
#include "Display.h"
#include "stm32f10x_it.h"
#include "Timer.h"

#define ADC1_DR_Address    ((u32)0x4001244C)	//ADC1�����ַ
/*
*@�����˲�ģʽ�ı���,����Ϊһ���ڴ��ַ,��ʼ��Ϊ0���洢DMA���������ADC�Ĵ���Ϊ16λ
*/
vu16 ADGetValue[ADC1_Count][ADC_ChannelCount];   		//AD����ֵ,ADC_ChannelCount��ͨ��,ÿ��ͨ������ADC1_Count��
vu16 ADC_Filter[ADC_ChannelCount];   					//AD�˲���,����һ��ͨ��ADC1_Count�����ݵľ�ֵ
static u32 TMonitor[3]={0};									//��ż�����õ��¶�ֵ������Ϊ����,����,�����¶�
u32 TMonitorDis[3]={0};									//������ʾ��ż�����õ��¶�ֵ������Ϊ����,����,�����¶�
u32 ADC_SmoothCount[ADC_ChannelCount];
static u32 ADC_SmoothFilter[ADC_ChannelCount][SmoothNum]={0};	//ADC_ChannelCount��ͨ����ÿ��SmoothNum��ƽ��ֵ
u32 Smoth_Num[ADC_ChannelCount]={0};
char Tflag=0;
/**
  * @brief  ���ò���ͨ���˿� ʹ��GPIOʱ��	 ����ADC����PA0�˿��ź�
  * @param None
  * @retval : None
  */
void ADC1_GPIO_Config(void)
{ 
	GPIO_InitTypeDef ADC_PORT;    
	RCC_APB2PeriphClockCmd(ADC_ClK, ENABLE);	  
	ADC_PORT.GPIO_Pin = ADC_Pin;				//ADC����ѡ��,6��ͨ��
	ADC_PORT.GPIO_Mode = GPIO_Mode_AIN;		    //GPIO����Ϊģ������
	GPIO_Init(ADC_CONTROL, &ADC_PORT);   
}
/**
  * @brief  ����ADC1�Ĺ���ģʽΪDMAģʽ
  * @param None
  * @retval : None
  */
 void ADC1_Mode_Config(void)
 {
	ADC_InitTypeDef ADC_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//ʹ��ADC1ʱ�� 
	/* ADC1 configuration */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 		//ʹ�ö���ģʽ��ɨ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;			//ѭ��ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 		//����ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1; //�ⲿ��ʱ������ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = ADC_ChannelCount;  //��ADC_ChannelCount��ת��ͨ��
	ADC_Init(ADC1, &ADC_InitStructure);
	/* ����Ϊ����ͨ�����ܹ�6·,�ֱ���AD10,AD11,AD12,AD13,AD14,AD15 */
	/* ADC1 regular channel10 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5); //ͨ��10,��Ϊ��1������Ŀ��,��������239.5��ʱ������
	/* ADC1 regular channel11 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5); //ͨ��11,��Ϊ��2������Ŀ��,��������55.5��ʱ������
	/* ADC1 regular channel12 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5); //ͨ��12,��Ϊ��3������Ŀ��,��������55.5��ʱ������
	/* ADC1 regular channel13 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_55Cycles5); //ͨ��13,��Ϊ��4������Ŀ��,��������55.5��ʱ������
	/* ADC1 regular channel14 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_55Cycles5); //ͨ��14,��Ϊ��5������Ŀ��,��������55.5��ʱ������
	/* ADC1 regular channel15 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 6, ADC_SampleTime_55Cycles5); //ͨ��15,��Ϊ��6������Ŀ��,��������55.5��ʱ������

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);	 //ʹ��ADC��DMA
	/*ʹ��ADC1�ⲿ����*/
	ADC_ExternalTrigConvCmd(ADC1,ENABLE);
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE); //ʹ��ADC1

	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
	 
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //��ʼת��
	
}
/**
  * @brief  ����DMA
  * @param None
  * @retval : None
  */
 void DMA_Config(void)
 {
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //ʹ��DMA1ʱ��
	/* DMA1 channel1 configuration */
	DMA_DeInit(DMA1_Channel1);  //ָ��DMA1ͨ��1
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//����DMA�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADGetValue;//����DMA�ڴ��ַ��ADCת�����ֱ�ӷ���õ�ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //��������Ϊ���ݴ������Դ
	DMA_InitStructure.DMA_BufferSize = (ADC_ChannelCount* ADC1_Count);	//DMA����������ΪADC_ChannelCount* ADC1_Count,��6��ͨ����
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//����Դ�����ݿ�Ȱ���,ADC1_DR��ֵ16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//ѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);  //ʹ��DMAͨ��
	//DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); //ʹ��DMA��������ж� 
}

/**
  * @brief  ��ʼ��ADC1
  * @param None
  * @retval : None
  */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
	DMA_Config();
	TIM1_Mode_Config();//������ʱ��1������PWN1�ⲿ����Դ
}

/**
  * @brief  ADC�������õ����¶�
  * @param None
  * @retval : None
  */
void ADC1_Tempera(void)
{
	u32 i,k,m,n;
	static u32 ValueCount;
	for(i=0;i<ADC_ChannelCount;i++)
	{
		/*
		*@ƽ��ֵ�˲�
		*/
		ValueCount=0;
		for(k=0;k<ADC1_Count;k++)//ȡ�м��100��ֵ(ADC1_Count)
		{
			ValueCount+=ADGetValue[k][i];
			ADC_Filter[i]=(ValueCount/ADC1_Count);//��ȡһ��ADCͨ����ƽ��ֵ
		}
		if(Tflag==0)
		{
			for(n=0;n<SmoothNum;n++)
			{
				ADC_SmoothFilter[i][n]=ADC_Filter[i];//��õ�һ��ƽ��ֵ
			}
			ADC_SmoothCount[i]=ADC_Filter[i];//��ȡ��һ��ƽ��ƽ��ֵ��
		}
		/*
		*@ƽ���˲�
		*/
		if(Tflag==1)
		{
			ADC_SmoothFilter[i][Smoth_Num[i]]=ADC_Filter[i];//����ֵѭ������
			Smoth_Num[i]++;
			if(Smoth_Num[i]==SmoothNum)
				Smoth_Num[i]=0;
			for(n=0;n<SmoothNum;n++)//ƽ���˲�
			{
				ADC_SmoothCount[i] += ADC_SmoothFilter[i][n];
			}
			ADC_SmoothCount[i]=(ADC_SmoothCount[i]/SmoothNum);//��ȡƽ��ƽ��ֵ��
		}
	}
	for(m=0;m<3;m++)
	{
		TMonitor[m]=(((2361*ADC_SmoothCount[2*m])/(ADC_SmoothCount[2*m+1])-2597)+TemperaSet[m+4]);//�Ѿ�����10����ֵת��Ϊ�¶�����,����,����,�����¶� TemperaSet[m+4]Ϊ�¶Ȳ���ϵ��
		if(Tflag==0)
			TMonitorDis[m]=TMonitor[m];
		/*
		*@��ʾ�������仯
		*/
		if(TMonitorDis[m]<(TMonitor[m]-1))
			TMonitorDis[m] += 1;
		if(TMonitorDis[m]>(TMonitor[m]+1))
			TMonitorDis[m] -= 1;
	}
	Tflag=1;
}

/**
  * @brief  ����������
  * @param None
  * @retval : None
  */
void AntifreezePro(void)
{
	ADC1_Tempera();
	/*
	*@�����¶�С��10�ȷ�������
	*
	*/
	if(TMonitorDis[2]<100)//��������
	{ 
 		while(TMonitorDis[0]<100)
		{
			ADC1_Tempera();
			if(TMonitorDis[0]<=20)//����ˮˮ��С��2��
			{
				/*�����������*/
				Chiller.ElBow_ON=1;
				Chiller.ElBow_OFF=0;
			}
			if(TMonitorDis[0]>50)//����ˮˮ�´���5��
			{
				/*�������������ˮ��*/
				Chiller.ElBow_ON=1;
				Chiller.ElBow_OFF=0;
				
				Chiller.Pump_ON=1;
				Chiller.Pump_OFF=0;
			}
			AntifreezeDis();
			OutPro();//������ƴ���
		}
		if(TMonitorDis[0]>=100)
		{
			/*�رյ��������ˮ��*/
			Chiller.ElBow_ON=0;
			Chiller.ElBow_OFF=1;
			
			Chiller.Pump_ON=0;
			Chiller.Pump_OFF=1;
			OutPro();//������ƴ���
		}
	}
}

/**
  * @brief  �¶ȴ�����
  * @param None
  * @retval : None
  */
void TemperaPro(void)
{
	/*
	*@�����¶�֮��
	*
	*/
	if(TMonitorDis[0]>=100)//�ڷ����¶�֮��
	{
		/*
		*@�����쳣ˮ�µı�������
		*
		*/
		if((TMonitorDis[0]>(TemperaSet[3]))||(TMonitorDis[1]>(TemperaSet[3])))//ˮ�¸�����������ֵ
		{
			/*���������¶��쳣��־*/
			Alarm.UpperWaterSet=1;
			Alarm.flag=1;
			if(AlarmSaveflag==0)//�ж�ǰһ����ʾ״̬�Ƿ�Ϊ����״̬
				SaveChillerNormalPara=Chiller;//��������ֵ
		}
		else
			Alarm.UpperWaterSet=0;		
		if((TMonitorDis[0]<(TemperaSet[2]))||(TMonitorDis[1]<(TemperaSet[2])))//ˮ�µ�����������ֵ
		{
			/*���������¶��쳣��־*/
			Alarm.LowerWaterSet=1;
			Alarm.flag=1;
			if(AlarmSaveflag==0)//�ж�ǰһ����ʾ״̬�Ƿ�Ϊ����״̬
				SaveChillerNormalPara=Chiller;//��������ֵ
		}
		else
			Alarm.LowerWaterSet=0;
		if((TMonitorDis[2]<30)||(TMonitorDis[2]>450))//���������Ļ����¶ȷ�Χ3~45��
		{
			/*���������¶��쳣��־*/
			Alarm.Environ=1;
			Alarm.flag=1;
			if(AlarmSaveflag==0)//�ж�ǰһ����ʾ״̬�Ƿ�Ϊ����״̬
				SaveChillerNormalPara=Chiller;//��������ֵ
		}
		else
			Alarm.Environ=0;
		/*
		*@���������¶��Զ����ڴ���
		*
		*/
	
		if(TMonitorDis[0]>(TemperaSet[0]))//����ˮˮ�¸����趨ֵ������ѹ�������
		{
			/*����ѹ����*/
			Chiller.Compressor_ON=1;
			Chiller.Compressor_OFF=0;
			
		}
		if(TMonitorDis[0]<(TemperaSet[0]-DValue.LowW))//����ˮˮ������ر�ѹ�������
		{
			/*�ر�ѹ�������*/
			Chiller.Compressor_ON=0;
			Chiller.Compressor_OFF=1;
			
		}
		if(TMonitorDis[1]>(TemperaSet[1]))//����ˮ�¸����趨ֵ������ˮ��
		{
			/*����ˮ��*/
			Chiller.Valve_ON=1;
			Chiller.Valve_OFF=0;
		}
		if(TMonitorDis[1]<(TemperaSet[1]-DValue.HighW))//����ˮˮ��������ر�ˮ��
		{
			/*�ر�ˮ��*/
			Chiller.Valve_ON=0;
			Chiller.Valve_OFF=1;
		}
	}
}

