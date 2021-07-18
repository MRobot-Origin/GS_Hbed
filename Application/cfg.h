#ifndef	CFG_H
#define	CFG_H

#include "Sys.h"
#define OFF 1		//ÈÈ´²×´Ì¬
#define ON  0
#define	ModeA 3
#define ModeB 4

extern uint8 SW_flag;

extern uint16 num;

sbit Beep = P0^4;
sbit BED	= P1^3;

sbit	Tem_Up =	P3^2;
sbit	Tem_Down =	P1^5;

sbit	Tem_Off =	P1^4;
sbit	Tem_SW =	P1^6;

#endif