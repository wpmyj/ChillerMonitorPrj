#include "IO_Process.h"
#include "Display.h"
#include "Tempra_AD.h"
#include "Delay_Timer.h"

	
/* private------- ------------------------------------------------------------*/
Alarm_Typedef Alarm={0,0,0,0,0,0,0,0,1,0,0,0};//报警标识位
Perif_Typedef Chiller={0,0,0,0,0,0,0,0,0,0,0,0,0,0};//冷水机外设工作使能标识
Perif_Typedef SaveChillerNormalPara={0,0,0,0,0,0,0,0,0,0,0,0,0,0};//保存冷水机外设异常前工作使能标识

/**
  * @brief	普通输入输出引脚配置
  * @param None
  * @retval : None
  */
/***************  配置输入输出用到的I/O口 *******************/
void IOputConfig(void)	
{
	/*配置输入口*/
	GPIO_InitTypeDef InPort_L,InPort_H,OutPort;
	RCC_APB2PeriphClockCmd(InPinCtrl_Clk ,ENABLE); //时钟使能
	InPort_L.GPIO_Pin  = InPin_L;//选择输出口
	InPort_L.GPIO_Speed= GPIO_Speed_50MHz;
	InPort_L.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_Init(InPinCtrl_L,&InPort_L);
	
	InPort_H.GPIO_Pin  = InPin_H|ExCtr_IN;//选择输出口
	InPort_H.GPIO_Speed= GPIO_Speed_50MHz;
	InPort_H.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
	GPIO_Init(InPinCtrl_H,&InPort_H);
	
	/* 配置输出口*/
	RCC_APB2PeriphClockCmd(OutPinCtrl_Clk ,ENABLE); //时钟使能
	OutPort.GPIO_Pin  = OutPin;//选择输出口
	OutPort.GPIO_Speed= GPIO_Speed_50MHz;
	//	LCD_port.GPIO_Mode = GPIO_Mode_Out_OD;//OD开路输出，外接上拉电阻到5V
	OutPort.GPIO_Mode = GPIO_Mode_Out_PP;//PP推挽输出
	GPIO_Init(OutPinCtrl,&OutPort);
}
/**
  * @brief	停止外设工作
  * @param 	
  * @retval : None
  */
void PerifStop(void)	
{
	OutPinCtrl->BSRR = OutPin;//输出高电平,所有外设停止工作
}
/**
  * @brief	开启外设工作
  * @param 	
  * @retval : None
  */
void PerifStart(void)	
{
	OutPinCtrl->BRR = OutPin;//输出高电平,所有外设停止工作
}

/**
  * @brief	输入监控
  * @param 处理各个输入的状态然后得出相应的标志位
  * @retval : None
  */
