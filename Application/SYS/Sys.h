#ifndef __SYS_H
#define __SYS_H
#include "STC12C5A60S2.h"
#include <intrins.h>
#define uint8 	unsigned char
#define uint16 	unsigned int
#define nops();  {_nop_(); _nop_(); _nop_(); _nop_();} //定义空指令
void delay_ms(uint16 n);						//延时函数
#endif