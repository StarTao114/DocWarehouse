
/******************************************************************************/
/*                        --安信可科技有限公司--                        */
/*
//  函数说明：main.c                                                              
//  说明：该程序用于测试lora模块                                                                
//  编写人员： sky                                                                 
// 编写日期： 2016-11-24                                                         
// 版本号:V1.0
// 维护日志：
//
//                                                          
*/ 
// 免责声明：该程序仅供参考学习，若自定义其他功能，请查看参考手册
// (c) Anxinke Corporation. All rights reserved.                                                               
/******************************************************************************/
#include "sx1278.h"

//#include "reg51.hcd"
lpCtrlTypefunc_t lpTypefunc = { 0, 0, 0 };
unsigned char power_data[8] = { 0X80, 0X80, 0X80, 0X83, 0X86, 0x89, 0x8c, 0x8f };

lpCtrlTypefunc_t ctrlTypefunc =
		{ RF_SPI_MasterIO, RF_SPI_READ_BYTE, cmdSwitchEn,

		};

/**
* @brief	新版分频方案，以434MHz为基准，频间距5.04MHz，频宽100Hz
* @date		01/10/24
*/
//unsigned char Frequency[3] = { 0x67, 0x74, 0xDE };	//413826088		单位：Hz
//unsigned char Frequency[3] = { 0x68, 0xB7, 0xA6 };	//418869566 
//unsigned char Frequency[3] = { 0x69, 0xFA, 0x6F };	//423913044 
//unsigned char Frequency[3] = { 0x6B, 0x3D, 0x37 };	//428956522 
unsigned char Frequency[3] = { 0x6C, 0x80, 0x00 };	//434000000 
//unsigned char Frequency[3] = { 0x6D, 0xC2, 0xC8 };	//439043478 
//unsigned char Frequency[3] = { 0x6F, 0x05, 0x90 };	//444086956 
//unsigned char Frequency[3] = { 0x70, 0x48, 0x59 };	//449130434 
//unsigned char Frequency[3] = { 0x71, 0x8B, 0x21 };	//454173912 
//unsigned char Frequency[3] = { 0x72, 0xCD, 0xE9 };	//459217390 
//unsigned char Frequency[3] = { 0x74, 0x10, 0xB2 };	//464260868 
//unsigned char Frequency[3] = { 0x75, 0x53, 0x7A };	//469304346 
//unsigned char Frequency[3] = { 0x76, 0x96, 0x42 };	//474347824 
//unsigned char Frequency[3] = { 0x77, 0xD9, 0x0B };	//479391302 
//unsigned char Frequency[3] = { 0x79, 0x1B, 0xD3 };	//484434780 
//unsigned char Frequency[3] = { 0x7A, 0x5E, 0x9B };	//489478258 
//unsigned char Frequency[3] = { 0x7B, 0xA1, 0x64 };	//494521736 
//unsigned char Frequency[3] = { 0x7C, 0xE4, 0x2C };	//499565214 
//unsigned char Frequency[3] = { 0x7E, 0x26, 0xF4 };	//504608692 
//unsigned char Frequency[3] = { 0x7F, 0x69, 0xBD };	//509652170 
//unsigned char Frequency[3] = { 0x80, 0xAC, 0x85 };	//514695648 
//unsigned char Frequency[3] = { 0x81, 0xEF, 0x4D };	//519739126 
//unsigned char Frequency[3] = { 0x83, 0x32, 0x16 };	//524782604 



unsigned char powerValue = 7;
unsigned char SpreadingFactor = 12;    //扩频因子7-12


//unsigned char CodingRate = 2;        //1-4
unsigned char CodingRate = 1;        //1-4


unsigned char Bw_Frequency = 7;      //带宽6-9,7--125K
unsigned char RF_EX0_STATUS;
unsigned char CRC_Value;
unsigned char SX1278_RLEN;
//unsigned char recv[512];


void Delay1s(unsigned int ii)
{
  int j;
   while(ii--){
     for(j=0;j<1000;j++);
   }
}

