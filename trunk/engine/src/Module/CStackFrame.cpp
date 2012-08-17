#include "stdafx.h"
#include "CStackFrame.h"
#include "ExpHelper.h"
#include "CMemoryLeakageMgr.h"
#include "CMapFileMgr.h"

CStackFrame::CStackFrame(void* pAddr)
:m_uRef(0),m_pAddr(pAddr)
{
}

CStackFrame::~CStackFrame(void)
{
}


void CStackFrame::AddRef()
{
	++m_uRef;
}

void CStackFrame::DelRef()
{
	--m_uRef;
}

uint32 CStackFrame::GetRef()const
{
	return m_uRef;
}

void CStackFrame::SetName(const char* szName)
{
	m_sName.assign(szName);
}

const char* CStackFrame::GetName()const
{
	return m_sName.c_str();
}

void* CStackFrame::GetAddr()const
{
	return m_pAddr;
}

const char* CStackFrame::GetText()
{
	if (m_sName.empty())
	{
#ifdef _WIN32
		char buf[24];
		sprintf(buf, "Addr: 0x%x\t", GetAddr());
		m_sName.append(buf);

		const char* szSymbol = CMapFileMgr::Inst().FindFunName((uint32)GetAddr());
		if (szSymbol)
			m_sName.append(szSymbol);
#else
		char ** arySymbol;
		void * pAddr=GetAddr();
		arySymbol=backtrace_symbols(&pAddr,1);
		m_sName.assign(arySymbol[0]);			
		free(arySymbol);

		if( m_sName.empty() )
			m_sName.assign("NULL");
#endif

	}

	return m_sName.c_str();
}


#ifndef _WIN32
void CStackFrame::FetchSymbol()
{
	char ** arySymbol;
	void * pAddr=GetAddr();
	arySymbol=backtrace_symbols(&pAddr,1);
	m_sName.assign(arySymbol[0]);			
	free(arySymbol);

	if( m_sName.empty() )
		m_sName.assign("NULL");
}
#endif

