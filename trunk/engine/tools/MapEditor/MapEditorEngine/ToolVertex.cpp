#include "stdafx.h"
#include "ToolVertex.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "UserMsg.h"

#include "CmdSetVertexColor.h"
#include "Operator.h"
#include "Operator.inl"
#include "ModelGroup.h"
#include "OperationLog.h"
#include "CLogicBursh.h"
#include "BaseHelper.h"
#include "CEventState.h"

namespace sqr
{
	CToolVertex::CToolVertex()
	{
		m_pLogicBrush		= NULL;
		m_dwColor			= 0;
		m_fVertexStrength	= 1.0f;
	}

	CToolVertex::~CToolVertex()
	{
		if( m_pLogicBrush )
			SafeDelete(m_pLogicBrush);
	}

	void CToolVertex::OnEvent(const CMsg& msg)
	{	
		switch ( msg.msgSource )
		{
		case MGS_KEYMSG:
			{
				switch( msg.msgType.key_type )
				{
				case MGT_KEY_DOWN:
					{
						if ( CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN  )
						{
							switch(msg.msgInfo.key_info)
							{
								///LCtrl + R RevertColorSetGridsEffectObject
							case MGI_KEY_R:
								this->RevertColor();
								return;

							default:
								break;
							}
						}

						if ( ( CEventState::GetInst()->GetState(MGI_KEY_Menu) == MGT_KEY_DOWN ) )
						{
							switch(msg.msgInfo.key_info)
							{
								///
							case MGI_KEY_F3:
								this->SetGridsEffectObject(false);
								return;

							default:
								break;
							}
						}

						switch (msg.msgInfo.key_info)
						{
						case MGI_KEY_C:
							{
								COpearationLog::GetInst()->WriteOperationLog("顶点快捷键C操作");
								DWORD bgr = RGB(0xff,0xff,0xff);
								SendMessage(m_hVertexView, WM_VERTEXVIEW_SETRGB,(WPARAM)bgr,NULL);

								this->SetVertexColor(0x00ffffff);
								return;
							}

						case MGI_KEY_F3:
							this->SetGridsEffectObject(true);
							return;

						default:
							break;
						}
					}
				}
			}
			break;
		case MGS_MOUSEMSG:
			{
				switch( msg.msgType.mouse_type )
				{
				case MGT_MOUSE_MOVE:
				case MGT_MOUSE_LBNDOWN:
					{
						if ( CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN )
						{
							this->PickColor();

							return;
						}

						this->OperatorVertexColor();

					}
				}
			}
		break;
		}
	}

	void CToolVertex::Render()
	{
		if( m_pLogicBrush )
			m_pLogicBrush->Render();
	}

	void CToolVertex::Intialize(HWND hVertexView)
 	{	
		m_hVertexView = hVertexView;

		if( m_pLogicBrush == NULL )
			m_pLogicBrush = new CLogicBrush;
		m_pLogicBrush->InitBrushParams(1, 1, 0x00ffffff, "home:res\\DefaultBrush.dds");
	}

	void CToolVertex::IncBrushSizeInX() 
	{ 
		m_pLogicBrush->IncBrushSizeInX();
	}

	void CToolVertex::DecBrushSizeInX() 
	{ 
		m_pLogicBrush->DecBrushSizeInX();
	}

	void CToolVertex::IncBrushSizeInZ() 
	{ 
		m_pLogicBrush->IncBrushSizeInZ();
	}

	void CToolVertex::DecBrushSizeInZ() 
	{ 
		m_pLogicBrush->DecBrushSizeInZ();
	}

	void CToolVertex::PickColor()
	{
		COperator*		pOperator	= CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh*	pTerrain	= CMapEditApp::GetInst()->GetEditingMesh();

		if( pTerrain == NULL )
			return;

		DWORD Red = 0;
		DWORD Green = 0;
		DWORD Blue = 0;

		SGrid& grid = pTerrain->GetGrid(pOperator->GetGridIndexHot());

		COpearationLog::GetInst()->WriteOperationLog(grid.dwGridIndex, "顶点LCTRL吸附顶点色操作");

		for ( int k = 0; k < 4; ++k )
		{
			SVertex& vertex = pTerrain->GetVertex(grid.dwVertexIndex[k]);
			Red		+= ((BYTE*)(&vertex.oColor))[2];
			Green	+= ((BYTE*)(&vertex.oColor))[1];
			Blue	+= ((BYTE*)(&vertex.oColor))[0];
		}

		Red		/=4;
		Green	/=4;
		Blue	/=4;

		DWORD bgr = RGB(Red,Green,Blue);

		SendMessage(m_hVertexView, WM_VERTEXVIEW_SETRGB,(WPARAM)bgr,NULL);

		this->SetVertexColor((Red<<16)|(Green<<8)|Blue);
	}