/*
function:SX1278 restart
*/

void  SX1276Reset(void) {
	RF_REST_L;
	Delay1s(200);
	RF_REST_H;
	Delay1s(500);
}


/*
function:control GPIO-EN
*/
void  cmdSwitchEn(cmdEntype_t cmd) {
	switch (cmd) {
	case enOpen: {
		RF_CE_L;
	}
		break;
	case enClose: {
		RF_CE_H;
	}
		break;
	default:
		break;
	}
}




void  RF_SPI_MasterIO(unsigned char out) {
	unsigned char i;
	for (i = 0; i < 8; i++) {
		if (out & 0x80) /* check if MSB is high */
			RF_SDI_H;
		else
			RF_SDI_L; /* if not, set to low */

		RF_CKL_H; /* toggle clock high */
		out = (out << 1); /* shift 1 place for next bit */
		RF_CKL_L; /* toggle clock low */
	}
}

unsigned char   RF_SPI_READ_BYTE() {
	unsigned char j;
	unsigned char i;
	j = 0;
	for (i = 0; i < 8; i++) {
		RF_CKL_H;
		j = (j << 1);			// shift 1 place to the left or shift in 0 //
		if ( SX1278_SDO)					// check to see if bit is high //
			j = j | 0x01; 					   // if high, make bit high //
											   // toggle clock high //
		RF_CKL_L; 							 // toggle clock low //
	}

	return j;								// toggle clock low //
}
unsigned char  SX1276ReadBuffer(unsigned char addr) {
	unsigned char Value;
	lpTypefunc.lpSwitchEnStatus(enOpen); //NSS = 0;
	lpTypefunc.lpByteWritefunc(addr & 0x7f);
	Value = lpTypefunc.lpByteReadfunc();
	lpTypefunc.lpSwitchEnStatus(enClose); //NSS = 1;

	return Value;
}

/*
function:set sx1278 work mode ,here you can control the mode which is send or recieve 
parm: 
*/
void  SX1276LoRaSetOpMode(RFMode_SET opMode) {
	unsigned char opModePrev;
	opModePrev = SX1276ReadBuffer(REG_LR_OPMODE);
	opModePrev &= 0xf8;
	opModePrev |= (unsigned char) opMode;
	SX1276WriteBuffer( REG_LR_OPMODE, opModePrev);
}



void  SX1276WriteBuffer(unsigned char addr,
		unsigned char buffer) {
	lpTypefunc.lpSwitchEnStatus(enOpen); //NSS = 0;
	lpTypefunc.lpByteWritefunc(addr | 0x80);
	lpTypefunc.lpByteWritefunc(buffer);
	lpTypefunc.lpSwitchEnStatus(enClose); //NSS = 1;
}

void  register_rf_func(lpCtrlTypefunc_t *func) {
	if (func->lpByteWritefunc != 0) {
		lpTypefunc.lpByteWritefunc = func->lpByteWritefunc;
	}
	if (func->lpByteReadfunc != 0) {
		lpTypefunc.lpByteReadfunc = func->lpByteReadfunc;
	}
	if (func->lpSwitchEnStatus != 0) {
		lpTypefunc.lpSwitchEnStatus = func->lpSwitchEnStatus;
	}

}

void  SX1276LoRaSetRFFrequency(void) {
	SX1276WriteBuffer( REG_LR_FRFMSB, Frequency[0]);//0x04射频载波频率最高有效位
	SX1276WriteBuffer( REG_LR_FRFMID, Frequency[1]);//0x07射频载波频率中间有效位
	SX1276WriteBuffer( REG_LR_FRFLSB, Frequency[2]);//0x00射频载波频率最低有效位
}

void  SX1276LoRaSetNbTrigPeaks(unsigned char value) {
	unsigned char RECVER_DAT;
	RECVER_DAT = SX1276ReadBuffer(0x31);
	RECVER_DAT = (RECVER_DAT & 0xF8) | value;
	SX1276WriteBuffer(0x31, RECVER_DAT);
}

