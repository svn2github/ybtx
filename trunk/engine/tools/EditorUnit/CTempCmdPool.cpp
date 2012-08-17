#include "stdafx.h"
#include "CTempCmdPool.h"
#include "CTemporaryCmd.h"

CTempCmdPool::CTempCmdPool(void)
{

}

CTempCmdPool::~CTempCmdPool(void)
{
	Clear();
}

void CTempCmdPool::Clear(void)
{
	TempCmdPool::iterator it,eit = m_cmdPool.end();
	for(it = m_cmdPool.begin();it!=eit;++it)
	{
		(*it)->_SetValid(false);
	}
	m_cmdPool.clear();
}

void CTempCmdPool::PushCmd(CTemporaryCmd* cmd)
{
	m_cmdPool.insert(cmd);
}

void CTempCmdPool::Destrory(CTemporaryCmd* cmd)
{
	m_cmdPool.erase(cmd);
}