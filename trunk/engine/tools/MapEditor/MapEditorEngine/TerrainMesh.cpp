#include "stdafx.h"
#include "TerrainMesh.h"
#include "GraphicBase.h"
#include "CGraphic.h"	
#include "MEMath.h"
#include "Render.h"
#include "Engine.h"
#include "CmdMgr.h"
#include "Operator.h"
#include "Operator.inl"
#include "WaterOperator.h"
#include "UserMsg.h"
#include "WireBox.h"
#include "River.h"
#include "CMapEditApp.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "CTerrainMgr.h"
#include "CTerrain.h"
#include "StringHelper.h"
#include "River.h"
#include "MathAlgorithm.h"
#include "CTexture.h"
#include "CVertexDeclaration.h"
#include "CRenderGroup.h"
#include "CWindowTarget.h"
#include "BaseHelper.h"
#include "DepathGroupModelInfo.h"
#include "IoSystem.h"
#include "CWindowTarget.h"
#include "Float.h"
#include "ToolSetTileTexture.h"
#include "ToolSetTileBlock.h"
#include "ToolSetTileHeight.h"
#include "ToolSetLogicHeight.h"
#include "TerrainAlgorithm.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "EditStateOpertiaonMgr.h"
#include "CRenderPipeline.h"
#include "LocalVersion.h"
#include "CEditTerrainMgr.h"
#include "CEditTerrainNode.h"

#include "CEditDataScene.h"
#include "CEditLogicDataMgr.h"
#include "CEditLogicData.h"

#define WATER_VISIBILITY_THRESHOLD (256.0f)
#define MAX_GRID_SIZE 1000000
const float divGRID_SPACE	 = 0.015625f; // 1.0 / 64.0
const float RangeWidth		 = 0.007825f; //1.0 / 128.0 = 0.007825f
const float mipMapBias		 = -0.5f;
//DWORD VERTEX_SPECULAR_COLOR = 0x202020;
///first sub region index ,second region index
map<int, int> m_RegIndexMap;

const uint CTerrainMesh::Tile_PreNum = eSRegionSpan / CTerrainMgr::MAP_SIZE;
const uint CTerrainMesh::Tile_PerSqr = CTerrainMesh::Tile_PreNum * CTerrainMesh::Tile_PreNum;

vector<CTerrainMesh::PX> CTerrainMesh::m_sortLight;

struct SCrossingPattern
{
	SCrossingPattern::SCrossingPattern()
	{
		bHigherThanGround[0] = bHigherThanGround[1] = bHigherThanGround[2] = bHigherThanGround[3] = false;
		fWeight[0] = fWeight[1] = fWeight[2] = fWeight[3] = 0.0f;
	}

	bool bHigherThanGround[4];
	float fWeight[4];
};
// -----------------------------------------------------------------------------

CTerrainMesh::CTerrainMesh()
{
	m_strTextureNameDefault = "";

	m_bWater			= false;
	m_pWater			= NULL;
	m_fHeightDeltaUp	= 0.0f;
	m_fHeightDeltaDown	= 0.0f;

	m_pRegions			= NULL;
	m_pVertices			= NULL;
	m_fMaxGridHeight	= 0.0f;

	m_strMapName		= "";

	m_pTerrain			= NULL;
	m_pTerrainTexture	= NULL;

	pLightSystem		= NULL;
	m_bDiffMapMark		= false;

	m_bLoadSmallMap		= false;
	m_bUseVertexBufferRenderBlock = false;

	m_uReSizeMapType	= -1;
}

CTerrainMesh::~CTerrainMesh()
{
	SAFE_DELETE_ARRAY(m_pVertices);
	SAFE_DELETE_ARRAY(m_pGrids);
	SAFE_DELETE_ARRAY(m_pRegions);

	SafeRelease(m_pTerrain);
	SafeRelease(m_pTerrainTexture);

	this->m_DepathModelMap.clear();
	map<int, SDivideRegion*>::iterator beg = m_pDivideRegions.begin();
	map<int, SDivideRegion*>::iterator end = m_pDivideRegions.end();
	for ( beg; beg != end; ++beg )
	{
		SafeDelete(beg->second);
	}
	m_pDivideRegions.clear();

	size_t size = m_vecTileRegionList.size();
	for ( size_t i = 0; i < size; ++i )
	{
		SafeDelete(m_vecTileRegionList[i]);
	}
	m_vecTileRegionList.clear();

	m_TerrainBufferIndexs.clear();
	_RegionAjenctList.clear();
	m_DynamicLightGrids.clear();
	m_VisibleDynamicLightGrids.clear();
	m_MainRoadWeight.clear();
	m_SubGridIndexs.clear();
	_RegionAjenctList.clear();
}

void CTerrainMesh::ReLoadLightMap(const bool bLoadLightMap)
{
	SQR_TRY
	{
		if( bLoadLightMap )
		{
			for ( UINT i = 0; i < m_dwDepthInTileRegion; ++i)
			{
				for (UINT j = 0; j < m_dwWidthInTileRegion; ++j )
				{
					string path = format("lit:%d_%d.dds",j,i);
					path = IoSystem::GetInst()->GetFullPath(path);
					// 先释放掉老的
					URegionID index = URegionID(j,i);

					ITexture* texture = ((TerrainNode*)m_pTerrain->GetNode(index))->text;
					if (NULL != texture)
					{
						Ast(1 == texture->GetRefCount());

						//二次释放
						ITexture* textmp=texture;
						textmp=NULL;
						texture->Release();
						//delete texture;  删除这一句
					}
					m_pTerrain->SetLightMap(index ,path.c_str());
				}
			}
		}

		m_pTerrain->TurnOnLightMap(bLoadLightMap);
	}
	SQR_CATCH(exp)
	{
		string msg = exp.ErrorMsg();
		MessageBox(NULL, "使用lightmap出错", "错误", MB_OK);

		return;
	}
	SQR_TRY_END;

}

void CTerrainMesh::LoadTerrainTexture()
{
	IoSystem* ioSystem = IoSystem::GetInst();
	string homePath = IoSystem::GetInst()->GetFullPath("home:");
	string selfTexPath = homePath + this->m_strMapName + ".jpg";

	m_pTerrain->LoadTexture(selfTexPath.c_str());
};

void CTerrainMesh::InitTerrain(CEditTerrainMgr* pTerrain)
{
	if ( !m_bWater )
	{
		CRenderDevice* pRender = CRenderDevice::GetInst();

		m_pTerrain = pTerrain;
		m_pTerrain->AddRef();
		//m_pTerrain->CreateTerrainMeshBuffer(m_dwWidthInTileRegion,m_dwDepthInTileRegion,TerrainFlag::NormalSpecular);
		this->LoadTerrainTexture();

		vector<sqr::vTerrain::Stream0>  Buf0;// buf0: pos, normal, diffuse(rgb为烘培顶点色，a为1，2层纹理混合alpha值)
		vector<sqr::vTerrain::Color>    Buf1;// buf1: vertex specular color顶点高光色
		vector<sqr::vTerrain::Color>    Buf2;// buf2: vertex original color顶点原始色（包括刷的顶点色）
		vector<sqr::vTerrain::StreamUV> Buf3;// buf3: layer0 texture uv address一层纹理在大纹理中寻址的坐标
		vector<sqr::vTerrain::StreamUV> Buf4;// buf3: layer1 texture uv address二层纹理在大纹理中寻址的坐标
		vector<sqr::vTerrain::StreamUV> Buf5;// buf4: cliff texture coordinate悬崖贴图坐标
		int dpTileVertexCount = CTerrainMgr::MAP_SIZE_SQR * 4;
		Buf0.resize(dpTileVertexCount);
		Buf1.resize(dpTileVertexCount);
		Buf2.resize(dpTileVertexCount);
		Buf3.resize(dpTileVertexCount);
		Buf4.resize(dpTileVertexCount);
		Buf5.resize(dpTileVertexCount);

		const float MagicL	= 128.5f/2304.0f;
		const float Magic	= 0.0f;
		const float MagicA	= Magic/2;
		string		texname	  = "";
		float		minH	= 9999.0f, maxH = -9999.0f;	// 顶点高度范围

		/*
		2---3 6---7
		|	| |	  |
		0---1 4---5
		*/
		Ast(m_vecTileRegionList.size() == m_dwTileRegionCnt);
		CVertexDeclaration* pVD = CTerrainMgr::st_TerrainDecl[TerrainFlag::NormalSpecular];

		CEditLogicDataMgr* pEditLogicDataMgr = CMapEditApp::GetInst()->GetDataScene()->GetLogicDataMgr();
		assert(pEditLogicDataMgr!=NULL);

		for ( DWORD bufferIndex = 0; bufferIndex < m_dwTileRegionCnt; ++bufferIndex )
		{
			int			j							= bufferIndex % m_dwWidthInTileRegion;
			int			i							= bufferIndex / m_dwWidthInTileRegion;
			STileRegion *pTileRegion				= m_vecTileRegionList[bufferIndex];
			int			nTileGridIndex				= 0;

			URegionID regionID = URegionID(j,i);
			CEditTerrainNode *pNode					= (CEditTerrainNode*)(m_pTerrain->GetNode(regionID));
			CEditLogicData* pLogicData				= (CEditLogicData*)pEditLogicDataMgr->GetNode(regionID);
			assert(pNode!=NULL);
			assert(pLogicData!=NULL);

			pNode->text=m_pTerrainTexture;

			pNode->m_TerrainTiles.resize(CTerrainMesh::Tile_PerSqr);
			pNode->m_TerrainTileVisibles.resize(CTerrainMesh::Tile_PerSqr);
				
			for ( uint m = 0; m < CTerrainMesh::Tile_PerSqr; ++m )
			{
				float wi = j*64.0f + (m % CTerrainMesh::Tile_PreNum) * CTerrainMgr::MAP_SIZE;
				float hi = i*64.0f + (m / CTerrainMesh::Tile_PreNum) * CTerrainMgr::MAP_SIZE;

				pNode->m_TerrainTileVisibles[m] = true;

				pNode->m_TerrainTiles[m].op.Create();
				pNode->m_TerrainTiles[m].op.vertexData->vertexDeclaration = pVD;

				size_t VerSize = 0;
				pNode->m_TerrainTiles[m].op.vertexData->vertexBufferBinding->DestroyAllBuffer();
				for ( int jj = 0; jj < 6; ++jj )
				{
					VerSize = pVD->GetVertexSize(jj);
					if (VerSize!=0)
					{
						pNode->m_TerrainTiles[m].op.vertexData->vertexBufferBinding->SetBinding(jj,
							CHardwareBufferMgr::GetInst()->createVertexBuffer(
							VerSize,
							CTerrainMgr::MAP_SIZE_SQR*4,
							HBU_DYNAMIC,NULL,false,true));
					}
				}		

				STileRenderRegionUnit *pRenderRegionUnit = pTileRegion->vRenderRegionUnits[m];
				UINT idx = 0;		//每一个TileRegion（4*4Region） 中切分的每个dp buffer 的idx

				int nGridIndex = 0, GridIndex = 0; // 格子索引
				uint size = CTerrainMgr::MAP_SIZE*CTerrainMgr::MAP_SIZE;
				for ( uint n = 0; n < size; ++n, ++GridIndex )
				{
					idx = GridIndex * 4;

					nGridIndex = pRenderRegionUnit->vGridIndices[n];
					SGrid &grid = this->GetGrid(nGridIndex);
					grid.dwBufferIndex = bufferIndex;

					texname = grid.nodeInfo.strTextureNames[0];
					CVector2f uv0;
					if ( texname == "" )
						uv0 = CVector2f(0.0f, 0.0f);
					else
						uv0 = m_pTerrain->getUV(pRender->m_TerrainTexIndexs[texname]);

					texname = grid.nodeInfo.strTextureNames[1];
					CVector2f uv1;
					if ( texname == "" )
						uv1 = CVector2f(0.0f, 0.0f);
					else
						uv1 = m_pTerrain->getUV(pRender->m_TerrainTexIndexs[texname]);

					SVertex& vertex0 = this->GetVertex(grid.dwVertexIndex[0]);
					SVertex& vertex1 = this->GetVertex(grid.dwVertexIndex[1]);
					SVertex& vertex2 = this->GetVertex(grid.dwVertexIndex[2]);
					SVertex& vertex3 = this->GetVertex(grid.dwVertexIndex[3]);
					bool tf = fabs(grid.uv[0][0].x - grid.uv[0][1].x) > fabs(grid.uv[0][0].x - grid.uv[0][3].x);

					//////////////////////////////////////////////////////////////////////////
					/// logic data
					pLogicData->SetLogicHeight(nGridIndex, (int)vertex0.fLogicHeight);
					pLogicData->SetVertexPosition(nGridIndex, vertex0.vPosition);
					pLogicData->SetLogicNormal(nGridIndex, vertex0.vNormal);
					pLogicData->SetVertexDiffuse(nGridIndex, vertex0.oColor);
					pLogicData->SetVertexBakingColor(nGridIndex, vertex0.bColor);

					++nTileGridIndex;
					//////////////////////////////////////////////////////////////////////////

					if (grid.bGridHide)
					{
						Buf0[idx].p	  = CVector3f(0.0f,0.0f,0.0f);
						Buf0[idx+1].p = CVector3f(0.0f,0.0f,0.0f);
						Buf0[idx+2].p = CVector3f(0.0f,0.0f,0.0f);
						Buf0[idx+3].p = CVector3f(0.0f,0.0f,0.0f);
					}
					else
					{
						Buf0[idx].p   = vertex0.vPosition;
						Buf0[idx+1].p = vertex3.vPosition;
						Buf0[idx+2].p = vertex1.vPosition;
						Buf0[idx+3].p = vertex2.vPosition;
					}

					// vertex1
					Buf0[idx].np = vertex0.vNormal;
					Buf0[idx].color  = vertex0.bColor;

					Buf1[idx].color = vertex0.sColor;
					Buf2[idx].color = vertex0.oColor;

					Buf3[idx].tu = uv0.x + MagicL * ( tf ? grid.uv[1][0].x : grid.uv[1][0].y);
					Buf3[idx].tv = uv0.y + MagicL * ( tf ? grid.uv[1][0].y : grid.uv[1][0].x);
					
					Buf4[idx].tu = uv1.x + MagicL * grid.uv[1][0].x;
					Buf4[idx].tv = uv1.y + MagicL * grid.uv[1][0].y;

					maxH = max(maxH,Buf0[idx].p.y);
					minH = min(minH,Buf0[idx].p.y);

					// vertex3
					Buf0[idx+1].np = vertex3.vNormal;
					Buf0[idx+1].color  = vertex3.bColor;

					Buf1[idx+1].color = vertex3.sColor;
					Buf2[idx+1].color = vertex3.oColor;

					Buf3[idx+1].tu = uv0.x + MagicL * ( tf ? grid.uv[1][3].x : grid.uv[1][3].y );
					Buf3[idx+1].tv = uv0.y + MagicL * ( tf ? grid.uv[1][3].y : grid.uv[1][3].x );
					
					Buf4[idx+1].tu = uv1.x + MagicL * grid.uv[1][3].x;
					Buf4[idx+1].tv = uv1.y + MagicL * grid.uv[1][3].y;

					Buf5[idx+1].tu = grid.uv[0][3].x;
					Buf5[idx+1].tv = grid.uv[0][3].y;

					// vertex1
					Buf0[idx+2].np = vertex1.vNormal;
					Buf0[idx+2].color  = vertex1.bColor;

					Buf1[idx+2].color = vertex1.sColor;
					Buf2[idx+2].color = vertex1.oColor;

					Buf3[idx+2].tu = uv0.x + MagicL * ( tf ? grid.uv[1][1].x : grid.uv[1][1].y );
					Buf3[idx+2].tv = uv0.y + MagicL * ( tf ? grid.uv[1][1].y : grid.uv[1][1].x );
				
					Buf4[idx+2].tu = uv1.x + MagicL * grid.uv[1][1].x;
					Buf4[idx+2].tv = uv1.y + MagicL * grid.uv[1][1].y;

					Buf5[idx+2].tu = grid.uv[0][1].x;
					Buf5[idx+2].tv = grid.uv[0][1].y;

					// vertex2
					Buf0[idx+3].np = vertex2.vNormal;
					Buf0[idx+3].color  = vertex2.bColor;

					Buf1[idx+3].color = vertex2.sColor;
					Buf2[idx+3].color = vertex2.oColor;

					Buf3[idx+3].tu = uv0.x + MagicL*(tf ? grid.uv[1][2].x : grid.uv[1][2].y);
					Buf3[idx+3].tv = uv0.y + MagicL*(tf ? grid.uv[1][2].y : grid.uv[1][2].x);
					
					Buf4[idx+3].tu = uv1.x + MagicL * grid.uv[1][2].x;
					Buf4[idx+3].tv = uv1.y + MagicL * grid.uv[1][2].y;

					Buf5[idx+3].tu = grid.uv[0][2].x;
					Buf5[idx+3].tv = grid.uv[0][2].y;
				}

				// write buffer
				if (minH == 9999.0f&&maxH == -9999.0f)
				{
					minH = 0.0f;
					maxH = 0.0f;
				}
				
				CVector3f min = CVector3f(j*4096.0f,minH,i*4096.0f);
				CVector3f max = CVector3f((j+1)*4096.0f,maxH,(i+1)*4096.0f);
				

				pNode->m_TerrainTiles[m].op.indexData->indexStart = 0;
				pNode->m_TerrainTiles[m].op.indexData->indexCount = CTerrainMgr::st_pIndex->getNumIndexes();
				pNode->m_TerrainTiles[m].op.indexData->indexBuffer = CTerrainMgr::st_pIndex;
				pNode->m_TerrainTiles[m].op.useIndexes = true;
				pNode->m_TerrainTiles[m].op.vertexData->vertexStart = 0;
				pNode->m_TerrainTiles[m].op.vertexData->vertexCount = CTerrainMgr::MAP_SIZE_SQR*4;
				CVertexBufferBinding*	pVBB = pNode->m_TerrainTiles[m].op.vertexData->vertexBufferBinding;

				pNode->m_TerrainTiles[m].aab.setMaximum( (wi+CTerrainMgr::MAP_SIZE)*64.0f, maxH,	(hi+CTerrainMgr::MAP_SIZE)*64.0f );
				pNode->m_TerrainTiles[m].aab.setMinimum(  wi*64.0f,						   minH,	 hi*64.0f  );
				pNode->m_TAAB.mergeBox(pNode->m_TerrainTiles[m].aab);

				CVertexHardwareBuffer* pBuf0 = pVBB->GetBuffer(0);
				pBuf0->write(0, pBuf0->getSize(), &(Buf0[0]), true);

				CVertexHardwareBuffer* pBuf1 = pVBB->GetBuffer(1);
				pBuf1->write(0, pBuf1->getSize(), &(Buf1[0]), true);

				CVertexHardwareBuffer* pBuf2 = pVBB->GetBuffer(2);
				pBuf2->write(0, pBuf2->getSize(), &(Buf2[0]), true);

				CVertexHardwareBuffer* pBuf3 = pVBB->GetBuffer(3);
				pBuf3->write(0, pBuf3->getSize(), &(Buf3[0]), true);

				CVertexHardwareBuffer* pBuf4 = pVBB->GetBuffer(4);
				pBuf4->write(0, pBuf4->getSize(), &(Buf4[0]), true);

				CVertexHardwareBuffer* pBuf5 = pVBB->GetBuffer(5);
				pBuf5->write(0, pBuf5->getSize(), &(Buf5[0]), true);
			}
			
			pNode->SetValid(true);
		}

		m_pTerrain->SetRenderFlag(TerrainFlag::NormalSpecular);
		Buf0.clear();
		Buf1.clear();
		Buf2.clear();
		Buf3.clear();
		Buf4.clear();
		Buf5.clear();
	}
}

