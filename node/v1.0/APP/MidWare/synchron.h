/**
 * @brief	�м����ר����ͳ��ϵͳͬ���ź�
 * @detail	��������	1. �ռ�������̨�ľ����ź�
 * 						2. ���ɽ�����һ���ڵ��ź�
 *						3. �����������ڵ�������ߵ��źţ���ǰ��������
 *						4. �źŵķ����������ļ�ִ��
 * @warning	���ļ�Ϊ������̨ר��
**/
 
#ifndef _SYN_H
#define _SYN_H
 
#include "uart.h"
#include "typedef.h"

#define Node

typedef union 
{
	u16 word;
	u8 byte[2];
}word2byte;

extern word2byte synPeriod;
 
u8 getReadySign(void);		//��ȡ�����ź�
//void advancePeriod(void);	//���ݻ�ȡ���ĸ����źţ��ƶ���������
//void signCreate(void);		//����ͬ�����ڣ������ź����ɱ�ʶ
 
#endif
 