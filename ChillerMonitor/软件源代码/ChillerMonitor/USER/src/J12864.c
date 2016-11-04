/**
  *****************************************************************************
  * @file  J12864.c
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	J12864液晶驱动函数.
  *****************************************************************************
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "J12864.h"
#include "Delay_Timer.h"
/**
  * @brief  写初始化命令
  * @param None
  * @retval : None
  */
void LCD_WriteInitCmd(uint16_t initcmd){
	LCD_RS_0;
	LCD_RW_0;
	Delayus(1);
	LCD_EN_1;
	Delayus(1);
	DATAOUT |= (initcmd<<8);
	Delayus(800);
	LCD_EN_0;
	LCD_RW_1;
	Delayus(1);
}
/**
  * @brief  等待J12864内部操作完成
  * @param None
  * @retval : None
  */
void LCD_WaitLaisure(void){
	static uint16_t flag=0x00;
	LCD_BF_IN;//MCU与12864相连的BF脚切换成输入模式
	Delayus(1);
	LCD_RS_0;
	LCD_RW_1;
	Delayus(1);
	do{
		LCD_EN_0;
		Delayus(500);
		LCD_EN_1;
		Delayus(500);
		flag=(uint16_t)LCD_BF;
	}while(flag);
	LCD_BF_OUT;//MCU与12864相连的BF脚切换成输出模式
	Delayus(1);
}
/**
  * @brief  写命令到J12864
  * @param None
  * @retval : None
  */

void LCD_WriteCmd(uint16_t cmd){
	LCD_WaitLaisure();
	LCD_RS_0;
	LCD_RW_0;
	Delayus(1);
	LCD_EN_1;
	Delayus(1);
	DATAOUT |= (cmd<<8);
	Delayus(800); //经过测试，取600以上。550都不可以
	LCD_EN_0;
	LCD_RW_1;
	Delayus(1);
}
/**
  * @brief  写一字节数据到J12864
  * @param None
  * @retval : None
  */
void LCD_WriteByte(uint16_t byte){
	LCD_WaitLaisure();
	LCD_RS_1;
	LCD_RW_0;
	Delayus(1);
	LCD_EN_1;
	Delayus(1);
	DATAOUT |= (byte<<8);
	Delayus(800);
	LCD_EN_0;
	LCD_RW_1;
	Delayus(1);
	DATAOUT |= (0<<8);
}
/**
  * @brief  LCD显示位置设置
  * @param None
  * @retval : None
  */

void LCD_pos(uint16_t pos){
	LCD_WriteCmd(pos);
}
/**
  * @brief  设定J12864的显示地址，根据习惯
  * @param None
  * @retval : None
  */
void LCD_Setpos(uint16_t row,uint16_t col){
	//uint16_t pos=0x00;
	uint16_t pos;
	pos=0x00;
	if(row+1>3)
		pos=0x08;
	if(row & 0x01)
		pos |= 0x80;
	else
		pos |= 0x90;
	pos += col-1;
	LCD_WriteCmd(pos);
}
/**
  * @brief  显示一个字符
  * @param None
  * @retval : None
  */
void LCD_DispChar(char ch){
	LCD_WriteByte(ch);
}
/**
  * @brief  在指定位置显示一个字符
  * @param None
  * @retval : None
  */
void LCD_Setpos_DispChar(uint16_t row,uint16_t col,char ch){
	LCD_Setpos(row,col);
	LCD_WriteByte(ch);
}
/**
  * @brief  显示一个字符串，显示位置需提前设定
  * @param None
  * @retval : None
  */
void LCD_DispString(char str[]){
	static uint16_t i;
	i=0;
	for(i=0;str[i]!='\0';i++)
		LCD_WriteByte(str[i]);
}
/**
  * @brief  在指定位置显示一个字符串
  * @param None
  * @retval : None
  */

void LCD_Setpos_DispString(uint16_t row,uint16_t col,char str[]){
	LCD_Setpos(row,col);
	LCD_DispString(str);
}
/**
  * @brief  显示一个不超过8位的整数，显示位置需提前设置
  * @param None
  * @retval : None
  */

void LCD_Dispnum(uint32_t num)
{
   char str[12];
   uint32_t q;
   uint16_t wei,i,j,k;
   if(num>=10000000)wei=8;
   else if(num>=1000000)wei=7;
   else if(num>=100000)wei=6;
   else if(num>=10000)wei=5;
   else if(num>=1000) wei=4;
   else if(num>=100)wei=3;
   else if(num>=10) wei=2;
   else wei=1;
   k=0;
   for(i=wei;i>0;i--)
   {   q=1;
       j=1; //i=1时，q=1,得到个位
    for(;j<i;j++)q *=10;
    str[k++]=num/q +'0';
    num %= q;
   }
   str[k] = '\0'; //添加字符串结束标志
   LCD_DispString(str);//显示字符串
}
/**
  * @brief  显示一个字符串，显示位置需提前设定
  * @param None
  * @retval : None
  */