void CTerrainMesh::SetTerrainNodeVisbileMark(const int nodeIndex, const int nodeSubIndex, const bool bMark)
{
	int			j = nodeIndex % m_dwWidthInTileRegion;
	int			i = nodeIndex / m_dwWidthInTileRegion;
	CEditTerrainNode *pNode = (CEditTerrainNode*)(m_pTerrain->GetNode(URegionID(j,i)));
 	if( pNode )
 		pNode->m_TerrainTileVisibles[nodeSubIndex] = bMark;
	else
	{
		string log = format("是一个无效节点索引:%d, 请转告给相关程序人员", nodeIndex);
		MessageBox(NULL, log.c_str(), "提示", MB_OK);
	}
}

void CTerrainMesh::GetTerrainNodeVisbileMark(const int nodeIndex, vector<bool>& visibles)
{
	int			j = nodeIndex % m_dwWidthInTileRegion;
	int			i = nodeIndex / m_dwWidthInTileRegion;
	CEditTerrainNode *pNode = (CEditTerrainNode*)(m_pTerrain->GetNode(URegionID(j,i)));
	if( pNode )
		visibles = pNode->m_TerrainTileVisibles;
	else
	{
		string log = format("是一个无效节点索引:%d, 请转告给相关程序人员", nodeIndex);
		MessageBox(NULL, log.c_str(), "提示", MB_OK);
	}
}

