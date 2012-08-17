#include "StdAfx.h"
#include "LocalMapResMgr.h"
#include "ChunkData/CEditorBlockCoder.h"
#include "ChunkData/CGameBlockCoder.h"
#include "ChunkData/CGameAmpArsCoder.h"
#include "ChunkData/CEditorAmpArsCoder.h"
#include "ChunkData/CGameTerrainCoder.h"
#include "ChunkData/CEditorTerrainCoder.h"
#include "ChunkData/CGameAudioCoder.h"
#include "ChunkData/CGameLightCoder.h"
#include "ChunkData/CGameObjectCoder.h"
#include "ChunkData/CEditorObjectCoder.h"
#include "ChunkData/CreateSceneObjectMgr.h"
#include "IoSystem.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "TerrainMeshObject.h"
#include "IoSystem.h"
#include "StringHelper.h"
#include "SceneRegion.h"
#include "Render.h"
#include "CGraphic.h"
#include "LocalVersion.h"

sqr::CLoadMapResInfo::CLoadMapResInfo( void )
{

}

sqr::CLoadMapResInfo::~CLoadMapResInfo( void )
{
	this->DestroyEditorModelInfo();
}

void  CLoadMapResInfo::DestroyEditorModelInfo()
{
	size_t size = vecEditorSingleModelInfo.size();
	for (size_t i = 0; i < size; ++i )
	{
		delete vecEditorSingleModelInfo[i];
	}
	vecEditorSingleModelInfo.clear();

	size = vecEditorLoginModelInfo.size();
	for (size_t i = 0; i < size; ++i )
	{
		delete vecEditorLoginModelInfo[i];
	}
	vecEditorLoginModelInfo.clear();

	size = vecEditorPointLightOffsetInfo.size();
	for (size_t i = 0; i < size; ++i )
	{
		delete vecEditorPointLightOffsetInfo[i];
	}
	vecEditorPointLightOffsetInfo.clear();

	size = vecEditorSpotLightOffsetInfo.size();
	for (size_t i = 0; i < size; ++i )
	{
		delete vecEditorSpotLightOffsetInfo[i];
	}
	vecEditorSpotLightOffsetInfo.clear();

	size = vecEditorAudioOffsetInfo.size();
	for (size_t i = 0; i < size; ++i )
	{
		delete vecEditorAudioOffsetInfo[i];
	}
	vecEditorAudioOffsetInfo.clear();
}

void CLoadMapResInfo::DestroyAllContainerInfo()
{
	m_SetUnit.clear();
	m_CueNameUnit.clear();

	m_mapTerrainTextureNames.clear();
	m_mapIndexTextureNames.clear();///用于记录地图中用到的所有纹理的名字
	m_setTerrainTexIndexs.clear();
	m_vecWaterTextureNames.clear();
}

void sqr::CLoadMapResInfo::DealGridObjectsToTileRegion()
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	DWORD		  dwTileRegionCnt		= pTerrain->GetTileRegionCount();
	int			  nGridIndex = 0;
	int16		  nFirstTextureIndex = 0, nSecondTextureIndex = 0;
	string		  strName = "";

	for ( DWORD j = 0; j < dwTileRegionCnt; ++j )
	{
		STileRegion *pTileRegion = pTerrain->GetTileRegion(j);
	}
}

