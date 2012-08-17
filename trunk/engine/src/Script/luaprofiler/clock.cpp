#include "clock.h"
using namespace sqr;
// 获取开始时间
void StartTimer(uint64 *TimeMarker)
{
	*TimeMarker = GetHDProcessTime();
}

// 返回开始到结束的时间差
static uint64 GetClocks(uint64 TimeMarker)
{
	return GetHDProcessTime() - TimeMarker;
}

// clock_t是一个长整形数。在time.h文件中，还定义了一个常量CLOCKS_PER_SEC，它用来表示一秒钟会有多少个时钟计时单元
double GetSeconds(uint64 TimeMarker)
{
	uint64 clocks = GetClocks(TimeMarker);
	return (double)(double(clocks) / 1000000.0);
}


