#include "stdafx.h"
#include "CEventDelegate.h"
#include "CEventOwner.h"
#include "CEditModel.h"
#include "BoundingBoxEditCtrl\CBoundingBoxEditImp.h"
#include "BoundingBoxEditCtrl\CBoundingBoxDelegate.h"

sqr_tools::CBoundingBoxDelegate::CBoundingBoxDelegate( CBoundingBoxEditImp* handle )
{
	m_pBoundingBoxImp = handle;
}

sqr_tools::CBoundingBoxDelegate::~CBoundingBoxDelegate( void )
{

}

bool sqr_tools::CBoundingBoxDelegate::MsgProc( const CMsg& msg )
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
	}

	return true;
}

void sqr_tools::CBoundingBoxDelegate::Render( void )
{
	m_pBoundingBoxImp->Render();
}