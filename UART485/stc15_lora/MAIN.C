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
u8 uartScheduler;			//串口资源调度标识

char contextBuffer[8]={12, 23, 34, '0'};
char te_buff[8] = "nzt!";
u16 wet=60, tem=26, swi=11, pwr=88;
u8 tx_buff[12]={0};
//p55p54, p32-35  IO空闲

void main(void) 
{
	synPeriod.word = 0;
	
	UartInit();
	KEY0 = 1; KEY1 = 1;
	
	while (1) 
	{
		P35 = 0;
		
		if(KEY0 == 0)
		{
			AUXR1 = 0x80;				//映射到P1617
			Delay5ms();
			uartSend(contextBuffer, 4);
			Delay10us();
			AUXR1 = 0x40;				//映射到P3637
		}
		
//		if(KEY1 == 0)
//		{
//			AUXR1 = 0x40;
//			uartSend(te_buff, 4);
//			Delay10us();
//		}
		
//		if((USART_RX_STA & 0x8000) != 0)
		if(USART_RX_STA & 0x8000)
		{
			uartSend(USART_RX_BUF, USART_RX_STA&0X3FFF);
			USART_RX_STA = 0;
		}
		
		if(USART_RX_STA&0x4000)
		{
			P33 = 0;
		}
		else
		{
			P33 = 1;
		}
		
		P32 = !P32;
//		if(!(USART_RX_STA&0x4000))	//若串口没有开始接收一个数据包
//		{
//			P35 = 1;
//			if(uartScheduler == 0)		//则串口资源需要服从调度
//			{
//				uartScheduler = 1;
//				AUXR1 = 0x80;
//				P34 = 0;
//			}
//			else
//			{
//				uartScheduler = 0;
//				AUXR1 = 0x40;
//				P33 = 0;
//			}
//				
//		}
//		Delay10us();
	}
}



