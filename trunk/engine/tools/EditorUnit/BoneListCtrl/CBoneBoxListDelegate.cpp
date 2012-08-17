#include "stdafx.h"
#include "CEventDelegate.h"
#include "CEventOwner.h"
#include "CEditModel.h"
#include "BoneBoxListCtrl\CBoneBoxListCtrlImp.h"
#include "BoneBoxListCtrl\CBoneBoxListDelegate.h"

sqr_tools::CBoneBoxListDelegate::CBoneBoxListDelegate( CBoneBoxListCtrlImp* handle )
{
	m_pBonegBoxListImp = handle;
}

sqr_tools::CBoneBoxListDelegate::~CBoneBoxListDelegate( void )
{

}

bool sqr_tools::CBoneBoxListDelegate::MsgProc( const CMsg& msg )
{
	switch( msg.msgSource )
	{
	case MGS_SYSMSG:
		{
			switch(msg.msgType.sys_type)
			{
			case MGT_SYS_ADDBONE:
				Update(*(std::vector<std::string>*)(msg.msgInfo._handle));
				break;
			}
		}
		break;
	}

	return true;
}

void sqr_tools::CBoneBoxListDelegate::Update( std::vector<std::string> str_vector )
{
	m_pBonegBoxListImp->UpdateData(str_vector);
}