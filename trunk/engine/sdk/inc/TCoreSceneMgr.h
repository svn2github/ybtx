#pragma once
#include "TWhole.h"
#include "CDynamicObject.h"
#include "CCoreSceneMallocObject.h"

namespace sqr
{
	class CTestSuiteCoreObjectClient;
	class CTestSuiteCoreObjectServer;

	template<typename Traits>
	class TCoreScene;

	template<typename Traits>
	class TMetaScene;

	template<typename Traits>
	class TCoreApp;

	class CCoreSceneMgrServer;

	template<typename Traits>
	class TCoreSceneMgr
		:public virtual CDynamicObject
		,public TWhole<typename Traits::CoreSceneMgr_t,typename Traits::CoreScene_t>
		,public CCoreSceneMallocObject
	{
		typedef typename Traits::CoreScene_t		ImpCoreScene_t;
		typedef typename Traits::MetaScene_t		ImpMetaScene_t;
		typedef typename Traits::CoreSceneMgr_t		ImpCoreSceneMgr_t;
	public:
		static ImpCoreSceneMgr_t* Inst();		
	protected:
		TCoreSceneMgr(void);
		~TCoreSceneMgr(void);
	private:
		static ImpCoreSceneMgr_t*& IntInst();

		friend class CRenderScene;
		friend class TCoreApp<Traits>;
#ifndef _WIN32
		friend class Traits::CoreObject_t;
		friend class Traits::CoreScene_t;
		friend class Traits::ConnMgr_t;
		friend class Traits::Conn_t;
#else
		friend Traits::CoreObject_t;
		friend Traits::CoreScene_t;
		friend Traits::ConnMgr_t;
		friend Traits::Conn_t;
#endif
		friend class CCoreSceneMgrServer;
		friend class CTestSuiteCoreObjectServer;
		friend class CTestSuiteCoreObjectClient;
	};

}

