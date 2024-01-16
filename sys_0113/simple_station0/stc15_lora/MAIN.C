#include "typedef.h"
#include "gpio.h"
#include "string.h"
#include "uart.h"
#include "SX1278.h"
#include "synchron.h"
#include "stdio.h"
#include "delay.h"
#include "lora.h"

extern u8 USART_RX_BUF[USART_REC_LEN];
extern u16 USART_RX_STA;
extern u16 currentNode;
extern u16 lastPeriod;

char contextBuffer[8]="";
u16 wet=60, tem=26, swi=11, pwr=88;
u8 tx_buff[12]={0};
u8 cycle_num = 0;

void main(void) 
{
	UartInit();
	hardwareInit();
	loraInit();
	synPeriod.word = 0;
	LED1 = 0;
	while (1) 
	{
		while(synPeriod.word < 25)
		{
			if( (synPeriod.word - lastPeriod) >= 1 )	//第n+1个周期开始
			{
				loraStationFreqHopping();
				lastPeriod = synPeriod.word;
				LED0 = !LED0;								//亮灯
			}
			else							//跳频结束
			{
//				LED0 = 1;								
			}
			loraRxData();		//常态接收
//			Delay100ms();
		}
		
	}
}