///streamIndex = 8表示处理地表阻挡
// streamIndex 0:buf0,buf2, 1:buf1, 2:buf3, 3:buf4
void CTerrainMesh::ReWriteTerrainBuffer( set<DWORD>& bufferIndexs, int streamIndex, const DWORD color )
{
	if( m_bWater )
		return;

	COperator * pOperator	= CMapEditApp::GetInst()->GetOperator();
	bool bUseSpecular		= pOperator->GetIsUseSpecular();
	if( !bUseSpecular && streamIndex == 1 )
		return;

	bool bHideVertexColor	 = CEditStateOpertiaonMgr::GetInst()->GetHideVertexColor();
	bool bSetRegionTool		 = CEditStateOpertiaonMgr::GetInst()->GetEditState() == CEditStateOpertiaonMgr::EES_SET_TILE_REGIOON;
	bool bHideLayerOne		 = CEditStateOpertiaonMgr::GetInst()->GetHideLayerOneTexture();
	bool bHideLayerTwo		 = CEditStateOpertiaonMgr::GetInst()->GetHideLayerTwoTexture();
	bool bShowSpecialMatGrid = pOperator->GetShowSpecialGridCueMat();
	vector<DWORD> errorLogicVertexs = CToolSetLogicHeight::GetInst()->GetIsCheckMapLogicHeight();
	CVector2f uv0, uv1; // 从大纹理中取的纹理坐标
	CVector2f griduv00, griduv01, griduv02, griduv03; // 第1层格子纹理坐标
	CVector2f griduv10, griduv11, griduv12, griduv13; // 第2层格子纹理坐标

	CRenderDevice * pRender = CRenderDevice::GetInst();

	vector<sqr::vTerrain::Stream0>  Buf0;// buf0: pos, normal, diffuse(rgb为烘培顶点色，a为1，2层纹理混合alpha值)
	vector<sqr::vTerrain::Color>    Buf1;// buf1: vertex specular color顶点高光色
	vector<sqr::vTerrain::Color>    Buf2;// buf2: vertex original color顶点原始色（包括刷的顶点色）
	vector<sqr::vTerrain::StreamUV> Buf3;// buf3: layer0 texture uv address一层纹理在大纹理中寻址的坐标
	vector<sqr::vTerrain::StreamUV> Buf4;// buf3: layer1 texture uv address二层纹理在大纹理中寻址的坐标
	vector<sqr::vTerrain::StreamUV> Buf5;// buf4: cliff texture coordinate悬崖贴图坐标
	int dpTileVertexCount = CTerrainMgr::MAP_SIZE_SQR * 4;
	Buf0.resize(dpTileVertexCount);
	Buf1.resize(dpTileVertexCount);
	Buf2.resize(dpTileVertexCount);
	Buf3.resize(dpTileVertexCount);
	Buf4.resize(dpTileVertexCount);
	Buf5.resize(dpTileVertexCount);

	const float MagicL	= 128.5f/2304.0f;
	const float Magic	= 0.0f;//.5f/2304.0f;
	const float MagicA	= Magic/2;//*2;//0.00038f;
	DWORD bufferIndex	= 0;
	string texname		= "";

	/*
	2---3 6---7
	|	| |	  |
	0---1 4---5
	*/

	set<DWORD>::iterator iter;
	set<DWORD>::iterator eiter = bufferIndexs.end();
	for ( iter  = bufferIndexs.begin(); iter != eiter; ++iter )
	{
		bufferIndex = *iter;
		int			j = bufferIndex % m_dwWidthInTileRegion;
		int			i = bufferIndex / m_dwWidthInTileRegion;
		UINT		idx		= 0;		// 每 4*4Region 即一个buffer里的顶点的索引
		STileRegion *pTileRegion = m_vecTileRegionList[bufferIndex];

		int GridIndex = 0; // 格子索引
		for ( uint m = 0; m < CTerrainMesh::Tile_PerSqr; ++m )
		{
			STileRenderRegionUnit *pRenderRegionUnit = pTileRegion->vRenderRegionUnits[m];
			UINT idx = 0;		//每一个TileRegion（4*4Region） 中切分的每个dp buffer 的idx

			int nGridIndex = 0, GridIndex = 0; // 格子索引
			uint size = CTerrainMgr::MAP_SIZE*CTerrainMgr::MAP_SIZE;
			for ( uint n = 0; n < size; ++n, ++GridIndex )
			{
				idx			= GridIndex * 4;
				nGridIndex  = pRenderRegionUnit->vGridIndices[n];
				SGrid &grid = this->GetGrid(nGridIndex);

				texname = grid.nodeInfo.strTextureNames[0];

				if ( texname == "" || bHideLayerOne )
					uv0 = CVector2f(0.0f, 0.0f);
				else
					uv0 = m_pTerrain->getUV(pRender->m_TerrainTexIndexs[texname]);

				texname = grid.nodeInfo.strTextureNames[1];
				if ( texname == "" || bHideLayerTwo )
					uv1 = CVector2f(0.0f, 0.0f);
				else
					uv1 = m_pTerrain->getUV(pRender->m_TerrainTexIndexs[texname]);


				if ( bHideLayerTwo )
				{
					griduv10 = CVector2f(0.0f, 0.0f);
					griduv11 = CVector2f(0.0f, 0.0f);
					griduv12 = CVector2f(0.0f, 0.0f);
					griduv13 = CVector2f(0.0f, 0.0f);
				}
				else
				{
					griduv10 = grid.uv[1][0];
					griduv11 = grid.uv[1][1];
					griduv12 = grid.uv[1][2];
					griduv13 = grid.uv[1][3];
				}

				if (grid.bGridHide)
				{
					switch (streamIndex)
					{
					case 0:
						{
							Buf0[idx].p = CVector3f(0.0f,0.0f,0.0f);
							Buf0[idx].np = CVector3f(0.0f,1.0f,0.0f);
							Buf0[idx].color  = 0x00000000;

							Buf0[idx+1].p =  CVector3f(0.0f,0.0f,0.0f);
							Buf0[idx+1].np = CVector3f(0.0f,1.0f,0.0f);
							Buf0[idx+1].color  = 0x00000000;

							Buf0[idx+2].p = CVector3f(0.0f,0.0f,0.0f);
							Buf0[idx+2].np = CVector3f(0.0f,1.0f,0.0f);
							Buf0[idx+2].color  = 0x00000000;

							Buf0[idx+3].p = CVector3f(0.0f,0.0f,0.0f);
							Buf0[idx+3].np = CVector3f(0.0f,1.0f,0.0f);
							Buf0[idx+3].color  = 0x00000000;

							break;
						}
					default:
						break;
					}
				}
				else
				{
					switch (streamIndex)
					{
					case 0:
						{
							SVertex vertex = this->GetVertex(grid.dwVertexIndex[0]);
							bool tf = fabs(grid.uv[0][0].x - grid.uv[0][1].x) > fabs(grid.uv[0][3].x - grid.uv[0][2].x);

							Buf0[idx].p = vertex.vPosition;
							Buf0[idx].np = vertex.vNormal;
							Buf0[idx].color  = vertex.bColor;

							vertex = this->GetVertex(grid.dwVertexIndex[3]);
							Buf0[idx+1].p = vertex.vPosition;
							Buf0[idx+1].np = vertex.vNormal;
							Buf0[idx+1].color  = vertex.bColor;

							vertex = this->GetVertex(grid.dwVertexIndex[1]);
							Buf0[idx+2].p = vertex.vPosition;
							Buf0[idx+2].np = vertex.vNormal;
							Buf0[idx+2].color  = vertex.bColor;

							vertex = this->GetVertex(grid.dwVertexIndex[2]);
							Buf0[idx+3].p = vertex.vPosition;
							Buf0[idx+3].np = vertex.vNormal;
							Buf0[idx+3].color  = vertex.bColor;

							break;
						}
					case 1:
						{
							SVertex vertex = this->GetVertex(grid.dwVertexIndex[0]);
							Buf1[idx].color = vertex.sColor;

							vertex = this->GetVertex(grid.dwVertexIndex[3]);
							Buf1[idx+1].color = vertex.sColor;

							vertex = this->GetVertex(grid.dwVertexIndex[1]);
							Buf1[idx+2].color = vertex.sColor;

							vertex = this->GetVertex(grid.dwVertexIndex[2]);
							Buf1[idx+3].color = vertex.sColor;

							break;
						}
					case 2:
						{
							SVertex vertex = this->GetVertex(grid.dwVertexIndex[0]);
							Buf2[idx].color = vertex.oColor;

							vertex = this->GetVertex(grid.dwVertexIndex[3]);
							Buf2[idx+1].color = vertex.oColor;

							vertex = this->GetVertex(grid.dwVertexIndex[1]);
							Buf2[idx+2].color = vertex.oColor;

							vertex = this->GetVertex(grid.dwVertexIndex[2]);
							Buf2[idx+3].color = vertex.oColor;

							if ( bSetRegionTool && grid.bInHideRegion )
							{
								Buf2[idx].color   = Buf2[idx].color   & 0xff000000 | color;
								Buf2[idx+1].color = Buf2[idx+1].color & 0xff000000 | color;
								Buf2[idx+2].color = Buf2[idx+2].color & 0xff000000 | color;
								Buf2[idx+3].color = Buf2[idx+3].color & 0xff000000 | color;
							}
							else if ( bHideVertexColor )
							{
								Buf2[idx].color   = Buf2[idx].color   & 0xff000000 | 0x00808080;
								Buf2[idx+1].color = Buf2[idx+1].color & 0xff000000 | 0x00808080;
								Buf2[idx+2].color = Buf2[idx+2].color & 0xff000000 | 0x00808080;;
								Buf2[idx+3].color = Buf2[idx+3].color & 0xff000000 | 0x00808080;
							}
							else if( grid.bMatNotEffectByTileSets && bShowSpecialMatGrid )
							{
								Buf2[idx].color   = Buf2[idx].color   & 0xff000000 | 0x0000ff;
								Buf2[idx+1].color = Buf2[idx+1].color & 0xff000000 | 0x0000ff;
								Buf2[idx+2].color = Buf2[idx+2].color & 0xff000000 | 0x0000ff;
								Buf2[idx+3].color = Buf2[idx+3].color & 0xff000000 | 0x0000ff;
							}

							break;
						}
					case 3:
						{
							SVertex vertex = this->GetVertex(grid.dwVertexIndex[0]);
							bool tf = fabs(grid.uv[0][0].x - grid.uv[0][1].x) > fabs(grid.uv[0][0].x - grid.uv[0][3].x);

							if ( bHideLayerOne )
							{
								Buf3[idx].tu = 0.0f;
								Buf3[idx].tv = 0.0f;

								Buf3[idx+1].tu = 0.0f;
								Buf3[idx+1].tv = 0.0f;

								Buf3[idx+2].tu = 0.0f;
								Buf3[idx+2].tv = 0.0f;

								Buf3[idx+3].tu = 0.0f;
								Buf3[idx+3].tv = 0.0f;
							}
							else
							{
								Buf3[idx].tu = uv0.x + MagicL * ( tf ? griduv10.x : griduv10.y);
								Buf3[idx].tv = uv0.y + MagicL * ( tf ? griduv10.y : griduv10.x);

								Buf3[idx+1].tu = uv0.x + MagicL * ( tf ? griduv13.x : griduv13.y );
								Buf3[idx+1].tv = uv0.y + MagicL * ( tf ? griduv13.y : griduv13.x );

								Buf3[idx+2].tu = uv0.x + MagicL * ( tf ? griduv11.x : griduv11.y );
								Buf3[idx+2].tv = uv0.y + MagicL * ( tf ? griduv11.y : griduv11.x );

								Buf3[idx+3].tu = uv0.x + MagicL*(tf ? griduv12.x : griduv12.y);
								Buf3[idx+3].tv = uv0.y + MagicL*(tf ? griduv12.y : griduv12.x);
							}


							break;
						}
					case 4:
						{
							if ( bHideLayerTwo )
							{
								Buf4[idx].tu = 0.0f;
								Buf4[idx].tv = 0.0f;

								Buf4[idx+1].tu = 0.0f;
								Buf4[idx+1].tv = 0.0f;

								Buf4[idx+2].tu = 0.0f;
								Buf4[idx+2].tv = 0.0f;

								Buf4[idx+3].tu = 0.0f;
								Buf4[idx+3].tv = 0.0f;
							}
							else
							{
								Buf4[idx].tu = uv1.x + MagicL * griduv10.x;
								Buf4[idx].tv = uv1.y + MagicL * griduv10.y;
								Buf4[idx+1].tu = uv1.x + MagicL * griduv13.x;
								Buf4[idx+1].tv = uv1.y + MagicL * griduv13.y;
								Buf4[idx+2].tu = uv1.x + MagicL * griduv11.x;
								Buf4[idx+2].tv = uv1.y + MagicL * griduv11.y;
								Buf4[idx+3].tu = uv1.x + MagicL * griduv12.x;
								Buf4[idx+3].tv = uv1.y + MagicL * griduv12.y;
							}

							break;
						}
					case 5:
						{
							Buf5[idx].tu = griduv00.x;///悬崖纹理坐标
							Buf5[idx].tv = griduv00.y;

							Buf5[idx+1].tu = griduv03.x;
							Buf5[idx+1].tv = griduv03.y;

							Buf5[idx+2].tu = griduv01.x;
							Buf5[idx+2].tv = griduv01.y;

							Buf5[idx+3].tu = griduv02.x;
							Buf5[idx+3].tv = griduv02.y;

							break;
						}
					case 6:
						{
							Buf3[idx].tu = uv0.x;
							Buf3[idx].tv = uv0.y;

							Buf3[idx+1].tu = uv0.x;
							Buf3[idx+1].tv = uv0.y;

							Buf3[idx+2].tu = uv0.x;
							Buf3[idx+2].tv = uv0.y;

							Buf3[idx+3].tu = uv0.x;
							Buf3[idx+3].tv = uv0.y;

							Buf5[idx].tu = griduv00.x;///悬崖纹理坐标
							Buf5[idx].tv = griduv00.y;

							Buf5[idx+1].tu = griduv03.x;
							Buf5[idx+1].tv = griduv03.y;

							Buf5[idx+2].tu = griduv01.x;
							Buf5[idx+2].tv = griduv01.y;

							Buf5[idx+3].tu = griduv02.x;
							Buf5[idx+3].tv = griduv02.y;

							break;
						}
					case 7:
						{
							SVertex vertex = this->GetVertex(grid.dwVertexIndex[0]);
							bool tf = fabs(grid.uv[0][0].x - grid.uv[0][1].x) > fabs(grid.uv[0][0].x - grid.uv[0][3].x);

							Buf2[idx].color = vertex.oColor;

							vertex = this->GetVertex(grid.dwVertexIndex[3]);
							Buf2[idx+1].color = vertex.oColor;

							vertex = this->GetVertex(grid.dwVertexIndex[1]);
							Buf2[idx+2].color = vertex.oColor;

							vertex = this->GetVertex(grid.dwVertexIndex[2]);
							Buf2[idx+3].color = vertex.oColor;
							Buf3[idx].tu = uv0.x + MagicL * ( tf ? griduv10.x : griduv10.y);
							Buf3[idx].tv = uv0.y + MagicL * ( tf ? griduv10.y : griduv10.x);

							Buf3[idx+1].tu = uv0.x + MagicL * ( tf ? griduv13.x : griduv13.y );
							Buf3[idx+1].tv = uv0.y + MagicL * ( tf ? griduv13.y : griduv13.x );

							Buf3[idx+2].tu = uv0.x + MagicL * ( tf ? griduv11.x : griduv11.y );
							Buf3[idx+2].tv = uv0.y + MagicL * ( tf ? griduv11.y : griduv11.x );

							Buf3[idx+3].tu = uv0.x + MagicL*(tf ? griduv12.x : griduv12.y);
							Buf3[idx+3].tv = uv0.y + MagicL*(tf ? griduv12.y : griduv12.x);

							Buf4[idx].tu   = uv1.x + MagicL * griduv10.x;
							Buf4[idx].tv   = uv1.y + MagicL * griduv10.y;

							Buf4[idx+1].tu = uv1.x + MagicL * griduv13.x;
							Buf4[idx+1].tv = uv1.y + MagicL * griduv13.y;

							Buf4[idx+2].tu = uv1.x + MagicL * griduv11.x;
							Buf4[idx+2].tv = uv1.y + MagicL * griduv11.y;

							Buf4[idx+3].tu = uv1.x + MagicL * griduv12.x;
							Buf4[idx+3].tv = uv1.y + MagicL * griduv12.y;

							break;
						}
					case 9:
						{
							SVertex vertex = this->GetVertex(grid.dwVertexIndex[0]);
							Buf2[idx].color = vertex.oColor;

							vertex = this->GetVertex(grid.dwVertexIndex[3]);
							Buf2[idx+1].color = vertex.oColor;

							vertex = this->GetVertex(grid.dwVertexIndex[1]);
							Buf2[idx+2].color = vertex.oColor;

							vertex = this->GetVertex(grid.dwVertexIndex[2]);
							Buf2[idx+3].color = vertex.oColor;

							int nBlockType = grid.nBlockType;
							if ( nBlockType == 3 ) // high
							{
								Buf2[idx].color   = Buf2[idx].color   & 0xff000000 | 0x00FD7171;
								Buf2[idx+1].color = Buf2[idx+1].color & 0xff000000 | 0x00FD7171;
								Buf2[idx+2].color = Buf2[idx+2].color & 0xff000000 | 0x00FD7171;
								Buf2[idx+3].color = Buf2[idx+3].color & 0xff000000 | 0x00FD7171;
							}
							else if ( nBlockType == 2 ) // mid
							{
								Buf2[idx].color   = Buf2[idx].color   & 0xff000000 | 0x005AA42E;
								Buf2[idx+1].color = Buf2[idx+1].color & 0xff000000 | 0x005AA42E;
								Buf2[idx+2].color = Buf2[idx+2].color & 0xff000000 | 0x005AA42E;
								Buf2[idx+3].color = Buf2[idx+3].color & 0xff000000 | 0x005AA42E;
							}
							else if ( nBlockType == 1 ) // low
							{
								Buf2[idx].color   = Buf2[idx].color   & 0xff000000 | 0x00E5D9F;
								Buf2[idx+1].color = Buf2[idx+1].color & 0xff000000 | 0x00E5D9F;
								Buf2[idx+2].color = Buf2[idx+2].color & 0xff000000 | 0x00E5D9F;
								Buf2[idx+3].color = Buf2[idx+3].color & 0xff000000 | 0x00E5D9F;
							}

							break;
						}

					case 10:
						{
							SVertex vertex = this->GetVertex(grid.dwVertexIndex[0]);
							Buf2[idx].color = vertex.oColor;

							vector<DWORD>::iterator iter;
							iter = find(errorLogicVertexs.begin(), errorLogicVertexs.end(), vertex.dwVertexIndex);
							if( iter != errorLogicVertexs.end() )
								Buf2[idx].color   = Buf2[idx].color   & 0xff000000 | 0xff;

							vertex = this->GetVertex(grid.dwVertexIndex[3]);
							Buf2[idx+1].color = vertex.oColor;
							iter = find(errorLogicVertexs.begin(), errorLogicVertexs.end(), vertex.dwVertexIndex);
							if( iter != errorLogicVertexs.end() )
								Buf2[idx+1].color   = Buf2[idx+1].color   & 0xff000000 | 0xff;

							vertex = this->GetVertex(grid.dwVertexIndex[1]);
							Buf2[idx+2].color = vertex.oColor;
							iter = find(errorLogicVertexs.begin(), errorLogicVertexs.end(), vertex.dwVertexIndex);
							if( iter != errorLogicVertexs.end() )
								Buf2[idx+2].color   = Buf2[idx+2].color   & 0xff000000 | 0xff;

							vertex = this->GetVertex(grid.dwVertexIndex[2]);
							Buf2[idx+3].color = vertex.oColor;
							iter = find(errorLogicVertexs.begin(), errorLogicVertexs.end(), vertex.dwVertexIndex);
							if( iter != errorLogicVertexs.end() )
								Buf2[idx+3].color   = Buf2[idx+3].color   & 0xff000000 | 0xff;

							break;
						}
					default:
						break;
					}
				}
			}

			CEditTerrainNode *pNode = (CEditTerrainNode*)(m_pTerrain->GetNode(URegionID(j,i)));
			CVertexBufferBinding*pVBB = pNode->m_TerrainTiles[m].op.vertexData->vertexBufferBinding;
			// write buffer
			switch (streamIndex)
			{
			case 0:
				{
					CHardwareBuffer* pBuf0 = pVBB->GetBuffer(0);
					pBuf0->write(0, pBuf0->getSize(), &(Buf0[0]), true);
					break;
				}
			case 1:
				{
					CHardwareBuffer* pBuf1 = pVBB->GetBuffer(1);
					pBuf1->write(0, pBuf1->getSize(), &(Buf1[0]), true);
					break;
				}
			case 2:
				{
					CHardwareBuffer* pBuf2 = pVBB->GetBuffer(2);
					pBuf2->write(0, pBuf2->getSize(), &(Buf2[0]), true);
					break;
				}
			case 3:
				{
					CHardwareBuffer* pBuf3 = pVBB->GetBuffer(3);
					pBuf3->write(0, pBuf3->getSize(), &(Buf3[0]), true);
					break;
				}
			case 4:
				{
					CHardwareBuffer* pBuf4 = pVBB->GetBuffer(4);
					pBuf4->write(0, pBuf4->getSize(), &(Buf4[0]), true);
					break;
				}
			case 5:
				{
					CHardwareBuffer* pBuf5 = pVBB->GetBuffer(5);
					pBuf5->write(0, pBuf5->getSize(), &(Buf5[0]), true);
					break;
				}
			case 6:
				{
					CHardwareBuffer* pBuf3 = pVBB->GetBuffer(3);
					pBuf3->write(0, pBuf3->getSize(), &(Buf3[0]), true);

					CHardwareBuffer* pBuf5 = pVBB->GetBuffer(5);
					pBuf5->write(0, pBuf5->getSize(), &(Buf5[0]), true);
					break;
				}
			case 7:
				{
					CHardwareBuffer* pBuf2 = pVBB->GetBuffer(2);
					pBuf2->write(0, pBuf2->getSize(), &(Buf2[0]), true);

					CHardwareBuffer* pBuf3 = pVBB->GetBuffer(3);
					pBuf3->write(0, pBuf3->getSize(), &(Buf3[0]), true);

					CHardwareBuffer* pBuf4 = pVBB->GetBuffer(4);
					pBuf4->write(0, pBuf4->getSize(), &(Buf4[0]), true);
					break;
				}
			case 9:
				{
					CHardwareBuffer* pBuf2 = pVBB->GetBuffer(2);
					pBuf2->write(0, pBuf2->getSize(), &(Buf2[0]), true);
					break;
				}
			case 10:
				{
					CHardwareBuffer* pBuf2 = pVBB->GetBuffer(2);
					pBuf2->write(0, pBuf2->getSize(), &(Buf2[0]), true);
					break;
				}
			default:
				break;
			}
		}
	}


	Buf0.clear();
	Buf1.clear();
	Buf2.clear();
	Buf3.clear();
	Buf4.clear();
	Buf5.clear();
}

