#include "stdafx.h"
#include "CStackFrameSet.h"
#include "CStackFrame.h"

bool CFrameComp::operator()(const CStackFrame* pLeft, const CStackFrame* pRight) const
{
	return pLeft->m_pAddr<pRight->m_pAddr;
}

