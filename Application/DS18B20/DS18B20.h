/**********************51单片机学习例程************************
*  平台:Keil v5 + STC89C52;兼容51单片机
*  名称:STC89C52获取DS18B20温度
*  编写：奇源创客
*  日期:2019-04-30
*   QQ :1738115850
*  晶体:11.0592MHZ
*	 变更记录：无
******************************************************************/

#ifndef __DS18B20_H
#define __DS18B20_H
#include <reg52.h>
#include "Sys.h"/*配合Sys.h和Sys.c使用*/

//#define uint8 	unsigned char
//#define uint16 	unsigned int
//#define nops();  {_nop_(); _nop_(); _nop_(); _nop_();} //定义空指令


sbit DQ = P1^0;//定义传感器接口,根据需求自行更改
/*内部函数不需要外部调用，如有需要自行更改，在C文件中去掉函数前面的static*/
//void Ds18b20_Reset(void);//温度传感器复位
//void Write_Byte(uint8 val);//写入字节
//uint8 Read_Byte(void);//读取字节
//void Start_Convert(void);//开始采集温度数据
//uint16 Read_Temp(void);//读取传感器数据
float Get_T(void);//将获取到的温度数据转化为十进制

#endif
/*
将DS18B20.c和.h文件都添加到工程中，然后在主函数中包含头文件，直接使用 “变量 = Get_T();”即可获取传感器温度,浮点型数据。
在main.c中包含头文件
eg：
printf("当前温度为 %0.2f\r\n",Get_T());
*/
