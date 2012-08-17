#pragma once
#include "CDynamicObject.h"
#include "TConnAllocator.h"

namespace sqr
{
	class IPipeReactor;
	class IPipe;

	template <typename Traits>
	class TApp;

	template <typename Traits>
	class TConnection;

	template<typename Traits>
	class TConnectionMgr
		:public virtual CDynamicObject
		,public CConnMallocObject
	{
	protected:
		typedef TApp<Traits>						App_t;
		typedef typename Traits::AppConfig_t		ImpAppConfig_t;
		typedef TConnection<Traits>					Conn_t;
		typedef typename Traits::Conn_t				ImpConn_t;
		typedef typename Traits::ConnMgr_t			ImpConnMgr_t;
		typedef typename Traits::ConnMgrHandler_t	ImpConnMgrHandler_t;
		typedef typename Traits::TimeSystem_t		ImpTimeSystem_t;

		typedef	std::list<ImpConn_t*,TConnAllocator<ImpConn_t*> >		ListConn_t;

		ListConn_t				m_listAllConn;

		void DoShutDownConn(ImpConn_t* pConn);

		TConnectionMgr();
		~TConnectionMgr();
	public:
		void SetHandler(ImpConnMgrHandler_t* pHandler);
		ImpConnMgrHandler_t* GetHandler()const;
		void ShutDownAllConn();
		
	private:
		friend class TApp<Traits>;
		friend class TConnection<Traits>;
#ifdef _WIN32
		friend ImpConn_t;
#else
		friend class Traits::Conn_t;
#endif

		ImpConnMgrHandler_t*	m_pHandler;

		static ImpConnMgr_t*& IntInst();
		static ImpConnMgr_t* Inst();
	};
}
