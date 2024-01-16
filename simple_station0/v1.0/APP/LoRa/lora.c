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
	register_rf_func(&ctrlTypefunc);	//注册LoRa回调
	SX1276Reset();		//复位LoRa模块
	SX1276LORA_INT();
	
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//清空各种中断标志寄存器
}

void loraTx(u8 *dat, u8 len)
{
	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//清空各种中断标志寄存器
	FUN_RF_SENDPACKET(dat, len);
	Delay1s(500);
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);
}

//void loraTxByte(u8 dat)
//{
//	u8 array[10] = {0};
//	
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//清空各种中断标志寄存器
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
		if ((RF_EX0_STATUS & 0x40) == 0x40) //若接收结束
		{
			CRC_Value = SX1276ReadBuffer( REG_LR_MODEMCONFIG2);
			if (CRC_Value & 0x04 == 0x04) 			//若发送端开启了CRC
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
//				uartSendString(recv);				//测试使用
				LED1 = 0;
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
		Delay100ms();Delay100ms();
	}
	else
	{
		LED1 = 1;
		
	}
}


//对接新节点后，判断节点工作状态
void loraNodeCheck(void)
{
	if(currentNode != (synPeriod.word /11*8 + loraStationNum))			//若通信目标节点发生变化
	{
		LED1 = 0;
		do
		{
			loraRxData();
//			synReady = 0;
			overTime++;
		}while(recv[0] != nodeReadyFlag && overTime < 500);	//收到应答或等待超时，跳出
		
		currentNode = synPeriod.word /11*8 + loraStationNum;	//更新通信节点编号
		
		if(overTime < 500)										//若收到应答，超时次数清空
		{
			LED1 = 1;
			synReady = 1;											//节点就绪标志位，LED1仅为测试阶段使用
			overTime = 0;			
		}		
		else
		{
			synReady = 0;
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
	
	while(currentNode == (synPeriod.word /11*8 + loraStationNum))
	{
		if(lastPeriod != synPeriod.word)				//若周期发生更新
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
//			break;			//测试用
		
	}while((crcCheck & 0x02) != 0x02);		//若CRC校验失败
	
	loraStationFreqHopping();
	lastPeriod = synPeriod.word;
	LED0 = 1;
}


void loraStationFreqHopping(void)
{
//1号电台使用
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
	
//0号电台使用
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

