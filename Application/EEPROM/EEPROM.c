#include "EEPROM.h"
#include "stdio.h"//使用printf需要
#include "Uart.h"
/*Declare SFR associated with the IAP*/

//sfr IAP_DATA	=	0xC2;		//	Flash data register
//sfr IAP_ADDRH	= 0xC3;		// 	Flash address HIGH
//sfr IAP_ADDRL	= 0xC4;		// 	Flash address LOW
//sfr IAP_CMD		= 0xC5;		//	Flash command register
//sfr IAP_TRIG 	= 0xC6;		//	Flash command trigger
//sfr IAP_CONTR = 0xC7;		//	Flash control register

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
#define	DEBUG_DATA	0x1234		//存储在 EEPROM 单元的数值

#define	IAP_ADDRESS_L	0x0000	//EEPROM存入地址	总1K大小，2个扇区，开始地址0000，结束地址03ff
#define IAP_ADDRESS_H	0x0001

static void Delay(void);
static void IapIdle(void);
static uint8 IapReadByte(uint16 addr);
static void IapProgramByte(uint16 addr,uint8 dat);
static void IapEraseSector(uint16 addr);
#ifdef TEST_EEPROM
void EEPROM_Test(void)
{
		uint8 i,j;
		uint16	a = 0;
		printf("Erase start\r\n");
		IapEraseSector(IAP_ADDRESS_L);	//Erase
		IapEraseSector(IAP_ADDRESS_H);	//Erase
		printf("Erase over\r\n");
		IapProgramByte(IAP_ADDRESS_L,(uint8)(DEBUG_DATA>>8));//write
		IapProgramByte(IAP_ADDRESS_H,(uint8)DEBUG_DATA);//write
		printf("Program over\r\n");
		Delay();
		i =IapReadByte(IAP_ADDRESS_L);//read
		j =IapReadByte(IAP_ADDRESS_H);//read
		printf("CCC: %x \r\n",i,j);
		a=((uint16)i<<8)+(uint16)j;//数据整合
		printf("%x",a);
		while(1);
}
#endif
uint8 Data_preservation(uint16 dat)
{
	uint8 read_dat1,read_dat2;
	uint16 ver;
	
	IapEraseSector(IAP_ADDRESS_L);	//Erase
	IapEraseSector(IAP_ADDRESS_H);	//Erase
	Delay();
	IapProgramByte(IAP_ADDRESS_L,(uint8)(dat>>8));//write
	IapProgramByte(IAP_ADDRESS_H,(uint8)dat);//write
	Delay();
	read_dat1 =IapReadByte(IAP_ADDRESS_L);//read
	read_dat2 =IapReadByte(IAP_ADDRESS_H);//read
	
	ver = ((uint16)read_dat1<<8)+(uint16)read_dat2;
	if(ver == dat)
	{
		printf("program success;\r\n");
		return 1;
	}else
	{
		printf("program fail:%x",ver);
		return 0;
	}
}
uint16 Get_Temp_set(void)
{
	uint8 read_dat1,read_dat2;
	read_dat1 =IapReadByte(IAP_ADDRESS_L);//read
	read_dat2 =IapReadByte(IAP_ADDRESS_H);//read
	return (((uint16)read_dat1<<8)+(uint16)read_dat2);
}
/*----------------------------------------
Software delay function
-----------------------------------------*/
static void Delay(void)
{
    uint8 i;
    uint16 d=5000;
    while (d--)
    {
        i=255;
        while (i--);
    }
}
/*----------------------------------------
Disable ISP/IAP/EEPROM function
Make MCU in a safe state
-----------------------------------------*/
static void IapIdle(void)
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
uint8 IapReadByte(uint16 addr)
{
	uint8 dat;										//Data buffer
	
	IAP_CONTR = ENABLE_IAP;				//Open IAP function,and set wait time
	IAP_CMD 	= CMD_READ;					//Set ISP/IAP/EEPROM READ command
	IAP_ADDRL = addr;							//Set ISP/IAP/EEPROM address low
	IAP_ADDRH = addr>>8;					//Set ISP/IAP/EEPROM address high
	IAP_TRIG 	= 0x5a;							//Send trigger command1(0x5a)
	IAP_TRIG 	= 0xa5;							//Send trigger command2(0xa5)
	_nop_();											//MCU will hold here until ISP/IAP/EEPROM operation complete
	dat 			= IAP_DATA;					//Read ISP/IAP/EEPROM data
	IapIdle();										//Close ISP/IAP/EEPROM function
	return dat;										//Return Flash data
}
/*----------------------------------------
Program one byte to ISP/IAP/EEPROM area
Input: 	addr(ISP/IAP/EEPROM address)
				dat(ISP/IAP/EEPROM data)
Output:-
-----------------------------------------*/
void IapProgramByte(uint16 addr,uint8 dat)
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
void IapEraseSector(uint16 addr)
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