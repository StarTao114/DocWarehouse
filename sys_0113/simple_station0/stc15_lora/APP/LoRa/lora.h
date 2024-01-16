/**
* @brief	该文件下的程序是对SX1278库的二次开发，
			旨在实现LoRa的多机双向通信，并加以模块
			化封装
* @author	辰尘
* @date		2023年12月20日09:09:24
*/

#ifndef _LORA_H
#define _LORA_H

#include "typedef.h"

#define bokenThreshold				5			//判定为节点损坏的超时次数阈值
#define nodeReadyFlag				0x5a		//节点回传该数据以表示就绪
#define packetLength				10
#define loraStationNum				0			//当前电台编号

void loraInit(void);							//LoRa通信初始化
void loraTx(u8 *dat, u8 len);
//void loraTxByte(u8 period);						//LoRa发送一个字节
//void loraTxArray(u8 *arr, u8 len);				//LoRa发送字符串/数组
void loraRxData(void);
void loraNodeCheck(void);
void loraNextPactet(void);
void loraStationFreqHopping(void);

#endif