void CTerrainMesh::ReWriteTerrainBuffer( const set<DWORD>& bufferIndexs, const set<int>& gridIndexs, const DWORD color0, const DWORD color1 )
{
	bool bSetRegionTool	= CEditStateOpertiaonMgr::GetInst()->GetEditState() == CEditStateOpertiaonMgr::EES_SET_TILE_REGIOON;
	
	vector<sqr::vTerrain::Color>    Buf2;// buf2: vertex original color顶点原始色（包括刷的顶点色）
	Buf2.resize(4096*4);

	DWORD bufferIndex	= 0;

	/*
	2---3 6---7
	|	| |	  |
	0---1 4---5
	*/

	set<DWORD>::const_iterator iter;
	set<DWORD>::const_iterator eiter = bufferIndexs.end();
	for ( iter  = bufferIndexs.begin(); iter != eiter; ++iter )
	{
		bufferIndex = *iter;
		int			j = bufferIndex % m_dwWidthInTileRegion;
		int			i = bufferIndex / m_dwWidthInTileRegion;
		UINT		idx		= 0;		// 每 4*4Region 即一个buffer里的顶点的索引
		STileRegion *pTileRegion = m_vecTileRegionList[bufferIndex];

		for ( uint m = 0; m < CTerrainMesh::Tile_PerSqr; ++m )
		{
			STileRenderRegionUnit *pRenderRegionUnit = pTileRegion->vRenderRegionUnits[m];
			UINT idx = 0;		//每一个TileRegion（4*4Region） 中切分的每个dp buffer 的idx

			int nGridIndex = 0, GridIndex = 0; // 格子索引
			uint size = CTerrainMgr::MAP_SIZE*CTerrainMgr::MAP_SIZE;
			for ( uint n = 0; n < size; ++n, ++GridIndex )
			{
				idx			= GridIndex * 4;
				nGridIndex  = pRenderRegionUnit->vGridIndices[n];
				idx			= GridIndex * 4;
				int nGridIndex = *iter;
				SGrid &grid = this->GetGrid(nGridIndex);
				
				SVertex vertex = this->GetVertex(grid.dwVertexIndex[0]);
				Buf2[idx].color = vertex.oColor;

				vertex = this->GetVertex(grid.dwVertexIndex[3]);
				Buf2[idx+1].color = vertex.oColor;

				vertex = this->GetVertex(grid.dwVertexIndex[1]);
				Buf2[idx+2].color = vertex.oColor;

				vertex = this->GetVertex(grid.dwVertexIndex[2]);
				Buf2[idx+3].color = vertex.oColor;

				if( gridIndexs.find(nGridIndex) != gridIndexs.end() )
				{
					Buf2[idx].color   = Buf2[idx].color   & 0xff000000 | color1;
					Buf2[idx+1].color = Buf2[idx+1].color & 0xff000000 | color1;
					Buf2[idx+2].color = Buf2[idx+2].color & 0xff000000 | color1;
					Buf2[idx+3].color = Buf2[idx+3].color & 0xff000000 | color1;
				}
				else
				{
					if ( bSetRegionTool && grid.bInHideRegion )
					{
						Buf2[idx].color   = Buf2[idx].color   & 0xff000000 | color0;
						Buf2[idx+1].color = Buf2[idx+1].color & 0xff000000 | color0;
						Buf2[idx+2].color = Buf2[idx+2].color & 0xff000000 | color0;
						Buf2[idx+3].color = Buf2[idx+3].color & 0xff000000 | color0;
					}
				}
			}

			CEditTerrainNode *pNode = (CEditTerrainNode*)(m_pTerrain->GetNode(URegionID(j,i)));
			
			// write buffer
			CHardwareBuffer* pBuf2 = pNode->m_TerrainTiles[m].op.vertexData->vertexBufferBinding->GetBuffer(2);
			pBuf2->write(0, pBuf2->getSize(), &(Buf2[0]), true);

			Buf2.clear();
		}
	}
}

void  CTerrainMesh::TurnOnCliff(bool tf)
{
	m_pTerrain->ClearRSShaderType();
	TerrainFlag::Mask mask = m_pTerrain->GetRenderFlag();
	if (tf)
	{
		this->ReWriteTerrainBuffer( this->m_TerrainBufferIndexs, 6 );
		mask |= TerrainFlag::Cliff;
	}
	else
	{
		this->ReWriteTerrainBuffer( this->m_TerrainBufferIndexs, 3 );
		mask &= ~TerrainFlag::Cliff;
	}

	if (m_pTerrain)
		m_pTerrain->SetRenderFlag(mask);
}