void LCD_Setpos_Dispnum(uint16_t row,uint16_t col,uint32_t num)
{
   LCD_Setpos(row,col); 
   LCD_Dispnum(num);
}
/**
  * @brief  显示一个有效位不超过8位的浮点数,显示位置需要提前设定
  * @param None
  * @retval : None
  */
void LCD_DispDecimal(uint32_t num,uint16_t dot){
	//static uint32_t i=0,j,k=0,wei;//static变量只会初始化一次，第二次调用函数不会初始化这些变量
	static uint32_t i,j,k,wei;//static变量只会初始化一次，第二次调用函数不会初始化这些变量
	static uint32_t q;
	char str[10];
	i=0;
	j=0;
	k=0;
	wei=0;
	q=0;
	if(num>=10000000)
		wei=8;
	else if(num>=1000000)
		wei=7;
	else if(num>=100000)
		wei=6;
	else if(num>=10000)
		wei=5;
	else if(num>=1000)
		wei=4;
	else if(num>=100)
		wei=3;
	else if(num>=10)
		wei=2;
	else 
		wei=1;
	for(i=wei;i>0;i--){
		q=1;
		j=1; //i=1时，q=1,得到个位
		for(;j<i;j++)
			q *=10;
		str[k++]=num/q +'0';
		num %= q;
	}
	//str[k] = '\0'; //添加字符串结束标志
	for(i=8;i>0;i--){
		if((str[i]>='0')&&(str[i]<='9')) 
			break;
	}
	i=wei-1;
	//str[i]='\0';   //添加字符串结束符
	for(j=0;j<dot;j++,i--){//小数点后各位依次右移，准备插入
		str[i+1]=str[i];
	}
	str[wei-dot]='.';    //插入小数点
	str[wei+1]='\0';   //添加字符串结束符
	LCD_DispString(str);//显示浮点小数
}
/**
  * @brief  在指定位置显示一个有效位不超过8位的浮点数
  * @param None
  * @retval : None
  */

void LCD_Setpos_DispDecimal(uint16_t row,uint16_t col,uint32_t num,uint16_t dot){
	LCD_Setpos(row,col);
	LCD_DispDecimal(num,dot);
}

/**
  * @brief  显示日历，显示日期与时间
  * @param None
  * @retval : None
  */
void LCD_DispDateTime(uint32_t year,uint16_t month,uint16_t day,uint16_t hour,uint16_t min,uint16_t sec){
	//extern uint16_t enableclock;
	LCD_Setpos(1,0);
	LCD_DispString("Date:");
	LCD_Dispnum((uint32_t)year);
	LCD_DispChar('-');
	LCD_Dispnum((uint32_t)month);
	LCD_DispChar('-');
	LCD_Dispnum((uint32_t)day);
	if((month<10)&&(day<10))    //清除无效位置可能残留的显示
	{
		LCD_DispChar(' ');
		LCD_DispChar(' ');
	}
	else if(((month<10)&&(day>9))||((month>9)&&(day<10)))
		LCD_DispChar(' ');
		LCD_Setpos(1,15);
	//if(enableclock)      //如果开启闹铃，显示闹铃标志
	//  LCD_DispChar('@');
	//else
	LCD_DispChar('*'); //第一行结束符显示
	LCD_Setpos(2,0);
	LCD_DispString("Time:");
	LCD_Dispnum((uint32_t)hour);
	LCD_DispChar(':');
	LCD_Dispnum((uint32_t)min);
	LCD_DispChar(':');
	LCD_Dispnum((uint32_t)sec);
	if(sec==0) 
		LCD_DispChar('0');
	LCD_DispChar(' ');
	LCD_DispChar(' ');
	if((hour<10)&&(min<10)&&(sec>0)&&(sec<10))
		LCD_DispChar(' '); //用于消隐
	LCD_Setpos(2,15);
	//if(enableclock)
	//  LCD_DispChar('@');
	//else
	LCD_DispChar('*');//第二行结束符显示
}

/**
  * @brief  显示一幅图像  128*64
  * @param None
  * @retval : None
  */
