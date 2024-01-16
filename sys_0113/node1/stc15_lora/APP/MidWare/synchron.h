/**
 * @brief	中间件；专用于统筹系统同步信号
 * @detail	具体任务	1. 收集其他电台的就续信号
 * 						2. 生成进入下一周期的信号
 *						3. 生成让其他节点继续休眠的信号（当前周期数）
 *						4. 信号的发布在其他文件执行
 * @warning	该文件为独立电台专用
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
 
u8 getReadySign(void);		//获取就续信号
//void advancePeriod(void);	//根据获取到的各种信号，推动周期运行
//void signCreate(void);		//根据同步周期，创建信号生成标识
 
#endif
 