bool CTerrainMesh::Create(DWORD dwWidth, DWORD dwDepth, string strTextureNameDefault, bool bWater, bool bNewTerrain)
{
	m_bHideAdhereGridsRender = false;
	m_bWater = bWater;

	m_DynamicLightGrids.clear();
	m_VisibleDynamicLightGrids.clear();

	if ( !m_bWater ) // terrain
	{
		m_dwGameClientID = 0;
		m_dwGameServerID = 0;

		m_strTerrainDefaultTexName = strTextureNameDefault;

		CGraphic * pCoreGraphic = CGraphic::GetInst();

		if ( !pLightSystem )
			pLightSystem = CMainWindowTarget::GetInst()->GetGroup()->GetLightSystem();

		CCmdMgr::GetInst()->Clear();
		m_strTextureNameDefault = strTextureNameDefault;

		m_dwWidthInRegion = dwWidth;
		m_dwDepthInRegion = dwDepth;

		m_dwWidthInTileRegion = dwWidth / TILE_WIDTH_IN_REGION;
		m_dwDepthInTileRegion = dwDepth / TILE_WIDTH_IN_REGION;

		m_dwWidthInDivideReg = dwWidth * REGION_STEP_DIVIDE;
		m_dwDepthInDivideReg = dwDepth * REGION_STEP_DIVIDE;

		m_dwWidth = dwWidth * REGION_SPACE;
		m_dwDepth = dwDepth * REGION_SPACE;
		//CSceneManager::GetInst()->InitScene(m_dwWidth*64.0f,m_dwDepth*64.0f,gLog2(m_dwWidth>m_dwDepth?m_dwWidth/64:m_dwDepth/64)+1);

		m_dwRegionCnt = m_dwWidthInRegion *	m_dwDepthInRegion;
		m_dwVisibleRegionCnt = m_dwRegionCnt;

		DWORD dwVerterxCntInRegion = (REGION_SPACE+1)*(REGION_SPACE+1);
		DWORD dwGridCntInRegion = (REGION_SPACE)*(REGION_SPACE);

		m_pRegions = new SRegion[m_dwRegionCnt];
		for (size_t i = 0; i < m_dwRegionCnt; i++ )
		{
			SRegion & reg = m_pRegions[i];

			int nX = i % m_dwWidthInRegion;
			int nY = i / m_dwWidthInRegion;

			int nVertexIndex = nX*REGION_SPACE + nY*REGION_SPACE*(this->GetWidth()+1);
			for ( size_t j = 0; j < dwVerterxCntInRegion; j++ )
			{
				if ( j != 0 && j % (REGION_SPACE+1) == 0 )
				{
					nVertexIndex += -1 + this->GetWidth()+1 - REGION_SPACE;
				}

				reg.vVertexIndices.push_back(nVertexIndex);

				nVertexIndex++;
			}

			int nGridIndex = nX*REGION_SPACE + nY*REGION_SPACE*(this->GetWidth());
			for ( size_t j = 0; j < dwGridCntInRegion; j++ )
			{
				if ( j != 0 && j % (REGION_SPACE) == 0 )
				{
					nGridIndex += -1 + this->GetWidth() - REGION_SPACE +1;
				}

				reg.vGridIndices.push_back(nGridIndex);

				nGridIndex++;
			}
		}

		m_colVertexCnt = m_dwWidth + 1; //横向
		m_rowVertexCnt = m_dwDepth + 1; //纵向

		m_dwVertexCnt = m_colVertexCnt * m_rowVertexCnt;

		m_pVertices = new SVertex[m_dwVertexCnt];

		ZeroMemory(m_pVertices, sizeof(SVertex) * m_dwVertexCnt);

		CVector3f vNormal(0.0, 1.0f, 0.0f);

		float stepu = 1.0f / m_dwWidth;
		float stepv = 1.0f / m_dwDepth;
		float delta = 0.001f;
		float U[2][4] =
		{
			{delta, 0.5f, 0.5f, delta},
			{0.5f, 1.0f - delta, 1.0f - delta, 0.5f}
		};
		float V[2][4] =
		{
			{1.0f - delta, 1.0f - delta, 0.5f, 0.5f},
			{0.5f, 0.5f, delta, delta}
		};

		int mod = 0, div = 0;
		for ( DWORD dwVertexIndex = 0; dwVertexIndex < m_dwVertexCnt; dwVertexIndex++ )
		{
			mod = dwVertexIndex % m_colVertexCnt;
			div = dwVertexIndex / m_colVertexCnt;

			float x = (float)mod * GRID_SPACE;
			float z = (float)div * GRID_SPACE;
			float y = 0.0f;

			SVertex & vertex = m_pVertices[dwVertexIndex];
			vertex.dwVertexIndex = dwVertexIndex;
			vertex.vPosition = CVector3f(x, y, z);
			vertex.vNormal = vNormal;
			vertex.oColor = VERTEX_COLOR;
			vertex.bColor = VERTEX_COLOR;
			vertex.tColor = VERTEX_COLOR;
			vertex.sColor = VERTEX_SPECULAR_COLOR;
			vertex.fLogicHeight = 0.0f;
			vertex.pWireBoxLogicHeight = NULL;
		}

		m_dwGridCnt = m_dwWidth * m_dwDepth;

		if ( m_dwGridCnt > MAX_GRID_SIZE )
		{
			MessageBox(NULL, "error : CTerrainMesh::Create : Too many grids...", "错误", MB_OK);

			return false;
		}

		m_pGrids = new SGrid[m_dwGridCnt];

		/*
		* z v3  --------  v2
		*		|        |
		*		|	     |
		*	v0	|________| v1   ---> x
		*/

		int subGridIndex = 0, subIndex = 0, index = 0, nSubWidth = m_dwWidth * 2;
		int cntcount = 0;static bool bFirst = true;
		for ( DWORD z = 0 ; z < m_dwDepth; z++ )
		{
			for ( DWORD x = 0 ; x < m_dwWidth ; x++ )
			{
 				DWORD dwIndex = x + z * m_dwWidth;
// 				if( !bFirst )
//  					DebugOut("%d Grid 列:%d, 行: %d, %d\n", cntcount, x, z, dwIndex);
 				++cntcount;

				DWORD v0, v1, v2, v3;

				v0 = z * (m_dwWidth + 1) + x;
				v1 = v0 + 1;
				v2 = v1 + (m_dwWidth + 1);
				v3 = v2 - 1;

				SGrid & grid = m_pGrids[dwIndex];
				grid.nBlockType = 0; // none
				grid.dwGridIndex = dwIndex;
				grid.nodeInfo.dwGridIndex = dwIndex;
				grid.nodeInfo.strTextureNames[0] = strTextureNameDefault;
				grid.pRiver = NULL;
				grid.pWave = NULL;
				grid.bVisible = true;
				grid.bSide = false;
				grid.bInHideRegion = false;

				grid.dwVertexIndex[0] = v0;
				grid.dwVertexIndex[1] = v1;
				grid.dwVertexIndex[2] = v2;
				grid.dwVertexIndex[3] = v3;

				subGridIndex = x * 2 + z * nSubWidth * 2;
				// sub grid
				for ( int mm = 0; mm < 2; ++mm )
				{
					for ( int nn = 0; nn < 2; ++nn )
					{
						subIndex = nn + 2 * mm;
						index = nn + mm * nSubWidth + subGridIndex;

						grid.subGridIndex[index] = subIndex;

						m_SubGridIndexs[index] = dwIndex;
					}
				}

				// -----------------------------------------------------------------------------

				int nX = x % 2;
				int nZ = z % 2;

				if ( ( !nX && !nZ ) || ( nX && nZ ) )
				{
					grid.bReverse = true;
				}

				for (int i = 0; i < 4; i++)
				{
					grid.uv[0][i].x = U[nX][i];
					grid.uv[0][i].y = V[nZ][i];
					grid.uv[1][i].x = U[nX][i];
					grid.uv[1][i].y = V[nZ][i];

					grid.uvOld[0][i] = grid.uv[0][i];
					grid.uvOld[1][i] = grid.uv[1][i];
				}

				DWORD dwGridIndex;
				dwGridIndex = m_pVertices[v0].usbUsedCnt;
				m_pVertices[v0].dwGridsBelongto[dwGridIndex] = dwIndex;
				m_pVertices[v0].usbUsedCnt++;

				dwGridIndex = m_pVertices[v1].usbUsedCnt;
				m_pVertices[v1].dwGridsBelongto[dwGridIndex] = dwIndex;
				m_pVertices[v1].usbUsedCnt++;

				dwGridIndex = m_pVertices[v2].usbUsedCnt;
				m_pVertices[v2].dwGridsBelongto[dwGridIndex] = dwIndex;
				m_pVertices[v2].usbUsedCnt++;

				dwGridIndex = m_pVertices[v3].usbUsedCnt;
				m_pVertices[v3].dwGridsBelongto[dwGridIndex] = dwIndex;
				m_pVertices[v3].usbUsedCnt++;

				grid.vCenter.x = (float)(grid.dwVertexIndex[0] % (m_dwWidth + 1)) * GRID_SPACE + GRID_SPACE/2;
				grid.vCenter.y = 0.0f;
				grid.vCenter.z = (float)(grid.dwVertexIndex[0] / (m_dwWidth + 1)) * GRID_SPACE + GRID_SPACE/2;

				// -----------------------------------------------------------------------------

				int nXMiniMap = x / MINIMAP_BLOCK_GRID_SIZE;
				int nZMiniMap = z / MINIMAP_BLOCK_GRID_SIZE;
				//grid.dwMiniBlockIndex = nZMiniMap * MINIMAP_BLOCK_SIZE + nXMiniMap;

				int nXMiniMapBlock = x % MINIMAP_BLOCK_GRID_SIZE;
				int nZMiniMapBlock = z % MINIMAP_BLOCK_GRID_SIZE;
				nZMiniMapBlock = MINIMAP_BLOCK_GRID_SIZE - nZMiniMapBlock - 1;

				//grid.dwMGridIndexInMiniBlock = nZMiniMapBlock * MINIMAP_BLOCK_GRID_SIZE + nXMiniMapBlock;

				int j = 0;
			}
		}

		bFirst = !bFirst;

		// 逻辑上分的四分之一区域
		int dwDivideRegionCnt = m_dwWidthInDivideReg * m_dwDepthInDivideReg;
		SDivideRegion* pDivideRegion = NULL;
		CVector3f center;

		CVector3f smallcenter, firstcenter;
		smallcenter.x = 32.0f;
		smallcenter.z = 32.0f;
		smallcenter.y = 0.0f;

		int cenetrindex = (REGION_SPACE / REGION_STEP_DIVIDE) -1;
		int index1 = cenetrindex * REGION_SPACE + cenetrindex;
		int nDiviceReginWidthGrids = REGION_SPACE / REGION_STEP_DIVIDE;
		int nDivideRegIndex = 0;
		int col = 0, row = 0, nGridIndex = 0, nDivideIndex = 0, nRgnGridIndex;
		int nDivideRgnGridCnt = nDiviceReginWidthGrids * nDiviceReginWidthGrids;
		index = 0;

		for (DWORD i = 0; i < m_dwRegionCnt; ++i )
		{
			SRegion& region = m_pRegions[i];

			SGrid grid0 = m_pGrids[region.vGridIndices[0]];
			SGrid grid1 = m_pGrids[region.vGridIndices[255]];

			region.vCenter = (grid1.vCenter + grid0.vCenter) * 0.5f;
			nDivideRegIndex = (i/dwWidth) * dwWidth * REGION_DIVIDE + (i%dwWidth) * REGION_STEP_DIVIDE;

			SGrid grid2 = m_pGrids[region.vGridIndices[index1]];
			firstcenter = (grid2.vCenter + grid0.vCenter) * 0.5f;

			for ( int m = 0; m < REGION_STEP_DIVIDE; ++m )
			{
				for ( int n = 0;n < REGION_STEP_DIVIDE; ++n )
				{
					index = n + m * REGION_STEP_DIVIDE;

					nDivideIndex = nDivideRegIndex + n + m * m_dwWidthInDivideReg;
					region.divideRgnIndices[index] = nDivideIndex;

					pDivideRegion = new SDivideRegion;
					m_pDivideRegions[nDivideIndex] = pDivideRegion;

					for ( int k = 0; k < nDiviceReginWidthGrids; ++k )
					{
						for ( int j = 0; j < nDiviceReginWidthGrids; ++j )
						{
							nRgnGridIndex = m * REGION_SPACE * (nDiviceReginWidthGrids) + n * nDiviceReginWidthGrids + j + k * REGION_SPACE;
							nGridIndex = region.vGridIndices[nRgnGridIndex];
							pDivideRegion->vGridIndices.push_back(nGridIndex);

							m_pGrids[nGridIndex].m_nDivideRegionIndex = nDivideIndex;
							m_pGrids[nGridIndex].m_nRegionIndex		  = i;
						}
					}

					grid0 = m_pGrids[pDivideRegion->vGridIndices[0]];
					grid1 = m_pGrids[pDivideRegion->vGridIndices[nDivideRgnGridCnt-1]];

					pDivideRegion->m_vCenter = (grid1.vCenter + grid0.vCenter) * 0.5f;
				}
			}
		}

		int left = 0, right = 0;
		index = 0;
		// ----------------------------------------------------------------------------
		// 计算单独一个区域的中心位置，一个区域的半径大小为512.f，一个区域的长度为64.0 * 16 = 1024.0f
		for (int m = 0; m < dwDivideRegionCnt; ++m )
		{
			vector<DWORD> regionlist;

			for ( int i = -(int)m_dwWidthInDivideReg; i <= (int)m_dwWidthInDivideReg; i += (int)m_dwWidthInDivideReg )
			{
				index = m + i;

				left = m % m_dwWidthInDivideReg ? index-1 : -1;
				if ( left >= 0 && left < dwDivideRegionCnt )
				{
					regionlist.push_back(left);
				}

				if ( index >= 0 && index < dwDivideRegionCnt )
				{
					regionlist.push_back(index);
				}

				right = (m + 1) % m_dwWidthInDivideReg ? index + 1 : -1;
				if ( right >= 0 && right < dwDivideRegionCnt )
				{
					regionlist.push_back(right);
				}

			}
			_RegionAjenctList.insert(make_pair(m, regionlist));
		}

		DWORD dwColVertexCnt = m_dwWidth + 1;///一行的顶点个数
		m_dwVerterxCntInTileRegion = (TILE_REGION_SPACE+1)*(TILE_REGION_SPACE+1);
		m_dwGridCntInTileRegion = (TILE_REGION_SPACE)*(TILE_REGION_SPACE);
		
		m_dwTileRegionCnt = m_dwWidthInTileRegion *	m_dwDepthInTileRegion;
		
		for (DWORD i = 0; i < m_dwTileRegionCnt; i++ )
		{
			STileRegion *pTileRegion = new STileRegion;
			m_vecTileRegionList.push_back(pTileRegion);

			int nX = i % m_dwWidthInTileRegion;
			int nY = i / m_dwWidthInTileRegion;//TILE_REGION_STEP_DIVIDE

			int nVertexIndex = nX * TILE_REGION_SPACE + nY * TILE_REGION_SPACE * dwColVertexCnt;
			for ( size_t j = 0; j < m_dwVerterxCntInTileRegion; j++ )
			{
				if ( j != 0 && j % (TILE_REGION_SPACE+1) == 0 )
				{
					nVertexIndex += -1 + dwColVertexCnt - TILE_REGION_SPACE;
				}

				pTileRegion->vVertexIndices.push_back(nVertexIndex);

				nVertexIndex++;
			}

			int nGridIndex = nX*TILE_REGION_SPACE + nY*TILE_REGION_SPACE*m_dwWidth;
			for ( size_t j = 0; j < m_dwGridCntInTileRegion; j++ )
			{
				if ( j != 0 && j % (TILE_REGION_SPACE) == 0 )
				{
					nGridIndex += -1 + m_dwWidth - TILE_REGION_SPACE +1;
				}

				pTileRegion->vGridIndices.push_back(nGridIndex);

				nGridIndex++;
			}

			int nRegionIndex = 0;
			int nRegionIndex4x4 = nX * 4 + nY * m_dwWidthInTileRegion * 16;	// 4*4Region的左下脚Region索引
			m_TerrainBufferIndexs.insert(i);

			for ( DWORD m = 0; m < 4; ++m )
			{
				for ( DWORD n = 0; n < 4; ++n )
				{
					nRegionIndex = nRegionIndex4x4 + n + m * m_dwWidthInRegion;
					pTileRegion->vRegionIndices.push_back(nRegionIndex);
				}
			}

			//
			///render region unit
			pTileRegion->vRenderRegionUnits.resize(CTerrainMesh::Tile_PerSqr);
			for ( uint n = 0; n < CTerrainMesh::Tile_PerSqr; ++n )
			{
				STileRenderRegionUnit *pRenderRegionUnit	= new STileRenderRegionUnit;
				pRenderRegionUnit->nTileRegionIndex			= i;
				pRenderRegionUnit->nIndexInTileRegion		= n;
				pRenderRegionUnit->vGridIndices.resize(CTerrainMgr::MAP_SIZE*CTerrainMgr::MAP_SIZE);
				pTileRegion->vRenderRegionUnits[n]			= pRenderRegionUnit;

				uint wi = (n % CTerrainMesh::Tile_PreNum) * CTerrainMgr::MAP_SIZE;
				uint hi = (n / CTerrainMesh::Tile_PreNum) * CTerrainMgr::MAP_SIZE;

				int nUnitIndex = 0;
				for( uint mm = 0; mm < CTerrainMgr::MAP_SIZE; ++mm )
				{
					for( uint nn = 0; nn < CTerrainMgr::MAP_SIZE; ++nn )
					{
						int index = (nn + wi) + (mm + hi)*eSRegionSpan;
						nGridIndex = pTileRegion->vGridIndices[index];
						pRenderRegionUnit->vGridIndices[nUnitIndex++] = nGridIndex;
						SGrid &grid = this->GetGrid(nGridIndex);
						grid.bSubRenderUnitIndex = n;
						//DebugOut("%d, wi = %d, hi = %d, index %d, nGridIndex = %d, %d\n", i, wi, hi, index, nGridIndex, grid.bSubRenderUnitIndex);
					}
				}
			}
		}
	}
	else // water
	{
		m_strTextureNameDefault = strTextureNameDefault;

		m_dwWidthInRegion = dwWidth;
		m_dwDepthInRegion = dwDepth;

		m_dwWidth = dwWidth * REGION_SPACE;
		m_dwDepth = dwDepth * REGION_SPACE;

		m_dwRegionCnt = m_dwWidthInRegion *	m_dwDepthInRegion;

		m_pRegions = new SRegion[m_dwRegionCnt];

		// create a default river
		vector<string> textureNames(2);
		textureNames[0] = "tile\\地编专用\\tex\\big.dds";
		textureNames[1] = "tile\\地编专用\\tex\\small.dds";
		CToolSetTileTexture::GetInst()->SetTextureNameHot(textureNames);
		CMapEditApp::GetInst()->GetWaterOperator()->NewRiver("test");
		CRiver* defaultRiver = CMapEditApp::GetInst()->GetWaterOperator()->GetRiver();
		this->AddRiver(defaultRiver);

		for (size_t i = 0; i < m_dwRegionCnt; i++ )
		{
			SRegion & reg = m_pRegions[i];

			int nX = i%this->GetWidthInRegion();
			int nY = i/this->GetWidthInRegion();

			int nVertexIndex = nX*REGION_SPACE + nY*REGION_SPACE*(this->GetWidth()+1);

			DWORD dwVerterxCntInRegion = (REGION_SPACE+1)*(REGION_SPACE+1);

			for ( size_t j = 0; j < dwVerterxCntInRegion; j++ )
			{
				if ( j != 0 && j % (REGION_SPACE+1) == 0 )
				{
					nVertexIndex += -1 + this->GetWidth()+1 - REGION_SPACE;
				}

				reg.vVertexIndices.push_back(nVertexIndex);

				nVertexIndex++;
			}

			int nGridIndex = nX*REGION_SPACE + nY*REGION_SPACE*(this->GetWidth());

			DWORD dwGridCntInRegion = (REGION_SPACE)*(REGION_SPACE);

			for ( size_t j = 0; j < dwGridCntInRegion; j++ )
			{
				if ( j != 0 && j % (REGION_SPACE) == 0 )
				{
					nGridIndex += -1 + this->GetWidth() - REGION_SPACE +1;
				}

				reg.vGridIndices.push_back(nGridIndex);

				nGridIndex++;
			}
		}

		m_dwVertexCnt = (m_dwWidth + 1) * (m_dwDepth + 1);

		m_pVertices = new SVertex[m_dwVertexCnt];
		ZeroMemory(m_pVertices, sizeof(SVertex) * m_dwVertexCnt);

		CVector3f vNormal(0.0, 1.0f, 0.0f);

		for ( DWORD dwVertexIndex = 0; dwVertexIndex < m_dwVertexCnt; dwVertexIndex++ )
		{
			float x = (float)(dwVertexIndex % (m_dwWidth + 1)) * GRID_SPACE;
			float z = (float)(dwVertexIndex / (m_dwWidth + 1)) * GRID_SPACE;
			SVertex & vertex = m_pVertices[dwVertexIndex];
			vertex.dwVertexIndex = dwVertexIndex;

			vertex.vPosition = CVector3f(x, WATER_LAYER_BASE, z);
			vertex.vNormal = vNormal;
			vertex.oColor = WATER_COLOR;
			vertex.fLogicHeight = 0.0f;
			vertex.pWireBoxLogicHeight = NULL;
		}

		m_dwGridCnt = m_dwWidth * m_dwDepth;

		if ( m_dwGridCnt > MAX_GRID_SIZE )
		{
			MessageBox(NULL, "error : CTerrainMesh::Create : Too many grids...", "错误", MB_OK);
			return false;
		}

		m_pGrids = new SGrid[m_dwGridCnt];

		/*
		* z v3  --------  v2
		*		|        |
		*		|	     |
		*	v0	|________| v1   ---> x
		*/

		for ( DWORD z = 0 ; z < m_dwDepth; z++ )
		{
			for ( DWORD x = 0 ; x < m_dwWidth ; x++ )
			{
				DWORD dwIndex = x + z * m_dwWidth;

				DWORD v0, v1, v2, v3;

				v0 = z * (m_dwWidth + 1) + x;
				v1 = v0 + 1;
				v2 = v1 + (m_dwWidth + 1);
				v3 = v2 - 1;

				SGrid & grid = m_pGrids[dwIndex];
				grid.nBlockType = 0; // none
				grid.dwGridIndex = dwIndex;
				grid.nodeInfo.dwGridIndex = dwIndex;
				grid.nodeInfo.strTextureNames[0] = strTextureNameDefault;
				//grid.pRiver = defaultRiver;
				grid.pWave = NULL;
				grid.bVisible = false;
				grid.bSide = false;
				grid.bInHideRegion = false;

				grid.dwVertexIndex[0] = v0;
				grid.dwVertexIndex[1] = v1;
				grid.dwVertexIndex[2] = v2;
				grid.dwVertexIndex[3] = v3;

				//////////////////////////////////////////////////////////////////////////

				int nX = x % 2;
				int nZ = z % 2;

				if ( ( !nX && !nZ ) || ( nX && nZ ) )
				{
					grid.bReverse = true;
				}

				nX = x % 4;
				nZ = z % 4;

				float delta = 0.001f;

				float U[4][4] =
				{
					{delta, 0.25f, 0.25f, delta},
					{0.25f, 0.5f, 0.5f, 0.25f},
					{0.5f, 0.75f, 0.75f, 0.5f},
					{0.75f, 1.0f - delta, 1.0f - delta, 0.75f}
				};
				float V[4][4] =
				{
					{1.0f - delta, 1.0f - delta, 0.75f, 0.75f},
					{0.75f, 0.75f, 0.5f, 0.5f},
					{0.5f, 0.5f, 0.25, 0.25f},
					{0.25f, 0.25f, delta, delta}
				};
				for (int i = 0; i < 4; i++)
				{
					grid.uv[0][i].x = U[nX][i];
					grid.uv[0][i].y = V[nZ][i];
					grid.uv[1][i].x = U[nX][i];
					grid.uv[1][i].y = V[nZ][i];
				}

				DWORD dwGridIndex;
				dwGridIndex = m_pVertices[v0].usbUsedCnt;
				m_pVertices[v0].dwGridsBelongto[dwGridIndex] = dwIndex;
				m_pVertices[v0].usbUsedCnt++;

				dwGridIndex = m_pVertices[v1].usbUsedCnt;
				m_pVertices[v1].dwGridsBelongto[dwGridIndex] = dwIndex;
				m_pVertices[v1].usbUsedCnt++;

				dwGridIndex = m_pVertices[v2].usbUsedCnt;
				m_pVertices[v2].dwGridsBelongto[dwGridIndex] = dwIndex;
				m_pVertices[v2].usbUsedCnt++;

				dwGridIndex = m_pVertices[v3].usbUsedCnt;
				m_pVertices[v3].dwGridsBelongto[dwGridIndex] = dwIndex;
				m_pVertices[v3].usbUsedCnt++;

				grid.vCenter.x = (float)(grid.dwVertexIndex[0] % (m_dwWidth + 1)) * GRID_SPACE + GRID_SPACE/2;
				grid.vCenter.y = WATER_LAYER_BASE;
				grid.vCenter.z = (float)(grid.dwVertexIndex[0] / (m_dwWidth + 1)) * GRID_SPACE + GRID_SPACE/2;

				int nXMiniMap = x / MINIMAP_BLOCK_GRID_SIZE;
				int nZMiniMap = z / MINIMAP_BLOCK_GRID_SIZE;
				//grid.dwMiniBlockIndex = nZMiniMap * MINIMAP_BLOCK_SIZE + nXMiniMap;

				int nXMiniMapBlock = x % MINIMAP_BLOCK_GRID_SIZE;
				int nZMiniMapBlock = z % MINIMAP_BLOCK_GRID_SIZE;
				nZMiniMapBlock = MINIMAP_BLOCK_GRID_SIZE - nZMiniMapBlock - 1;

				//grid.dwMGridIndexInMiniBlock = nZMiniMapBlock * MINIMAP_BLOCK_GRID_SIZE + nXMiniMapBlock;

				int j = 0;
			}
		}
	}
	return true;
}

