#include "stdafx.h"
#include "CClientSceneQuery.h"

bool	CClientSceneQuery::SceneIntersects( const CVector3f& vec )
{
	return m_pTMM->Intersects(vec) || (LogicalHeight(vec.x,vec.z) + 32.0f) > vec.y;
}

float	CClientSceneQuery::TerrainHeight(float x, float y)
{
	return 0.0f;
}

float	CClientSceneQuery::LogicalHeight(float x, float y)
{	
	return m_pSecene->GetOffsetHeightInPixel( limit2(x,0.0f,float(m_pSecene->GetWidthInPixel())-64.0f), limit2(y,0.0f,float(m_pSecene->GetHeightInPixel())-64.0f) );
}