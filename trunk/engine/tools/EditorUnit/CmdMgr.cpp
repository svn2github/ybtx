#include "stdafx.h"
#include "CmdMgr.h"
#include "Cmd.h"
#include "BaseHelper.h"

CCmdMgr::CCmdMgr()
{
}

CCmdMgr::~CCmdMgr(void)
{
	Clear();
}

void CCmdMgr::Do(CCmd * pCmd)
{
	ClearPool(m_UnDoQueue);
	pCmd->_Execute();
	m_CmdQueue.push(pCmd);
}

void CCmdMgr::UnDo(void)
{	
	CCmd* pTemp = NULL; 
	while(pTemp ==NULL || !pTemp->IsValid())
	{
		if(m_CmdQueue.empty())
			return;

		if(pTemp)
			pTemp->Destroy();

		pTemp = m_CmdQueue.top();
		m_CmdQueue.pop();
	}

	pTemp->_UnExecute();
	m_UnDoQueue.push(pTemp);
}
	

void CCmdMgr::ReDo(void)
{
	CCmd* pTemp = NULL; 
	while(pTemp ==NULL || !pTemp->IsValid())
	{
		if(m_UnDoQueue.empty())
			return;

		if(pTemp)
			pTemp->Destroy();

		pTemp = m_UnDoQueue.top();
		m_UnDoQueue.pop();
	}
	
	pTemp->_Execute();
	m_CmdQueue.push(pTemp);
}

void CCmdMgr::Clear(void)
{
	ClearPool(m_CmdQueue);
	ClearPool(m_UnDoQueue);
}

void CCmdMgr::ClearPool(CCmdMgr::CommandPool& Pool)
{
	while(!Pool.empty())
	{
		Pool.top()->Destroy();
		Pool.pop();
	}
}