//------------------------------------------------------------------------------
SDivideRegion * CTerrainMesh::GetDivideRegion(int nDivideRegionIndex)
{
	int size = (int)m_pDivideRegions.size();
	bool bOutRange = nDivideRegionIndex > size;
	if ( nDivideRegionIndex < 0 || bOutRange )
	{
		TCHAR szMessage[256];

		sprintf(szMessage, "%d子区域越界" , nDivideRegionIndex);

		MessageBox(NULL, szMessage, "提示", MB_OK | MB_ICONERROR);

		return m_pDivideRegions[0];
	}

	return m_pDivideRegions[nDivideRegionIndex];
}

bool CTerrainMesh::IsSubRegionValid(int nSubRgnIndex)
{
	int size = (int)m_pDivideRegions.size();

	return nSubRgnIndex < size && nSubRgnIndex > 0; 
}

void CTerrainMesh::RenderTerrainBlocks()
{
}

void CTerrainMesh::RenderLogicHeight()
{
	COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
	if ( !CEditStateOpertiaonMgr::GetInst()->GetDoesShowLogicHeight() )
		return;

	if( CSceneEnvMgr::GetInst()->GetIsGenerateMinMap() )
		return;

	SVertex vertex;
	CGraphic *pCoreGraphic = CGraphic::GetInst();
	bool bLineShowLogic = CEditStateOpertiaonMgr::GetInst()->GetIsLineShowLogic();

 	if( CSceneEnvMgr::GetInst()->GetIsShowScreenRangeGrid() )
 	{
 		int    gridindex = 0;
 		size_t size = calRangeVisibleGridIndices.size();
 		for ( size_t i = 0; i < size; ++i )
 		{
 			gridindex = calRangeVisibleGridIndices[i];
 			SGrid &grid = this->GetGrid(gridindex);
 			vertex = this->GetVertex(grid.dwVertexIndex[0]);
			if ( vertex.pWireBoxLogicHeight )
				vertex.pWireBoxLogicHeight->Render(pCoreGraphic, false, bLineShowLogic);
 		}
 	}
 	else
	{
		if( VisibleGridIndices.empty() )
			return;

		int nGridIndex = 0;
		size_t size = VisibleGridIndices.size();
		for ( size_t i = 0; i < size; ++i )
		{
			nGridIndex = VisibleGridIndices[i];
			SGrid &grid = m_pGrids[nGridIndex];
			vertex = m_pVertices[grid.dwVertexIndex[0]];

			if ( vertex.pWireBoxLogicHeight )
				vertex.pWireBoxLogicHeight->Render(pCoreGraphic, false, bLineShowLogic);
		}
	}

}

void CTerrainMesh::UpdateSceneLight()
{
	if ( this->m_bWater )
		return;

	if ( CEditStateOpertiaonMgr::GetInst()->GetEditState() == CEditStateOpertiaonMgr::EES_SET_TILE_REGIOON )
		return;

	if ( this->m_bHideAdhereGridsRender )
	{
		for ( int i = 1; i < 8;++i )
		{
			CRenderPipeline::GetInst()->SetLight( i, NULL );
		}
	}
	else
	{
		bool bLightMap			= CSceneEnvMgr::GetInst()->GetIsLightMapShadow();
		bool bRenerBlock		= CEditStateOpertiaonMgr::GetInst()->GetDoesShowBlock();
		bool bRenderLogicHeight = CEditStateOpertiaonMgr::GetInst()->GetDoesShowLogicHeight();

		//////只有在实时状态下，或者渲染阻挡，逻辑高的时候，才计算
		if( bRenderLogicHeight )
			SetVisibleSceneGrids();

		//////////////////////////////////////////////////////////////////////////
		///只有在实时状态下，光照才有作用，烘培状态下的场景点光源，聚光灯不需要再进行处理
		if( !bLightMap )
		{
			m_sortLight.clear();

			//可见动态光的判断
			bool bSetLightByScreenRange = CSceneEnvMgr::GetInst()->GetIsShowScreenRangeGrid();
 			if( !bSetLightByScreenRange )
 			{
 				this->DecideVisibleDynamicLights();
 			}
 			else
			{
				this->DecideCurRegDynamicLights();
			}

			SetSceneLight();
		}
		//////////////////////////////////////////////////////////////////////////
	}
}

void CTerrainMesh::SetSceneLight()
{
	if( m_sortLight.empty() )
	{
		for ( int i = 1; i < 8;++i )
		{
			//BOOL enable = FALSE;
			//pDevice->GetLightEnable(i, &enable);
			//pDevice->LightEnable( i, FALSE );
			CRenderPipeline::GetInst()->SetLight( i, NULL );
		}
		return;
	}

	CLightSystem* lightSystem = CMainWindowTarget::GetInst()->GetGroup()->GetLightSystem();

	sort( m_sortLight.begin(), m_sortLight.end(),cmplight );

	set<int>::iterator iiter;
	CLight tempLight;
	int coo = 1;
	size_t size = m_sortLight.size();

	for ( size_t i = 0; i < 7 && i < size; ++i )
	{
		int j = m_sortLight[i].indexj;
		iiter = m_VisibleDynamicLightGrids.find(m_sortLight[i].indexi);
		if ( iiter == m_VisibleDynamicLightGrids.end() )
			continue;
		SGrid & grid    = m_pGrids[*iiter];
		CEditModelGroup *pModelGroup = grid.vecCenterObjects[j]->GetModelGroup();
		bool isInfluenceTerrain = false;
		if ( m_sortLight[i].bSpotLight )
		{
			SceneLightToCLight(tempLight, pModelGroup->m_SpotLight);
			isInfluenceTerrain = pModelGroup->m_SpotLight.m_bEffectTerrain;
		}
		else
		{
			SceneLightToCLight(tempLight, pModelGroup->m_SL);
			isInfluenceTerrain = pModelGroup->m_SL.m_bEffectTerrain;
		}

		LightStruct& tLight = *tempLight.GetLightStruct(); 
		
		if (m_sortLight[i].Dynamic)
		{
			unsigned __int32 CurColor = RefreshDynamicSceneLight( pModelGroup->m_SL, ::GetTickCount() );
			tLight.Diffuse.r = ((BYTE)( GetRValue(CurColor)))/255.0f;
			tLight.Diffuse.g = ((BYTE)( GetGValue(CurColor)))/255.0f;
			tLight.Diffuse.b = ((BYTE)( GetBValue(CurColor)))/255.0f;
		}
		tLight.Position      = grid.vCenter ;
		tLight.Position.x   += pModelGroup->GetSelfPresentation(0)->GetShiftXEx();
		tLight.Position.y   += pModelGroup->GetSelfPresentation(0)->GetLiftEx();
		tLight.Position.z   += pModelGroup->GetSelfPresentation(0)->GetShiftZEx();

		lightSystem->SetLight( coo,&tempLight, isInfluenceTerrain);

		++coo;
	}

	// disable the other lights
	//ZeroMemory( &tempLight, sizeof(CLight) );
	for ( int i = coo; i < 8; ++i )
	{
		lightSystem->SetLight( i, NULL);
	}
}

void CTerrainMesh::SetVisibleSceneGrids()
{
	//以主干为主
	VisibleGridIndices.clear();

	DWORD dwRegionIndex4x4 = 0, dwRegionIndex = 0, bufferIndex = 0;
	vector<DWORD> visibleBufferIndices;

	set<DWORD>::iterator beg = m_TerrainBufferIndexs.begin();
	set<DWORD>::iterator end = m_TerrainBufferIndexs.end();
	for ( beg; beg != end; ++beg )
	{
		bufferIndex = *beg;
		int	j = bufferIndex % m_dwWidthInTileRegion;
		int	i = bufferIndex / m_dwWidthInTileRegion;

		if ( m_pTerrain->isVisable(URegionID(j,i)) )
			visibleBufferIndices.push_back(bufferIndex);
	}

	size_t size = visibleBufferIndices.size();
	vector<int>::iterator iter, eiter;
	for ( size_t i = 0; i < size; ++i )
	{
		bufferIndex = visibleBufferIndices[i];
		STileRegion *pTileRegion = m_vecTileRegionList[bufferIndex];

		int GridIndex = 0; // 格子索引
		for ( uint m = 0; m < CTerrainMesh::Tile_PerSqr; ++m )
		{
			STileRenderRegionUnit *pRenderRegionUnit = pTileRegion->vRenderRegionUnits[m];
			uint size = CTerrainMgr::MAP_SIZE*CTerrainMgr::MAP_SIZE;
			for ( uint n = 0; n < size; ++n )
			{
				VisibleGridIndices.push_back(pRenderRegionUnit->vGridIndices[n]);
			}
		}
	}

	if (VisibleGridIndices.size() > this->m_dwGridCnt )
		VisibleGridIndices.clear();
}

void CTerrainMesh::CalRangeVisibleGrid()
{
	calRangeVisibleGridIndices.clear();

	COperator* pOperator = CMapEditApp::GetInst()->GetOperator();
	if( pOperator == NULL )
		return ;

	int nWidthRange = 20;
	int nDepthRange = 20;
	int nHotGridIndex  = pOperator->GetGridIndexHot();
	int width = this->m_dwWidth;
	int nGridIndex = 0;

	if ( this->IsValid(nHotGridIndex) )
	{
		if( CEditStateOpertiaonMgr::GetInst()->GetDoesShowBlock() || CEditStateOpertiaonMgr::GetInst()->GetDoesShowLogicHeight() )
		{
			for( int i = -nDepthRange; i < nDepthRange; ++i )
			{
				for ( int j = -nWidthRange; j < nWidthRange; ++j )
				{
					nGridIndex = nHotGridIndex + j + i * width ;
					if ( this->IsValid(nGridIndex) )
						calRangeVisibleGridIndices.push_back(nGridIndex);
				}
			}
		}
	}
}

void CTerrainMesh::DecideVisibleDynamicLights()
{
	m_VisibleDynamicLightGrids.clear();

	//确定可见格子的动态光源
	int gridindex = 0;
	for ( set<int>::iterator beg = m_DynamicLightGrids.begin(); beg != m_DynamicLightGrids.end(); ++beg )
	{
		gridindex = *beg;
		if ( this->EnableDynamicLight(gridindex) )
		{
			m_VisibleDynamicLightGrids.insert(gridindex);
		}
	}

	if( m_VisibleDynamicLightGrids.empty() )
		return;

	size_t			   size = m_VisibleDynamicLightGrids.size();
	set<int>::iterator iter = m_VisibleDynamicLightGrids.begin();
	CVector3f		   HitP = CMapEditApp::GetInst()->GetOperator()->GetHitPoint();
	int				   gridIndex = 0;

	for ( size_t i = 0; i < size; ++i, ++iter )
	{
		gridIndex = *iter;

		SGrid & grid = m_pGrids[gridIndex];
		if ( grid.vecCenterObjects.empty() )
			continue;

		for (size_t j = 0; j < grid.vecCenterObjects.size(); j++)
		{
			if (( grid.vecCenterObjects[j]->GetModelGroup()->m_SL.m_bIAmbScenePointLight ) || grid.vecCenterObjects[j]->GetModelGroup()->m_SpotLight.m_bIAmbSceneSpotLight )
			{
				CVector3f oP   = *((CVector3f*)&grid.vCenter);
				oP.y           += grid.vecCenterObjects[j]->GetModelGroup()->GetSelfPresentation(0)->GetLiftEx();
				CVector3f JG   = oP - HitP;
				PX        temp = PX(JG.x*JG.x + JG.y*JG.y + JG.z*JG.z, gridIndex, j, grid.vecCenterObjects[j]->GetModelGroup()->m_SL.m_bDynamic, grid.vecCenterObjects[j]->GetModelGroup()->m_SpotLight.m_bIAmbSceneSpotLight);
				m_sortLight.push_back( temp );
			}
		}
	}
}

void CTerrainMesh::DecideCurRegDynamicLights()
{
	m_VisibleDynamicLightGrids.clear();

	this->CalRangeVisibleGrid();

	size_t size      = calRangeVisibleGridIndices.size();
	int	   gridIndex = 0;
	CVector3f   HitP = CMapEditApp::GetInst()->GetOperator()->GetHitPoint();

	for ( size_t i = 0; i < size; ++i )
	{
		gridIndex = calRangeVisibleGridIndices[i];
		SGrid &grid = this->GetGrid(gridIndex);

		if ( grid.vecCenterObjects.empty() )
			continue;

		if( m_DynamicLightGrids.find(gridIndex) == m_DynamicLightGrids.end() )
			continue;

		for (size_t j = 0; j < grid.vecCenterObjects.size(); j++)
		{
			if (( grid.vecCenterObjects[j]->GetModelGroup()->m_SL.m_bIAmbScenePointLight ) || grid.vecCenterObjects[j]->GetModelGroup()->m_SpotLight.m_bIAmbSceneSpotLight )
			{
				CVector3f oP   = *((CVector3f*)&grid.vCenter);
				oP.y           += grid.vecCenterObjects[j]->GetModelGroup()->GetSelfPresentation(0)->GetLiftEx();
				CVector3f JG   = oP - HitP;
				PX        temp = PX(JG.x*JG.x + JG.y*JG.y + JG.z*JG.z, gridIndex, j, grid.vecCenterObjects[j]->GetModelGroup()->m_SL.m_bDynamic, grid.vecCenterObjects[j]->GetModelGroup()->m_SpotLight.m_bIAmbSceneSpotLight);
				m_sortLight.push_back( temp );

				m_VisibleDynamicLightGrids.insert(gridIndex);
			}
		}
	}
}

bool CTerrainMesh::EnableDynamicLight(const int& gridindex)
{
	//根据灯光的范围确定最终开启的点光源
	float range = 0.0f;
	int boxwidth = 0;//包围盒的宽度

	SGrid & grid = m_pGrids[gridindex];
	if (!grid.vecCenterObjects.empty())
	{
		for ( size_t i  = 0; i < grid.vecCenterObjects.size(); ++i )
		{
			if ( grid.vecCenterObjects[i]->GetModelGroup()->m_SL.m_bDynamic )
			{
				range = grid.vecCenterObjects[i]->GetModelGroup()->m_SL.m_Rang;

				boxwidth = (int)(range * RangeWidth);
			}
		}
	}

	if ( boxwidth == 0 )
		return true;

// 	int colgridIndex = 0, rowgridIndex = 0;
// 
// 	for ( int i = -boxwidth; i < boxwidth; ++i )
// 	{
// 		rowgridIndex = gridindex + i * m_dwWidth;
// 
// 		for ( int j = -boxwidth; j < boxwidth; ++j )
// 		{
// 			colgridIndex = rowgridIndex + j;
// 
// 			if ( this->IsVisibleGrid(colgridIndex) )
// 				return true;
// 		}
// 	}

//	return false;

	DWORD bufferIndex = grid.dwBufferIndex;
	if ( m_pTerrain->isVisable(bufferIndex) )
		return true;
	else
		return false;
}

bool CTerrainMesh::IsVisibleGrid(const DWORD& gridindex)
{
	SGrid &grid = m_pGrids[gridindex];
	
	DWORD bufferIndex = grid.dwBufferIndex;
	if ( m_pTerrain->isVisable(bufferIndex) )
		return true;
	else
		return false;
}

bool CTerrainMesh::IsVisibleTileRegion(const DWORD& bufferIndex)
{
	return m_pTerrain->isVisable(bufferIndex);
}

