#include "delay.h"
#include "typedef.h"

//void Delay1ms(u16 count)		//@22.1184MHz		//ʵ������350usһ�Σ���ԭʼ������ͬʱ����Ԫ
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


//1s����ʱ��sx1278�Ŀ����ж��壬�������ظ�����

