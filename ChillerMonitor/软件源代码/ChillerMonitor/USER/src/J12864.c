/**
  *****************************************************************************
  * @file  J12864.c
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	J12864Һ����������.
  *****************************************************************************
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "J12864.h"
#include "Delay_Timer.h"
/**
  * @brief  д��ʼ������
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
  * @brief  �ȴ�J12864�ڲ��������
  * @param None
  * @retval : None
  */
void LCD_WaitLaisure(void){
	static uint16_t flag=0x00;
	LCD_BF_IN;//MCU��12864������BF���л�������ģʽ
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
	LCD_BF_OUT;//MCU��12864������BF���л������ģʽ
	Delayus(1);
}
/**
  * @brief  д���J12864
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
	Delayus(800); //�������ԣ�ȡ600���ϡ�550��������
	LCD_EN_0;
	LCD_RW_1;
	Delayus(1);
}
/**
  * @brief  дһ�ֽ����ݵ�J12864
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
  * @brief  LCD��ʾλ������
  * @param None
  * @retval : None
  */

void LCD_pos(uint16_t pos){
	LCD_WriteCmd(pos);
}
/**
  * @brief  �趨J12864����ʾ��ַ������ϰ��
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
  * @brief  ��ʾһ���ַ�
  * @param None
  * @retval : None
  */
void LCD_DispChar(char ch){
	LCD_WriteByte(ch);
}
/**
  * @brief  ��ָ��λ����ʾһ���ַ�
  * @param None
  * @retval : None
  */
void LCD_Setpos_DispChar(uint16_t row,uint16_t col,char ch){
	LCD_Setpos(row,col);
	LCD_WriteByte(ch);
}
/**
  * @brief  ��ʾһ���ַ�������ʾλ������ǰ�趨
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
  * @brief  ��ָ��λ����ʾһ���ַ���
  * @param None
  * @retval : None
  */

void LCD_Setpos_DispString(uint16_t row,uint16_t col,char str[]){
	LCD_Setpos(row,col);
	LCD_DispString(str);
}
/**
  * @brief  ��ʾһ��������8λ����������ʾλ������ǰ����
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
       j=1; //i=1ʱ��q=1,�õ���λ
    for(;j<i;j++)q *=10;
    str[k++]=num/q +'0';
    num %= q;
   }
   str[k] = '\0'; //����ַ���������־
   LCD_DispString(str);//��ʾ�ַ���
}
/**
  * @brief  ��ʾһ���ַ�������ʾλ������ǰ�趨
  * @param None
  * @retval : None
  */

void LCD_Setpos_Dispnum(uint16_t row,uint16_t col,uint32_t num)
{
   LCD_Setpos(row,col); 
   LCD_Dispnum(num);
}
/**
  * @brief  ��ʾһ����Чλ������8λ�ĸ�����,��ʾλ����Ҫ��ǰ�趨
  * @param None
  * @retval : None
  */
void LCD_DispDecimal(uint32_t num,uint16_t dot){
	//static uint32_t i=0,j,k=0,wei;//static����ֻ���ʼ��һ�Σ��ڶ��ε��ú��������ʼ����Щ����
	static uint32_t i,j,k,wei;//static����ֻ���ʼ��һ�Σ��ڶ��ε��ú��������ʼ����Щ����
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
		j=1; //i=1ʱ��q=1,�õ���λ
		for(;j<i;j++)
			q *=10;
		str[k++]=num/q +'0';
		num %= q;
	}
	//str[k] = '\0'; //����ַ���������־
	for(i=8;i>0;i--){
		if((str[i]>='0')&&(str[i]<='9')) 
			break;
	}
	i=wei-1;
	//str[i]='\0';   //����ַ���������
	for(j=0;j<dot;j++,i--){//С������λ�������ƣ�׼������
		str[i+1]=str[i];
	}
	str[wei-dot]='.';    //����С����
	str[wei+1]='\0';   //����ַ���������
	LCD_DispString(str);//��ʾ����С��
}
/**
  * @brief  ��ָ��λ����ʾһ����Чλ������8λ�ĸ�����
  * @param None
  * @retval : None
  */

void LCD_Setpos_DispDecimal(uint16_t row,uint16_t col,uint32_t num,uint16_t dot){
	LCD_Setpos(row,col);
	LCD_DispDecimal(num,dot);
}

/**
  * @brief  ��ʾ��������ʾ������ʱ��
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
	if((month<10)&&(day<10))    //�����Чλ�ÿ��ܲ�������ʾ
	{
		LCD_DispChar(' ');
		LCD_DispChar(' ');
	}
	else if(((month<10)&&(day>9))||((month>9)&&(day<10)))
		LCD_DispChar(' ');
		LCD_Setpos(1,15);
	//if(enableclock)      //����������壬��ʾ�����־
	//  LCD_DispChar('@');
	//else
	LCD_DispChar('*'); //��һ�н�������ʾ
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
		LCD_DispChar(' '); //��������
	LCD_Setpos(2,15);
	//if(enableclock)
	//  LCD_DispChar('@');
	//else
	LCD_DispChar('*');//�ڶ��н�������ʾ
}

/**
  * @brief  ��ʾһ��ͼ��  128*64
  * @param None
  * @retval : None
  */
