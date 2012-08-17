#include "stdafx.h"
#include "River.h"
#include "Render.h"
#include "CMapEditApp.h"
#include "MEMath.h"
#include "TerrainMeshGrid.h"
#include "TerrainMesh.h"
#include "WaterOperator.h"
#include "CWaterMgr.h"
#include "CGraphic.h"
#include "CRenderGroup.h"
#include "StringHelper.h"
#include "BaseHelper.h"
#include "CWaterRegion.h"
#include "CEditDataScene.h"
#include "CEditWaterMgr.h"

namespace sqr
{
#pragma region CRiverWave
	CRiverWave::CRiverWave()
	{	
		for( int i = 0; i < MAX_WAVENUM_PER_GRID; i++ )
		{
			m_WavePiecesReserved[i].vUV[0] = CVector2f(0.0f,1.0f);
			m_WavePiecesReserved[i].vUV[1] = CVector2f(1.0f,1.0f);
			m_WavePiecesReserved[i].vUV[2] = CVector2f(1.0f,0.0f);
			m_WavePiecesReserved[i].vUV[3] = CVector2f(0.0f,0.0f);
		}
	}	

	void CRiverWave::Create(int n, CVector3f vCenter)
	{	
		m_vCenter = vCenter;
		
		m_nBase = n;
		m_fBase = n*3.1415926f/4;

		CWaterOperator* pWaterOperator = CMapEditApp::GetInst()->GetWaterOperator();
		
		int nWaveSize	 = pWaterOperator->GetWaveSize();
		int nWaveOff	 = pWaterOperator->GetWaveOff();
		float fWaveSpeed = pWaterOperator->GetWaveSpeed();
		int nWaveLife	 = pWaterOperator->GetWaveLife();
		int nWaveDensity = pWaterOperator->GetWaveDensity();
		
		float fWaveSize = nWaveSize * 0.5f;
		m_vCorners[0] = CVector3f(-fWaveSize, 0.0f, -fWaveSize);
		m_vCorners[1] = CVector3f(fWaveSize, 0.0f, -fWaveSize);
		m_vCorners[2] = CVector3f(fWaveSize, 0.0f, fWaveSize);
		m_vCorners[3] = CVector3f(-fWaveSize, 0.0f, fWaveSize);
		
		for( int i = 0; i < MAX_WAVENUM_PER_GRID; i++ )
		{
			CMatrix mat;
			float f = m_fBase + sqr::GetRandomFloat(-WAVE_RANDOM_ROTATION,WAVE_RANDOM_ROTATION); 
			m_WavePiecesReserved[i].vDir = CVector3f(cos(f),0.0f,sin(f));
			mat.SetRotateY(-f);
			mat._41 = vCenter.x - m_WavePiecesReserved[i].vDir.x*nWaveOff;
			mat._42 = vCenter.y - m_WavePiecesReserved[i].vDir.y*nWaveOff;
			mat._43 = vCenter.z - m_WavePiecesReserved[i].vDir.z*nWaveOff;
			m_WavePiecesReserved[i].vCornersMoving[0] = m_vCorners[0] * mat;
			m_WavePiecesReserved[i].vCornersMoving[1] = m_vCorners[1] * mat;
			m_WavePiecesReserved[i].vCornersMoving[2] = m_vCorners[2] * mat;
			m_WavePiecesReserved[i].vCornersMoving[3] = m_vCorners[3] * mat;
			m_WavePiecesReserved[i].dwClr[3] = m_WavePiecesReserved[i].dwClr[2] = m_WavePiecesReserved[i].dwClr[1] =
			m_WavePiecesReserved[i].dwClr[0] = 0x00ffffff;
			m_WavePiecesReserved[i].bInUse = false;
			m_WavePiecesReserved[i].fSpeed = sqr::GetRandomFloat(fWaveSpeed/2, fWaveSpeed);
			m_WavePiecesReserved[i].nLife = sqr::GetRandomInt(nWaveLife/2, nWaveLife);
			m_WavePiecesReserved[i].nLifeElapsed = 0;
		}
		
		m_dwLifeIdleCycle = m_nWaveCycle = m_WavePiecesReserved[0].nLife/ nWaveDensity;
	}	

