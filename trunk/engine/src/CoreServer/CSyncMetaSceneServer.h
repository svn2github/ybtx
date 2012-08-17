#pragma once
#include "CSyncTraitsServer.h"
#include "TBaseMetaSceneServer.h"

namespace sqr
{
	struct CSceneData
	{
		string				m_sSceneName;
		int32				m_nMagicID;
		int32				m_nVersion;
		int32				m_nVersionID;
		uint8				m_uGridWidth;
		bool				m_bCave;
		int					m_nSubRegionNum;
		uint32				m_uRegionNum;
		uint32				m_uSubWidthInRegion;
		uint32				m_uWidthInRegion;
		uint32				m_uHeightInRegion;
		uint32				m_uWidthInGrid;
		uint32				m_uHeightInGrid;
		uint32				m_uWidthInPixel;
		uint32				m_uHeightInPixel;
	};

	class CSyncMetaSceneServer
		:public TBaseMetaSceneServer<CSyncTraitsServer>
	{
		typedef TBaseMetaSceneServer<CSyncTraitsServer> Parent_t;
		friend class TMetaSceneMgr<CSyncTraitsServer>;
		friend class CSyncMetaSceneMgrServer;
		friend class TPart<CSyncMetaSceneServer,CSyncMetaSceneMgrServer>;

	private:
		CSyncMetaSceneServer(const char* szSceneName, const uint32 sceneId, CSyncMetaSceneMgrServer* pMgr);
		~CSyncMetaSceneServer();
	};
}
