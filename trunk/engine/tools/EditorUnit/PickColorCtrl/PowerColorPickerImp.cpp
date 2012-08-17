#include "StdAfx.h"
#include "PickColorCtrl/PowerColorPickerImp.h"

DEF_UI_IMP(CPowerColorPickerImp);
CPowerColorPickerImp::CPowerColorPickerImp(void* param)
{
}

CPowerColorPickerImp::~CPowerColorPickerImp(void)
{
}

void CPowerColorPickerImp::SetCurPickColor( const uint8 r, const uint8 g, const uint8 b )
{
	m_curPickColor.R = r;
	m_curPickColor.G = g;
	m_curPickColor.B = b;
}

void CPowerColorPickerImp::Update()
{
	CCtrlBase::Update();
}


bool CPowerColorPickerImp::_ProcMsg( const CMsg& msg )
{
	switch(msg.msgSource)
	{
	case MGS_CUSTOM:
		{
			switch(msg.msgType.custom_type)
			{
			case MGT_UPDATE_KEY:
				{
					MSG_INFO info = msg.msgInfo;

					break;
				}
			}
		}
	}
	return false;
}

