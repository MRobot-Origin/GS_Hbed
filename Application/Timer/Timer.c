/***************************************************
��ʱ������������
****************************************************/
#include "STC12C5A60S2.h"
#include "Timer.h"
#include "sys.h"
#include "key.h"
#include "cfg.h"

//#include "Uart.h"
uint16 num = 0;

/***************************************************
*				��ʱ��0��ʼ���Ӻ���
*			������ʽ1��ÿ10ms�ж�һ��
****************************************************/
void Timer0_init(void)//10ms��ʱ�жϣ�16λ�Զ�����
{
	AUXR &= 0x80;			//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;			//���ö�ʱ��ģʽ
	TL0 = 0x00;				//���ö�ʱ��ʼֵ
	TH0 = 0x28;				//���ö�ʱ��ʼֵ
	TF0 = 0;					//���TF0��־
	ET0 = 1;			 		//����ʱ��0�ж�
	EA = 1;			 			//�����ж�
	TR0 = 1;					//��ʱ��0��ʼ��ʱ
}

/***************************************************
*								�жϷ����Ӻ���
****************************************************/
void Timer0() interrupt 1
{
	if(num>100)
	{
		BED = ~BED;
//		printf("Timer %d \r\n",num);
	}
}