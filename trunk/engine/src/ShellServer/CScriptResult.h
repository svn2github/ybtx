#pragma once
#include "CError.h"

namespace sqr
{
	class CScriptThread;

	class CScriptResult
	{
	public:
		static void* operator new(size_t stSize, CScriptThread* pThread);
		static void operator delete(void* , CScriptThread*){}
		static void operator delete(void* ){}

		void Add(CScriptThread* pThread);
		virtual ~CScriptResult(){}

		static void* AllocMem(size_t stSize, CScriptThread *pThread);
		static void* CloneData(const void* pData, size_t stSize, CScriptThread *pThread);
		static char* CloneString(const char* pData, CScriptThread *pThread);

	};

	class CScriptPostPackRes : public CScriptResult
	{
	public:
		CScriptPostPackRes(const char* strType, size_t stTypeSize,
			const char* vecBuf, size_t stBufSize, CScriptThread* pThread);
		~CScriptPostPackRes();

	private:
		char*					m_szType;
		size_t					m_stTypeSize;
		void*					m_pBuffer;
		size_t					m_stBufSize;
	};

	class CScriptTellErrMsgResult : public CScriptResult
	{
	public:
		CScriptTellErrMsgResult(const char* szMsg, CScriptThread* pThread);
		~CScriptTellErrMsgResult();

	private:
		char*	m_szMsg;
	};
}

