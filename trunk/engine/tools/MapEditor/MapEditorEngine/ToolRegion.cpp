#include "stdafx.h"
#include "ToolRegion.h"
#include "UserMsg.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "Render.h"
#include "Operator.h"
#include "Operator.inl"
#include "CLogicBursh.h"
#include "BaseHelper.h"

#include "CmdSetHideGrid.h"
#include "SceneRegionMgr.h"
#include "CRegionAmbientEffectMgr.h"
#include "IoSystem.h"
#include "CEventState.h"
#include "CEditorConfig.h"
#include "StringHelper.h"

namespace sqr
{
	const string REGION_NODE_NAME		= "RegionBaking";
	const string REGION_NODE_KEY		= "Property";
	const string Terrain_Attribute_NAME = "terrain";
	const string Object_Attribute_NAME	= "object";
	const string Logic_Attribute_NAME	= "logic";
	const string Range_Attribute_NAME	= "range";

	CToolRegion::CToolRegion()
	{
		m_nRegionHot		= INVALID;
		m_pLogicBrush		= NULL;
		m_strCurRegionName  = "";
		m_pSubRegionLockTexture  = NULL;
		m_pRS				= NULL;

		m_pRegionBakingPro	= new sRegionBakingPro;
	}

	CToolRegion::~CToolRegion()
	{
		if( m_pLogicBrush )
			SafeDelete(m_pLogicBrush);

		SafeDelete(m_pRegionBakingPro);
		SafeRelease(m_pSubRegionLockTexture);
	}

	void CToolRegion::OnEvent(const CMsg& msg)
	{	
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetTerrain();
		
		switch ( msg.msgSource )
		{
		case MGS_KEYMSG:
			{
				if ( ( CEventState::GetInst()->GetState(MGI_KEY_Menu) == MGT_KEY_DOWN ) )
				{
					switch(msg.msgInfo.key_info)
					{
					case MGI_KEY_H:
						this->HideRegionMark();
						return;

					default:
						return;
					}
				}

				switch(msg.msgInfo.key_info)
				{
				case MGI_KEY_ShiftKey:
					this->SelectSceneRegion();
					return;

				case MGI_KEY_CtrlKey:
					{
						vector<int> vecBrushRegions = m_pLogicBrush->GetBrushSubRegionIndices();
						this->CanleSecneRegion(vecBrushRegions);

						return;
					}
					
					break;

				default:
					break;
				}
			}

		case MGS_MOUSEMSG:
			{
				switch(msg.msgType.mouse_type)
				{
				case MGT_MOUSE_MOVE:
					::SendMessage(m_hRegionView, WM_REGIONVIEW_SHARPREGUNIT,0,0);
					break;

				default:
					break;
				}
			}
		}
	}

	void CToolRegion::Render()
	{
		this->UpdateCurSelectRegion();
		m_pLogicBrush->Render();
	}

	void CToolRegion::Initialize(HWND hRegionView)
	{
		m_hRegionView = hRegionView;

		if( m_pLogicBrush == NULL )
			m_pLogicBrush = new CLogicBrush;
		m_pLogicBrush->InitBrushParams(1, 1, 0x00ffffff, "home:res\\DefaultBrush.dds");

		string config = "MEditorRes:DefaultLock.dds";
		string str = IoSystem::GetInst()->GetFullPath(config);
		m_pSubRegionLockTexture = CRenderDevice::GetInst()->GetTexture(str);

		this->LoadRegionBakingProConfig();
	}

	void CToolRegion::IncBrushSizeInX() 
	{ 
		m_pLogicBrush->IncBrushSizeInX();
	}

	void CToolRegion::DecBrushSizeInX() 
	{ 
		m_pLogicBrush->DecBrushSizeInX();
	}

	void CToolRegion::IncBrushSizeInZ() 
	{ 
		m_pLogicBrush->IncBrushSizeInZ();
	}

	void CToolRegion::DecBrushSizeInZ() 
	{ 
		m_pLogicBrush->DecBrushSizeInZ();
	}

	void CToolRegion::SetHideGrid( const bool bHideGrid )
	{
		CCmdSetHideGrid * pCmd;
		pCmd = new CCmdSetHideGrid;

		pCmd->SetGridIsHide(bHideGrid);
		vector<int> vecBrushGridIndices = m_pLogicBrush->GetBrushGridIndices();

		for ( size_t i = 0; i < vecBrushGridIndices.size(); ++i )
		{
			pCmd->AddGrid(vecBrushGridIndices[i]);
		}

		//////////////////////////////////////////////////////////////////////////
		pCmd->Execute();
	}

