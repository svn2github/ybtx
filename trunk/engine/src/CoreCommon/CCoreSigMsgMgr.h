#pragma once                                                                                                                 
#include "CMallocObject.h"                                                                                                   
#include "TMallocAllocator.h"                                                                                                
#include <signal.h>

namespace sqr                                                                                                                
{                                                                                                                            
#ifndef _WIN32     
	class CThreadWathcer;                                                                                                          

	class CCoreSigMsgMgr
		: public CMallocObject                                                                              
	{
		friend class CThreadWatcher;
	public:                                                                                                              
		CCoreSigMsgMgr();                                                                                                
		~CCoreSigMsgMgr();                                                                                               
		static CCoreSigMsgMgr& Inst();                                                                                   

		static void SigHandler(int signum, siginfo_t* info, void* context);                                                                            

		void SetSigHandler(void (*pFun)(int32 iSignal));                                                                 
		bool JustContinued();

		void FlushSigMsg();

	private:
		void PushSigMsg(int nSigNum);

	private:
		typedef void (*SigHandlerFun)(int32 iSignal);
		SigHandlerFun m_pHandlerFun;

		int	m_arySignalQueue[32];
		bool	m_bJustContinued;

		int32		m_nIndexOut;
		volatile int32	m_nIndexIn;
	};                                                                                                                   

#endif                                                                                  

}
