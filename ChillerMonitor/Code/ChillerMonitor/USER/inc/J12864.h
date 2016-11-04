/**
  *****************************************************************************
  * @file  J12864.h
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	J12864液晶驱动函数声明及相关宏定义头文件,
  *			数据并行输出
  *****************************************************************************
  **/ 
#ifndef __J12864_H
#define __J12864_H 
/* Define to prevent recursive inclusion -------------------------------------------*/
#include "stm32f10x.h"
/* Exported constants ----------------------------------------------------------*/
/* Exported macro -------------------------------------------------------------*/
#define LCD_CONTROL  GPIOB//默认J12864的控制口在PB口
#define LCD_DATAPORT GPIOB//默认J12864的数据口在PB口；高8位输出

#define LCD_RESET_Pin GPIO_Pin_0 //默认J12864的复位引脚连接到PB.0 
#define LCD_RS_Pin GPIO_Pin_5  //默认J12864 RS -- PB.5
#define LCD_RW_Pin GPIO_Pin_6  //默认J12864 RW -- PB.6
#define LCD_EN_Pin GPIO_Pin_7  //默认J12864 E  -- PB.7
#define LCD_BF_Pin 15  		//默认12864忙碌输出脚接MCU的GPIO_Pin_15脚

#define LCD_CONTROL_CLOCK  RCC_APB2Periph_GPIOB//默认J12864的控制口时钟
#define LCD_DATAPORT_CLOCK RCC_APB2Periph_GPIOB//默认J12864的数据口时钟

#define LCD_RS_1 LCD_CONTROL->BSRR &=~LCD_RS_Pin;LCD_CONTROL->BSRR |=LCD_RS_Pin  //RS置高电平
#define LCD_RS_0 LCD_CONTROL->BRR  &=~LCD_RS_Pin;LCD_CONTROL->BRR  |=LCD_RS_Pin  //RS置低电平
#define LCD_RW_1 LCD_CONTROL->BSRR &=~LCD_RW_Pin;LCD_CONTROL->BSRR |=LCD_RW_Pin  //RW置高电平
#define LCD_RW_0 LCD_CONTROL->BRR  &=~LCD_RW_Pin;LCD_CONTROL->BRR  |=LCD_RW_Pin  //RW置低电平
#define LCD_EN_1 LCD_CONTROL->BSRR &=~LCD_EN_Pin;LCD_CONTROL->BSRR |=LCD_EN_Pin  //EN置高电平
#define LCD_EN_0 LCD_CONTROL->BRR  &=~LCD_EN_Pin;LCD_CONTROL->BRR  |=LCD_EN_Pin  //EN置低电平

#define LCD_RESET_0  LCD_CONTROL->BRR  = LCD_RESET_Pin  // 复位
#define LCD_RESET_1  LCD_CONTROL->BSRR = LCD_RESET_Pin  // 复位脚拉高
#define DATAOUT LCD_DATAPORT->ODR &=0x00ff;LCD_DATAPORT->ODR // 数据输出寄存器高八位输出
#define DATAIN  LCD_DATAPORT->IDR        				// 数据输入寄存器
#define LCD_BF  ((DATAIN)& 0x8000)        				// 忙状态DB7=1

#define LCD_BF_IN	LCD_DATAPORT->CRH &= ~(3<<(14<<1));LCD_DATAPORT->CRH &= ~(3<<(LCD_BF_Pin<<1))//MODE为输入模式 模拟输入
					
#define LCD_BF_OUT	LCD_DATAPORT->CRH |= (3<<(14<<1));LCD_DATAPORT->CRH |= (0<<(LCD_BF_Pin<<1))	//MODE为输出模式最大速度50MHz,通用推挽输出

/* Exported functions ------------------------------------------------------- ---*/ 
void LCD_WriteInitCmd(uint16_t initcmd); //写初始化命令
void LCD_WaitLaisure(void);     //一直等待到LCD内部操作完成，变为空闲状态
void LCD_WriteCmd(uint16_t cmd);    //写命令到J12864
void LCD_WriteByte(uint16_t byte);  //写一字节数据到J12864
void LCD_pos(uint16_t pos);     //LCD显示位置设置
void LCD_Setpos(uint16_t row,uint16_t col);//设定J12864的显示地址，根据习惯
void LCD_DispChar(char ch);     //显示一个字符
void LCD_Setpos_DispChar(uint16_t row,uint16_t col,char ch);//在指定位置显示一个字符
void LCD_DispString(char str[]);   //显示一个字符串，显示位置需提前设定
void LCD_Setpos_DispString(uint16_t row,uint16_t col,char str[]);//在指定位置显示一个字符串
void LCD_Dispnum(uint32_t num);    //显示一个不超过8位的整数，显示位置需提前设置
void LCD_Setpos_Dispnum(uint16_t row,uint16_t col,uint32_t num);//在指定位置显示一个不超过8位的整数
void LCD_DispDecimal(uint32_t num,uint16_t dot);//显示一个有效位不超过8位的浮点数,显示位置需要提前设定
void LCD_Setpos_DispDecimal(uint16_t row,uint16_t col,uint32_t num,uint16_t dot);//在指定位置显示一个有效位不超过8位的浮点数
void LCD_DispDateTime(uint32_t year,uint16_t month,uint16_t day,uint16_t hour,uint16_t min,uint16_t sec);//显示日历，显示日期与时间
void LCD_DispPicture(const unsigned char picture[]);   //显示一幅图像  128*64
void LCD_ShiftLeft(void);//屏幕整体左移一格，用于滚动显示
void LCD_ShiftRight(void);//屏幕整体右移一格，用于滚动显示
void LCD_Clear(void);//清屏，清除显示
void LCD_Return(void);//使光标还回原点位置
void LCD_Close(void);//关显示
void LCD_Open(void);//开显示
void LCD_FlickerChar(uint16_t row,uint16_t col);//关闭字符闪烁
void LCD_CloseFlicker(void);//屏幕秒闪烁一次
void LCD_FlickerScreen(void);
void LCD_PORT_Init(void);       //LCD连接端口初始化
void LCD_Init(void);        //LCD配置初始化

#endif

