/**
  *****************************************************************************
  * @file  J12864.h
  * @author  Feng wenyun
  * @version  V1.0.0
  * @date  24/08/2016
  * @brief	J12864Һ������������������غ궨��ͷ�ļ�,
  *			���ݲ������
  *****************************************************************************
  **/ 
#ifndef __J12864_H
#define __J12864_H 
/* Define to prevent recursive inclusion -------------------------------------------*/
#include "stm32f10x.h"
/* Exported constants ----------------------------------------------------------*/
/* Exported macro -------------------------------------------------------------*/
#define LCD_CONTROL  GPIOB//Ĭ��J12864�Ŀ��ƿ���PB��
#define LCD_DATAPORT GPIOB//Ĭ��J12864�����ݿ���PB�ڣ���8λ���

#define LCD_RESET_Pin GPIO_Pin_0 //Ĭ��J12864�ĸ�λ�������ӵ�PB.0 
#define LCD_RS_Pin GPIO_Pin_5  //Ĭ��J12864 RS -- PB.5
#define LCD_RW_Pin GPIO_Pin_6  //Ĭ��J12864 RW -- PB.6
#define LCD_EN_Pin GPIO_Pin_7  //Ĭ��J12864 E  -- PB.7
#define LCD_BF_Pin 15  		//Ĭ��12864æµ����Ž�MCU��GPIO_Pin_15��

#define LCD_CONTROL_CLOCK  RCC_APB2Periph_GPIOB//Ĭ��J12864�Ŀ��ƿ�ʱ��
#define LCD_DATAPORT_CLOCK RCC_APB2Periph_GPIOB//Ĭ��J12864�����ݿ�ʱ��

#define LCD_RS_1 LCD_CONTROL->BSRR &=~LCD_RS_Pin;LCD_CONTROL->BSRR |=LCD_RS_Pin  //RS�øߵ�ƽ
#define LCD_RS_0 LCD_CONTROL->BRR  &=~LCD_RS_Pin;LCD_CONTROL->BRR  |=LCD_RS_Pin  //RS�õ͵�ƽ
#define LCD_RW_1 LCD_CONTROL->BSRR &=~LCD_RW_Pin;LCD_CONTROL->BSRR |=LCD_RW_Pin  //RW�øߵ�ƽ
#define LCD_RW_0 LCD_CONTROL->BRR  &=~LCD_RW_Pin;LCD_CONTROL->BRR  |=LCD_RW_Pin  //RW�õ͵�ƽ
#define LCD_EN_1 LCD_CONTROL->BSRR &=~LCD_EN_Pin;LCD_CONTROL->BSRR |=LCD_EN_Pin  //EN�øߵ�ƽ
#define LCD_EN_0 LCD_CONTROL->BRR  &=~LCD_EN_Pin;LCD_CONTROL->BRR  |=LCD_EN_Pin  //EN�õ͵�ƽ

#define LCD_RESET_0  LCD_CONTROL->BRR  = LCD_RESET_Pin  // ��λ
#define LCD_RESET_1  LCD_CONTROL->BSRR = LCD_RESET_Pin  // ��λ������
#define DATAOUT LCD_DATAPORT->ODR &=0x00ff;LCD_DATAPORT->ODR // ��������Ĵ����߰�λ���
#define DATAIN  LCD_DATAPORT->IDR        				// ��������Ĵ���
#define LCD_BF  ((DATAIN)& 0x8000)        				// æ״̬DB7=1

#define LCD_BF_IN	LCD_DATAPORT->CRH &= ~(3<<(14<<1));LCD_DATAPORT->CRH &= ~(3<<(LCD_BF_Pin<<1))//MODEΪ����ģʽ ģ������
					
#define LCD_BF_OUT	LCD_DATAPORT->CRH |= (3<<(14<<1));LCD_DATAPORT->CRH |= (0<<(LCD_BF_Pin<<1))	//MODEΪ���ģʽ����ٶ�50MHz,ͨ���������

/* Exported functions ------------------------------------------------------- ---*/ 
void LCD_WriteInitCmd(uint16_t initcmd); //д��ʼ������
void LCD_WaitLaisure(void);     //һֱ�ȴ���LCD�ڲ�������ɣ���Ϊ����״̬
void LCD_WriteCmd(uint16_t cmd);    //д���J12864
void LCD_WriteByte(uint16_t byte);  //дһ�ֽ����ݵ�J12864
void LCD_pos(uint16_t pos);     //LCD��ʾλ������
void LCD_Setpos(uint16_t row,uint16_t col);//�趨J12864����ʾ��ַ������ϰ��
void LCD_DispChar(char ch);     //��ʾһ���ַ�
void LCD_Setpos_DispChar(uint16_t row,uint16_t col,char ch);//��ָ��λ����ʾһ���ַ�
void LCD_DispString(char str[]);   //��ʾһ���ַ�������ʾλ������ǰ�趨
void LCD_Setpos_DispString(uint16_t row,uint16_t col,char str[]);//��ָ��λ����ʾһ���ַ���
void LCD_Dispnum(uint32_t num);    //��ʾһ��������8λ����������ʾλ������ǰ����
void LCD_Setpos_Dispnum(uint16_t row,uint16_t col,uint32_t num);//��ָ��λ����ʾһ��������8λ������
void LCD_DispDecimal(uint32_t num,uint16_t dot);//��ʾһ����Чλ������8λ�ĸ�����,��ʾλ����Ҫ��ǰ�趨
void LCD_Setpos_DispDecimal(uint16_t row,uint16_t col,uint32_t num,uint16_t dot);//��ָ��λ����ʾһ����Чλ������8λ�ĸ�����
void LCD_DispDateTime(uint32_t year,uint16_t month,uint16_t day,uint16_t hour,uint16_t min,uint16_t sec);//��ʾ��������ʾ������ʱ��
void LCD_DispPicture(const unsigned char picture[]);   //��ʾһ��ͼ��  128*64
void LCD_ShiftLeft(void);//��Ļ��������һ�����ڹ�����ʾ
void LCD_ShiftRight(void);//��Ļ��������һ�����ڹ�����ʾ
void LCD_Clear(void);//�����������ʾ
void LCD_Return(void);//ʹ��껹��ԭ��λ��
void LCD_Close(void);//����ʾ
void LCD_Open(void);//����ʾ
void LCD_FlickerChar(uint16_t row,uint16_t col);//�ر��ַ���˸
void LCD_CloseFlicker(void);//��Ļ����˸һ��
void LCD_FlickerScreen(void);
void LCD_PORT_Init(void);       //LCD���Ӷ˿ڳ�ʼ��
void LCD_Init(void);        //LCD���ó�ʼ��

#endif