void InputMonitor(void){
	if(((GPIO_ReadInputData(InPinCtrl_H)&(InPin_H))==0)&& \
	((GPIO_ReadInputData(InPinCtrl_L)&(InPin_L))==0)&& \
	((Alarm.Environ+Alarm.LowerWaterSet+Alarm.UpperWaterSet)==0))
	{
		/*延时消抖*/
		Delayms(20);
		if(((GPIO_ReadInputData(InPinCtrl_H)&(InPin_H))==0)&& \
		((GPIO_ReadInputData(InPinCtrl_L)&(InPin_L))==0)&& \
		((Alarm.Environ+Alarm.LowerWaterSet+Alarm.UpperWaterSet)==0))
		{
			Alarm.flag=0;
			/*清除所有的警告*/
			Alarm.Powerphase=0;
			Alarm.Compressor=0;
			Alarm.Level=0;
			Alarm.HighWaterFlow=0;
			Alarm.LowWaterFlow=0;
			Alarm.Pumphigh=0;
			Alarm.Pumplow=0;
			if(Alarm.Recover==0)
			{
				Alarm.Recover=1;//进入正常界面
				OldMenu=10;//恢复到初始值是，警告前一次的正常界面
			}
			Chiller.Pump_ON=1;//没有报警开启水泵
			Chiller.Pump_OFF=0;
		}
		
	}
	else//一个时候只显示一个警告
	{
		/*延时消抖*/
		Delayms(20);
		if(((GPIO_ReadInputData(InPinCtrl_H)&(InPin_H))!=0)|| \
		((GPIO_ReadInputData(InPinCtrl_L)&(InPin_L))!=0)|| \
		((Alarm.Environ+Alarm.LowerWaterSet+Alarm.UpperWaterSet)!=0))
		{
			Alarm.flag=1;//有报警
			Alarm.Recover=0;//进入异常界面
			if(AlarmSaveflag==0)//判断前一次显示状态是否为正常状态
				SaveChillerNormalPara=Chiller;//保存正常值
			if(GPIO_ReadInputDataBit(InPinCtrl_L,Powerphase_IN) == Bit_SET){//相序异常
				/*异常显示标识开启*/
				Alarm.Powerphase=1;
				/*禁止所有外设工作*/
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
			if(GPIO_ReadInputDataBit(InPinCtrl_L,Compressor_IN) == Bit_SET){//压缩机过载
				Alarm.Compressor=1;
				/*关闭压缩机*/
				Chiller.Compressor_OFF=1;
				Chiller.Compressor_ON=0;
			}
			else
				Alarm.Compressor=0;
			if(GPIO_ReadInputDataBit(InPinCtrl_L,Level_IN) == Bit_SET){//液位开关异常
				Alarm.Level=1;
				/*关闭水泵,压缩机*/
				Chiller.Pump_OFF=1;
				Chiller.Pump_ON=0;
				Chiller.Compressor_OFF=1;
				Chiller.Compressor_ON=0;
			}
			else
				Alarm.Level=0;
			if(GPIO_ReadInputDataBit(InPinCtrl_L,HighWater_IN) == Bit_SET){//高温水流量异常
				Alarm.HighWaterFlow=1;
				/*关闭水泵,压缩机*/
				Chiller.Pump_OFF=1;
				Chiller.Pump_ON=0;
				Chiller.Compressor_OFF=1;
				Chiller.Compressor_ON=0;
			}
			else
				Alarm.HighWaterFlow=0;
			if(GPIO_ReadInputDataBit(InPinCtrl_H,LowWater_IN) == Bit_SET){//低温水流量异常
				Alarm.LowWaterFlow=1;
				/*关闭水泵,压缩机*/
				Chiller.Pump_OFF=1;
				Chiller.Pump_ON=0;
				Chiller.Compressor_OFF=1;
				Chiller.Compressor_ON=0;
			}
			else
				Alarm.LowWaterFlow=0;
			if(GPIO_ReadInputDataBit(InPinCtrl_H,Pumphigh_IN) == Bit_SET){//高温水泵过载
				Alarm.Pumphigh=1;
				/*关闭水泵*/
				Chiller.Pump_OFF=1;
				Chiller.Pump_ON=0;
			}
			else
				Alarm.Pumphigh=0;
			if(GPIO_ReadInputDataBit(InPinCtrl_H,Pumplow_IN) == Bit_SET){//低温水泵过载
				Alarm.Pumplow=1;
				/*关闭水泵,压缩机*/
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
  * @brief	输出处理
  * @param 根据标志位控制相应的输出电平
  * @retval : None
  */
void OutPro(void){//处理外设输出
	InputMonitor();//获得输入状态
	OutPinCtrl->BRR=((Chiller.Pump_ON<<4)|(Chiller.Valve_ON<<5)|(Chiller.Compressor_ON<<6)|(Chiller.ElBow_ON<<7)| \
					(Chiller.AlarmOUT_OFF<<8)|(Chiller.Opticalcoupler1_ON<<9)|(Chiller.Opticalcoupler2_ON<<10));
	OutPinCtrl->BSRR=((Chiller.Pump_OFF<<4)|(Chiller.Valve_OFF<<5)|(Chiller.Compressor_OFF<<6)|(Chiller.ElBow_OFF<<7)| \
					(Chiller.AlarmOUT_ON<<8)|(Chiller.Opticalcoupler1_OFF<<9)|(Chiller.Opticalcoupler2_OFF<<10));
}


