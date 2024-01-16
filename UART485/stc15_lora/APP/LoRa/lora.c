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
	register_rf_func(&ctrlTypefunc);	//注册LoRa回调
	SX1276Reset();		//复位LoRa模块
	SX1276LORA_INT();
	
//	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//清空RxBuffer
}

void loraTx(u8 *dat, u8 len)
{
	SX1276WriteBuffer( REG_LR_IRQFLAGS, 0xff);						//清空RxBuffer
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
		LED1 = 0;			//通信开始，亮灯
		if ((RF_EX0_STATUS & 0x40) == 0x40) 
		{
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
		Delay1s(300);
	} 
	else 
	{
		LED1 = 1;				//通信结束，灭灯
	}
}


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
	u8 crcCheck = 0;
	if(lastPeriod != synPeriod)				//若周期发生更新
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

