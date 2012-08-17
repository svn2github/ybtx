#include "stdafx.h"
#include "CMetaRegion.h"
#include "BaseHelper.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

CMetaRegion::RegionTempData::RegionTempData()
{
	memset( m_aryTerranBakingColor,		0, sizeof( m_aryTerranBakingColor ) );
	memset( m_aryTerranSpecularColor,   0, sizeof( m_aryTerranSpecularColor ) );
	memset( m_aryLayer1UV,				0, sizeof( m_aryLayer1UV ) );
	memset( m_aryLayer2UV,				0, sizeof( m_aryLayer2UV ) );
	memset( m_aryLayer3UV,				0, sizeof( m_aryLayer3UV ) );
	memset( m_aryLayer4UV,				0, sizeof( m_aryLayer4UV ) );
	memset( m_aryTextureIndex,			0, sizeof( m_aryTextureIndex ) );
	memset( m_aryHollowMask,			0, sizeof( m_aryHollowMask ) );
}

CMetaRegion::CMetaRegion( uint16 uRegionId )
: m_uRegionId( uRegionId )
, m_pWaterInfo( NULL )
, m_pTrapInfo(NULL)
, m_bSetRegionEnv(false)
{
	m_pTempData = new RegionTempData;
	memset( m_aryOffsetHeight,	     0, sizeof( m_aryOffsetHeight ) );
	memset( m_aryTerranHeight,	     0, sizeof( m_aryTerranHeight ) );
	memset( m_aryTerranNomal,	     0, sizeof( m_aryTerranNomal ) );
	memset( m_aryTerranLogicNomal,	 0, sizeof( m_aryTerranLogicNomal ) );
	memset( m_aryTerranDiffuseColor, 0, sizeof( m_aryTerranDiffuseColor ) );
	memset( m_aryTerrainMatIndex,    0, sizeof( m_aryTerrainMatIndex ) );
	m_strRegionMusicName = "";
}

CMetaRegion::~CMetaRegion()
{
	GfkSafeDelete(m_pTrapInfo);
	GfkSafeDelete(m_pWaterInfo);
	GfkSafeDelete(m_pTempData);
}

//void CMetaRegion::Release()
//{
//	
//}