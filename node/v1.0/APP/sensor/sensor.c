#include "sensor.h"

sensorStruct sensorData;

u16 getWet(void)
{
	return 0;
}

u16 getTemperature(void)
{
	return 0;
}

u16 getPower(void)
{
	return 0;
}

void getAllData(sensorStruct* dat)
{
	float temp[2] = {12.344f, 56.785f};
	dat->wet = (u16)(temp[0]*100+0.5f);
	dat->temperature = (u16)(temp[1]*100+0.5f);
	dat->power = 67;
	dat->swi = 16;
}

