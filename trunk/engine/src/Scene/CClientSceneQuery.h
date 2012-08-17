#pragma once
#include "CSceneQuery.h"
#include "CTreeScene.h"
#include "CSceneManagerClient.h"
namespace sqr
{ 
	class CClientSceneQuery : public CSceneQuery
	{
	public:
		CClientSceneQuery(CTreeScene* pTMM,CDataScene* pScene)
			: m_pTMM(pTMM),m_pSecene(pScene){}
		bool	SceneIntersects( const CVector3f& vec );
		float	TerrainHeight(float x, float y);
		float	LogicalHeight(float x, float y);

	protected:
		CTreeScene*		m_pTMM;
		CDataScene*		m_pSecene;
	};
}