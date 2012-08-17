#pragma once
#include "TMallocAllocator.h"
#include "CStackString.h"
#include "CMemCallStackInfoSet.h"
#include "CMallocObject.h"
#include <vector>

namespace sqr
{
	class CStackFrame;

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4275)	//dll linkage
#pragma warning(disable:4251)	//dll linkage
#endif

	class CMemCallStackAddrVector;

	class CMemCallStackInfo
		:public vector<CStackFrame*,TMallocAllocator<void*> >
		,public CMallocObject
	{
	public:
		CMemCallStackInfo();
		CMemCallStackInfo(size_t uBegin, size_t uEnd);
		~CMemCallStackInfo(void);

		size_t GetText(char* szBuffer,size_t uLength)const;
		void Print()const;

		CMemCallStackInfoSet::iterator	m_it;

		void AddRef(size_t uMem);
		bool DelRef(size_t uMem);
		size_t GetCount()const;
		size_t	GetMemUsage()const;

		CMemCallStackAddrVector* TakeAddrVec();

	private:
		void GetStack(size_t uBegin, size_t uEnd);

		friend class CStackComp;

		size_t	m_uMemUsage;
		int32	m_nCounter;
		CMemCallStackAddrVector*	m_pVecAddr;
	};

#ifdef _WIN32
#pragma warning(pop)
#endif
}
