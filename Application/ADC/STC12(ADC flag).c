#include "STC12C5A60S2.h"
#include "intrins.h"
#include "STC12(ADC flag).h"
#include <math.h>
float temp;
float R1=23.84,R2=23.65,R3=24.05,R4=23.65,R5=23.7,R0_1,R0_2,R0_3,R0_4;
//int c;           
//**********************************延时函数(n=1: 1T 8.65ms,12T 59.9ms)
void Delay1(uint n)
{
    uint x;
    while (n--)
    {
        x = 5000;
        while (x--);
    }
}
//******************************ADC初始化函数
void Init_ADC()
{
    P1ASF = 0x07;                      //设置P1口为开漏模式，使用AD功能
    ADC_RES = 0;                       //AD数据寄存器高8位清空
		ADC_RESL= 0;        //AD数据寄存器低2位清空
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL;  //打开AD电源，转换周期540
    Delay1(2);                        //延时
}
//***********************************ADC数据转换函数
uint Get_ADC_Result(uchar ch)
{
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ADC_START | ch ;//开始转换
    _nop_();                          //延时一个机器周期
    _nop_();        //延时一个机器周期
    _nop_();        //延时一个机器周期
    _nop_();        //延时一个机器周期
    while (!(ADC_CONTR & ADC_FLAG));  //等待转换结束(ADC_FLAG=0时一直等待，直到变为1跳出)
    ADC_CONTR &= ~ADC_FLAG;           //关闭AD转换
    return (ADC_RES*4+ADC_RESL);            //返回数据（10位AD值，ADC_RES高8位+ADC_RESL低2位）
}
/////////////////////P1.0接入的是标准的参考电压，经测量恒定为2.47V/////////////////
//////可作为系统修正值实时修正系统的参考电压保证测量精度/////////////////////
float Operating_Voltage()
	{
		float V0;
		float sv=0.895;//2.465
		V0=(1024*sv)/(float)Get_ADC_Result(0);//根据标准参考电压计算芯片的工作电压（系统的参考电压）
		return V0;
	}
	//************************************ADC数据计算函数
float Count(uchar ch)
{
	temp=(float)Get_ADC_Result(ch)*Operating_Voltage()/1024; //4.85为参考电压，1024个“门”，10位AD
	return temp;
}
///////////////////////////////连续测量十次取平均值返回，保证所得数据的稳定性/////////////////////////
float Filtering_Voltage(uchar ch)
{
	float sum=0,average;
	uchar i;
	for(i=0;i<10;i++)
	{
		sum=sum+Count(ch);
	}
	average=sum/10;
	return average;
}

float Res_Calculation()//根据电阻分压原理计算测量导体的电阻值
{
	float R0,R;
		R0_1=R1/((Filtering_Voltage(1)-Filtering_Voltage(2))/Filtering_Voltage(5));
		R0_2=R2/((Filtering_Voltage(2)-Filtering_Voltage(3))/Filtering_Voltage(5));
		R0_3=R3/((Filtering_Voltage(3)-Filtering_Voltage(4))/Filtering_Voltage(5));
		R0_4=R4/((Filtering_Voltage(4)-Filtering_Voltage(5))/Filtering_Voltage(5));
		
		R0=(R0_1+R0_2+R0_3+R0_4)/4;//求平均值得出最终的测得的RO（R5与导体并联之后的电阻）
		
	R=((R5*R0)/(R5-R0))-1.07;//求得测量阻值,,//1.08为修正值
	if (R>2000)
		return 0;
	else
		return R;
}

float p_Calculation()
{
	float p;
	if(Res_Calculation()==0)
	{return 0;}
	else
	{
		p=(Res_Calculation()*R_S)*1000/R_L;//计算测量导体的电阻率单位为 mΩ/m
	  return p;
	}
	
}
//////////////////////////////////////////////////////////////////////////////////////////////////
