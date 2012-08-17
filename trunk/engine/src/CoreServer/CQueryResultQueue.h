#pragma once

namespace sqr
{
	class CQueryJob;
	class CDbContext;
	class CDbThreadMgr;

	//this is a "Multi In Single Out" lock free queue
	class CQueryResultQueue
	{
	public:
		CQueryResultQueue(void);
		~CQueryResultQueue(void);

		bool Empty()const;
		void Push( CQueryJob* pJob );

		CDbContext*	Get(CDbThreadMgr* pMgr);
		
		bool Pop(CDbThreadMgr* pMgr);
	private:

		CQueryJob*volatile		m_pIn;
		CQueryJob*				m_pOut;
	};


}
