#pragma once
#include "CStackString.h"
#include "CMallocObject.h"


namespace sqr
{
	class CFrameComp;
	class CStackFrameMgr;

	class CStackFrame
		:public CMallocObject
	{
	public:
		void* GetAddr()const;
		const char* GetText();
#ifndef _WIN32
		void FetchSymbol();
#endif
	private:
		friend class CFrameComp;
		friend class CStackFrameMgr;
		friend class CCallStackInfo;

		CStackFrame(void* pAddr);
		~CStackFrame(void);

		void AddRef();
		void DelRef();
		uint32 GetRef()const;

		void SetName(const char* szName);
		const char* GetName()const;

	private:
		uint32			m_uRef;
		CStackString	m_sName;
		void*			m_pAddr;
	};

}

