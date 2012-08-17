#pragma once
#include "PatternCOR.h"

//引擎线程专用的PatternCOR,必须和逻辑线程使用的PatternCOR区分开

namespace sqr
{
	class CSyncCOREventMgr : public CCOREventMgr
	{
	public:
		static CSyncCOREventMgr* Inst()
		{
			static CSyncCOREventMgr ls_Inst;
			return &ls_Inst;
		}
	};

	class CSyncPtCORSubject : public CPtCORSubject
	{
	public:
		CSyncPtCORSubject(uint32 uHighFreqEvent);
		virtual ~CSyncPtCORSubject();

		virtual bool Notify(uint32 uEventType);
		virtual bool Notify(uint32 uEventType,void* pArg);
	};

}
