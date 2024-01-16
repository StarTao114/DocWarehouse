
#ifndef _GPIO_H
#define _GPIO_H

#include "typedef.h"
#include "gpio.h"

void hardwareInit(void)
{
	P1M1 = 0X03;		//P10和P11为准双向IO
	P1M0 = 0XFC;		//P12到P17推挽输出
	P3M1 &= 0XFC;		//P30和P31保持
	P3M0 |= 0X04;		//P32推挽输出
	
	LED1 = 1;
	LED0 = 1;
}

#endif
