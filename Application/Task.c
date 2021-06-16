#include "Sys.h"
#include "Uart.h"
#include "LCD1602.h"
#include "Task.h"
#include "STC12(ADC flag).h"
#include "cfg.h"
#include	"math.h"
void setup(void)
{
	LCD_Init();         // 初始化LCD
	Uart_Init();				//串口初始化
	Init_ADC();  				//AD转换初始化
	Beep = 0;
	
//	LCD_Write_String(0,0,"GSH-BED");
//	printf("GSH");
//		LCD_Clear(); 
		LCD_Write_String(0,0,"sys_V:");	//显示系统工作电压
		LCD_V(6,0,1);
printf("%f",(int)Filtering_Voltage(1)*6);
}
void loop(void)
{
	while(1)
	{
		
	}
}
 void LCD_V(uint8 a,uint8 l,uint8 p)					//显示各通道所测得的电压值，需要在P1.0引脚加TL431引出的标准电压2.47V才能够正常工作，并保证测量准确，误差在0.02V以内
 {
	LCD_Write_Char(a,l,(int)(Filtering_Voltage(p)*6)/10+0x30);     //个位
	LCD_Write_Char(a+1,l,(int)(Filtering_Voltage(p)*6)%10+0x30);     //个位
	LCD_Write_Char(a+2,l,'.');
	LCD_Write_Char(a+3,l,(int)(Filtering_Voltage(p)*6*100)%100/10+0x30); //十分位
	LCD_Write_Char(a+4,l,(int)(Filtering_Voltage(p)*6*100)%100%10+0x30); //百分位
	LCD_Write_Char(a+5,l,'V');
 }