//------------------------------------------------------------------------------
void sqr::CLoadMapResInfo::MakeTerrainTexture()
{
	CRenderDevice * pRender = NULL;
	pRender = CRenderDevice::GetInst();
	if ( pRender == NULL )
		return;

	CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetTerrain();

	int texCnt = 0;
	ITexture* pTexture = NULL;

	if ( pTerrain->m_pTerrainTexture == NULL )
		CGraphic::GetInst()->CreateEmptyTexture(EXPAND_MAX_SIZE,EXPAND_MAX_SIZE,1, TF_XRGB32,&(pTerrain->m_pTerrainTexture));

	string texname = "";
	map<string, int16>::iterator iter = m_mapTerrainTextureNames.begin();
	map<string, int16>::iterator eiter = m_mapTerrainTextureNames.end();
	set<string> erroTexList;
	for ( iter; iter != eiter; ++iter )
	{
		texname = iter->first;
		if( texname.empty() )
			continue;

		pTexture = pRender->GetTexture(texname);
		if( texname.find("water.dds") != -1 )
			continue;

		if (NULL==pTexture)
		{
			string log = "拼接过程:"+texname+"纹理无法找到";
			MessageBox(NULL,log.c_str(),"错误",MB_OK);
			erroTexList.insert(texname);
			continue;
		}

		if (NULL==pTexture->GetBuffer())
		{
			string log = "拼接过程:"+texname+"纹理无法找到";
			MessageBox(NULL,log.c_str(),"错误",MB_OK);
			continue;
		}

		UINT H = (texCnt/ROW_SIZE)*EXPAND_TEXTURE_SIZE;
		UINT W = (texCnt%ROW_SIZE)*EXPAND_TEXTURE_SIZE;


		if (pTexture->GetWidth()!=TEXTURE_SIZE||pTexture->GetHeight()!=TEXTURE_SIZE)
		{
			string log = "拼接过程:"+texname+"尺寸不合法,所有贴图纹理应为128X128";
			erroTexList.insert(texname);
			MessageBox(NULL,log.c_str(),"错误",MB_OK);
			continue;
		}

		pRender->m_TerrainTexIndexs[texname] = texCnt;
		//DebugOut("%d, %s\n", texCnt, texname.c_str());
		++texCnt;

		CIRect rectT,rectO;
		_TEXLOCKED_RECT lockT,lockO;
		rectO.top = rectO.left = 0;
		rectO.bottom = rectO.right = TEXTURE_SIZE;
		rectT.top = H;
		rectT.bottom = H + EXPAND_TEXTURE_SIZE;
		rectT.left = W;
		rectT.right = W + EXPAND_TEXTURE_SIZE;
		pTerrain->m_pTerrainTexture->Lock(0,&lockT,&rectT,0,LOCK_DISCARD);
		pTexture->Lock(0,&lockO,&rectO,0,LOCK_READONLY);
		//pTerrain->pTar = (TCHAR*)lockT.pBits;
		TCHAR* pTar = (TCHAR*)lockT.pBits;
		TCHAR* pOrc = (TCHAR*)lockO.pBits;

		for (UINT i=0;i<EXPAND_SIZE;++i,pTar+=lockT.Pitch)
		{
			for ( UINT j=0;j<EXPAND_SIZE;++j)
			{
				memcpy(pTar+PixelSize*j,pOrc,PixelSize);
				memcpy(pTar+lockO.Pitch+PixelSize*(j+EXPAND_SIZE),pOrc+lockO.Pitch-PixelSize,PixelSize);
			}
			memcpy(pTar+PixelSize*EXPAND_SIZE,pOrc,lockO.Pitch);
		}

		for (UINT i = 0;i<TEXTURE_SIZE;++i,pTar+=lockT.Pitch,pOrc+=lockO.Pitch)
		{

			for ( UINT j=0;j<EXPAND_SIZE;++j)
			{
				memcpy(pTar+PixelSize*j,pOrc,PixelSize);
				memcpy(pTar+lockO.Pitch+PixelSize*(j+EXPAND_SIZE),pOrc+lockO.Pitch-PixelSize,PixelSize);
			}
			memcpy(pTar+PixelSize*EXPAND_SIZE,pOrc,lockO.Pitch);
		}
		pOrc-=lockO.Pitch;
		for (UINT i=0;i<EXPAND_SIZE;++i,pTar+=lockT.Pitch)
		{
			for ( UINT j=0;j<EXPAND_SIZE;++j)
			{
				memcpy(pTar+PixelSize*j,pOrc,PixelSize);
				memcpy(pTar+lockO.Pitch+PixelSize*(j+EXPAND_SIZE),pOrc+lockO.Pitch-PixelSize,PixelSize);
			}
			memcpy(pTar+PixelSize*EXPAND_SIZE,pOrc,lockO.Pitch);
		}

		pTexture->Unlock(0);
		pTerrain->m_pTerrainTexture->Unlock(0);
	}

	///删掉载入不成功的纹理列表
	set<string>::iterator erroriter = erroTexList.begin();
	set<string>::iterator eeiter = erroTexList.end();
	map<string, DWORD>::iterator fiter;
	map<string, DWORD>::iterator feiter = pRender->m_TerrainTexIndexs.end();
	for (erroriter; erroriter != eeiter; ++erroriter)
	{
		texname = *erroriter;

		fiter = pRender->m_TerrainTexIndexs.find(texname);

		if ( fiter != feiter )
			pRender->m_TerrainTexIndexs.erase(fiter);
	}

	int nGridCnt = pTerrain->GetGridCount();
	string gridtexname = "";
	for ( int i = 0; i < nGridCnt; ++i )
	{
		SGrid &Grid = pTerrain->GetGrid(i);

		gridtexname = Grid.nodeInfo.strTextureNames[0];
		erroriter = erroTexList.find(gridtexname);
		if (erroriter != eeiter)
			Grid.nodeInfo.strTextureNames[0] = "";

		gridtexname = Grid.nodeInfo.strTextureNames[1];
		erroriter = erroTexList.find(gridtexname);
		if (erroriter != eeiter)
			Grid.nodeInfo.strTextureNames[1] = "";

		gridtexname = Grid.nodeInfo.strTextureNames[2];
		erroriter = erroTexList.find(gridtexname);
		if (erroriter != eeiter)
			Grid.nodeInfo.strTextureNames[2] = "";
	}
	texCnt = 0;
	map<string, DWORD>::iterator titer = pRender->m_TerrainTexIndexs.begin();
	map<string, DWORD>::iterator teiter = pRender->m_TerrainTexIndexs.end();

	for ( titer; titer != teiter; ++titer )
	{
		texname = titer->first;
		pRender->m_TerrainTexIndexs[texname] = texCnt;

		++texCnt;
	}

	IoSystem* ioSystem = IoSystem::GetInst();
	string homePath = IoSystem::GetInst()->GetFullPath("home:");
	string selfTexPath = homePath + pTerrain->m_strMapName + ".jpg";

	pTerrain->m_pTerrainTexture->SaveToFile(selfTexPath.c_str(),IFF_JPG);
	//D3DXSaveTextureToFile(selfTexPath.c_str(),D3DXIFF_JPG,pTerrain->m_pTerrainTexture,&pale);
}

//////////////////////////////////////////////////////////////////////////
