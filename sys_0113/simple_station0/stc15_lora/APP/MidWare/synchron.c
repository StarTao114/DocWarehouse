#include "synchron.h"
#include "gpio.h"

//����ı����ᱻ������д���������жϣ���ϣ����ÿ�β�д����������ȫ����Ч
volatile word2byte synPeriod; 
u16 lastPeriod = 0; 
u8 currentNode = 0;//��ǰͨ�ŵ�Ŀ��ڵ���
u8 synReady = 0;


#ifdef specialStation
//��ʱ�ð�����������ź�
u8 getReadySign(void)
{
	if(KEY0 == 0)
	{
		synReady = 1;
	}
	return 0;
}

void advancePeriod(void)
{
	if(synReady == 1)
	{
		synPeriod+=10;//�������
		synReady = 0;
	}
}

void signCreate(void)
{
	
}

#endif
