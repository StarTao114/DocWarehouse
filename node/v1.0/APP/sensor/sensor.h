#ifndef _SENSOR_H
#define _SENSOR
#include "typedef.h"

typedef struct _SENSORDATA
{
	u16 wet;			//wet和temperature，前一个字节放整数部分，后一个字节放小数部分
	u16 temperature;	//数值范围：0.0-255.255，够用了
	u8	power;
	u8	swi;
	u8	ID;
}sensorStruct;

//u16 getWet(void);
//u16 getTemperature(void);
//u16 getPower(void);
void getAllData(sensorStruct* dat);

#endif
