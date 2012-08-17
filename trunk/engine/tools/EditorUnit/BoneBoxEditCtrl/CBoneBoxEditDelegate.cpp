#include "stdafx.h"
#include "CEventDelegate.h"
#include "CEventOwner.h"
#include "CEditModel.h"
#include "BoneBoxEditCtrl\CBoneBoxEditCtrlImp.h"
#include "BoneBoxEditCtrl\CBoneBoxEditDelegate.h"

sqr_tools::CBoneBoxEditDelegate::CBoneBoxEditDelegate( CBoneBoxEditCtrlImp* handle )
{
	m_pBonegBoxEditImp = handle;
}

sqr_tools::CBoneBoxEditDelegate::~CBoneBoxEditDelegate( void )
{

}

bool sqr_tools::CBoneBoxEditDelegate::MsgProc( const CMsg& msg )
{
	std::string str;
	switch( msg.msgSource )
	{
	case MGS_SYSMSG:
		{
			switch(msg.msgType.sys_type)
			{
			case MGT_SYS_SELBONE:
				str = (char*)(msg.msgInfo._handle);
				Update(str);
				break;
			case MGT_SYS_RENDER:
				Render();
				break;
			}
		}
		break;
	}

	return true;
}

void sqr_tools::CBoneBoxEditDelegate::Update( std::string str )
{
	m_pBonegBoxEditImp->UpdateData(str);
}

void sqr_tools::CBoneBoxEditDelegate::Render( void )
{
	m_pBonegBoxEditImp->Render();
}