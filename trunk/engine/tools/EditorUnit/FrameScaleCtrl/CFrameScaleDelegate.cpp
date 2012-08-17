#include "stdafx.h"
#include "CEventDelegate.h"
#include "CEventOwner.h"
#include "CEditModel.h"
#include "FrameScaleCtrl\CFrameScaleCtrlImp.h"
#include "FrameScaleCtrl\CFrameScaleDelegate.h"

sqr_tools::CFrameScaleDelegate::CFrameScaleDelegate( CFrameScaleCtrlImp* handle )
{
	m_pFrameScaleEditImp = handle;
}

sqr_tools::CFrameScaleDelegate::~CFrameScaleDelegate( void )
{

}

bool sqr_tools::CFrameScaleDelegate::MsgProc( const CMsg& msg )
{
	switch( msg.msgSource )
	{
	case MGS_SYSMSG:
		{
			switch(msg.msgType.sys_type)
			{
			case MGT_SYS_RENDER:
				Render();
				break;

			case MGT_SYS_RESIZE:
				Resize();
				break;
			}
		}
		break;
	}
	return true;
}

void sqr_tools::CFrameScaleDelegate::Render( void )
{
	m_pFrameScaleEditImp->Update();
}

void sqr_tools::CFrameScaleDelegate::Resize( void )
{

}