#pragma once 
#include "ThreadTypes.h"
#include "CMallocObject.h"

namespace sqr
{
	class CMemThreadMgr;

	class CMemThread : public CMallocObject
	{
	public:
		CMemThread(CMemThreadMgr* pThreadMgr);
		~CMemThread();

		bool				m_bQuit;
	private:
		HTHREAD				m_hThread;
		CMemThreadMgr*		m_pThreadMgr;
		
		static void Execute(void* pParam);

		void DoExecute();
	};

}
