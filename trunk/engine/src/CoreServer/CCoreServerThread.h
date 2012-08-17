#pragma once
#include "ThreadTypes.h"

namespace sqr
{

	class CCoreServerThreadMgr;

	class CCoreServerThread
	{
	public:
		CCoreServerThread(CCoreServerThreadMgr* pMgr);
		~CCoreServerThread(void);

	private:

		HTHREAD					m_hThread;
		CCoreServerThreadMgr*	m_pThreadMgr;

		static void Execute(void* pParam);
		void DoExecute();
	};

}
