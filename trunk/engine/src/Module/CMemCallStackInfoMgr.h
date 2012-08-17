#pragma once

#include "ThreadTypes.h"
#include "CMemCallStackInfoSet.h"
#include "CMallocObject.h"

namespace sqr
{
	class CMemCallStackInfoMgr
		:public CMallocObject
	{
	public:
		void AddStack(CMemCallStackInfo*& pStackInfo, uint32 uMemSize);
		void DelStack(CMemCallStackInfo*& pStackInfo,uint32 uMemSize);

		static CMemCallStackInfoMgr& Inst();

		void Print();

		int32 nLastElem; // 记录的最小一个Top
		int32 nHadElem; // Top中已经存在的元素个数
	private:
		HLOCK					m_Lock;
		CMemCallStackInfoSet    m_setStack;

		CMemCallStackInfoMgr(void);
		~CMemCallStackInfoMgr(void);
	};
}
