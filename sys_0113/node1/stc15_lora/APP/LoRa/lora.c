#include "lora.h"
#include "stdio.h"
#include "gpio.h"
#include "sx1278.h"
#include "delay.h"
#include "synchron.h"
#include "sensor.h"

extern u16 lastPeriod;
extern u8 synReady;
extern u16 currentNode;

u8 nodeBrokenFlag = 0;
unsigned char recv[20];
u8 overTime = 0;
unsigned char Frequency[23][3] = {	{ 0x6C, 0x80, 0x00 },
									{ 0x67, 0x74, 0xDE },
									{ 0x68, 0xB7, 0xA6 },
									{ 0x6B, 0x3D, 0x37 },
									{ 0x6D, 0xC2, 0xC8 },
									{ 0x6F, 0x05, 0x90 },
									{ 0x70, 0x48, 0x59 },
									{ 0x71, 0x8B, 0x21 },
									{ 0x72, 0xCD, 0xE9 },
									{ 0x74, 0x10, 0xB2 },
									{ 0x75, 0x53, 0x7A },
									{ 0x76, 0x96, 0x42 },
									{ 0x77, 0xD9, 0x0B },
									{ 0x79, 0x1B, 0xD3 },
									{ 0x7A, 0x5E, 0x9B },
									{ 0x7B, 0xA1, 0x64 },
									{ 0x7C, 0xE4, 0x2C },
									{ 0x7E, 0x26, 0xF4 },
									{ 0x7F, 0x69, 0xBD },
									{ 0x80, 0xAC, 0x85 },
									{ 0x81, 0xEF, 0x4D },
									{ 0x83, 0x32, 0x16 }};

void loraInit(void)
{
	register_rf_func(&ctrlTypefunc);	//注册LoRa回调
	SX1276Reset();		//复位LoRa模块
	SX1276LORA_INT();
	
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);
}

void loraTx(u8 *dat, u8 len)
{
	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);
	FUN_RF_SENDPACKET(dat, len);
	Delay1s(500);
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);
}

//void loraTxByte(u8 dat)
//{
//	u8 array[10] = {0};
//	
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//清空RxBuffer
//	sprintf(array, "%d", dat);
//	FUN_RF_SENDPACKET(array, 1);
//}

//void loraTxWord(u16 dat)
//{
//	u8 array[10] = {0};
//	
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//清空RxBuffer
//	sprintf(array, "%d", dat);
//	FUN_RF_SENDPACKET(array, 2);
//}

//void loraTx8Byte(u8 *arr)
//{
////	u8 array[10] = {0};
//	
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//清空RxBuffer
////	sprintf(array, "%d", dat);
//	FUN_RF_SENDPACKET(arr, 8);
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
	
	
	RF_EX0_STATUS = SX1276ReadBuffer( REG_LR_IRQFLAGS);
	if (RF_EX0_STATUS > 0) 
	{
		if ((RF_EX0_STATUS & 0x40) == 0x40) 
		{
			LED1 = 0;
			CRC_Value = SX1276ReadBuffer( REG_LR_MODEMCONFIG2);
			if (CRC_Value & 0x04 == 0x04) 	//若开启了CRC
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
				synPeriod.byte[0] = recv[0];
				synPeriod.byte[1] = recv[1];
				RF_RECEIVE();			
//				uartSendString(recv);				//测试使用
			}
		} 
		else if ((RF_EX0_STATUS & 0x04) == 0x04) 
		{
			if ((RF_EX0_STATUS & 0x01) == 0x01) //表示CAD 检测到扩频信号 模块进入了接收状态来接收数据
			{ 
				SX1276LoRaSetOpMode(Stdby_mode);
				SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_RXD_Value); //打开发送中断
				SX1276WriteBuffer(REG_LR_HOPPERIOD, PACKET_MIAX_Value);
				SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X02);
				SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0x00);
				SX1276LoRaSetOpMode(Receiver_mode);
			} 
			else 
			{                          
				SX1276LoRaSetOpMode(Stdby_mode);
				SX1276WriteBuffer(REG_LR_IRQFLAGSMASK,IRQN_SEELP_Value);   //打开发送中断
				SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00);
				SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0X00);
				SX1276LoRaSetOpMode(Sleep_mode);
				//PA_SEELP_OUT();
			}
		}
		else 
		{
			SX1276LoRaSetOpMode(Stdby_mode);
			SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_RXD_Value);  //打开发送中断
			SX1276WriteBuffer(REG_LR_HOPPERIOD, PACKET_MIAX_Value);
			SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0x02);
			SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0x00);
			SX1276LoRaSetOpMode(Receiver_mode);
		}
		RF_REC_RLEN_i = 0;
		SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);
		//Delay1ms(300);
		Delay100ms();
		Delay100ms();
		Delay100ms();
	}
	else
		LED1 = 1;
}


