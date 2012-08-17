#pragma once 

namespace sqr
{
	class CScriptThread;

	class CScriptJob
	{
	public:
		static void* operator new(size_t stSize, CScriptThread* pThread);
		static void operator delete(void* , CScriptThread* ){}
		static void operator delete(void* ){}

		void Add(CScriptThread* pThread);
		virtual ~CScriptJob(){}

		static void* AllocMem(size_t stSize, CScriptThread *pThread);
		static void* CloneData(const void* pData, size_t stSize, CScriptThread *pThread);

	};

	class CScript;

	class CScriptPostPackJob
		: public CScriptJob
	{
	public:
		CScriptPostPackJob(const char* strType, size_t stTypeSize,
						   const char* vecBuf, size_t stBufSize, 
						   CScriptThread* pThread);
		
		~CScriptPostPackJob();

	private:
		char*					m_szType;
		size_t					m_stTypeSize;
		void*					m_pBuffer;
		size_t					m_stBufSize;
		CScript*				m_pScript;
	};

	class CFileWatcherData;
	class CChangeFileJob
		: public CScriptJob
	{
	public:
		CChangeFileJob(CFileWatcherData* pData, CScriptThread* pThread);
		~CChangeFileJob();

	private:
		CFileWatcherData*	m_pData;
		CScriptThread*		m_pThread;
	};
}

