#include "typedef.h"
#include "gpio.h"
#include "uart.h"
#include "synchron.h"

static bit bUartFlag;
bit busy;

volatile u16 USART_RX_STA=0;       //接收状态标记	
volatile u8 USART_RX_BUF[USART_REC_LEN] = {0};

void UartInit(void)
{

	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器时钟1T模式
	T2L = (65536 - MCU_FREQ /4 / UART_BUAD);
	T2H = (65536 - MCU_FREQ /4 / UART_BUAD) >> 8;
	AUXR |= 0x10;		//定时器2开始计时
	AUXR1 = 0x80;
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
//	Delay10us();
    while (len--)                     
    {
        UART_SendData(*pS++);
//		Delay10us();
    }
	UART_SendData(0xbb);
}

u8 buffer[8]="qwer?";
void UART1_IRQ()    interrupt 4 using 1
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
		P34 = 0;
		
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
	else
		P34 = 1;
}

