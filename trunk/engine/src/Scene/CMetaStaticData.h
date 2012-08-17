#pragma once
#include "CVector3.h"
#include "CSceneMallocObject.h"

class CMetaStaticData
	: public CSceneMallocObject
{
	enum
	{ 
		TERRAIN_NORMAL_YAW 		= 9,
		TERRAIN_NORMAL			= 256,
		TERRAIN_NORMAL_PIECE	= TERRAIN_NORMAL/4,
		TERRAIN_NORMAL_SGX		= 0x1,
		TERRAIN_NORMAL_SGZ		= 0x2
	};

public:

	struct StaticGrid : public CSceneMallocObject
	{
		//四个顶点的贴图坐标left,top,right,bottom
		float fWater[4];
		float fTerran[4];

		//得到格子贴图坐标
		float Left()	const { return fTerran[0]; }
		float Top()		const { return fTerran[1]; }
		float Right()	const { return fTerran[2]; }
		float Bottom()	const { return fTerran[3]; }
		//得到水贴图坐标
		float WLeft()	const { return fWater[0]; }
		float WTop()	const { return fWater[1]; }
		float WRight()	const { return fWater[2]; }
		float WBottom()	const { return fWater[3]; }
	};

	static const StaticGrid&	GetGrid( int32 xGrid, int32 yGrid )	{ return s_Static.m_Grids[(yGrid+1)&3][xGrid&3]; }
	static CVector3f			GetNormal( uint8 uNor )				{ return s_Static.m_vecNormal[uNor]; }
	static uint8				GetIndex( CVector3f vecNor );

private:
	CMetaStaticData();
	static CMetaStaticData	s_Static;

	CVector3f			m_vecNormal[TERRAIN_NORMAL];
	uint32				m_uNorStr[TERRAIN_NORMAL_YAW+1];
	uint32				m_uNorNum[TERRAIN_NORMAL_YAW+1];
	StaticGrid			m_Grids[4][4];
};
