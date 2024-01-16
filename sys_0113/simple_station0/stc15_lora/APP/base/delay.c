#include "delay.h"
#include "typedef.h"

void Delay1ms(u16 count)		//@22.1184MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 22;
	j = 128;
	while(count--)
	{
		do
		{
			while (--j);
		} while (--i);
	}
}

void Delay1s(unsigned int ii)
{
  int j;
   while(ii--){
     for(j=0;j<1000;j++);
   }
}

void Delay100ms()		//@22.1184MHz
{
	unsigned char i, j, k;

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


void Delay300ms(void)	//@22.1184MHz
{
	unsigned char data i, j, k;

	_nop_();
	_nop_();
	i = 26;
	j = 55;
	k = 171;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}



void Delay1000ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 85;
	j = 12;
	k = 155;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);

}
//1s的延时在sx1278的库中有定义，避免有重复定义

