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

uint16 Set_temp = 0;//设置目标温度

uint8 Data_pre_Flag = 0;//数据保存Flag,全局变量
uint8 SW_flag = 0;			//为1时，允许开始温控程序

extern uint8 Data_preservation(uint16 dat);//保存设定温度到EEPROM
extern uint16 Get_Temp_set(void);

//extern void EEPROM_Test(void);
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
	
	if((Get_Temp_set()>0)&&(Get_Temp_set()<271))	//获取并保证设置温度读取正确
	{Set_temp = Get_Temp_set();}
	else{Set_temp = 0;}
	printf("Set: %d \r\n",Set_temp);//串口输出当前温度
	
	delay_ms(2000);
	LCD_Clear();
	LCD_Write_String(0,0,"Set:");	//显示设定温度	//工作界面显示
	LCD_TEM(7,0,Set_temp);//设置温度值显示
	LCD_Write_String(0,1,"Temp:");

#ifdef TEST_EEPROM//测试屏蔽编译
	EEPROM_Test();
#endif
//	if(Data_preservation(270))//数据保存
//	{
//		printf("OK \r\n");
//	}
//	printf("Get:%d \r\n",Get_Temp_set());
}
void loop(void)
{
	while(1)//温度控制
	{
		uint16 Flash_date;		
		if(Sys_SW ==1 )
		{
			Tem_calculation();//采集当前温度
			//简单的温度控制
			if(SW_flag == 1)//如果当前允许加热，则开始温度控制
			{
				if((Temp<(Set_temp+7))&&(Temp>0))//小于设定温度7度开始加热
				{
					BED = ON;
				}else if((Temp>Set_temp)||(Temp==0)||(Temp<0))//大于设定温度停止加热
				{
					BED = OFF;
				}
			}
			if(Temp<0)//最小值为0
			{Temp=0;}
			LCD_TEM(7,0,Set_temp);		//显示设定温度值
			LCD_TEM(7,1,Temp);				//显示当前温度值
			/**********************重要代码，修改可能引起过度写入EEPROM导致芯片损毁***************************/
			////先读取数据验证，然后再进行写入，避免多次重复写入
			if(Dat_Save_flag==1)
			{
				Flash_date = Get_Temp_set();//读取数据
				if(Flash_date != Set_temp)//如果设置是一样的，则跳过,如果改变了目标温度则将当前设置写入Flash
				{
					Data_preservation(Set_temp);
					printf("save success.%d \r\n",Set_temp);
				}
				Dat_Save_flag=0;
			}
			/************************************************/
//		printf("Set: %d \r\n",Set_temp);//串口输出当前温度、
//			printf("Set: %f \r\n",Rt);//串口输出当前阻值
//		printf("T: %f \r\n",Temp);//串口输出当前温度
			delay_ms(100);//大概0.1秒刷新一次屏幕			
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

// void LCD_V(uint8 a,uint8 l,uint8 p)					//显示各通道所测得的电压值，需要在P1.0引脚加TL431引出的标准电压2.47V才能够正常工作，并保证测量准确，误差在0.02V以内
// {
//	LCD_Write_Char(a,l,(int)(Filtering_Voltage(p))/10+0x30);     //个位
//	LCD_Write_Char(a+1,l,(int)(Filtering_Voltage(p))%10+0x30);     //个位
//	LCD_Write_Char(a+2,l,'.');
//	LCD_Write_Char(a+3,l,(int)(Filtering_Voltage(p)*100)%100/10+0x30); //十分位
//	LCD_Write_Char(a+4,l,(int)(Filtering_Voltage(p)*100)%100%10+0x30); //百分位
//	LCD_Write_Char(a+5,l,'V');
// }
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
	LCD_Write_Char(a+4,b,'C');
}
//
