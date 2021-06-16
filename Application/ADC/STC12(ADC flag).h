#ifndef _STC12_ADC_FLAG_H_
#define _STC12_ADC_FLAG_H_
#define uchar unsigned char
#define uint unsigned int
#define FOSC    11059200L
#define BAUD    9600
/********************************************************************
                             初始化设置
*********************************************************************/
//sfr ADC_CONTR   =   0xBC;           //ADC 控制寄存器
//sfr ADC_RES     =   0xBD;           //ADC 转换结果寄存器
//sfr ADC_RESL    =   0xBE;           //ADC 低2位结果寄存器
//sfr P1ASF       =   0x9D;           //P1口模拟功能控制寄存器
/********************************************************************
                              ADC 数据转换函数
*********************************************************************/
#define ADC_POWER   0x80            //ADC 电源控制位
#define ADC_FLAG    0x10            //ADC 转换结束标志位
#define ADC_START   0x08            //ADC 开始转换控制位
#define ADC_SPEEDLL 0x00            //540 个时钟周期转换一次
//#define ADC_SPEEDL  0x20            //360 个时钟周期转换一次
//#define ADC_SPEEDH  0x40            //180 个时钟周期转换一次
//#define ADC_SPEEDHH 0x60            //90 个时钟周期转换一次
#define COR   2.47										//P10端口标准参考电压
#define R_L  0.025											//导电材料的长度，单位	M
#define R_S  0.0000785										//导电材料的横截面积 单位平方米
//**********************************延时函数
extern void Delay1(uint n);
//******************************ADC初始化函数
void Init_ADC();
//***********************************ADC数据转换函数
uint Get_ADC_Result(uchar ch);
//************************************ADC数据计算函数
float Operating_Voltage();
float Count(uchar ch);
float Filtering_Voltage(uchar ch);
float Res_Calculation();				//计算电阻值，根据电阻分压计算
float p_Calculation();					//计算测量导体的电导值
#endif 