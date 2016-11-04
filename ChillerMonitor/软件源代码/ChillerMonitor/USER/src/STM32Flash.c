#include "STM32Flash.h"
#include "stm32f10x_flash.h"
#include "Delay_Timer.h"
#include "stm32f10x.h"
#include "key.h"

/* private------- ------------------------------------------------------------*/
u16 Config_Data[] = {1,0,220,280,100,450,0,0,0};//����ϵͳ����,����Ϊ���Ʒ�ʽ����������ѡ�񡢵���ˮ������ˮ�������¶ȡ������¶ȵȲ���
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�

#define SIZE sizeof(Config_Data)	 	//���鳤��
#define FLASH_SAVE_ADDR  0x0803f800 	//����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000),���һҳ�洢��ַ
/**
  * @brief	��ȡָ����ַ�İ���(16λ����)
  * @param faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
  * @retval : ����ֵ:��Ӧ����.
  */

u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}  
/**
  * @brief	������д��
  * @param 	WriteAddr:��ʼ��ַ,
  				pBuffer:����ָ��,
  				NumToWrite:����(16λ)�� 
  * @retval : ��.
  */

void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
} 
/**
  * @brief	��ָ����ַ��ʼд��ָ�����ȵ�����
  * @param 	WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!),
  				pBuffer:����ָ��,
  				NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
  * @retval : ��.
  */

void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	if((WriteAddr<STM32_FLASH_BASE)||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return;		//�Ƿ���ַ
	FLASH_Unlock();	//����
	offaddr=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)
		secremain=NumToWrite;				//�����ڸ�������Χ
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)
				break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������  
		}
		else 
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumToWrite==secremain)
			break;//д�������
		else//д��δ����
		{
			secpos++;							//������ַ��1
			secoff=0;							//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  				//ָ��ƫ��
			WriteAddr+=secremain;				//д��ַƫ��	   
		   	NumToWrite-=secremain;				//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/2))
				secremain=STM_SECTOR_SIZE/2;	//��һ����������д����
			else 
				secremain=NumToWrite;			//��һ����������д����
		}	 
	};	
	FLASH_Lock();//����
}
/**
  * @brief	��ָ����ַ��ʼ����ָ�����ȵ�����
  * @param 	ReadAddr:��ʼ��ַ
  				pBuffer:����ָ��
  				NumToRead:����(16λ)����
  * @retval : ��.
  */

void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);	//��ȡ2���ֽ�.
		ReadAddr+=2;								//ƫ��2���ֽ�.	
	}
}
/**
  * @brief	�洢ϵͳ����
  * @param 	
  * @retval : ��.
  */

void StoreSysPrama(void)
{
	Config_Data[0]=(u16)Key.Model_count;//����ģʽ����
	Config_Data[2]=TemperaSet[0];//�����������ֵ
	Config_Data[3]=TemperaSet[1];//�����������ֵ
	Config_Data[4]=TemperaSet[2];//���������¶�����ֵ
	Config_Data[5]=TemperaSet[3];//���������¶�����ֵ
	Config_Data[6]=TemperaSet[4];//���滷���¶�ϵ��
	Config_Data[7]=TemperaSet[5];//�������ˮ�¶�ϵ��
	Config_Data[8]=TemperaSet[6];//�������ˮ�¶�ϵ��
	STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)Config_Data,SIZE);
}

/**
  * @brief	���ϵͳ����
  * @param 	
  * @retval : ��.
  */

void GetSysPrama(void)
{
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)Config_Data,SIZE);
	Key.Model_count=(char)Config_Data[0];//����ģʽ����
	TemperaSet[0]=Config_Data[2];//�����������ֵ
	TemperaSet[1]=Config_Data[3];//�����������ֵ
	TemperaSet[2]=Config_Data[4];//���������¶�����ֵ
	TemperaSet[3]=Config_Data[5];//���������¶�����ֵ
	TemperaSet[4]=Config_Data[6];//��ȡ�����¶�ϵ��
	TemperaSet[5]=Config_Data[7];//��ȡ����ˮ�¶�ϵ��
	TemperaSet[6]=Config_Data[8];//��ȡ����ˮ�¶�ϵ��
}










