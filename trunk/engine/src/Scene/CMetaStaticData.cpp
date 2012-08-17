#include "stdafx.h"
#include "CMetaStaticData.h"
#include "MathAlgorithm.h"

CMetaStaticData CMetaStaticData::s_Static;

CMetaStaticData::CMetaStaticData(void)
{
	for( uint32 y = 0; y < 4; y++ )
	{
		for( uint32 x = 0; x < 4; x++ )
		{
			//////////////////////////////////////////////////////////////////////////
			//  0left,1top,2right,3bottom
			m_Grids[y][x].fTerran[0] = x*0.5f;
			m_Grids[y][x].fTerran[1] = y*0.5f;
			m_Grids[y][x].fTerran[2] = x*0.5f + 0.5f;
			m_Grids[y][x].fTerran[3] = y*0.5f + 0.5f;
			m_Grids[y][x].fWater[0] = x*0.25f;
			m_Grids[y][x].fWater[1] = y*0.25f;
			m_Grids[y][x].fWater[2] = x*0.25f + 0.25f;
			m_Grids[y][x].fWater[3] = y*0.25f + 0.25f;
		}
	}

	DivSphSurface( m_vecNormal, TERRAIN_NORMAL_YAW, m_uNorStr, m_uNorNum );
	for( int n = 1, i = TERRAIN_NORMAL_PIECE; n < 4; n++, i += TERRAIN_NORMAL_PIECE )
	{
		memcpy( m_vecNormal + i, m_vecNormal, sizeof( CVector3f )*TERRAIN_NORMAL_PIECE );
		CVector3f vecSG( n&TERRAIN_NORMAL_SGX ? -1.0f : 1.0f, 1, n&TERRAIN_NORMAL_SGZ ? -1.0f : 1.0f );
		for( int j = 0; j < TERRAIN_NORMAL_PIECE; j++ )
		{
			m_vecNormal[ i + j ].x *= vecSG.x;
			m_vecNormal[ i + j ].z *= vecSG.z;
		}
	}
}

uint8 CMetaStaticData::GetIndex( CVector3f vecNor )
{
	vecNor.Normalize();
	uint32 uIndex = GetIndexByVec( vecNor.y, vecNor.z, TERRAIN_NORMAL_YAW, s_Static.m_uNorStr, s_Static.m_uNorNum );
	uint16 _n = 0;
	if( vecNor.x < 0 )_n |= TERRAIN_NORMAL_SGX;
	if( vecNor.z < 0 )_n |= TERRAIN_NORMAL_SGZ;
	return static_cast<uint8>(_n*TERRAIN_NORMAL_PIECE + uIndex);
}