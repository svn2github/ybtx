
#include "stdafx.h"
#include "CGraphPath.h"
#include "TSqrAllocator.inl"


CGraphPath::CGraphPath()
{
}

CGraphPath::~CGraphPath()
{
}

CGraphPath::GraphaPath& CGraphPath::GetPathMap()
{
	return m_PathMap;
}


CGraphPath::PathList& CGraphPath::GetEndPath(int end)
{
	return m_PathMap.at(end);
}

