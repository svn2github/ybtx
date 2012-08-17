#include "stdafx.h"
#include "CEventState.h"
#include "CMsg.h"
#include "CHotKeyMap.h"

CEventState::CEventState()
{
	m_KeyState.resize(0xFF,MGT_KEY_UP);
	m_MouseState.resize(0xF,MGT_MOUSE_MOVE);
	CHotKeyMap::Create();
}

CEventState::~CEventState()
{
	CHotKeyMap::Destroy();
}

MSG_KEY_TYPE	CEventState::GetState(MSG_KEY_INF key_inf)
{
	return m_KeyState[key_inf];
}

MSG_MOUSE_TYPE	CEventState::GetState(MSG_MOUSE_TYPE mouse_inf)
{
	uint32 Idx = mouse_inf >> 4;
	return m_MouseState[Idx];
}

MSG_MOUSE_INF	CEventState::GetMousePosition(void)
{
	return m_MousePos;
}

bool	CEventState::UpdateState(const CMsg& msg)
{
	switch(msg.msgSource)
	{
	case MGS_KEYMSG:
		{
			m_KeyState[msg.msgInfo.key_info] = msg.msgType.key_type;
			if(msg.msgType.key_type == MGT_KEY_DOWN)
				return !CHotKeyMap::GetInst()->PressHotKey(msg.msgInfo.key_info);
		}
	case  MGS_MOUSEMSG:
		{
			uint32 Idx = msg.msgType.mouse_type >> 4;
			m_MouseState[Idx] = msg.msgType.mouse_type;
			m_MousePos = msg.msgInfo.mouse_info;
		}
	}
	return true;
}