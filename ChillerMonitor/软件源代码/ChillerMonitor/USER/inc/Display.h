#ifndef __DISPLAY_H
#define __DISPLAY_H

/* Private macro -------------------------------------------------------------*/
#define NULL ((void*)0)

#define MenuFreshTime 5	//������ʾˢ��ʱ��
#define LogoFreshTime 50	//��˾LOGO��ʾʱ��

#define MenuNum 14	//������ʾˢ��ʱ��

/* Private typedef -----------------------------------------------------------*/
typedef void (* pfMenu)(void);//����һ������ָ�����ͺ�
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
extern char OldMenu;//�����л�ǰһ�εĽ���
extern char AlarmSaveflag;

/* Exported functions ------------------------------------------------------- ---*/ 

extern void EnterNormalDis(void);//��ͨ��ʾ����
extern void EnterModelSetting(void);//����ģʽѡ�����
extern void EnterTemperaSetting(void);//�����¶Ⱥ���
extern void AlarmDis(void);//������ʾ����

extern void DisLogo(void);//��˾LOGO��ʾ����
extern void WaitOperation(void);//����ģʽ�ȴ�����
extern void AntifreezeDis(void);//�����������

extern void Dis_Menu(char MenuDis);//��ʾ���溯��
#endif

