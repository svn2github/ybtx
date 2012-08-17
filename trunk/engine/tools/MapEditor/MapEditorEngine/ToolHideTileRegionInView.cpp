#include "stdafx.h"
#include "ToolHideTileRegionInView.h"
#include "CTexture.h"
#include "UserMsg.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "Render.h"
#include "Operator.h"
#include "Operator.inl"
#include "CLogicBursh.h"
#include "BaseHelper.h"
#include "CEventState.h"
#include "IoSystem.h"
#include "CTerrainMgr.h"

namespace sqr
{
	const int nGridCountInRenderTileRegion  = CTerrainMgr::MAP_SIZE * CTerrainMgr::MAP_SIZE;

	CToolHideTileRegionInView::CToolHideTileRegionInView()
	{
		m_pLogicBrush			 = NULL;
		m_pTileRegionLockTexture = NULL;
		m_pTileRegionSetTexture	 = NULL;
		m_pRS					 = NULL;
		m_bShow					 = true;
	}

	CToolHideTileRegionInView::~CToolHideTileRegionInView()
	{
		m_invisibleTileRegionIndexs.clear();

		if( m_pLogicBrush )
			SafeDelete(m_pLogicBrush);

		SafeRelease(m_pTileRegionLockTexture);
		SafeDelete(m_pTileRegionSetTexture);
	}

	void CToolHideTileRegionInView::OnEvent(const CMsg& msg)
	{	
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetTerrain();
		switch ( msg.msgSource )
		{
		case MGS_KEYMSG:
			{
				switch(msg.msgType.key_type)
				{
				case MGT_KEY_DOWN:
					{
						switch(msg.msgInfo.key_info)
						{
						case MGI_KEY_F3:
							{
								this->OperatorAddTileRegionIndex();
							}
							return;

						default:
							break;
						}
					}
	
				default:
					return;
				}
			}

		case MGS_MOUSEMSG:
			{
				switch(msg.msgType.mouse_type)
				{
				case MGT_MOUSE_MOVE:
					CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
					COperator *pOperator   = CMapEditApp::GetInst()->GetOperator();
					int nHotGrid			= pOperator->GetGridIndexHot();
					SGrid &grid				= pTerrain->GetGrid(nHotGrid);
					int nHotTileRegionIndex = grid.dwBufferIndex;
					::SendMessage(m_hTileRegionView,WM_TILEREGIONVIEW_MARK, 0,(LPARAM)&nHotTileRegionIndex);
					break;
				}

				default:
					break;
			}
		}
	}

	void CToolHideTileRegionInView::Render()
	{
		if( m_bShow )
		{
			invisiblePool::iterator beg = m_invisibleTileRegionIndexs.begin();
			invisiblePool::iterator end = m_invisibleTileRegionIndexs.end();
			for ( beg; beg!= end; ++beg )
			{
				int nTileRegionIndex = beg->first;
				int nSubTileRegionIndex = -1;
				const subRegIndexPool subIndexs = beg->second;
				for ( subRegIndexPool::const_iterator iter = subIndexs.begin(); iter != subIndexs.end(); ++ iter )
				{
					nSubTileRegionIndex = *iter;
					this->RenderOneTileRegion(nTileRegionIndex, nSubTileRegionIndex, m_pTileRegionSetTexture);
				}
			}
		}

		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		COperator *pOperator   = CMapEditApp::GetInst()->GetOperator();
		int nHotGrid			= pOperator->GetGridIndexHot();
		SGrid &grid				= pTerrain->GetGrid(nHotGrid);
		int nHotTileRegionIndex = grid.dwBufferIndex;
		int nSubTileRegionIndex = grid.bSubRenderUnitIndex;
		this->RenderOneTileRegion(nHotTileRegionIndex, nSubTileRegionIndex, m_pTileRegionLockTexture);
	}