	void CToolRegion::RevertAllHideGrid(const bool bHideGrid)
	{
		CTerrainMesh *pTerrainMesh = NULL;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;

		int nGridCnt = pTerrainMesh->GetGridCount();
		for ( int n = 0; n < nGridCnt; ++n )
		{
			SGrid &grid = pTerrainMesh->GetGrid(n);
			grid.bGridHide = bHideGrid;
		}

		pTerrainMesh->ReWriteTerrainBuffer(pTerrainMesh->m_TerrainBufferIndexs, 0);
	}

	void CToolRegion::SetTerrainGridIsHide( vector<int>& gridindexs, bool b )
	{
		CTerrainMesh *pTerrainMesh = NULL;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;

		size_t gridsCount = gridindexs.size();
		if ( gridsCount == 0 )
			return;

		if ( pTerrainMesh->IsWater() )
		{
			CTerrainMesh *pWater = NULL;
			pWater = CMapEditApp::GetInst()->GetWater();
			if ( pWater == NULL )
				return;

			for ( size_t i = 0; i < gridsCount; ++i )
			{
				SGrid &grid = pWater->GetGrid(gridindexs[i]);
				grid.bGridHide = b;
			}
		}
		else
		{
			for ( size_t i = 0; i < gridsCount; ++i )
			{
				SGrid &grid = pTerrainMesh->GetGrid(gridindexs[i]);
				grid.bGridHide = b;
			}
		}
	}

	// 直到面板窗口操作了插入窗口，否则lshift一直表现成连选
	void CToolRegion::SetCurrentSelectRegions( const vector<int>& regions )
	{
		size_t size = regions.size();
		for( size_t i = 0; i < size; ++i )
			m_CurSelectRegions.insert(regions[i]);
	}

	void CToolRegion::SelectSceneRegion()
	{
		vector<int>   vecBrushRegions, vecSetBrushRgnIndexs;
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		set<DWORD>   bufferindexs;

		vecBrushRegions = m_pLogicBrush->GetBrushSubRegionIndices();
		if( vecBrushRegions.empty() )
			return;

		size_t size = vecBrushRegions.size();
		for( size_t i = 0; i < size; ++i )
		{
			SDivideRegion *pDivideRgn = pTerrain->GetDivideRegion(vecBrushRegions[i]);
			int nDivideRgnIndex = vecBrushRegions[i];
			string rgnName = "";
			if( CSceneRegionMgr::GetInst()->DecideRegionIsSet(nDivideRgnIndex, rgnName) )
			{
				string log = "";
				log = format("选择的区域: %d,已在区域: %s ,YES不选择,No选择", nDivideRgnIndex, rgnName.c_str());
				if ( IDYES == MessageBox(NULL, log.c_str(), "提示", MB_YESNO) )
					continue;
			}

			vecSetBrushRgnIndexs.push_back(nDivideRgnIndex);
			size_t size = pDivideRgn->vGridIndices.size();
			for ( size_t i = 0; i < size; ++i )
			{
				SGrid & grid = pTerrain->GetGrid(pDivideRgn->vGridIndices[i]);

				grid.bInHideRegion = true;
				bufferindexs.insert(grid.dwBufferIndex);
			}
		}

		this->SetCurrentSelectRegions(vecSetBrushRgnIndexs);

		this->SharpTerrainRegion(bufferindexs, 0x00ff0000);
	}

	bool CToolRegion::CanleSecneRegion(const vector<int>& vecBrushRegions)
	{
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		set<int>   vecGridIndexs;
		set<DWORD> bufferindexs;
		size_t size = vecBrushRegions.size();
		for( size_t i = 0; i < size; ++i )
		{
			SDivideRegion *pDivideRgn = pTerrain->GetDivideRegion(vecBrushRegions[i]);

			size_t size = pDivideRgn->vGridIndices.size();
			for ( size_t i = 0; i < size; ++i )
			{
				SGrid & grid = pTerrain->GetGrid(pDivideRgn->vGridIndices[i]);
				grid.bInHideRegion = false;
				vecGridIndexs.insert(grid.dwGridIndex);
				bufferindexs.insert(grid.dwBufferIndex);
			}
		}

		if ( IDYES == MessageBox(NULL, "是否要删除选择区域", "YES删除", MB_YESNO) )
		{
			int nSubRegIndex = 0;
			size_t size = vecBrushRegions.size();
			for( size_t i = 0; i < size; ++i )
			{
				nSubRegIndex = vecBrushRegions[i];
				CSceneRegionMgr::GetInst()->DeleteSceneRegionUnit( m_strCurRegionName, nSubRegIndex );
			}

			::SendMessage(m_hRegionView, WM_REGIONVIEW_RESETREGUNIT,0,(LPARAM)nSubRegIndex);

			this->SharpTerrainRegion(bufferindexs, 0x00ff0000);
		}
		else
			return false;

		return true;
	}

