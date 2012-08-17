#include "StdAfx.h"
#include "ColorCtrl/CColorCtrlImp.h"

DEF_UI_IMP(CColorCtrlImp);
CColorCtrlImp::CColorCtrlImp(void* param)
{
}

CColorCtrlImp::~CColorCtrlImp(void)
{
}

void CColorCtrlImp::Update()
{
	CCtrlBase::Update();
}


bool CColorCtrlImp::_ProcMsg( const CMsg& msg )
{
	switch(msg.msgSource)
	{
	case MGS_CUSTOM:
		{
			switch(msg.msgType.custom_type)
			{
			case MGT_UPDATE_COLOR:
				{
					MSG_INFO info = msg.msgInfo;
					m_dwColor = *(DWORD*)(info._handle);
					
					break;
				}
			}
		}
	}
	return false;
}

void CColorCtrlImp::SetColor( const DWORD& color )
{
	m_dwColor = color;
	this->Update();
}