	void CToolHideTileRegionInView::Initialize(HWND hTileRegionView)
	{
		m_hTileRegionView = hTileRegionView;

		if( m_pLogicBrush == NULL )
			m_pLogicBrush = new CLogicBrush;
		m_pLogicBrush->InitBrushParams(1, 1, 0x00ffffff, "home:res\\DefaultBrush.dds");

		string config = "MEditorRes:DefaultLock.dds";
		string strTexture = IoSystem::GetInst()->GetFullPath(config);
		m_pTileRegionLockTexture = CRenderDevice::GetInst()->GetTexture(strTexture);

		config = "MEditorRes:white.dds";
		strTexture = IoSystem::GetInst()->GetFullPath(config);
		m_pTileRegionSetTexture = CRenderDevice::GetInst()->GetTexture(strTexture);
		
	}

	void CToolHideTileRegionInView::IncBrushSizeInX() 
	{ 
		m_pLogicBrush->IncBrushSizeInX();
	}

	void CToolHideTileRegionInView::DecBrushSizeInX() 
	{ 
		m_pLogicBrush->DecBrushSizeInX();
	}

	void CToolHideTileRegionInView::IncBrushSizeInZ() 
	{ 
		m_pLogicBrush->IncBrushSizeInZ();
	}

	void CToolHideTileRegionInView::DecBrushSizeInZ() 
	{ 
		m_pLogicBrush->DecBrushSizeInZ();
	}