	void CToolVertex::RevertColor()
	{
		DWORD bgr = RGB(0x80,0x80,0x80);
		SendMessage(m_hVertexView, WM_VERTEXVIEW_SETRGB,(WPARAM)bgr,NULL);

		COpearationLog::GetInst()->WriteOperationLog("CTRL+R 恢复当前笔刷颜色为顶点默认颜色");

		this->SetVertexColor(0x00808080);
	}

	void CToolVertex::DeleteVertexColor()
	{
		CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain == NULL )
			return;

		if( pTerrain->IsWater() )
		{
			MessageBox(NULL, "水面层状态下不要进行Delete操作", "提示", MB_OK);
			return;
		}

		COperator* pOperator = CMapEditApp::GetInst()->GetOperator();
		
		if ( pTerrain->IsValid(pOperator->GetGridIndexHot()) )
		{
			COpearationLog::GetInst()->WriteOperationLog(pOperator->GetGridIndexHot(), "Delete 删除顶点色");
			this->SetVertexColor(0x00808080);
			DWORD bgr = RGB(0x80,0x80,0x80);
			SendMessage(m_hVertexView, WM_VERTEXVIEW_SETRGB,(WPARAM)bgr,NULL);

			CCmdSetVertexColor * pCmd = NULL;
			pCmd = new CCmdSetVertexColor;

			if( pCmd == NULL )
				return;

			pCmd->SetGridIndexHot(pOperator->GetGridIndexHot());
			pCmd->SetProcessParams(m_pLogicBrush->GetBrushSizeInX(), m_pLogicBrush->GetBrushSizeInZ(), m_dwColor);

			vector<int>& vecBrushGridIndices = m_pLogicBrush->GetBrushGridIndices();

			for ( size_t i = 0; i < vecBrushGridIndices.size(); ++i )
			{
				SGrid & grid = pTerrain->GetGrid(vecBrushGridIndices[i]);

				for ( int i = 0; i < 4; i++)
				{
					pCmd->AddVertex(grid.dwVertexIndex[i]);
				}
			}

			this->IncBrushSizeInX();
			this->IncBrushSizeInZ();

			vecBrushGridIndices = m_pLogicBrush->GetBrushGridIndices();

			for ( size_t i = 0; i < vecBrushGridIndices.size(); ++i )
			{
				pCmd->AddGrid(vecBrushGridIndices[i]);
			}

			this->DecBrushSizeInX();
			this->DecBrushSizeInZ();

			pCmd->Execute();
		}
	}

	void CToolVertex::SetGridsEffectObject(const bool bEffectObject)
	{
		CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain == NULL )
			return;

		COperator* pOperator = CMapEditApp::GetInst()->GetOperator();

		set<DWORD>	 bufferindexs;
		vector<int>& vecBrushGridIndices = m_pLogicBrush->GetBrushGridIndices();
		vector< CTObjectPtr >::iterator b,e;

		COpearationLog::GetInst()->WriteOperationLog("设置物体是否受顶点色影响");

		for ( size_t i = 0; i < vecBrushGridIndices.size(); ++i )
		{
			SGrid & grid = pTerrain->GetGrid(vecBrushGridIndices[i]);
			grid.bEffectByVertexColor = bEffectObject;
			bufferindexs.insert(grid.dwBufferIndex);

			if(!grid.vecCenterObjects.empty())
			{
				b = grid.vecCenterObjects.begin();
				e = grid.vecCenterObjects.end();
				grid.vecCenterObjects.end();

				for( b; b != e; ++b )
				{
					CEditModelGroup *pModelGroup = (*b)->m_pModelGroup;

					if( !bEffectObject )
						pModelGroup->ResetOverallAmbientRatio();
					else
						pModelGroup->UpdateOverallAmbientRatio();
				}
			}

		}

