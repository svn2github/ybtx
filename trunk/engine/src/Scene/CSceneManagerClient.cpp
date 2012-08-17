#include "stdafx.h"
#include "CSceneManagerClient.h"
#include "CSceneClientCoder.h"
#include "CTerrainCoder.h"
#include "BaseHelper.h"
#include "CShadowedGrids.h"
#include "CBakedColorData.h"
#include "CMetaGridMgr.h"
#include "CAppClient.h"
#include "CRenderSystem.h"
#include "IProcessCallBack.h"
#include "CSceneConfig.h"
#include "CMetaStaticData.h"
#include "CVertexDeclaration.h"
#include "CodeCvs.h"
#include "CTexAniWaterEffect.h"
#include "CWaterCoder.h"
#include "CCameraPath.h"
#include "CGraphic.h"
#include "CLogicDataMgr.h"
#include "CTerrainMgr.h"
#include "CWaterMgr.h"
#include "CSkyMgr.h"
#include "CTerrainModelCoder.h"
#include "CLogicDataCoder.h"
#include "CGpuProgramMgr.h"
#include "TSqrAllocator.inl"
#include "CGraphic.h"

ITexture* CWaterSurTex::GetTexture( void )
{
	if( !m_Texture )
	{
		if( FAILED(CGraphic::GetInst()->CreateTexture(  PATH_ALIAS_RES.c_str(), m_TextName.c_str(), &m_Texture ) ) )
		{
			ostringstream strm;
			strm<<"CreateTexture failed with file \""<<m_TextName<<"\"";
			ArtErr(strm.str());
			return NULL;
		}
	}
	return m_Texture;
}

CWaterSurTex::~CWaterSurTex()
{
	SafeRelease( m_Texture );
}
//------------------------------------------------------------------------------
CSceneManagerClient::CSceneManagerClient( const SString& SceneName, CTreeScene* pTreeScene, CRenderGroup* pGroup )
: CDataScene( SceneName, pTreeScene, pGroup )
, m_pBakedColorData(new CBakedColorData())
, m_pGridMgr(new CMetaGridMgr)
{	
	CGraphic::GetInst()->SetAsynSwitch(false);
	SString strState = "CSceneManagerClient开始构造:" + SceneName;
	GraphicErr::SetErrState(strState.c_str());
	SQR_TRY
	{
		if(pGroup)
			Initialize();
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CSceneManagerClient::Initialize");
		GfkLogExp(exp);
	}
	SQR_TRY_END;
	
}

CSceneManagerClient::CSceneManagerClient( const SString& SceneName, CTreeScene* pTreeScene, CRenderGroup* pGroup, uint32 Wdith , uint32 Height, uint32 RecycleSize )
: CDataScene( SceneName, pTreeScene, pGroup, Wdith, Height, RecycleSize )
, m_pBakedColorData(new CBakedColorData())
, m_pGridMgr(new CMetaGridMgr)
{
	CGraphic::GetInst()->SetAsynSwitch(false);
	SString strState = "CSceneManagerClient开始构造:" + SceneName;
	GraphicErr::SetErrState(strState.c_str());
	SQR_TRY
	{
		if(pGroup)
			Initialize();
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CSceneManagerClient::Initialize");
		GfkLogExp(exp);
	}
	SQR_TRY_END;
}

void CSceneManagerClient::Initialize(void)
{
	CGraphic::GetInst()->SetAsynSwitch(false);
	m_fPercent = 0.0f;
	//m_bEnableBaking	= CSceneConfig::GetInst()->IsBakingEnabled();

	GetTreeScene()->SetCoder(new CTerrainModelCoder(this));
	CDataScene::Initialize();

	TerrainFlag::Mask mask;
	if( CSceneConfig::GetInst()->GetTerrainQuality() > 0 && CGpuProgramMgr::GetInst()->GetShaderVer()>= Shader_V_2 )
	{
		m_pTerrainMgr->SetCoder(new CShaderTerrainCoder(this));
		mask = TerrainFlag::Cliff | TerrainFlag::LightMap;
	}
	else
	{
		m_pTerrainMgr->SetCoder(new CNormalTerrainCoder(this));
		mask = TerrainFlag::Normal | TerrainFlag::LightMap;
	}

	
	//if (m_bEnableBaking)
	//{
	mask |= TerrainFlag::Baking;	
	//}
	m_pTerrainMgr->SetRenderFlag(mask);

	//初始化水
	m_pWaterMgr->SetLowestEffectTexNameVec( CSceneConfig::GetInst()->GetLakeTexNames() );
	m_pWaterMgr->SetWaterType((EWaterType)CSceneConfig::GetInst()->GetWaterType());
	m_pWaterMgr->SetCoder(new CWaterCoder(this));
	m_pLogicDataMgr->SetCoder(new CLogicDataCoder(this));

	//---------------------------------------------

	m_pClientCoder = new CSceneClientCoder;

	if(m_strSceneName.empty())
		return;

	SString tpStr = m_strSceneName + "_c.tex";
	if( !m_pTerrainMgr->LoadTexture(tpStr.c_str()) )
	{
		ostringstream strm;
		strm<<"Create terrain texture \""<<tpStr<<"\"!";
		GfkLogErr(GraphicErr::GetErrTypeStr(TextureCreate_Err),strm.str());
	}
}

