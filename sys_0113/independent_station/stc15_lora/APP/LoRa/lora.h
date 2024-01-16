/**
* @brief	���ļ��µĳ����Ƕ�SX1278��Ķ��ο�����
			ּ��ʵ��LoRa�Ķ��˫��ͨ�ţ�������ģ��
			����װ
* @author	����
* @date		2023��12��20��09:09:24
*/

#ifndef _LORA_H
#define _LORA_H

#include "typedef.h"

#define bokenThreshold				5			//�ж�Ϊ�ڵ��𻵵ĳ�ʱ������ֵ
#define nodeReadyFlag				0xee		//�ڵ�ش��������Ա�ʾ����
#define packetLength				10
#define loraStationNum				99			//��ǰ��̨���

void loraInit(void);							//LoRaͨ�ų�ʼ��
void loraTx(u8 *dat, u8 len);					//LoRa�����ַ���/����
void loraRxData(void);


void loraNodeCheck(void);
void loraNextPactet(void);
void loraNodeFreqHopping(void);



void loraCommunicateStart(void);
void loraNodeSleep(void);


#endif