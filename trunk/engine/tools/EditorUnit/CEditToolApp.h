#pragma once 
#include "IApplication.h"
#include "ICtrlImpFactory.h"
#include "winsock2.h"
#include "../../src/CoreCommon/TTimeSystem.h"

namespace sqr
{
	class CTickMgr;
	class CTick;
}

namespace sqr_tools
{
	class CEditContext;
	typedef void (*SysSigCallBack)(void);

	class CEditTimeSystem : public TTimeSystem<CEditTimeSystem>
	{
	public:
		typedef TTimeSystem<CEditTimeSystem>	Parent_t;
		friend class TTimeSystem<CEditTimeSystem>;

		CEditTimeSystem(uint32 uBaseCyc);
		~CEditTimeSystem()
		{

		}
	};

	class CEditToolApp : public IApplication
	{
		friend class IApplication;
	public:
		typedef CEditTimeSystem ImpTimeSystem_t; //主要为了以后有可能整合的话 方便
		ICtrlImpFactory* GetCtrlImpFactory(void);
	public:
		~CEditToolApp(void);
		void	Run(void);
		void	SetOnCanceled( SysSigCallBack pBack );
		void	SetOnIdle( SysSigCallBack pBack );
		uint32	GetCurTime(void);


		void	RegisterTick(CTick* pTick,uint32 uCyc);
		void	UnRegisterTick(CTick* pTick);
	protected:
		CEditToolApp(void);
		
		bool	MainFrame(void);
		void	OnCanceled(void);
		void	OnSysMsg(void);
		uint32	OnIdle(uint32 uErrTime);
		uint32	GetEvent(unsigned uWaitTime=0);
		
		void	Release(void);
		void	Close(void);
	protected:
		typedef vector<WSAEVENT>		VecEvent_t;
		VecEvent_t		m_vecEvent;
		HANDLE			m_evCancelBlock;
		bool			m_bQuit;

		SysSigCallBack	m_pfnCanceled;
		SysSigCallBack	m_pfnIdle;

		CEditTimeSystem	m_TimeSystem;
	protected:		
		friend class CEditContext;
		void	_AddContext( CEditContext* pContext );
		void	_DelContext( CEditContext* pContext );
		typedef vector<CEditContext*>	VecContext;
		VecContext		m_vecContext;
		uint32			m_uCurTime;
	};
}