CSceneManagerClient::~CSceneManagerClient()
{
	SString strState = "CSceneManagerClient开始析构:" + m_strSceneName;
	GraphicErr::SetErrState(strState.c_str());
	DelElementManager(m_pTerrainMgr);
	DelElementManager(m_pWaterMgr);
	DelElementManager(m_pSkyMgr);
	DelElementManager(m_pLogicDataMgr);
	SafeRelease(m_pSkyMgr);
	SafeRelease(m_pWaterMgr);
	SafeRelease(m_pTerrainMgr);
	GfkSafeDelete(m_pLogicDataMgr);
	size_t size = m_RgnsList.size();
	for( size_t i  = 0; i < size; ++i )
	{
		delete m_RgnsList[i];
		m_RgnsList[i] = NULL;
	}

	m_RgnsList.clear();
	m_SetUnit.clear();

	size = m_vecCamPath.size();
	for ( size_t i=0; i<size; ++i )
	{
		delete m_vecCamPath[i];
		m_vecCamPath[i]	= NULL;
	}
	m_vecCamPath.clear();

	ClearMap(m_SubRegionsMap);
	GfkSafeDelete(m_pShadowedGrids);
	GfkSafeDelete(m_pBakedColorData);
	GfkSafeDelete(m_pGridMgr);
	strState = "CSceneManagerClient析构完成:" + m_strSceneName;
	GraphicErr::SetErrState(strState.c_str());
}

void CSceneManagerClient::DoProcess( float fPercent )
{
	if( fPercent - m_fPercent < 2.0f )
		return;

	IProcessCallBack* pHandler= CRenderSystem::GetInst()->GetProcessCallBack();
	if( pHandler )
		pHandler->DoProcess( fPercent );

	m_fPercent = fPercent;
}

void CSceneManagerClient::OnPreLoad(void) 
{
	DoProcess( 0.0f );
}

void CSceneManagerClient::OnLoaded(void) 
{
	m_pShadowedGrids->Load(GetSceneName(), m_uWidthInGrid, m_uHeightInGrid);
	m_pBakedColorData->Load(GetSceneName(), m_uWidth, m_uHeight);	
	CalculateNomal();
	DoProcess( 30.0f );
	//	CMetaGridMgr& GridMgr = *m_pGridMgr;

	//	CGraphic::GetInst()->EnableAsynRead(false);

	//if( CSceneConfig::GetInst()->IsUseShaderTile() )
	//	InitShaderTerrain();
	//else
	//	InitNormalTerrain();


	//m_pTerrainMgr->TurnOnLightMap(true);

	//#if 1
	//	
	//	uint32 Wsize = GridMgr.GetWidthInRegion()*GridMgr.GetHeightInRegion();
	//	for(uint32 i =0; i<Wsize ;++i)
	//	{
	//		//InitWater(i);
	//		//DoProcess( i*40.0f/Wsize + 60.0f );
	//		//GridMgr.GetRegion(i)->Release();
	//	}
	//#endif
}

//-------------------  OffsetHeight = TerrainHeight + LogicHeight  -------------------
int16	CSceneManagerClient::GetOffsetHeight( uint32 uX, uint32 uY )
{ 
	return m_pGridMgr->GetOffsetHeight(uX,uY); 
}

//-------------------		TerranHeight	-------------------
int16	CSceneManagerClient::GetTerranHeight( uint32 uX, uint32 uY ) 
{ 
	return m_pGridMgr->GetTerranHeight(uX,uY);
}

