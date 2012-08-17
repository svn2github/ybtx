#ifdef _WIN32

#pragma once
#include "ThreadTypes.h"

namespace sqr
{
	struct CReadRequest
	{
		HANDLE			m_hFile;
		void*			m_pBuffer;
		uint32			m_uBufferSize;
		LPOVERLAPPED	m_pOverlapped;

		void (*m_pCallBack)(uint32 dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
	};

	class CReadThread
	{
	public:
		static void Init();
		static void Unit();
		static CReadThread*& Inst();

		void Read(HANDLE hFile,void* pBuffer,uint32 uBufferSize,LPOVERLAPPED pOverlapped,
			void (*pCallBack)(uint32 dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped));

		CReadThread();

		~CReadThread();

		void ReadThread();

		static void ThreadProc(void* pArg);
	private:
		HTHREAD		m_hThread;
		HLOCK	m_lkCallBack;
		HSEMAPHORE	m_semMsg;

		list<CReadRequest*>	m_lstRequest;
	};

}

#endif

