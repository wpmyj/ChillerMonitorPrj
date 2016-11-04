#ifndef __DELAY_TIMER_H 
#define __DELAY_TIMER_H 

#define  DWT_CR      *(volatile unsigned int *)0xE0001000
#define  DWT_CYCCNT  *(volatile unsigned int *)0xE0001004
#define  DEM_CR      *(volatile unsigned int *)0xE000EDFC
#define  DBGMCU_CR   *(volatile unsigned int *)0xE0042004
#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)

#define Delayms(msec) Delayus(msec*1000)  /*������ʱ���뼶��ֻ��Ҫ����һ����*/

static unsigned int cpuclkfeq;     /*���ڱ���CPU����Ƶ��,��������ʱ��̬�޸�*/
void DelayInit(unsigned int clk);
char Delayus(unsigned int usec);
#endif
