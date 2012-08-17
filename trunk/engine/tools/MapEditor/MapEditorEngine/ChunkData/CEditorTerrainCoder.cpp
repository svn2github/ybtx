#include "stdafx.h"
#include "../ChunkData/CEditorTerrainCoder.h"
#include "LoadMapMgr.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "LocalMapResMgr.h"

BEGIN_VERSION_MAP(CEditorTerGridCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CEditorTerGridCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int			  nGridIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	int16		  nFirstTextureIndex = 0, nSecondTextureIndex = 0;
	string		  strName = "";
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	map<string, int16> &terrainTextureNames = CLoadMapResInfo::GetInst()->m_mapTerrainTextureNames;

// 	for (DWORD i = 0; i < dwGridCntInTileRegion; ++i )
// 	{
// 		nGridIndex = pTileRegion->vGridIndices[i];
// 		SGrid &grid = pTerrain->GetGrid(nGridIndex);
// 
// 		//1
// 		strName = grid.nodeInfo.strTextureNames[0];
// 		nFirstTextureIndex = terrainTextureNames[strName];
// 		wdi.Write(&nFirstTextureIndex,			 sizeof(nFirstTextureIndex));
// // 		if( nGridIndex == 9668 )
// // 			DebugOut("Save %d, %s %d\n", nGridIndex, strName.c_str(), nFirstTextureIndex);
// 	
// 		//2
// 		strName = grid.nodeInfo.strTextureNames[1];
// 		nSecondTextureIndex = terrainTextureNames[strName];
// 		wdi.Write(&nSecondTextureIndex,			 sizeof(nSecondTextureIndex));
// 
// 
// 		wdi.Write(&grid.bLowBlockNotEffectByTex, sizeof(bool));
// 		wdi.Write(&grid.bMatNotEffectByTileSets, sizeof(bool));
// 		wdi.Write(&grid.bCliffLayerOne,			 sizeof(bool));
// 
// 		wdi.Write(&grid.vCenter.y, sizeof(float));
// 
// 		///物体是否受顶点色影响
// 		wdi.Write(&grid.bEffectByVertexColor,	 sizeof(bool));
// 
// 		///保存顶点色是否影响场景物体标志
// 		wdi.Write(&grid.bNpcIsInfluence,		 sizeof(bool));
// 	}

	return true;
}

bool sqr::CEditorTerGridCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int			  nGridIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	int16		  nTextureIndex = 0;
	string		  gridtexname = "";
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	map<int16, string> &terrainTextureNames = CLoadMapResInfo::GetInst()->m_mapIndexTextureNames;
	set<string> terrainTexIndexsSet;

	string strDefaultTerrainTexName = pTerrain->GetTerrainDefaultTextureName();

	for (DWORD i = 0; i < dwGridCntInTileRegion; ++i )
	{
		nGridIndex = /*pTileRegion->vGridIndices[i]*/0;
		SGrid &grid = pTerrain->GetGrid(nGridIndex);

		//////////////////////////////////////////////////////////////
		//读取纹理信息
		//1
		tData.SafeRead( &nTextureIndex,	sizeof(nTextureIndex) );
		gridtexname = terrainTextureNames[nTextureIndex] ;
		grid.nodeInfo.strTextureNames[0]  = gridtexname;
		if ( terrainTexIndexsSet.find(gridtexname) == terrainTexIndexsSet.end() && gridtexname != "" )
		{
			terrainTexIndexsSet.insert(gridtexname);;
		}

// 		if( nGridIndex == 9668 )
// 			DebugOut("Load %d, %s %d\n", nGridIndex, gridtexname.c_str(), nTextureIndex);

		//2
		tData.SafeRead( &nTextureIndex,	sizeof(nTextureIndex) );
		gridtexname= terrainTextureNames[nTextureIndex] ;
		grid.nodeInfo.strTextureNames[1]  = gridtexname;
		if ( terrainTexIndexsSet.find(gridtexname) == terrainTexIndexsSet.end() && gridtexname != "" )
		{
			terrainTexIndexsSet.insert(gridtexname);
		}

		// 保证至少有第一层纹理，而且不会出现第一层纹理为空而存在第二层纹理的情况
		if ( grid.nodeInfo.strTextureNames[0] == "" )
		{
			if ( grid.nodeInfo.strTextureNames[1] == "" ) 
			{
				grid.nodeInfo.strTextureNames[0] = strDefaultTerrainTexName;
			}
			else
			{
				grid.nodeInfo.strTextureNames[0] = grid.nodeInfo.strTextureNames[1];
				grid.nodeInfo.strTextureNames[1];
			}
		}

		//3
		grid.nodeInfo.strTextureNames[2] = "" ;
		//4
		grid.nodeInfo.strTextureNames[3] = "" ;

		tData.SafeRead( &grid.bMatNotEffectByTileSets,	sizeof(bool) );
		tData.SafeRead( &grid.bCliffLayerOne,			sizeof(bool) );

		tData.SafeRead( &grid.vCenter.y,				sizeof(float) );

		///物体是否受顶点色影响
		tData.SafeRead( &grid.bEffectByVertexColor,		sizeof(bool) );
	}

	
	return true;
}
//////////////////////////////////////////////////////////////////////////

BEGIN_VERSION_MAP(CEditorTerWaterCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CEditorTerWaterCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	return true;
}

bool sqr::CEditorTerWaterCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CTerrainMesh *pTerrain =CMapEditApp::GetInst()->GetTerrain();

	return true;
}

void sqr::CEditorTerWaterCoder::LoadWaveSideInfo(CBufFile& tData)
{

}

void sqr::CEditorTerWaterCoder::SaveWaveSideInfo(WriteDataInf& wdi)
{

}
//////////////////////////////////////////////////////////////////////////

CEditorTerrainFormat::CEditorTerrainFormat(void)
: CFileFormatMgr( 'STFT', 0 ) //src terrain format
{
	InitTerrainCode();
}

CEditorTerrainFormat::~CEditorTerrainFormat(void)
{ /*do nothing~~~*/ }

void sqr::CEditorTerrainFormat::InitTerrainCode()
{
	RegisterCoder(new CEditorTerGridCoder);
	RegisterCoder(new CEditorTerWaterCoder);
}

bool sqr::CEditorTerrainFormat::LoadTerrainFile( const string& strFile, int nRegionIndex )
{
	return Load( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex)  );
}

bool sqr::CEditorTerrainFormat::SaveTerrainFile( const string& strFile, int nRegionIndex )
{
	return Save( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex)  ); 
}