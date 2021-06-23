#include "Sys.h"
#include "Uart.h"
#include "LCD1602.h"
#include "Task.h"
#include "Thermistortables.h"
#include <math.h>
#include "STC12(ADC flag).h"
#include "cfg.h"
extern float Rt;
extern float Temp;
extern uint8 Data_preservation(uint16 dat);
extern uint16 Get_Temp_set(void);

//extern void EEPROM_Test(void);
void setup(void)
{
	uint16 Set_temp;//EEPROM获取的设置温度
	
	Uart_Init();				//串口初始化
	Init_ADC();  				//AD转换初始化
	LCD_Init();         // 初始化LCD
	Beep = 0;						//初始化蜂鸣器
	BED = 1;						//初始化热床
	LCD_Write_String(2,0,"<Geek Start>");	//开机界面
	LCD_Write_String(2,1,"GSH_Bed V1.0");
	
	if((Get_Temp_set()>0)&&(Get_Temp_set()<271))//获取并保证设置温度读取正确
	{Set_temp = Get_Temp_set();}
	else{Set_temp = 0;}
	
	delay_ms(4000);
	LCD_Clear();
	
	LCD_Write_String(0,0,"Set:");	//显示设定温度
	LCD_TEM(7,0,Set_temp);//设置温度值显示
//	LCD_TEM(7,1,Temp);//测量温度值显示
	LCD_Write_String(0,1,"Temp:");
#ifdef TEST_EEPROM//测试屏蔽编译
	EEPROM_Test();
#endif
	if(Data_preservation(270))//数据保存
	{
		printf("OK \r\n");
	}
	printf("Get:%d \r\n",Get_Temp_set());
	
}
void loop(void)
{
	while(1)
	{
		BED = ~BED;
//		Beep = ~Beep;
//		printf("V: %f \r\n",(float)Filtering_Voltage(2));//
		Tem_calculation();//温度计算
//		printf("R: %f \r\n",Rt);
		printf("T: %f \r\n",Temp);
//		LCD_V(7,0,2);//电压值
		LCD_TEM(7,1,Temp);//测量温度值显示
		delay_ms(3000);
	}
}

 void LCD_V(uint8 a,uint8 l,uint8 p)					//显示各通道所测得的电压值，需要在P1.0引脚加TL431引出的标准电压2.47V才能够正常工作，并保证测量准确，误差在0.02V以内
 {
	LCD_Write_Char(a,l,(int)(Filtering_Voltage(p))/10+0x30);     //个位
	LCD_Write_Char(a+1,l,(int)(Filtering_Voltage(p))%10+0x30);     //个位
	LCD_Write_Char(a+2,l,'.');
	LCD_Write_Char(a+3,l,(int)(Filtering_Voltage(p)*100)%100/10+0x30); //十分位
	LCD_Write_Char(a+4,l,(int)(Filtering_Voltage(p)*100)%100%10+0x30); //百分位
	LCD_Write_Char(a+5,l,'V');
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

	LCD_Write_Char(a+4,b,'C');
}
//
