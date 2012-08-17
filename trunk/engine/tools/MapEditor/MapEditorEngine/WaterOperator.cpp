#include "stdafx.h"
#include "WaterOperator.h"
#include "River.h"
#include "ToolSetTileTexture.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "UserMsg.h"

namespace sqr
{
	//////////////////////////////////////////////////////////////////////////
	// manager
	//////////////////////////////////////////////////////////////////////////

	CWaterOperator::CWaterOperator()
	{
		m_bAddWave				= true;
		m_bVisualizeWaterBase	= true;
		m_bChangeWaterVetexAlpha= true;
		m_nWavePerGrid			= 2;
		m_nWaveSize				= (int)(GRID_SPACE*1.5);//96; modified by goodman
		m_nWaveOff				= (int)GRID_SPACE;//64; modified by goodman
		m_fWaveSpeed			= 1.5f;
		m_nWaveLife				= 100;

		m_strWaveTextureName	= "tile\\water\\wave.dds";
	}

	CWaterOperator::~CWaterOperator()
	{
		m_mapRiverSpecies.clear();
	}

	void CWaterOperator::SetWaveDensity( int n )
	{
		m_nWavePerGrid = n;
	}

	void CWaterOperator::SetWaveTexture( string strTextureFileName )
	{
		m_strWaveTextureName = strTextureFileName;

		if ( CMapEditApp::GetInst()->GetWater() )
		{
			CMapEditApp::GetInst()->GetWater()->UpdateRiverSideTexture();
		}
	}

	std::string CWaterOperator::GetWaveTextureName() const
	{
		return m_strWaveTextureName;
	}

	void CWaterOperator::SetWaveSize( int n )
	{
		m_nWaveSize = n;
	}

	void CWaterOperator::SetWaveOff( int n )
	{
		m_nWaveOff = n;
	}

	void CWaterOperator::SetWaveSpeed( float f )
	{
		m_fWaveSpeed = f;
	}

	void CWaterOperator::SetWaveLife( int n )
	{
		m_nWaveLife = n;
	}

	int CWaterOperator::GetWaveDensity() const
	{
		return m_nWavePerGrid;
	}

	int CWaterOperator::GetWaveSize() const
	{
		return m_nWaveSize;
	}

	int CWaterOperator::GetWaveOff() const
	{
		return m_nWaveOff;
	}

	float CWaterOperator::GetWaveSpeed() const
	{
		return m_fWaveSpeed;
	}

	int CWaterOperator::GetWaveLife() const
	{
		return m_nWaveLife;
	}

	float CWaterOperator::GetWaveSpeedForClient() const
	{
		return m_fWaveSpeed;
	}

	int CWaterOperator::GetWaveLifeForClient() const
	{
		return m_nWaveLife*1000;
	}

	bool CWaterOperator::IsAddWave() const
	{
		return m_bAddWave;
	}

	void CWaterOperator::SetAddWave( bool b )
	{
		m_bAddWave = b;
	}

	void CWaterOperator::NewRiver( string const & strItemName )
	{
		CRiver *pRiver = NULL;

		if ( m_mapRiverSpecies.find(strItemName) == m_mapRiverSpecies.end() )
		{
			pRiver = CSceneManager::GetInst()->AddNodeRiver(CToolSetTileTexture::GetInst()->GetTextureNamesHot());
			pRiver->SetItemName(strItemName);
			m_mapRiverSpecies[strItemName] = pRiver;
			if (CMapEditApp::GetInst()->GetWater())
			{
				CMapEditApp::GetInst()->GetWater()->AddRiver(pRiver);
			}
		}
		else
		{
			pRiver = m_mapRiverSpecies[strItemName]; 
		}

		CToolSetTileTexture::GetInst()->SetRiver(pRiver);
	}

	void CWaterOperator::RebirthRiver( string & strItemName )
	{
		::SendMessage(m_hTextureView, WM_TEXTURE_REBIRTH_RIVER, (WPARAM)strItemName.c_str(),0);
	}

	CRiver* CWaterOperator::GetRiver()
	{
		return CToolSetTileTexture::GetInst()->GetRiver();
	}
}