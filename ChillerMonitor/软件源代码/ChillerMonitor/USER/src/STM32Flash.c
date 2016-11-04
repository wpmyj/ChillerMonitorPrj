#include "STM32Flash.h"
#include "stm32f10x_flash.h"
#include "Delay_Timer.h"
#include "stm32f10x.h"
#include "key.h"

/* private------- ------------------------------------------------------------*/
u16 Config_Data[] = {1,0,220,280,100,450,0,0,0};//配置系统参数,依次为控制方式参数、语言选择、低温水、高温水、下限温度、上限温度等参数
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节

#define SIZE sizeof(Config_Data)	 	//数组长度
#define FLASH_SAVE_ADDR  0x0803f800 	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000),最后一页存储地址
/**
  * @brief	读取指定地址的半字(16位数据)
  * @param faddr:读地址(此地址必须为2的倍数!!)
  * @retval : 返回值:对应数据.
  */

u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}  
/**
  * @brief	不检查的写入
  * @param 	WriteAddr:起始地址,
  				pBuffer:数据指针,
  				NumToWrite:半字(16位)数 
  * @retval : 无.
  */

void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
} 
/**
  * @brief	从指定地址开始写入指定长度的数据
  * @param 	WriteAddr:起始地址(此地址必须为2的倍数!!),
  				pBuffer:数据指针,
  				NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
  * @retval : 无.
  */

void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
	if((WriteAddr<STM32_FLASH_BASE)||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return;		//非法地址
	FLASH_Unlock();	//解锁
	offaddr=WriteAddr-STM32_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if(NumToWrite<=secremain)
		secremain=NumToWrite;				//不大于该扇区范围
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)
				break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//写入整个扇区  
		}
		else 
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumToWrite==secremain)
			break;//写入结束了
		else//写入未结束
		{
			secpos++;							//扇区地址增1
			secoff=0;							//偏移位置为0 	 
		   	pBuffer+=secremain;  				//指针偏移
			WriteAddr+=secremain;				//写地址偏移	   
		   	NumToWrite-=secremain;				//字节(16位)数递减
			if(NumToWrite>(STM_SECTOR_SIZE/2))
				secremain=STM_SECTOR_SIZE/2;	//下一个扇区还是写不完
			else 
				secremain=NumToWrite;			//下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
}
/**
  * @brief	从指定地址开始读出指定长度的数据
  * @param 	ReadAddr:起始地址
  				pBuffer:数据指针
  				NumToRead:半字(16位)个数
  * @retval : 无.
  */

void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);	//读取2个字节.
		ReadAddr+=2;								//偏移2个字节.	
	}
}
/**
  * @brief	存储系统参数
  * @param 	
  * @retval : 无.
  */

void StoreSysPrama(void)
{
	Config_Data[0]=(u16)Key.Model_count;//保存模式参数
	Config_Data[2]=TemperaSet[0];//保存低温设置值
	Config_Data[3]=TemperaSet[1];//保存高温设置值
	Config_Data[4]=TemperaSet[2];//保存下限温度设置值
	Config_Data[5]=TemperaSet[3];//保存上限温度设置值
	Config_Data[6]=TemperaSet[4];//保存环境温度系数
	Config_Data[7]=TemperaSet[5];//保存低温水温度系数
	Config_Data[8]=TemperaSet[6];//保存高温水温度系数
	STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)Config_Data,SIZE);
}

/**
  * @brief	获得系统参数
  * @param 	
  * @retval : 无.
  */

void GetSysPrama(void)
{
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)Config_Data,SIZE);
	Key.Model_count=(char)Config_Data[0];//保存模式参数
	TemperaSet[0]=Config_Data[2];//保存低温设置值
	TemperaSet[1]=Config_Data[3];//保存高温设置值
	TemperaSet[2]=Config_Data[4];//保存下限温度设置值
	TemperaSet[3]=Config_Data[5];//保存上限温度设置值
	TemperaSet[4]=Config_Data[6];//获取环境温度系数
	TemperaSet[5]=Config_Data[7];//获取低温水温度系数
	TemperaSet[6]=Config_Data[8];//获取高温水温度系数
}










