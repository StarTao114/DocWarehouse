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
		loraFreqHopping();		//监听独立电台跳频
		LED0 = 0;
		Delay1000ms();
		Delay1000ms();
		loraNodeCommunicate();
		loraFreqDisHopping();
		LED0 = 1;				//发送结束
		
		do
		{
			loraRxData();
		}while(synPeriod.word == (loraNodeNum/8));	//得到一个新周期后，跳出
		
		while(1)
		{
			//留给定时器，定时重启
			//或者休眠，等待唤醒
		}
		
		Delay1ms(10);

	}
}



