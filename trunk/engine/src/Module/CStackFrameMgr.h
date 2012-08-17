#pragma once
#include "CMallocObject.h"
#include "CStackFrameSet.h"

namespace sqr
{
	class CStackFrame;

	class CStackFrameMgr
		: public CMallocObject
	{
	public:
		CStackFrame* AddFrame(void*);
		void DelFrame(CStackFrame*);

#ifndef _WIN32
		//将当前所有存在的Frame的名字取出来放在Frame上
		void FetchAllSymbol();
#endif

		static CStackFrameMgr& Inst();
	private:
		CStackFrameInfoSet	m_setStack;

		CStackFrameMgr(void);
		~CStackFrameMgr(void);
	};
}
