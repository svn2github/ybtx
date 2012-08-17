#include "stdafx.h"

#include "CPath.inl"
#include "CPathFinding.h"
#include "TSqrAllocator.inl"


CPath::CPath(CMap* pMap)
	: m_pMap(pMap)
{
}

CPath::CPath(const PathList& pathList,CMap* pMap)
	: m_Path(pathList)
	, m_pMap(pMap)
{
}

CPath::CPath(const int16* pPathData, uint16 uPathDataLen, const CPos& Begin, const CPos& End, CMap* pMap)
	: m_pMap(pMap)
{
	m_Path.clear();

	m_Path.push_back(Begin);
	//cout << Begin.x << "," << Begin.y << "->";
	uint16 uWayPointNum = uPathDataLen / (2 * sizeof(int16));
	for(uint16 i = 0; i < uWayPointNum; ++i)
	{
		CPos CurWayPoint(*(pPathData + 2 * i), *(pPathData + 2 * i + 1));
		m_Path.push_back(CurWayPoint);
		//cout << CurWayPoint.x << "," << CurWayPoint.y << "->";
	}
	m_Path.push_back(End);
	//cout << End.x << "," << End.y << endl;
	//cout << "-----------------" << endl;
}


/*
// 获得位置
bool CPath::GetPos(CPos& Pos,uint32 uIndex)const
{
	if(uIndex >= m_Path.size())
		return false;
	Pos = m_Path[uIndex];
	return true;
}
*/

// 克隆寻路节点路径
IPath* CPath::Clone()
{
	return new CPath(*this);
}

// 用新的路径替换旧现有路径
void CPath::SwapPath(CPath& NewPath)
{ 
	m_Path.swap(NewPath.GetPath());
}


PathList& CPath::GetPath()
{
	return m_Path;
}

const PathList& CPath::GetPath()const
{
	return m_Path;
}

// 释放自身节点
void CPath::Release()
{
	delete this;
}


void CPath::ChangeToQuarter()
{
	PathList::iterator iter = m_Path.begin();
	for (; iter != m_Path.end(); ++iter)
	{
		iter->x = iter->x / 2;
		iter->y = iter->y / 2;
	}
}

