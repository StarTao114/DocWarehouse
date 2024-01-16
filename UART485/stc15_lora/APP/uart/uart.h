#ifndef __UART_H
#define __UART_H

#include "typedef.h"

#define UART_BUAD           115200

#define USART_REC_LEN		20

void UartInit(void);
void UART_SendData(unsigned char Byte);
void uartSend(char *s, char len);
//void uartSendString(char *pS, u8 num);




#endif
