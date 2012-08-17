#pragma once

#include "CDynamicObject.h"
#include "ThreadTypes.h"
#include "TTrThreadMsgBuffer.h"
#include "IDbThreadMgr.h"

namespace sqr
{

	class CScriptThreadMgr;
	class CScriptJob;
	class CScriptResult;
	class CScript;
	class IDbConnectionMgr;
	class IDbThreadScriptHandler;
	class IScriptThreadHandler;
	class CError;
	class CScriptThread;
	class CLogOwner;

	class CScriptThreadTraits
	{
	public:
		typedef CScriptJob		LeftMsgType;
		typedef CScriptResult	RightMsgType;
	};
	

	class CScriptThread
		: public virtual CDynamicObject
		, public TTrThreadMsgBuffer<CScriptThreadTraits>
		, public IDbThreadMgrHandler
	{
		typedef TTrThreadMsgBuffer<CScriptThreadTraits> Parent_t;
		friend class TTrThreadMsgBuffer<CScriptThreadTraits>;
	
		friend class CScriptThreadMgr ;
		friend class CScriptJob;
		friend class CScriptResult;
		friend class CScriptHandleResultsEvent;

	public:
		static CScriptThread* Inst();

		void Spawn(const char* szFileName);
		void Join();

		void Run();
		void Quit();

		void SetHandler(IScriptThreadHandler* pHandler);
		IScriptThreadHandler* GetHandler() const;

		CScript* GetScript() const;
		
		IDbThreadMgr* GetDbThreadMgr(uint8 uNumThread, IDbThreadScriptHandler* pHandler);
		void ReleaseDbThreadsMgr();

		IDbConnectionMgr* GetDbConnectionMgr( uint8 uNumConn, const char* szHost,unsigned short uPort,
											  const char* szUser,const char* szPassword, const char* szDatabase );

		IDbConnectionMgr* GetDbConnectionMgr( uint8 uNumConn, const char* szPipe,
											  const char* szUser, const char* szPassword, const char* szDatabase );

		void ReleaseDbConnectionMgr(IDbConnectionMgr* pMgr);		
	private:
		CScriptThread(CScriptThreadMgr* pThreadMgr);
		~CScriptThread();

		static void Execute(void* pParam);
		void DoExecute();

		void HandleErrMsg(const char* szMsg);
		static void OnErrLogCallBack(const char* szError, CLogOwner* pOwner);

	private:
		void OnWakeRight();
		void OnLeftException(CError& error){}
		void OnRightException(CError& error){}

		void RegistThreadCoreServer(CScript& Script);
		
		void OnQueryJobDone();
		
	private:
		CScriptThreadMgr*		m_pThreadMgr;
	
		uint32					m_uIndex;
		
		HTHREAD					m_hThread;
		CScript*				m_pScript;
	
		HSEMAPHORE				m_smThread;
		volatile int32			m_nHasLeftMsg;
		volatile int32			m_nHasDbResult;
		
		std::string				m_strRunFile;

		IScriptThreadHandler*	m_pHandler;

		bool					m_bQuit;

		IDbThreadMgr*				m_pDbThreadMgr;
		IDbThreadScriptHandler*		m_pScriptDbHandler;
	};

}
