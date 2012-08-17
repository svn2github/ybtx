#pragma once 

#include "ThreadTypes.h"

namespace sqr
{

	class CScriptThread;
	class CScript;
	class CScriptJob;
	class CScriptResult;
	class CFileWatcherData;

	class CScriptThreadMgr
	{
		friend class CScriptThread;
		
	public:
		
		static void Init();
		static void Unit();
		static CScriptThreadMgr* Inst();

		static void PostPackFunJob(uint32 uThreadIndex, const char* strType, size_t stTypeSize,
			const char* vecBuf, size_t stBufSize);
		static void PostPackFunRes(const char* strType, size_t stTypeSize,
			const char* vecBuf, size_t stBufSize);

		void AddChangeFileJob(CFileWatcherData* pData);

		void Start(const char* szFileName);
		void Stop();

		CScript* CreateScript();
		void ReleaseAllScript();
		CScript* GetScript(uint32 uNum);

		CScriptThreadMgr();
		~CScriptThreadMgr();

	private:
		void MgrInit();
		void MgrUnit();
		void ReleaseThreads();
		
	private:
		static CScriptThreadMgr*				ms_pInst;

		bool									m_bQuitSignal;

		std::vector<CScriptThread*>				m_vecThread;	

		HTLSKEY									m_tlsThreadKey;
	};

}
