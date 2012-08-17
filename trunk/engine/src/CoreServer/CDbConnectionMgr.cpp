#include "stdafx.h"
#include "CDbConnectionMgr.h"
#include "CDbThread.h"
#include "ThreadHelper.h"
#include "CQueryJob.h"
#include "IDatabase.h"
#include "ExpHelper.h"
#include "CAppServer.h"
#include "TSqrAllocator.inl"
#include "TraceHelper.h"


namespace sqr
{
	IDbConnectionMgr* GetDbConnectionMgr(IDatabase* pDatabase, uint8 uNumConn, const char* szPipe, const char* szUser, 
		const char* szPassword, const char* szDatabase)
	{
		return new CDbConnectionMgr(pDatabase, uNumConn, szPipe, szUser, szPassword, szDatabase);
	}

	IDbConnectionMgr* GetDbConnectionMgr(IDatabase* pDatabase, uint8 uNumConn, const char* szHost,unsigned short uPort, 
		const char* szUser,const char* szPassword, const char* szDatabase)
	{
		return new CDbConnectionMgr(pDatabase, uNumConn, szHost, uPort, szUser, szPassword, szDatabase);
	}
}



CDbConnectionMgr::CDbConnectionMgr(IDatabase* pDatabase, uint8 uNumConn, const char* szPipe, const char* szUser, const char* szPassword, const char* szDatabase)
{
	SQR_TRY
	{
		CAppServer::Inst()->m_pDatabase->BeginDbThread();	
		CreateDbConnection(pDatabase,uNumConn, szPipe, szUser, szPassword, szDatabase);
		CreateConnectionMgrThread();		
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;
		strm << exp.ErrorTitle() << endl << exp.ErrorMsg() << endl;
		ShowErrorMsgAndExit(strm.str().c_str());
	}
	SQR_TRY_END;
}

CDbConnectionMgr::CDbConnectionMgr(IDatabase* pDatabase, uint8 uNumConn, const char* szHost,unsigned short uPort, const char* szUser,const char* szPassword, const char* szDatabase)
{
	SQR_TRY
	{
		CAppServer::Inst()->m_pDatabase->BeginDbThread();	
		CreateDbConnection(pDatabase,uNumConn, szHost, uPort, szUser, szPassword, szDatabase);
		CreateConnectionMgrThread();
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;
		strm << exp.ErrorTitle() << endl << exp.ErrorMsg() << endl;
		ShowErrorMsgAndExit(strm.str().c_str());
	}
	SQR_TRY_END;
}

CDbConnectionMgr::~CDbConnectionMgr(void)
{
	PutSemaphore(&m_smPing);

	ReleaseConnections();

	JoinThread(&m_hPingThread);

	DestroySemaphore(&m_smPing);

	CAppServer::Inst()->m_pDatabase->EndDbThread();
}

IDbConnection* CDbConnectionMgr::GetConnection(uint8 uIndex) const
{
	Ast(uIndex < m_vecConnection.size());
	return m_vecConnection[uIndex];
}

uint8 CDbConnectionMgr::GetConnectionNum()const
{
	return (uint8)m_vecConnection.size();
}

void CDbConnectionMgr::CreateDbConnection(IDatabase* pDatabase,uint8 uNumConn, const char* szPipe,
										  const char* szUser, const char* szPassword, const char* szDatabase)
{
	m_vecConnection.resize(uNumConn);

	SQR_TRY
	{
		for(size_t i=0; i < m_vecConnection.size(); ++i)
		{
			IDbConnection* pConn = pDatabase->CreateConnection(szPipe,szUser,szPassword,szDatabase);

			m_vecConnection[i] = pConn;
		}
	}
	SQR_CATCH(exp)
	{
		ostringstream strm;
		strm << exp.ErrorTitle() << endl << exp.ErrorMsg() << endl;
		ShowErrorMsgAndExit(strm.str().c_str());
	}
	SQR_TRY_END;

}

void CDbConnectionMgr::CreateDbConnection(IDatabase* pDatabase,uint8 uNumConn, const char* szHost,unsigned short uPort,
										  const char* szUser,const char* szPassword, const char* szDatabase)
{
	m_vecConnection.resize(uNumConn);

	SQR_TRY
	{
		for(size_t i=0; i< m_vecConnection.size(); ++i)
		{
			IDbConnection* pConn = pDatabase->CreateConnection(szHost,uPort,szUser,szPassword,szDatabase);
			if(!pConn)
				GenErr("IDatabase::CreateConnection failed.");

			m_vecConnection[i] = pConn;
		}

	}
	SQR_CATCH(exp)
	{
		ostringstream strm;
		strm << exp.ErrorTitle() << endl << exp.ErrorMsg() << endl;
		ShowErrorMsgAndExit(strm.str().c_str());
	}
	SQR_TRY_END;

}

void CDbConnectionMgr::CreateConnectionMgrThread()
{
	CreateSemaphore(&m_smPing,0,INT_MAX);
	CreateThread(&m_hPingThread,1024,&CDbConnectionMgr::ConnectionPing,this);
}

void CDbConnectionMgr::ConnectionPing(void* pParam)
{
	CDbConnectionMgr* pMgr = static_cast<CDbConnectionMgr*>(pParam);
	AddWatchCpuThreadID("DbConnection",GetCurTID());
	return pMgr->PingMysql();
}

void CDbConnectionMgr::PingMysql()
{
	const unsigned uOneHour=3600000;		// 3600000为1小时的毫秒数

	bool bQuit = false;

	CAppServer::Inst()->m_pDatabase->BeginDbThread();	

	for(;;)
	{
		switch(GetSemaphore(&m_smPing, uOneHour))
		{
		case 0:		//quit
			bQuit = true;
			break;
		case 1:		//timeout
			break;
		default:
			{
				ostringstream strm;
				strm<<"GetSemaphore failed with error:"<<
#ifndef _WIN32
					strerror(errno);
#else
					GetLastError();
#endif
				GenErr("CDbConnectionMgr::PingMysql GetSemaphore Error", strm.str());
			}
		}

		if(bQuit)
			break;

		size_t MaxSize = m_vecConnection.size();

		for( size_t n=0; n < MaxSize; n++ )
		{
			IDbConnection* pConn = m_vecConnection[n];
			if(!pConn->Ping())
			{
				ostringstream strm;
				strm<<"Connection to mysql lost."<<ends;
				GenErr(strm.str());	//直接crash，不crash也没用
			}
		}
	}

	CAppServer::Inst()->m_pDatabase->EndDbThread();
}

void CDbConnectionMgr::Release()
{
	delete this;
}

void CDbConnectionMgr::ReleaseConnections()
{
	for( size_t n=0; n < m_vecConnection.size(); n++ )
	{
		m_vecConnection[n]->Release();
	}
}