	void CRiverWave::Update()
	{
		m_dwLifeIdleCycle++;
		
		CWaterOperator *pWaterOperator = CMapEditApp::GetInst()->GetWaterOperator();
		int nWaveDensity = pWaterOperator->GetWaveDensity();
		
		for ( int i = 0; i < MAX_WAVENUM_PER_GRID -  nWaveDensity; ++i )
		{
			m_WavePiecesReserved[MAX_WAVENUM_PER_GRID - i - 1].bInUse = false;
		}
		
		int nWaveSize		= pWaterOperator->GetWaveSize();
		int nWaveOff		= pWaterOperator->GetWaveOff();
		float fWaveSpeed	= pWaterOperator->GetWaveSpeed();
		int nWaveLife		= pWaterOperator->GetWaveLife();
		
		float fWaveSize = nWaveSize * 0.5f;
		m_vCorners[0] = CVector3f(-fWaveSize,0.0f,-fWaveSize);
		m_vCorners[1] = CVector3f(fWaveSize,0.0f,-fWaveSize);
		m_vCorners[2] = CVector3f(fWaveSize,0.0f,fWaveSize);
		m_vCorners[3] = CVector3f(-fWaveSize,0.0f,fWaveSize);
		
		for ( int i = 0; i < nWaveDensity; i++ )
		{
			if ( m_WavePiecesReserved[i].bInUse )
			{
				if ( m_WavePiecesReserved[i].nLife < m_WavePiecesReserved[i].nLifeElapsed )
				{
					m_WavePiecesReserved[i].bInUse = false;
					continue;
				}
				
				for ( int j = 0; j < 4; ++j )
				{
					m_WavePiecesReserved[i].vCornersMoving[j] += m_WavePiecesReserved[i].fSpeed*m_WavePiecesReserved[i].vDir; 
				}
				
				m_WavePiecesReserved[i].nLifeElapsed++;
				
				float fPercent = float(m_WavePiecesReserved[i].nLifeElapsed)/m_WavePiecesReserved[i].nLife;
				
				m_WavePiecesReserved[i].dwClr[3] = m_WavePiecesReserved[i].dwClr[2] = m_WavePiecesReserved[i].dwClr[1] =
					m_WavePiecesReserved[i].dwClr[0] = ( ((DWORD)min( abs( 0.5f - abs( 0.5f - fPercent ) )*511, 255 ) ) << 24 ) | 0xffffff;
			}
			else
			{
				if ( m_dwLifeIdleCycle > (DWORD)m_nWaveCycle )
				{			
					m_dwLifeIdleCycle -= m_nWaveCycle;
					m_WavePiecesReserved[i].bInUse = true;
					m_nWaveCycle = m_WavePiecesReserved[i].nLife/nWaveDensity;
					
					CMatrix mat;
					float f = m_fBase + sqr::GetRandomFloat(-WAVE_RANDOM_ROTATION,WAVE_RANDOM_ROTATION);
					m_WavePiecesReserved[i].vDir = CVector3f(cos(f),0.0f,sin(f));
					mat.SetRotateY(-f);
					mat._41 = m_vCenter.x - m_WavePiecesReserved[i].vDir.x*nWaveOff;
					mat._42 = m_vCenter.y - m_WavePiecesReserved[i].vDir.y*nWaveOff;
					mat._43 = m_vCenter.z - m_WavePiecesReserved[i].vDir.z*nWaveOff;
					m_WavePiecesReserved[i].vCornersMoving[0] = m_vCorners[0] * mat;
					m_WavePiecesReserved[i].vCornersMoving[1] = m_vCorners[1] * mat;
					m_WavePiecesReserved[i].vCornersMoving[2] = m_vCorners[2] * mat;
					m_WavePiecesReserved[i].vCornersMoving[3] = m_vCorners[3] * mat;
					m_WavePiecesReserved[i].fSpeed = sqr::GetRandomFloat(fWaveSpeed/2, fWaveSpeed);
					m_WavePiecesReserved[i].nLife = sqr::GetRandomInt(nWaveLife/2, nWaveLife);
					m_WavePiecesReserved[i].nLifeElapsed = 0;
					m_WavePiecesReserved[i].dwClr[3] = m_WavePiecesReserved[i].dwClr[2] = m_WavePiecesReserved[i].dwClr[1] =
						m_WavePiecesReserved[i].dwClr[0] = 0x00ffffff;
				}
			}
		}
	}

#pragma endregion CRiverWave
	//////////////////////////////////////////////////////////////////////////
	CWaterMgr* CRiver::s_pWaterMgr = NULL;
	CRiver::CRiver(ISceneNode * pParent, bool bIsVisible) : ISceneNode(pParent,bIsVisible)
	{
		m_nCnt = 0;
		m_dwTextureBaseIndex = INVALID;
		m_pRiverWaveTexture = NULL;
		m_bEnableFog = false;
		m_bIsVisible = true;
		m_pWaterSurface = NULL;
		if(NULL==s_pWaterMgr)
			s_pWaterMgr = CMapEditApp::GetInst()->GetDataScene()->GetWaterMgr();//new CWaterMgr(CMainWindowTarget::GetInst()->GetGroup());
	}

