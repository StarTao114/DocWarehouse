#ifndef __GPIO_H
#define __GPIO_H

#define KEY0                                 P30
#define KEY1                                 P31

#define LED0                                 P36		//��������������˸
#define LED1                                 P37		//UART��������һ�����л�״̬
#define LED2                                 P16		//LoRa����ȷ����һ�Σ�ͨ��CRCУ�飩


void hardwareInit(void);
//#define UART_TX_PIN                          P31
//#define UART_TX_SET(n)                       UART_TX_PIN = n
//#define UART_TX_HIGH()                       UART_TX_SET(1)
//#define UART_TX_LOW()                        UART_TX_SET(0)
//#define UART_TX_FLIP()                       UART_TX_PIN = !UART_TX_PIN

//#define UART_RX_PIN                          P30
//#define UART_RX_SET(n)                       UART_RX_PIN = n
//#define UART_RX_HIGH()                       UART_RX_SET(1)
//#define UART_RX_LOW()                        UART_RX_SET(0)
//#define UART_RX_FLIP()                       UART_RX_PIN = !UART_RX_PIN









#endif 





