#include "stdafx.h"
#include "ToolSetTileTexture.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "UserMsg.h"

#include "CmdSetSingleTexture.h"
#include "CmdDeleteTexture.h"
#include "CmdSetSerialTexture.h"
#include "CmdSetVertexAlpha.h"
#include "render.h"
#include "MEMath.h"
#include "BaseHelper.h"
#include "CGraphic.h"
#include "Operator.h"
#include "Operator.inl"
#include "OperationLog.h"
#include "Float.h"
#include "TerrainAlgorithm.h"
#include "SceneAudioMgr.h"
#include "AutoSearchFile.h"
#include "IoSystem.h"
#include "CEventState.h"
#include "CEditRenderPipeline.h"
#include "MapGUIManager.h"
#include "BluelyGUI.h"

namespace sqr
{
	const string strCompare = "1234";

	CToolSetTileTexture::CToolSetTileTexture()
	{
		m_bInSubGrid = false;
		m_bCliffLayerOne = false;

		m_pBrushTexture = NULL;
		m_pRS = NULL;

		m_pCheckBoxTextureLayerOne = NULL;
		m_pCheckBoxTextureLayerTwo = NULL;
		
		m_fAlphaStrength = 16.0f;
		m_fAlphaSpecular = 16.0f;
		m_fCliffStrength = 1.0f;

		m_eCopyTerrainType = ECT_ALL;
	}

	CToolSetTileTexture::~CToolSetTileTexture()
	{
		if(m_pBrushTexture)
			m_pBrushTexture = NULL;
		
		if(m_pRS)
			m_pRS = NULL;

		SafeDelete(m_pCheckBoxTextureLayerOne);
		SafeDelete(m_pCheckBoxTextureLayerTwo);

		m_ReplaceTexNames.clear();
	}	

	void CToolSetTileTexture::DoOne()
	{
		CCheckBox::SetHotID(0);
		::SendMessage(m_hGroundView, WM_SETLAYERHOT, 1, 0);
		this->SetLayerHot(0);
		this->SetTextureNameHot(m_pCheckBoxTextureLayerOne->GetTextureName());
	}

	void CToolSetTileTexture::DoTwo()
	{
		CCheckBox::SetHotID(1);
		::SendMessage(m_hGroundView, WM_SETLAYERHOT, 2, 0);
		this->SetLayerHot(1);
		this->SetTextureNameHot(m_pCheckBoxTextureLayerTwo->GetTextureName());
	}

	void CToolSetTileTexture::OnCtrKeyEvent(const MSG_KEY_INF key)
	{
		switch(key)
		{
		case MGI_KEY_C:
			m_copyGridIndexs = this->GetBrushGridIndices();
			COpearationLog::GetInst()->WriteOperationLog("纹理面板下Ctrl+C操作");
			break;

		case MGI_KEY_V:
			PasteCopyTerrain();
			COpearationLog::GetInst()->WriteOperationLog("纹理面板下Ctrl+V操作");
			break;

		case MGI_KEY_D1:
			this->ShowCurLayerTexture();
			return;

		case MGI_KEY_D3:
			::SendMessage( m_hGroundView,WM_TEXTUREVIEW_CHIFFLAYONE, 1, 0);
			return;

		default:
			break;
		}
	}

	void CToolSetTileTexture::OnOnlyKeyEvent(const MSG_KEY_INF key)
	{
		switch(key)
		{
			///操作第一层
		case MGI_KEY_D1:
			this->DoOne();
			break;

			///操作第二层
		case MGI_KEY_D2:
			this->DoTwo();
			break;

			///强行设置地表材质
		case MGI_KEY_F5:
			{
				int8 nMatIndex = CSceneAudioMgr::GetInst()->GetCurSeleteMaterialIndex();
				if( nMatIndex == -1 )
				{
					MessageBox(NULL, "没有选择强行设置的地表材质索引，请在图素面板下进行选择", "提示", MB_OK);
					return;
				}

				this->SetGridMaterialIndex(nMatIndex, true);
				break;
			}

			///强行将设置的地表材质对应的音效索引值取消，赋为-1无效
		case MGI_KEY_F6:
			this->SetGridMaterialIndex(-1, false);
			break;

			///隐藏所有特殊材质颜色
		case MGI_KEY_F7:
			this->HideAllSpecialMaterialGrid();
			break;

		case MGI_KEY_J:
			{
				COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
				this->ShowLayers(!pOperator->GetSnapMode());
				pOperator->SwitchSnapMode();

				break;
			}

		default:
			break;
		}
	}

	void CToolSetTileTexture::OnEvent(const CMsg& msg)
	{
		COperator * pOperator  = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		switch ( msg.msgSource )
		{
		case MGS_KEYMSG:
			{
				switch(msg.msgType.key_type)
				{
				case MGT_KEY_DOWN:
					{
						if ( CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN )
						{
							this->OnCtrKeyEvent(msg.msgInfo.key_info);
							return;
						}

						this->OnOnlyKeyEvent(msg.msgInfo.key_info);

					}
				}
			}
			break;
		case MGS_MOUSEMSG:
			{
				switch(msg.msgType.mouse_type)
				{
				case MGT_MOUSE_RBNDOWN:
					{
						if ( CEventState::GetInst()->GetState(MGI_KEY_ShiftKey) == MGT_KEY_DOWN  )
						{
							///显示当前需要替换成四方连续纹理的纹理名
							::SendMessage( m_hGroundView,WM_TEXTUREVIEW_SELECTREPLACE, 1, 0);

							COpearationLog::GetInst()->WriteOperationLog("纹理面板下LSHIFT+RBUTTON操作,选中需要替换的纹理名");

							return;
						}
					}
				case MGT_MOUSE_MOVE:
					
					if ( CEventState::GetInst()->GetState(MGT_MOUSE_LBUTTON) == MGT_MOUSE_LBNDOWN )
					{
						if ( CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN  )
						{	
							string szMatch = CMapEditApp::GetInst()->GetEditingMesh()->GetCursorHotTextureName(pOperator->GetGridIndexHot(), this->GetLayerHot());
							this->SetTextureNameHot(szMatch);
							::SendMessage( m_hGroundView,WM_TEXTUREVIEW_GROUPUNITNAMES, (WPARAM)szMatch.c_str(), 0);
						}
						else
						{				
							// set grids via brush size
							if ( pTerrain->IsValid(pOperator->GetGridIndexHot()) )
							{
								if( this->IsSequenceGridTextures(m_processParams.vecTextureNames) )
								{
									this->BrushSerialTextures();
								}
								else
									this->BrushSingleTexture();
							}
						}
					}
				}
			}
			break;
		}
	}

