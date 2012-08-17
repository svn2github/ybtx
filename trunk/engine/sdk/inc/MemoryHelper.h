#pragma once


#include "ModuleDefs.h"

namespace sqr
{
	MODULE_API size_t GetAllMemUsage();  //获取当前使用的内存总量
	MODULE_API size_t GetAllCapacity(); //获取当前占用的内存总量

	
	//nCheckLevel:1 报告泄漏数量 2打印详细堆栈
	MODULE_API void WatchLeakage_Begin(int nCheckLevel);
	MODULE_API void WatchLeakage_End();
#ifndef _WIN32
	MODULE_API void WatchLeakage_FetchAllSymbol();
#endif
	//在打开内存泄露查找的情况下，打印当前所有内存分配堆栈情况
	MODULE_API void WatchLeakage_PrintMemInfo();

	//打开内存管理开关的时候写log，不打开什么都不干
	MODULE_API void WatchLeakage_MemLog(const char* szName);

	struct MODULE_API ProcessMemInfo
	{
	public:
		size_t stAvailPhysMem;  // 系统当前剩余物理内存
		size_t stAvailPageMem;  // 系统当前剩余页交换文件
		size_t stProcessPhys;   // 进程占用的物理内存
		size_t stProcessPage;   // 进程占用的页交换文件
	};

	//获取当前进程的内存信息
	MODULE_API void GetProcessMemInfo(ProcessMemInfo* pMemInfo);

}
