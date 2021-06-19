#include "Thermistortables.h"
#include <math.h>
#include "STC12(ADC flag).h"
static const float Rp = 100000.0;				//100K
static const float T2 = (273.15+25.0);	//T2
static const float Bx = 3950.0;					//B
static const float	Ka = 273.15;				//K

float Rt=0;//全局变量
float Temp=0;//全局变量
//void Tem_calculation(void);
void Tem_calculation(void)//计算温度值
{
	float V = Filtering_Voltage(2);
	Rt = (Rp*V)/(Operating_Voltage()-V);
	Temp = (1/(log(Rt/Rp)/Bx + (1/T2)))-Ka+0.5;
}
