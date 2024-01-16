#include "numtools.h"

#if NUMTOOLS

void num2ascii(u16 num, u8 *ascii_buff)
{
	ascii_buff[0] = (u8)(num / 100)+48;
	ascii_buff[1] = (u8)(num % 100 / 10)+48;
	ascii_buff[2] = (u8)(num % 10)+48;
	ascii_buff[3] = '\0';
	
}

#endif
