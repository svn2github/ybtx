#pragma once 
#include "CVector3.h"
#include "CSceneMallocObject.h"
namespace sqr
{	
	class CSceneQuery : public CSceneMallocObject
	{
	public:
		virtual bool	SceneIntersects( const CVector3f& vec ) = 0;
		virtual float	TerrainHeight(float x, float y)			= 0;
		virtual float	LogicalHeight(float x, float y)			= 0;
	};
}