#include "stdafx.h"
#include "CRenderLight.h"
#include "CRenderGroup.h"
#include "CGraphic.h"

CRenderLight::CRenderLight( CRegionLI& LightInfo, float xPosition, float yPosition, float zPosition )
: m_LightInfo(LightInfo), m_xPosition(xPosition), m_yPosition(yPosition), m_zPosition(zPosition), m_uPreChangeTime(0)
{
}

CRenderLight::~CRenderLight(void)
{
}

int32 CRenderLight::GetDist( CPos& posCenterGrid )
{
	int32 x = posCenterGrid.x - GetGridByFloatPixel( m_xPosition );
	int32 y = posCenterGrid.y - GetGridByFloatPixel( m_zPosition );
	return x*x + y*y;
}

void CRenderLight::Render()
{
	return;
	CColor Color = CColor( 0xff, m_LightInfo.rColor, m_LightInfo.gColor, m_LightInfo.bColor );
	if( m_uPreChangeTime == 0 && m_LightInfo.Dyn_BrightTime )
	{
		if( (uint32)m_Timer.GetElapse()*256/2000 > m_LightInfo.Dyn_BrightTime )
		{
			if( CQuickRand::Rand2( 0, 256 ) < m_LightInfo.Dyn_ChangeProbability )
				m_uPreChangeTime = m_LightInfo.Dyn_BrightTime;
			else
				m_Timer.SetCheckPoint();
		}
	}
	else if( m_uPreChangeTime == m_LightInfo.Dyn_BrightTime && m_LightInfo.Dyn_ChangeTime )
	{
		float fDelTime = m_Timer.GetElapse()*256.0f/2000.0f - m_uPreChangeTime;
		if( fDelTime > static_cast<float>(m_LightInfo.Dyn_ChangeTime) )
		{
			m_uPreChangeTime += static_cast<uint16>(m_LightInfo.Dyn_ChangeTime);
			Color = CColor( 0xff, m_LightInfo.rDyn_Color, m_LightInfo.gDyn_Color, m_LightInfo.bDyn_Color );
		}
		else
		{
			float f = fDelTime/m_LightInfo.Dyn_ChangeTime;
			Color = Color*( 1 - f ) + CColor( 0xff, m_LightInfo.rDyn_Color, m_LightInfo.gDyn_Color, m_LightInfo.bDyn_Color )*f;
		}
	}
	else if( m_uPreChangeTime == m_LightInfo.Dyn_BrightTime + m_LightInfo.Dyn_ChangeTime && m_LightInfo.Dyn_DarkTime )
	{
		Color = CColor( 0xff, m_LightInfo.rDyn_Color, m_LightInfo.gDyn_Color, m_LightInfo.bDyn_Color );
		float fDelTime = m_Timer.GetElapse()*256.0f/2000.0f - m_uPreChangeTime;
		if( fDelTime > static_cast<float>(m_LightInfo.Dyn_DarkTime) )
			m_uPreChangeTime += static_cast<uint16>(m_LightInfo.Dyn_DarkTime);
	}
	else if( m_LightInfo.Dyn_ChangeTime )
	{
		float fDelTime = m_Timer.GetElapse()*256.0f/2000.0f - m_uPreChangeTime;
		if( fDelTime > (float)m_LightInfo.Dyn_ChangeTime )
		{
			m_uPreChangeTime = 0;
			m_Timer.SetCheckPoint();
		}
		else
		{
			float f = fDelTime/m_LightInfo.Dyn_ChangeTime;
			Color = Color*f + CColor( 0xff, m_LightInfo.rDyn_Color, m_LightInfo.gDyn_Color, m_LightInfo.bDyn_Color )*( 1 - f );
		}
	}
	else
	{
		m_uPreChangeTime = 0;
		m_Timer.SetCheckPoint();
	}

	CLightSystem* pLightSystem = CMainWindowTarget::GetInst()->GetGroup()->GetLightSystem();
	pLightSystem->AddDynamicPointLight( CVector3f( m_xPosition, m_yPosition, m_zPosition ), m_LightInfo.fRang, Color, 
		m_LightInfo.fAttenuation0, m_LightInfo.fAttenuation1, m_LightInfo.fAttenuation2 );
}
