/***************************************************
定时器计数器配置
****************************************************/
#include "STC12C5A60S2.h"
#include "Timer.h"
#include "sys.h"
#include "cfg.h"
//#include "Task.h"
/***************************************************
*				定时器0初始化子函数
*			工作方式1，每10ms中断一次
****************************************************/
uint16	Time_num = 0;

uint8		Tem_sw_flag = 0;
uint8		Tem_Off_flag = 0;
uint8		Tem_Up_flag	 = 0;
uint8		Tem_Down_flag = 0;
uint8		Ctrol_up_flag = 0;
uint8		Ctrol_Down_flag = 0;
uint8		Ctrol_mode_flag = 0;

uint8		Dat_Save_T_S = 0;	//保存数据所用的计时开始flag
uint8		Dat_Save_flag = 0;//1则可以保存

uint8		Time_count = 0;		//计时，单位秒
uint8		Beep_flag = 0;		//蜂鸣器启停
uint8		Beep_Action = 0;

uint16	Sys_Time = 0;//系统时间
uint8		Sys_SW = 1;//主流程开关

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
*			中断服务子函数，包含按键检测和超时保护
****************************************************/
void Timer0() interrupt 1
{
/**************************************************************************************************/
	if((Ctrol_mode_flag == 1)&&(Tem_SW == 1))//按键松开检测//step3
	{
		if(MODE == ModeA)
		{
			MODE = ModeB;
		}else if(MODE == ModeB)
		{
			MODE = ModeA;
		}
		Ctrol_mode_flag = 0;
		Dat_Save_T_S = 1;	//允许保存计时
		Time_count = 0;		//不满10秒的计时将被清零
		Tem_sw_flag = 0;	//按键检测Flag复位
		Beep_Action = 0;
		Sys_SW = 1;				//唤醒主流程
	}
	if((Tem_sw_flag == 1)&&(Tem_SW == 0))//模式切换按钮被按下//step2
	{
		SW_flag = 1;			//当前允许加热
		Ctrol_mode_flag = 1;//允许模式设置
		if(Beep_Action == 0)
		{Beep_flag = 1;}//蜂鸣器音效
	}
	if((Tem_SW == 0)&&(Tem_sw_flag == 0))//第一次检测到停止按钮按下，记录下来，等待下次检测（按键消抖）step1
	{Tem_sw_flag = 1;}
/**************************************************************************************************/
	if((Tem_Off_flag == 1)&&(Tem_Off == 0))//停止加热
	{
		Tem_Off_flag = 0;//Flag复位
		SW_flag = 0;		//当前不允许加热
		Sys_SW = 0;			//关闭主流程
	}
	if((Tem_Off == 0)&&(Tem_Off_flag!=1))//第一次检测到停止按钮按下，记录下来，等待下次检测（按键消抖）
	{Tem_Off_flag = 1;}
/**************************************************************************************************/
/**************************************************************************************************/
	if((Ctrol_up_flag == 1)&&(Tem_Up == 1))//按键松开检测//step3
	{
		if(MODE == ModeA)
		{
			Set_temp1++;				//设定值+1
			if(Set_temp1>270)
			{Set_temp1=270;}
		}else if(MODE == ModeB)
		{
			Set_temp2++;				//设定值+1
			if(Set_temp2>270)
			{Set_temp2=270;}
		}
		Ctrol_up_flag = 0;
		Dat_Save_T_S = 1;	//允许保存计时
		Time_count = 0;		//不满10秒的计时将被清零
		Tem_Up_flag = 0;	//按键检测Flag复位
		Beep_Action = 0;
		Sys_SW = 1;				//唤醒主流程
	}
	if((Tem_Up_flag == 1)&&(Tem_Up == 0))//增加设定温度按钮被按下//step2
	{
		SW_flag = 1;			//当前允许加热
		Ctrol_up_flag = 1;//允许温度设置
		if(Beep_Action == 0)
		{Beep_flag = 1;}//蜂鸣器音效
	}
	if((Tem_Up == 0)&&(Tem_Up_flag == 0))//第一次检测到停止按钮按下，记录下来，等待下次检测（按键消抖）step1
	{Tem_Up_flag = 1;}
/**************************************************************************************************/
/**************************************************************************************************/
	if((Ctrol_Down_flag == 1)&&(Tem_Down == 1))//按键松开检测//step3
	{
		if(MODE == ModeA)
		{
			Set_temp1--;				//设定值-1
			if(Set_temp1<0)
			{Set_temp1=0;}
		}else if(MODE == ModeB)
		{
			Set_temp2--;				//设定值-1
			if(Set_temp2<0)
			{Set_temp2=0;}
		}
		Ctrol_Down_flag = 0;
		Dat_Save_T_S = 1;	//允许保存计时
		Time_count = 0;		//不满10秒的计时将被清零
		Tem_Down_flag = 0;//Flag复位
		Beep_Action = 0;
		Sys_SW = 1;				//唤醒主流程
	}
	if((Tem_Down_flag == 1)&&(Tem_Down == 0))//减小设定温度//step2
	{
		SW_flag = 1;		//当前允许加热
		Ctrol_Down_flag = 1;
		if(Beep_Action == 0)
		{Beep_flag = 1;}//蜂鸣器音效
	}
	if((Tem_Down == 0)&&(Tem_Down_flag == 0))//第一次检测到停止按钮按下，记录下来，等待下次检测（按键消抖）//step1
	{Tem_Down_flag = 1;}
/**************************************************************************************************/
	Time_num++;
	if(Time_num>10)//1秒计时，每次调整温度设定后开始计时，超过5秒则保存当前设定温度
	{
		Time_num = 0;//清除计数等待重新计数		
		Sys_Time++;//系统时间//单位秒
		if(Dat_Save_T_S==1)//允许保存计时
		{
			Time_count++;
			if(Time_count>50)//停止操作的时候，开始计时5秒，保存当前设置
			{
				Dat_Save_flag = 1;
				Time_count = 0;//恢复参数初始化，等待下次使用
				Dat_Save_T_S = 0;
			}
		}

		if(Sys_Time>12000)//超过20分钟则关闭主流程
		{
			if(MODE==ModeA)//只有在模式A下，才会自动停止加热
			{Sys_SW = 0;}//关闭主流程			
			Sys_Time = 0;
		}
/**************************************************************************************************/
		if(Beep_flag == 0)//蜂鸣器短促的响一个定时周期，0.1秒
		{
			Beep = 0;//停
		}else if(Beep_flag == 1)
		{
			Beep = 1;//响
			Beep_flag = 0;
			Beep_Action = 1;
		}
	}

}

