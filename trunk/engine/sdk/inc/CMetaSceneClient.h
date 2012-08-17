#pragma once

#include "TMetaScene.h"
#include "CFindPathMgrTemplate.h"
#include "CTraitsClient.h"
#include "CColor.h"
#include "CSceneManagerClient.h"

namespace sqr
{
	class CTestSuiteMetaSceneClient;
	class CTestSuiteCoreObjectClient;
	class IMetaSceneClientHandler;

	class CoreSceneManagerClient
		: public CSceneManagerClient
	{
	public:
		CoreSceneManagerClient(const SString& SceneName, CTreeScene* pTreeScene , uint32 Wdith , uint32 Height, uint32 RecycleSize );;
	protected:
		IMap*		CreateIMap(uint32 uWidthInRegion, uint32 uHeightInRegion);
	};

	class CMetaSceneClient
		:public TMetaScene<CTraitsClient>
	{
	private:
		typedef TMetaScene<CTraitsClient>		ParentMetaScene_t;
		friend class CRenderScene;
		friend class TMetaSceneMgr<CTraitsClient>;
		friend class TPart<ImpMetaScene_t,ImpMetaSceneMgr_t>;
		friend class CTestSuiteCoreObjectClient;

		CMetaSceneClient( const char* szSceneName, const uint32 sceneId, CMetaSceneMgrClient* pMgr );
		~CMetaSceneClient();

	public:
		bool						m_bUsedByCore;			//引擎正在使用的标记，如果为true，即使shell要求释放该metascene，实际上也不会释放。
		uint32						m_uUsedCountByShell;	//逻辑的使用计数
	
		CTreeScene*					m_pTreeScene;
		CoreSceneManagerClient		m_SceneManager;
	};
}