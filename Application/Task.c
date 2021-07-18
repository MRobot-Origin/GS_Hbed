#include "Sys.h"
#include "Uart.h"
#include "LCD1602.h"
#include "Task.h"
#include "Thermistortables.h"
#include <math.h>
#include "STC12(ADC flag).h"
#include "cfg.h"
#include "Timer.h"

extern float Rt;	//计算测到的热敏电阻值
extern float Temp;//测量的温度
extern uint8 Dat_Save_flag;

uint16	 MODE = ModeA;
uint16 Set_temp1 = 0;//设置目标温度
uint16 Set_temp2 = 0;//设置目标温度

uint8 Data_pre_Flag = 0;//数据保存Flag,全局变量
uint8 SW_flag = 0;			//为1时，允许开始温控程序

extern uint8 Data_preservation(uint16 dat1,uint16 dat2,uint16 mode);//保存设定温度到EEPROM
extern uint16 Get_Temp_set1(void);
extern uint16 Get_Temp_set2(void);
extern uint16 Get_Mode(void);
static uint16 Flash_date1,Flash_date2,Flash_date3;
//extern void EEPROM_Test(void);
void LCD_V(uint8 a,uint8 l,uint8 p);
void setup(void)
{
	Uart_Init();				//串口初始化
	Init_ADC();  				//AD转换初始化
	LCD_Init();         // 初始化LCD
	Timer0_Init();			//定时器初始化
	Beep = 0;						//初始化蜂鸣器
	BED = OFF;						//初始化热床
	SW_flag = 1;				//开机允许加热
	LCD_Write_String(2,0,"<Geek Start>");					//开机界面
	LCD_Write_String(2,1,"GSH_Bed V1.0");
	
	if((Get_Temp_set1()>0)&&(Get_Temp_set1()<271))	//获取并保证设置温度读取正确
	{Set_temp1 = Get_Temp_set1();}
	else{Set_temp1 = 0;}
	
	if((Get_Temp_set2()>0)&&(Get_Temp_set2()<271))	//获取并保证设置温度读取正确
	{Set_temp2 = Get_Temp_set2();}
	else{Set_temp2 = 0;}
	MODE = Get_Mode();//获取默认模式
	printf("Mode: %d \r\n",MODE);//串口输出当前温度	
	if((MODE != ModeA)&&(MODE != ModeB))
	{MODE = ModeA;}
	printf("Set1: %d \r\n",Set_temp1);//串口输出当前温度
	printf("Set2: %d \r\n",Set_temp2);//串口输出当前温度
	
	delay_ms(2000);
	LCD_Clear();
	LCD_Write_String(0,0,"Sys:  .  V Mode ");	//显示系统工作电压和当前工作模式
	LCD_Write_String(0,1,"Tem:    C->    C");	//显示设定温度	//工作界面显示
	if(MODE == ModeA)
	{
		LCD_TEM(11,1,Set_temp1);
		LCD_Write_String(15,0,"A");//LCD显示当前模式
	}//设置温度值显示
	else if(MODE == ModeB)
	{
		LCD_TEM(11,1,Set_temp2);
		LCD_Write_String(15,0,"B");//LCD显示当前模式
	}
	LCD_V(4,0,1);//显示系统供电电压
//	EEPROM_Test();
}
void loop(void)
{
	while(1)//温度控制
	{
		if(Sys_SW ==1 )//系统控制运行开关
		{
			Tem_calculation();//采集当前温度
			//简单的温度控制
			if(SW_flag == 1)//如果当前允许加热，则开始温度控制
			{
				if(MODE == ModeA)//模式A温控
				{
					LCD_Write_String(15,0,"A");//LCD显示当前模式
					LCD_TEM(11,1,Set_temp1);		//显示设定温度值
					if((Temp<(Set_temp1-5))&&(Temp>0))//小于设定温度5度开始加热
					{
						BED = ON;
					}else if((Temp>Set_temp1)||(Temp==0)||(Temp<0))//大于设定温度停止加热
					{
						BED = OFF;
					}
				}else if(MODE == ModeB)//模式B温控
				{
					LCD_Write_String(15,0,"B");//LCD显示当前模式
					LCD_TEM(11,1,Set_temp2);		//显示设定温度值
					if((Temp<(Set_temp2-5))&&(Temp>0))//小于设定温度5度开始加热
					{
						BED = ON;
					}else if((Temp>Set_temp2)||(Temp==0)||(Temp<0))//大于设定温度停止加热
					{
						BED = OFF;
					}
				}
			}
			if(Temp<0)//最小值为0
			{Temp=0;}
			LCD_V(4,0,1);//显示系统供电电压
			
			LCD_TEM(4,1,Temp);				//显示当前温度值
			/**********************重要代码，修改可能引起过度写入EEPROM导致芯片损毁***************************/
			////先读取数据验证，然后再进行写入，避免多次重复写入
			if(Dat_Save_flag==1)
			{
				Flash_date1 = Get_Temp_set1();//读取数据
				Flash_date2 = Get_Temp_set2();//读取数据
				Flash_date3 = Get_Mode();//读取数据
				
				if((Flash_date1 != Set_temp1)||(Flash_date2 != Set_temp2)||(Flash_date3 != MODE))//如果设置是一样的，则跳过,如果改变了目标温度则将当前设置写入Flash
				{
					Data_preservation(Set_temp1,Set_temp2,MODE);//保存数据
					printf("save success 1:%d,2:%d,3:%d \r\n",Set_temp1,Set_temp2,MODE);
				}
				Dat_Save_flag=0;
			}
			/************************************************/
//		printf("Set: %d \r\n",Set_temp);//串口输出当前温度、
//			printf("Set: %f \r\n",Rt);//串口输出当前阻值
//		printf("T: %f \r\n",Temp);//串口输出当前温度
			delay_ms(500);//大概0.1秒刷新一次屏幕			
		}
		else
		{
			BED = OFF;
			Beep = 1;			 //超时关闭热床前，蜂鸣器响一秒
			delay_ms(1000);
			Beep = 0;
			LCD_Clear();
		}
	}
}

 void LCD_V(uint8 a,uint8 l,uint8 p)					//显示各通道所测得的电压值，需要在P1.0引脚加TL431引出的标准电压2.47V才能够正常工作，并保证测量准确，误差在0.02V以内
 {
	LCD_Write_Char(a,l,(int)(Filtering_Voltage(p)*6)/10+0x30);     //个位
	LCD_Write_Char(a+1,l,(int)(Filtering_Voltage(p)*6)%10+0x30);     //个位
	LCD_Write_Char(a+2,l,'.');
	LCD_Write_Char(a+3,l,(int)(Filtering_Voltage(p)*600)%100/10+0x30); //十分位
	LCD_Write_Char(a+4,l,(int)(Filtering_Voltage(p)*600)%100%10+0x30); //百分位
 }
void LCD_TEM(uint8 a,uint8 b,float p)//温度值显示
{
	if(((int)p/100)==0)//清除首位0显示
	{
		LCD_Write_Char(a,b,' ');
		LCD_Write_Char(a+1,b,' ');
	}
	else
	{LCD_Write_Char(a,b,(int)p/100+0x30);}//百位
	
	if((((int)p%100)/10)!=0)
	{LCD_Write_Char(a+1,b,((int)p%100)/10+0x30);}//十位
	
	LCD_Write_Char(a+2,b,(int)p%10+0x30);     //个位
}
//