void  SX1276LoRaSetSpreadingFactor(unsigned char factor) {
	unsigned char RECVER_DAT;
	SX1276LoRaSetNbTrigPeaks(3);
	RECVER_DAT = SX1276ReadBuffer( REG_LR_MODEMCONFIG2);
	RECVER_DAT = (RECVER_DAT & RFLR_MODEMCONFIG2_SF_MASK) | (factor << 4);
	SX1276WriteBuffer( REG_LR_MODEMCONFIG2, RECVER_DAT);
}

void  SX1276LoRaSetErrorCoding(unsigned char value) {
	unsigned char RECVER_DAT;
	RECVER_DAT = SX1276ReadBuffer( REG_LR_MODEMCONFIG1);
	RECVER_DAT = (RECVER_DAT & RFLR_MODEMCONFIG1_CODINGRATE_MASK)
			| (value << 1);
	SX1276WriteBuffer( REG_LR_MODEMCONFIG1, RECVER_DAT);
// LoRaSettings.ErrorCoding = value;
}

void  SX1276LoRaSetSignalBandwidth(unsigned char bw) {
	unsigned char RECVER_DAT;
	RECVER_DAT = SX1276ReadBuffer( REG_LR_MODEMCONFIG1);
	RECVER_DAT = (RECVER_DAT & RFLR_MODEMCONFIG1_BW_MASK) | (bw << 4);
	SX1276WriteBuffer( REG_LR_MODEMCONFIG1, RECVER_DAT);
// LoRaSettings.SignalBw = bw;
}

void  SX1276LoRaSetImplicitHeaderOn(BOOL enable) {
	unsigned char RECVER_DAT;
	RECVER_DAT = SX1276ReadBuffer( REG_LR_MODEMCONFIG1);
	RECVER_DAT = (RECVER_DAT & RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK)
			| (enable);
	SX1276WriteBuffer( REG_LR_MODEMCONFIG1, RECVER_DAT);
}

void  SX1276LoRaSetPayloadLength(unsigned char value) {
	SX1276WriteBuffer( REG_LR_PAYLOADLENGTH, value);
}

void  SX1276LoRaSetSymbTimeout(unsigned int value) {
	unsigned char RECVER_DAT[2];
	RECVER_DAT[0] = SX1276ReadBuffer( REG_LR_MODEMCONFIG2);
	RECVER_DAT[1] = SX1276ReadBuffer( REG_LR_SYMBTIMEOUTLSB);
	RECVER_DAT[0] = (RECVER_DAT[0] & RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK)
			| ((value >> 8) & ~RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK);
	RECVER_DAT[1] = value & 0xFF;
	SX1276WriteBuffer( REG_LR_MODEMCONFIG2, RECVER_DAT[0]);
	SX1276WriteBuffer( REG_LR_SYMBTIMEOUTLSB, RECVER_DAT[1]);
}

void  SX1276LoRaSetMobileNode(BOOL enable) {
	unsigned char RECVER_DAT;
	RECVER_DAT = SX1276ReadBuffer( REG_LR_MODEMCONFIG3);
	RECVER_DAT = (RECVER_DAT & RFLR_MODEMCONFIG3_MOBILE_NODE_MASK)
			| (enable << 3);
	SX1276WriteBuffer( REG_LR_MODEMCONFIG3, RECVER_DAT);
}

void  RF_RECEIVE(void) {
	SX1276LoRaSetOpMode(Stdby_mode);
	SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_RXD_Value);  //打开发送中断
	SX1276WriteBuffer(REG_LR_HOPPERIOD, PACKET_MIAX_Value);
	SX1276WriteBuffer( REG_LR_DIOMAPPING1, 0X00);
	SX1276WriteBuffer( REG_LR_DIOMAPPING2, 0X00);
	SX1276LoRaSetOpMode(Receiver_mode);
//   lpTypefunc.paSwitchCmdfunc(rxOpen);
}