// 		if( !bufferindexs.empty() && !pTerrain->IsWater() )
// 		{
// 			//烘焙1 固定 0
// 			pTerrain->ReWriteTerrainBuffer( bufferindexs, 2 );
// 		}
	}

	void CToolVertex::SetAllObjectsIsEffectByVertexColor( const bool bEffectObject )
	{
		CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain == NULL )
			return;

		DWORD dwGridCnt = pTerrain->GetGridCount();
		vector< CTObjectPtr >::iterator b,e;

		COpearationLog::GetInst()->WriteOperationLog("设置场景所有物体是否受顶点色影响");

		for ( DWORD i = 0; i < dwGridCnt; ++i )
		{
			SGrid & grid = pTerrain->GetGrid(i);
			grid.bEffectByVertexColor = bEffectObject;

			if(!grid.vecCenterObjects.empty())
			{
				for ( size_t j = 0; j < grid.vecCenterObjects.size(); ++j )
				{
					CEditModelGroup *pModelGroup = grid.vecCenterObjects[j]->GetModelGroup();

					if( !bEffectObject )
						pModelGroup->ResetOverallAmbientRatio();
					else
						pModelGroup->UpdateOverallAmbientRatio();
				}
			}

		}
	}

	void CToolVertex::RevertVertexColorToIntial( bool bDiffuse )
	{
		CTerrainMesh *pTerrainMesh;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;
		if ( pTerrainMesh->IsWater() )
			return;

		int nVertexCnt = pTerrainMesh->GetVertexCount();
		int nGridCnt   = pTerrainMesh->GetGridCount();
		for ( int n = 0; n < nVertexCnt; ++n )
		{
			SVertex & vertex = pTerrainMesh->GetVertex(n);

			if( bDiffuse )
				vertex.oColor = VERTEX_COLOR;
			else
				vertex.sColor = VERTEX_SPECULAR_COLOR;
		}

		//烘焙就不要
		if( bDiffuse )
			pTerrainMesh->ReWriteTerrainBuffer( pTerrainMesh->m_TerrainBufferIndexs, 2 );
		else
			pTerrainMesh->ReWriteTerrainBuffer( pTerrainMesh->m_TerrainBufferIndexs, 1 );

		MessageBox(NULL,"恢复完毕","提示",MB_OK);
	}

	void CToolVertex::SetAllVertexToColor( DWORD color, int index /*= 0*/ )
	{
		CTerrainMesh *pTerrainMesh;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;
		if ( pTerrainMesh->IsWater() )
			return;

		int nVertexCnt = pTerrainMesh->GetVertexCount();
		int nGridCnt   = pTerrainMesh->GetGridCount();

		if( index == 0 )
		{
			for ( int n = 0; n < nVertexCnt; ++n )
			{
				SVertex & vertex = pTerrainMesh->GetVertex(n);
				vertex.oColor = color;
			}

			//烘焙就不要
			pTerrainMesh->ReWriteTerrainBuffer( pTerrainMesh->m_TerrainBufferIndexs, 2 );
		}
		else
		{
			if( index == 1 )
			{
				for ( int n = 0; n < nVertexCnt; ++n )
				{
					SVertex & vertex = pTerrainMesh->GetVertex(n);
					vertex.sColor = color;
				}

				//烘焙就不要
				pTerrainMesh->ReWriteTerrainBuffer( pTerrainMesh->m_TerrainBufferIndexs, 1 );
			}
		}
	}

	void CToolVertex::OperatorVertexColor()
	{
		COperator* pOperator   = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		DWORD dwGridHotIndex = pOperator->GetGridIndexHot();
		if ( pTerrain->IsValid(dwGridHotIndex) )
		{
			COpearationLog::GetInst()->WriteOperationLog("鼠标左键刷顶点色操作");
			CCmdSetVertexColor * pCmd;
			pCmd = new CCmdSetVertexColor;
			pCmd->SetGridIndexHot(dwGridHotIndex);
			pCmd->SetProcessParams(m_pLogicBrush->GetBrushSizeInX(), m_pLogicBrush->GetBrushSizeInZ(), m_dwColor);

			vector<int> & vecBrushGridIndices = m_pLogicBrush->GetBrushGridIndices();
			for ( size_t i = 0; i < vecBrushGridIndices.size(); ++i )
			{
				SGrid& grid = pTerrain->GetGrid(vecBrushGridIndices[i]);
				pCmd->AddGrid(vecBrushGridIndices[i]);
				for ( int i = 0; i < 4; i++)
				{
					pCmd->AddVertex(grid.dwVertexIndex[i]);
				}
			}

			pCmd->Execute();
			return;
		}
	}
}
