#include "StdAfx.h"
#include "CEditObserver.h"
#include "CEventDelegate.h"
#include "CRenderNode.h"
#include "CCtrlBaseObserver.h"
#include "CEditContext.h"


sqr_tools::CCtrlBaseObserver::CCtrlBaseObserver()
: CCtrlBase()
, CEditObserver()
{

}

sqr_tools::CCtrlBaseObserver::~CCtrlBaseObserver()
{

}

void CCtrlBaseObserver::ActiveNode( CRenderNode* pNode )
{
	
}

void CCtrlBaseObserver::Active( void )
{
	//这种监听控件默认是不能够被激活的
}

bool CCtrlBaseObserver::MsgProc(const CMsg& msg)
{
	switch( msg.msgSource )
	{
	case MGS_SYSMSG:
		switch( msg.msgType.sys_type )
		{
		case MGT_SYS_SELCHANGE:
			_Init(m_pOwnContext);	
			break;
		}
		break;
	}

	return CEventDelegate::MsgProc(msg);
}

void CCtrlBaseObserver::_Init(CEventOwner* pOwner)
{
	if(m_pOwnContext == pOwner)
	{
		if( m_pOwnContext->GetActiveObj() )
		{
			DWORD filter = Filter();
			if( filter == 0 || m_pOwnContext->GetActiveObj()->GetNodeType() == filter )
			{
				ActiveNode(m_pOwnContext->GetActiveObj());
			}
			else
				ActiveNode(NULL);
		}
	}
}

void CCtrlBaseObserver::SetContext(CEditContext* pContext)
{
	if( m_pOwnContext == pContext )
		return;
	m_pOwnContext = pContext;
	m_pOwnContext->RegisterDelegate(this);
}

DWORD	CCtrlBaseObserver::Filter(void)
{
	return 0;
}

