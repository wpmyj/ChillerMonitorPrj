/**
  *****************************************************************************
  * @file  Tempera_ad.c
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	DMA方式读取ADC值应用函数库 
  *****************************************************************************
  */ 
#include "key.h" 
#include "Tempra_AD.h"
#include "IO_Process.h"
#include "Display.h"
#include "stm32f10x_it.h"
#include "Timer.h"

#define ADC1_DR_Address    ((u32)0x4001244C)	//ADC1外设地址
/*
*@采用滤波模式的变量,设置为一段内存地址,初始化为0，存储DMA放入的数据ADC寄存器为16位
*/
vu16 ADGetValue[ADC1_Count][ADC_ChannelCount];   		//AD采样值,ADC_ChannelCount个通道,每个通道采样ADC1_Count次
vu16 ADC_Filter[ADC_ChannelCount];   					//AD滤波后,计算一个通道ADC1_Count次数据的均值
static u32 TMonitor[3]={0};									//存放检测所得的温度值，依次为低温,高温,环境温度
u32 TMonitorDis[3]={0};									//用来显示存放检测所得的温度值，依次为低温,高温,环境温度
u32 ADC_SmoothCount[ADC_ChannelCount];
static u32 ADC_SmoothFilter[ADC_ChannelCount][SmoothNum]={0};	//ADC_ChannelCount个通道，每个SmoothNum次平滑值
u32 Smoth_Num[ADC_ChannelCount]={0};
char Tflag=0;
/**
  * @brief  配置采样通道端口 使能GPIO时钟	 设置ADC采样PA0端口信号
  * @param None
  * @retval : None
  */
void ADC1_GPIO_Config(void)
{ 
	GPIO_InitTypeDef ADC_PORT;    
	RCC_APB2PeriphClockCmd(ADC_ClK, ENABLE);	  
	ADC_PORT.GPIO_Pin = ADC_Pin;				//ADC引脚选择,6个通道
	ADC_PORT.GPIO_Mode = GPIO_Mode_AIN;		    //GPIO设置为模拟输入
	GPIO_Init(ADC_CONTROL, &ADC_PORT);   
}
/**
  * @brief  配置ADC1的工作模式为DMA模式
  * @param None
  * @retval : None
  */
 void ADC1_Mode_Config(void)
 {
	ADC_InitTypeDef ADC_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//使能ADC1时钟 
	/* ADC1 configuration */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; 		//使用独立模式，扫描模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;			//循环扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 		//连续转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1; //外部定时器触发转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = ADC_ChannelCount;  //有ADC_ChannelCount个转换通道
	ADC_Init(ADC1, &ADC_InitStructure);
	/* 以下为配置通道数总共6路,分别是AD10,AD11,AD12,AD13,AD14,AD15 */
	/* ADC1 regular channel10 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_55Cycles5); //通道10,作为第1个采样目标,采样周期239.5个时钟周期
	/* ADC1 regular channel11 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_55Cycles5); //通道11,作为第2个采样目标,采样周期55.5个时钟周期
	/* ADC1 regular channel12 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_55Cycles5); //通道12,作为第3个采样目标,采样周期55.5个时钟周期
	/* ADC1 regular channel13 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_55Cycles5); //通道13,作为第4个采样目标,采样周期55.5个时钟周期
	/* ADC1 regular channel14 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 5, ADC_SampleTime_55Cycles5); //通道14,作为第5个采样目标,采样周期55.5个时钟周期
	/* ADC1 regular channel15 configuration */ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 6, ADC_SampleTime_55Cycles5); //通道15,作为第6个采样目标,采样周期55.5个时钟周期

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);	 //使能ADC的DMA
	/*使能ADC1外部触发*/
	ADC_ExternalTrigConvCmd(ADC1,ENABLE);
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE); //使能ADC1

	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
	 
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //开始转换
	
}
/**
  * @brief  配置DMA
  * @param None
  * @retval : None
  */
 void DMA_Config(void)
 {
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); //使能DMA1时钟
	/* DMA1 channel1 configuration */
	DMA_DeInit(DMA1_Channel1);  //指定DMA1通道1
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;//设置DMA外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADGetValue;//设置DMA内存地址，ADC转换结果直接放入该地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //外设设置为数据传输的来源
	DMA_InitStructure.DMA_BufferSize = (ADC_ChannelCount* ADC1_Count);	//DMA缓冲区设置为ADC_ChannelCount* ADC1_Count,有6个通道；
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不增加
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设源的数据宽度半字,ADC1_DR的值16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	/* Enable DMA channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);  //使能DMA通道
	//DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE); //使能DMA传输完成中断 
}

/**
  * @brief  初始化ADC1
  * @param None
  * @retval : None
  */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
	DMA_Config();
	TIM1_Mode_Config();//开启定时器1，产生PWN1外部触发源
}

