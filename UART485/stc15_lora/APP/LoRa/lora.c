#include "lora.h"
#include "stdio.h"
#include "gpio.h"
#include "sx1278.h"
#include "delay.h"
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
	
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//���RxBuffer
}

void loraTx(u8 *dat, u8 len)
{
	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//���RxBuffer
	FUN_RF_SENDPACKET(dat, len);
	Delay1s(500);
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);
}


//void loraTxArray(u8 *arr, u8 len)
//{
//	while(len--)
//	{
//		loraTxByte(*arr);
//		arr++;
//	}
//}


#if(loraStationNum < 8)
void loraRxData(void)
{
	unsigned char RF_REC_RLEN_i = 0;
	
	
	RF_EX0_STATUS = SX1276ReadBuffer( REG_LR_IRQFLAGS);
	if (RF_EX0_STATUS > 0) 
	{
		LED1 = 0;			//ͨ�ſ�ʼ������
		if ((RF_EX0_STATUS & 0x40) == 0x40) 
		{
			CRC_Value = SX1276ReadBuffer( REG_LR_MODEMCONFIG2);
			if (CRC_Value & 0x04 == 0x04) 	//��������CRC
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
//				uartSendString(recv);				//����ʹ��
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
		Delay1s(300);
	} 
	else 
	{
		LED1 = 1;				//ͨ�Ž��������
	}
}


//�Խ��½ڵ���жϽڵ㹤��״̬
void loraNodeCheck(void)
{
	if(currentNode != (synPeriod /11*8 + loraStationNum))			//��ͨ��Ŀ��ڵ㷢���仯
	{
		if(recv[0] == nodeReadyFlag)	
		{
			LED2 = 0;
			synReady = 1;											//�ڵ������־λ��LED2��Ϊ���Խ׶�ʹ��
			currentNode = synPeriod /11*8 + loraStationNum;			//����ͨ�Žڵ���
			overTime = 0;											//��ʱ�������
			
		}
		else
		{
			LED2 = 1;
			synReady = 0;
			overTime++;
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
	u8 crcCheck = 0;
	if(lastPeriod != synPeriod)				//�����ڷ�������
	{
		loraTxByte('N');
		loraRxData();
	}
	
	crcCheck = SX1276ReadBuffer(RegIrqFlags2);
	if((crcCheck & 0x02) != 0x02)
	{
		loraTxByte('R');	
		LED2 = 1;
	}
	
	do
	{
		loraRxData();
		crcCheck = SX1276ReadBuffer(RegIrqFlags2);
		if((synPeriod - lastPeriod) > 1)
		{
			lastPeriod = synPeriod;
			overTime++;
			return;
		}
	}while((crcCheck & 0x02) != 0x02);		//��CRCУ��ʧ��
	
	loraNodeFreqHopping();
	lastPeriod = synPeriod;
	LED2 = 0;
}


void loraNodeFreqHopping(void)
{
	if(synPeriod % (packetLength + 1) == packetLength)
	{
		loraTxByte('J');
	}
}

#endif


#if(loraStationNum == 99)
void loraCommunicateStart(void)
{
	u8 arr[2] = {0xdd, '\0'};
	loraTx(arr, 1);
	Delay1s(800);
	loraTx(arr, 1);
	Delay1s(800);
}


void loraNodeSleep(void)
{
	u8 arr[2] = "S";
	
	loraTx(arr, 2);
	Delay1s(50);
	loraTx(arr, 2);
	Delay1s(50);
}

//u16 loraCycle(u16 period)
//{
//	
//}

#endif

