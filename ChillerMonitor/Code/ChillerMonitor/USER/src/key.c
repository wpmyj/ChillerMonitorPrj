/**
  *****************************************************************************
  * @file  key.c
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	按键应用函数库        .
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
Key_Typedef Key={0,0,0,0,0,1,0};//初始化按键标志
DValue_Typedef DValue={20,20};//设定初始差值
u16 TemperaSet[7]={220,280,100,450,0,0,0};//初始化温度数据,依次为低温,高温,下限,上限报警的预设值
u16 *ptemper=&TemperaSet[0];//设置一指针指向温度数组
static char Set3SecFlag=0;//设置键长按3秒标志位
/**
  * @brief	配置按键用到的I/O口 
  * @param None
  * @retval : None
  */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure,OnOf;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //开启按键端口的时钟
	GPIO_InitStructure.GPIO_Pin = (Key_set|Key_up|Key_down); 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //端口配置为上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);	//初始化端口

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE); //开启按键端口的时钟
	OnOf.GPIO_Pin = Key_OnOf; 
	OnOf.GPIO_Speed = GPIO_Speed_10MHz; 
	OnOf.GPIO_Mode = GPIO_Mode_IPU; //端口配置为上拉输入
	GPIO_Init(GPIOD, &OnOf);	//初始化端口
}
 /**
   * @brief  检测是否有按键按下，并判断是哪个按键
   * @param None
   * @retval : None
   */
u8 Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin)
{			
	uint16_t Key_judge;
	/*检测是否有按键按下 */
	if((GPIO_ReadInputData(GPIOx)&(GPIO_Pin))!=(GPIO_Pin))
	{
		/*延时消抖*/
		Delayms(20);
		if((GPIO_ReadInputData(GPIOx)&(GPIO_Pin))!=(GPIO_Pin))
		{
			Key_judge=(GPIO_ReadInputData(GPIOx)&(GPIO_Pin));
			switch(Key_judge)
			{
				/*判断是否为设置键*/
				case (Key_up|Key_down)://如果等于UP键和DOWN键只和的值那么说明set键被按下
					if(GPIO_ReadInputDataBit(GPIOx,Key_set) == KEY_ON)
					{
						Key.Set_flag=1;
						LongKeyTime=0;			//清除计数
						Set3SecFlag=1;
					}
					Key.Up_flag=0;			//在按了设置键的时候清除上下键标志
					Key.Down_flag=0;		//在按了设置键的时候清除上下键标志
					break;
				/*判断是否为加键*/
				case (Key_set|Key_down):	//如果等于SET键和DOWN键只和的值那么说明UP键被按下
						Key.Up_flag=1;
						Key.Set_flag=0;		//清除设置键
						Set3SecFlag=0;//清除该标志位
					break;
				/*判断是否为减键*/
				case (Key_set|Key_up):		//如果等于UP键和SET键只和的值那么说明DOWN键被按下
						Key.Down_flag=1;
						Key.Set_flag=0;		//清除设置键
						Set3SecFlag=0;//清除该标志位
					break;
				default:
					break;
			}
			/*等待按键释放 */
			while((GPIO_ReadInputData(GPIOx)&(GPIO_Pin))!=(GPIO_Pin))
			{

				if(Set3SecFlag==1)//判断SET键是否为长按
				{
					if(LongKeyTime>=20)//判断SET键是否为长按
					{
						Set3SecFlag=0;//清除该标志位
						Key.Model_flag++;
						if(Key.Model_flag>=2)
						{
							Key.Model_flag=0;
							Key.MenuSelect=1;
						}
						LongKeyTime=0;			//清除计数
					}
				}
				Set_Pro();
				Dis_Menu(Key.MenuSelect);	//显示更新
			}
			return 	KEY_ON; 				//有按键按下
		}
		else
			return KEY_OFF;					//无按键按下
	}
	else
		return KEY_OFF;						//无按键按下
}
 /**
   * @brief  显示设置处理
   * @param None
   * @retval : None
   */
 void Set_Pro(void)
 {
	static char i=0;
	if(Key.Model_flag==1)//是否进入设置模式
	{					
		if(Key.Set_flag==1)
		{
			Key.Set_flag=0;
			Key.MenuSelect++;
			if(Key.MenuSelect>=10)//按键调节的界面有9个
				Key.MenuSelect=2;
		}
		if((Key.Up_flag==1)||(Key.Down_flag==1))//按键加减计数处理
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
				*@确定各个温度的设定范围
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
			StoreSysPrama();	//保存设置值
		}
	}
 }
 /**
   * @brief  按键输入处理
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
			if(Key.MenuSetFlag>=3)//Key.MenuSetFlag只有三个值1,2加上初始化的0值
				Key.MenuSetFlag=1;
		}
		while(GPIO_ReadInputDataBit(GPIOD,Key_OnOf) == KEY_ON);//等待按键释放
	}
	 if(Key.MenuSetFlag==1)
	 {
		if( Key_Scan(GPIOC,(Key_set|Key_up |Key_down)) == KEY_ON  )//判断是否有键按下
		{
			Set_Pro();
		}
	 }
}
 
