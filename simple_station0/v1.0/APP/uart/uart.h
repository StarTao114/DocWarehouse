#ifndef __UART_H
#define __UART_H

#include "typedef.h"

#define UART_BUAD                            115200

#define USART_REC_LEN		20

void UartInit(void);
void uartSendData(u8 ucData);
void UART_SendData(unsigned char Byte);
void uartSend(u8 *pS, u8 len);
//void uartSendString(char *pS, u8 num);




#endif
