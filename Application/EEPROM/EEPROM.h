#ifdef	EEPROM_H
#define EEPROM_H
#include "Sys.h"
extern uint8 IapReadByte(uint16 addr);
extern void IapProgramByte(uint16 addr,uint8 dat);
extern void IapEraseSector(uint16 addr);
void EEPROM_Test(void);

#endif