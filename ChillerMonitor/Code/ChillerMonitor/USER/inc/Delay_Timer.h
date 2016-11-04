#ifndef __DELAY_TIMER_H 
#define __DELAY_TIMER_H 

#define  DWT_CR      *(volatile unsigned int *)0xE0001000
#define  DWT_CYCCNT  *(volatile unsigned int *)0xE0001004
#define  DEM_CR      *(volatile unsigned int *)0xE000EDFC
#define  DBGMCU_CR   *(volatile unsigned int *)0xE0042004
#define  DEM_CR_TRCENA                   (1 << 24)
#define  DWT_CR_CYCCNTENA                (1 <<  0)

#define Delayms(msec) Delayus(msec*1000)  /*对于延时毫秒级的只需要定义一个宏*/

static unsigned int cpuclkfeq;     /*用于保存CPU运行频率,可以运行时动态修改*/
void DelayInit(unsigned int clk);
char Delayus(unsigned int usec);
#endif
