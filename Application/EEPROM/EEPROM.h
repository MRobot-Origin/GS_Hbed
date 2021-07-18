#ifdef	EEPROM_H
#define EEPROM_H
#include "Sys.h"

//void EEPROM_Test(void);

extern uint16	 MODE;

uint8 Data_preservation(uint16 dat1,uint16 dat2,uin16 mode);
uint16 Get_Temp_set1(void);
uint16 Get_Temp_set2(void);
uint16 Get_Mode(void);
#endif