#pragma once

#include "CDynamicObject.h"

namespace sqr
{
	class IPipeModule;
	class IPipeReactor;
	class IPipe;
	class IPipeHandler;
	class IMulticast;
	
	class CError;
	class CTickMgr;
	class CTick;
	class CDistortedTick;

	class CCoreMsgMgr;


	template <typename Traits>
	class TApp
		: public virtual CDynamicObject
	{
		typedef typename Traits::App_t					ImpApp_t;
		typedef typename Traits::AppHandler_t			ImpAppHandler_t;
		typedef typename Traits::TimeSystem_t			ImpTimeSystem_t;
		typedef typename Traits::AppConfig_t			ImpAppConfig_t;
		typedef typename Traits::Conn_t					ImpConnType_t;
		typedef typename Traits::ConnMgr_t				ImpConnMgr_t;

#ifndef _WIN32
		friend class Traits::ConnMgr_t;
#else
		friend Traits::ConnMgr_t;
#endif
	public:
		static ImpApp_t* Inst();
		
		int Run();

		void RegisterTick(CTick*,uint32 uCyc);
		void UnRegisterTick(CTick*);

		void RegisterDistortedTick(CDistortedTick* pTick, uint32 uCyc);
		void UnRegisterDistortedTick(CDistortedTick* pTick);

		uint64 GetFrameTime()const;

		ImpConnMgr_t* GetConnMgr()const;

		void SetHandler(ImpAppHandler_t * pHandler);
		ImpAppHandler_t* GetHandler()const;

		IMulticast* GetIMulticast();
		IPipe* GetIPipe(IPipeHandler* pHandler);
		IPipe* GetIPipe(IPipeHandler* pHandler,size_t stSendBufferSize,size_t stRecvBufferSize,bool bNeedSomeDataSentCallback);
	
#ifdef _WIN32
		void OnSysMsg();
		void OnIocp();
#endif
		uint32 OnIdle( uint32 uTimeLeft );

		void OnCanceled();
	protected:
		static ImpApp_t*& IntInst();

		void IntQuit();

		TApp();
		~TApp();

#ifndef _WIN32
		void FlushSigMsgDerived();
#endif

		bool MainFrame();

		void ReactorCancelBlock();

		void	OnBeginMainCycleDerived();
		void	OnMainCycleEndedDerived();
		
		void	OnMainFrameEndedDerived();
	protected:
		bool			m_bQuit;
		ImpAppHandler_t*	m_pHandler;

		IPipeModule*	m_pPipeModule;
		IPipeReactor*	m_pPipeReactor;

		void CreatePipeReactor();

		static const char* GetThreadName();
	};
}
