#pragma once

/*
内存：
提供这套内存管理机制的目的：
提供查找内存泄露和进行内存用量统计的功能


按照effective stl里说明：
定制allocator并不能根本解决问题，因为某些stl container并不总是用allocator来分配内存(list,map,set)。
然而按照实际测试：
vc.net2003的stl以及stlport4.3.3的list container的内存分配是完全使用定制的allocator来分配内存的.
我推测他们是通过rebind函数来实现这个功能。
*/

#include "ModuleDefs.h"
#include "ThreadTypes.h"
#include "CMallocObject.h"	

#ifndef _WIN32
#define MAX_PATH PATH_MAX
#endif

namespace sqr
{
	class CMemThreadMgr;
	/*

	内存泄露查找的技术要点：

	1.所有相关类型都派生CMallocObject或者使用TMallocAllocator，使得这些对象的内存消耗不计入统计
	2.除了CMemoryLeakageMgr之外，所有的Mgr或者貌似单件的物体，全部都只构造不释放，主要为了回避单件构造时序问题
	3.由于管理所有内存的功能是在独立线程CMemThread中运行，而所有线程在单件对象析构的时候已经被系统强行关闭，因此最后时刻
	  打印内存泄漏的功能，不能指望在CMemoryLeakageMgr单件析构的时候执行，必须明确在Main函数里调用。
    4.由于无法利用单件析构来作为最后时机打印泄漏的手段，整个RunModule以及Module库里面应该完全没有持续生存的全局变量或
	  静态变量。
    5.不要使用cin,cout,cerr,使用fprintf,fputs,puts,这些c++对象也是会分配内存的长期存在的全局对象。
	*/

	class CMemoryLeakageMgr
		:public CMallocObject
	{
	public:		
		static void Init(int nCheckLevel);
		static void Unit();
		static CMemoryLeakageMgr& Inst();
		//find leakage function		
		void WriteMemLog(const char* szLog);

#ifndef _WIN32
		void FetchAllSymbol();
#endif
		void DumpStack();		
	private:
		void DumpLeakageNum();

		CMemThreadMgr*	m_pThreadMgr;
		
		FILE*			m_fdLeakage;

		CMemoryLeakageMgr(int nCheckLevel);
		~CMemoryLeakageMgr();

		static CMemoryLeakageMgr	ms_Inst;


		wchar_t	 m_szMemFileName[MAX_PATH];
	};

}