/**
  * @brief  ADC采样所得到的温度
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
		*@平均值滤波
		*/
		ValueCount=0;
		for(k=0;k<ADC1_Count;k++)//取中间的100次值(ADC1_Count)
		{
			ValueCount+=ADGetValue[k][i];
			ADC_Filter[i]=(ValueCount/ADC1_Count);//求取一个ADC通道的平均值
		}
		if(Tflag==0)
		{
			for(n=0;n<SmoothNum;n++)
			{
				ADC_SmoothFilter[i][n]=ADC_Filter[i];//获得第一次平滑值
			}
			ADC_SmoothCount[i]=ADC_Filter[i];//获取第一次平滑平均值线
		}
		/*
		*@平滑滤波
		*/
		if(Tflag==1)
		{
			ADC_SmoothFilter[i][Smoth_Num[i]]=ADC_Filter[i];//把数值循环放置
			Smoth_Num[i]++;
			if(Smoth_Num[i]==SmoothNum)
				Smoth_Num[i]=0;
			for(n=0;n<SmoothNum;n++)//平滑滤波
			{
				ADC_SmoothCount[i] += ADC_SmoothFilter[i][n];
			}
			ADC_SmoothCount[i]=(ADC_SmoothCount[i]/SmoothNum);//获取平滑平均值线
		}
	}
	for(m=0;m<3;m++)
	{
		TMonitor[m]=(((2361*ADC_SmoothCount[2*m])/(ADC_SmoothCount[2*m+1])-2597)+TemperaSet[m+4]);//已经扩大10倍的值转化为温度数据,低温,高温,环境温度 TemperaSet[m+4]为温度补偿系数
		if(Tflag==0)
			TMonitorDis[m]=TMonitor[m];
		/*
		*@显示处理缓慢变化
		*/
		if(TMonitorDis[m]<(TMonitor[m]-1))
			TMonitorDis[m] += 1;
		if(TMonitorDis[m]>(TMonitor[m]+1))
			TMonitorDis[m] -= 1;
	}
	Tflag=1;
}

/**
  * @brief  防冻处理函数
  * @param None
  * @retval : None
  */
void AntifreezePro(void)
{
	ADC1_Tempera();
	/*
	*@环境温度小于10度防冻处理
	*
	*/
	if(TMonitorDis[2]<100)//防冻功能
	{ 
 		while(TMonitorDis[0]<100)
		{
			ADC1_Tempera();
			if(TMonitorDis[0]<=20)//低温水水温小于2度
			{
				/*开启电加热器*/
				Chiller.ElBow_ON=1;
				Chiller.ElBow_OFF=0;
			}
			if(TMonitorDis[0]>50)//低温水水温大于5度
			{
				/*开启电加热器和水泵*/
				Chiller.ElBow_ON=1;
				Chiller.ElBow_OFF=0;
				
				Chiller.Pump_ON=1;
				Chiller.Pump_OFF=0;
			}
			AntifreezeDis();
			OutPro();//输出控制处理
		}
		if(TMonitorDis[0]>=100)
		{
			/*关闭电加热器和水泵*/
			Chiller.ElBow_ON=0;
			Chiller.ElBow_OFF=1;
			
			Chiller.Pump_ON=0;
			Chiller.Pump_OFF=1;
			OutPro();//输出控制处理
		}
	}
}

/**
  * @brief  温度处理函数
  * @param None
  * @retval : None
  */
void TemperaPro(void)
{
	/*
	*@防冻温度之外
	*
	*/
	if(TMonitorDis[0]>=100)//在防冻温度之外
	{
		/*
		*@所有异常水温的报警处理
		*
		*/
		if((TMonitorDis[0]>(TemperaSet[3]))||(TMonitorDis[1]>(TemperaSet[3])))//水温高于上限设置值
		{
			/*开启上限温度异常标志*/
			Alarm.UpperWaterSet=1;
			Alarm.flag=1;
			if(AlarmSaveflag==0)//判断前一次显示状态是否为正常状态
				SaveChillerNormalPara=Chiller;//保存正常值
		}
		else
			Alarm.UpperWaterSet=0;		
		if((TMonitorDis[0]<(TemperaSet[2]))||(TMonitorDis[1]<(TemperaSet[2])))//水温低于下限设置值
		{
			/*开启下限温度异常标志*/
			Alarm.LowerWaterSet=1;
			Alarm.flag=1;
			if(AlarmSaveflag==0)//判断前一次显示状态是否为正常状态
				SaveChillerNormalPara=Chiller;//保存正常值
		}
		else
			Alarm.LowerWaterSet=0;
		if((TMonitorDis[2]<30)||(TMonitorDis[2]>450))//超出工作的环境温度范围3~45度
		{
			/*开启环境温度异常标志*/
			Alarm.Environ=1;
			Alarm.flag=1;
			if(AlarmSaveflag==0)//判断前一次显示状态是否为正常状态
				SaveChillerNormalPara=Chiller;//保存正常值
		}
		else
			Alarm.Environ=0;
		/*
		*@正常工作温度自动调节处理
		*
		*/
	
		if(TMonitorDis[0]>(TemperaSet[0]))//低温水水温高于设定值，开启压缩机风机
		{
			/*开启压缩机*/
			Chiller.Compressor_ON=1;
			Chiller.Compressor_OFF=0;
			
		}
		if(TMonitorDis[0]<(TemperaSet[0]-DValue.LowW))//低温水水正常后关闭压缩机风机
		{
			/*关闭压缩机风机*/
			Chiller.Compressor_ON=0;
			Chiller.Compressor_OFF=1;
			
		}
		if(TMonitorDis[1]>(TemperaSet[1]))//高温水温高于设定值，开启水阀
		{
			/*开启水阀*/
			Chiller.Valve_ON=1;
			Chiller.Valve_OFF=0;
		}
		if(TMonitorDis[1]<(TemperaSet[1]-DValue.HighW))//高温水水温正常后关闭水阀
		{
			/*关闭水阀*/
			Chiller.Valve_ON=0;
			Chiller.Valve_OFF=1;
		}
	}
}

