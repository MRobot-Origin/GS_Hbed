#ifdef	EEPROM_H
#define EEPROM_H
#include "Sys.h"
//#define TEST_EEPROM

#ifdef TEST_EEPROM
void EEPROM_Test(void);
#endif


uint8 Data_preservation(uint16 dat);
#endif