	void CToolSetTileTexture::Render()
	{
		CRenderDevice * pRender = CRenderDevice::GetInst();

		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		// for brush
		static CVector3f vertex[4];
		static CVector2f uv0[4];
		DWORD color = 0xffffffff;
		static VerColorTex rvertex[VERTEX_BATCH_SIZE];

		vector<int> & vecBrushGridIndices = this->GetBrushGridIndices();

		int cnt = vecBrushGridIndices.size();
		if ( cnt == 0 )
			return;

		float fStepInX = 1.0f/(m_processParams.dwBrushSizeInX*2);
		float fStepInZ = 1.0f/(m_processParams.dwBrushSizeInZ*2);
		int drawnCnt = 0, n = 0;
		for ( n, drawnCnt; n < cnt; ++n, ++drawnCnt )
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

			rvertex[6 * drawnCnt + 0] = VerColorTex(vertex[0].x, vertex[0].y, vertex[0].z, color, uv0[0].x, uv0[0].y);
			rvertex[6 * drawnCnt + 1] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, color, uv0[1].x, uv0[1].y);
			rvertex[6 * drawnCnt + 2] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, color, uv0[2].x, uv0[2].y);

			rvertex[6 * drawnCnt + 3] = VerColorTex(vertex[1].x, vertex[1].y, vertex[1].z, color, uv0[1].x, uv0[1].y);
			rvertex[6 * drawnCnt + 4] = VerColorTex(vertex[3].x, vertex[3].y, vertex[3].z, color, uv0[3].x, uv0[3].y);
			rvertex[6 * drawnCnt + 5] = VerColorTex(vertex[2].x, vertex[2].y, vertex[2].z, color, uv0[2].x, uv0[2].y);
		}

		if( drawnCnt > 0)
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
	}

	int CToolSetTileTexture::GetLayerHot() 
	{ 
		return m_processParams.nLayerHot; 
	}

	void CToolSetTileTexture::SetLayerHot(int n) 
	{ 
		m_processParams.nLayerHot = n; 
	}

	string CToolSetTileTexture::GetTextureNameHot()
	{
		return m_processParams.vecTextureNames[0];
	}

	vector<string> & CToolSetTileTexture::GetTextureNamesHot()
	{
		return m_processParams.vecTextureNames;
	}

	void CToolSetTileTexture::SetTextureNameHot(string name)
	{
		m_processParams.vecTextureNames.clear();
		m_processParams.vecTextureNames.push_back(name);
		switch(m_processParams.nLayerHot)
		{
		case 0:
			ResetCheckBoxTextureLayerOne(m_processParams.vecTextureNames);
			break;
		case 1:
			ResetCheckBoxTextureLayerTwo(m_processParams.vecTextureNames);
		    break;
		default:
			break;
		}
	}

	void CToolSetTileTexture::SetTextureNameHot(vector<string> names)
	{
		m_processParams.vecTextureNames = names;

		switch(m_processParams.nLayerHot)
		{
		case 0:
			ResetCheckBoxTextureLayerOne(m_processParams.vecTextureNames);
			break;
		case 1:
			ResetCheckBoxTextureLayerTwo(m_processParams.vecTextureNames);
			break;
		default:
			break;
		}
	}

	void CToolSetTileTexture::ReplaceTerrainTexture(const int& mark, vector<string>& deletetexnames)
	{
		CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain->IsWater() )
			return;

		if( mark < 4 && deletetexnames.empty() )
		{
			MessageBox(NULL, "请选择需要替换的纹理", "错误操作", MB_OK);
			return;
		}

		if( mark < 4 && m_processParams.vecTextureNames[0].find("Default") != -1 )
		{
			MessageBox(NULL, "请选择具体四方连续贴图图素集", "错误操作", MB_OK);
			return;
		}

		int layhot = 0;
		int nOffset = 0;

		layhot = m_processParams.nLayerHot;
		nOffset = sqr::GetRandomInt(0,m_processParams.vecTextureNames.size()-1);
		
		vector<string>deletenames;
		size_t dwGridCnt = m_ReplaceGrids.size(); 
		string texname = "";
		size_t size = m_processParams.vecTextureNames.size();
		bool bCliffLayer = m_bCliffLayerOne;

		char szTexBuf[64];
		sprintf(szTexBuf, "替换纹理类型 %d", mark);
		COpearationLog::GetInst()->WriteOperationLog(szTexBuf);

		switch (mark)
		{
		case 1:
			if ( m_processParams.vecTextureNames.empty() )
			{	
				MessageBox(NULL, "请选择被替换的纹理资源", "错误操作", MB_OK);
				return;
			}

			texname = m_processParams.vecTextureNames[0];
			for ( size_t i = 0; i < dwGridCnt; ++i )
			{
				SGrid &grid = pTerrain->GetGrid(m_ReplaceGrids[i]);
				pTerrain->SetGridTextureHandle(0, m_ReplaceGrids[i], texname, bCliffLayer);
			}

			dwGridCnt = m_ReplaceTwoGrids.size();
			for ( size_t i = 0; i < dwGridCnt; ++i )
			{
				SGrid &grid = pTerrain->GetGrid(m_ReplaceTwoGrids[i]);
				pTerrain->SetGridTextureHandle(1, m_ReplaceTwoGrids[i], texname, bCliffLayer);
			}

			break;

		case 2:
			if ( m_processParams.vecTextureNames.empty() )
			{	
				MessageBox(NULL, "请选择被替换的纹理资源", "错误操作", MB_OK);
				return;
			}
	
			for ( size_t i = 0; i < dwGridCnt; ++i )
			{
				SGrid &grid = pTerrain->GetGrid(m_ReplaceGrids[i]);

				pTerrain->SetGridTextureHandle(layhot, m_ReplaceGrids[i], m_processParams.vecTextureNames[nOffset], bCliffLayer);

				nOffset = sqr::GetRandomInt(0, size-1);
			}

			dwGridCnt = m_ReplaceTwoGrids.size();
			for ( size_t i = 0; i < dwGridCnt; ++i )
			{
				SGrid &grid = pTerrain->GetGrid(m_ReplaceTwoGrids[i]);
				pTerrain->SetGridTextureHandle(1, m_ReplaceTwoGrids[i], m_processParams.vecTextureNames[nOffset], bCliffLayer);

				nOffset = sqr::GetRandomInt(1, size-1);
			}

			break;

		case 3:
			{
				if ( deletetexnames.empty() )
				{	
					MessageBox(NULL, "请选择需要替换的纹理资源", "错误操作", MB_OK);
					return;
				}

				if ( m_processParams.vecTextureNames.empty() )
				{	
					MessageBox(NULL, "请选择被替换的四方连续图包", "错误操作", MB_OK);
					return;
				}

				if( deletetexnames.size() != m_processParams.vecTextureNames.size() )
				{
					MessageBox(NULL, "替换的四方连续图包中的纹理数量不相等", "错误操作", MB_OK);
					return;
				}

				string strcompare = m_processParams.vecTextureNames[0];
				size_t npos = strcompare.find("1.bmp");
				if( npos != -1 )
				{
					strcompare = strcompare.substr(0, npos);
				}
				else
				{
					MessageBox(0, "所选的四方连续图的命名不符合规则，提示", "", 0);
					return ;
				}

				string texname = "";
				for ( size_t i = 0; i < dwGridCnt; ++i )
				{
					SGrid &grid = pTerrain->GetGrid(m_ReplaceGrids[i]);
					string oldtexname = grid.nodeInfo.strTextureNames[0];
					size_t pos = oldtexname.rfind(".");
					oldtexname = oldtexname.substr(0, pos);
					string posStr = oldtexname.substr(oldtexname.length() - 1, oldtexname.length());
					int index = atoi(posStr.c_str());
					texname = m_processParams.vecTextureNames[index-1];
					pTerrain->SetGridTextureHandle(0, m_ReplaceGrids[i], texname, bCliffLayer);
				}

				dwGridCnt = m_ReplaceTwoGrids.size();
				for ( size_t i = 0; i < dwGridCnt; ++i )
				{
					SGrid &grid = pTerrain->GetGrid(m_ReplaceTwoGrids[i]);
					string oldtexname = grid.nodeInfo.strTextureNames[1];
					size_t pos = oldtexname.rfind(".");
					oldtexname = oldtexname.substr(0, npos);
					string posStr = oldtexname.substr(oldtexname.length() - 1, oldtexname.length());
					int index = atoi(posStr.c_str());
					texname = m_processParams.vecTextureNames[index-1];
					pTerrain->SetGridTextureHandle(1, m_ReplaceTwoGrids[i], texname, bCliffLayer);
				}
			}

			break;

		case 4:
			{
				if( m_processParams.vecTextureNames.empty() )
				{
					MessageBox(NULL, "请选择需要删除的纹理", "错误操作", MB_OK);
					return;
				}

				if( m_processParams.vecTextureNames.size() > 1 )
				{
					MessageBox(NULL, "请选择需要删除的单张纹理，而不是纹理包", "错误操作", MB_OK);
					return;
				}

				int nGridCnt = pTerrain->GetGridCount();
				string deletetexname = m_processParams.vecTextureNames[0];
				if( deletetexname.empty() )
					return;

				deletetexnames.push_back(deletetexname);

				string log = "是否删除" + deletetexname;
				if ( IDYES == MessageBox(NULL, log.c_str(), "提示", MB_YESNO) )
				{			
					string strFirstTexName(""), strSecondTexName("");
					string strDefault = pTerrain->GetTerrainDefaultTextureName();

					for ( int i = 0; i < nGridCnt; ++i )
					{
						SGrid &grid		 = pTerrain->GetGrid(i);
						strFirstTexName  = grid.nodeInfo.strTextureNames[0];
						strSecondTexName = grid.nodeInfo.strTextureNames[1];

						if( strFirstTexName == deletetexname )
							grid.nodeInfo.strTextureNames[0] = strDefault;

						if( strSecondTexName == deletetexname )
						{
							grid.nodeInfo.strTextureNames[1] = "";

							for (int i = 0; i < 4; ++i)
							{
								SVertex &vertex = pTerrain->GetVertex(grid.dwVertexIndex[i]);
								vertex.oColor &= 0x00ffffff;
							}
						}
					}
				}
				else
					return;

				m_processParams.vecTextureNames.clear();
			}
			break;

		case 5:
			{
				if( m_processParams.vecTextureNames.size() < 2 )
				{
					MessageBox(NULL, "请选择需要删除的纹理包", "错误操作", MB_OK);
					return;
				}

				set<string> setDeleteTexNames;
				size_t size = m_processParams.vecTextureNames.size();
				for ( size_t i = 0; i < size; ++i )
					setDeleteTexNames.insert(m_processParams.vecTextureNames[i]);

				string log = "是否删除以" + m_processParams.vecTextureNames[0] + "开头的纹理包";
				if ( IDYES == MessageBox(NULL, log.c_str(), "提示", MB_YESNO) )
				{			
					string strFirstTexName(""), strSecondTexName("");
					string strDefault = pTerrain->GetTerrainDefaultTextureName();

					int nGridCnt = pTerrain->GetGridCount();
					for ( int i = 0; i < nGridCnt; ++i )
					{
						SGrid &grid = pTerrain->GetGrid(i);
						strFirstTexName  = grid.nodeInfo.strTextureNames[0];
						strSecondTexName = grid.nodeInfo.strTextureNames[1];

						if( setDeleteTexNames.find(strFirstTexName) != setDeleteTexNames.end() )
							grid.nodeInfo.strTextureNames[0] = strDefault;

						if( setDeleteTexNames.find(strSecondTexName) != setDeleteTexNames.end() )
						{
							grid.nodeInfo.strTextureNames[1] = "";

							for (int i = 0; i < 4; ++i)
							{
								SVertex &vertex = pTerrain->GetVertex(grid.dwVertexIndex[i]);
								vertex.oColor &= 0x00ffffff;
							}
						}
					}
				}
				else
					return;
			}

			m_processParams.vecTextureNames.clear();
			break;

		default:
			break;
		}
		
		vector<string> temp = m_processParams.vecTextureNames;
		if( mark > 3 )
			temp.clear();

		///将新的纹理写入拼接纹理中
		if( !pTerrain->CreateTerrainTextureAgain( deletetexnames, temp ) )
		{
			m_ReplaceTexNames.clear();
			MessageBox(NULL, "替换失败", "错误操作", MB_OK);
			return;
		}
		else
		{
			pTerrain->ReWriteTerrainBuffer( pTerrain->m_TerrainBufferIndexs, 7 );///1,2纹理同时处理
			m_ReplaceTexNames.empty();
		}

		MessageBox(0, "替换完毕", "正确", 0);
	}

	void CToolSetTileTexture::ReplaceTerrainTexture( const int& mark )
	{
		if ( CMapEditApp::GetInst()->GetOperator() == NULL )
			return;

		if( !this->SelectRelateContinueTex(mark, m_ReplaceTexNames) )
			return;

		switch (mark)
		{
		case 1:
		case 2:
			{
				this->ReplaceTerrainTexture(mark, m_ReplaceTexNames);
			}
			break;

		case 3:
			{
				if(m_ReplaceTexNames.empty())
				{
					MessageBox(NULL,"请选择需要被删除的四方连续图包","提示",MB_OK);
					return;
				}

				this->ReplaceTerrainTexture(mark, m_ReplaceTexNames);
			}
			break;

		case 4:
			{
				this->ReplaceTerrainTexture(mark, m_ReplaceTexNames);

			}
			break;

		case 5:
			{
				this->ReplaceTerrainTexture(mark, m_ReplaceTexNames);
			}
			break;

		default:
			break;
		}
	}

	bool CToolSetTileTexture::SelectRelateContinueTex(const int& mark, vector<string>& deletetexnames)
	{
		//从所有层都可以全铺操作；到只允许第1层进行全铺操作
		if ( this->GetLayerHot() > 1  ) 
		{
			MessageBox(NULL, "请选择第一层或第二层", "错误操作", MB_OK);
			return false;
		}

		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain == NULL )
			return false;

		DWORD dwGridCnt = pTerrain->GetGridCount();

		m_ReplaceGrids.clear();
		m_ReplaceTwoGrids.clear();

		int nlayhot = 0;
		nlayhot = this->GetLayerHot();
		string str = "";
		if( !m_ReplaceTexNames.empty() )
			str = m_ReplaceTexNames[0];

		switch (mark)
		{
		case 1:
		case 2:
			{
				COperator * pOperator = NULL;
				pOperator = CMapEditApp::GetInst()->GetOperator();
				if( pOperator == NULL )
					return false;

				for( DWORD i = 0; i < dwGridCnt; ++i )
				{
					SGrid &grid = pTerrain->GetGrid(i);


					if( grid.nodeInfo.strTextureNames[0].compare(str) == 0 )
						m_ReplaceGrids.push_back(i);
		
					if( grid.nodeInfo.strTextureNames[1].compare(str) == 0 )
						m_ReplaceTwoGrids.push_back(i);
				}
			}
			break;

		case 3:
			{
				if( deletetexnames.empty() )
				{
					MessageBox(0, "请按CLTRL+鼠标左键选中需要替换的四方连续图素包，提示", "", 0);
					return false;
				}
				string strcompare = deletetexnames[0];
				size_t npos = strcompare.find("1.bmp");
				if( npos != -1 )
				{
					strcompare = strcompare.substr(0, npos);
				}
				else
				{
					MessageBox(0, "所选的四方连续图的命名不符合规则，提示", "", 0);
					return false;
				}

				for( DWORD i = 0; i < dwGridCnt; ++i )
				{
					SGrid &grid = pTerrain->GetGrid(i);

					if( grid.nodeInfo.strTextureNames[0].find(strcompare) != -1 )
						m_ReplaceGrids.push_back(i);

					if( grid.nodeInfo.strTextureNames[1].find(strcompare) != -1 )
						m_ReplaceTwoGrids.push_back(i);
				}
			}
			break;

		case 4:
			return true;

		case 5:
			{
				return true;
			}
			return true;

		default:
			break;
		}
		
		if( !m_ReplaceGrids.empty() )
			return true;
		else
		{
			if( m_ReplaceTwoGrids.empty() )
			{
				MessageBox(0, "没有搜到相关格子，提示", "", 0);
				return false;
			}
			else
				return true;
		}
	}

	void CToolSetTileTexture::ResetCheckBoxTextureLayerOne(vector<string> & vecTextureNames)
	{
		m_pCheckBoxTextureLayerOne->SetTextureName(vecTextureNames);
	}

	void CToolSetTileTexture::ResetCheckBoxTextureLayerTwo(vector<string> & vecTextureNames)
	{
		m_pCheckBoxTextureLayerTwo->SetTextureName(vecTextureNames);
	}

	void CToolSetTileTexture::ResetCheckBoxTextureLayerThree(vector<string> & vecTextureNames)
	{
	}

	void CToolSetTileTexture::ResetCheckBoxTextureLayerForth(vector<string> & vecTextureNames)
	{
		//m_pCheckBoxTextureLayerForth->SetTextureName(vecTextureNames);
	}

	void CToolSetTileTexture::InitProcessParams(HWND hGroundView)
	{
		m_hGroundView = hGroundView;

		m_processParams.vecTextureNames.clear();
		CRenderDevice * pRender = CRenderDevice::GetInst();

		//创建画刷纹理对象
		string config("MEditorRes:DefaultBrush.dds");
		string str = IoSystem::GetInst()->GetFullPath(config);
		m_pBrushTexture = CRenderDevice::GetInst()->GetTexture(str);

		this->CreateCheckBox();
	}

	void CToolSetTileTexture::CreateCheckBox()
	{
		// create gui
		DWORD dwWorldWidth = CMainWindowTarget::GetInst()->GetWidth();
		DWORD dwWorldHeight = CMainWindowTarget::GetInst()->GetHeight();

		RECT rc = {0, 0, dwWorldWidth, dwWorldHeight};

		// check boxes
		// box 1
		rc.left = 10;
		rc.right = 50;
		rc.top = dwWorldHeight - 50;
		rc.bottom = dwWorldHeight - 10;
		m_pCheckBoxTextureLayerOne = new CCheckBox;
		m_pCheckBoxTextureLayerOne->Create("check1",&rc,BWS_VISIBLE,CMapGUIManager::GetInst()->GetMainWnd());

		// box 2
		rc.left = 55;
		rc.right = 95;
		rc.top = dwWorldHeight - 50;
		rc.bottom = dwWorldHeight - 10;
		m_pCheckBoxTextureLayerTwo = new CCheckBox;
		m_pCheckBoxTextureLayerTwo->Create("check2",&rc,BWS_VISIBLE,CMapGUIManager::GetInst()->GetMainWnd());

		//将纹理对应名字插进去
		vector<string> sv1, sv2;

		string config("MEditorRes:defaultlayerone.dds");
		string str = IoSystem::GetInst()->GetFullPath(config);
		sv1.push_back(str);
		
		config = "MEditorRes:defaultlayertwo.dds";
		str = IoSystem::GetInst()->GetFullPath(config);
		sv2.push_back(str);

		m_pCheckBoxTextureLayerOne->SetTextureName(sv1);
		m_pCheckBoxTextureLayerTwo->SetTextureName(sv2);
	}

	void CToolSetTileTexture::SetDirType(sqr::EDIR_TYPE e)
	{
		m_processParams.eDirType = e;
	}

	void CToolSetTileTexture::SetWayType(sqr::EWAY_TYPE e)
	{
		m_processParams.eWayType = e;
	}

	vector<int> & CToolSetTileTexture::GetBrushGridIndices()
	{
		static vector<int> vecBrushGridIndices;

		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		COperator * pOperator  = CMapEditApp::GetInst()->GetOperator();
		DWORD BrushSizeInX = this->GetBrushSizeInX();
		DWORD BrushSizeInZ = this->GetBrushSizeInZ();

		vecBrushGridIndices.clear();
		DWORD vertexIndex = 0;

		if ( pTerrain->IsValid(pOperator->GetGridIndexHot()) )
		{
			CVector3f v = *((CVector3f*)&pOperator->GetHitPoint());

			vertexIndex = pTerrain->GetGrid(pOperator->GetGridIndexHot()).dwVertexIndex[0];
			
			if( pTerrain->IsValidVertexIndex(vertexIndex) )
			{
				SVertex & vertex = pTerrain->GetVertex(vertexIndex);

				if 	(	
					vertex.vPosition.x - BrushSizeInX*GRID_SPACE < -FLT_MIN ||    
					vertex.vPosition.x + BrushSizeInX*GRID_SPACE > pTerrain->GetWidth()*GRID_SPACE ||
					vertex.vPosition.z - BrushSizeInZ*GRID_SPACE < -FLT_MIN ||    
					vertex.vPosition.z + BrushSizeInZ*GRID_SPACE > pTerrain->GetDepth()*GRID_SPACE
					)
				{
					return vecBrushGridIndices;
				}

				v = v - vertex.vPosition; 

				SGrid & grid = pTerrain->GetGrid(pOperator->GetGridIndexHot());

				//////////////////////////////////////////////////////////////////////////

				DWORD dwCnt = 4 * BrushSizeInX * BrushSizeInZ;

				int nGridIndex = pOperator->GetGridIndexHot() - BrushSizeInX - BrushSizeInZ * pTerrain->GetWidth();

				for ( DWORD j = 0; j < dwCnt; j++)
				{
					if ( j )
					{
						if ( !(j % (2 * BrushSizeInX)) ) 
							nGridIndex += pTerrain->GetWidth() - 2 * BrushSizeInX + 1;
						else
							++nGridIndex;
					}

					// check for valid grid
					if ( pTerrain->IsValid(nGridIndex) )
					{
						vecBrushGridIndices.push_back(nGridIndex);
					}
				}
			}
		}

		return vecBrushGridIndices;
	}

	DWORD CToolSetTileTexture::GetBrushSizeInX() 
	{ 
		return m_processParams.dwBrushSizeInX; 
	}

	void CToolSetTileTexture::SetBrushSizeInX(DWORD dw) 
	{ 
		m_processParams.dwBrushSizeInX = dw;	
	}

	void CToolSetTileTexture::IncBrushSizeInX() 
	{ 
		++m_processParams.dwBrushSizeInX; 
	}

	void CToolSetTileTexture::DecBrushSizeInX() 
	{ 
		--m_processParams.dwBrushSizeInX;
	}

	DWORD CToolSetTileTexture::GetBrushSizeInZ() 
	{ 
		return m_processParams.dwBrushSizeInZ; 
	}

	void CToolSetTileTexture::SetBrushSizeInZ(DWORD dw) 
	{ 
		m_processParams.dwBrushSizeInZ = dw;	
	}

	void CToolSetTileTexture::IncBrushSizeInZ() 
	{ 
		++m_processParams.dwBrushSizeInZ; 
	}

	void CToolSetTileTexture::DecBrushSizeInZ() 
	{ 
		--m_processParams.dwBrushSizeInZ;
	}

	void CToolSetTileTexture::ShowLayers(bool b)
	{
		m_pCheckBoxTextureLayerOne->ShowWnd(b);
		m_pCheckBoxTextureLayerTwo->ShowWnd(b);
	}

	void CToolSetTileTexture::ResetLayers(int winX,int winY,int winWidth,int winHeight)
	{
		if(m_pCheckBoxTextureLayerOne)
		{
			m_pCheckBoxTextureLayerOne->m_wndInfo.rcRect.left=winX+30;
			m_pCheckBoxTextureLayerOne->m_wndInfo.rcRect.top=winY+winHeight-60;
			m_pCheckBoxTextureLayerOne->m_wndInfo.rcRect.right=winX+60;
			m_pCheckBoxTextureLayerOne->m_wndInfo.rcRect.bottom=winY+winHeight-30;
		}
		if(m_pCheckBoxTextureLayerTwo)
		{
			m_pCheckBoxTextureLayerTwo->m_wndInfo.rcRect.left=winX+70;
			m_pCheckBoxTextureLayerTwo->m_wndInfo.rcRect.top=winY+winHeight-60;
			m_pCheckBoxTextureLayerTwo->m_wndInfo.rcRect.right=winX+100;
			m_pCheckBoxTextureLayerTwo->m_wndInfo.rcRect.bottom=winY+winHeight-30;
		}
	}

	CRiver * CToolSetTileTexture::GetRiver()
	{
		return m_processParams.pRiver;
	}

	void CToolSetTileTexture::SetRiver(CRiver * p)
	{
		m_processParams.pRiver = p;
	}

	void CToolSetTileTexture::BrushLayerOneTexture()
	{
		if ( IDYES == MessageBox(NULL, "是否全铺第一层纹理", "提示", MB_YESNO) )
		{
			CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
			if( pTerrain->IsWater() )
				return;

			//从所有层都可以全铺操作；到只允许第1层进行全铺操作
			if ( this->GetLayerHot() != 0  ) 
			{
				MessageBox(0, "只有第一层才能使用全铺功能", "提示", 0);
				return;
			}

			if ( m_processParams.vecTextureNames.empty() )
				return;

			string texname = "";
			size_t npos = 0;
			locale loc;
			for( size_t i = 0; i < m_processParams.vecTextureNames.size(); ++i )
			{
				texname = m_processParams.vecTextureNames[i];
				use_facet<ctype<TCHAR> >( loc ).tolower
					( &texname[0], &texname[texname.length()] );

				npos = texname.find("tools\\mapeditor");
				if( npos != -1 )
					return;
			}

			int dwGridCnt = pTerrain->GetGridCount(); 
			CCmdSetSingleTexture * pCmd = new CCmdSetSingleTexture;

			SProcessParams p = m_processParams;
			p.dwBrushSizeInX = pTerrain->GetWidth()/2;

			pCmd->SetProcessParams(p);				

			for ( int i = 0; i < dwGridCnt; i++ )
			{
				pCmd->AddGrid(i);
			}

			pCmd->Execute();
		}
		else
			return;
	}

	void CToolSetTileTexture::DeleteTexture()
	{
		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		int nLayerHot = this->GetLayerHot();

		CCmdDeleteTexture * pCmd = new CCmdDeleteTexture;

		pCmd->SetProcessParams(m_processParams);

		vector<int> & vecBrushGridIndices = this->GetBrushGridIndices();

		for ( size_t i = 0; i < vecBrushGridIndices.size(); ++i )
		{
			SGrid & grid = pTerrain->GetGrid(vecBrushGridIndices[i]);

			pCmd->AddGrid(vecBrushGridIndices[i]);

		}

		if (  pCmd->GetGridCnt() > 0 )
		{
			if ( pTerrain->IsWater() && nLayerHot == 0 )
			{
				pCmd->Execute();
			}

			if ( !pTerrain->IsWater() )
			{
				pCmd->Execute();
			}
		}
		else
		{
			SafeDelete(pCmd);
		}
	}

	void CToolSetTileTexture::PasteCopyTerrain()
	{
		if ( IDYES == MessageBox(NULL, "是否粘贴复制的地表信息", "提示", MB_YESNO) )
		{
			vector<int> pasteGridIndexs = this->GetBrushGridIndices();
			if( pasteGridIndexs.size() != m_copyGridIndexs.size() )
			{
				TCHAR name[256] = "";

				MessageBox(NULL, "复制的格子数与粘贴的格子数不一致", "提示", MB_OK);
				return;
			}

			CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
			if( pTerrain->IsWater() )
			{
				MessageBox(NULL, "当前操作应是地表而不是水", "提示", MB_OK);
				return;
			}

			set<DWORD> bufferindexs;
			size_t size = pasteGridIndexs.size();

			switch (m_eCopyTerrainType)
			{
			case ECT_ALL:
				{
					set<DWORD> dwVertexIndexs;
					int nVertexIndex = 0;
					for ( size_t i = 0; i < size; ++i )
					{
						SGrid &copygrid = pTerrain->GetGrid(m_copyGridIndexs[i]);
						SGrid &pastergrid = pTerrain->GetGrid(pasteGridIndexs[i]);
						bufferindexs.insert(pastergrid.dwBufferIndex);

						pastergrid.nodeInfo.strTextureNames[0] = copygrid.nodeInfo.strTextureNames[0];
						pastergrid.nodeInfo.strTextureNames[1] = copygrid.nodeInfo.strTextureNames[1];

						for ( int m = 0; m < 4; ++m )
						{
							nVertexIndex = pastergrid.dwVertexIndex[m];
							dwVertexIndexs.insert(nVertexIndex);
							const SVertex &copyvertex = pTerrain->GetVertex(copygrid.dwVertexIndex[m]);
							SVertex &pastervertex = pTerrain->GetVertex(nVertexIndex);

							pastervertex.oColor = copyvertex.oColor;
							pastervertex.bColor = copyvertex.bColor;
							pastervertex.sColor = copyvertex.sColor;
							pastervertex.vPosition.y = copyvertex.vPosition.y;
						}
					}

					set<DWORD>::iterator iter = dwVertexIndexs.begin();
					for ( iter; iter != dwVertexIndexs.end(); ++iter )
					{
						nVertexIndex = *iter;

						CTerrainAlgorithm::GetInst()->CalculateVertexNormal(nVertexIndex);
						CTerrainAlgorithm::GetInst()->ReCalculateGridsHeight(nVertexIndex);
					}

					if ( !bufferindexs.empty() )
					{
						pTerrain->ReWriteTerrainBuffer( bufferindexs, 0 );
						pTerrain->ReWriteTerrainBuffer( bufferindexs, 1 );
						pTerrain->ReWriteTerrainBuffer( bufferindexs, 2 );
						pTerrain->ReWriteTerrainBuffer( bufferindexs, 3 );
					}

					break;
				}

			case ECT_HEIGHT:
				{
					int nVertexIndex = 0;
					set<DWORD> dwVertexIndexs;
					for ( size_t i = 0; i < size; ++i )
					{
						SGrid &copygrid = pTerrain->GetGrid(m_copyGridIndexs[i]);
						SGrid &pastergrid = pTerrain->GetGrid(pasteGridIndexs[i]);
						bufferindexs.insert(pastergrid.dwBufferIndex);

						for ( int m = 0; m < 4; ++m )
						{
							nVertexIndex = pastergrid.dwVertexIndex[m];
							dwVertexIndexs.insert(nVertexIndex);
							const SVertex &copyvertex = pTerrain->GetVertex(copygrid.dwVertexIndex[m]);
							SVertex &pastervertex = pTerrain->GetVertex(nVertexIndex);

							pastervertex.vPosition.y = copyvertex.vPosition.y;
						}
					}

					set<DWORD>::iterator iter = dwVertexIndexs.begin();
					for ( iter; iter != dwVertexIndexs.end(); ++iter )
					{
						nVertexIndex = *iter;

						CTerrainAlgorithm::GetInst()->CalculateVertexNormal(nVertexIndex);
						CTerrainAlgorithm::GetInst()->ReCalculateGridsHeight(nVertexIndex);
					}

					if ( !bufferindexs.empty() )
					{
						pTerrain->ReWriteTerrainBuffer( bufferindexs, 0 );
					}

					break;
				}

			case ECT_VCOLOR:
				{
					int nVertexIndex = 0;
					for ( size_t i = 0; i < size; ++i )
					{
						SGrid &copygrid = pTerrain->GetGrid(m_copyGridIndexs[i]);
						SGrid &pastergrid = pTerrain->GetGrid(pasteGridIndexs[i]);
						bufferindexs.insert(pastergrid.dwBufferIndex);

						for ( int m = 0; m < 4; ++m )
						{
							nVertexIndex = pastergrid.dwVertexIndex[m];
							const SVertex &copyvertex = pTerrain->GetVertex(copygrid.dwVertexIndex[m]);
							SVertex &pastervertex = pTerrain->GetVertex(nVertexIndex);

							byte alpha = byte(pastervertex.oColor >> 24);

							pastervertex.oColor = copyvertex.oColor;
							pastervertex.oColor |= 0xff000000;
							pastervertex.oColor &= (DWORD)alpha << 24 | 0x00ffffff;
							pastervertex.bColor = pastervertex.oColor;
							pastervertex.sColor = copyvertex.sColor;
						}
					}

					if ( !bufferindexs.empty() )
					{
						pTerrain->ReWriteTerrainBuffer( bufferindexs, 2 );
					}

					break;
				}

			case ECT_TEXTURE:
				{
					int nVertexIndex = 0;
					for ( size_t i = 0; i < size; ++i )
					{
						SGrid &copygrid = pTerrain->GetGrid(m_copyGridIndexs[i]);
						SGrid &pastergrid = pTerrain->GetGrid(pasteGridIndexs[i]);
						bufferindexs.insert(pastergrid.dwBufferIndex);

						pastergrid.nodeInfo.strTextureNames[0] = copygrid.nodeInfo.strTextureNames[0];
						pastergrid.nodeInfo.strTextureNames[1] = copygrid.nodeInfo.strTextureNames[1];

						for ( int m = 0; m < 4; ++m )
						{
							nVertexIndex = pastergrid.dwVertexIndex[m];
							const SVertex &copyvertex = pTerrain->GetVertex(copygrid.dwVertexIndex[m]);
							SVertex &pastervertex = pTerrain->GetVertex(nVertexIndex);

							byte alpha = byte(copyvertex.oColor >> 24);

							pastervertex.oColor |= 0xff000000;
							pastervertex.oColor &= (DWORD)alpha << 24 | 0x00ffffff;
							pastervertex.bColor = pastervertex.oColor;
							pastervertex.sColor = copyvertex.sColor;
						}
					}

					if ( !bufferindexs.empty() )
					{
						pTerrain->ReWriteTerrainBuffer( bufferindexs, 0 );
						pTerrain->ReWriteTerrainBuffer( bufferindexs, 1 );
						pTerrain->ReWriteTerrainBuffer( bufferindexs, 2 );
					}

					break;
				}

			default:
				break;
			}

			bufferindexs.clear();
			m_copyGridIndexs.clear();
		}
	}

	void CToolSetTileTexture::SetAlphaType( sqr::EALPHA_TYPE eAlphaType )
	{
		m_processParams.eAlphaType = eAlphaType;
	}

	void CToolSetTileTexture::SetGridMaterialIndex( const int8 nMatIndex, bool bForce )
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain == NULL )
			return;

		CMapEditApp::GetInst()->GetOperator()->SetShowSpecialGridCueMat(true);

		vector<int> & vecBrushGridIndices = this->GetBrushGridIndices();
		set<DWORD> bufferindexs;
		size_t size = vecBrushGridIndices.size();
		for ( size_t i = 0; i < size; ++i )
		{
			SGrid & grid = pTerrain->GetGrid(vecBrushGridIndices[i]);
			bufferindexs.insert(grid.dwBufferIndex);
			grid.nMaterialIndex = nMatIndex;
			grid.bMatNotEffectByTileSets = bForce;
		}

		if( !bufferindexs.empty() )
			pTerrain->ReWriteTerrainBuffer( bufferindexs, 2 );
	}

	void CToolSetTileTexture::HideAllSpecialMaterialGrid() const
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain == NULL )
			return;

		static bool bShow = false;
		CMapEditApp::GetInst()->GetOperator()->SetShowSpecialGridCueMat(bShow);
		pTerrain->ReWriteTerrainBuffer( pTerrain->m_TerrainBufferIndexs, 2 );
		bShow = !bShow;
	}

	void CToolSetTileTexture::ShowCurLayerTexture()
	{
		string tex1 = "", tex2 = "", tex3 = "", tex4 = "";
		this->GetHotGridTextureInfo(tex1, tex2, tex3, tex4);

		string texname("");

		switch (m_processParams.nLayerHot)
		{
		case 0:
			texname = tex1;
			break;
		case 1:
			texname = tex2;
			break;
		default:
			break;
		}

		if( texname.empty() )
			return;

		locale loc = locale::global(locale(""));
		ShellExecute(m_hGroundView, "open", texname.c_str(), "", "", SW_SHOW);
		locale::global(loc);
	}

	void CToolSetTileTexture::GetHotGridTextureInfo( string& tex1, string& tex2, string& tex3, string& tex4 )
	{
		DWORD GridIndexHot = CMapEditApp::GetInst()->GetOperator()->GetGridIndexHot();

		CTerrainMesh * pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain )
		{
			if( pTerrain->IsValid(GridIndexHot) )
			{
				SGrid & grid = pTerrain->GetGrid(GridIndexHot);

				tex1 = grid.nodeInfo.strTextureNames[0];
				tex2 = grid.nodeInfo.strTextureNames[1];
				tex3 = grid.nodeInfo.strTextureNames[2];
				tex4 = grid.nodeInfo.strTextureNames[3];
			}

		}
	}

	void CToolSetTileTexture::SetAlphaStrength( float f )
	{
		int nLayHot = m_processParams.nLayerHot;

		if( nLayHot == 1 )
			m_fAlphaStrength = f;
		if( nLayHot == 2 )
			m_fAlphaSpecular = f;
	}

	float CToolSetTileTexture::GetAlphaStrength()
	{
		int nLayHot = m_processParams.nLayerHot;

		if( nLayHot == 1 )
			return m_fAlphaStrength;
		else if( nLayHot == 2 )
			return m_fAlphaSpecular;
		else
			return 16.0f;
	}

	void CToolSetTileTexture::RevertCliffTexCoordinate()
	{
		CTerrainMesh *pTerrainMesh;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;

		if ( pTerrainMesh->IsWater() )
			return;

		int nGridCnt = pTerrainMesh->GetGridCount();
		for ( int n = 0; n < nGridCnt; ++n )
		{
			SGrid &grid = pTerrainMesh->GetGrid(n);

			for (int i = 0; i < 4; ++i)
			{
				grid.uv[0][i].x = grid.uv[1][i].x;
				grid.uv[0][i].y = grid.uv[1][i].y;
			}
		}

		MessageBox(NULL,"恢复完毕","提示",MB_OK);
	}

	void CToolSetTileTexture::FindErrorMinMapFormat()
	{
		set<string> mapnames;
		vector<string> ddsnames = AutoSearchFile::GetInst()->GetAllFiles("map", ".dds");
		if( ddsnames.empty() )
			return;
		string texname = "";

		locale loc = locale::global(locale(""));

		size_t size = ddsnames.size();
		for (size_t i = 0; i < size; ++i)
		{
			texname = ddsnames[i];
			use_facet<ctype<TCHAR> >( loc ).tolower
				( &texname[0], &texname[texname.length()] );

			size_t npos = texname.find("\\minmap\\");
			if( npos != -1 )
			{
				ITexture *pMinMapTexture = NULL;
				pMinMapTexture = CRenderDevice::GetInst()->GetTexture(texname);

				if( pMinMapTexture != NULL )
				{
					eTextureFormat eFormat = pMinMapTexture->GetTexFormat();
					if( eFormat != TF_DXT1 )
					{
						string mapname = texname.substr(0, npos);
						mapnames.insert(mapname);
					}
				}
			}
		}

		locale::global(loc);//恢复全局locale	
	}

	void GetStringTextInfo(const string& srcText, string& strPrefix, string& strPostfix)
	{
		size_t npos = srcText.rfind(".");
		if( npos != -1 )
		{
			string strText = srcText.substr(0, npos);
			size_t nLen = strText.length();
			strPrefix = strText.substr(0, nLen-1);
			strPostfix = strText.substr(nLen-1, nLen);
		}
	}

	bool CToolSetTileTexture::IsSequenceGridTextures(const vector<string>& texnames)
	{
		if( texnames.size() != 4 )
			return false;

		string texname0 = texnames[0];
		string texname1 = texnames[1];
		string texname2 = texnames[2];
		string texname3 = texnames[3];

		//如果四个格子纹理相同，则不进行处理
		if( texname0 == texname1 && texname0 == texname2 && texname0 == texname3 )
			return false;

		string tex0Prefix = "", tex0Postfix = "",tex1Prefix = "", tex1Postfix = "",tex2Prefix = "", tex2Postfix = "",tex3Prefix = "", tex3Postfix = "";
		GetStringTextInfo(texname0, tex0Prefix, tex0Postfix);
		GetStringTextInfo(texname1, tex1Prefix, tex1Postfix);
		GetStringTextInfo(texname2, tex2Prefix, tex2Postfix);
		GetStringTextInfo(texname3, tex3Prefix, tex3Postfix);

		if( tex0Prefix == tex1Prefix && tex0Prefix == tex2Prefix && tex0Prefix == tex3Prefix )
		{
			if( strCompare.find(tex0Postfix) != -1 && strCompare.find(tex1Postfix) != -1 &&
				strCompare.find(tex2Postfix) != -1 && strCompare.find(tex3Postfix) != -1)
			{	
				return true;
			}
		}

		return false;
	}

	bool CToolSetTileTexture::NeiborGridIsSequenceTexture(const string& texname)
	{
		string texname0 = m_processParams.vecTextureNames[0];
		string texname1 = m_processParams.vecTextureNames[1];
		string texname2 = m_processParams.vecTextureNames[2];
		string texname3 = m_processParams.vecTextureNames[3];
		if( texname == texname0 || texname == texname1 || texname == texname2 || texname == texname3 )
			return true;

		return false;
	}

	void CToolSetTileTexture::CmdNeiborGridSequenceTexture(CCmdSetSerialTexture *pCmd, const int nGridIndex1, const int nGridIndex2, const int nNeiGridIndex1, const int nNeiGridIndex2)
	{
		CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		SGrid &grid1 = pTerrainMesh->GetGrid(nGridIndex1);
		SGrid &grid2 = pTerrainMesh->GetGrid(nGridIndex2);
		SGrid &neiborgrid1 = pTerrainMesh->GetGrid(nNeiGridIndex1);
		SGrid &neiborgrid2 = pTerrainMesh->GetGrid(nNeiGridIndex2);

		string strNeiGrid1Tex = neiborgrid1.nodeInfo.strTextureNames[m_processParams.nLayerHot];
		string strNeiGrid2Tex = neiborgrid2.nodeInfo.strTextureNames[m_processParams.nLayerHot];
		bool bNei1 = this->NeiborGridIsSequenceTexture(strNeiGrid1Tex);
		bool bNei2 = this->NeiborGridIsSequenceTexture(strNeiGrid2Tex);

		pCmd->AddGrid(nGridIndex1);
		pCmd->AddGrid(nGridIndex2);

// 		DebugOut("Do grid = %d\n", nGridIndex1);
// 		DebugOut("Do grid = %d\n", nGridIndex2);

		if( bNei1 )
		{
			if( strNeiGrid1Tex == strNeiGrid2Tex )
			{
				grid1.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strNeiGrid1Tex;
				grid2.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strNeiGrid1Tex;
			}
			else
			{
				if( bNei2 )
				{
					grid1.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strNeiGrid1Tex;
					grid2.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strNeiGrid1Tex;
					
					pCmd->AddGrid(nNeiGridIndex2);
					//DebugOut("Do grid = %d\n", nNeiGridIndex2);

					neiborgrid2.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strNeiGrid1Tex;

					//DebugOut("修改 %d, new: %s, old: %s\n", nNeiGridIndex2, strNeiGrid1Tex.c_str(), texname.c_str());
				}
				else
				{
					int nOffset = sqr::GetRandomInt(0, 3);
					string strTexName = m_processParams.vecTextureNames[nOffset];
					grid1.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strNeiGrid1Tex;
					grid2.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strNeiGrid1Tex;
				}
			}
		}
		else
		{
			if( bNei2 )
			{
				grid1.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strNeiGrid2Tex;
				grid2.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strNeiGrid2Tex;
 				
				pCmd->AddGrid(nNeiGridIndex1);
				//DebugOut("Do grid = %d\n", nNeiGridIndex1);
				neiborgrid1.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strNeiGrid2Tex;

//				DebugOut("修改 %d, new: %s, old: %s\n", nNeiGridIndex1, strNeiGrid2Tex.c_str(), texname.c_str());
			}
			else
			{
				int nOffset = sqr::GetRandomInt(0, 3);
				string strTexName = m_processParams.vecTextureNames[nOffset];
				grid1.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strTexName;
				grid2.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strTexName;
			}
		}
	}

	/*
	* z nGridIndex3  --------  nGridIndex2
	*				|        |
	*				|	     |
	*	nGridIndex0	|________| nGridIndex1   ---> x
	*/
	void CToolSetTileTexture::ReBrushSequenceGridTextures( const vector<int>& gridIndexs, const int nLayer )
	{
		CTerrainMesh *pTerrainMesh = NULL;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;

		SGrid &grid0 = pTerrainMesh->GetGrid(gridIndexs[0]);
		SGrid &grid1 = pTerrainMesh->GetGrid(gridIndexs[1]);
		SGrid &grid2 = pTerrainMesh->GetGrid(gridIndexs[2]);
		SGrid &grid3 = pTerrainMesh->GetGrid(gridIndexs[3]);

 		string texname0 = grid0.nodeInfo.strTextureNames[nLayer];
 		string texname1 = grid1.nodeInfo.strTextureNames[nLayer];
 		string texname2 = grid2.nodeInfo.strTextureNames[nLayer];
		string texname3 = grid3.nodeInfo.strTextureNames[nLayer];
		
		vector<string> texNames;
		if( texname0 != "" )
			texNames.push_back(texname0);
		if( texname1 != "" )
			texNames.push_back(texname1);
		if( texname2 != "" )
			texNames.push_back(texname2);
		if( texname3 != "" )
			texNames.push_back(texname3);

		if( this->IsSequenceGridTextures(texNames) )
		{
			grid0.nodeInfo.strTextureNames[nLayer] = texname3;
			grid1.nodeInfo.strTextureNames[nLayer] = texname3;
			grid2.nodeInfo.strTextureNames[nLayer] = texname3;
		}
	}

	void CToolSetTileTexture::PatchSceneSequenceGridTex()
	{
		CTerrainMesh *pTerrainMesh = NULL;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;

		if ( pTerrainMesh->IsWater() )
			return;

		MessageBox(NULL,"改变之前最好将地图文件提交，否则后果自负","提示",MB_OK);
		if ( IDNO == MessageBox(NULL, "是否进行批处理", "提示", MB_YESNO) )
			return;

		int nWidth = pTerrainMesh->GetWidth();
		int nDepth = pTerrainMesh->GetDepth();
		for ( int z = 0 ; z < nDepth;  )
		{
			for ( int x = 0 ; x < nWidth ;  )
			{
				vector<int> gridIndexs;

				int nGridIndex0 = x + z * nWidth;
				int nGridIndex1 = nGridIndex0 + 1;
				int nGridIndex2 = nGridIndex1 + nWidth;
				int nGridIndex3 = nGridIndex2 - 1;

				gridIndexs.push_back(nGridIndex0);
				gridIndexs.push_back(nGridIndex1);
				gridIndexs.push_back(nGridIndex2);
				gridIndexs.push_back(nGridIndex3);

				this->ReBrushSequenceGridTextures(gridIndexs, 0);
				this->ReBrushSequenceGridTextures(gridIndexs, 1);

				x +=2;
			}

			z +=2;
		}

		pTerrainMesh->ReWriteTerrainBuffer( pTerrainMesh->m_TerrainBufferIndexs, 3 );
		pTerrainMesh->ReWriteTerrainBuffer( pTerrainMesh->m_TerrainBufferIndexs, 4 );
		MessageBox(NULL,"处理完毕","提示",MB_OK);
	}

	void CToolSetTileTexture::CmdSequenceGridTex(CCmdSetSerialTexture *pCmd)
	{
		//map<int, string> renewgridIndexs;

		CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		int nTerrainWidth = pTerrainMesh->GetWidth();

		vector<int> vecBrushGridIndices = pCmd->GetBrushGridIndices();
		if( vecBrushGridIndices.empty() )
			return;

		int nGridIndex = vecBrushGridIndices[0];
		int nX = nGridIndex % nTerrainWidth; // 列
		int nZ = nGridIndex / nTerrainWidth; // 行
		int nColMod = nX % 2;
		int nRowMod = nZ % 2;
		int nType = -1;
		if( nRowMod == 0 )
		{
			if( nColMod == 0 )
				nType = 0;
			else
				nType = 1;
		}
		else
		{
			if( nColMod == 0 )
				nType = 2;
			else
				nType = 3;
		}

		SProcessParams  processParams = pCmd->GetProcessParams();

		int dwBrushWidth = processParams.dwBrushSizeInX * 2;
		int dwBrushDepth = processParams.dwBrushSizeInZ * 2;
		int nRowCompare  = dwBrushDepth - nRowMod;
		int nColCompare  = dwBrushWidth - nColMod;
		for( int z = nRowMod; z < nRowCompare; )
		{
			for( int x = nColMod; x < nColCompare; )
			{
				vector<int> gridIndexs;

				int nIndex0 = x + z * dwBrushWidth;
				int nIndex1 = nIndex0 + 1;
				int nIndex2 = nIndex1 + dwBrushWidth;
				int nIndex3 = nIndex2 - 1;

				int nGridIndex0 = vecBrushGridIndices[nIndex0];
				int nGridIndex1 = vecBrushGridIndices[nIndex1];
				int nGridIndex2 = vecBrushGridIndices[nIndex2];
				int nGridIndex3 = vecBrushGridIndices[nIndex3];

				pCmd->AddGrid(nGridIndex0);
				pCmd->AddGrid(nGridIndex1);
				pCmd->AddGrid(nGridIndex2);
				pCmd->AddGrid(nGridIndex3);

// 				DebugOut("Do grid = %d\n", nGridIndex0);
// 				DebugOut("Do grid = %d\n", nGridIndex1);
// 				DebugOut("Do grid = %d\n", nGridIndex2);
// 				DebugOut("Do grid = %d\n", nGridIndex3);

				SGrid &grid0 = pTerrainMesh->GetGrid(nGridIndex0);
				SGrid &grid1 = pTerrainMesh->GetGrid(nGridIndex1);
				SGrid &grid2 = pTerrainMesh->GetGrid(nGridIndex2);
				SGrid &grid3 = pTerrainMesh->GetGrid(nGridIndex3);

// 				renewgridIndexs.insert(make_pair(nGridIndex0, grid0.nodeInfo.strTextureNames[processParams.nLayerHot]));
// 				renewgridIndexs.insert(make_pair(nGridIndex1, grid1.nodeInfo.strTextureNames[processParams.nLayerHot]));
// 				renewgridIndexs.insert(make_pair(nGridIndex2, grid2.nodeInfo.strTextureNames[processParams.nLayerHot]));
// 				renewgridIndexs.insert(make_pair(nGridIndex3, grid3.nodeInfo.strTextureNames[processParams.nLayerHot]))

				int nOffset = sqr::GetRandomInt(0, 3);
				string strTexName = processParams.vecTextureNames[nOffset];

				grid0.nodeInfo.strTextureNames[processParams.nLayerHot] = strTexName;
				grid1.nodeInfo.strTextureNames[processParams.nLayerHot] = strTexName;
				grid2.nodeInfo.strTextureNames[processParams.nLayerHot] = strTexName;
				grid3.nodeInfo.strTextureNames[processParams.nLayerHot] = strTexName;

				//DebugOut("Normal %d, %d, %d, %d, %s\n", nIndex0, nIndex1, nIndex2, nIndex3, strTexName.c_str());

				x += 2;
			}

			z += 2;
		}

		///奇数第一列
		if( nColMod != 0 )
		{
			//不补格
			for ( int m = nRowMod; m < dwBrushDepth - nRowMod; m +=2 )
			{
				int nIndex1 = 0 + m * dwBrushWidth;
				int nGridIndex1 = vecBrushGridIndices[nIndex1];
				int nGridIndex2 = nGridIndex1 + nTerrainWidth;
				int nNeiborGridIndex1 = nGridIndex1 - 1;///grid0
				int nNeiborGridIndex2 = nGridIndex2 - 1;///grid3

				this->CmdNeiborGridSequenceTexture(pCmd, nGridIndex1, nGridIndex2, nNeiborGridIndex2, nNeiborGridIndex1);
// 				if( pTerrainMesh->IsValid(reNewCmdGridIndex) )
// 				{
// 					renewgridIndexs.insert(make_pair(reNewCmdGridIndex, texname));
// 					//DebugOut("相邻切换 %d, %s\n", reNewCmdGridIndex, texname.c_str());
// 				}
			}

			///补的，以图片左上角为参考点
			for ( int z = nRowMod; z < dwBrushDepth - nRowMod; z +=2 )
			{
				int nIndex0 = dwBrushWidth - 1 + z * dwBrushWidth;

				int nGridIndex0 = vecBrushGridIndices[nIndex0];
				int nGridIndex3 = nGridIndex0 + nTerrainWidth;
				int nNeiborGridIndex0 = nGridIndex0 + 1;///grid1
				int nNeiborGridIndex3 = nGridIndex3 + 1;///grid2

				//DebugOut("右: %d, %d， %d, %d\n",nGridIndex0, nNeiborGridIndex0,nNeiborGridIndex3, nGridIndex3 );

				this->CmdNeiborGridSequenceTexture(pCmd, nGridIndex0, nGridIndex3, nNeiborGridIndex0, nNeiborGridIndex3);
// 				if( pTerrainMesh->IsValid(reNewCmdGridIndex) )
// 				{
// 					renewgridIndexs.insert(make_pair(reNewCmdGridIndex, texname));
// 					//DebugOut("相邻切换 %d, %s\n", reNewCmdGridIndex, texname.c_str());
// 				}
			}
		}
		if( nRowMod != 0 )
		{
			int nIndex = dwBrushWidth * (dwBrushDepth-1);
			for ( int m = nColMod; m < dwBrushWidth - nColMod; m += 2)
			{
				int nGridIndex0 = vecBrushGridIndices[m+nIndex];
				int nGridIndex1 = nGridIndex0 + 1;
				int nNeiborGridIndex0 = nGridIndex0 + nTerrainWidth;///grid3
				int nNeiborGridIndex1 = nGridIndex1 + nTerrainWidth;///grid2

				//DebugOut("上: %d, %d， %d, %d\n",nGridIndex0, nGridIndex1,nNeiborGridIndex1, nNeiborGridIndex0 );

				this->CmdNeiborGridSequenceTexture(pCmd, nGridIndex0, nGridIndex1, nNeiborGridIndex0, nNeiborGridIndex1);
// 				if( pTerrainMesh->IsValid(reNewCmdGridIndex) )
// 				{
// 					renewgridIndexs.insert(make_pair(reNewCmdGridIndex, texname));
// 					//DebugOut("相邻切换 %d, %s\n", reNewCmdGridIndex, texname.c_str());
// 				}
			}

			///补的，以图片左上角为参考点
			for( int m = nColMod; m < dwBrushWidth - nColMod; m += 2 )
			{					
				int nGridIndex3 = vecBrushGridIndices[m];
				///四个格子中的坐上角格子是图片的左上角
				int nGridIndex2 = nGridIndex3 + 1;

				int nNeiborGridIndex3 = nGridIndex3 - nTerrainWidth;///grid0
				int nNeiborGridIndex2 = nGridIndex2 - nTerrainWidth;///grid1

				//DebugOut("下: %d, %d， %d, %d\n",nNeiborGridIndex3, nNeiborGridIndex2, nGridIndex2, nGridIndex3 );

// 				SGrid &grid2 = pTerrainMesh->GetGrid(nGridIndex2);
// 				SGrid &grid3 = pTerrainMesh->GetGrid(nGridIndex3);
// 
// 				int nOffset = sqr::GetRandomInt(0, 3);
// 				string strTexName = processParams.vecTextureNames[nOffset];
// 				grid2.nodeInfo.strTextureNames[processParams.nLayerHot] = strTexName;
// 				grid3.nodeInfo.strTextureNames[processParams.nLayerHot] = strTexName;

				this->CmdNeiborGridSequenceTexture(pCmd, nGridIndex3, nGridIndex2, nNeiborGridIndex3, nNeiborGridIndex2);
// 				if( pTerrainMesh->IsValid(reNewCmdGridIndex) )
// 				{
// 					//DebugOut("相邻切换 %d, %s\n", reNewCmdGridIndex, texname.c_str());
// 					renewgridIndexs.insert(make_pair(reNewCmdGridIndex, texname));
// 				}
			}
		}
		if( nType == 3 )
		{
			//left botton
			int nIndex = 0;
			int nGridIndex2 = vecBrushGridIndices[nIndex];
			int nGridIndex3 = nGridIndex2 - 1;
			int nGridIndex0 = nGridIndex3 - nTerrainWidth;
			int nGridIndex1 = nGridIndex2 - nTerrainWidth;

			//DebugOut("下: %d, %d， %d, %d\n",nGridIndex0, nGridIndex1, nGridIndex2, nGridIndex3 );

			pCmd->AddGrid(nGridIndex2);
			//DebugOut("Do grid = %d\n", nGridIndex2);

			SGrid grid0 = pTerrainMesh->GetGrid(nGridIndex0);
			SGrid grid1 = pTerrainMesh->GetGrid(nGridIndex1);
			SGrid grid2 = pTerrainMesh->GetGrid(nGridIndex2);
			SGrid grid3 = pTerrainMesh->GetGrid(nGridIndex3);
			string strtex0 = grid0.nodeInfo.strTextureNames[processParams.nLayerHot];
			string strtex1 = grid1.nodeInfo.strTextureNames[processParams.nLayerHot];
			string strtex2 = grid2.nodeInfo.strTextureNames[processParams.nLayerHot];
			string strtex3 = grid3.nodeInfo.strTextureNames[processParams.nLayerHot];
			if( strtex0 == strtex1 && strtex0 == strtex2 && strtex0 == strtex3 &&
				this->NeiborGridIsSequenceTexture(strtex0) )
			{
				SGrid& grid = pTerrainMesh->GetGrid(nGridIndex2);
				grid.nodeInfo.strTextureNames[processParams.nLayerHot] = strtex0;
			}
			else
			{
				int nOffset = sqr::GetRandomInt(0, 3);
				string strTexName = processParams.vecTextureNames[nOffset];
				SGrid& grid = pTerrainMesh->GetGrid(nGridIndex2);
				grid.nodeInfo.strTextureNames[processParams.nLayerHot] = strTexName;
			}


			//left top
			nIndex = 0 + dwBrushWidth * (dwBrushDepth -1 );
			nGridIndex1 = vecBrushGridIndices[nIndex];
			nGridIndex2 = nGridIndex1 + nTerrainWidth;
			nGridIndex0 = nGridIndex1 - 1;
			nGridIndex3 = nGridIndex0 + nTerrainWidth;
			//DebugOut("下: %d, %d， %d, %d\n",nGridIndex0, nGridIndex1, nGridIndex2, nGridIndex3 );

			pCmd->AddGrid(nGridIndex1);
			//DebugOut("Do grid = %d\n", nGridIndex1);

			grid0 = pTerrainMesh->GetGrid(nGridIndex0);
			grid1 = pTerrainMesh->GetGrid(nGridIndex1);
			grid2 = pTerrainMesh->GetGrid(nGridIndex2);
			grid3 = pTerrainMesh->GetGrid(nGridIndex3);
			strtex0 = grid0.nodeInfo.strTextureNames[processParams.nLayerHot];
			strtex1 = grid1.nodeInfo.strTextureNames[processParams.nLayerHot];
			strtex2 = grid2.nodeInfo.strTextureNames[processParams.nLayerHot];
			strtex3 = grid3.nodeInfo.strTextureNames[processParams.nLayerHot];
			if( strtex0 == strtex1 && strtex0 == strtex2 && strtex0 == strtex3 &&
				this->NeiborGridIsSequenceTexture(strtex0) )
			{
				SGrid& grid = pTerrainMesh->GetGrid(nGridIndex1);
				grid.nodeInfo.strTextureNames[processParams.nLayerHot] = strtex0;
			}
			else
			{
				int nOffset = sqr::GetRandomInt(0, 3);
				string strTexName = processParams.vecTextureNames[nOffset];
				SGrid& grid = pTerrainMesh->GetGrid(nGridIndex1);
				grid.nodeInfo.strTextureNames[processParams.nLayerHot] = strTexName;
			}

			//right botton
			nIndex = dwBrushWidth - 1;
			nGridIndex3 = vecBrushGridIndices[nIndex];
			nGridIndex0 = nGridIndex3 - nTerrainWidth;
			nGridIndex2 = nGridIndex3 + 1;
			nGridIndex1 = nGridIndex0 + 1;
			//DebugOut("下: %d, %d， %d, %d\n",nGridIndex0, nGridIndex1, nGridIndex2, nGridIndex3 );
			pCmd->AddGrid(nGridIndex3);
			//DebugOut("Do grid = %d\n", nGridIndex3);

			grid0 = pTerrainMesh->GetGrid(nGridIndex0);
			grid1 = pTerrainMesh->GetGrid(nGridIndex1);
			grid2 = pTerrainMesh->GetGrid(nGridIndex2);
			grid3 = pTerrainMesh->GetGrid(nGridIndex3);
			strtex0 = grid0.nodeInfo.strTextureNames[processParams.nLayerHot];
			strtex1 = grid1.nodeInfo.strTextureNames[processParams.nLayerHot];
			strtex2 = grid2.nodeInfo.strTextureNames[processParams.nLayerHot];
			strtex3 = grid3.nodeInfo.strTextureNames[processParams.nLayerHot];
			if( strtex0 == strtex1 && strtex0 == strtex2 && strtex0 == strtex3 &&
				this->NeiborGridIsSequenceTexture(strtex0) )
			{
				SGrid& grid = pTerrainMesh->GetGrid(nGridIndex3);
				grid.nodeInfo.strTextureNames[processParams.nLayerHot] = strtex0;
			}
			else
			{
				int nOffset = sqr::GetRandomInt(0, 3);
				string strTexName = processParams.vecTextureNames[nOffset];
				SGrid& grid = pTerrainMesh->GetGrid(nGridIndex3);
				grid.nodeInfo.strTextureNames[processParams.nLayerHot] = strTexName;
			}

			//right top
			nIndex = vecBrushGridIndices.size() - 1;
			nGridIndex0 = vecBrushGridIndices[nIndex];
			nGridIndex1 = nGridIndex0 + 1;
			nGridIndex3 = nGridIndex0 + nTerrainWidth;
			nGridIndex2 = nGridIndex3 + 1;
			//DebugOut("下: %d, %d， %d, %d\n",nGridIndex0, nGridIndex1, nGridIndex2, nGridIndex3 );
			pCmd->AddGrid(nGridIndex0);
			//DebugOut("Do grid = %d\n", nGridIndex0);

			grid0 = pTerrainMesh->GetGrid(nGridIndex0);
			grid1 = pTerrainMesh->GetGrid(nGridIndex1);
			grid2 = pTerrainMesh->GetGrid(nGridIndex2);
			grid3 = pTerrainMesh->GetGrid(nGridIndex3);
			strtex0 = grid0.nodeInfo.strTextureNames[processParams.nLayerHot];
			strtex1 = grid1.nodeInfo.strTextureNames[processParams.nLayerHot];
			strtex2 = grid2.nodeInfo.strTextureNames[processParams.nLayerHot];
			strtex3 = grid3.nodeInfo.strTextureNames[processParams.nLayerHot];
			if( strtex0 == strtex1 && strtex0 == strtex2 && strtex0 == strtex3 &&
				this->NeiborGridIsSequenceTexture(strtex0) )
			{
				SGrid& grid = pTerrainMesh->GetGrid(nGridIndex0);
				grid.nodeInfo.strTextureNames[processParams.nLayerHot] = strtex0;
			}
			else
			{
				int nOffset = sqr::GetRandomInt(0, 3);
				string strTexName = processParams.vecTextureNames[nOffset];
				SGrid& grid = pTerrainMesh->GetGrid(nGridIndex0);
				grid.nodeInfo.strTextureNames[m_processParams.nLayerHot] = strTexName;
			}

		}
	}

	void CToolSetTileTexture::BrushSingleTexture()
	{
		COperator   *pOperator = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		int nLayerHot = this->GetLayerHot();
		if ( nLayerHot == INVALID  )
			return; // do nothing

		if ( m_processParams.vecTextureNames.empty() )
			return;

		if( !pTerrain->IsWater() )
		{
			string texname = "";
			size_t npos = 0;
			locale loc;
			for( size_t i = 0; i < m_processParams.vecTextureNames.size(); ++i )
			{
				texname = m_processParams.vecTextureNames[i];
				use_facet<ctype<TCHAR> >( loc ).tolower
					( &texname[0], &texname[texname.length()] );

				npos = texname.find("tools\\mapeditor");
				if( npos != -1 )
					return;
			}
		}

		vector<int> & vecBrushGridIndices = this->GetBrushGridIndices();

		CCmdSetVertexAlpha   *pCmdSetVertexAlpha = NULL;
		CCmdSetSingleTexture *pCmd = NULL;
		pCmd = new CCmdSetSingleTexture;
		if( pCmd == NULL )
			return;

		pCmd->SetProcessParams(m_processParams);

		
		if ( nLayerHot == 1 )
		{
			if ( !m_bInSubGrid )
			{
				pCmdSetVertexAlpha = new CCmdSetVertexAlpha;

				if( pCmdSetVertexAlpha == NULL )
					return;

				pCmdSetVertexAlpha->SetGridIndexHot(pOperator->GetGridIndexHot());
				pCmdSetVertexAlpha->SetProcessParams(m_processParams);
			}
		}

		for ( size_t i = 0; i < vecBrushGridIndices.size(); ++i )
		{
			SGrid & grid = pTerrain->GetGrid(vecBrushGridIndices[i]);

			pCmd->AddGrid(vecBrushGridIndices[i]);

			// add vertex for alpha manipulation
			if ( nLayerHot == 1 )
			{
				if ( !m_bInSubGrid )
				{
					pCmdSetVertexAlpha->AddGrid(vecBrushGridIndices[i]);

					for ( int i = 0; i < 4; i++)
					{
						pCmdSetVertexAlpha->AddVertex(grid.dwVertexIndex[i]);
					}
				}
			}
		}

		if (  pCmd->GetGridCnt() > 0 )
		{
			if ( pTerrain->IsWater() && nLayerHot == 0 )
			{
				pCmd->Execute();
			}

			if ( !pTerrain->IsWater() )
			{
				pCmd->Execute();
			}
		}
		else
		{
			SafeDelete(pCmd);
		}

		if ( nLayerHot == 1 )
		{
			if ( !m_bInSubGrid )
			{
				if( pCmdSetVertexAlpha ->GetVertexCnt() > 0 )
					pCmdSetVertexAlpha->Execute();
				else
					SafeDelete(pCmdSetVertexAlpha);
			}
		}
	}

	void CToolSetTileTexture::BrushSerialTextures()
	{
		srand( (unsigned)time( NULL ) );

		COperator   *pOperator = CMapEditApp::GetInst()->GetOperator();
		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		int nLayerHot = this->GetLayerHot();
		if ( nLayerHot == INVALID  )
			return; // do nothing

		if ( m_processParams.vecTextureNames.empty() )
			return;

		if( !pTerrain->IsWater() )
		{
			string texname = "";
			size_t npos = 0;
			locale loc;
			for( size_t i = 0; i < m_processParams.vecTextureNames.size(); ++i )
			{
				texname = m_processParams.vecTextureNames[i];
				use_facet<ctype<TCHAR> >( loc ).tolower
					( &texname[0], &texname[texname.length()] );

				npos = texname.find("tools\\mapeditor");
				if( npos != -1 )
					return;
			}
		}

		vector<int> & vecBrushGridIndices = this->GetBrushGridIndices();

		CCmdSetVertexAlpha   *pCmdSetVertexAlpha = NULL;
		CCmdSetSerialTexture *pCmd = NULL;
		pCmd = new CCmdSetSerialTexture;
		if( pCmd == NULL )
			return;

		pCmd->SetProcessParams(m_processParams);
		pCmd->SetBrushGridIndices(vecBrushGridIndices);

		if ( nLayerHot == 1 )
		{
			if ( !m_bInSubGrid )
			{
				pCmdSetVertexAlpha = new CCmdSetVertexAlpha;

				if( pCmdSetVertexAlpha == NULL )
					return;

				pCmdSetVertexAlpha->SetGridIndexHot(pOperator->GetGridIndexHot());
				pCmdSetVertexAlpha->SetProcessParams(m_processParams);
			}
		}

		for ( size_t i = 0; i < vecBrushGridIndices.size(); ++i )
		{
			SGrid & grid = pTerrain->GetGrid(vecBrushGridIndices[i]);

			// add vertex for alpha manipulation
			if ( nLayerHot == 1 )
			{
				if ( !m_bInSubGrid )
				{
					pCmdSetVertexAlpha->AddGrid(vecBrushGridIndices[i]);

					for ( int i = 0; i < 4; i++)
					{
						pCmdSetVertexAlpha->AddVertex(grid.dwVertexIndex[i]);
					}
				}
			}
		}

		if (  vecBrushGridIndices.size() > 0 )
		{
			if ( pTerrain->IsWater() && nLayerHot == 0 )
			{
				pCmd->Execute();
			}

			if ( !pTerrain->IsWater() )
			{
				pCmd->Execute();
			}
		}
		else
		{
			SafeDelete(pCmd);
		}

		if ( nLayerHot == 1 )
		{
			if( pCmdSetVertexAlpha ->GetVertexCnt() > 0 )
				pCmdSetVertexAlpha->Execute();
			else
				SafeDelete(pCmdSetVertexAlpha);
		}
	}

	void CToolSetTileTexture::SwitchEnableTexInstead()
	{
		bool bEnableTexInstead = !CEditRenderPipeline::GetInst()->IsEnableTexInstead();
		if( bEnableTexInstead )
		{
			ITexture* pInsteadTex = NULL;
			pInsteadTex = CGraphic::GetInst()->GetSystemTexture(eSTT_WHITE);
			CEditRenderPipeline::GetInst()->EnableTexInstead(pInsteadTex);
		}
		else
			CEditRenderPipeline::GetInst()->DisableTexInstead();
	}
}
