#include "lora.h"
#include "stdio.h"
#include "gpio.h"
#include "sx1278.h"
#include "synchron.h"

extern u16 lastPeriod;
extern u8 synReady;
extern u16 currentNode;

u8 nodeBrokenFlag = 0;
unsigned char recv[20];
u8 overTime = 0;

void loraInit(void)
{
	register_rf_func(&ctrlTypefunc);	//ע��LoRa�ص�
	SX1276Reset();		//��λLoRaģ��
	SX1276LORA_INT();
	
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//��ո����жϱ�־�Ĵ���
}

void loraTx(u8 *dat, u8 len)
{
	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//��ո����жϱ�־�Ĵ���
	FUN_RF_SENDPACKET(dat, len);
	Delay1s(500);
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);
}

//void loraTxByte(u8 dat)
//{
//	u8 array[10] = {0};
//	
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//��ո����жϱ�־�Ĵ���
//	sprintf(array, "%d", dat);
//	FUN_RF_SENDPACKET(array, 8);
//}

//void loraTxArray(u8 *arr, u8 len)
//{
//	while(len--)
//	{
//		loraTxByte(*arr);
//		arr++;
//	}
//}


void loraRxData(void)
{
	unsigned char RF_REC_RLEN_i = 0;
	u8 crcCheck = 0;
	
	RF_EX0_STATUS = SX1276ReadBuffer( REG_LR_IRQFLAGS);
	if (RF_EX0_STATUS > 0) 
	{
		if ((RF_EX0_STATUS & 0x40) == 0x40) //�����ս���
		{
			CRC_Value = SX1276ReadBuffer( REG_LR_MODEMCONFIG2);
			if (CRC_Value & 0x04 == 0x04) 			//�����Ͷ˿�����CRC
			{
				SX1276WriteBuffer(REG_LR_FIFOADDRPTR, 0x00);
				SX1278_RLEN = SX1276ReadBuffer(REG_LR_NBRXBYTES);
				lpTypefunc.lpSwitchEnStatus(enOpen);
				lpTypefunc.lpByteWritefunc(0x00);
				
				for (RF_REC_RLEN_i = 0; RF_REC_RLEN_i < SX1278_RLEN;RF_REC_RLEN_i++) 
				{
					recv[RF_REC_RLEN_i] = lpTypefunc.lpByteReadfunc();
				}
				lpTypefunc.lpSwitchEnStatus(enClose);
				recv[RF_REC_RLEN_i] = '\0';
				RF_RECEIVE();	
//				SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);		
//				uartSendString(recv);				//����ʹ��
				LED1 = 0;
			}
		} 
		else if ((RF_EX0_STATUS & 0x04) == 0x04) 
		{
			if ((RF_EX0_STATUS & 0x01) == 0x01) //��ʾCAD ��⵽��Ƶ�ź� ģ������˽���״̬����������
			{ 
				SX1276LoRaSetOpMode(Stdby_mode);
				SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_RXD_Value); //�򿪷����ж�
				SX1276WriteBuffer(REG_LR_HOPPERIOD, PACKET_MIAX_Value);
				SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X02);
				SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0x00);
				SX1276LoRaSetOpMode(Receiver_mode);
			} 
			else 
			{                          
				SX1276LoRaSetOpMode(Stdby_mode);
				SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,IRQN_SEELP_Value);   //�򿪷����ж�
				SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00);
				SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0X00);
				SX1276LoRaSetOpMode(Sleep_mode);
				//PA_SEELP_OUT();
			}
		}
		else 
		{
			SX1276LoRaSetOpMode(Stdby_mode);
			SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_RXD_Value);  //�򿪷����ж�
			SX1276WriteBuffer(REG_LR_HOPPERIOD, PACKET_MIAX_Value);
			SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0x02);
			SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0x00);
			SX1276LoRaSetOpMode(Receiver_mode);
		}
		RF_REC_RLEN_i = 0;
		SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);
		Delay100ms();Delay100ms();
	}
	else
	{
		LED1 = 1;
		
	}
}


//�Խ��½ڵ���жϽڵ㹤��״̬
void loraNodeCheck(void)
{
	if(currentNode != (synPeriod.word /11*8 + loraStationNum))			//��ͨ��Ŀ��ڵ㷢���仯
	{
		LED1 = 0;
		do
		{
			loraRxData();
//			synReady = 0;
			overTime++;
		}while(recv[0] != nodeReadyFlag && overTime < 500);	//�յ�Ӧ���ȴ���ʱ������
		
		currentNode = synPeriod.word /11*8 + loraStationNum;	//����ͨ�Žڵ���
		
		if(overTime < 500)										//���յ�Ӧ�𣬳�ʱ�������
		{
			LED1 = 1;
			synReady = 1;											//�ڵ������־λ��LED1��Ϊ���Խ׶�ʹ��
			overTime = 0;			
		}		
		else
		{
			synReady = 0;
		}
		
		//����ʱ�������࣬���Ѿ�����������û���յ��������ģ�����Ϊ�ڵ���
		//�ⲿ����Ҫͳһ���뿴�Ź�
//		if(overTime > bokenThreshold || (synPeriod - lastPeriod) > 1)	
//		{
//			nodeBrokenFlag = 1
//		}
	}
}

void loraNextPactet(void)
{
	u8 buffer[8] = {0};
	u8 crcCheck = 0;
	
	while(currentNode == (synPeriod.word /11*8 + loraStationNum))
	{
		if(lastPeriod != synPeriod.word)				//�����ڷ�������
		{
			buffer[0]='C'; buffer[1]='N';
			loraTx(buffer, 2);
			loraRxData();
		}
	}
	
	
	
	crcCheck = SX1276ReadBuffer(RegIrqFlags2);	
	if((crcCheck & 0x02) != 0x02)
	{
		buffer[0]='C'; buffer[1]='R';
		loraTx(buffer, 2);
		LED0 = 0;
	}
	
	do
	{
		loraRxData();
		crcCheck = SX1276ReadBuffer(RegIrqFlags2);
		if((synPeriod.word - lastPeriod) > 1)
		{
			lastPeriod = synPeriod.word;
			overTime++;
			return;
		}
		
//		if(KEY0 == 0)
//			break;			//������
		
	}while((crcCheck & 0x02) != 0x02);		//��CRCУ��ʧ��
	
	loraStationFreqHopping();
	lastPeriod = synPeriod.word;
	LED0 = 1;
}


void loraStationFreqHopping(void)
{
//1�ŵ�̨ʹ��
	if(synPeriod.word % 2 == 0)
	{
		Frequency[0] = 0x68;
		Frequency[1] = 0xB7;
		Frequency[2] = 0xA6;
	}
	else
	{
		Frequency[0] = 0x75;
		Frequency[1] = 0x53;
		Frequency[2] = 0x7A;
	}
	
//0�ŵ�̨ʹ��
//	if(synPeriod.word % 2 == 0)
//	{
//		Frequency[0] = 0x67;
//		Frequency[1] = 0x74;
//		Frequency[2] = 0xDE;
//	}
//	else
//	{
//		Frequency[0] = 0x74;
//		Frequency[1] = 0x10;
//		Frequency[2] = 0xB2;
//	}
	
//		Frequency[0] = 0x68;
//		Frequency[1] = 0xB7;
//		Frequency[2] = 0xA6;
	loraInit();
}

