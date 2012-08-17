#pragma once
#include "CFindPathMgrTemplate.h"
#include "TMetaScene.h"


namespace sqr
{
	template<typename CTraits>
	class TBaseMetaSceneServer
		:public TMetaScene<CTraits>
	{
		typedef typename CTraits::MetaSceneMgr_t	ImpMetaSceneMgr_t;
	public:
		virtual uint32	GetRegionVersionID() const;
	protected:
		TBaseMetaSceneServer( const char* szSceneName, const uint32 sceneId, ImpMetaSceneMgr_t* pMgr );
		~TBaseMetaSceneServer();

		bool HasObject()const;

		typedef TMetaScene<CTraits>		ParentMetaScene_t;
		friend class CTestSuiteMetaSceneServer;
		//friend class TMetaSceneMgr<CTraits>;
		//friend class TPart<ImpMetaScene_t,ImpMetaSceneMgr_t>;
		void LoadRegionTrap( CPkgFile& File, uint32 uRegionId );
		void LoadRegionObject( CPkgFile& File, uint32 uRegionId );
		void LoadRegionAbsoluteHeight( CPkgFile& File, uint32 uRegionId );


		virtual void LoadOneChunk( CPkgFile& File, uint32 uRegionId, uint32 uMagicID );
		virtual const char* GetRegionFileName( string& str );
		virtual void PreLoadRegionInfo();
	};
}