void CTerrainMesh::SetGridTextureHandle(DWORD dwLayer, DWORD dwGridIndex, string strTextureName, const bool bCliff)
{
	Ast( dwLayer < 4 );
	SGrid & grid = this->GetGrid(dwGridIndex);

	//没有刷第2层，但却操作第2层以上的地表
	if ( grid.nodeInfo.strTextureNames[1] == "" && dwLayer == 2 )
	{
		//MessageBox(NULL, "确保第2层已刷", "错误操作", MB_OK);
		return;
	}

	grid.nodeInfo.strTextureNames[dwLayer] = strTextureName;
	grid.bCliffLayerOne = bCliff;
}

void CTerrainMesh::AdjustWaterMeshVisibility(int nGridIndex, bool bLoadTime)
{
	CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetTerrain();
	bool isAddWave = CMapEditApp::GetInst()->GetWaterOperator()->IsAddWave();
	bool bChangeAlpha = CMapEditApp::GetInst()->GetWaterOperator()->GetIsChangeWaterVetexAlpha();

	SGrid & gridT = pTerrain->GetGrid(nGridIndex);
	SGrid & gridW = this->GetGrid(nGridIndex);
	// add dirty region to river
	int regionIndex = this->GetRegionIndexByGridIndex(gridW.dwGridIndex);
	if (NULL != gridW.pRiver)
	{
		gridW.pRiver->AddDirtyRegion(regionIndex);
	}

	SCrossingPattern patterns;
	CVector2f vDirections[4];
	vDirections[0] = CVector2f(-1,-1);
	vDirections[1] = CVector2f(1,-1);
	vDirections[2] = CVector2f(1,1);
	vDirections[3] = CVector2f(-1,1);

	int nDirections[3][3] = { {-1,2,6}, {0,1,7}, {4,3,5} };
	float fTotalWeight = 0.0f;
	BYTE byTrue = 0;

	for (size_t i = 0; i < 4; ++i )
	{
		SVertex & vertexT = pTerrain->GetVertex(gridT.dwVertexIndex[i]);
		SVertex & vertexW = this->GetVertex(gridW.dwVertexIndex[i]);

		//DebugOut("grid %d, vertex %d, %f, %f\n", nGridIndex, gridT.dwVertexIndex[i], vertexT.vPosition.y, vertexW.vPosition.y);

		if ( vertexT.vPosition.y < vertexW.vPosition.y )
		{
			patterns.bHigherThanGround[i] = true;
			if ( !bLoadTime && bChangeAlpha )
			{
				float fRatio = 1.0f - min(vertexW.vPosition.y - vertexT.vPosition.y, WATER_VISIBILITY_THRESHOLD)/WATER_VISIBILITY_THRESHOLD;
				byTrue = ((BYTE*)(&vertexW.oColor))[3] = 0x22 + (BYTE)(0xdd*fRatio);
			}
		}
		else
		{
			patterns.fWeight[i] = vertexT.vPosition.y - vertexW.vPosition.y;
			fTotalWeight += patterns.fWeight[i];
		}
	}

	if ( patterns.bHigherThanGround[0] || patterns.bHigherThanGround[1] || patterns.bHigherThanGround[2] || patterns.bHigherThanGround[3] )
	{
		gridW.bVisible = true;
		this->InsertGridIndexVisibleWater(nGridIndex);

		//DebugOut("InsertGridIndexVisibleWater %d\n", nGridIndex);
		bool isWaveSide = !(patterns.bHigherThanGround[0] && patterns.bHigherThanGround[1] && patterns.bHigherThanGround[2] && patterns.bHigherThanGround[3]);
		if ( isWaveSide && isAddWave )
		{
			gridW.bSide = true;
			for (size_t i = 0; i < 4; ++i )
			{
				if ( !patterns.bHigherThanGround[i] )
				{
					SVertex & vertexW = this->GetVertex(gridW.dwVertexIndex[i]);
					((BYTE*)(&vertexW.oColor))[3] = 0xff; //byTrue;
				}
			}

			CVector2f vTotal(0.0f,0.0f);
			for ( int i = 0; i < 4; ++i )
			{
				if ( !patterns.bHigherThanGround[i] )
					vTotal += vDirections[i];
			}

			CVector2f vDirection = vTotal;
			int nX,nY;

			if ( vDirection.x < -0.01f )
				nX = 2;
			else if ( vDirection.x > 0.01f )
				nX = 1;
			else
				nX = 0;

			if ( vDirection.y < -0.01f )
				nY = 2;
			else if ( vDirection.y > 0.01f )
				nY = 1;
			else
				nY = 0;

			int n = nDirections[nX][nY];
			if ( !gridW.pWave )
			{
				gridW.pWave = gridW.pRiver->AddWave(regionIndex);
			}
			gridW.pWave->Create(n,gridW.vCenter);
		}
		else
		{
			gridW.bSide = false;
			if (NULL != gridW.pWave)
			{
				gridW.pRiver->RemoveWave(regionIndex, gridW.pWave);
				gridW.pWave = NULL;
			}
		}
	}
	else // lower than ground
	{
		gridW.bVisible = false;
		gridW.bSide = false;
		if (NULL != gridW.pWave)
		{
			gridW.pRiver->RemoveWave(regionIndex, gridW.pWave);
			gridW.pWave = NULL;
		}
		this->EraseGridIndexVisibleWater(nGridIndex);
	}
}

bool CTerrainMesh::IsOverlappingByVertexIndex(DWORD dwWidthNew, DWORD dwDepthNew, DWORD dwVertexIndexNew, DWORD & dwVertexIndexOld, const uint8 uResizeType)
{
	switch ( uResizeType )
	{
	case 0:
		{
			int nXMid = dwWidthNew/2 - m_dwWidthInRegion/2;
			int nZMid = dwDepthNew/2 - m_dwDepthInRegion/2;

			int nXDeltaBase = nXMid * REGION_SPACE;
			int nZDeltaBase = nZMid * REGION_SPACE;

			int nXDelta = dwVertexIndexNew % (dwWidthNew * REGION_SPACE + 1);
			int nZDelta = dwVertexIndexNew / (dwWidthNew * REGION_SPACE + 1);

			if ( nXDelta - nXDeltaBase > int(this->GetWidth()) || nZDelta - nZDeltaBase > int(this->GetDepth())
			        || nXDelta < nXDeltaBase || nZDelta < nZDeltaBase )
			{
				return false;
			}
			else
			{
				dwVertexIndexOld = (nZDelta - nZDeltaBase) * ( this->GetWidth() + 1 ) + nXDelta - nXDeltaBase;
				return true;
			}
		}
		break;

	case 1:
		{
			DWORD newTerrainWidth = dwWidthNew * REGION_SPACE;
			int nXDelta = dwVertexIndexNew % (newTerrainWidth + 1);
			int nZDelta = dwVertexIndexNew / (newTerrainWidth + 1);

			if ( nXDelta < int( this->m_dwWidth ) && nZDelta < int(this->m_dwDepth) )
			{
				dwVertexIndexOld = nZDelta * ( this->m_dwWidth + 1 ) + nXDelta;
				return true;
			}
			else
				return false;
		}
		break;

	case 2:
		{
			int newTerrainWidth = dwWidthNew * REGION_SPACE;
			int newTerrainDepth = dwDepthNew * REGION_SPACE;
			int nXDelta = dwVertexIndexNew % (newTerrainWidth + 1);
			int nZDelta = dwVertexIndexNew / (newTerrainWidth + 1);
			int nDeltaOffsetZ = newTerrainDepth - this->m_dwDepth + 1;

			if ( nXDelta < int( this->m_dwWidth ) && 
				 nZDelta >= nDeltaOffsetZ && nZDelta < (newTerrainDepth+1) )
			{
				dwVertexIndexOld = ( nZDelta - nDeltaOffsetZ ) * (this->m_dwWidth+1) + nXDelta;

				return true;
			}
			else
				return false;
		}
		break;

	case 3:
		{
			int newTerrainWidth = dwWidthNew * REGION_SPACE;
			int newTerrainDepth = dwDepthNew * REGION_SPACE;
			int nXDelta = dwVertexIndexNew % (newTerrainWidth + 1);
			int nZDelta = dwVertexIndexNew / (newTerrainWidth + 1);
			int nDeltaOffsetX = newTerrainWidth - this->m_dwWidth + 1;

			if ( nXDelta >= nDeltaOffsetX && nXDelta < (newTerrainWidth+1) && 
				 nZDelta < int(this->m_dwDepth) )
			{
				dwVertexIndexOld = nZDelta * ( this->m_dwWidth + 1 ) + nXDelta - nDeltaOffsetX;
				return true;
			}
			else
				return false;
		}
		break;

	case 4:
		{
			int newTerrainWidth = dwWidthNew * REGION_SPACE;
			int newTerrainDepth = dwDepthNew * REGION_SPACE;
			int nXDelta = dwVertexIndexNew % (newTerrainWidth + 1);
			int nZDelta = dwVertexIndexNew / (newTerrainWidth + 1);

			int nDeltaOffsetX = newTerrainWidth - this->m_dwWidth + 1;
			int nDeltaOffsetZ = newTerrainDepth - this->m_dwDepth + 1;

			if ( nXDelta >= nDeltaOffsetX && nXDelta < (newTerrainWidth+1) && 
				 nZDelta >= nDeltaOffsetZ && nZDelta < (newTerrainDepth+1) )
			{
				dwVertexIndexOld = ( nZDelta - nDeltaOffsetZ ) * (this->m_dwWidth+1) + nXDelta - nDeltaOffsetX;

				return true;
			}
			else
				return false;
		}
		break;

	default:
		break;
	}

	return false;
}

bool CTerrainMesh::IsOverlappingByGridIndex(DWORD dwWidthNew, DWORD dwDepthNew, DWORD dwGridIndexNew, DWORD & dwGridIndexOld, const uint8 uResizeType)
{
	switch ( uResizeType )
	{
	case 0:
		{
			int nXMid = dwWidthNew/2 - m_dwWidthInRegion/2;
			int nZMid = dwDepthNew/2 - m_dwDepthInRegion/2;

			int nXDeltaBase = nXMid * REGION_SPACE;
			int nZDeltaBase = nZMid * REGION_SPACE;

			int nXDelta = dwGridIndexNew % (dwWidthNew * REGION_SPACE);
			int nZDelta = dwGridIndexNew / (dwWidthNew * REGION_SPACE);

			if ( nXDelta - nXDeltaBase > int(this->m_dwWidth - 1) || nZDelta - nZDeltaBase > int(this->m_dwDepth - 1)
			        || nXDelta < nXDeltaBase || nZDelta < nZDeltaBase )
			{
				return false;
			}
			else
			{
				dwGridIndexOld = (nZDelta - nZDeltaBase) * this->m_dwWidth + nXDelta - nXDeltaBase;
				return true;
			}
		}
		break;

	case 1:
		{
			DWORD newTerrainWidth = dwWidthNew * REGION_SPACE;

			int nXDelta = dwGridIndexNew % newTerrainWidth;
			int nZDelta = dwGridIndexNew / newTerrainWidth;

			if ( nXDelta < int(this->m_dwWidth) && nZDelta < int(this->m_dwDepth) )
			{
				dwGridIndexOld = nZDelta * this->m_dwWidth + nXDelta;

				return true;
			}
			else
				return false;
		}
		break;

	case 2:
		{
			int newTerrainWidth = dwWidthNew * REGION_SPACE;
			int newTerrainDepth = dwDepthNew * REGION_SPACE;

			int nXDelta = dwGridIndexNew % newTerrainWidth;
			int nZDelta = dwGridIndexNew / newTerrainWidth;

			int nDeltaOffsetZ = newTerrainDepth - this->m_dwDepth;

			if ( nXDelta < (int)(this->m_dwWidth) && 
				 nZDelta < newTerrainDepth && nZDelta >= nDeltaOffsetZ )
			{
				dwGridIndexOld = ( nZDelta - nDeltaOffsetZ ) * this->m_dwWidth + nXDelta;

				return true;
			}
			else
				return false;
		}
		break;

	case 3:
		{
			int newTerrainWidth = dwWidthNew * REGION_SPACE;
			int newTerrainDepth = dwDepthNew * REGION_SPACE;

			int nXDelta = dwGridIndexNew % newTerrainWidth;
			int nZDelta = dwGridIndexNew / newTerrainWidth;

			int nDeltaOffsetX = newTerrainWidth - this->m_dwWidth;

			if ( nXDelta < newTerrainWidth && nXDelta > nDeltaOffsetX && 
				 nZDelta < int(this->m_dwDepth) )
			{
				dwGridIndexOld = nZDelta * this->m_dwWidth + nXDelta - nDeltaOffsetX;

				return true;
			}
			else
				return false;
		}
		break;

	case 4:
		{
			int newTerrainWidth = dwWidthNew * REGION_SPACE;
			int newTerrainDepth = dwDepthNew * REGION_SPACE;

			int nXDelta = dwGridIndexNew % newTerrainWidth;
			int nZDelta = dwGridIndexNew / newTerrainWidth;

			int nDeltaOffsetX = newTerrainWidth - this->m_dwWidth;
			int nDeltaOffsetZ = newTerrainDepth - this->m_dwDepth;

			if ( nXDelta >= nDeltaOffsetX && nXDelta < newTerrainWidth && 
			 	 nZDelta >= nDeltaOffsetZ && nZDelta < newTerrainDepth )
			{
				dwGridIndexOld = ( nZDelta - nDeltaOffsetZ ) * this->m_dwWidth + nXDelta - nDeltaOffsetX;

				return true;
			}
			else
				return false;
		}
		break;

	default:
		break;
	}

	return false;
}

void CTerrainMesh::ChangeGridsTextureName(const string &oldname, const string &newname)
{
	DWORD dwGridCnt = this->GetGridCount();

	for ( DWORD i = 0; i < dwGridCnt; ++i )
	{
		SGrid &grid = this->GetGrid(i);

		if ( grid.nodeInfo.strTextureNames[0].compare(oldname) == 0 )
			grid.nodeInfo.strTextureNames[0] = newname;
		if ( grid.nodeInfo.strTextureNames[1].compare(oldname) == 0 )
			grid.nodeInfo.strTextureNames[1] = newname;
		if ( grid.nodeInfo.strTextureNames[2].compare(oldname) == 0 )
			grid.nodeInfo.strTextureNames[2] = newname;

	}
}

int CTerrainMesh::ChangeSceneModelName(const string &oldname, string &newname)
{
	DWORD dwGridCnt = this->GetGridCount();
	int nReplaceCnt = 0;

	for ( DWORD i = 0; i < dwGridCnt; ++i )
	{
		SGrid &grid = this->GetGrid(i);

		for ( vector< CTObjectPtr >::iterator iter = grid.vecCenterObjects.begin(); iter != grid.vecCenterObjects.end(); ++iter)
		{
			CTObjectPtr p = (*iter);

			CMapEditObject * pModel = p->GetModelGroup()->GetSelfPresentation(0);
			string modelname = pModel->GetName();

			if ( modelname.find(oldname) != -1 )
			{
				p->GetModelGroup()->SetItemName(newname);
				++nReplaceCnt;
			}
		}
	}

	return nReplaceCnt;
}

