#pragma once

#include "TMallocAllocator.h"
#include "CMallocObject.h"

namespace sqr
{
	class CMemCallStackInfo;

	class CStackComp
	{
	public:
		bool operator()(const CMemCallStackInfo* pLeft, const CMemCallStackInfo* pRight) const;
	};

	class CMemCallStackInfoSet
		:public set<CMemCallStackInfo*,CStackComp,
		TMallocAllocator<CMemCallStackInfo*> >
	{
	};

	class CMemCallStackAddrVector
		:public vector<void*, TMallocAllocator<void*> >
		,public CMallocObject
	{
	};
}
