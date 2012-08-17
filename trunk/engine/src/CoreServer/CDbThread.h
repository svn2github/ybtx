#pragma once
#include "ThreadTypes.h"
#include "CMtDbMallocObject.h"

namespace sqr
{
	class IDbConnection;
	class CDbThreadMgr;
	
	class CDbThread
		:public CMtDbMallocObject
	{
		friend class CDbThreadMgr;

	public:
		CDbThread(CDbThreadMgr* pMgr);
		~CDbThread(void);

	private:

		HTHREAD		m_hThread;
		CDbThreadMgr*	m_pThreadMgr;
		IDbConnection*	m_pDbConn;

		static void Execute(void* pParam);
		void DoExecute();
	};
}
