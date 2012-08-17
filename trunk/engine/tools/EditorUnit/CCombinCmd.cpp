#include "stdafx.h"
#include "CCombinCmd.h"


CCombinCmd::~CCombinCmd(void)
{
	CommandPool::iterator it,eit = m_ComPool.end();
	for( it = m_ComPool.begin(); it!=eit; ++it )
	{
		(*it)->Destroy();
	}
	m_ComPool.clear();
}

void CCombinCmd::Push(CCmd* pCmd)
{
	m_ComPool.push_back(pCmd);
}

void CCombinCmd::_Execute(void)
{
	CommandPool::iterator it,eit = m_ComPool.end();
	for( it = m_ComPool.begin(); it!=eit; ++it )
	{
		if((*it)->IsValid())
			(*it)->_Execute();
	}
}

void CCombinCmd::_UnExecute(void)
{
	if( m_ComPool.empty() )
		return;

	CommandPool::iterator it,eit = m_ComPool.begin();
	it = m_ComPool.end();
	do 
	{
		--it;
		if((*it)->IsValid())
			(*it)->_UnExecute();
	}while (it!=eit);
}

bool CCombinCmd::IsValid(void)
{
	bool isValid = false;
	CommandPool::iterator it,eit = m_ComPool.end();
	for( it = m_ComPool.begin(); it!=eit; ++it )
	{
		isValid |= (*it)->IsValid();
	}
	return isValid || m_ComPool.empty();
}