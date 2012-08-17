#include "stdafx.h"
#include "ToolSetTileBlock.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "Operator.inl"

#include "Picker.h"
#include "CmdSetTileBlock.h"
#include "render.h"
#include "OperationLog.h"
#include "LocalVersion.h"
#include "MapGUIManager.h"

// 低障 1 show blue
// 中障 2 show green
// 高障 3 show red

namespace sqr
{

CToolSetTileBlock::CToolSetTileBlock()
{
	m_bShowBlock = false;
	m_pRS		 = NULL;
}

CToolSetTileBlock::~CToolSetTileBlock()
{
	if (m_pRS)
		m_pRS = NULL;
}

void CToolSetTileBlock::OnEvent(const CMsg& msg)
{
	CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	if( pTerrain->IsWater() )
		return;

	switch ( msg.msgSource )
	{
	case MGS_KEYMSG:
		{
			switch(msg.msgType.key_type)
			{
			case MGT_KEY_DOWN:
				switch(msg.msgInfo.key_info)
				{
					///删除阻挡
				case MGI_KEY_F3:
					{
						this->DeleteBlock();
						return;
					}
				
				case MGI_KEY_F1:
					this->SetExactGridIndex();
					break;

				default:
					break;
				}
			}
		}
		break;
	default:
		return;
	}
}

void CToolSetTileBlock::Render()
{
	switch (m_processParams.eEditBrushType)
	{
	case EBT_NORMAL:
		{
			this->RenderNormalBlockBrush();
		}
		break;

	case EBT_EXCAT:
		{
			this->RenderExactBlockBrush();
		}
		break;

	default:
		break;
	}
}

vector<int>& CToolSetTileBlock::GetBrushGridIndices()
{
	static vector<int> vecBrushGridIndices;

	vecBrushGridIndices.clear();

	CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

	DWORD BrushSizeInX = this->GetBrushSizeInX();
	DWORD BrushSizeInZ = this->GetBrushSizeInZ();
	int   nHotGridIndex = CMapEditApp::GetInst()->GetOperator()->GetGridIndexHot();
	int   width = pTerrain->GetWidth();

	if ( pTerrain->IsValid(nHotGridIndex) )
	{
		int nGridIndex = 0;

		for ( DWORD i = 0; i < BrushSizeInZ; ++i )
		{
			for ( DWORD j = 0; j < BrushSizeInX; ++j )
			{
				nGridIndex = nHotGridIndex + j + i * width;
				if ( pTerrain->IsValid(nGridIndex) )
					vecBrushGridIndices.push_back(nGridIndex);
			}
		}
	}

	return vecBrushGridIndices;
}

map<int, vector<int>> & CToolSetTileBlock::GetBrushSubGridIndices()
{
	static map<int, vector<int>> mapBrushGridIndices;
	static map<int, vector<int>>::iterator iter;

	mapBrushGridIndices.clear();

	if ( m_processParams.eEditBrushType == EBT_NORMAL )
		return mapBrushGridIndices;

	CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

	int BrushSizeInX	 = this->GetBrushSizeInX();
	int BrushSizeInZ	 = this->GetBrushSizeInZ();
	int nHotGridIndex	 = CMapEditApp::GetInst()->GetOperator()->GetGridIndexHot();
	int width			 = pTerrain->GetWidth();
	int nSubGridIndex	 = 0, nGridIndex = 0, nWhichGridSubIndex = 0;
	int nHotSubGridIndex = this->GetExactGridIndex(nHotGridIndex);

	for ( int i = 0; i < BrushSizeInZ; ++i )
	{
		for ( int j = 0; j < BrushSizeInX; ++j )
		{
			nSubGridIndex = nHotSubGridIndex + j + i * width * 2;

			nGridIndex = pTerrain->m_SubGridIndexs[nSubGridIndex];
			SGrid &grid = pTerrain->GetGrid(nGridIndex);

			nWhichGridSubIndex = grid.subGridIndex[nSubGridIndex];

			iter = mapBrushGridIndices.find(nGridIndex);
			if ( iter == mapBrushGridIndices.end() )
			{
				vector<int> sublist;
				sublist.push_back(nWhichGridSubIndex);

				mapBrushGridIndices[nGridIndex] = sublist;
			}
			else
			{
				iter->second.push_back(nWhichGridSubIndex);
			}
		}
	}

	return mapBrushGridIndices;
}

bool CToolSetTileBlock::Save( const string& path )
{
	CTerrainMesh* pTerrain = NULL;
	pTerrain =CMapEditApp::GetInst()->GetTerrain();
	if( pTerrain->IsWater() )
	{
		MessageBox(NULL,"将当前状态改成地形状态，而非水面层状态，否则不能保存地图阻挡信息","提示",MB_OK);
		return false;
	}

	string strFileName = path;
	locale loc;
	use_facet<ctype<TCHAR> >( loc ).tolower
		( &strFileName[0], &strFileName[strFileName.length()] );

	size_t npos = strFileName.rfind(".");
	if( npos != -1 )
		strFileName = strFileName.substr(0, npos);
	
	strFileName += ".block";

	FILE * fp = NULL;
	fp = fopen(strFileName.c_str(),"wb");
	if (!fp)
	{
		string log = strFileName + "无法保存";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
		return false;
	}

	int nGridCnt = pTerrain->GetGridCount();

	//////////////////////////////////////////////////////////////////////////
	///version
	DWORD dwVersion = BLOCK_VERSION;
	fwrite(&dwVersion, sizeof(dwVersion), 1, fp);

	//////////////////////////////////////////////////////////////////////////
	for ( int i  = 0; i < nGridCnt; ++i )
	{
		SGrid &grid = pTerrain->GetGrid(i);

		fwrite(&grid.nBlockType, sizeof(int), 1, fp);
	}
	//////////////////////////////////////////////////////////////////////////

	fclose(fp);

	return true;
}

bool CToolSetTileBlock::ReSizeSave( const string& path, int nWidth, int nDepth )
{
	COperator *pOperator   = CMapEditApp::GetInst()->GetOperator();
	CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	if( pTerrain->IsWater() )
	{
		MessageBox(NULL,"将当前状态改成地形状态，而非水面层状态，否则不能保存地图阻挡信息","提示",MB_OK);
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	//保存扩展备份文件
	string strFileName = path;
	locale loc;
	use_facet<ctype<TCHAR> >( loc ).tolower
		( &strFileName[0], &strFileName[strFileName.length()] );

	size_t npos = strFileName.rfind(".");
	if( npos != -1 )
		strFileName = strFileName.substr(0, npos);

	string strBackUpName = strFileName + "_扩展备份.block";
	this->Save(strBackUpName);
	//////////////////////////////////////////////////////////////////////////

	
	strFileName += ".block";
	FILE * fp = NULL;
	fp = fopen(strFileName.c_str(),"wb");

	if (!fp)
	{
		string log = strFileName + "无法保存";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
		return false;
	}

	DWORD dwMapWidth = nWidth * REGION_SPACE;
	DWORD dwMapDepth = nDepth * REGION_SPACE;
	DWORD newGridCnt = dwMapWidth * dwMapDepth;

	///扩展后旧地图格子索引在新地图中的新索引映射 first new grid   index second old grid index
	map<int, int> m_NewMapGridIndexMap;
	map<int, int>::iterator beg, end = pOperator->m_OldMapGridIndexMap.end();
	for( beg = pOperator->m_OldMapGridIndexMap.begin(); beg != end; ++beg )
		m_NewMapGridIndexMap[beg->second] = beg->first;

	//////////////////////////////////////////////////////////////////////////
	///version
	int   nNoBlock  = 0;
	DWORD dwVersion = BLOCK_VERSION;

	fwrite(&dwVersion, sizeof(dwVersion), 1, fp);
	int nGridIndex = 0;
	for ( DWORD i  = 0; i < newGridCnt; ++i )
	{
		beg = m_NewMapGridIndexMap.find(i);
		if( beg != m_NewMapGridIndexMap.end() )
		{
			nGridIndex = beg->second;
			SGrid &grid = pTerrain->GetGrid(nGridIndex);

			fwrite(&grid.nBlockType, sizeof(int), 1, fp);
		}
		else
			fwrite(&nNoBlock, sizeof(int), 1, fp);
	}
	//////////////////////////////////////////////////////////////////////////
	fclose(fp);

	return true;
}

void CToolSetTileBlock::Load( const string& path )
{
	CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetTerrain();

	//////////////////////////////////////////////////////////////////////////
	string strFileName = path;
	locale loc;
	use_facet<ctype<TCHAR> >( loc ).tolower
		( &strFileName[0], &strFileName[strFileName.length()] );

	size_t npos = strFileName.rfind(".");
	if( npos != -1 )
		strFileName = strFileName.substr(0, npos);

	string strLastBlockName = strFileName + "_last.block";
	strFileName += ".block";

	FILE * fp = NULL;
	fp = fopen(strFileName.c_str(),"rb");
	if (! fp )
	{
		string log = strFileName + "阻挡文件无法打开，查看是否被删掉了";

		MessageBox(NULL,log.c_str(),"请换其他地图",MB_OK);
		return;
	}

	::CopyFile(strFileName.c_str(), strLastBlockName.c_str(), FALSE);

	//////////////////////////////////////////////////////////////////////////
	///version
	DWORD dwVersion = BLOCK_VERSION;
	fread(&dwVersion,sizeof(dwVersion),1,fp);

	switch (dwVersion)
	{
	case BLOCK_TEX_INFO:
		{
			///读取设置障纹理名
			int8 nBlockTpyeCnt = EBT_CNT - 1;
			int  nBlockTpye = -1;
			for ( int8 i = 0; i < nBlockTpyeCnt; ++i )
			{
				nBlockTpye = i + 1;
				size_t size = 0;

				fread(&size,sizeof(size),1,fp);
				if( size != 0 )
				{
					string name("");
					int nLen = 0;

					for ( size_t i = 0; i < size; ++i )
					{	
						fread(&nLen,sizeof(nLen),1,fp);
						name.resize(nLen);
						fread((TCHAR*)name.data(),nLen,1,fp);
					}
				
				}
			}
		}
	case BLOCK_VERSION:
		{
			//////////////////////////////////////////////////////////////////////////
			///grid
			int nGridCnt = pTerrain->GetGridCount();

			for ( int i  = 0; i < nGridCnt; ++i )
			{
				int nBlockType = 0;
				fread(&nBlockType,sizeof(nBlockType),1,fp);

				SGrid &grid = pTerrain->GetGrid(i);
				grid.nBlockType = nBlockType;
			}
			//////////////////////////////////////////////////////////////////////////
		}
	default:
		break;
	}

	fclose(fp);
}

void CToolSetTileBlock::DeleteBlock()
{
	CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	if( pTerrain->IsWater() )
		return;

	COpearationLog::GetInst()->WriteOperationLog("Delete删除阻挡");

	set<DWORD> bufferindexs;
	int nGridIndex = 0;
	vector<int> & vecBrushGridIndices = this->GetBrushGridIndices();
	size_t size = vecBrushGridIndices.size();
	for( size_t i = 0; i < size; ++i )
	{
		nGridIndex = vecBrushGridIndices[i];
		SGrid &grid = pTerrain->GetGrid(nGridIndex);
		grid.nBlockType = 0;

		bufferindexs.insert(grid.dwBufferIndex);
	}

	if ( !bufferindexs.empty() )
	{
		pTerrain->ReWriteTerrainBuffer( bufferindexs, 9 );
	}
}

void CToolSetTileBlock::DeleteSceneBlocks(const int nType)
{
	CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	if( pTerrain->IsWater() )
		return;

	char szObjectBuf[256];
	sprintf(szObjectBuf, "DO Delete Scene all block  %d", nType);
	COpearationLog::GetInst()->WriteOperationLog(szObjectBuf);

	int nGridCnt = pTerrain->GetGridCount();
	for ( int i = 0; i < nGridCnt; ++i )
	{
		SGrid & grid = pTerrain->GetGrid(i);

		if( grid.nBlockType == nType )
			grid.nBlockType = 0;
	}

	CMapGUIManager::GetInst()->RefreshMiniMap(1);
}

int CToolSetTileBlock::GetExactGridIndex(const int nGridIndex)
{
	CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

	SGrid &hotgrid = pTerrain->GetGrid(nGridIndex);
	CVector3f girdCenter = hotgrid.vCenter;

	CTObject * p = NULL;
	CVector3f v = CVector3f(0.0f, 0.0f, 0.0f);
	DWORD dwGridIndexHot = CPicker::GetInst()->GetPickPos(&p,&v);
	int nWhichSubIndex = 0;

	if ( v.x < girdCenter.x )
	{
		if ( v.z < girdCenter.z )
			nWhichSubIndex = 0;//0;
		else
			nWhichSubIndex = 2;//2;
	}
	else
	{
		if ( v.z < girdCenter.z )
			nWhichSubIndex = 1;//1;
		else
			nWhichSubIndex = 3;//3;
	}

	map<int, int>::iterator iter = hotgrid.subGridIndex.begin();

	for ( int i = 0 ; i < nWhichSubIndex; ++i )
		++iter;

	return iter->first;
}

void CToolSetTileBlock::SetExactGridIndex()
{
	if ( m_processParams.eEditBrushType == EBT_NORMAL )
		return;

	map<int, vector<int>> & vecVertexIndices = this->GetBrushSubGridIndices();
	map<int, vector<int>>::iterator beg = vecVertexIndices.begin();
	map<int, vector<int>>::iterator end = vecVertexIndices.end();
	CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

	int nGridIndex = 0, nSubGridIndex = 0, nBlockType = 0;
	size_t size = 0;
	DWORD drawnCnt = 0;
	CVector3f center = CVector3f(0.0f, 0.0f, 0.0f);
	float step = GRID_SPACE * 0.5f;
	vector<int> subIndexs;
	bool bSetExactBlock = false;

	if ( m_processParams.eEditType == EET_WRITE )
	{
		nBlockType = m_processParams.eBlockTypeHot;
		bSetExactBlock = true;
	}
	else
	{
		nBlockType = 0;
		bSetExactBlock = false;
	}

	char szBlockBuf[64];
	sprintf(szBlockBuf, "F1操作，设置精确阻挡：阻挡类型是 = %d", nBlockType);
	COpearationLog::GetInst()->WriteOperationLog(szBlockBuf);

	for ( beg; beg != end; ++ beg )
	{
		nGridIndex = beg->first;
		size = beg->second.size();
		SGrid &grid = pTerrain->GetGrid(nGridIndex);

		for ( size_t i = 0; i < size; ++i )
		{
			grid.subGridTypes[beg->second[i]] = nBlockType;
		}

		grid.m_bSetExactBlock = bSetExactBlock;
	}
}

void CToolSetTileBlock::SetTerrainBlock()
{
	CCmdSetTileBlock * pCmd = new CCmdSetTileBlock;
	pCmd->SetProcessParams(m_processParams);

	vector<int> & vecBrushGridIndices = this->GetBrushGridIndices();
	size_t size = vecBrushGridIndices.size();
	for( size_t i = 0; i < size; ++i )
		pCmd->AddGrid(vecBrushGridIndices[i]);

	pCmd->Execute();
}

void CToolSetTileBlock::RenderNormalBlockBrush()
{
	CRenderDevice * pRender = CRenderDevice::GetInst();
	CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

	// for brush
	static CVector3f vertex[4];
	static CVector2f uv0[4];
	static VerColorTex rvertex[VERTEX_BATCH_SIZE];
	static WORD rvertexIndex[VERTEX_BATCH_SIZE];
	DWORD color[4];
	color[0] = color[1] = color[2] = color[3] = 0xffffffff;

	vector<int> & vecBrushGridIndices = this->GetBrushGridIndices();
	if ( vecBrushGridIndices.empty() )
		return;

	size_t cnt = vecBrushGridIndices.size();
	DWORD drawnCnt = 0, n = 0;
	for ( n, drawnCnt; n < cnt; n++, drawnCnt++ )
	{
		if ( drawnCnt && !((drawnCnt) % GRID_BATCH_SIZE) )
		{
			pRender->DrawLayer3DWireQuad(&m_pRS, rvertex, GRID_BATCH_SIZE);
			this->SetBrushRS();
			drawnCnt = 0;
		}

		SGrid & s = pTerrain->GetGrid(vecBrushGridIndices[n]);

		vertex[0] = CMapEditApp::GetInst()->GetEditingMesh()->GetVertex(s.dwVertexIndex[0]).vPosition;
		vertex[1] = CMapEditApp::GetInst()->GetEditingMesh()->GetVertex(s.dwVertexIndex[3]).vPosition;
		vertex[2] = CMapEditApp::GetInst()->GetEditingMesh()->GetVertex(s.dwVertexIndex[2]).vPosition;
		vertex[3] = CMapEditApp::GetInst()->GetEditingMesh()->GetVertex(s.dwVertexIndex[1]).vPosition;

		rvertex[4 * drawnCnt + 0] = VerColorTex(vertex[0].x, vertex[0].y + 1.5f, vertex[0].z + 1.5f, color[0], 0.0f, 1.0f);
		rvertex[4 * drawnCnt + 1] = VerColorTex(vertex[1].x, vertex[1].y + 1.5f, vertex[1].z + 1.5f, color[1], 0.0f, 0.0f);
		rvertex[4 * drawnCnt + 2] = VerColorTex(vertex[2].x, vertex[2].y + 1.5f, vertex[2].z + 1.5f, color[2], 1.0f, 1.0f);
		rvertex[4 * drawnCnt + 3] = VerColorTex(vertex[3].x, vertex[3].y + 1.5f, vertex[3].z + 1.5f, color[3], 0.0f, 0.0f);

		rvertexIndex[ drawnCnt*6 + 0 ] = WORD( drawnCnt<<2 ) + 0;
		rvertexIndex[ drawnCnt*6 + 1 ] = WORD( drawnCnt<<2 ) + 1;
		rvertexIndex[ drawnCnt*6 + 2 ] = WORD( drawnCnt<<2 ) + 2;
		rvertexIndex[ drawnCnt*6 + 3 ] = WORD( drawnCnt<<2 ) + 3;
		rvertexIndex[ drawnCnt*6 + 4 ] = WORD( drawnCnt<<2 ) + 0;
		rvertexIndex[ drawnCnt*6 + 5 ] = WORD( drawnCnt<<2 ) + 2;
	}
	if ( drawnCnt > 0 )
	{
		pRender->DrawLayer3DWireQuad(&m_pRS, rvertex, rvertexIndex, drawnCnt);
		this->SetBrushRS();
	}
}

void CToolSetTileBlock::RenderExactBlockBrush()
{
	CRenderDevice * pRender = CRenderDevice::GetInst();
	CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

	// for brush
	static CVector3f vertex[4];
	static CVector2f uv0[4];
	static VerColorTex rvertex[VERTEX_BATCH_SIZE];
	static WORD rvertexIndex[VERTEX_BATCH_SIZE];
	DWORD vecColor[4];
	vecColor[0] = vecColor[1] = vecColor[2] = vecColor[3] =0xffffffff;

	map<int, vector<int>> & vecVertexIndices = this->GetBrushSubGridIndices();
	map<int, vector<int>>::iterator beg = vecVertexIndices.begin();
	map<int, vector<int>>::iterator end = vecVertexIndices.end();

	if ( vecVertexIndices.empty() )
		return;

	int nGridIndex = 0, nSubGridIndex = 0;
	size_t size = 0;
	DWORD drawnCnt = 0;
	CVector3f center(0.0f, 0.0f, 0.0f);
	float step = GRID_SPACE * 0.5f;

	for ( beg; beg != end; ++ beg )
	{
		nGridIndex = beg->first;
		size = beg->second.size();
		SGrid &grid = pTerrain->GetGrid(nGridIndex);
		center = grid.vCenter;

		for ( size_t i = 0; i < size; ++i )
		{
			nSubGridIndex = beg->second[i];

			switch ( nSubGridIndex)
			{
			case 0:
				{
					vertex[0] = center + CVector3f(-step, 0.01f, -step);
					vertex[1] = center + CVector3f(-step, 0.01f, 0.0f);
					vertex[2] = center + CVector3f(0.0f,  0.01f, -step);
					vertex[3] = center + CVector3f(0.0f,  0.01f, 0.0f);
				}
				break;

			case 1:
				{
					vertex[0] = center + CVector3f(0.0f, 0.01f, -step);
					vertex[1] = center + CVector3f(0.0f, 0.01f, 0.0f);
					vertex[2] = center + CVector3f(step, 0.01f, -step);
					vertex[3] = center + CVector3f(step, 0.01f, 0.0f);
				}
				break;

			case 2:
				{
					vertex[0] = center + CVector3f(-step, 0.01f, 0.0f);
					vertex[1] = center + CVector3f(-step, 0.01f, step);
					vertex[2] = center + CVector3f(0.0f,  0.01f, 0.0f);
					vertex[3] = center + CVector3f(0.0f,  0.01f, step);
				}
				break;

			case 3:
				{
					vertex[0] = center + CVector3f(0.0f, 0.01f, 0.0f);
					vertex[1] = center + CVector3f(0.0f, 0.01f, step);
					vertex[2] = center + CVector3f(step, 0.01f, 0.0f);
					vertex[3] = center + CVector3f(step, 0.01f, step);
				}
				break;

			default:
				break;
			}

			rvertex[6 * drawnCnt + 0] = VerColorTex(vertex[0].x, vertex[0].y, vertex[0].z, vecColor[0], 0.0f, 1.0f);
			rvertex[6 * drawnCnt + 1] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, vecColor[1], 0.0f, 0.0f);
			rvertex[6 * drawnCnt + 2] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, vecColor[2], 1.0f, 1.0f);

			rvertex[6 * drawnCnt + 3] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, vecColor[1], 0.0f, 0.0f);
			rvertex[6 * drawnCnt + 4] = VerColorTex(vertex[3].x, vertex[3].y, vertex[3].z, vecColor[3], 1.0f, 0.0f);
			rvertex[6 * drawnCnt + 5] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, vecColor[2], 1.0f, 1.0f);

