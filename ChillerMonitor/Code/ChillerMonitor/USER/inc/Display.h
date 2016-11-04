#ifndef __DISPLAY_H
#define __DISPLAY_H

/* Private macro -------------------------------------------------------------*/
#define NULL ((void*)0)

#define MenuFreshTime 5	//界面显示刷新时间
#define LogoFreshTime 50	//公司LOGO显示时间

#define MenuNum 14	//界面显示刷新时间

/* Private typedef -----------------------------------------------------------*/
typedef void (* pfMenu)(void);//定义一个函数指针类型宏
/* Private variables ---------------------------------------------------------*/
typedef struct
{
	char* pText1;
	char* pText2;
	char* pText3;
	char* pText4;
	pfMenu pMenuFunc;
}Menu_Typedef;

extern Menu_Typedef CH_SetupMenuItems[MenuNum];
extern char OldMenu;//保存切换前一次的界面
extern char AlarmSaveflag;

/* Exported functions ------------------------------------------------------- ---*/ 

extern void EnterNormalDis(void);//普通显示界面
extern void EnterModelSetting(void);//工作模式选择界面
extern void EnterTemperaSetting(void);//设置温度函数
extern void AlarmDis(void);//报警显示界面

extern void DisLogo(void);//公司LOGO显示界面
extern void WaitOperation(void);//操作模式等待界面
extern void AntifreezeDis(void);//防冻处理界面

extern void Dis_Menu(char MenuDis);//显示界面函数
#endif

