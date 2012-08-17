#include "stdafx.h"
#include "../ChunkData/CGameTerrainCoder.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "WaterOperator.h"
#include "CWaterRegion.h"
#include "Render.h"
#include "LoadMapMgr.h"
#include "LocalVersion.h"
#include "River.h"

///将region存储在terrain中

BEGIN_VERSION_MAP(CGameTerVertexCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool CGameTerVertexCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwVerCntInTileRegion = pTerrain->GetVertexCountInTileRegion();
	int			  nVertexIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	DWORD		  oColor = VERTEX_COLOR, sColor = VERTEX_COLOR, tColor = VERTEX_COLOR;//原始顶点色，高光顶点色，烘培色
	float         fPositionY = 0.0f;
	CVector3f     vNormal(0.0, 1.0f, 0.0f);
	short         sLogicHeight = 0;

	for (DWORD i = 0; i < dwVerCntInTileRegion; ++i )
	{
		nVertexIndex = pTileRegion->vVertexIndices[i];

		SVertex &vertex = pTerrain->GetVertex(nVertexIndex);

		oColor		 = vertex.oColor;
		sColor		 = vertex.sColor;
		tColor		 = vertex.tColor;
		fPositionY   = vertex.vPosition.y;
		vNormal		 = vertex.vNormal;
		sLogicHeight = short(vertex.fLogicHeight);

		wdi.Write(&fPositionY, sizeof(fPositionY));
		wdi.Write(&vNormal, sizeof(vNormal));
		wdi.Write(&oColor, sizeof(oColor));
		wdi.Write(&sColor, sizeof(sColor));
		wdi.Write(&tColor, sizeof(tColor));
		wdi.Write(&sLogicHeight, sizeof(sLogicHeight));
	}

	return true;
}