void  SX1276LoRaSetPacketCrcOn(BOOL enable) {
	unsigned char RECVER_DAT;
	RECVER_DAT = SX1276ReadBuffer( REG_LR_MODEMCONFIG2);
	RECVER_DAT = (RECVER_DAT & RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK)
			| (enable << 2);
	SX1276WriteBuffer( REG_LR_MODEMCONFIG2, RECVER_DAT);
}

void  SX1276LoRaFsk(Debugging_fsk_ook opMode) {
	unsigned char opModePrev;
	opModePrev = SX1276ReadBuffer(REG_LR_OPMODE);
	opModePrev &= 0x7F;
	opModePrev |= (unsigned char) opMode;
	SX1276WriteBuffer( REG_LR_OPMODE, opModePrev);
}


void  SX1276LoRaSetRFPower(unsigned char power) {
	SX1276WriteBuffer( REG_LR_PADAC, 0x87);
	SX1276WriteBuffer( REG_LR_PACONFIG, power_data[power]);
}


/*
function :you must call it ,the function is to init the module.
*/
void  SX1276LORA_INT(void) {
	SX1276LoRaSetOpMode(Sleep_mode);  //设置睡眠模式0x01
	SX1276LoRaFsk(LORA_mode);	      // 设置扩频模式,只能在睡眠模式下修改
	SX1276LoRaSetOpMode(Stdby_mode);   // 设置为普通模式
	SX1276WriteBuffer( REG_LR_DIOMAPPING1, GPIO_VARE_1);
	SX1276WriteBuffer( REG_LR_DIOMAPPING1, GPIO_VARE_1);
	SX1276WriteBuffer( REG_LR_DIOMAPPING2, GPIO_VARE_2);
	SX1276LoRaSetRFFrequency();
	SX1276LoRaSetRFPower(powerValue);
	SX1276LoRaSetSpreadingFactor(SpreadingFactor);	 // 扩频因子设置
	SX1276LoRaSetErrorCoding(CodingRate);		 //有效数据比
	SX1276LoRaSetPacketCrcOn(true);			  //CRC 校验打开
	SX1276LoRaSetSignalBandwidth(Bw_Frequency);	 //设置扩频带宽
	SX1276LoRaSetImplicitHeaderOn(false);		//同步头是显性模式
	SX1276LoRaSetPayloadLength(0xff);//0x22 timeout中断
	SX1276LoRaSetSymbTimeout(0x3FF);
	SX1276LoRaSetMobileNode(true); 			 // 低数据的优化
	RF_RECEIVE();
//	uartSendString("\r\ninit finish\r\n");

}

/*
function :if you want to send data,you can call it 
RF_TRAN_P:data
ASM_i:the length of the data
*/
void  FUN_RF_SENDPACKET(unsigned char *RF_TRAN_P,
		unsigned char LEN) {
	unsigned char ASM_i;
//   lpTypefunc.paSwitchCmdfunc(txOpen);
	SX1276LoRaSetOpMode(Stdby_mode);
	SX1276WriteBuffer( REG_LR_HOPPERIOD, 0);	//不做频率跳变
	SX1276WriteBuffer(REG_LR_IRQFLAGSMASK, IRQN_TXD_Value);	//打开发送中断
	SX1276WriteBuffer( REG_LR_PAYLOADLENGTH, LEN);	 //最大数据包
	SX1276WriteBuffer( REG_LR_FIFOTXBASEADDR, 0);
	SX1276WriteBuffer( REG_LR_FIFOADDRPTR, 0);
	lpTypefunc.lpSwitchEnStatus(enOpen);
	lpTypefunc.lpByteWritefunc(0x80);
	for (ASM_i = 0; ASM_i < LEN; ASM_i++) {
		lpTypefunc.lpByteWritefunc(*RF_TRAN_P);
		RF_TRAN_P++;
	}
	lpTypefunc.lpSwitchEnStatus(enClose);
	SX1276WriteBuffer(REG_LR_DIOMAPPING1, 0x40);
	SX1276WriteBuffer(REG_LR_DIOMAPPING2, 0x00);
	SX1276LoRaSetOpMode(Transmitter_mode);
}

