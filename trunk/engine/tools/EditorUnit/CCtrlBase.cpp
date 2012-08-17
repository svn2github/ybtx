#include "StdAfx.h"
#include "CCtrlBase.h"
#include "CCtrlImpFactory.h"

DEF_BASE_IMP(CCtrlBase);
CCtrlBase::CCtrlBase()
: CEventCollector(NULL)
, m_pOwnContext(NULL)
{
}

CCtrlBase::~CCtrlBase(void)
{
}

CCtrlBase*		CCtrlBase::CreateNewCtrl(void* param){ return NULL; }
void			CCtrlBase::Destroy(void) { delete this; }

bool CCtrlBase::_ProcMsg(const CMsg& msg )
{
	switch(msg.msgSource)
	{
	case MGS_UIMSG:
		{
			switch(msg.msgType.ui_type)
			{
			case MGT_UI_SIZE:
				{
					int Width=GET_X_LPARAM(msg.msgInfo._uinfo);
					int Height=GET_Y_LPARAM(msg.msgInfo._uinfo);
					return OnCtrlSize(Width,Height);
				}
			case MGT_UI_CREATE:
				{
					return OnCtrlCreate();
				}
			case MGT_UI_PAINT:
				{
					return OnCtrlPaint();
				}
			case MGT_UI_CLOSE:
				{
					return OnCtrlClose();
				}
			case MGT_UI_QUIT:
				{
					return OnCtrlQuit();
				}
			}
			break;
		}
	case MGS_KEYMSG:
		{
			switch(msg.msgType.key_type)
			{
			case MGT_KEY_DOWN:
				{
					return OnCtrlKeyDown(msg.msgInfo.key_info);
				}
			case MGT_KEY_UP:
				{
					return OnCtrlKeyUp(msg.msgInfo.key_info);
				}
			}
			break;
		}
	case MGS_MOUSEMSG:
		{
			switch(msg.msgType.mouse_type)
			{
			case MGT_MOUSE_MOVE:
				{
					return OnCtrlMouseMove(msg.msgInfo.mouse_info.X,msg.msgInfo.mouse_info.Y);
				}
			case MGT_MOUSE_LBNUP:
				{
					return OnCtrlLBtUp(msg.msgInfo.mouse_info.X,msg.msgInfo.mouse_info.Y);
				}
			case MGT_MOUSE_RBNUP:
				{
					return OnCtrlRBtUp(msg.msgInfo.mouse_info.X,msg.msgInfo.mouse_info.Y);
				}
			case MGT_MOUSE_MBNUP:
				{
					return OnCtrlMBtUp(msg.msgInfo.mouse_info.X,msg.msgInfo.mouse_info.Y);
				}
			case MGT_MOUSE_LBNDOWN:
				{
					return OnCtrlLBtDown(msg.msgInfo.mouse_info.X,msg.msgInfo.mouse_info.Y);
				}
			case MGT_MOUSE_RBNDOWN:
				{
					return OnCtrlRBtDown(msg.msgInfo.mouse_info.X,msg.msgInfo.mouse_info.Y);
				}
			case MGT_MOUSE_MBNDOWN:
				{
					return OnCtrlMBtDown(msg.msgInfo.mouse_info.X,msg.msgInfo.mouse_info.Y);
				}
			case MGT_MOUSE_LBNDCLK:
				{
					return OnCtrlLBtDLK(msg.msgInfo.mouse_info.X,msg.msgInfo.mouse_info.Y);
				}
			case MGT_MOUSE_RBNDCLK:
				{
					return OnCtrlRBtDLK(msg.msgInfo.mouse_info.X,msg.msgInfo.mouse_info.Y);
				}
			case MGT_MOUSE_MBNDCLK:
				{
					return OnCtrlMBtDLK(msg.msgInfo.mouse_info.X,msg.msgInfo.mouse_info.Y);
				}
			case MGT_MOUSE_WHEEL:
				{
					return OnCtrlMouseWheel(msg.msgInfo._iinfo);
				}
			}
			break;
		}
	default:
		OnCtrlDefaultMsg(msg);
	}
	return false;
}

bool CCtrlBase::OnCtrlDefaultMsg(const CMsg& msg)
{
	return true;
}

//UI MSG
bool CCtrlBase::OnCtrlSize(int width,int height)
{
	return true;
}
bool CCtrlBase::OnCtrlCreate()
{
	return true;
}
bool CCtrlBase::OnCtrlPaint()
{
	return true;
}
bool CCtrlBase::OnCtrlClose()
{
	return true;
}
bool CCtrlBase::OnCtrlQuit()
{
	return true;
}
//KEY MSG
bool CCtrlBase::OnCtrlKeyDown(MSG_KEY_INF key)
{
	return true;
}
bool CCtrlBase::OnCtrlKeyUp(MSG_KEY_INF key)
{
	return true;
}
bool CCtrlBase::OnCtrlKeyPress(MSG_KEY_INF key)
{
	return true;
}
//MOUSE MSG
bool CCtrlBase::OnCtrlMouseMove(int x,int y)
{
	return true;
}
bool CCtrlBase::OnCtrlLBtDown(int x,int y)
{
	return true;
}
bool CCtrlBase::OnCtrlLBtUp(int x,int y)
{
	return true;
}
bool CCtrlBase::OnCtrlLBtDLK(int x,int y)
{
	return true;
}
bool CCtrlBase::OnCtrlRBtDown(int x,int y)
{
	return true;
}
bool CCtrlBase::OnCtrlRBtUp(int x,int y)
{
	return true;
}
bool CCtrlBase::OnCtrlRBtDLK(int x,int y)
{
	return true;
}
bool CCtrlBase::OnCtrlMBtDown(int x,int y)
{
	return true;
}
bool CCtrlBase::OnCtrlMBtUp(int x,int y)
{
	return true;
}
bool CCtrlBase::OnCtrlMBtDLK(int x,int y)
{
	return true;
}
bool CCtrlBase::OnCtrlMouseWheel(int delta)
{
	return true;
}


void CCtrlBase::RegisterUICallBack( void* key, UICallBack* pCall )
{
	if(pCall)
		m_CallBackPool[key] = pCall;
	else
		m_CallBackPool.erase(key);
}

void CCtrlBase::Update(void)
{
	CallBackPool::iterator it,eit = m_CallBackPool.end();
	for( it = m_CallBackPool.begin(); it!=eit; ++it )
	{
		(*it->second)(it->first);
	}
}

void CCtrlBase::SetContext( CEditContext* pContext )
{
	m_pOwnContext = pContext;
}