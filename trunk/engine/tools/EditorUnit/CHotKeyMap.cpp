#include "stdafx.h"
#include "CHotKeyMap.h"
#include "CEventState.h"
#include "CmdMgr.h"

void UN_DO(MSG_KEY_INF key)
{
	CCmdMgr::GetInst()->UnDo();
}

void RE_DO(MSG_KEY_INF key)
{
	CCmdMgr::GetInst()->ReDo();
}

void	CHotKeyMap::DefaultHotKey(void)
{
	RegCtrlHotKey(UN_DO,MGI_KEY_Z);
	RegCtrlHotKey(RE_DO,MGI_KEY_Y);
}

//---------------------------------------------------------------
void	CHotKeyMap::RegAltHotKey(HOT_KEY_CALL* pFun, MSG_KEY_INF key)
{
	m_AltCallBack[key] = pFun;
}

void	CHotKeyMap::RegCtrlHotKey(HOT_KEY_CALL* pFun, MSG_KEY_INF key)
{
	m_CtrlCallBack[key] = pFun;
}

void	CHotKeyMap::RegShiftHotKey(HOT_KEY_CALL* pFun, MSG_KEY_INF key)
{
	m_ShiftCallBack[key] = pFun;
}

CHotKeyMap::CHotKeyMap()
{
	m_AltCallBack.resize(0xFF,NULL);
	m_CtrlCallBack.resize(0xFF,NULL);
	m_ShiftCallBack.resize(0xFF,NULL);
	DefaultHotKey();
}

CHotKeyMap::~CHotKeyMap()
{

}

bool	CHotKeyMap::PressHotKey(MSG_KEY_INF key)
{
	if(sqr::CEventState::GetInst()->GetState(MGI_KEY_CtrlKey) == MGT_KEY_DOWN)
	{
		if(m_CtrlCallBack[key])
		{
			m_CtrlCallBack[key](key);
			return true;	
		}

	}

	if(sqr::CEventState::GetInst()->GetState(MGI_KEY_ShiftKey) == MGT_KEY_DOWN)
	{
		if(m_ShiftCallBack[key])
		{
			m_ShiftCallBack[key](key);
			return true;
		}
	}

	if(sqr::CEventState::GetInst()->GetState(MGI_KEY_Menu) == MGT_KEY_DOWN)
	{
		if(m_AltCallBack[key])
		{
			m_AltCallBack[key](key);
			return true;
		}
	}
	return false;
}