
#ifndef __STM32FLASH_H
#define __STM32FLASH_H
#include "stm32f10x.h"
/*****************************************************************************************************/
#define STM32_FLASH_SIZE 256 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
#define STM_SECTOR_SIZE	2048
/*****************************************************************************************************/

extern u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
extern u16 Config_Data[];//配置系统参数,依次为控制方式参数、低温水、高温水、下限温度、上限温度等参数
/* Exported functions ------------------------------------------------------- ---*/ 
u16 STMFLASH_ReadHalfWord(u32 faddr);		  						//读出半字  
void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//指定地址开始写入指定长度的数据
u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//指定地址开始读取指定长度数据
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);			//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据
void StoreSysPrama(void);		//存储系统参数	
void GetSysPrama(void);		//获得系统参数	
#endif











