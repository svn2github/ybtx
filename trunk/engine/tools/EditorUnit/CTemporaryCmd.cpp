#include "stdafx.h"
#include "CTemporaryCmd.h"
#include "CTempCmdPool.h"

CTemporaryCmd::CTemporaryCmd(CTempCmdPool* pPool)
: m_pPool(pPool)
, m_isValid(true)
{
	m_pPool->PushCmd(this);
}

bool	CTemporaryCmd::IsValid(void)
{
	return m_isValid;
}

void	CTemporaryCmd::_SetValid(bool valid)
{
	m_isValid = valid;
	if(!m_isValid)
		m_pPool = NULL;
}

void CTemporaryCmd::Destroy(void)
{
	if(m_pPool)
		m_pPool->Destrory(this);
	CCmd::Destroy();
}