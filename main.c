#include "reg52.h"
#include "Sys.h"
#include "stdio.h"//使用printf需要
#include "DS18B20.h"
#include "Uart.h"
#include "LCD1602.h"
#include "key.h"
sbit j = P1^7;
sbit i = P1^6;
sbit Beep = P0^4;
sbit BED	= P1^3;
void main()
{
	LCD_Init();         // 初始化LCD
	LCD_Clear();				//清屏
	Uart_Init();				//串口初始化
	Beep = 0;
//	BED = 0;
	LCD_Write_String(0,0,"GSH-BED");
	printf("GSH");
//	EEPROM_Test();
		while(1)
		{
			BED = 1;
//			Beep = 0;
			delay_ms(3000);
			BED = 0;
//			Beep = 1;
			delay_ms(3000);
		}
}