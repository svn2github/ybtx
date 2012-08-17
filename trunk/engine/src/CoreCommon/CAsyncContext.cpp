#include "stdafx.h"
#include "CAsyncContext.h"
#include "BaseHelper.h"

CAsyncContext::CAsyncContext()
{
	m_pContext = NULL;
}

CAsyncContext::~CAsyncContext()
{
	AdvDelPvRef(m_pContext);
}

void CAsyncContext::SetContext(void* pContext)
{
	AdvDelPvRef(m_pContext);
	m_pContext=pContext;
	AdvAddPvRef(m_pContext);
}

void* CAsyncContext::GetContext()const
{
	return m_pContext;
}

