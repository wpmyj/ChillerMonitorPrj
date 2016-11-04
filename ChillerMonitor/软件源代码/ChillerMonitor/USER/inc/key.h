#ifndef __KEY_H
#define	__KEY_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @按键按下标置
  * @KEY_ON 0, KEY_OFF 1
  */

#define KEY_ON	0
#define KEY_OFF	1

/** 
  * @定义三个按键的接口
  */
#define Key_set		GPIO_Pin_12	//设置键
#define Key_up 		GPIO_Pin_11	//数值加键
#define Key_down 	GPIO_Pin_10	//数值减键
#define Key_OnOf	GPIO_Pin_2	//进入操作模式

//#define Key_down GPIO_Pin_10//数值减键
typedef struct
{
	char Model_flag;
	char Set_flag;
	char Up_flag;
	char Down_flag;
	char MenuSelect;//选择显示页面
	char Model_count;//模式计数
	char MenuSetFlag;//进入工作界面标志
}Key_Typedef;
/** 
  * @定义冷却到正常情况下高温低温水设定值与实际值得差值边界
  */

typedef struct
{
	char LowW;
	char HighW;
}DValue_Typedef;

/* private------- ------------------------------------------------------------*/
extern Key_Typedef Key;//定义按键数据结构
extern DValue_Typedef DValue;
extern u16 TemperaSet[7];//定义温度数组
extern u16* ptemper;//定义一些变量



void Key_GPIO_Config(void);
u8 Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);//按键扫描
void Set_Pro(void);//按键设置处理
void Key_Process(void);//按键处理

#endif /* __LED_H */

