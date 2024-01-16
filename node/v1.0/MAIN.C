#include "typedef.h"
#include "gpio.h"
#include "string.h"
#include "uart.h"
#include "SX1278.h"
#include "synchron.h"
#include "stdio.h"
#include "delay.h"
#include "lora.h"
#include "numtools.h"
#include "synchron.h"
#include "sensor.h"

//extern u8 USART_RX_BUF[USART_REC_LEN];
//extern u16 USART_RX_STA;
//extern u16 currentNode;
extern sensorStruct sensorData;

char contextBuffer[8]="";
u16 tx_buff[12]={0};

void main(void) 
{
	UartInit();
	hardwareInit();
	loraInit();
	
	while (1) 
	{
		loraFreqHopping();		//����������̨��Ƶ
		LED0 = 0;
		Delay1000ms();
		Delay1000ms();
		loraNodeCommunicate();
		loraFreqDisHopping();
		LED0 = 1;				//���ͽ���
		
		do
		{
			loraRxData();
		}while(synPeriod.word == (loraNodeNum/8));	//�õ�һ�������ں�����
		
		while(1)
		{
			//������ʱ������ʱ����
			//�������ߣ��ȴ�����
		}
		
		Delay1ms(10);

	}
}


