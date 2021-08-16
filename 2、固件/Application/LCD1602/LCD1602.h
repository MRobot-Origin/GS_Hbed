#ifndef __LCD1602_H__
#define __LCD1602_H__
sbit RS = P0^5;   //定义端口 端口名字可以自行更改
sbit RW = P0^6;
sbit EN = P0^7;
#define DataPort P2

void DelayUs2x(unsigned char t);//延时函数
void DelayMs(unsigned char t);

bit LCD_Check_Busy(void);
void LCD_Write_Com(unsigned char com);
void LCD_Write_Data(unsigned char Data);
void LCD_Clear(void);
void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s);//X:位置，0~15，Y：行0~1，内容
void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data);
void LCD_Init(void);
/*使用方法：eg:
	  LCD_Init();               // 初始化LCD
		
		while(1)
		{
				LCD_Clear(); 
				LCD_Write_Char(3,0,(int)Count(0)+0x30);     //个位
				LCD_Write_Char(4,0,'.');
				 
				LCD_Write_String(0,1,"hello");
		}
	
*/
#endif