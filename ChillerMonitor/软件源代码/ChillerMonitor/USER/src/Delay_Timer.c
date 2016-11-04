/**************************************************************************//**
 * @file     Delay_Timer.c
 *
 * @note
 * ����CM3�����ݹ۲�������(DWT)������ʵ�־�ȷ��ʱ,������������
 * http://www.openedv.com/posts/list/22061.htm
 *
 ******************************************************************************/

#include "Delay_Timer.h"

void DelayInit(unsigned int clk){/*��ʼ����ʱϵͳ,����ΪCPUƵ��*/
	cpuclkfeq = clk;
	DEM_CR         |=  DEM_CR_TRCENA; /*��CYCCNT����,*/
	//DWT_CYCCNT = 0u;/*������Ҫ,������Ի���������Ҫ�õ�CYCCNTʱ��ע�͵�;�������ֱ������*/
	DWT_CR         |= DWT_CR_CYCCNTENA;/*�򿪼�������CPU�������ϼ���*/
}

char Delayus(unsigned int usec){						/*�ӳٺ���,����Ϊ��Ҫ��ʱ��΢�뺯��*/
	unsigned int startts,endts,ts;
	startts = DWT_CYCCNT;									/*������뺯��ʱ�ļ�����ֵ*/
	ts =  usec * (cpuclkfeq /(1000*1000) );/*����ﵽ��Ҫ��ʱ��CPUʱ����,
															�����Ҫ����ȷ�˴����Լ�ȥ����ǰ������ʱ����*/
	endts = startts + ts;  				/*����ﵽ�����ӳ�ʱ���DWT_CYCCNTֵ,
										����32bit���ܱ������ֵ2��32�η�-1���Զ��ƻز�������λ*/
	if(endts > startts){            /*�ж��Ƿ��Խ���ֵ�߽�*/
		while(DWT_CYCCNT < endts);        	/*�ȵ���������������ʱ��CPUʱ����ֵ*/
	}
		else{																/*��Խ32bit�����ֵ��һ�·�ʽ����*/
		while(DWT_CYCCNT > endts);       /*�ȴ���Խ32bit�����ֵ*/
		while(DWT_CYCCNT < endts);       /*�ȵ���������������ʱ��CPUʱ����ֵ*/
	}
	return (1);						/*��ʱ��ɷ���һ����־λ*/
}


