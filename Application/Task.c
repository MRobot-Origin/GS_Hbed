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
void setup(void)
{
	LCD_Init();         // 初始化LCD
	Uart_Init();				//串口初始化
	Init_ADC();  				//AD转换初始化
	Beep = 0;
	
//	LCD_Write_String(0,0,"GSH-BED");
//	printf("GSH");
//		LCD_Clear(); 
		LCD_Write_String(0,0,"Tem_V:");	//显示系统工作电压
		LCD_Write_String(0,1,"Tem:");
}
void loop(void)
{
	while(1)
	{
		
		printf("V: %f \r\n",(float)Filtering_Voltage(2));//
		Tem_calculation();//温度计算
		printf("R: %f \r\n",Rt);
		printf("T: %f \r\n",Temp);
		LCD_V(7,0,2);//电压值
		LCD_TEM(7,1,Temp);//温度值
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
void LCD_TEM(uint8 a,uint8 b,float p)
{
	LCD_Write_Char(a,b,(int)p/100+0x30);     //个位
	LCD_Write_Char(a+1,b,((int)p%100)/10+0x30);     //个位
	LCD_Write_Char(a+2,b,(int)p%10+0x30);     //个位
	LCD_Write_Char(a+3,b,'.');
	LCD_Write_Char(a+4,b,(int)(p*100)%100/10+0x30); //十分位
	LCD_Write_Char(a+5,b,(int)(p*100)%100%10+0x30); //百分位
	LCD_Write_Char(a+6,b,'C');
}