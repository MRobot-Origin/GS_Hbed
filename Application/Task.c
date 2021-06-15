#include <reg52.h>
//#include <STC12C5A60S2.H>
#include "intrins.h"

#include "Sys.h"
#include "stdio.h"//使用printf需要
//#include "DS18B20.h"
#include "Uart.h"
//#include "LCD1602.h"
//#include "key.h"
/*Declare SFR associated with the IAP*/

sfr IAP_DATA	=	0xC2;		//	Flash data register
sfr IAP_ADDRH	= 0xC3;		// 	Flash address HIGH
sfr IAP_ADDRL	= 0xC4;		// 	Flash address LOW
sfr IAP_CMD		= 0xC5;		//	Flash command register
sfr IAP_TRIG 	= 0xC6;		//	Flash command trigger
sfr IAP_CONTR = 0xC7;		//	Flash control register

/*Define ISP/IAP/EEPROM command*/
#define	CMD_IDLE		0		//Stand-By
#define	CMD_READ		1		//Byte-Read
#define	CMD_PROGRAM	2		//Byte-Program
#define	CMD_ERASE		3		//Sector_Erase

/*Define ISP/IAP/EEPROM operation const for IAP_CONTR*/
//#define ENABLE_IAP		0x80		//if SYSCLK<30MHZ
//#define ENABLE_IAP		0x81		//if SYSCLK<24MHZ
#define ENABLE_IAP		0x82		//if SYSCLK<20MHZ
//#define ENABLE_IAP		0x83		//if SYSCLK<12MHZ
//#define ENABLE_IAP		0x84		//if SYSCLK<6MHZ
//#define ENABLE_IAP		0x85		//if SYSCLK<3MHZ
//#define ENABLE_IAP		0x86		//if SYSCLK<2MHZ
//#define ENABLE_IAP		0x87		//if SYSCLK<1MHZ
//Start address for STC12C5A60S2 series EEPROM
#define	IAP_ADDRESS	0x0000
void Delay(uint8 n);
void IapIdle();
uint8 IapReadByte(uint8 addr);
void IapProgramByte(uint8 addr,uint8 dat);
void IapEraseSector(uint8 addr);
void main()
{
		uint8 i;
		Uart_Init();				//串口初始化
	
		P1 = 0xfe;										//1111,1110 System Reset OK
		Delay(10);										//Delay
		IapEraseSector(IAP_ADDRESS);	//Erase current sector
		for(i=0;i<512;i++)
		{
			if(IapReadByte(IAP_ADDRESS)!=0xff)
				printf("Erase error\r\n");
				goto Error;								//if error,break
		}
		printf("Erase OK\r\n");
		P1 = 0xfc;										//1111,1100 Erase successful
		Delay(10);										//Delay
		for(i=0;i<512;i++)						//Verify 512 bytes data
		{
			if(IapReadByte(IAP_ADDRESS+i)!=(uint8)i)
				printf("Verify error\r\n");
				goto Error;								//if error,break
		}
		printf("Verify OK\r\n");
		P1 = 0xf0;										//1111,0000 Verify successful
		while(1);
	Error:
		P1 &= 0x7f;									//0xxx,xxxx IAP operation fail
		while(1);
}
/*----------------------------------------
Software delay function
-----------------------------------------*/
void Delay(uint8 n)
{
	uint8 x;
	while(n--)
	{
		x = 0;
		while(++x);
	}
}
/*----------------------------------------
Disable ISP/IAP/EEPROM function
Make MCU in a safe state
-----------------------------------------*/
void IapIdle()
{
	IAP_CONTR = 0;				//Close IAP function
	IAP_CMD		= 0;				//Clear command to standby
	IAP_TRIG	= 0;				//Clear trigger register
	IAP_ADDRH	=	0x80;			//Data ptr point to non-EEPROM area
	IAP_ADDRL	=	0;				//Clear IAP address to prevent misuse
}
/*----------------------------------------
Read one byte from ISP/IAP/EEPROM area
Input:addr(ISP/IAP/EEPROM address)
Output:Flash data
-----------------------------------------*/
uint8 IapReadByte(uint8 addr)
{
	uint8 dat;											//Data buffer
	
	IAP_CONTR = ENABLE_IAP;				//Open IAP function,and set wait time
	IAP_CMD = CMD_READ;						//Set ISP/IAP/EEPROM READ command
	IAP_ADDRL = addr;							//Set ISP/IAP/EEPROM address low
	IAP_ADDRH = addr>>8;					//Set ISP/IAP/EEPROM address high
	IAP_TRIG = 0x5a;							//Send trigger command1(0x5a)
	IAP_TRIG = 0xa5;							//Send trigger command2(0xa5)
	_nop_();											//MCU will hold here until ISP/IAP/EEPROM operation complete
	dat = IAP_DATA;								//Read ISP/IAP/EEPROM data
	IapIdle();										//Close ISP/IAP/EEPROM function
	return dat;										//Return Flash data
}
/*----------------------------------------
Program one byte to ISP/IAP/EEPROM area
Input: 	addr(ISP/IAP/EEPROM address)
				dat(ISP/IAP/EEPROM data)
Output:-
-----------------------------------------*/
void IapProgramByte(uint8 addr,uint8 dat)
{
	IAP_CONTR = ENABLE_IAP;				//Open IAP function,and set wait time
	IAP_CMD 	= CMD_PROGRAM;				//Set ISP/IAP/EEPROM READ command
	IAP_ADDRL = addr;							//Set ISP/IAP/EEPROM address low
	IAP_ADDRH = addr>>8;					//Set ISP/IAP/EEPROM address high
	IAP_DATA 	= dat;							//Write ISP/IAP/EEPROM data
	IAP_TRIG 	= 0x5a;							//Send trigger command1(0x5a)
	IAP_TRIG 	= 0xa5;							//Send trigger command2(0xa5)
	_nop_();											//MCU will hold here until ISP/IAP/EEPROM operation complete
	IapIdle();										//Close ISP/IAP/EEPROM function
}
/*----------------------------------------
Erase one sector area
Input: 	addr(ISP/IAP/EEPROM address)
Output:-
-----------------------------------------*/
void IapEraseSector(uint8 addr)
{
	IAP_CONTR = ENABLE_IAP;				//Open IAP function,and set wait time
	IAP_CMD 	= CMD_ERASE;				//Set ISP/IAP/EEPROM READ command
	IAP_ADDRL = addr;							//Set ISP/IAP/EEPROM address low
	IAP_ADDRH = addr>>8;					//Set ISP/IAP/EEPROM address high
	IAP_TRIG 	= 0x5a;							//Send trigger command1(0x5a)
	IAP_TRIG 	= 0xa5;							//Send trigger command2(0xa5)
	_nop_();											//MCU will hold here until ISP/IAP/EEPROM operation complete
	IapIdle();										//Close ISP/IAP/EEPROM function
}