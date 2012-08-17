#include "stdafx.h"
#include "ToolSetTileHeight.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "UserMsg.h"

#include "CmdSetVertexHeight.h"
#include "Square.h"
#include "Render.h"
#include "Operator.h"
#include "Operator.inl"
#include "OperationLog.h"
#include "Float.h"
#include "IoSystem.h"
#include "CEventState.h"

namespace sqr
{
	CToolSetTileHeight::CToolSetTileHeight()
	{
		m_nGridIndexLockCenter		= INVALID;
		m_pLockArea					= NULL;
		m_fReferenceHeight			= 0.0f;
		m_fHeightStrength			= 4.0f;
		m_fCutRadio					= 0.2f;
		m_nLiftPower				= 8;

		m_pBrushTexture				= NULL;
		m_pRS						= NULL;
	}

	CToolSetTileHeight::~CToolSetTileHeight()
	{
		if(m_pBrushTexture)
			m_pBrushTexture = NULL;
		if(m_pRS)
			m_pRS = NULL;
	}

	void CToolSetTileHeight::OnEvent(const CMsg& msg)
	{	
		switch ( msg.msgSource )
		{
		case MGS_MOUSEMSG:
			{
				switch(msg.msgType.mouse_type)
				{
				case MGT_MOUSE_LBNUP:
					{	
						if( CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN )
							this->ShowAreLockEvent();
					}
				case MGT_MOUSE_LBNDOWN:
				case MGT_MOUSE_MOVE :	
					{
						if ( CEventState::GetInst()->GetState(MGT_MOUSE_LBUTTON) == MGT_MOUSE_LBNDOWN )
						{
							if ( CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_UP )
							{
								this->OperaterTileHeight();
							}
						}
					}
				}
			}
		
		break;
		}
	}

	void CToolSetTileHeight::Render()
	{
		CRenderDevice * pRender = CRenderDevice::GetInst();
		COperator * pOperator   = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		
		// for brush
		static CVector3f vertex[VERTEX_BATCH_SIZE];
		static CVector2f uv0[VERTEX_BATCH_SIZE];
		static VerColorTex rvertex[VERTEX_BATCH_SIZE];
		DWORD vecColor[4];
		vecColor[0] = 
		vecColor[1] =
		vecColor[2] =
		vecColor[3] =0xffffffff;

		vector<int> & vecBrushGridIndices = this->GetBrushGridIndices();
		
		size_t cnt = vecBrushGridIndices.size();
		
		float fStepInX = 1.0f/(m_processParams.dwBrushSizeInX*2);
		float fStepInZ = 1.0f/(m_processParams.dwBrushSizeInZ*2);
		
		DWORD drawnCnt = 0;
		for ( DWORD n = 0; n < cnt; n++, drawnCnt++ )
		{	
			if ( drawnCnt && !((drawnCnt) % GRID_BATCH_SIZE) )
			{	
				pRender->DrawLayer3DSolidQuad(&m_pRS, rvertex, m_pBrushTexture, GRID_BATCH_SIZE);

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
				m_pRS->m_ZTestFun = 4;
				m_pRS->m_Cull = CULLT_CCW;
				drawnCnt = 0;
			}
			
			SGrid & s = pTerrain->GetGrid(vecBrushGridIndices[n]);
			
			int nStepsInX = n % (2*m_processParams.dwBrushSizeInX);
			int nStepsInZ = m_processParams.dwBrushSizeInZ*2 - n / (2*m_processParams.dwBrushSizeInX);

			vertex[0] = pTerrain->GetVertex(s.dwVertexIndex[0]).vPosition;
			uv0[0] = CVector2f(nStepsInX*fStepInX, nStepsInZ*fStepInZ);

			vertex[1] = pTerrain->GetVertex(s.dwVertexIndex[3]).vPosition;
			uv0[1] = CVector2f(nStepsInX*fStepInX, nStepsInZ*fStepInZ - fStepInZ);

			vertex[2] = pTerrain->GetVertex(s.dwVertexIndex[1]).vPosition;
			uv0[2] = CVector2f(nStepsInX*fStepInX + fStepInX, nStepsInZ*fStepInZ);

			vertex[3] = pTerrain->GetVertex(s.dwVertexIndex[2]).vPosition;
			uv0[3] = CVector2f(nStepsInX*fStepInX + fStepInX, nStepsInZ*fStepInZ - fStepInZ );

			rvertex[6 * drawnCnt + 0] = VerColorTex(vertex[0].x, vertex[0].y, vertex[0].z, vecColor[0], uv0[0].x, uv0[0].y);
			rvertex[6 * drawnCnt + 1] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, vecColor[1], uv0[1].x, uv0[1].y);
			rvertex[6 * drawnCnt + 2] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, vecColor[2], uv0[2].x, uv0[2].y);

