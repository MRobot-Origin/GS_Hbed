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
#define	DEBUG_DATA1	0x9876		//存储在 EEPROM 单元的数值
#define DEBUG_DATA2 0x8765
#define DEBUG_DATA3 1

#define	IAP_ADDRESS1_L		0x0000	//EEPROM存入地址	总1K大小，2个扇区，开始地址0000，结束地址03ff
#define IAP_ADDRESS1_H		0x0001
#define	IAP_ADDRESS2_L		0x0002	//EEPROM存入地址	总1K大小，2个扇区，开始地址0000，结束地址03ff
#define IAP_ADDRESS2_H		0x0003
#define	IAP_ADDRESS_MODE_L	0x0004
#define	IAP_ADDRESS_MODE_H	0x0005
static void Delay(void);
static void IapIdle(void);
static uint8 IapReadByte(uint16 addr);
static void IapProgramByte(uint16 addr,uint8 dat);
static void IapEraseSector(uint16 addr);

/*void EEPROM_Test(void)
{
		uint8 i,j,m,n,x,y;
		uint16	a,b,c;
		printf("Erase start\r\n");
		IapEraseSector(IAP_ADDRESS1_L);	//Erase
		IapEraseSector(IAP_ADDRESS1_H);	//Erase
		IapEraseSector(IAP_ADDRESS2_L);	//Erase
		IapEraseSector(IAP_ADDRESS2_H);	//Erase
		IapEraseSector(IAP_ADDRESS_MODE_L);	//Erase
		IapEraseSector(IAP_ADDRESS_MODE_H);	//Erase
		printf("Erase over\r\n");
	
		IapProgramByte(IAP_ADDRESS1_L,(uint8)(DEBUG_DATA1>>8));//write
		IapProgramByte(IAP_ADDRESS1_H,(uint8)DEBUG_DATA1);//write
	
		IapProgramByte(IAP_ADDRESS2_L,(uint8)(DEBUG_DATA2>>8));//write
		IapProgramByte(IAP_ADDRESS2_H,(uint8)DEBUG_DATA2);//write
	
		IapProgramByte(IAP_ADDRESS_MODE_L,(uint8)DEBUG_DATA3>>8);//write
		IapProgramByte(IAP_ADDRESS_MODE_H,(uint8)DEBUG_DATA3);//write
		printf("Program over\r\n");
		Delay();
		i = IapReadByte(IAP_ADDRESS1_L);//read
		j = IapReadByte(IAP_ADDRESS1_H);//read
		
		m = IapReadByte(IAP_ADDRESS2_L);//read
		n = IapReadByte(IAP_ADDRESS2_H);//read
		
		x = IapReadByte(IAP_ADDRESS_MODE_L);//read
		y = IapReadByte(IAP_ADDRESS_MODE_H);//read
		
		printf("CCC1: %x \r\n",i,j);
		printf("CCC2: %x \r\n",m,n);
		printf("mode: %x \r\n",x,y);
		
		a=((uint16)i<<8)+(uint16)j;//数据整合
		b=((uint16)m<<8)+(uint16)n;//数据整合
		c=((uint16)x<<8)+(uint16)y;
		printf("a:%x,b:%x c:%x \r\n",a,b,c);
		while(1);
}*/
uint8 Data_preservation(uint16 dat1,uint16 dat2,uint16 mode)
{
	uint8 read_dat1,read_dat2,read_dat3,read_dat4,read_dat5,read_dat6;
	uint16 ver1,ver2,ver3;
	
	IapEraseSector(IAP_ADDRESS1_L);	//Erase
	IapEraseSector(IAP_ADDRESS1_H);	//Erase
	IapEraseSector(IAP_ADDRESS2_L);	//Erase
	IapEraseSector(IAP_ADDRESS2_H);	//Erase
	IapEraseSector(IAP_ADDRESS_MODE_L);	//Erase
	IapEraseSector(IAP_ADDRESS_MODE_H);	//Erase
	
	Delay();
	IapProgramByte(IAP_ADDRESS1_L,(uint8)(dat1>>8));//write
	IapProgramByte(IAP_ADDRESS1_H,(uint8)dat1);//write
	
	IapProgramByte(IAP_ADDRESS2_L,(uint8)(dat2>>8));//write
	IapProgramByte(IAP_ADDRESS2_H,(uint8)dat2);//write
	
	IapProgramByte(IAP_ADDRESS_MODE_L,(uint8)mode>>8);//write
	IapProgramByte(IAP_ADDRESS_MODE_H,(uint8)mode);//write
	
	Delay();
	read_dat1 = IapReadByte(IAP_ADDRESS1_L);//read
	read_dat2 = IapReadByte(IAP_ADDRESS1_H);//read
	
	read_dat3 = IapReadByte(IAP_ADDRESS2_L);//read
	read_dat4 = IapReadByte(IAP_ADDRESS2_H);//read
	
	read_dat5 = IapReadByte(IAP_ADDRESS_MODE_L);//read
	read_dat6 = IapReadByte(IAP_ADDRESS_MODE_H);//read
	ver1 = ((uint16)read_dat1<<8)+(uint16)read_dat2;
	ver2 = ((uint16)read_dat3<<8)+(uint16)read_dat4;
	ver3 = ((uint16)read_dat5<<8)+(uint16)read_dat6;
	if((ver1 == dat1)&&(ver2 == dat2)&&(ver3 == mode))
	{
		printf("program success;\r\n");
		return 1;
	}else
	{
		printf("program fail:%x ,%x ,%x\r\n",ver1,ver2,ver3);
		return 0;
	}
}
uint16 Get_Temp_set1(void)
{
	uint8 read_dat1,read_dat2;
	read_dat1 =IapReadByte(IAP_ADDRESS1_L);//read
	read_dat2 =IapReadByte(IAP_ADDRESS1_H);//read
	return (((uint16)read_dat1<<8)+(uint16)read_dat2);
}
uint16 Get_Temp_set2(void)
{
	uint8 read_dat1,read_dat2;
	read_dat1 =IapReadByte(IAP_ADDRESS2_L);//read
	read_dat2 =IapReadByte(IAP_ADDRESS2_H);//read
	return (((uint16)read_dat1<<8)+(uint16)read_dat2);
}
uint16 Get_Mode(void)
{
	uint8 read_dat1,read_dat2;
	read_dat1 =IapReadByte(IAP_ADDRESS_MODE_L);//read
	read_dat2 =IapReadByte(IAP_ADDRESS_MODE_H);//read
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