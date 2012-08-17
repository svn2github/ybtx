#pragma once 
#include "ThreadTypes.h"
#include "CMallocObject.h"
#include "TMallocAllocator.h"

namespace sqr
{

	class CError;
	class CErrLogJob;

	class CErrLogThreadMgr : public CMallocObject
	{
		friend class CErrLogJob;
		friend class CWriteErrLogJob;

	public:
		static void Init();
		static void Unit();
		static CErrLogThreadMgr* Inst(){return ms_pInst;}

		void	AddCreateErrlogJob(const wchar_t* szFileName, const char* szDeviceInfo);
		void	AddCloseErrlogJob();
		void	AddWriteErrlogJob(const CError& exp,const char* szErrlogInfo);

	private:
		CErrLogThreadMgr();
		~CErrLogThreadMgr();

		static void Execute(void* pParam);
		void DoExecute();

		void HandleAllJobs();

		void AddJob(CErrLogJob* pJob);

	private:
		static CErrLogThreadMgr*	ms_pInst;

		HTHREAD		m_hThread;

		typedef vector<CErrLogJob*, TMallocAllocator<CErrLogJob*> > VecJob_t;

		VecJob_t	m_queWaitingJob;
		VecJob_t	m_queRuningJob;

		HLOCK	m_hLock;

		HSEMAPHORE	m_hSem;

		bool		m_bEndThread;
	};
}
