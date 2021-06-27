#ifndef	CFG_H
#define	CFG_H

#include "Sys.h"
#define OFF 1		//ÈÈ´²×´Ì¬
#define ON  0

extern uint8 SW_flag;



extern uint16 num;

sbit Beep = P0^4;
sbit BED	= P1^3;

sbit	Tem_Up =	P1^4;
sbit	Tem_Down =	P1^6;
sbit	Tem_Off =	P3^2;
sbit	Tem_On =	P1^5;



#endif