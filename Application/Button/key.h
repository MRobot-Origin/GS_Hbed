/**********************51单片机例程************************
*  平台:Keil v5 + STC89C52;兼容51单片机
*  名称:STC89C52按键检测
*  编写：奇源创客
*  日期:2019-04-30
*   QQ :1738115850
*  晶体:11.0592MHZ
*	 变更记录：无
******************************************************************/
#ifndef __KEY_H
#define __KEY_H
#include <reg52.h>
#include "Sys.h"//包合被注释的部分，用的时候，删掉这句，然后把下面的注释去掉
//#define uint8 	unsigned char
//#define uint16 	unsigned int

sbit Key1 = P2^0;//按键端口初始化
sbit Key2 = P2^1;
sbit Key3 = P2^2;
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
uint8 Key_Scan(uint8 mode);//按键检测
/*使用方法,根据自己需要的按键，在.C文件自行添加。当对应按键按下之后会返回一个对应的数值，然后判断这个数值就可以了。
这段代码是根据原子的例程更改的，用起来还不错，代码也比较简洁。适用于所有的按键检测单片机程序。
1、
	if(Key_Scan(0)==1)
		{
		}
2、switch(Key_Scan(0))
		case 1:
				break;
		case 2:
				break;
		...
		dafault:
		break;
*/

#endif