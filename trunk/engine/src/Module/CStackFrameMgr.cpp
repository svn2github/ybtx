#include "stdafx.h"
#include "CStackFrameMgr.h"
#include "CStackFrame.h"
#include "ExpHelper.h"

CStackFrameMgr::CStackFrameMgr(void)
{

}

CStackFrameMgr::~CStackFrameMgr(void)
{
	GenErr("CStackFrameMgr should never destruct.");
}

CStackFrameMgr& CStackFrameMgr::Inst()
{
	static CStackFrameMgr*	ls_pInst=new CStackFrameMgr;
	return *ls_pInst;
}

#ifndef _WIN32
void CStackFrameMgr::FetchAllSymbol()
{
	for( CStackFrameInfoSet::iterator it=m_setStack.begin();it != m_setStack.end(); ++it )
		(*it)->FetchSymbol();	
}
#endif

CStackFrame* CStackFrameMgr::AddFrame(void* pStackPtr)
{
	CStackFrame* pFrame=new CStackFrame(pStackPtr);

	pair< CStackFrameInfoSet::iterator, bool >	p=m_setStack.insert( pFrame );

	if( !p.second )
	{
		//第一次插入
		delete pFrame;
		pFrame=*p.first;
	}

	pFrame->AddRef();

	return pFrame;
}

void CStackFrameMgr::DelFrame(CStackFrame* pFrame)
{
	pFrame->DelRef();
	if( pFrame->GetRef() > 0 )
		return;

	Ver( m_setStack.erase(pFrame) );
	delete pFrame;
}

