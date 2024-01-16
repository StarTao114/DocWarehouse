#include "synchron.h"
#include "gpio.h"

//下面的变量会被反复擦写，尤其是中断，我希望它每次擦写都能立刻在全局生效
volatile word2byte synPeriod; 
u16 lastPeriod = 0; 
u8 currentNode = 0;//当前通信的目标节点编号
u8 synReady = 0;


#ifdef specialStation
//暂时用按键代替就绪信号
u8 getReadySign(void)
{
	if(KEY0 == 0)
	{
		synReady = 1;
	}
	return 0;
}

void advancePeriod(void)
{
	if(synReady == 1)
	{
		synPeriod+=10;//方便测试
		synReady = 0;
	}
}

void signCreate(void)
{
	
}

#endif
