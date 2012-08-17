#pragma once
#include "ThreadTypes.h"
#include "IDbConnectionMgr.h"
#include "TMtDbAllocator.h"

namespace sqr
{
	class IDbConnection;
	class IDatabase;
	class CDbThread;

	class CDbConnectionMgr
		:public IDbConnectionMgr
		,public CMtDbMallocObject
	{
	public:

		CDbConnectionMgr(IDatabase* pDatabase, uint8 uNumConn, const char* szHost,unsigned short uPort, 
			const char* szUser,const char* szPassword, const char* szDatabase);

		CDbConnectionMgr(IDatabase* pDatabase, uint8 uNumConn, const char* szPipe, 
			const char* szUser, const char* szPassword, const char* szDatabase);

		IDbConnection* GetConnection(uint8 uIndex)const;
		uint8 GetConnectionNum()const;

		~CDbConnectionMgr(void);
	
		void	Release(void);
	private:
		void	CreateConnectionMgrThread(void);

		void	CreateDbConnection(IDatabase* pDatabase,uint8 uNumConn, const char* szHost,unsigned short uPort, 
			const char* szUser,const char* szPassword, const char* szDatabase);
		void	CreateDbConnection(IDatabase* pDatabase,uint8 uNumConn, const char* szPipe, 
			const char* szUser, const char* szPassword, const char* szDatabase);

		void	ReleaseConnections(void);


		static void ConnectionPing(void* pParam);

		void PingMysql(void);	

	private:
		bool			m_bEndThreads;

		HTHREAD		m_hPingThread;

		HSEMAPHORE	m_smPing;
		
		std::vector<IDbConnection*,TMtDbAllocator<IDbConnection*> >		m_vecConnection;

	};
}