	void CRiver::Create(vector<string> vecTextureNames)
	{
		CRenderDevice * pRender = CRenderDevice::GetInst();

		m_strRiverWaveTextureName = CMapEditApp::GetInst()->GetWaterOperator()->GetWaveTextureName();
		
		m_vecTextureNames = vecTextureNames;

		m_pRiverWaveTexture = pRender->GetTexture(m_strRiverWaveTextureName);
		SVector<ITexture*> textures;
		for (index_t i = 0; i < vecTextureNames.size(); ++i)
		{
			if( pRender->GetTexture(vecTextureNames[i]) == NULL )
			{
				string log = vecTextureNames[i] + " 水的纹理资源创建失败，检查是否有这个资源";
				MessageBox(NULL,log.c_str(),"提示",MB_OK);
				return;
			}
			textures.push_back(pRender->GetTexture(vecTextureNames[i]));
		}
		CWaterMgr::WaterEffectType type = (textures.size() > 1) ? CWaterMgr::TextureAnimation : CWaterMgr::EnvironmentMapping;
		m_pWaterSurface = s_pWaterMgr->CreateSurface(type, textures);
	}

	void CRiver::UpdateWaveTexture()
	{
		m_strRiverWaveTextureName = CMapEditApp::GetInst()->GetWaterOperator()->GetWaveTextureName();
		m_pRiverWaveTexture = CRenderDevice::GetInst()->GetTexture(m_strRiverWaveTextureName);
	}

