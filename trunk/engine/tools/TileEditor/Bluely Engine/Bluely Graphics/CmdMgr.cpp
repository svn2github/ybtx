//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "CmdMgr.h"
#include "Cmd.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

CCmdMgr * CCmdMgr::one = NULL;

CCmdMgr::CCmdMgr()
{
	// head
	m_pCmdCur = new CCmd;
	m_pCmdCur->Execute();
}

CCmdMgr::~CCmdMgr(void)
{
	Clear();
}

void CCmdMgr::Do(CCmd * pCmd)
{	
	pCmd->Execute();
	pCmd->SetCmdPre(m_pCmdCur);
	pCmd->SetCmdNxt(NULL);
	
	// forward release
	if ( m_pCmdCur )
	{
		CCmd * pCmdNxt = m_pCmdCur->GetCmdNxt();
		while ( pCmdNxt )
		{
			CCmd * pCmd = pCmdNxt->GetCmdNxt();
			SAFE_DELETE(pCmdNxt);
			pCmdNxt = pCmd;
		}
	}
	
	if ( m_pCmdCur )
	{
		m_pCmdCur->SetCmdNxt(pCmd);
	}
	
	m_pCmdCur = pCmd;
}

void CCmdMgr::UnDo()
{
	if ( m_pCmdCur && m_pCmdCur->GetCmdPre() != NULL )
	{
		if ( m_pCmdCur->DoExecuted() )
		{
			m_pCmdCur->UnExecute();

			CCmd * pCmd = m_pCmdCur->GetCmdPre();
			
			if ( pCmd )
			{
				m_pCmdCur = pCmd;
			}
		}
	}
}

void CCmdMgr::ReDo()
{
	if ( m_pCmdCur )
	{	
		if ( !m_pCmdCur->DoExecuted() )
		{
			m_pCmdCur->Execute();
		}
		else
		{
			CCmd * pCmdNxt = m_pCmdCur->GetCmdNxt();
			
			if ( pCmdNxt )
			{
				if ( !pCmdNxt->DoExecuted() )
				{
					pCmdNxt->Execute();
					m_pCmdCur = m_pCmdCur->GetCmdNxt();
				}
			}
		}
	}
}

void CCmdMgr::Clear()
{
	if ( m_pCmdCur )
	{
		// backward
		CCmd * pCmdPre = m_pCmdCur->GetCmdPre();
		while ( pCmdPre )
		{
			CCmd * pCmd = pCmdPre->GetCmdPre();
			SAFE_DELETE(pCmdPre);
			pCmdPre = pCmd;
		}
		
		// forward
		CCmd * pCmdNxt = m_pCmdCur->GetCmdNxt();
		while ( pCmdNxt )
		{
			CCmd * pCmd = pCmdNxt->GetCmdNxt();
			SAFE_DELETE(pCmdNxt);
			pCmdNxt = pCmd;
		}

		// self
		SAFE_DELETE(m_pCmdCur);

		// add head
		m_pCmdCur = new CCmd;
		m_pCmdCur->Execute();
	}
}

}
}