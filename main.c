#include "Sys.h"
#include <math.h>
#include "STC12(ADC flag).h"
#include "Uart.h"
#include "LCD1602.h"
#include "key.h"

#include "Task.h"
extern void setup(void);
extern void loop(void);

void main()
{
	setup();
	loop();
	
//	BED = 0;
//	EEPROM_Test();
		while(1)
		{
//			BED = 1;
//			Beep = 0;
			delay_ms(3000);
//			BED = 0;
//			Beep = 1;
			delay_ms(3000);
		}
}