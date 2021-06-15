#include <reg52.h>
#include <intrins.h>
#include "DS18B20.h"
#include "Sys.h"//自定义延时函数，uint8和uint16
/***************************************************
*								DS18B20复位子函数
*						 总线拉低550us，释放66us
****************************************************/
void ds18b20_reset(void)
{
	bit flag=1;
	while (flag)				//判断初始化是否成功
	{
		while (flag)			//判断DS18B20是否给出反馈
	 	{
 			DQ = 1;
			delay_11us(1);	//高电平约11us
 			DQ = 0;
 			delay_11us(50); //低电平550us
 			DQ = 1; 
 			delay_11us(6);  //释放66us
 			flag = DQ;			//总线状态采样
   	}
		delay_11us(50);   //延时550us
		flag = ~DQ;				//总线状态采样
	}
	DQ=1;
}
/***************************************************
*							向总线写入一个字节
****************************************************/
void write_byte(uint8 val)
{
	uint8 i;
	for (i=0; i<8; i++)
	{
		DQ = 1;								//写时隙准备工作
		_nop_();
		DQ = 0;								//告诉DS18B20，准备采样
		nops(); 							//4us
		DQ = val & 0x01;      //最低位移出
		delay_11us(6);        //66us
		val >>= 1;          	//右移一位
	}
	DQ = 1;
	delay_11us(1);  
}
/***************************************************
*					    	从总线读出一个字节
****************************************************/
uint8 read_byte(void)
{
	uint8 i, value=0;
	for (i=0; i<8; i++)
	{
		DQ=1;								//读时隙准备工作
		_nop_();
		value >>= 1;
		DQ = 0;							//告诉DS18B20，准备发送
		nops();   					//4us
		DQ = 1;							//释放总线
		nops();   					//4us 
		if (DQ)							//0和1的判断
			value|=0x80;
		delay_11us(6);           //66us
	}
	DQ=1;
	return(value);				//返回采集到的数据
}
/***************************************************
*								温度转换子函数
*						根据操作顺序，发出转换命令
****************************************************/
void start_convert(void)
{
	ds18b20_reset();
	write_byte(0xCC); 		// 发Skip ROM命令
	write_byte(0x44); 		// 发转换命令
}
/***************************************************
*								温度读取子函数
*						根据操作顺序，读取温度值
****************************************************/
uint16 read_temp(void)
{
	uint8 temp_data[2]; 			// 读出温度暂放
	uint16 temp;

	ds18b20_reset();  				// 复位
	write_byte(0xCC); 				// 发Skip ROM命令
	write_byte(0xBE); 				// 发读命令
	temp_data[0]=read_byte();  //温度低8位
	temp_data[1]=read_byte();  //温度高8位
	temp = temp_data[1];
	temp <<= 8;
	temp |= temp_data[0];
	return temp;
}
float Get_T(void)
{
	float T;
	uint16 Tem;
	start_convert();//开始采集温度数据
	//delay_ms(100);
	Tem = read_temp();
	if(Tem>60000)//温度为负
	{
		Tem = ~Tem;
		Tem++;
		T = -(Tem*0.0625);
		return T;
	}
	else
	{T = Tem*0.0625;return T;}
}
