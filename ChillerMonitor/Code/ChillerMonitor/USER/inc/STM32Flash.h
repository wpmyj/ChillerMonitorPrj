
#ifndef __STM32FLASH_H
#define __STM32FLASH_H
#include "stm32f10x.h"
/*****************************************************************************************************/
#define STM32_FLASH_SIZE 256 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
#define STM_SECTOR_SIZE	2048
/*****************************************************************************************************/

extern u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�
extern u16 Config_Data[];//����ϵͳ����,����Ϊ���Ʒ�ʽ����������ˮ������ˮ�������¶ȡ������¶ȵȲ���
/* Exported functions ------------------------------------------------------- ---*/ 
u16 STMFLASH_ReadHalfWord(u32 faddr);		  						//��������  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);			//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
void StoreSysPrama(void);		//�洢ϵͳ����	
void GetSysPrama(void);		//���ϵͳ����	
#endif











