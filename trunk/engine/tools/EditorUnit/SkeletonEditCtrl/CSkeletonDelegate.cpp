#include "stdafx.h"
#include "CEventDelegate.h"
#include "CEventOwner.h"
#include "CEditModel.h"
#include "SkeletonEditCtrl\CSkeletonEditImp.h"
#include "SkeletonEditCtrl\CSkeletonDelegate.h"

sqr_tools::CSkeletonDelegate::CSkeletonDelegate( CSkeletonEditImp* handle )
{
	m_pSkeletonEditImp = handle;
}

sqr_tools::CSkeletonDelegate::~CSkeletonDelegate( void )
{

}

bool sqr_tools::CSkeletonDelegate::MsgProc( const CMsg& msg )
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
			}
		}
		break;

	case MGS_MOUSEMSG:
		{
			switch(msg.msgType.sys_type)
			{
			case MGT_MOUSE_LBNDOWN:
				Pick(msg.msgInfo.mouse_info.X, msg.msgInfo.mouse_info.Y);
				break;
			}
		}
		break;
	}
	return true;
}

void sqr_tools::CSkeletonDelegate::Render( void )
{
	if (m_pSkeletonEditImp)
		m_pSkeletonEditImp->Render();
}

void sqr_tools::CSkeletonDelegate::Pick( int mouseX, int mouseY )
{
	if (m_pSkeletonEditImp)
		m_pSkeletonEditImp->PickBone(mouseX, mouseY);
}