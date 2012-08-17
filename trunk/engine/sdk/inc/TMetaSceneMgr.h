#pragma once
#include "TWhole.h"
#include "CDynamicObject.h"
#include "CMetaSceneMallocObject.h"


namespace sqr
{
	class CTestSuiteMetaSceneClient;
	class CTestSuiteMetaSceneServer;
	class CTestSuiteCoreObjectClient;
	class CTestSuiteCoreObjectServer;
	class CTestSuiteAoi;
	class CMetaSceneMgrServer;
	class CAppServer;

	template<typename Traits>
	class TMetaScene;

	template <typename Traits>
	class TCoreApp;

	template<typename Traits>
	class TMetaSceneMgr
		:public virtual CDynamicObject
		,public TWhole<typename Traits::MetaSceneMgr_t,typename Traits::MetaScene_t>
		,public CMetaSceneMallocObject
	{
		typedef typename Traits::MetaSceneMgr_t				ImpMetaSceneMgr_t;
		typedef typename Traits::MetaScene_t				ImpMetaScene_t;
		typedef TMetaScene<Traits>							MetaScene_t;
	public:
		static ImpMetaSceneMgr_t* Inst();

	protected:
		ImpMetaScene_t* CreateMetaScene(const char* szSceneName, uint32 sceneId);
		void DestroyMetaScene(ImpMetaScene_t* pScene);

		TMetaSceneMgr(void);
		~TMetaSceneMgr(void);
	private:
		friend class TCoreApp<Traits>;
#ifndef _WIN32
		friend class Traits::CoreSceneMgr_t;
#else
		friend Traits::CoreSceneMgr_t;
#endif
		friend class CTestSuiteMetaSceneClient;
		friend class CTestSuiteMetaSceneServer;
		friend class CTestSuiteCoreObjectServer;
		friend class CTestSuiteCoreObjectClient;
		friend class CTestSuiteAoi;
		friend class CMetaSceneMgrServer;
		friend class CAppServer;

		static ImpMetaSceneMgr_t*& IntInst();
	};

}

