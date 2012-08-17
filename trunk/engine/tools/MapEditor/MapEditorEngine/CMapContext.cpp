#include "stdafx.h"
#include "CMapContext.h"
#include "Engine.h"
#include "Operator.h"
#include "Operator.inl"
#include "CMapEditApp.h"
#include "MapGUIManager.h"
#include "CCameraManager.h"
#include "CMapEditorManage.h"

#include "CCoreViewImp.h"

void	CMapDelegate::_Init(CEventOwner* pOwner)
{
	//Active();
}

bool  CMapDelegate::MsgProc( const CMsg& msg )
{
	switch( msg.msgSource )
	{
	case MGS_SYSMSG:
		switch( msg.msgType.sys_type )
		{
		case MGT_SYS_INIT:
			if(m_pOwner)_Init(m_pOwner);
			return true;
			break;
		case MGT_SYS_RENDER:
			{
				//CEngine::GetInst()->Update();
			}
			return false;
		}
		break;
	}
	return true;
}