	void CRiver::Render()
	{
		if( m_pWaterSurface == NULL )
			return;

		CWaterRegion* pWaterRegion = (CWaterRegion*)s_pWaterMgr->CreateNode(0);
		if (!CMapEditApp::GetInst()->GetWaterOperator()->GetDoesVisualizeWaterBase())
		{
			m_pWaterSurface->SetHide(true);
			return;
		}
		else
		{
			m_pWaterSurface->SetHide(false);
		}
		CRenderDevice * pRender = CRenderDevice::GetInst();
		
		//////////////////////////////////////////////////////////////////////////
		// river center
		//////////////////////////////////////////////////////////////////////////
		if (!this->m_setDirtyRegion.empty())
		{
			CTerrainMesh* waterMesh = CMapEditApp::GetInst()->GetWater();
			for (index_t i = 0; i < (uint)waterMesh->GetRegionCount(); ++i)
			{
				if (this->m_setDirtyRegion.end() != this->m_setDirtyRegion.find(i))
					this->UpdateRegion(i);
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// river sides
		//////////////////////////////////////////////////////////////////////////
		int nWaveDensity = CMapEditApp::GetInst()->GetWaterOperator()->GetWaveDensity();
		uint index[] = {0, 3, 1, 3, 2, 1};
		CCamera* pCam = CMainWindowTarget::GetInst()->GetCamera();
		for (map<index_t, set<CRiverWavePtr> >::iterator itWaveSet = m_mapRegionWaves.begin();
			itWaveSet != m_mapRegionWaves.end(); ++itWaveSet)
		{
			// check if this region is visible
			if(!pWaterRegion->IsHasMesh(m_pWaterSurface,itWaveSet->first))
				continue;

			CWaterRegionMesh* regionMesh = pWaterRegion->GetMesh(m_pWaterSurface,itWaveSet->first);
			if (NULL == regionMesh || ClipStatus::Outside == pCam->cull(regionMesh->GetBoundingBox()))
				continue;

			// build vertex buffer
			vector<VerNCT> vertices;
			set<CRiverWavePtr>& waveSet = itWaveSet->second;
			for (set<CRiverWavePtr>::iterator it = waveSet.begin(); it != waveSet.end(); ++it)
			{
				CRiverWavePtr& pWave = *it;
			
				for ( int j = 0; j < nWaveDensity; j++ )
				{
					CRiverWave::SWavePiece & wavePiece = pWave->GetWavePiece(j);
					if ( !wavePiece.bInUse )
						continue;

					for (uint i = 0; i < 6; i++)
					{
						CVector3f& pos = wavePiece.vCornersMoving[index[i]];
						DWORD color = wavePiece.dwClr[0];
						CVector2f& uv = wavePiece.vUV[index[i]];
						vertices.push_back(VerNCT(pos.x, pos.y, pos.z, 0.0f, 1.0f, 0.0f,
							color, uv.x, uv.y));
					}
				}
			}
			// render
			if(vertices.empty())
				continue;
			pRender->DrawLayer3DSolidQuad(&m_pRS, &vertices.front(), m_pRiverWaveTexture, vertices.size() / 6);
			m_pRS->m_TexOP_S0 = TOP_SELECTARG1;
			m_pRS->m_TexArg1_S0 = TA_TEXTURE;
			m_pRS->m_AlpOP_S0 = TOP_MODULATE;
			m_pRS->m_AlpArg1_S0 = TA_DIFFUSE;
			m_pRS->m_AlpArg2_S0 = TA_TEXTURE;
			m_pRS->m_AlphaTestEnable = false;
			m_pRS->m_AlphaBlendEnable = true;
			m_pRS->m_SrcBlend         = BLEND_SRCALPHA;
			m_pRS->m_DestBlend        = BLEND_INVSRCALPHA;
			m_pRS->m_LightEnable = TRUE;
			m_pRS->m_ZTestEnable = true;
			m_pRS->m_ZWrite = false;
			m_pRS->m_Cull = CULLT_CCW;
		}// for in m_mapRegionWaves

		ISceneNode::Render();
	}

	void CRiver::UpdateFPSLimited()
	{
		// update wave
		CWaterRegion* pWaterRegion = (CWaterRegion*)s_pWaterMgr->CreateNode(0);
		CCamera* pCam = CMainWindowTarget::GetInst()->GetCamera();
		for (map<index_t, set<CRiverWavePtr> >::iterator itWaveSet = m_mapRegionWaves.begin();
			itWaveSet != m_mapRegionWaves.end(); ++itWaveSet)
		{
			// check if this region is visible
			if(!pWaterRegion->IsHasMesh(m_pWaterSurface,itWaveSet->first))
				continue;
			CWaterRegionMesh* regionMesh = pWaterRegion->GetMesh(m_pWaterSurface,itWaveSet->first);
			if (NULL == regionMesh || !pCam->IsVisible(regionMesh->GetBoundingBox()))
				continue;
			set<CRiverWavePtr>& waveSet = itWaveSet->second;
			for_each(waveSet.begin(), waveSet.end(), mem_fun(&CRiverWave::Update));
		}
	}

	DWORD CRiver::SaveTexture(FILE * fpResSet, DWORD dwTextureBaseIndex)
	{
		m_dwTextureBaseIndex = dwTextureBaseIndex;
		
		string str;
		
		string temp = "tile\\水\\wave.dds";
						
		static const basic_string <TCHAR>::size_type npos = -1;
			
		for ( size_t i = 0; i < m_vecTextureNames.size(); ++i )
		{
			str = m_vecTextureNames[i];
			size_t tga_pos;
			sqr::tolower(str);
			if ((tga_pos = str.rfind(".tga")) != string::npos)
			{
				str = str.substr(0, tga_pos);
				str += ".dds";
			}

			if ( str.find("MapEditor") != npos || str.find("mapeditor") != npos || str.find("MAPEDITOR") != npos )
			{
				str = temp;																
			}
			
			int n = str.length();
			fwrite(&n,sizeof(int),1,fpResSet);
			fwrite(str.c_str(),n,1,fpResSet);
		}
		
		return this->GetTextureCount();
	}

	vector<string> CRiver::GetTextureNams(DWORD dwTextureBaseIndex)
	{
		m_dwTextureBaseIndex = dwTextureBaseIndex;
		string temp = "tile\\水\\wave.dds";
		string str;
		for ( size_t i = 0; i < m_vecTextureNames.size(); ++i )
		{
			str = m_vecTextureNames[i];
			size_t tga_pos;
			sqr::tolower(str);
			if ((tga_pos = str.rfind(".tga")) != string::npos)
			{
				str = str.substr(0, tga_pos);
				str += ".dds";
			}

			if ( str.find("mapeditor") != -1 )
			{
				str = temp;																
			}
		}

		return m_vecTextureNames;
	}

	//------------------------------------------------------------------------------
	CRiver::~CRiver()
	{
		m_pRiverWaveTexture = NULL;
		m_pRS = NULL;
	}

	//------------------------------------------------------------------------------
	void
	CRiver::UpdateRegion( index_t regionIndex )
	{
		CTerrainMesh* waterMesh = CMapEditApp::GetInst()->GetWater();
		SRegion& region = waterMesh->GetRegion(regionIndex);
		vector<VerColorTex> vertices;
		for (vector<int>::iterator it = region.vGridIndices.begin();
			it != region.vGridIndices.end(); ++it)
		{
			SGrid& grid = waterMesh->GetGrid(*it);
			if (this != grid.pRiver)
			{
				continue; // not this river
			}
			int index[4] = {1, 0, 2, 3};
			for (index_t i = 0; i < 4; ++i)
			{
				SVertex& vertex = waterMesh->GetVertex(grid.dwVertexIndex[index[i]]);
				vertices.push_back(VerColorTex(vertex.vPosition.x,
					vertex.vPosition.y, vertex.vPosition.z, vertex.oColor,
					grid.uv[0][index[i]].x, grid.uv[0][index[i]].y));
			}
		}
		
		CWaterRegion* pWaterRegion = (CWaterRegion*)s_pWaterMgr->CreateNode(0);
		// empty water region should be removed
		if (vertices.empty())
		{			
			pWaterRegion->ReleaseMesh(m_pWaterSurface,regionIndex);
			//m_pWaterSurface->RemoveRegionMesh(regionIndex);
			return;
		}
		// write to mesh buffer
		CWaterRegionMesh* regionMesh = pWaterRegion->GetMesh(m_pWaterSurface,regionIndex);
		//if (NULL == regionMesh)
		//{
			/*regionMesh = m_pWaterSurface->CreateRegionMesh(regionIndex);*/
		CAxisAlignedBox aabb;
		SVertex& firstVertex = waterMesh->GetVertex(region.vVertexIndices.front());
		SVertex& lastVertex = waterMesh->GetVertex(region.vVertexIndices.back());
		aabb.setExtents(firstVertex.vPosition.x, firstVertex.vPosition.y, firstVertex.vPosition.z,
			lastVertex.vPosition.x, lastVertex.vPosition.y, lastVertex.vPosition.z);
		regionMesh->SetBoundingBox(aabb);
		//}
		CVertexHardwareBuffer* pBuffer = CHardwareBufferMgr::GetInst()->createVertexBuffer(sizeof(VerColorTex),
			vertices.size(), HBU_WRITE_ONLY); 

		VerColorTex* buffer = (VerColorTex*)pBuffer->lock(HBL_DISCARD);
		memcpy(buffer, &vertices.front(), sizeof(VerColorTex) * vertices.size());
		pBuffer->unlock();
		regionMesh->InitRegion(pBuffer,vertices.size() / 4);
		this->m_setDirtyRegion.erase(regionIndex);
	}

	void CRiver::RemoveWave( index_t regionIndex, const CRiverWavePtr& pWave )
	{
		m_mapRegionWaves[regionIndex].erase(pWave);
	}

	CRiverWave* CRiver::AddWave( index_t regionIndex )
	{
		CRiverWavePtr wave = new CRiverWave();
		m_mapRegionWaves[regionIndex].insert(wave);
		return wave;
	}
}