	void CToolRegion::UpdateCurSelectRegion()
	{
		set<int>     vecGridIndexs;
		vector<int>  vecBrushRegions = m_pLogicBrush->GetBrushSubRegionIndices();;
		static set<int> lastVecBrushRegions;
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

 		if( vecBrushRegions.empty() )
 			return;

		int nBrushX = m_pLogicBrush->GetBrushSizeInX();
		int nBurshZ = m_pLogicBrush->GetBrushSizeInZ();
		int nSubRgnNum = (int)vecBrushRegions.size();;
		int nDiviceReginWidthGrids = REGION_SPACE / REGION_STEP_DIVIDE;

		SDivideRegion *pLeftBottomDivideRgn = pTerrain->GetDivideRegion(vecBrushRegions[0]);
		SGrid &leftbottomgrid = pTerrain->GetGrid(pLeftBottomDivideRgn->vGridIndices[0]);
		
		SDivideRegion *pRightBottomDivideRgn = pTerrain->GetDivideRegion(vecBrushRegions[nBrushX-1]);
		SGrid &rightbottomgrid = pTerrain->GetGrid(pRightBottomDivideRgn->vGridIndices[nDiviceReginWidthGrids-1]);
		
		SDivideRegion *pRightTopDivideRgn = pTerrain->GetDivideRegion(vecBrushRegions[nSubRgnNum-1]);
		size_t subRegionGridNum = pRightTopDivideRgn->vGridIndices.size();
		SGrid &righttopgrid = pTerrain->GetGrid(pRightTopDivideRgn->vGridIndices[subRegionGridNum-1]);

		SDivideRegion *pLeftTopDivideRgn = pTerrain->GetDivideRegion(vecBrushRegions[nSubRgnNum-nBrushX]);
		SGrid &lefttopgrid = pTerrain->GetGrid(pLeftTopDivideRgn->vGridIndices[subRegionGridNum-nDiviceReginWidthGrids]);

		static CVector3f	vertex[4];
		static CVector2f	uv0[4];
		static VerColorTex	rvertex[6];
		static DWORD		vecColor[4];
		vecColor[0] = vecColor[1] = vecColor[2] = vecColor[3] = 0x00ffffff;

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

		rvertex[0] = VerColorTex(vertex[0].x, vertex[0].y, vertex[0].z, vecColor[0], uv0[0].x, uv0[0].y);
		rvertex[1] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, vecColor[1], uv0[1].x, uv0[1].y);
		rvertex[2] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, vecColor[2], uv0[2].x, uv0[2].y);

		rvertex[3] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, vecColor[1], uv0[1].x, uv0[1].y);
		rvertex[4] = VerColorTex(vertex[3].x, vertex[3].y, vertex[3].z, vecColor[3], uv0[3].x, uv0[3].y);
		rvertex[5] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, vecColor[2], uv0[2].x, uv0[2].y);

		CRenderDevice::GetInst()->DrawLayer3DSolidQuad(&m_pRS, rvertex, m_pSubRegionLockTexture, 1);
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

	void CToolRegion::SharpTerrainRegion(set<DWORD>& bufferindexs, const DWORD color)
	{
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( !bufferindexs.empty() && !pTerrain->IsWater() )
		{
			//烘焙1 固定0
			pTerrain->ReWriteTerrainBuffer( bufferindexs, 2, color );
		}
	}

	void CToolRegion::HideRegionMark()
	{
		static bool bShowRegion = false;
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		if( !pTerrain->IsWater() )
		{
			//烘焙1 固定0
			pTerrain->ReWriteTerrainBuffer( pTerrain->m_TerrainBufferIndexs, 2, 0x00ff0000 );
		}

		bShowRegion = !bShowRegion;
	}

	void CToolRegion::SwitchShowAllRgnAmbientEffect(const bool b)
	{
		CMapRegionAmbientEffectMgr::GetInst()->SwitchShowAllRgnAmbientEffect(b);
	}

	int CToolRegion::GetCurrentHotSunRegionIndex()
	{
		int nHotGridIndex = CMapEditApp::GetInst()->GetOperator()->GetGridIndexHot();
		if( nHotGridIndex == INVALID )
			return -1;

		SGrid& grid	= CMapEditApp::GetInst()->GetEditingMesh()->GetGrid(nHotGridIndex);
		return grid.m_nDivideRegionIndex;
	}

	void CToolRegion::GetSelectSceneRegionIndexs( set<int>& bufferindexs )
	{
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain == NULL )
			return;

		set<int> regionUnits = CSceneRegionMgr::GetInst()->GetCurrentRegions(m_strCurRegionName);
		for ( set<int>::iterator iter = regionUnits.begin(); iter != regionUnits.end(); ++iter )
		{
			int subRegionIndex = *iter;
			SDivideRegion *pDivideRgn = pTerrain->GetDivideRegion(subRegionIndex);
			SGrid & grid = pTerrain->GetGrid(pDivideRgn->vGridIndices[0]);
			bufferindexs.insert(grid.dwBufferIndex);
		}
	}

	CToolRegion::sRegionBakingPro* CToolRegion::GetRegionBakingPro() const
	{
		return m_pRegionBakingPro;
	}

	void CToolRegion::SetIsTerrain( const bool b )
	{
		m_pRegionBakingPro->bTerrain = b;
	}

	void CToolRegion::SetIsObject( const bool b )
	{
		m_pRegionBakingPro->bObject = b;
	}

	void CToolRegion::SetIsLogic( const bool b )
	{
		m_pRegionBakingPro->bLogic = b;
	}

	void CToolRegion::SetRegionDistrubSize( const int distrubSize )
	{
		m_pRegionBakingPro->nDistrubSize = distrubSize;
	}

	void CToolRegion::SaveRegionBakingProConfig()
	{
		string temp;
		TiXmlNode* pNode = CEditorConfig::GetInst()->GetEditorConfig(REGION_NODE_NAME.c_str());
		if( pNode )
		{
			// 		//结点存在 获取属性值
			TiXmlElement* pElement=pNode->ToElement();
			TiXmlElement newNode(REGION_NODE_KEY);

			temp = format("%d", m_pRegionBakingPro->bTerrain);
			newNode.SetAttribute(Terrain_Attribute_NAME.c_str(), temp);

			temp = format("%d", m_pRegionBakingPro->bObject);
			newNode.SetAttribute(Object_Attribute_NAME.c_str(), temp);

			temp = format("%d", m_pRegionBakingPro->bLogic);
			newNode.SetAttribute(Logic_Attribute_NAME.c_str(), temp);

			temp = format("%d", m_pRegionBakingPro->nDistrubSize);
			newNode.SetAttribute(Range_Attribute_NAME.c_str(), temp);

			pNode->InsertEndChild(newNode);

			CEditorConfig::GetInst()->SaveEditorConfig();
		}
	}

	void CToolRegion::LoadRegionBakingProConfig()
	{
		TiXmlNode* pNode = CEditorConfig::GetInst()->GetEditorConfig(REGION_NODE_NAME.c_str());
		if( pNode )
		{
			// 		//结点存在 获取属性值
			pNode = pNode->FirstChild(REGION_NODE_KEY);
			if(pNode == NULL)
				return;

			TiXmlElement* pElement = NULL;
			pElement = pNode->ToElement();
			if( pElement )
			{
				string temp = pElement->Attribute(Terrain_Attribute_NAME.c_str());
				int nValue = atoi(temp.c_str());
				m_pRegionBakingPro->bTerrain = nValue == 1 ? true : false;

				temp = pElement->Attribute(Object_Attribute_NAME.c_str());
				nValue = atoi(temp.c_str());
				m_pRegionBakingPro->bObject = nValue == 1 ? true : false;

				temp = pElement->Attribute(Logic_Attribute_NAME.c_str());
				nValue = atoi(temp.c_str());
				m_pRegionBakingPro->bLogic = nValue == 1 ? true : false;

				temp = pElement->Attribute(Range_Attribute_NAME.c_str());
				nValue = atoi(temp.c_str());
				m_pRegionBakingPro->nDistrubSize = nValue;
			}
		}
	}

	CToolRegion::sRegionBakingPro::sRegionBakingPro()
	{
		bTerrain = bObject = bLogic = true;
		nDistrubSize = 3;
	}
}