#if(loraStationNum < 8)
//对接新节点后，判断节点工作状态
void loraNodeCheck(void)
{
	if(currentNode != (synPeriod /11*8 + loraStationNum))			//若通信目标节点发生变化
	{
		if(recv[0] == nodeReadyFlag)	
		{
			LED2 = 0;
			synReady = 1;											//节点就绪标志位，LED2仅为测试阶段使用
			currentNode = synPeriod /11*8 + loraStationNum;			//更新通信节点编号
			overTime = 0;											//超时次数清空
			
		}
		else
		{
			LED2 = 1;
			synReady = 0;
			overTime++;
		}
		
		//若超时次数过多，或已经有两个周期没有收到就绪报文，则视为节点损坏
		//这部分需要统一加入看门狗
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
	if(lastPeriod != synPeriod)				//若周期发生更新
	{
		buffer[0]='C'; buffer[1]='N';
		loraTx(buffer, 2);
		loraRxData();
	}
	
	crcCheck = SX1276ReadBuffer(RegIrqFlags2);
	if((crcCheck & 0x02) != 0x02)
	{
		buffer[0]='C'; buffer[1]='R';
		loraTx(buffer, 2);
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
		
		if(KEY0 == 0)
			break;			//测试用
		
	}while((crcCheck & 0x02) != 0x02);		//若CRC校验失败
	
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
	loraTxByte(0xDD);
	Delay1ms(50);
	loraTxByte(0xDD);
	Delay1ms(50);
}


void loraNodeSleep(void)
{
	loraTxByte('S');
	Delay1ms(50);
	loraTxByte('S');
	Delay1ms(50);
}

//u16 loraCycle(u16 period)
//{
//	
//}

#endif


#ifdef loraNodeNum

extern u8 Freq[3];
void loraFreqHopping(void)
{
	do
	{
		loraRxData();
//		Delay100ms();
	}while(synPeriod.word != (loraNodeNum/8));	//当周期轮到自己通信时
	
	if(synPeriod.word % 2 == 0)		//判断是奇数组还是偶数组
	{
		Freq[0] = Frequency[loraNodeNum % 8+1][0];		//0号频段给独立电台长期占用了
		Freq[1] = Frequency[loraNodeNum % 8+1][1];
		Freq[2] = Frequency[loraNodeNum % 8+1][2];
	}
	else
	{
		Freq[0] = Frequency[loraNodeNum % 8+9][0];
		Freq[1] = Frequency[loraNodeNum % 8+9][1];
		Freq[2] = Frequency[loraNodeNum % 8+9][2];
	}
	
//		Freq[0] = 0x68;
//		Freq[1] = 0xB7;
//		Freq[2] = 0xA6;
	loraInit();
}


void loraFreqDisHopping(void)
{
	Freq[0] = 0x6c;
	Freq[1] = 0x80;
	Freq[2] = 0x00;
	loraInit();
}


void loraNodeAck(void)
{
	u8 buffer[8] = {0};
	
	buffer[0]=nodeReadyFlag;
	loraTx(buffer, 1);
	while(1)
	{
		loraRxData();
		if(recv[0] ==  'C' && recv[1] !=  'N')
		{
			buffer[0]=nodeReadyFlag;
			loraTx(buffer, 1);
		}
		else
			break;
		
		if(recv[0] ==  'C' && (recv[1] ==  'R' || recv[1] ==  'J'))
			break;		//预防措施
		Delay1ms(10);
	}
}

extern sensorStruct sensorData;
void loraNodeCommunicate(void)
{
	u8 i=0;
//	word2byte tx_buff[4];
	u8 tx_buff[8];
	u16 lastDataNum = 0;
	
	getAllData(&sensorData);
//	tx_buff[0] = (u8)(sensorData.wet >> 8);
//	tx_buff[1] = (u8)(sensorData.wet);
//	tx_buff[2] = (u8)(sensorData.temperature >> 8);
//	tx_buff[3] = (u8)(sensorData.temperature);
//	tx_buff[4] = sensorData.power;
//	tx_buff[5] = sensorData.swi;
//	tx_buff[6] = 0;
//	tx_buff[7] = 0;
	tx_buff[0] = 0x12;
	tx_buff[1] = 0x23;
	tx_buff[2] = 0x34;
	tx_buff[3] = 0x45;
	tx_buff[4] = 0x56;
	tx_buff[5] = 0x67;
	tx_buff[6] = 0x78;
	tx_buff[7] = 0x89;
	
	for(i=0; i<5; i++)//
	{
		loraTx(tx_buff, 8);
		lastDataNum++;
		Delay100ms();Delay100ms();//Delay300ms();				//太高的发送频率会导致射频部分发热严重，且通信质量差
		if(KEY1 == 0)
		{
			break;
		}
	}
}

#endif

