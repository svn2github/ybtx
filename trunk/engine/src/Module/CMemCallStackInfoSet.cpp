#include "stdafx.h"
#include "CMemCallStackInfoSet.h"
#include "CMemCallStackInfo.h"

bool CStackComp::operator()(const CMemCallStackInfo* pLeft, const CMemCallStackInfo* pRight) const
{
	return *pLeft<*pRight;
}

