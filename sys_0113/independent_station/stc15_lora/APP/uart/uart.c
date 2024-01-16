#include "typedef.h"
#include "gpio.h"
#include "uart.h"
#include "synchron.h"

static bit bUartFlag;
bit busy;

u16 USART_RX_STA=0;       //接收状态标记	
u8 USART_RX_BUF[USART_REC_LEN] = {0};


//初始化串口
void UartInit(void)
{
	SCON = 0x50;	
	T2L = (65536 - MCU_FREQ /4 / UART_BUAD);
	T2H = (65536 - MCU_FREQ /4 / UART_BUAD) >> 8;
	AUXR = 0x14;
	AUXR |= 0x01;
	ES = 1;
	EA = 1;
	TI = 0;
}


void UART_SendData(unsigned char Byte)
{
	u8 temp;
	while(busy);
//	ACC = Byte;
	temp = Byte;
	TB8 = 0;
	
	busy = 1;
	SBUF = temp;
}

/******************************************************************************/
// 函数名称uartSendString 
// 输入参数:ps:字符串首地址
// 输出参数:无
// 串口功能:发送字符串通过串口输出
/******************************************************************************/
void uartSend(u8 *pS, u8 len)
{
    UART_SendData(0xaa);
    while (len--)                     
    {
        UART_SendData(*pS++);
    }
	UART_SendData(0xbb);
}


void UART_Routine()    interrupt 4 using 1
{
	u8 res = 0;
	if(TI)
	{
		TI=0;			//清除发送完成标志位
		busy = 0;
	}
	if(RI)
	{
		RI = 0;
		
		res = SBUF;
		
		
		if(USART_RX_STA&0x4000)						//若已经开始接收
		{
			
			if(res == 0xbb)							//若当前帧是结束帧
			{
				USART_RX_STA |= 0x8000;				//接收已完成
				USART_RX_BUF[USART_RX_STA&0X3FFF]='\0';
			}
			else
			{
				USART_RX_BUF[USART_RX_STA&0X3FFF]=res ;
				USART_RX_STA++;
//				synPeriod*=10;	synPeriod+=res;		//获得当前同步周期数
			}
		}
		
		if(res == 0xaa)								//起始帧
		{
			USART_RX_STA = 0;
			USART_RX_STA |= 0x4000;					//接收开始标志
		}
		
		if((USART_RX_STA&0X3FFF) > USART_REC_LEN)
		{
			USART_RX_STA = 0;
		}
			
		
	}
	
}