void LCD_DispPicture(const unsigned char picture[]){
	static uint16_t i,j;
	LCD_WriteCmd(0x34); //写数据时,关闭图形显示
	Delayms(20);
	for(i=0;i<32;i++) //32行
	{
		LCD_WriteCmd(0x80+i);
		Delayms(150);
		LCD_WriteCmd(0x80);
		Delayms(150);
		for(j=0;j<16;j++) //一行，16个字节
		{
			LCD_WriteByte(picture[j]);
			Delayms(150);
		}
		Delayms(150);
	}    
	LCD_WriteCmd(0x36);  //写完数据,开图形显示
	Delayms(50);
}
/**
  * @brief  屏幕整体左移一格，用于滚动显示
  * @param None
  * @retval : None
  */
void LCD_ShiftLeft()
{
	LCD_WriteCmd(0x18);
}
/**
  * @brief  屏幕整体右移一格，用于滚动显示
  * @param None
  * @retval : None
  */
void LCD_ShiftRight(void)
{
 	LCD_WriteCmd(0x1C);
}
/**
  * @brief  清屏，清除显示
  * @param None
  * @retval : None
  */
void LCD_Clear(void){
	LCD_WriteCmd(0x01);
}
/**
  * @brief 使光标还回原点位置
  * @param None
  * @retval : None
  */
void LCD_Return(void){
	LCD_WriteCmd(0x02);
}
/**
  * @brief  关显示
  * @param None
  * @retval : None
  */
void LCD_Close(void){
	LCD_WriteCmd(0x08);
}
/**
  * @brief  开显示
  * @param None
  * @retval : None
  */
void LCD_Open(void){
	LCD_WriteCmd(0x0C);
}
/**
  * @brief  关闭字符闪烁
  * @param None
  * @retval : None
  */
void LCD_FlickerChar(uint16_t row,uint16_t col){
	LCD_WriteCmd(0x0D);
	LCD_Setpos(row,col);
}
/**
  * @brief  关闭字符闪烁
  * @param None
  * @retval : None
  */
void LCD_CloseFlicker(void){
	LCD_WriteCmd(0x0C);
}
/**
  * @brief  屏幕秒闪烁一次
  * @param None
  * @retval : None
  */
void LCD_FlickerScreen(void){
	LCD_WriteCmd(0x08);//关显示
	Delayms(500);
	LCD_WriteCmd(0x0C);//开显示
	Delayms(500);
}
/**
  * @brief  LCD连接端口初始化
  * @param None
  * @retval : None
  */
void LCD_PORT_Init(void){
	GPIO_InitTypeDef LCD_port;
	RCC_APB2PeriphClockCmd(LCD_CONTROL_CLOCK ,ENABLE); //时钟使能
	RCC_APB2PeriphClockCmd(LCD_DATAPORT_CLOCK ,ENABLE);
	/* 配置并行数据端口 -------------------------------------*/
	LCD_port.GPIO_Pin  = (uint16_t)0xff00;//选择高8位作为数据输出
	LCD_port.GPIO_Speed= GPIO_Speed_50MHz;
//	LCD_port.GPIO_Mode = GPIO_Mode_Out_OD;//OD开路输出，外接上拉电阻到5V
	LCD_port.GPIO_Mode = GPIO_Mode_Out_PP;//PP推挽输出
	GPIO_Init(LCD_DATAPORT,&LCD_port);
	/* 配置J12864控制端口 -------------------------------------*/
	LCD_port.GPIO_Pin  |= (LCD_RESET_Pin | LCD_RS_Pin | LCD_RW_Pin | LCD_EN_Pin);
	LCD_port.GPIO_Speed= GPIO_Speed_50MHz;
//	LCD_port.GPIO_Mode = GPIO_Mode_Out_OD;//OD开路输出
	LCD_port.GPIO_Mode = GPIO_Mode_Out_PP;//PP推挽输出
	GPIO_Init(LCD_CONTROL,&LCD_port);
}
/**
  * @brief  LCD配置初始化
  * @param None
  * @retval : None
  */
void LCD_Init(void){
	LCD_RESET_0;
	Delayms(300);   //复位J12864
	LCD_RESET_1;
	Delayms(300);
	LCD_WriteInitCmd(0x30);        //选择扩充指令集
	LCD_WriteInitCmd(0x0C);        //开显示(无游标、不反白)
	LCD_WriteInitCmd(0x01);        //清除显示，并且设定地址指针为00H
	LCD_WriteInitCmd(0x02);			//DRAM地址归位
	LCD_WriteInitCmd(0x06);        //指定在资料的读取及写入时，设定游标的移动方向及指定显示的移位
}


