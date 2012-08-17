#include "stdafx.h"
#include "CEventOwner.h"
#include "IApplication.h"
#include "ContextMsg.h"

CEventOwner::CEventOwner()
{
	m_bIsRunning = true;
	m_ActiveDelegate = NULL;
}

CEventOwner::~CEventOwner()
{

}

bool CEventOwner::IsEmpty()
{
	return m_qMsg.empty();
}

void CEventOwner::DoNext()
{
	CMsg msg = m_qMsg.front();
	m_qMsg.pop();
	switch(msg.msgSource)
	{
	case MGS_SYSCOM:
		{
			switch(msg.msgType.sys_type)
			{
			case MGT_SYS_END:
				IApplication::GetInst()->Close();
				break;
			}
		}
		break;
	case MGS_SYSMSG:
		{
			switch(msg.msgType.sys_type)
			{
			case MGT_SYS_RUN:
				m_bIsRunning = true;
				break;
			case MGT_SYS_PAUSE:
				m_bIsRunning = false; 
				break;
			}
		}
		break;
	}

	_PostMsg(msg);
}

void CEventOwner::EventCollect( const CMsg& msg )
{
	if( m_bIsRunning )
		m_qMsg.push( msg );
}


bool CEventOwner::Run()
{ return m_bIsRunning = true; }

bool CEventOwner::Pause()
{ return m_bIsRunning = false;}

void CEventOwner::RegisterDelegate( CEventDelegate* pDele )
{
	if(pDele && pDele->m_pOwner != this)
	{
		pDele->UnBind();
		pDele->m_pOwner = this;
		m_DelegatePool.push_back(pDele);
		pDele->MsgProc(MSG_SYS_INIT());
	}
}

void CEventOwner::UnRegisterDelegate( CEventDelegate* pDele )
{
	if(pDele && pDele->m_pOwner == this)
	{
		pDele->m_pOwner = NULL;
		std::list<CEventDelegate*>::iterator it=m_DelegatePool.begin();
		while(it!=m_DelegatePool.end())
		{
			if((*it)==pDele)
			{
				m_DelegatePool.erase(it);
				return ;
			}
			it++;
		}
	}
}

void  CEventOwner::ActiveDelegate( CEventDelegate* pDele )
{
	if( pDele && pDele->m_pOwner == this )
	{
		if(m_ActiveDelegate && m_ActiveDelegate!=pDele)
			m_ActiveDelegate->_UnActive();
		m_ActiveDelegate = pDele;
	}
}

bool CEventOwner::_ProcMsg( const CMsg& MSG )
{
	return true;
}

bool CEventOwner::_PostMsg( const CMsg& MSG )
{
	if(m_ActiveDelegate == NULL || m_ActiveDelegate->MsgProc(MSG))
	{
		if( _ProcMsg(MSG) )
		{
			DelegatePool::iterator it,eit = m_DelegatePool.end();
			for( it = m_DelegatePool.begin(); it!=eit; ++it )
			{
				if(m_ActiveDelegate != *it)
					(*it)->MsgProc(MSG);
			}
		}
	}
	return true;
}