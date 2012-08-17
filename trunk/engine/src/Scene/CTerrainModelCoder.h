#pragma once
#include "CElementManager.h"
#include "CSceneManagerClient.h"
//#include "CModelRegionMgr.h"
#include "CTreeScene.h"

namespace sqr
{
	class CRenderModel;
	class CRenderScene;
	class CTerrainModelCoder : public CElementCoderEx
	{
	public:
		CTerrainModelCoder(CSceneManagerClient* pDataScene);
		bool	_CElementDeCode( const DeCodeParams& Params );
	private:
		CSceneManagerClient*		m_pDataScene;
		DECLARE_VERSION_MAP();
		virtual bool DeCode_Ver_0( const CDataChunk& inData ,OBJ_HANDLE handle);	
	};

	class CTerrainModelLoader :  public CElementCoder
	{
		friend class CTerrainModelCoder;
	protected:
		struct CTerrainCoderInf : public CSceneMallocObject
		{
			CRenderModel*	pTerrainObj;
			uint32					GridIndex;
			SString					FileName;
			uint32					RSIndex;
		};

		typedef SList< CTerrainCoderInf > CTerrainObjList;
		CTerrainObjList			m_ObjList;
		CSceneManagerClient*	m_pDataScene;
		//CSceneTreeNode*		m_pTerMod; 
	public:
		CTerrainModelLoader(CSceneTreeNode* pTerMod,CSceneManagerClient* pScene);
		bool AutoDeCode(void);

	};
}