bool CGameTerVertexCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	CTerrainMesh  *pWater = pTerrain->GetWater();
	DWORD		  dwVerCntInTileRegion = pTerrain->GetVertexCountInTileRegion();
	int			  nVertexIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	DWORD		  oColor = VERTEX_COLOR, sColor = VERTEX_COLOR, tColor = VERTEX_COLOR;//原始顶点色，高光顶点色，烘培色
	float         fPositionY = 0.0f;
	CVector3f     vNormal(0.0, 1.0f, 0.0f);
	short         sLogicHeight = 0;

	for (DWORD i = 0; i < dwVerCntInTileRegion; ++i )
	{
		nVertexIndex = pTileRegion->vVertexIndices[i];

		SVertex &vertex = pTerrain->GetVertex(nVertexIndex);

		oColor		 = vertex.oColor;
		sColor		 = vertex.sColor;
		tColor		 = vertex.tColor;
		fPositionY   = vertex.vPosition.y;
		vNormal		 = vertex.vNormal;
		sLogicHeight = short(vertex.fLogicHeight);

		tData.SafeRead( &fPositionY,	sizeof(fPositionY) );
		tData.SafeRead( &vNormal,		sizeof(vNormal) );
		tData.SafeRead( &oColor,		sizeof(oColor) );
		tData.SafeRead( &sColor,		sizeof(sColor) );
		tData.SafeRead( &tColor,	    sizeof(tColor) );
		tData.SafeRead( &sLogicHeight,	sizeof(sLogicHeight) );

		vertex.vPosition.y  = fPositionY;
		vertex.vNormal		= vNormal;
		vertex.oColor		= oColor;
		vertex.sColor		= sColor;
		vertex.tColor		= tColor;
		vertex.bColor		= tColor;
		vertex.fLogicHeight = sLogicHeight;

		pWater->GetVertex(nVertexIndex).vPosition.y = fPositionY + WATER_LAYER_BASE;
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////////////

BEGIN_VERSION_MAP(CGameTerGridCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool CGameTerGridCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CRenderDevice *pRender = CRenderDevice::GetInst();
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int			  nGridIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	
	for ( uint m = 0; m < CTerrainMesh::Tile_PerSqr; ++m )
	{
		STileRenderRegionUnit *pRenderRegionUnit = pTileRegion->vRenderRegionUnits[m];

		nGridIndex = /*pRenderRegionUnit->vGridIndices[i]*/0;
		SGrid &grid = pTerrain->GetGrid(nGridIndex);

		string strFirst = grid.nodeInfo.strTextureNames[0];
		string strSecond = grid.nodeInfo.strTextureNames[1];

		BYTE byFirst = 0, bySecond = 0;

		if ( strFirst == "")
			byFirst = 0xff;
		else
			byFirst = (BYTE)pRender->m_TerrainTexIndexs[strFirst];

		if ( strSecond == "")
			bySecond = 0xff;
		else
			bySecond = (BYTE)pRender->m_TerrainTexIndexs[strSecond];

		wdi.Write(&byFirst, sizeof(byFirst));
		wdi.Write(&bySecond, sizeof(bySecond));

		wdi.Write(&grid.uv[0][0], sizeof(CVector2f));
		wdi.Write(&grid.uv[0][1], sizeof(CVector2f));
		wdi.Write(&grid.uv[0][2], sizeof(CVector2f));
		wdi.Write(&grid.uv[0][3], sizeof(CVector2f));

		//grid hide mask
		wdi.Write(&grid.bGridHide,sizeof(bool));
		// 		if( grid.bGridHide )
		// 			DebugOut("gird %d, bhide %d\n", nGridIndex, grid.bGridHide);

		wdi.Write(&grid.nMaterialIndex, sizeof(grid.nMaterialIndex));
	}

	return true;
}

bool CGameTerGridCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int			  nGridIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);

	for (DWORD i = 0; i < dwGridCntInTileRegion; ++i )
	{
		nGridIndex = /*pTileRegion->vGridIndices[i]*/0;
		SGrid &grid = pTerrain->GetGrid(nGridIndex);

		BYTE byFirst = 0, bySecond = 0;

		tData.SafeRead( &byFirst,	sizeof(byFirst) );
		tData.SafeRead( &bySecond,	sizeof(bySecond) );

		tData.SafeRead( &grid.uv[0][0], sizeof(CVector2f) );
		tData.SafeRead( &grid.uv[0][1], sizeof(CVector2f) );
		tData.SafeRead( &grid.uv[0][2], sizeof(CVector2f) );
		tData.SafeRead( &grid.uv[0][3], sizeof(CVector2f) );

		//grid hide mask
		tData.SafeRead( &grid.bGridHide, sizeof(bool) );

//  		if( grid.bGridHide )
//  			DebugOut("gird %d, bhide %d\n", nGridIndex, grid.bGridHide);

		tData.SafeRead(&grid.nMaterialIndex, sizeof(grid.nMaterialIndex));
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////

BEGIN_VERSION_MAP(CGameTerWaterCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CGameTerWaterCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	CTerrainMesh *pWater   = pTerrain->GetWater();
	DWORD		 dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int			  nGridIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	map< CRiver*, vector<int> > mapRivers;
	vector<int>::iterator indexiter;

	set<int> setGridIndicesVisibleWater = pWater->GetGridIndicesVisibleWater();
	for ( set<int>::iterator iter = setGridIndicesVisibleWater.begin(); iter != setGridIndicesVisibleWater.end();  )
	{
		int nGridIndex = *iter;
		++iter;

		SGrid & gridW = pWater->GetGrid(nGridIndex);
		Ast(gridW.bVisible);

// 		if ( gridW.pRiver )
// 		{
// 			///将属于这个区域的格子索引存储
// 			indexiter = find(pTileRegion->vGridIndices.begin(), pTileRegion->vGridIndices.end(), nGridIndex);
// 			if ( indexiter != pTileRegion->vGridIndices.end() )
// 			{
// 				mapRivers[gridW.pRiver].push_back(nGridIndex);
// 			}
// 
// 			setGridIndicesVisibleWater.erase(nGridIndex);
// 		}
	}

	size_t dwCnt = mapRivers.size();
	wdi.Write(&dwCnt, sizeof(dwCnt));

 	// visible and occupied
	map< CRiver*, vector<int> >::iterator end = mapRivers.end();
 	for ( map< CRiver*, vector<int> >::iterator iterMapRivers =  mapRivers.begin(); iterMapRivers != end; ++iterMapRivers )
 	{
 		CRiver * pRiver = iterMapRivers->first;
 
 		string strItemName = pRiver->GetItemName();
		int   nLen         = strItemName.length();
		wdi.Write(&nLen,			    sizeof(nLen));
		wdi.Write(strItemName.c_str(),  nLen);

 		DWORD dwTextureBaseIndex = pRiver->GetTextureBaseIndex();
 		DWORD dwTextureCnt = pRiver->GetTextureCount();
 
 		wdi.Write(&dwTextureBaseIndex,  sizeof(dwTextureBaseIndex));
 		wdi.Write(&dwTextureCnt,	    sizeof(dwTextureCnt));
 
		size_t dwGridCnt = iterMapRivers->second.size();
		wdi.Write(&dwGridCnt,			sizeof(dwGridCnt));

		for (size_t i = 0; i < iterMapRivers->second.size(); ++i  )
		{
			int nGridIndex = iterMapRivers->second[i];
			SGrid & gridW = pWater->GetGrid(nGridIndex);
			
			wdi.Write(&gridW.dwGridIndex,  sizeof(DWORD));
			wdi.Write(&gridW.vCenter.y,	   sizeof(float));

			for ( int j = 0; j < 4; ++j )
			{
				int nVertexIndex = gridW.dwVertexIndex[j];
				SVertex & vertex = pWater->GetVertex(nVertexIndex);

				wdi.Write(&vertex.vPosition.y, sizeof(float));
				wdi.Write(&vertex.oColor,	   sizeof(vertex.oColor));
			}
		}
 	}

	// unoccupied but visible
	dwCnt = setGridIndicesVisibleWater.size();
	wdi.Write(&dwCnt,			sizeof(dwCnt));
	for ( set<int>::iterator iter = setGridIndicesVisibleWater.begin(); iter != setGridIndicesVisibleWater.end(); ++iter )
	{
		int nGridIndex = *iter;
		SGrid & gridW = pWater->GetGrid(nGridIndex);
		wdi.Write(&gridW.dwGridIndex,  sizeof(DWORD));
		wdi.Write(&gridW.vCenter.y,	   sizeof(float));

		for ( int j = 0; j < 4; ++j )
		{
			int nVertexIndex = gridW.dwVertexIndex[j];
			SVertex & vertex = pWater->GetVertex(nVertexIndex);
			
			wdi.Write(&vertex.vPosition.y, sizeof(float));
			wdi.Write(&vertex.oColor,	   sizeof(vertex.oColor));
		}
	}

	///wave side
	SGrid* grids = CMapEditApp::GetInst()->GetWater()->GetGrids();
	uint fourcc = 'WVSD';
	wdi.Write(&fourcc,	   sizeof(uint));
	
	vector<bool> waveGrids;
	waveGrids.resize(dwGridCntInTileRegion);

// 	for (index_t i = 0; i < dwGridCntInTileRegion; ++i)
// 	{
// 		SGrid &grid  = pTerrain->GetGrid(pTileRegion->vGridIndices[i]);
// 		waveGrids[i] = grid.bSide;
// 	}
	wdi.Write(&waveGrids.front(), sizeof(bool)*dwGridCntInTileRegion);
	return true;
}

bool sqr::CGameTerWaterCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	CTerrainMesh *	pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	CWaterOperator* pWaterOperator = CMapEditApp::GetInst()->GetWaterOperator();
	CTerrainMesh *	pWater   = pTerrain->GetWater();
	DWORD			dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int				nGridIndex = 0;
	int				nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   * pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	
 	vector<int> vecGridIndicesRiver;
 
 	size_t dwRiverCnt = 0;
 	tData.SafeRead( &dwRiverCnt,	sizeof(dwRiverCnt) );
 
 	for ( size_t nRiverIndex = 0; nRiverIndex < dwRiverCnt; ++nRiverIndex )
 	{
  		string strItemName;
		int    nLen = 0;
		tData.SafeRead( &nLen,	         sizeof(nLen) );
		strItemName.resize(nLen);
		tData.SafeRead( &strItemName[0], nLen );

		DWORD dwTextureBaseIndex = 0;
		DWORD dwTextureCnt = 0;
		tData.SafeRead( &dwTextureBaseIndex, sizeof(dwTextureBaseIndex) );
		tData.SafeRead( &dwTextureCnt,		 sizeof(dwTextureCnt) );

 		if( strItemName.find("test") == -1 )
 		{
 			if( strItemName.find("环境映射水") == -1 )
 			{
 				if( strItemName.find("岩浆") == -1 )
 				{
 					string log = "";
 					log = "地图中存在水的纹理是用地表纹理刷的，刷的纹理图素包包名是：" + strItemName;
 					log = log + "\n是否需要将其替换成环境映射水，替换点YES，不替换NO";
 					if ( IDYES == MessageBox(NULL, log.c_str(), "提示", MB_YESNO) )
 						strItemName = "环境映射水";
 				}
 			}
 		}
 
		CMapEditApp::GetInst()->GetWaterOperator()->RebirthRiver(strItemName);
 		CRiver * pRiver = pWaterOperator->GetRiver();
 
 		size_t dwWaterCnt = 0;
		tData.SafeRead( &dwWaterCnt,		 sizeof(dwWaterCnt) );
 		for ( size_t nWaterIndex = 0; nWaterIndex < dwWaterCnt; ++nWaterIndex )
 		{
 			DWORD nGridIndex = 0;
 			tData.SafeRead( &nGridIndex,	 sizeof(nGridIndex) );
 
 			vecGridIndicesRiver.push_back(nGridIndex);
 
 			SGrid & gridW = pWater->GetGrid(nGridIndex);
 			gridW.pRiver = pRiver;

			tData.SafeRead( &gridW.vCenter.y,	 sizeof(float) );
 
 			for ( int j = 0; j < 4; ++j )
 			{
 				int nVertexIndex = gridW.dwVertexIndex[j];
 
 				SVertex & vertex = pWater->GetVertex(nVertexIndex);
 
				tData.SafeRead( &vertex.vPosition.y, sizeof(float) );
				tData.SafeRead( &vertex.oColor,	     sizeof(vertex.oColor) );
 			}
 		}
 	}
 
 	size_t dwUnoccupiedWaterCnt = 0;
 	tData.SafeRead( &dwUnoccupiedWaterCnt,	 sizeof(dwUnoccupiedWaterCnt) );
 	for ( size_t nWaterIndex = 0; nWaterIndex < dwUnoccupiedWaterCnt; ++nWaterIndex )
 	{
		DWORD nGridIndex = 0;
		tData.SafeRead( &nGridIndex,	 sizeof(nGridIndex) );
 
 		vecGridIndicesRiver.push_back(nGridIndex);
 
 		SGrid & gridW = pWater->GetGrid(nGridIndex);
 
 		tData.SafeRead( &gridW.vCenter.y,	 sizeof(float) );
 
 		for ( int j = 0; j < 4; ++j )
 		{
 			int nVertexIndex = gridW.dwVertexIndex[j];
 
 			SVertex & vertex = pWater->GetVertex(nVertexIndex);
 
			tData.SafeRead( &vertex.vPosition.y, sizeof(float) );
			tData.SafeRead( &vertex.oColor,	     sizeof(vertex.oColor) );
 		}
 	}
 
 	for (size_t i = 0; i < vecGridIndicesRiver.size(); ++i )
 	{
 		pWater->AdjustWaterMeshVisibility(vecGridIndicesRiver[i],true);
 	}

	///wave side
	uint fourcc;
	tData.SafeRead( &fourcc,	sizeof(fourcc) );
	if(fourcc == 'WVSD')
	{
		bool bSide = false;
		for (DWORD i = 0; i < dwGridCntInTileRegion; ++i)
		{
			tData.SafeRead( &bSide,	sizeof(bool) );
			SGrid &grid  = pTerrain->GetGrid(/*pTileRegion->vGridIndices[i]*/0);
			grid.bSide = bSide;
			if (!grid.bSide && NULL != grid.pWave)
			{
				index_t regionId = pTerrain->GetWater()->GetRegionIndexByGridIndex(grid.dwGridIndex);
				grid.pRiver->RemoveWave(regionId, grid.pWave);
				grid.pWave = NULL;
			}
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////

BEGIN_VERSION_MAP(CGameRegionCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CGameRegionCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	int			  nRegionIndex = *(static_cast<int*>(handle));
	int			  nSubRgnIndex = 0;
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	map<string, sqr::uint16> &sceneRgnIndexs = CLoadSavedMapMgr::GetInst()->sceneRgnIndexs;
	sqr::uint16 nSceneRgnIndex = 0;

	vector<int>	vRegionIndices = pTileRegion->vRegionIndices;
	for ( int n = 0; n < TILE_WIDTH_IN_REGION; ++n )
	{
		int nRgnIndex = vRegionIndices[n];
		SRegion &reg = pTerrain->GetRegion(nRgnIndex);
		for ( int m = 0; m < REGION_DIVIDE; ++m )
		{
			nSubRgnIndex = reg.divideRgnIndices[m];

			SDivideRegion *pDivideRegion = NULL;
			pDivideRegion = pTerrain->GetDivideRegion(nSubRgnIndex);
			if ( pDivideRegion )
			{
				///该区域对应的区域名字所对应的索引
				nSceneRgnIndex   = sceneRgnIndexs[pDivideRegion->m_strRegionName];
				CVector3f center = pDivideRegion->m_vCenter;
				bool	bNeedCal = pDivideRegion->bNeedCalRegion;

				wdi.Write(&nSceneRgnIndex,  sizeof(nSceneRgnIndex));
				wdi.Write(&center.x,		sizeof(float));
				wdi.Write(&center.z,		sizeof(float));
				wdi.Write(&bNeedCal,		sizeof(bNeedCal));
			}
		}
	}

	return true;

	return true;
}

bool sqr::CGameRegionCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	CVector3f center;
	bool	bNeedCal = false;
	sqr::uint16 nSceneRgnIndex = 0;

	for ( int n = 0; n < TILE_WIDTH_IN_REGION; ++n )
	{
		for ( int m = 0; m < REGION_DIVIDE; ++m )
		{
			///该区域对应的区域名字所对应的索引
			tData.SafeRead( &nSceneRgnIndex, sizeof(nSceneRgnIndex) );
			tData.SafeRead( &center.x,		 sizeof(float) );
			tData.SafeRead( &center.z,		 sizeof(float) );
			tData.SafeRead( &bNeedCal,		 sizeof(bNeedCal) );
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

CGameTerrainFormat::CGameTerrainFormat(void)
: CFileFormatMgr( 'TTFT', 0 )//tgt terrain format
{
	InitTerrainCode();
}

CGameTerrainFormat::~CGameTerrainFormat(void)
{ /*do nothing~~~*/ }

void CGameTerrainFormat::InitTerrainCode()
{
	RegisterCoder(new CGameTerVertexCoder);
	RegisterCoder(new CGameTerGridCoder);
	RegisterCoder(new CGameTerWaterCoder);
}

bool CGameTerrainFormat::LoadTerrainFile( const string& strFile, int nRegionIndex )
{
	return Load( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) );
}

///nRegionIndex为4*4大区域index
bool CGameTerrainFormat::SaveTerrainFile( const string& strFile, int nRegionIndex )
{
	Save( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) ); 

	return true;
}