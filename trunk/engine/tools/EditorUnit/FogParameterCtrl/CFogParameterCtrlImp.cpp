#include "StdAfx.h"
#include "FogParameterCtrl/CFogParameterCtrlImp.h"
#include "ColorCtrl/CColorCtrlImp.h"
#include "BaseHelper.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"

DEF_UI_IMP(CFogParameterCtrlImp);
CFogParameterCtrlImp::CFogParameterCtrlImp(void* param)
{
	//m_pFogColorCtrlImp	= new CColorCtrlImp;

	m_nFogStart			= FOGSTART;
	m_nFogEnd			= FOGEND;
	m_fFogDesnity		= FOGDENSITY;
	m_eFogType			= FMT_LINEAR;
	m_bEnableFog		= true;
}

CFogParameterCtrlImp::~CFogParameterCtrlImp(void)
{
}

void CFogParameterCtrlImp::Update()
{
	CCtrlBase::Update();
}

void CFogParameterCtrlImp::SetFogType( const sqr_tools::FogModeType eType )
{
	if( m_eFogType != eType )
	{
		m_eFogType = eType;
		this->UpdateFog();
	}
}

void CFogParameterCtrlImp::SetFogStart( const int start )
{
	if( m_nFogStart != start )
	{
		m_nFogStart = start;
		this->UpdateFog();
	}
}

void CFogParameterCtrlImp::SetFogEnd( const int end )
{
	if( m_nFogEnd != end )
	{
		m_nFogEnd  = end;
		this->UpdateFog();
	}
}

void CFogParameterCtrlImp::SetFogDesnity( const float desnity )
{
	if( m_fFogDesnity != desnity )
	{
		m_fFogDesnity  = desnity;
		this->UpdateFog();
	}
}

void CFogParameterCtrlImp::SetFogIsEnable( const bool b )
{
	m_bEnableFog = b;
	this->UpdateFog();
}

bool CFogParameterCtrlImp::_ProcMsg( const CMsg& msg )
{
	switch(msg.msgSource)
	{
	case MGS_CUSTOM:
		{
			switch(msg.msgType.custom_type)
			{
			case MGT_UPDATE_FOGCOLOR:
				{
					MSG_INFO info = msg.msgInfo;
					DWORD color = info._iinfo;
					m_pFogColorCtrlImp->SetColor(color);
					this->UpdateFog();

					break;
				}

			case MGT_UPDATE_FOGPARA:
				{
					this->UpdateFog();
					break;
				}

			default:
				break;
			}
		}
	}
	return false;
}

void CFogParameterCtrlImp::UpdateFog()
{
	if( m_bEnableFog )
	{
		CColor fogColor = m_pFogColorCtrlImp->GetColor();
		if( m_eFogType == FMT_LINEAR )
		{
			CMainWindowTarget::GetInst()->GetGroup()->SetFog(sqr::FMT_LINEAR, fogColor, m_nFogStart, m_nFogEnd);
		}
		else
		{
			CMainWindowTarget::GetInst()->GetGroup()->SetFog(sqr::FMT_EXP, fogColor, m_fFogDesnity);
		}
	}

	CMainWindowTarget::GetInst()->GetGroup()->EnableFog(m_bEnableFog);
}

void CFogParameterCtrlImp::SetFogColorImp( CColorCtrlImp* pImp )
{
	m_pFogColorCtrlImp = pImp;
}