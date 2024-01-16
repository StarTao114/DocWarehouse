#ifndef _SENSOR_H
#define _SENSOR
#include "typedef.h"

typedef struct _SENSORDATA
{
	u16 wet;			//wet��temperature��ǰһ���ֽڷ��������֣���һ���ֽڷ�С������
	u16 temperature;	//��ֵ��Χ��0.0-255.255��������
	u8	power;
	u8	swi;
	u8	ID;
}sensorStruct;

//u16 getWet(void);
//u16 getTemperature(void);
//u16 getPower(void);
void getAllData(sensorStruct* dat);

#endif