			++drawnCnt;
		}
	}

	if ( drawnCnt > 0 )
	{
		pRender->DrawLayer3DWireQuad(&m_pRS, rvertex, drawnCnt);
		this->SetBrushRS();
	}
}

void CToolSetTileBlock::SetBrushRS()
{
	m_pRS->m_TexOP_S0 = TOP_MODULATE;
	m_pRS->m_TexArg1_S0 = TA_TEXTURE;
	m_pRS->m_TexArg2_S0 = TA_DIFFUSE;

	m_pRS->m_AlpOP_S0 = TOP_SELECTARG1;
	m_pRS->m_AlpArg1_S0 = TA_TEXTURE;

	m_pRS->m_AlphaTestEnable = true;
	m_pRS->m_Alpharef = 0x40;
	m_pRS->m_AlphaTestFun = CMP_GREATER;
	m_pRS->m_AlphaBlendEnable = false;
	m_pRS->m_LightEnable = false;
	m_pRS->m_ZTestEnable = false;
	m_pRS->m_ZWrite = true;
	m_pRS->m_Cull = CULLT_CCW;
}

void CToolSetTileBlock::DeleteTerrainAllBlocks()
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	DWORD dwGridCnt = pTerrain->GetGridCount();
	for( DWORD i = 0; i < dwGridCnt; ++i )
	{
		SGrid &grid = pTerrain->GetGrid(i);
		grid.nBlockType = 0;
	}

	COpearationLog::GetInst()->WriteOperationLog("删除所有阻挡操作");
	MessageBox(NULL, "删除场景所有阻挡信息完毕!", "提示", MB_OK);
}
}
