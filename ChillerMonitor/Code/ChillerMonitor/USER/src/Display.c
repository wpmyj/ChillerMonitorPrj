/**
  *****************************************************************************
  * @file  J12864.c
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	J12864液晶驱动函数.
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
char OldMenu=10;	//定义并初始化上一界面
char AntifreezeFlag;//防冻标志位,一次性有效
char AlarmSaveflag=0;//发生异常时提示保存外设输出各个状态


char *pnull[1]={"                "};//定义一个空白字符串指针数组

char *pctrl[3]={"手动","自动","外控"};//定义一个字符串指针数组
char *pWarning[]={"电源相序异常","压缩机过载","液位过低","高温水流量异常","低温水流量异常",\
					"高温水泵过载","低温水泵过载","环境温度异常","水温高于上限","水温低于下限"};//定义异常警告消息

/**
  * @brief	显示界面结构体,把显示界面的内容做成结构体,总共显示6个界面,它们都是平行关系
  * @param None
  * @retval : None
  */

Menu_Typedef CH_SetupMenuItems[MenuNum] = 
{
	{"                ","雅酷佳制冷设备  ","    为您服务    ","                ",NULL},
	{"正在进行防冻处理","请等待 		 ","低温水: 	 2℃","环境温度：	2℃",NULL},
	{"    本机现为:   ","  手动操作模式  ","  等待按键操作  ","                ",NULL},
	{"    本机现为:   ","  自动操作模式  ","    30秒后      ","自动启动冷水机  ",NULL},
	{"    本机现为:   ","  外控操作模式  ","  等待外控信号  ","                ",NULL},
	{"环境温度:   24℃","低温水:     22℃","高温水:     28℃","手动模式正常运行",EnterNormalDis},
	{"  工作模式设置  ","选择:     模式  ","按  键设置模式  ","                ",EnterModelSetting},       
	{"低温水  温度设置","预设温度：    ℃","按  键设置温度  ","                ",EnterTemperaSetting},
	{"高温水  温度设置","预设温度：    ℃","按  键设置温度  ","                ",EnterTemperaSetting},
	{"下限报警温度设置","预设温度：    ℃","按  键设置温度  ","                ",EnterTemperaSetting},
	{"上限报警温度设置","预设温度：    ℃","按  键设置温度  ","                ",EnterTemperaSetting},
	{"                ","温度参数校准界面","低温水系数   0℃","                ",NULL},
	{"                ","温度参数校准界面","高温水系数   0℃","                ",NULL},
	{"                ","温度参数校准界面","环境系数     0℃","                ",NULL}
};

/** 
  * @brief  平时显示界面
  * @param None
  * @retval : None		Dis_Menu(Key.MenuSelect);
  */
void EnterNormalDis(void)
{
	LCD_Setpos_DispString(4,1,pctrl[Key.Model_count-1]);
	LCD_Setpos_DispDecimal(2,6,TMonitorDis[0],1);//显示低温水
	if(TMonitorDis[0]<10)//如果显示时间小于10
	{
		LCD_WriteByte(0x20);//小数点后第一位显示空白
		LCD_WriteByte(0x20);//小数点后第二位显示空白
	}
	else if(TMonitorDis[0]<100)//如果显示时间小于100
		LCD_WriteByte(0x20);//小数点后第二位显示空白
	LCD_Setpos_DispDecimal(3,6,TMonitorDis[1],1);//显示高温水
	if(TMonitorDis[1]<10)//如果显示时间小于10
	{
		LCD_WriteByte(0x20);//小数点后第一位显示空白
		LCD_WriteByte(0x20);//小数点后第二位显示空白
	}
	else if(TMonitorDis[1]<100)//如果显示时间小于100
		LCD_WriteByte(0x20);//小数点后第二位显示空白
	LCD_Setpos_DispDecimal(1,6,TMonitorDis[2],1);//显示环境温度
	if(TMonitorDis[2]<10)//如果显示时间小于10
	{
		LCD_WriteByte(0x20);//小数点后第一位显示空白
		LCD_WriteByte(0x20);//小数点后第二位显示空白
	}
	else if(TMonitorDis[2]<100)//如果显示时间小于100
		LCD_WriteByte(0x20);//小数点后第二位显示空白

}
/**
  * @brief  进入模式显示界面
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
  * @brief  进入温度设置 显示界面
  * @param None
  * @retval : None
  */
