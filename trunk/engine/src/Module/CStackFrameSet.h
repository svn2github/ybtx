#pragma once
#include "TMallocAllocator.h"

namespace sqr
{
	class CStackFrame;

	class CFrameComp
	{
	public:
		bool operator()(const CStackFrame* pLeft, const CStackFrame* pRight) const;
	};


	class CStackFrameInfoSet
		:public set<CStackFrame*,CFrameComp,TMallocAllocator<CStackFrame*> >
	{
	};


}

