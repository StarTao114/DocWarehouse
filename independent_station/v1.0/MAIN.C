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

extern u8 USART_RX_BUF[USART_REC_LEN];
extern u16 USART_RX_STA;
extern u16 currentNode;
extern unsigned char recv[20];

char contextBuffer[8]="";
u16 wet=60, tem=26, swi=11, pwr=88;
u8 tx_buff[12]={0};
u8	cycle_num = 0, 
	stationReady = 0, 
	timeout = 0, 
	retryNum = 0;



void main(void) 
{
	synPeriod.word = 0;
	
	UartInit();
	hardwareInit();
	loraInit();
	
	while (1) 
	{
		loraCommunicateStart();			//唤醒节点
		
		while(synPeriod.word <= 25)
		{
			loraTx(synPeriod.byte, 2);
			uartSend(&synPeriod.byte, 2);
			retryNum++;
//			if(retryNum >= 500)
//			{
//				timeout = 1;
//			}
			stationReady = getReadySign();			//暂时用KEY0替代
			
			if((stationReady == 0xff) || (timeout == 1))
			{
				synPeriod.word++;
				timeout = 0;
				retryNum = 0;
			}
			
			if(synPeriod.word == 1)
				LED0 = 0;
			else
				LED0 = 1;			
			
			LED1 = !LED1;
			Delay1s(100);
		}

//		loraNodeSleep();
	}
}