DWORD CTerrainMesh::ToResizeGridIndex(DWORD dwWidthNew, DWORD dwDepthNew, DWORD dwGridIndex, const uint8 uResizeType)
{
	switch ( uResizeType )
	{
	case 0:
		{
			int nXMid = dwWidthNew/2 - m_dwWidthInRegion/2;
			int nZMid = dwDepthNew/2 - m_dwDepthInRegion/2;

			int nXDeltaBase = nXMid * REGION_SPACE;
			int nZDeltaBase = nZMid * REGION_SPACE;

			int nXDelta = dwGridIndex % this->GetWidth();
			int nZDelta = dwGridIndex / this->GetWidth();

			return (nZDeltaBase+nZDelta) * dwWidthNew * REGION_SPACE + nXDeltaBase + nXDelta;
		}
		break;

	default:
		break;
	}

	return 0;
}

float CTerrainMesh::GetGridTotalHeight(DWORD dwGridIndex)
{
	SGrid & grid = this->GetGrid(dwGridIndex);
	SVertex & v0 = this->GetVertex(grid.dwVertexIndex[0]);
	SVertex & v1 = this->GetVertex(grid.dwVertexIndex[1]);
	SVertex & v2  = this->GetVertex(grid.dwVertexIndex[2]);
	SVertex & v3  = this->GetVertex(grid.dwVertexIndex[3]);
	return grid.vCenter.y + (v0.fLogicHeight + v1.fLogicHeight + v2.fLogicHeight + v3.fLogicHeight) * 0.25f;
}

void CTerrainMesh::SetGridRiver(int nGridIndex, CRiver * pRiver)
{
	Ast(this->m_bWater);
	SGrid &gridW = this->GetGrid(nGridIndex);
	if (gridW.pRiver != pRiver)
	{
		int regionIndex = this->GetRegionIndexByGridIndex(nGridIndex);
		if (NULL != gridW.pRiver) 
			gridW.pRiver->AddDirtyRegion(regionIndex);
		if (NULL != pRiver) 
			pRiver->AddDirtyRegion(regionIndex);

		if( gridW.pWave )
		{
			int nBase		 = gridW.pWave->GetBase();
			CVector3f center = gridW.pWave->GetCenter();
			gridW.pRiver->RemoveWave(regionIndex, gridW.pWave);
			gridW.pWave = NULL;

			gridW.pWave = pRiver->AddWave(regionIndex);
			gridW.pWave->Create(nBase, center);
		}
	}
	gridW.pRiver = pRiver;
}

void CTerrainMesh::CopyGrids(DWORD dwGridIndexCenterDest, DWORD dwWidth, DWORD dwDepth, vector<int> & vGridIndicesSrc )
{
	float fRelativeBase = FLT_MAX;

	for (size_t i = 0; i < vGridIndicesSrc.size(); ++i )
	{
		int nGridIndexSrc = vGridIndicesSrc[i];

		if ( !this->IsValid(nGridIndexSrc) ) return;

		SGrid & gridSrc = this->GetGrid(nGridIndexSrc);

		fRelativeBase = min(gridSrc.vCenter.y,fRelativeBase);
	}

	int nDeltaX = dwGridIndexCenterDest % this->GetWidth();
	int nDeltaZ = dwGridIndexCenterDest / this->GetWidth();

	int nGridIndexStartDeltaX = nDeltaX;
	int nGridIndexStartDeltaZ = nDeltaZ;

	set< pair<int,int> > setPairSrcDest;

	for ( DWORD nDeltaZ = 0; nDeltaZ < dwDepth; ++nDeltaZ )
	{
		for ( DWORD nDeltaX = 0; nDeltaX < dwWidth; ++nDeltaX )
		{
			int nGridIndexDest = (nGridIndexStartDeltaZ+nDeltaZ) * this->GetWidth() + (nGridIndexStartDeltaX+nDeltaX);

			if ( !this->IsValid(nGridIndexDest) ) return;

			int nGridIndexSrc = vGridIndicesSrc[nDeltaZ*dwWidth + nDeltaX];

			if ( !this->IsValid(nGridIndexSrc) ) return;

			SGrid & gridDest = this->GetGrid(nGridIndexDest);
			SGrid & gridSrc = this->GetGrid(nGridIndexSrc);

			gridDest.nodeInfo.strTextureNames[0] = gridSrc.nodeInfo.strTextureNames[0];
			gridDest.nodeInfo.strTextureNames[1] = gridSrc.nodeInfo.strTextureNames[1];
			gridDest.nodeInfo.strTextureNames[2] = gridSrc.nodeInfo.strTextureNames[2];
			gridDest.nodeInfo.strTextureNames[3] = gridSrc.nodeInfo.strTextureNames[3];

			for (size_t i = 0; i < 4; ++i )
			{
				int nVertexIndexDest = gridDest.dwVertexIndex[i];
				int nVertexIndexSrc = gridSrc.dwVertexIndex[i];

				setPairSrcDest.insert(make_pair(nVertexIndexSrc,nVertexIndexDest));
			}
		}
	}

	for ( set< pair<int,int> >::iterator iter = setPairSrcDest.begin(); iter != setPairSrcDest.end(); ++iter )
	{
		SVertex & vertexDest = this->GetVertex((*iter).second);
		SVertex & vertexSrc = this->GetVertex((*iter).first);

		vertexDest.oColor = vertexSrc.oColor;
		vertexDest.vPosition.y = vertexSrc.vPosition.y;
	}

	for ( set< pair<int,int> >::iterator iter = setPairSrcDest.begin(); iter != setPairSrcDest.end(); ++iter )
	{
		int nVertexIndexDest = (*iter).second;
		CTerrainAlgorithm::GetInst()->CalculateVertexNormal(nVertexIndexDest);
		CTerrainAlgorithm::GetInst()->ReCalculateGridsHeight(nVertexIndexDest);
	}
}

void CTerrainMesh::RotateGrids( DWORD dwGridIndexCenterDest, DWORD dwWidth, DWORD dwDepth, vector<int> & vGridIndicesSrc, int nRot, int nAnchorPoint, byte nMode )
{
	if (nRot < 0)
	{
		MessageBox(0, "nRot不能小于0！", "操作失败", 0);
		return;
	}

	float fRelativeBase = FLT_MAX;

	if (!m_vecOldGrids.empty())
		m_vecOldGrids.clear();

	if (!m_vecTextureNames.empty())
		m_vecTextureNames.clear();

	for (size_t i = 0; i < vGridIndicesSrc.size(); ++i )
	{
		int nGridIndexSrc = vGridIndicesSrc[i];

		if ( !this->IsValid(nGridIndexSrc) ) return;

		SGrid & gridSrc = this->GetGrid(nGridIndexSrc);

		fRelativeBase = min(gridSrc.vCenter.y,fRelativeBase);
	}

	int nDeltaX = dwGridIndexCenterDest % this->GetWidth();
	int nDeltaZ = dwGridIndexCenterDest / this->GetWidth();

	int nGridIndexStartDeltaX, nGridIndexStartDeltaZ;

	set< pair<int,int> > setPairSrcDest;
	set<DWORD> bufferindexs;

	nRot = nRot % 4;
	if (nRot == 0) // 不旋转
	{
		m_nOldWidth = dwWidth;
		m_nOldDepth = dwDepth;
			
		nGridIndexStartDeltaX = nDeltaX;
		nGridIndexStartDeltaZ = nDeltaZ;

		for ( DWORD nDeltaZ = 0; nDeltaZ < dwDepth; ++nDeltaZ )
		{
			for ( DWORD nDeltaX = 0; nDeltaX < dwWidth; ++nDeltaX )
			{
				if ( nGridIndexStartDeltaX+(int)nDeltaX >= this->GetWidth() )
					continue;

				DWORD nGridIndexDest = (nGridIndexStartDeltaZ+nDeltaZ) * this->GetWidth() + (nGridIndexStartDeltaX+nDeltaX);

				if ( !this->IsValid(nGridIndexDest) ) return;

				int nGridIndexSrc = vGridIndicesSrc[nDeltaZ*dwWidth + nDeltaX];

				if ( !this->IsValid(nGridIndexSrc) ) return;

				SGrid & gridDest = this->GetGrid(nGridIndexDest);
				bufferindexs.insert(gridDest.dwBufferIndex);

				// Save the old grids
				SGrid gridOld = gridDest;
				m_vecOldGrids.push_back(gridOld);
				SdwTextureNames dwTextureHandle;
				dwTextureHandle.strTextureNames[0] = gridDest.nodeInfo.strTextureNames[0];
				dwTextureHandle.strTextureNames[1] = gridDest.nodeInfo.strTextureNames[1];
				dwTextureHandle.strTextureNames[2] = gridDest.nodeInfo.strTextureNames[2];
				dwTextureHandle.strTextureNames[3] = gridDest.nodeInfo.strTextureNames[3];


				m_vecTextureNames.push_back(dwTextureHandle);


				SVertex &v0 = this->GetVertex(gridDest.dwVertexIndex[0]);
				SVertex &v1 = this->GetVertex(gridDest.dwVertexIndex[1]);
				SVertex &v2 = this->GetVertex(gridDest.dwVertexIndex[2]);
				SVertex &v3 = this->GetVertex(gridDest.dwVertexIndex[3]);
	
				SGrid & gridSrc = this->GetGrid(nGridIndexSrc);

				//memcpy(gridDest.nodeInfo.strTextureNames, gridSrc.nodeInfo.strTextureNames, sizeof(string)*3);

				gridDest.nodeInfo.strTextureNames[0] = gridSrc.nodeInfo.strTextureNames[0];
				gridDest.nodeInfo.strTextureNames[1] = gridSrc.nodeInfo.strTextureNames[1];
				gridDest.nodeInfo.strTextureNames[2] = gridSrc.nodeInfo.strTextureNames[2];
				gridDest.nodeInfo.strTextureNames[3] = gridSrc.nodeInfo.strTextureNames[3];

				for (size_t i = 0; i < 4; ++i )
				{
					int nVertexIndexDest = gridDest.dwVertexIndex[i];
					int nVertexIndexSrc = gridSrc.dwVertexIndex[i];

					setPairSrcDest.insert(make_pair(nVertexIndexSrc,nVertexIndexDest));
				}
			}
		}

	}

	if (nMode == 0) // 正常粘贴地表
	{
		for ( set< pair<int,int> >::iterator iter = setPairSrcDest.begin(); iter != setPairSrcDest.end(); ++iter )
		{
			SVertex & vertexDest = this->GetVertex((*iter).second);
			SVertex & vertexSrc = this->GetVertex((*iter).first);

			vertexDest.oColor = vertexSrc.oColor;
			vertexDest.bColor = vertexSrc.oColor;
			vertexDest.sColor = vertexSrc.sColor;

			vertexDest.vPosition.y = vertexSrc.vPosition.y;
		}

	}

	for ( set< pair<int,int> >::iterator iter = setPairSrcDest.begin(); iter != setPairSrcDest.end(); ++iter )
	{
		int nVertexIndexDest = (*iter).second;
		CTerrainAlgorithm::GetInst()->CalculateVertexNormal(nVertexIndexDest);
		CTerrainAlgorithm::GetInst()->ReCalculateGridsHeight(nVertexIndexDest);
	}

	if ( !bufferindexs.empty() && !this->IsWater() )
	{
		this->ReWriteTerrainBuffer( bufferindexs, 0 );
		this->ReWriteTerrainBuffer( bufferindexs, 1 );
		this->ReWriteTerrainBuffer( bufferindexs, 2 );
		this->ReWriteTerrainBuffer( bufferindexs, 3 );
	}
}

int CTerrainMesh::GetRegionHot()
{
	int nGridIndexHot =	CMapEditApp::GetInst()->GetOperator()->GetGridIndexHot();

	if ( !this->IsValid(nGridIndexHot) )
		return INVALID;

	return this->GetRegionIndexByGridIndex(nGridIndexHot);
}

int CTerrainMesh::GetGroundTextureCnt()
{
	CRenderDevice * pRender = CRenderDevice::GetInst();
	return pRender->m_TerrainTexIndexs.size();
}

DWORD CTerrainMesh::GetVertexIndexHot()
{
	return this->GetGrid(CMapEditApp::GetInst()->GetOperator()->GetGridIndexHot()).dwVertexIndex[0];
}

SGrid & CTerrainMesh::GetGridHot()
{
	return this->GetGrid(CMapEditApp::GetInst()->GetOperator()->GetGridIndexHot());
}

SVertex & CTerrainMesh::GetVertexHot()
{
	SGrid & grid = this->GetGrid(CMapEditApp::GetInst()->GetOperator()->GetGridIndexHot());
	return this->GetVertex(grid.dwVertexIndex[0]);
}

//------------------------------------------------------------------------------
void CTerrainMesh::RenderRivers()
{
	for_each(m_vecRivers.begin(),m_vecRivers.end(),mem_fun(&CRiver::Render));
}

//------------------------------------------------------------------------------
void CTerrainMesh::UpdateRiverSideTexture()
{
	for_each(m_vecRivers.begin(),m_vecRivers.end(),mem_fun(&CRiver::UpdateWaveTexture));
}

//------------------------------------------------------------------------------
void CTerrainMesh::UpdateFPSLimited()
{
	for_each(m_vecRivers.begin(),m_vecRivers.end(),mem_fun(&CRiver::UpdateFPSLimited));
}

CTerrainMesh::PX::PX() : distance(0.0f), indexi(0), indexj(0), Dynamic(false), bSpotLight(false)
{

}

CTerrainMesh::PX::PX( float dis, int i, int j, bool dyn, bool spotlight )
{
	distance = dis;
	indexi = i;
	indexj = j;
	Dynamic = dyn;
	bSpotLight = spotlight;
}

SVertex & sqr::CTerrainMesh::GetVertex( DWORD dwVertexIndex )
{
	if ( !this->IsValidVertexIndex(dwVertexIndex) )
	{
		TCHAR szMessage[256];

		sprintf(szMessage, "%u顶点越界" , dwVertexIndex);
		MessageBox(NULL, szMessage, "提示", MB_OK | MB_ICONERROR);

		return m_pVertices[0];
	}

	return m_pVertices[dwVertexIndex];
}

STileRegion* sqr::CTerrainMesh::GetTileRegion( DWORD nTileRegionIndex )
{
	if( nTileRegionIndex < 0 || nTileRegionIndex >= m_dwTileRegionCnt)
	{
		TCHAR szMessage[256];

		sprintf(szMessage, "%d大区域越界" , nTileRegionIndex);

		MessageBox(NULL, szMessage, "提示", MB_OK | MB_ICONERROR);
		return NULL;
	}
	else
		return m_vecTileRegionList[nTileRegionIndex];
}

uint32 CTerrainMesh::GetTileCoordByGrid( uint32 nGridIdx )
{
	uint32 xTile = (nGridIdx % GetWidth()) / REGION_SPACE / 4;
	uint32 zTile = (nGridIdx / GetWidth()) / REGION_SPACE / 4;

	return ((xTile & 0xffff)<<16) | (zTile & 0xffff);
}
