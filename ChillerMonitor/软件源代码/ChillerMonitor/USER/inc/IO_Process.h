#ifndef __IO_Process_H
#define	__IO_Process_H
/* Define to prevent recursive inclusion -------------------------------------------*/
#include "stm32f10x.h"
/* Exported constants ----------------------------------------------------------*/
/** 
  * @输入报警状态标识
  */
typedef struct
{
	char Powerphase;//相序异常标识位
	char Compressor;//压缩机异常标识位
	char Level;//液位异常标识位
	char HighWaterFlow;//高温水水流量异常标识位
	char LowWaterFlow;//低温水水流量异常标识位
	char Pumphigh;//高温水水泵异常标识位
	char Pumplow;//低温水水泵异常标识位
	char flag;//异常出现
	char Recover;//恢复正常显示
	
	/*有MCU采样的温度数据标志位*/
	char LowerWaterSet;//水温下限报警标志
	char UpperWaterSet;//水温上限报警标志
	char Environ;//环境温度异常标志
}Alarm_Typedef;

/** 
  * @输出外设使能标志
  */

typedef struct
{
	char Pump_ON; 				//水泵使能脚
	char Valve_ON; 				//水阀使能脚
	char Compressor_ON; 		//压缩机使能脚
	char ElBow_ON;				//电加热器使能脚
	char AlarmOUT_ON; 			//报警信号使能脚
	char Opticalcoupler1_ON; 	//光耦输出2使能脚
	char Opticalcoupler2_ON; 	//光耦输出2使能脚

	char Pump_OFF; 				//水泵禁能脚
	char Valve_OFF; 			//水阀禁能脚
	char Compressor_OFF; 		//压缩机禁能脚
	char ElBow_OFF;				//电加热器禁能脚
	char AlarmOUT_OFF; 			//报警信号禁能脚
	char Opticalcoupler1_OFF; 	//光耦输出2禁能脚
	char Opticalcoupler2_OFF; 	//光耦输出2禁能脚
}Perif_Typedef;


/* Exported macro -------------------------------------------------------------*/
#define OutPinCtrl  GPIOA						//默认输出口的控制口在PA口
#define OutPinCtrl_Clk  RCC_APB2Periph_GPIOA	//默认OUT控制口时钟

#define InPinCtrl_L  GPIOC						//默认输入口前半部分PC口
#define InPinCtrl_H  GPIOA						//默认输入口后半部分PA口
#define InPinCtrl_Clk  RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA//默认IN控制口时钟

/** 
  * @引脚定义输出部分
  */
#define Pump_OUT 				GPIO_Pin_4  	//水泵输出脚
#define Valve_OUT 				GPIO_Pin_5  	//水阀输出脚
#define Compressor_OUT 			GPIO_Pin_6  	//压缩机输出脚
#define ElBow_OUT 				GPIO_Pin_7 		//电加热器输出脚
#define Alarm_OUT 				GPIO_Pin_8		//报警信号输出脚
#define Opticalcoupler1_OUT 	GPIO_Pin_9		//光耦输出1输出脚
#define Opticalcoupler2_OUT 	GPIO_Pin_10		//光耦输出2输出脚


#define OutPin Pump_OUT|Valve_OUT|Compressor_OUT|ElBow_OUT|Alarm_OUT|Opticalcoupler1_OUT|Opticalcoupler2_OUT//定义输出

/** 
  * @输入部分
  */
/*GPIOC*/
#define Powerphase_IN 	GPIO_Pin_6		//电源相序输入
#define Compressor_IN 	GPIO_Pin_7  	//压缩机输入
#define Level_IN 		GPIO_Pin_8		//液位输入
#define HighWater_IN 	GPIO_Pin_9		//高温水流量输入
/*GPIOA*/
#define LowWater_IN 	GPIO_Pin_0		//低温水流量输入
#define Pumphigh_IN 	GPIO_Pin_1  	//高温水水泵输入
#define Pumplow_IN 		GPIO_Pin_2		//低温水水泵输入
/*外控输入*/
#define ExCtr_IN 		GPIO_Pin_3		//外控输入PIN_IN14

#define InPin_L Powerphase_IN|Compressor_IN|Level_IN|HighWater_IN	//定义前半部分输入使能
#define InPin_H	LowWater_IN|Pumphigh_IN|Pumplow_IN	//定义后半部分输入使能

/* private------- ------------------------------------------------------------*/
extern Alarm_Typedef Alarm;		//报警标志
extern Perif_Typedef Chiller;	//冷却装置
extern Perif_Typedef SaveChillerNormalPara;//保存冷却装置异常前数据

/* Exported functions ------------------------------------------------------- ---*/
void IOputConfig(void);//输入输出配置
void PerifStop(void);//停止外设工作
void PerifStart(void);//开启外设工作
void InputMonitor(void);//输入监控
void OutPro(void);//输输出监控

#endif