	void CToolHideTileRegionInView::RenderOneTileRegion( const int nTileRegionIndex, const int nSubTileRegionIndex, ITexture *tex )
	{
		CTerrainMesh *pTerrain		= CMapEditApp::GetInst()->GetEditingMesh();
		STileRegion  *pTileregion	= pTerrain->GetTileRegion(nTileRegionIndex);
		int ntest					= CTerrainMgr::MAP_SIZE;

		STileRenderRegionUnit *pRenderRegionUnit = pTileregion->vRenderRegionUnits[nSubTileRegionIndex];
		SGrid &leftbottomgrid		= pTerrain->GetGrid(pRenderRegionUnit->vGridIndices[0]);
		SGrid &rightbottomgrid		= pTerrain->GetGrid(pRenderRegionUnit->vGridIndices[CTerrainMgr::MAP_SIZE - 1]);
		SGrid &lefttopgrid			= pTerrain->GetGrid(pRenderRegionUnit->vGridIndices[nGridCountInRenderTileRegion - CTerrainMgr::MAP_SIZE]);
		SGrid &righttopgrid			= pTerrain->GetGrid(pRenderRegionUnit->vGridIndices[nGridCountInRenderTileRegion - 1]);


		static CVector3f	vertex[4];
		static CVector2f	uv0[4];
		static VerColorTex	rvertex[6];
		const SVertex& leftbottom = pTerrain->GetVertex(leftbottomgrid.dwVertexIndex[0]);
		const SVertex& lefttop    = pTerrain->GetVertex(lefttopgrid.dwVertexIndex[3]);
		const SVertex& righttop   = pTerrain->GetVertex(righttopgrid.dwVertexIndex[2]);
		const SVertex& rightbottom= pTerrain->GetVertex(rightbottomgrid.dwVertexIndex[1]);

		vertex[0] = leftbottom.vPosition;
		uv0[0] = CVector2f(0.0f, 1.0f);

		vertex[1] = lefttop.vPosition;
		uv0[1] = CVector2f(0.0f, 0.0f);

		vertex[2] = rightbottom.vPosition;
		uv0[2] = CVector2f(1.0f, 1.0f);

		vertex[3] = righttop.vPosition;
		uv0[3] = CVector2f(1.0f,0.0f);

		rvertex[0] = VerColorTex(vertex[0].x, vertex[0].y, vertex[0].z, 0x00ffffff, uv0[0].x, uv0[0].y);
		rvertex[1] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, 0x00ffffff, uv0[1].x, uv0[1].y);
		rvertex[2] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, 0x00ffffff, uv0[2].x, uv0[2].y);

		rvertex[3] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, 0x00ffffff, uv0[1].x, uv0[1].y);
		rvertex[4] = VerColorTex(vertex[3].x, vertex[3].y, vertex[3].z, 0x00ffffff, uv0[3].x, uv0[3].y);
		rvertex[5] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, 0x00ffffff, uv0[2].x, uv0[2].y);

		CRenderDevice::GetInst()->DrawLayer3DSolidQuad(&m_pRS, rvertex, tex, 1);
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
		m_pRS->m_ZTestEnable = true;
		m_pRS->m_ZWrite = true;
		m_pRS->m_Cull = CULLT_CCW;
	}

	void CToolHideTileRegionInView::OperatorAddTileRegionIndex()
	{
		COperator *pOperator	 = CMapEditApp::GetInst()->GetOperator();
		int nHotGrid			 = pOperator->GetGridIndexHot();
		CTerrainMesh *pTerrain	 = CMapEditApp::GetInst()->GetEditingMesh();
		const SGrid &grid		 = pTerrain->GetGrid(nHotGrid);
		int nHotTileRegionIndex  = grid.dwBufferIndex;
		STileRegion *pTileregion = pTerrain->GetTileRegion(nHotTileRegionIndex);
		if( pTileregion )
		{
			m_invisibleTileRegionIndexs[nHotTileRegionIndex].insert(grid.bSubRenderUnitIndex);

			STileRenderRegionUnit *pRenderRegionUnit = pTileregion->vRenderRegionUnits[grid.bSubRenderUnitIndex];
			pRenderRegionUnit->bIsVisibleInView = false;
			pTerrain->SetTerrainNodeVisbileMark(nHotTileRegionIndex, grid.bSubRenderUnitIndex, false);
		}
		::SendMessage(m_hTileRegionView, WM_TILEREGIONVIEW_REFRESH,0,0);
	}

	void CToolHideTileRegionInView::OperatorDeleteTileRegion()
	{
		CTerrainMesh *pTerrain		= CMapEditApp::GetInst()->GetEditingMesh();
		COperator *pOperator		= CMapEditApp::GetInst()->GetOperator();
		int nHotGrid				= pOperator->GetGridIndexHot();
		const SGrid &grid			= pTerrain->GetGrid(nHotGrid);
		STileRegion  *pTileregion	= pTerrain->GetTileRegion(grid.dwBufferIndex);
		int nHotTileRegionIndex		= grid.dwBufferIndex;
		if( pTileregion )
		{
			invisiblePool::iterator iter = m_invisibleTileRegionIndexs.find(nHotTileRegionIndex);
			if ( iter != m_invisibleTileRegionIndexs.end() )
			{
				int nTileRegionIndex = iter->first;
				int nSubTileRegionIndex = -1;
				const subRegIndexPool subIndexs = iter->second;
				iter->second.erase(grid.bSubRenderUnitIndex);

				if( iter->second.empty() )
					m_invisibleTileRegionIndexs.erase(iter);
			}

			STileRenderRegionUnit *pRenderRegionUnit = pTileregion->vRenderRegionUnits[grid.bSubRenderUnitIndex];
			pRenderRegionUnit->bIsVisibleInView = true;
			pTerrain->SetTerrainNodeVisbileMark(nHotTileRegionIndex, grid.bSubRenderUnitIndex, true);
		}

		::SendMessage(m_hTileRegionView, WM_TILEREGIONVIEW_REFRESH,0,0);
	}

	void CToolHideTileRegionInView::GotoSelectTileRegion( const int nTileRegionIndex )
	{
		CTerrainMesh *pTerrain		= CMapEditApp::GetInst()->GetEditingMesh();
		STileRegion  *pTileregion	= pTerrain->GetTileRegion(nTileRegionIndex);
		if( pTileregion == NULL )
			return;

		for ( uint n = 0; n < CTerrainMesh::Tile_PerSqr; ++n )
		{
			STileRenderRegionUnit *pRenderRegionUnit = pTileregion->vRenderRegionUnits[n];
			if( pRenderRegionUnit->bIsVisibleInView )
				continue;

			size_t nGridCountInTileRegion  = pRenderRegionUnit->vGridIndices.size();;
			int    nCenterGrid			   = nGridCountInTileRegion / 2;

			DWORD dwGridIndex			   = pRenderRegionUnit->vGridIndices[nCenterGrid];
			COperator *pOperator		   =  CMapEditApp::GetInst()->GetOperator();
			int nX = 0, nZ = 0;
			pOperator->GetMapCoordinate(dwGridIndex, nX, nZ);
			pOperator ->MoveTo(nX,nZ);
		}
	}

	void CToolHideTileRegionInView::SetIsShow( bool bShow )
	{
		m_bShow = bShow;
	}

	CToolHideTileRegionInView::invisiblePool CToolHideTileRegionInView::GetTileRegionSetIndexs() const
	{
		return m_invisibleTileRegionIndexs;
	}

	void CToolHideTileRegionInView::SaveTileRegionInfo( FILE * fp )
	{
		size_t size = m_invisibleTileRegionIndexs.size();
		fwrite(&size, sizeof(size), 1, fp);

		if( size > 0 )
		{
			invisiblePool::iterator beg = m_invisibleTileRegionIndexs.begin();
			invisiblePool::iterator end = m_invisibleTileRegionIndexs.end();
			for ( beg; beg!= end; ++beg )
			{
				int nTileRegionIndex = beg->first;
				fwrite(&nTileRegionIndex, sizeof(nTileRegionIndex), 1, fp);

				int8 nSubTileRegionIndex = -1;
				const subRegIndexPool subIndexs = beg->second;
				size_t subSize = subIndexs.size();
				fwrite(&subSize, sizeof(subSize), 1, fp);

				for ( subRegIndexPool::const_iterator iter = subIndexs.begin(); iter != subIndexs.end(); ++ iter )
				{
					nSubTileRegionIndex = *iter;
					fwrite(&nSubTileRegionIndex, sizeof(nSubTileRegionIndex), 1, fp);
				}
			}
		}
	}

	void CToolHideTileRegionInView::SaveClientTileRegionInfo( FILE * fp )
	{
		size_t size = m_invisibleTileRegionIndexs.size();
		fwrite(&size, sizeof(size), 1, fp);

		if( size > 0 )
		{
			CTerrainMesh *pTerrain	  = CMapEditApp::GetInst()->GetEditingMesh();
			invisiblePool::iterator beg = m_invisibleTileRegionIndexs.begin();
			invisiblePool::iterator end = m_invisibleTileRegionIndexs.end();
			for ( beg; beg!= end; ++beg )
			{
				int nTileRegionIndex = beg->first;
				fwrite(&nTileRegionIndex, sizeof(nTileRegionIndex), 1, fp);

				vector<bool> visibles;
				visibles.resize(CTerrainMesh::Tile_PerSqr);
				pTerrain->GetTerrainNodeVisbileMark(nTileRegionIndex, visibles);
				for ( uint m = 0; m < CTerrainMesh::Tile_PerSqr; ++m )
				{
					bool bVisible = visibles[m];
					fwrite(&bVisible, sizeof(bVisible), 1, fp);
				}
			}
		}
	}

	void CToolHideTileRegionInView::LoadTileRegionInfo( FILE * fp )
	{
		size_t size = 0;
		fread(&size, sizeof(size), 1, fp);
		
		CTerrainMesh *pTerrain	  = CMapEditApp::GetInst()->GetEditingMesh();

		int nTileRegionIndex = -1;
		for ( size_t i = 0; i < size; ++i )
		{
			fread(&nTileRegionIndex, sizeof(nTileRegionIndex), 1, fp);

			STileRegion  *pTileregion = pTerrain->GetTileRegion(nTileRegionIndex);
			if( pTileregion )
			{
				size_t subSize = 0;
				fread(&subSize, sizeof(subSize), 1, fp);
				for ( size_t m = 0; m < subSize; ++m )
				{
					int8 nSubTileRegionIndex = -1;
					fread(&nSubTileRegionIndex, sizeof(nSubTileRegionIndex), 1, fp);
					
					STileRenderRegionUnit *pRenderRegionUnit = pTileregion->vRenderRegionUnits[nSubTileRegionIndex];
					pRenderRegionUnit->bIsVisibleInView = false;

					m_invisibleTileRegionIndexs[nTileRegionIndex].insert(nSubTileRegionIndex);

					pTerrain->SetTerrainNodeVisbileMark(nTileRegionIndex, nSubTileRegionIndex, false);
				}
			}
		}
	}

	void CToolHideTileRegionInView::LoadOldTileRegionInfo( FILE * fp )
	{
		size_t size = 0;
		fread(&size, sizeof(size), 1, fp);

		CTerrainMesh *pTerrain	  = CMapEditApp::GetInst()->GetEditingMesh();

		int nTileRegionIndex = -1;
		for ( size_t i = 0; i < size; ++i )
		{
			fread(&nTileRegionIndex, sizeof(nTileRegionIndex), 1, fp);

			STileRegion  *pTileregion = pTerrain->GetTileRegion(nTileRegionIndex);
			if( pTileregion )
			{
				for ( size_t m = 0; m < CTerrainMesh::Tile_PerSqr; ++m )
				{
					STileRenderRegionUnit *pRenderRegionUnit = pTileregion->vRenderRegionUnits[m];
					pRenderRegionUnit->bIsVisibleInView = false;

					m_invisibleTileRegionIndexs[nTileRegionIndex].insert(m);

					pTerrain->SetTerrainNodeVisbileMark(nTileRegionIndex, m, false);
				}
			}
		}
	}

	void CToolHideTileRegionInView::SetIsShowTileRegionVisible( const bool b )
	{
		int nTileRegionIndex = -1;
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( b )
		{
			invisiblePool::iterator beg = m_invisibleTileRegionIndexs.begin();
			invisiblePool::iterator end = m_invisibleTileRegionIndexs.end();
			for ( beg; beg!= end; ++beg )
			{
				nTileRegionIndex = beg->first;

				int8 nSubTileRegionIndex = -1;
				const subRegIndexPool subIndexs = beg->second;
				for ( subRegIndexPool::const_iterator iter = subIndexs.begin(); iter != subIndexs.end(); ++ iter )
				{
					nSubTileRegionIndex = *iter;
					pTerrain->SetTerrainNodeVisbileMark(nTileRegionIndex, nSubTileRegionIndex, false);
				}
			}
		}
		else
		{
			set<DWORD> buffers = pTerrain->m_TerrainBufferIndexs;
			set<DWORD>::iterator beg = buffers.begin();
			set<DWORD>::iterator end = buffers.end();
			for ( beg; beg != end; ++beg )
			{
				nTileRegionIndex = *beg;

				for (  uint nSubTileRegionIndex = 0; nSubTileRegionIndex < CTerrainMesh::Tile_PerSqr; ++nSubTileRegionIndex )
				{
					pTerrain->SetTerrainNodeVisbileMark(nTileRegionIndex, nSubTileRegionIndex, true);
				}
			}
		}
	}

	void CToolHideTileRegionInView::HideAllTileRegion( const bool bHide )
	{
		int index = -1;
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		set<DWORD> buffers = pTerrain->m_TerrainBufferIndexs;
		set<DWORD>::iterator beg = buffers.begin();
		set<DWORD>::iterator end = buffers.end();
		for ( beg; beg != end; ++beg )
		{
			index = *beg;
			for (  uint nSubTileRegionIndex = 0; nSubTileRegionIndex < CTerrainMesh::Tile_PerSqr; ++nSubTileRegionIndex )
			{
				pTerrain->SetTerrainNodeVisbileMark(index, nSubTileRegionIndex, !bHide);
			}
		}
		if( !bHide )
		{
			this->SetIsShowTileRegionVisible(true);
		}
	}

	void CToolHideTileRegionInView::ClearAllTileRegionVisibleMark()
	{
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		size_t size = m_invisibleTileRegionIndexs.size();
		for ( size_t i = 0; i < size; ++i )
		{
			int nTileRegionIndex = 0;
			invisiblePool::iterator beg;
			invisiblePool::iterator end = m_invisibleTileRegionIndexs.end();
			for ( beg = m_invisibleTileRegionIndexs.begin(); beg!= end; ++beg )
			{
				nTileRegionIndex = beg->first;
				STileRegion  *pTileregion = pTerrain->GetTileRegion(nTileRegionIndex);
				if( pTileregion )
				{
					for ( uint n = 0; n < CTerrainMesh::Tile_PerSqr; ++n )
					{
						STileRenderRegionUnit *pRenderRegionUnit = pTileregion->vRenderRegionUnits[n];
						pRenderRegionUnit->bIsVisibleInView = true;
					}
				}
			}

		}
		this->SetIsShowTileRegionVisible(false);

		m_invisibleTileRegionIndexs.clear();
		::SendMessage(m_hTileRegionView, WM_TILEREGIONVIEW_REFRESH,0,0);
	}
}