void EnterTemperaSetting(void)
{
	if(Key.MenuSelect>=7)
		LCD_Setpos_DispDecimal(3,6,TemperaSet[Key.MenuSelect-3],1);//显示温度
	else
		LCD_Setpos_DispDecimal(2,6,TemperaSet[Key.MenuSelect-3],1);//显示温度
	LCD_Setpos(3,2);
	LCD_WriteByte(0x1e);//上键图标
	LCD_WriteByte(0x1f);//下键图标
}
/**
  * @brief  报警显示界面
  * @param None
  * @retval : None
  */
void AlarmDis(void){
	if(Alarm.flag>0)
	{
		LCD_Clear();										//清屏
		Chiller.AlarmOUT_ON=1;								//有报警信号输出
		Chiller.AlarmOUT_OFF=0;								//有报警信号输出
		if(Alarm.Powerphase==1)								//电源供电相数异常
			LCD_Setpos_DispString(2,1,*pWarning);
		if(Alarm.Compressor==1)								//压缩机过载异常
			LCD_Setpos_DispString(2,1,*(pWarning+1));
		if(Alarm.Level==1)									//液位过低异常
			LCD_Setpos_DispString(2,1,*(pWarning+2));
		if(Alarm.HighWaterFlow==1)							//高温水流量异常
			LCD_Setpos_DispString(2,1,*(pWarning+3));
		if(Alarm.LowWaterFlow==1)							//低温水流量异常
			LCD_Setpos_DispString(2,1,*(pWarning+4));
		if(Alarm.Pumphigh==1)								//高温水泵过载异常
			LCD_Setpos_DispString(2,1,*(pWarning+5));
		if(Alarm.Pumplow==1)								//低温水泵过载异常
			LCD_Setpos_DispString(2,1,*(pWarning+6));
		/*
		*@显示防冻处理过程中的一些警告
		**/
		if(Alarm.Environ==1)								//环境温度异常
			LCD_Setpos_DispString(2,1,*(pWarning+7));
		if(Alarm.UpperWaterSet==1)							//水温高于上限异常
			LCD_Setpos_DispString(2,1,*(pWarning+8));
		if(Alarm.LowerWaterSet==1)							//水温低于下限异常 
			LCD_Setpos_DispString(2,1,*(pWarning+9));
	}
}

/**
  * @brief  公司LOGO显示界面
  * @param None
  * @retval : None
  */

void DisLogo(void)
{
	Menu_Typedef *pMenu;//定义一个结构体指针
	pMenu=CH_SetupMenuItems;
	LCD_Setpos_DispString(1,1,pMenu->pText1);
	LCD_Setpos_DispString(2,1,pMenu->pText2);
	LCD_Setpos_DispString(3,1,pMenu->pText3);
	LCD_Setpos_DispString(4,1,pMenu->pText4);
}
/**
  * @brief  操作模式等待界面
  * @param None
  * @retval : None
  */

