#include "stdafx.h"
#include "CAsynFunParamsMgr.h"
#include "BaseHelper.h"
#include "ExpHelper.h"
#include "GraphicBase.h"
#include "TSqrAllocator.inl"

void CAsynFunParamsMgr::Insert( FunCommand* pFunCom )
{
	Ast(pFunCom&&"pFunCom指针不能为空");
	if(!m_bLock)
		m_lFunPool.push_back(pFunCom);
}

void CAsynFunParamsMgr::Do(DWORD type)
{
	size_t maxS = m_lFunPool.size();
	FunPool::iterator it,eit = m_lFunPool.end();
	it = m_lFunPool.begin();
	for(size_t i = 0 ; it!=eit && i<maxS; ++i )
	{
		if(type && type != (*it)->GetType())
			continue;

		m_ActiveCmd = *it;
		if( m_ActiveCmd->Do() )
		{
			delete m_ActiveCmd;
			it = m_lFunPool.erase(it);
		}
		else
			++ it;
		m_ActiveCmd = NULL;
	}
}

bool CAsynFunParamsMgr::IsActive(void)
{
	return m_ActiveCmd!=NULL;
}

bool CAsynFunParamsMgr::Delete( DWORD type ,const void* param )
{
	if(NULL!=m_ActiveCmd)
		return false;

	FunPool::iterator it,eit = m_lFunPool.end();
	it = m_lFunPool.begin();
	bool isFind = false;
	for(; it!=eit;  )
	{
		if( ((*it)->GetType() == type && (param == NULL || (*it)->Compare(param) )) )
		{
			delete *it;
			it = m_lFunPool.erase(it);
			isFind = true;
		}
		else
			++it;
	}
	return isFind;
}

size_t CAsynFunParamsMgr::Size(void)
{
	return m_lFunPool.size();
}

void CAsynFunParamsMgr::Clear(void)
{
	m_bLock = true;
	FunPool::iterator it,eit = m_lFunPool.end();
	for( it = m_lFunPool.begin(); it!=eit; ++it )
		GfkSafeDelete(*it);
	m_lFunPool.clear();
	m_bLock = false;
}

