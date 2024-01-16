
#ifndef _GPIO_H
#define _GPIO_H

#include "typedef.h"
#include "gpio.h"

void hardwareInit(void)
{
	P1M1 = 0X03;		//P10��P11Ϊ׼˫��IO
	P1M0 = 0XFC;		//P12��P17�������
	P3M1 &= 0XFC;		//P30��P31����
	P3M0 |= 0X04;		//P32�������
	
	LED1 = 1;
	LED0 = 1;
}

#endif
