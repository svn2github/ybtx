#include "stdafx.h"
#include "CDataScene.h"
#include "CShadowedGrids.h"
#include "CLogicDataMgr.h"
#include "CTerrainMgr.h"
#include "CWaterMgr.h"
#include "CSkyMgr.h"
#include "CRegion.h"
#include "BaseHelper.h"
//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

CSceneCoder::CSceneCoder(void)
: m_pLockScene(NULL)
{
}

bool CSceneCoder::Lock(CDataScene* pScene)
{
	if(m_pLockScene==NULL)
	{
		m_pLockScene = pScene;
		return true;
	}
	return false;
}

void CSceneCoder::UnLock(void)
{
	m_pLockScene = NULL;
}

//---------------------------------------------
CDataScene::CDataScene( const SString& SceneName, CTreeScene* pTreeScene, CRenderGroup* pRenderGroup )
: CBaseScene(SceneName,pTreeScene)
, m_pRenderGroup(pRenderGroup)
, m_pShadowedGrids(new CShadowedGrids())
, m_pClientCoder(NULL)
, m_pTerrainMgr(NULL)
, m_pWaterMgr(NULL)
, m_pSkyMgr(NULL)
, m_pLogicDataMgr(NULL)
{
	m_uWidthInRegion	= 0;		// 场景的宽度，以1/4区域为单位
	m_uHeightInRegion	= 0;		// 场景的高度，以1/4区域为单位
	m_uWidthInGrid		= 0;		// 场景的宽度，以格子为单位
	m_uHeightInGrid		= 0;		// 场景的高度，以格子为单位
	m_uWidthInPixel		= 0;		// 场景的宽度，以象素为单位
	m_uHeightInPixel	= 0;		// 场景的高度，以象素为单位
}

CDataScene::CDataScene( const SString& SceneName, CTreeScene* pTreeScene, CRenderGroup* pRenderGroup, uint32 Wdith , uint32 Height, uint32 RecycleSize )
: CBaseScene(SceneName,pTreeScene,Wdith,Height,RecycleSize)
, m_pRenderGroup(pRenderGroup)
, m_pShadowedGrids(new CShadowedGrids())
, m_pClientCoder(NULL)
, m_pTerrainMgr(NULL)
, m_pWaterMgr(NULL)
, m_pSkyMgr(NULL)
, m_pLogicDataMgr(NULL)
{
	m_uWidthInRegion	= 0;		// 场景的宽度，以1/4区域为单位
	m_uHeightInRegion	= 0;		// 场景的高度，以1/4区域为单位
	m_uWidthInGrid		= 0;		// 场景的宽度，以格子为单位
	m_uHeightInGrid		= 0;		// 场景的高度，以格子为单位
	m_uWidthInPixel		= 0;		// 场景的宽度，以象素为单位
	m_uHeightInPixel	= 0;		// 场景的高度，以象素为单位
}

CDataScene::~CDataScene()
{
	SafeDelete(m_pClientCoder);
}

void CDataScene::Create( void )
{
	OnPreLoad();

	m_isInitialize = true;
	if(m_pClientCoder)
		m_isInitialize = m_pClientCoder->SceneDeCode( this );

	if(m_isInitialize)
	{
		m_ImpHalfWidth	= (m_ImpWidth>m_uWidth	||m_ImpWidth<2	) ?  m_uWidth / 2 : m_ImpWidth / 2;
		m_ImpHalfHeight = (m_ImpHeight>m_uHeight||m_ImpHeight<2	) ?  m_uHeight / 2 : m_ImpHeight / 2;
		OnLoaded();
	}
}

void CDataScene::Destroy( void )
{
	m_isInitialize = false;
}

SubRegionData* CDataScene::GetSubRegion( const int index )
{
	SMap<int,SubRegionData*>::iterator iter = m_SubRegionsMap.find(index);

	if( iter != m_SubRegionsMap.end() )
		return iter->second;
	else
	{
		return NULL;
	}
}

RegionsInfo* CDataScene::GetRegionInfo( const int index )
{
	if( index >= (int)m_RgnsList.size() )
	{
		ostringstream strm;
		strm << "Invalid Region Info Index: "  << index;
		GenErr(GraphicErr::GetErrTypeStr(InvalidIndex_Err),strm.str());
		return NULL;
	}

	return m_RgnsList[index];
}


float	CDataScene::GetOffsetHeightInPixel( float fPixelX, float fPixelY )
{	
	//可优化
	float n			  = 1.0f/eSGridSpan;
	float x			  = ((static_cast<int32>(fPixelX) & 0x3f) + fPixelX - static_cast<int32>(fPixelX)) * n;
	float z			  = ((static_cast<int32>(fPixelY) & 0x3f) + fPixelY - static_cast<int32>(fPixelY)) * n;
	
	int16 xGrid		  = static_cast<int16>(fPixelX/eSGridSpan);
	int16 yGrid		  = static_cast<int16>(fPixelY/eSGridSpan);
	int16 xGridLeft   = xGrid ? xGrid - 1:0;
	int16 yGridBottom = yGrid ? yGrid - 1:0;

	float p0 = 0.0f, p1 = 0.0f, p2 = 0.0f, p3 = 0.0f;

	p0	= IsGridValid(xGridLeft	, yGridBottom	)?m_pLogicDataMgr->GetLogicHight( xGridLeft, yGridBottom	):0.0f;
	p1  = IsGridValid(xGridLeft	, yGrid			)?m_pLogicDataMgr->GetLogicHight( xGridLeft, yGrid			):0.0f;
	p2  = IsGridValid(xGrid		, yGridBottom	)?m_pLogicDataMgr->GetLogicHight( xGrid,     yGridBottom	):0.0f;
	p3	= IsGridValid(xGrid		, yGrid			)?m_pLogicDataMgr->GetLogicHight( xGrid,	  yGrid			):0.0f;

	return TriInterpolate( p0, p1, p2, p3, x, z );
}

//float	CDataScene::GetOffsetHeightInPixel( float fPixelX, float fPixelY, bool& bHasLogicHeight )
//{
//	return 0.0f;
//}

bool	CDataScene::IsGridValid( uint32 uX, uint32 uY )
{
	return uX < m_uWidthInGrid && uY < m_uHeightInGrid && 
		m_RegionMgr.find(URegionID(int16(uX>>6),int16(uY>>6)))!=m_RegionMgr.end();
}

sqr::RegionsInfo::~RegionsInfo()
{
	size_t size = rgnAmbientFxList.size();
	for( size_t i  = 0; i < size; ++i )
	{
		delete rgnAmbientFxList[i];
		rgnAmbientFxList[i] = NULL;
	}
}

void sqr::CDataScene::SetCoder( CSceneCoder* pCoder )
{
	if(pCoder!=m_pClientCoder)
		SafeDelete(m_pClientCoder);

	m_pClientCoder = pCoder;
}

void sqr::CDataScene::Initialize( void )
{
	if(NULL==m_pTerrainMgr)		m_pTerrainMgr	= new CTerrainMgr( m_pRenderGroup );
	if(NULL==m_pWaterMgr)		m_pWaterMgr		= new CWaterMgr( m_pRenderGroup );
	if(NULL==m_pSkyMgr)			m_pSkyMgr		= new CSkyMgr( CMainWindowTarget::GetInst() );
	if(NULL==m_pLogicDataMgr)	m_pLogicDataMgr = new CLogicDataMgr();

	AddElementManager( m_pTerrainMgr );
	AddElementManager( m_pWaterMgr );
	AddElementManager( m_pLogicDataMgr );
}