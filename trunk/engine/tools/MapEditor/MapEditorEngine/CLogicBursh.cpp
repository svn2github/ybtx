#include "stdafx.h"
#include "CLogicBursh.h"
#include "render.h"
#include "CGraphic.h"
#include "BaseHelper.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "EditStateOpertiaonMgr.h"
#include "IoSystem.h"

namespace sqr
{
	CLogicBrush::CLogicBrush()
	{
		m_pBrushTexture = NULL;
		m_pRS			= NULL;
	}

	CLogicBrush::~CLogicBrush()
	{
		if( m_pBrushTexture )
			m_pBrushTexture = NULL;
		if( m_pRS )
			m_pRS = NULL;
	}

	void CLogicBrush::InitBrushParams( const DWORD dwBurshInX, const DWORD dwBrushInZ, const DWORD color, const string& strBurshTexName )
	{
		m_dwBrushSizeInX = dwBurshInX;
		m_dwBrushSizeInZ = dwBrushInZ;
		m_dwColor		 = color;

		string config("MEditorRes:DefaultBrush.dds");
		string str = IoSystem::GetInst()->GetFullPath(config);
		m_pBrushTexture  = CRenderDevice::GetInst()->GetTexture(str);
	}

	void CLogicBrush::SetBrushTextureName( const string& strTexName )
	{
		if( m_pBrushTexture )
			SafeDelete(m_pBrushTexture);

		m_pBrushTexture  = CRenderDevice::GetInst()->GetTexture(strTexName);
	}

	DWORD CLogicBrush::GetBrushSizeInX()
	{
		return m_dwBrushSizeInX;
	}

	void CLogicBrush::SetBrushSizeInX( DWORD dw )
	{
		m_dwBrushSizeInX = dw;
	}

	void CLogicBrush::IncBrushSizeInX()
	{
		++m_dwBrushSizeInX;
	}

	void CLogicBrush::DecBrushSizeInX()
	{
		if( m_dwBrushSizeInX > 1 )
			--m_dwBrushSizeInX;
	}

	DWORD CLogicBrush::GetBrushSizeInZ()
	{
		return m_dwBrushSizeInZ;
	}

	void CLogicBrush::SetBrushSizeInZ( DWORD dw )
	{
		m_dwBrushSizeInZ = dw;
	}

	void CLogicBrush::IncBrushSizeInZ()
	{
		++m_dwBrushSizeInZ;
	}

	void CLogicBrush::DecBrushSizeInZ()
	{
		if( m_dwBrushSizeInZ > 1 )
			--m_dwBrushSizeInZ;
	}

	vector<int> & CLogicBrush::GetBrushGridIndices()
	{
		static vector<int> vecBrushGridIndices;
		vecBrushGridIndices.clear();

		CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain == NULL )
			return vecBrushGridIndices;

		DWORD BrushSizeInX = m_dwBrushSizeInX;
		DWORD BrushSizeInZ = m_dwBrushSizeInZ;
		int nHotGridIndex  = CEditStateOpertiaonMgr::GetInst()->GetHotGridIndex();
		int width = pTerrain->GetWidth();
		int nGridIndex = 0;

		if ( pTerrain->IsValid(nHotGridIndex) )
		{
			if( CEditStateOpertiaonMgr::GetInst()->GetEditState() == CEditStateOpertiaonMgr::EEDIT_STATE::EES_REGION )
			{
				for( DWORD i = 0; i < BrushSizeInZ; ++i )
				{
					for ( DWORD j = 0; j < BrushSizeInX; ++j )
					{
						nGridIndex = nHotGridIndex + j + i * width ;

						if(pTerrain->IsValid(nGridIndex))
							vecBrushGridIndices.push_back(nGridIndex);
					}
				}
			}
			else
			{
				DWORD dwCnt = 4 * BrushSizeInX * BrushSizeInZ;

				nGridIndex = nHotGridIndex - BrushSizeInX - BrushSizeInZ * width;
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
					{
						vecBrushGridIndices.push_back(nGridIndex);
					}
				}
			}
		}

		return vecBrushGridIndices;
	}

	vector<int> & CLogicBrush::GetBrushSubRegionIndices()
	{
		static vector<int> vecBrushSubRgnIndices;
		vecBrushSubRgnIndices.clear();

		CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if( pTerrain == NULL )
			return vecBrushSubRgnIndices;

		DWORD BrushSizeInX = m_dwBrushSizeInX;
		DWORD BrushSizeInZ = m_dwBrushSizeInZ;
		int nHotGridIndex  = CEditStateOpertiaonMgr::GetInst()->GetHotGridIndex();
		int width = pTerrain->GetWidthInRegion() * REGION_STEP_DIVIDE;
		int nDivideRgnIndex = 0;

		if ( pTerrain->IsValid(nHotGridIndex) )
		{
			SGrid &hotgrid = pTerrain->GetGrid(nHotGridIndex);
			int nHotDivideRgnIndex = pTerrain->GetDivideRgnIndexByGridIndex(nHotGridIndex);

			for( DWORD i = 0; i < BrushSizeInZ; ++i )
			{
				for ( DWORD j = 0; j < BrushSizeInX; ++j )
				{
					nDivideRgnIndex = nHotDivideRgnIndex + j + i * width ;
					if( !pTerrain->IsSubRegionValid(nDivideRgnIndex) )
						continue;

					int mod = nHotDivideRgnIndex % width;
					bool b = mod < width;
					//DebugOut("subregion %d, mod = %d, width = %d, tiaojian = %d\n", nDivideRgnIndex, mod, width, b);
					if( mod < width )
						vecBrushSubRgnIndices.push_back(nDivideRgnIndex);
// 					else
// 						int tt = 0;
				}
			}
		}

		return vecBrushSubRgnIndices;
	}

	void CLogicBrush::Render()
	{
		CRenderDevice*	pRender		= CRenderDevice::GetInst();
		CTerrainMesh*	pTerrain	= CMapEditApp::GetInst()->GetEditingMesh();

		// for brush
		static CVector3f	vertex[VERTEX_BATCH_SIZE];
		static CVector2f	uv0[VERTEX_BATCH_SIZE];
		static VerColorTex	rvertex[VERTEX_BATCH_SIZE];

		DWORD				vecColor[4];
		vecColor[0] = vecColor[1] = vecColor[2] = vecColor[3] = m_dwColor;

		vector<int>& vecBrushGridIndices = this->GetBrushGridIndices();
		size_t cnt = vecBrushGridIndices.size();

		DWORD dwStepInX = 2 * m_dwBrushSizeInX;   
		DWORD dwStepInZ = 2 * m_dwBrushSizeInZ; 
		if( CEditStateOpertiaonMgr::GetInst()->GetEditState() == CEditStateOpertiaonMgr::EEDIT_STATE::EES_REGION )
		{
			dwStepInX = m_dwBrushSizeInX;   
			dwStepInZ = m_dwBrushSizeInZ; 
		}

		float fStepInX  = 1.0f / dwStepInX;
		float fStepInZ  = 1.0f / dwStepInZ;
		size_t drawnCnt = 0, n = 0;

		for (n , drawnCnt; n < cnt; n++, drawnCnt++ )
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

			int nStepsInX = n % dwStepInX;
			int nStepsInZ = dwStepInZ - n / dwStepInX;

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
			m_pRS->m_Cull = CULLT_CCW;
		}
	}
}