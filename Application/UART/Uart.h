/**********************51单片机例程************************
*  平台:Keil v5 + STC89C52;兼容51单片机
*  名称:STC89C52串口配置
*  编写：奇源创客
*  日期:2019-04-30
*   QQ :1738115850
*  晶体:11.0592MHZ
*	 变更记录：无
******************************************************************/
#ifndef __UART_H
#define __UART_H
#include <reg52.h>
#include "Sys.h"//宏定义uint16;uint8
//#define uint8 	unsigned char
//#define uint16 	unsigned int
void Uart_Init(void);//串口初始化
//void UART_Send_Byte(uint8 dat);//发送一个字符
//void UART_Send_STR(uint8 *str);//发送一个字符串

/*
主函数串口初始化，
Uart_Init();
printf("Hello");

*/
#endif