//-------------------		TerranLogicHeight	-------------------
int16	CSceneManagerClient::GetTerranLogicHeight( uint32 uX, uint32 uY )
{
	return m_pGridMgr->GetTerranLogicHeight(uX,uY);
}

//-------------------		TerranNomal	-------------------
int8	CSceneManagerClient::GetTerranNomal( uint32 uX, uint32 uY ) 
{ 
	return m_pGridMgr->GetTerranNomal(uX,uY);
}

//-------------------		TerranNomal	-------------------
CVector3f CSceneManagerClient::GetTerranLogicNomal( uint32 uX, uint32 uY )
{
	return m_pGridMgr->GetTerranLogicNomal(uX,uY);
}

//-------------------		Terrain Material Index	-------------------
// int8	CSceneManagerClient::GetTerrainMaterialIndex( uint32 uX, uint32 uY )
// {
// 	return m_pGridMgr->GetTerrainMaterialIndex(uX,uY);
// }

CMetaRegion* CSceneManagerClient::GetRegion(uint32 uGridX, uint32 uGridY)
{
	return m_pGridMgr->GetRegion(uGridX,uGridY);
}

CMetaRegion* CSceneManagerClient::GetRegion(uint32 RegionId)
{
	return m_pGridMgr->GetRegion(RegionId);
}

const GridColor* CSceneManagerClient::GetGridColor(uint32 RegionId )
{
	//if(!m_bEnableBaking)
	//	return NULL;
	return m_pBakedColorData->GetGridColor(RegionId);
}

CVertexHardwareBuffer* CSceneManagerClient::GetColorBuffer(void)
{
	return m_pBakedColorData->GetColorBuffer();
}

///得到当前像素的高度 = TerrainHeight + LogicHeight
//float CSceneManagerClient::GetOffsetHeightInPixel( float fPixelX, float fPixelY )
//{
//	return m_pGridMgr->GetOffsetHeightInPixel(fPixelX,fPixelY);
//}
//
//float CSceneManagerClient::GetOffsetHeightInPixel( float fPixelX, float fPixelY, bool& bHasLogicHeight )
//{
//	return m_pGridMgr->GetOffsetHeightInPixel(fPixelX,fPixelY,bHasLogicHeight);
//}
//
//bool CSceneManagerClient::IsGridValid( uint32 uX, uint32 uY )
//{
//	return m_pGridMgr->IsGridValid( uX, uY );
//}

void CSceneManagerClient::CalculateNomal()
{
	CPos DeltaVer[4] = { CPos( -1, 0 ), CPos( 0, 1 ), CPos( 1, 0 ), CPos( 0, -1 ) };

	for( uint32 y = 1; y < m_uHeightInGrid - 1; ++y )
	{
		for( uint32 x = 1; x < m_uWidthInGrid - 1; ++x )
		{
			int16 nHeight[4], nAllHeight[4];
			int32 j = 0;
			for( ; j < 4; ++j )
			{
				CPos posVer( x + DeltaVer[j].x, y + DeltaVer[j].y );
				if( !m_pGridMgr->IsGridValid( posVer.x, posVer.y ))
					break;
				nHeight[j]    = GetTerranHeight( posVer.x, posVer.y );
				nAllHeight[j] = GetOffsetHeight( posVer.x, posVer.y );
			}

			if( j < 4 )
				continue;
			// Slope法线生成算法
			uint32 nNorId = ( y&0xf )*eRegionSpan + ( x&0xf );
			CVector3f vecNor( float( nHeight[0] - nHeight[2] ), eGridSpan*2.0f, float( nHeight[3] - nHeight[1] ) );

			CVector3f vecLogicNor( float( nAllHeight[0] - nAllHeight[2] ), eGridSpan*2.0f, float( nAllHeight[3] - nAllHeight[1] ) );
			vecLogicNor.normalize();

			m_pGridMgr->GetRegion( x, y )->SetTerranNomal( nNorId, CMetaStaticData::GetIndex( vecNor ) );
			m_pGridMgr->GetRegion( x, y )->SetTerranLogicNomal( nNorId, vecLogicNor );
		}
	}
}
//------------------------------------------------------------------------------
//void CSceneManagerClient::InitWater( uint32 regionId )
//{
//
//}