void LCD_DispPicture(const unsigned char picture[]){
	static uint16_t i,j;
	LCD_WriteCmd(0x34); //д����ʱ,�ر�ͼ����ʾ
	Delayms(20);
	for(i=0;i<32;i++) //32��
	{
		LCD_WriteCmd(0x80+i);
		Delayms(150);
		LCD_WriteCmd(0x80);
		Delayms(150);
		for(j=0;j<16;j++) //һ�У�16���ֽ�
		{
			LCD_WriteByte(picture[j]);
			Delayms(150);
		}
		Delayms(150);
	}    
	LCD_WriteCmd(0x36);  //д������,��ͼ����ʾ
	Delayms(50);
}
/**
  * @brief  ��Ļ��������һ�����ڹ�����ʾ
  * @param None
  * @retval : None
  */
void LCD_ShiftLeft()
{
	LCD_WriteCmd(0x18);
}
/**
  * @brief  ��Ļ��������һ�����ڹ�����ʾ
  * @param None
  * @retval : None
  */
void LCD_ShiftRight(void)
{
 	LCD_WriteCmd(0x1C);
}
/**
  * @brief  �����������ʾ
  * @param None
  * @retval : None
  */
void LCD_Clear(void){
	LCD_WriteCmd(0x01);
}
/**
  * @brief ʹ��껹��ԭ��λ��
  * @param None
  * @retval : None
  */
void LCD_Return(void){
	LCD_WriteCmd(0x02);
}
/**
  * @brief  ����ʾ
  * @param None
  * @retval : None
  */
void LCD_Close(void){
	LCD_WriteCmd(0x08);
}
/**
  * @brief  ����ʾ
  * @param None
  * @retval : None
  */
void LCD_Open(void){
	LCD_WriteCmd(0x0C);
}
/**
  * @brief  �ر��ַ���˸
  * @param None
  * @retval : None
  */
void LCD_FlickerChar(uint16_t row,uint16_t col){
	LCD_WriteCmd(0x0D);
	LCD_Setpos(row,col);
}
/**
  * @brief  �ر��ַ���˸
  * @param None
  * @retval : None
  */
void LCD_CloseFlicker(void){
	LCD_WriteCmd(0x0C);
}
/**
  * @brief  ��Ļ����˸һ��
  * @param None
  * @retval : None
  */
void LCD_FlickerScreen(void){
	LCD_WriteCmd(0x08);//����ʾ
	Delayms(500);
	LCD_WriteCmd(0x0C);//����ʾ
	Delayms(500);
}
/**
  * @brief  LCD���Ӷ˿ڳ�ʼ��
  * @param None
  * @retval : None
  */
void LCD_PORT_Init(void){
	GPIO_InitTypeDef LCD_port;
	RCC_APB2PeriphClockCmd(LCD_CONTROL_CLOCK ,ENABLE); //ʱ��ʹ��
	RCC_APB2PeriphClockCmd(LCD_DATAPORT_CLOCK ,ENABLE);
	/* ���ò������ݶ˿� -------------------------------------*/
	LCD_port.GPIO_Pin  = (uint16_t)0xff00;//ѡ���8λ��Ϊ�������
	LCD_port.GPIO_Speed= GPIO_Speed_50MHz;
//	LCD_port.GPIO_Mode = GPIO_Mode_Out_OD;//OD��·���������������赽5V
	LCD_port.GPIO_Mode = GPIO_Mode_Out_PP;//PP�������
	GPIO_Init(LCD_DATAPORT,&LCD_port);
	/* ����J12864���ƶ˿� -------------------------------------*/
	LCD_port.GPIO_Pin  |= (LCD_RESET_Pin | LCD_RS_Pin | LCD_RW_Pin | LCD_EN_Pin);
	LCD_port.GPIO_Speed= GPIO_Speed_50MHz;
//	LCD_port.GPIO_Mode = GPIO_Mode_Out_OD;//OD��·���
	LCD_port.GPIO_Mode = GPIO_Mode_Out_PP;//PP�������
	GPIO_Init(LCD_CONTROL,&LCD_port);
}
/**
  * @brief  LCD���ó�ʼ��
  * @param None
  * @retval : None
  */
void LCD_Init(void){
	LCD_RESET_0;
	Delayms(300);   //��λJ12864
	LCD_RESET_1;
	Delayms(300);
	LCD_WriteInitCmd(0x30);        //ѡ������ָ�
	LCD_WriteInitCmd(0x0C);        //����ʾ(���αꡢ������)
	LCD_WriteInitCmd(0x01);        //�����ʾ�������趨��ַָ��Ϊ00H
	LCD_WriteInitCmd(0x02);			//DRAM��ַ��λ
	LCD_WriteInitCmd(0x06);        //ָ�������ϵĶ�ȡ��д��ʱ���趨�α���ƶ�����ָ����ʾ����λ
}