			rvertex[6 * drawnCnt + 3] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, vecColor[1], uv0[1].x, uv0[1].y);
			rvertex[6 * drawnCnt + 4] = VerColorTex(vertex[3].x, vertex[3].y, vertex[3].z, vecColor[3], uv0[3].x, uv0[3].y);
			rvertex[6 * drawnCnt + 5] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, vecColor[2], uv0[2].x, uv0[2].y);
		}

		if( drawnCnt > 0 )
		{
			pRender->DrawLayer3DSolidQuad(&m_pRS, rvertex, m_pBrushTexture, drawnCnt);

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
			m_pRS->m_ZTestFun = 4;
			m_pRS->m_Cull = CULLT_CCW;
		}
			
		// for lock
		if ( m_nGridIndexLockCenter != INVALID && m_processParams.eWayType == sqr::EWT_CUT )
		{
			m_pLockArea->Render();
		}	
	}

	void CToolSetTileHeight::InitProcessParams(HWND hGroundView)
	{
		m_hGroundView = hGroundView;
		string config("MEditorRes:DefaultLock.dds");
		string str = IoSystem::GetInst()->GetFullPath(config);

		m_pLockArea = CSceneManager::GetInst()->AddNodeSquare(GRID_SPACE, GRID_SPACE, 0xffffffff, str);
		m_pLockArea->SetVisible(false);

		m_vecGridIndicesLock.clear();

		CRenderDevice * pRender = CRenderDevice::GetInst();

		config = "MEditorRes:DefaultBrush.dds";
		str = IoSystem::GetInst()->GetFullPath(config);
		m_pBrushTexture = pRender->GetTexture(str);
	}

	vector<int> & CToolSetTileHeight::GetBrushGridIndices()
	{
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();
		
		static vector<int> vecBrushGridIndices;
		
		vecBrushGridIndices.clear();
		
		if ( pTerrain->IsValid(pOperator->GetGridIndexHot()) )
		{
			DWORD BrushSizeInX = this->GetBrushSizeInX();
			DWORD BrushSizeInZ = this->GetBrushSizeInZ();
			DWORD dwCnt = 4 * BrushSizeInX * BrushSizeInZ;
			int width = pTerrain->GetWidth();
					
			int nGridIndex = pOperator->GetGridIndexHot() - BrushSizeInX - BrushSizeInZ * width;
			
			for ( DWORD j = 0; j < dwCnt; j++)
			{
				if ( j )
				{
					if ( !(j % (2 * BrushSizeInX)) ) 
						nGridIndex += width - 2 * BrushSizeInX + 1;
					else
						++nGridIndex;
				}
				
				// check for valid grid
				if ( pTerrain->IsValid(nGridIndex) )
					vecBrushGridIndices.push_back(nGridIndex);
			}
		}

		return vecBrushGridIndices;
	}

	DWORD CToolSetTileHeight::GetBrushSizeInX() 
	{ 
		return m_processParams.dwBrushSizeInX; 
	}

	void CToolSetTileHeight::SetBrushSizeInX(DWORD dw) 
	{ 
		m_processParams.dwBrushSizeInX = dw;	
	}

	void CToolSetTileHeight::IncBrushSizeInX() 
	{ 
		++m_processParams.dwBrushSizeInX; 
	}

	void CToolSetTileHeight::DecBrushSizeInX() 
	{ 
		--m_processParams.dwBrushSizeInX;
	}

	DWORD CToolSetTileHeight::GetBrushSizeInZ() 
	{ 
		return m_processParams.dwBrushSizeInZ; 
	}

	void CToolSetTileHeight::SetBrushSizeInZ(DWORD dw) 
	{ 
		m_processParams.dwBrushSizeInZ = dw;	
	}

	void CToolSetTileHeight::IncBrushSizeInZ() 
	{ 
		++m_processParams.dwBrushSizeInZ; 
	}

	void CToolSetTileHeight::DecBrushSizeInZ() 
	{ 
		--m_processParams.dwBrushSizeInZ;
	}

	void CToolSetTileHeight::SetHeightType(const int nType)

	{
		sqr::EHEIGHT_TYPE eTpye = (sqr::EHEIGHT_TYPE)nType;
		m_processParams.eHeightType = eTpye;
	}

	void CToolSetTileHeight::SetTemplateType(const int nType)
	{
		sqr::EHEIGHT_TEMPLATE_TYPE e = (sqr::EHEIGHT_TEMPLATE_TYPE)nType;
		m_processParams.eTemplateType = e;
		m_processParamsLock.eTemplateType = e;
	}

	sqr::EHEIGHT_TYPE CToolSetTileHeight::GetHeightType()
	{
		return m_processParams.eHeightType;
	}

	void CToolSetTileHeight::SetDirType(const int n)
	{
		sqr::EDIR_TYPE e = (sqr::EDIR_TYPE)n;
		m_processParams.eDirType = e;
	}

	void CToolSetTileHeight::SetWayType(const int n)
	{
		sqr::EWAY_TYPE e = (sqr::EWAY_TYPE)n;
		m_processParams.eWayType = e;
	}

	void CToolSetTileHeight::ShowAreLockEvent()
	{
		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		m_vecGridIndicesLock = GetBrushGridIndices();
		m_nGridIndexLockCenter = pOperator->GetGridIndexHot();
		m_processParamsLock = m_processParams;
		m_pLockArea->SetWidth(m_processParams.dwBrushSizeInX*2*GRID_SPACE);
		m_pLockArea->SetDepth(m_processParams.dwBrushSizeInZ*2*GRID_SPACE);
		SGrid &grid = pTerrain->GetGrid(m_nGridIndexLockCenter);
		DWORD vertexIndex = grid.dwVertexIndex[0];

		if( !pTerrain->IsValidVertexIndex(vertexIndex) )
			return;

		CVector3f v = pTerrain->GetVertex(vertexIndex).vPosition;
		m_fReferenceHeight = v.y;
		m_pLockArea->SetRelativePos(v);
		m_pLockArea->UpdateAbsolute();

		::SendMessage(m_hGroundView, WM_MESHVIEW_SETHEIGHT, 4, 0);

		char szHeightBuf[64];
		sprintf(szHeightBuf, "网格面板下格子：%d取高", m_nGridIndexLockCenter);
		COpearationLog::GetInst()->WriteOperationLog(szHeightBuf);
	}

	void CToolSetTileHeight::OperaterTileHeight()
	{
		COperator * pOperator   = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		int nGridHotIndex = pOperator->GetGridIndexHot();

		// set grids via brush size
		if ( pTerrain->IsValid(nGridHotIndex) )
		{
			CCmdSetVertexHeight * pCmd = new CCmdSetVertexHeight;

			pCmd->SetGridIndexHot(nGridHotIndex);
			pCmd->SetProcessParams(m_processParams);

			vector<int> & vecBrushGridIndices = this->GetBrushGridIndices();
			vector<int> vecVertexIndices;

			size_t size = vecBrushGridIndices.size();
			for ( size_t i = 0; i < size; ++i )
			{
				SGrid & grid = pTerrain->GetGrid(vecBrushGridIndices[i]);

				for ( size_t j = 0; j < 4; j++)
				{
					pCmd->AddVertex(grid.dwVertexIndex[j]);

					if ( m_processParams.eWayType == sqr::EWT_CUT )
					{
						vecVertexIndices.push_back(grid.dwVertexIndex[j]);
					}
				}
			}

			// for cut
			if ( m_processParams.eWayType == sqr::EWT_CUT )
			{
				if ( m_nGridIndexLockCenter != INVALID )
				{
					pCmd->SetReferenceHeight(m_fReferenceHeight);
				}
				else
				{
					float fMinY = FLT_MAX;

					size_t size = vecVertexIndices.size();
					for ( size_t m = 0; m < size; ++m )
					{	
						SVertex & vertex = pTerrain->GetVertex(vecVertexIndices[m]);
						fMinY = min(vertex.vPosition.y,fMinY);
					}

					pCmd->SetReferenceHeight(fMinY);
				}
			}

			vecBrushGridIndices = this->GetBrushGridIndices();
			size = vecBrushGridIndices.size();
			for ( size_t i = 0; i < size; ++i )
			{
				pCmd->AddGrid(vecBrushGridIndices[i]);
			}

			pCmd->Execute();
		}
	}
}
