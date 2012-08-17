#pragma once
#include "ThreadTypes.h"

namespace sqr
{
	class CAoiThreadMgr;

	class CAoiThread
	{
	public:
		CAoiThread(CAoiThreadMgr* pMgr);
		~CAoiThread(void);

	private:

		HTHREAD		m_hThread;
		CAoiThreadMgr*	m_pThreadMgr;

		static void Execute(void* pParam);
		void DoExecute();

	};
}
