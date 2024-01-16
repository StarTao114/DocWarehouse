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
#define loraStationNum				66			//当前电台编号
#define loraNodeNum					9			//当前节点编号

//通用
void loraInit(void);							//LoRa通信初始化
void loraTx(u8 *dat, u8 len);
//void loraTxByte(u8 period);						//LoRa发送一个字节
//void loraTxArray(u8 *arr, u8 len);				//LoRa发送字符串/数组
void loraRxData(void);
void loraTxWord(u16 dat);
void loraTx8Byte(u8 *arr);

//普通电台专用
void loraNodeCheck(void);
void loraNextPactet(void);
void loraNodeFreqHopping(void);

//独立电台专用
void loraCommunicateStart(void);
void loraNodeSleep(void);

//节点专用
void loraFreqHopping(void);
void loraFreqDisHopping(void);
void loraNodeAck(void);
void loraNodeCommunicate(void);

#endif