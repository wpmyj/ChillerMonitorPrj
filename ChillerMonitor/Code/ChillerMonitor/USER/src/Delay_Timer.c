/**************************************************************************//**
 * @file     Delay_Timer.c
 *
 * @note
 * 利用CM3里数据观察点与跟踪(DWT)功能来实现精确延时,函数来自网络
 * http://www.openedv.com/posts/list/22061.htm
 *
 ******************************************************************************/

#include "Delay_Timer.h"

void DelayInit(unsigned int clk){/*初始化延时系统,参数为CPU频率*/
	cpuclkfeq = clk;
	DEM_CR         |=  DEM_CR_TRCENA; /*打开CYCCNT功能,*/
	//DWT_CYCCNT = 0u;/*根据需要,如果调试或其他程序要用到CYCCNT时则注释掉;否则可以直接清零*/
	DWT_CR         |= DWT_CR_CYCCNTENA;/*打开计数器对CPU进行向上计数*/
}

char Delayus(unsigned int usec){						/*延迟函数,参数为需要延时的微秒函数*/
	unsigned int startts,endts,ts;
	startts = DWT_CYCCNT;									/*保存进入函数时的计数器值*/
	ts =  usec * (cpuclkfeq /(1000*1000) );/*计算达到所要延时的CPU时钟数,
															如果想要更精确此处可以减去运行前面代码的时钟数*/
	endts = startts + ts;  				/*计算达到所需延迟时间的DWT_CYCCNT值,
										超过32bit所能表达的最大值2的32次方-1是自动绕回并丢弃进位*/
	if(endts > startts){            /*判断是否跨越最大值边界*/
		while(DWT_CYCCNT < endts);        	/*等到计数到达所需延时的CPU时钟数值*/
	}
		else{																/*跨越32bit的最大值按一下方式计数*/
		while(DWT_CYCCNT > endts);       /*等待跨越32bit的最大值*/
		while(DWT_CYCCNT < endts);       /*等到计数到达所需延时的CPU时钟数值*/
	}
	return (1);						/*延时完成返回一个标志位*/
}