void WaitOperation(void)
{
	Menu_Typedef *pvMenu;//定义一个结构体指针
	static char workstart;//静态变量只在本函数中改变数值
	workstart=0;
	if(Alarm.flag==0)//若没有警告则进入模式显示界面一段时间
	{
		if(Config_Data[0]==1)//手动操作模式界面
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
			while((Key.MenuSetFlag==2)||(Key.MenuSetFlag==0))//等待按键操作
			{
				Key_Process();
				PerifStop();//停止所有外设工作
				/*
				*@以下为复位显示参数初值
				*/
				Key.Model_flag=0;
				Key.Set_flag=0;
				Key.Up_flag=0;
				Key.Down_flag=0;
				Key.MenuSelect=0;
				OldMenu=10;//保持的界面恢复到初始化状态
			}
		}	
		else if(Config_Data[0]==2)//自动操作模式界面
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
				while(AutoTime<100)//等待计数时间
				{
					LCD_Setpos_Dispnum(3,3,(10-(AutoTime/10)));//显示等待时间
					if(AutoTime>10)//如果显示时间小于10S
						LCD_WriteByte(0x20);//个位显示空白
					PerifStop();//停止所有外设工作
					/*
					*@以下为复位显示参数初值
					*/
					Key.Model_flag=0;
					Key.Set_flag=0;
					Key.Up_flag=0;
					Key.Down_flag=0;
					Key.MenuSelect=0;
					OldMenu=10;//保持的界面恢复到初始化状态
				}
			}
		}	
		else if(Config_Data[0]==3)//外控操作模式界面
		{
			pvMenu=(CH_SetupMenuItems+4);
			workstart=1;
			if((Key.MenuSetFlag==2)||(Key.MenuSetFlag==0))
			{
				LCD_Setpos_DispString(1,1,pvMenu->pText1);
				LCD_Setpos_DispString(2,1,pvMenu->pText2);
				LCD_Setpos_DispString(3,1,pvMenu->pText3);
				LCD_Setpos_DispString(4,1,pvMenu->pText4);
				PerifStop();//停止所有外设工作
				/*
				*@以下为复位显示参数初值
				*/
				Key.Model_flag=0;
				Key.Set_flag=0;
				Key.Up_flag=0;
				Key.Down_flag=0;
				Key.MenuSelect=0;
				OldMenu=10;//保持的界面恢复到初始化状态
				while(GPIO_ReadInputDataBit(GPIOA,ExCtr_IN) == Bit_SET)//等待外控信号
				{
				}
			}
		}
		if(workstart==1)//如果有启动正常工作标志，则初始化外设输出
		{
			workstart=0;
			Key.MenuSelect=1;//时间到了进入正常显示界面
			Key.MenuSetFlag=1;//工作模式
			/*
			*按了按键之后开启水泵，其他的外设关闭
			*/
			Chiller.Pump_OFF=0;
			Chiller.Pump_ON=1;//开启水泵
			Chiller.Valve_ON=0;
			Chiller.Valve_OFF=1;
			Chiller.Compressor_ON=0;
			Chiller.Compressor_OFF=1;
			Chiller.ElBow_ON=0;
			Chiller.ElBow_OFF=1;
			Chiller.AlarmOUT_ON=0;
			Chiller.AlarmOUT_OFF=1;
			GetSysPrama();	//获得设置值准备启动工作模式
		}
	}
}
/**
  * @brief  防冻处理界面
  * @param None
  * @retval : None
  */

void AntifreezeDis(void)
{
	Menu_Typedef *pMenu;//定义一个结构体指针
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
	if(TMonitorDis[1]<10)//如果显示时间小于10
	{
		LCD_WriteByte(0x20);//小数点后第二位显示空白
		LCD_WriteByte(0x20);//小数点后第二位显示空白
	}
	else if(TMonitorDis[1]<100)//如果显示时间小于100
		LCD_WriteByte(0x20);//小数点后第二位显示空白
	LCD_Setpos_DispDecimal(3,6,TMonitorDis[2],1);
	if(TMonitorDis[2]<10)//如果显示时间小于10
	{
		LCD_WriteByte(0x20);//小数点后第二位显示空白
		LCD_WriteByte(0x20);//小数点后第二位显示空白
	}
	else if(TMonitorDis[2]<100)//如果显示时间小于100
		LCD_WriteByte(0x20);//小数点后第二位显示空白
}

/**
  * @brief  显示界面
  * @param None
  * @retval : None
  */

void Dis_Menu(char MenuDis)
{
	Menu_Typedef *pCurrentMenu;//定义一个结构体指针
	if(Alarm.flag==0)//先判断是否报警，报警的界面优先处理
	{
		if(AlarmSaveflag==1)
		{
			AlarmSaveflag=0;
			Chiller=SaveChillerNormalPara;//获取异常前一刻保存保存的外设输出状态
			Chiller.AlarmOUT_ON=0;//无报警信号输出
			Chiller.AlarmOUT_OFF=1;//无报警信号输出
		} 
		if(Key.MenuSetFlag==1)
		{
			if(MenuDis!=OldMenu)//若同一个界面这部分数据不用刷新
			{
				pCurrentMenu=(CH_SetupMenuItems+MenuDis+4);
				OldMenu=MenuDis;
				LCD_Setpos_DispString(1,1,pCurrentMenu->pText1);
				LCD_Setpos_DispString(2,1,pCurrentMenu->pText2);
				LCD_Setpos_DispString(3,1,pCurrentMenu->pText3);
				LCD_Setpos_DispString(4,1,pCurrentMenu->pText4);
			}
			(*pCurrentMenu->pMenuFunc)();//更改参数指针
		}
		if((Key.MenuSetFlag==2)||(Key.MenuSetFlag==0))//关闭工作模式
		{
			WaitOperation();//等待按键操作
		}
	}
	if(Alarm.flag>0)//先判断是否报警，报警的界面优先处理
	{
		AlarmDis();//显示报警界面
		AlarmSaveflag=1;
	}

}


