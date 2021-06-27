/***************************************************
定时器计数器配置
****************************************************/
#include "STC12C5A60S2.h"
#include "Timer.h"
#include "sys.h"
#include "key.h"
#include "cfg.h"

//#include "Uart.h"
uint16 num = 0;

/***************************************************
*				定时器0初始化子函数
*			工作方式1，每10ms中断一次
****************************************************/
void Timer0_init(void)//10ms定时中断，16位自动重载
{
	AUXR &= 0x80;			//定时器时钟12T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x00;				//设置定时初始值
	TH0 = 0x28;				//设置定时初始值
	TF0 = 0;					//清除TF0标志
	ET0 = 1;			 		//开定时器0中断
	EA = 1;			 			//开总中断
	TR0 = 1;					//定时器0开始计时
}

/***************************************************
*								中断服务子函数
****************************************************/
void Timer0() interrupt 1
{
	if(num>100)
	{
		BED = ~BED;
//		printf("Timer %d \r\n",num);
	}
}