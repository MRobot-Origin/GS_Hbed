#include <reg52.h>
#include "key.h"
#include "Sys.h"


//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，KEY3按下
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3...!!
uint8 Key_Scan(uint8 mode)//按键检测
{
	static uint8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(Key1==0||Key2==0||Key3==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(Key1==0)return 1;
		else if(Key2==0)return 2;
		else if(Key3==0)return 3;
	}else if(Key1==1&&Key2==1&&Key3==1)key_up=1;
 	return 0;// 无按键按下
}

