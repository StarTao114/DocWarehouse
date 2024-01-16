#include "delay.h"
#include "typedef.h"

//void Delay1ms(u16 count)		//@22.1184MHz		//实际上是350us一次，和原始保持相同时基单元
//{
//	int j;
//   while(count--){
//     for(j=0;j<1000;j++);
//   }
//}

void Delay10us(void)	//@22.1184MHz
{
	unsigned char data i;

	_nop_();
	_nop_();
	_nop_();
	i = 52;
	while (--i);
}


void Delay100ms(void)	//@22.1184MHz
{
	unsigned char data i, j, k;

	_nop_();
	_nop_();
	i = 9;
	j = 104;
	k = 139;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void Delay5ms(void)	//@22.1184MHz
{
	unsigned char data i, j;

	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
}


//1s的延时在sx1278的库中有定义，避免有重复定义

