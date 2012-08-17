#include "stdafx.h"

#include "CPathDirect.h"
#include "TSqrAllocator.inl"

CPathDirect::CPathDirect()
	:m_fNearSize(0.0f)
{

}

void CPathDirect::Clear()
{
	m_VecNodeList.clear();
}


void CPathDirect::AddPos(int x, int y, float fLength)
{
	m_VecNodeList.push_back(CPathDirectNode(x, y , fLength));
}

uint32 CPathDirect::GetPosNum()
{
	return (uint32)m_VecNodeList.size();
}

CPathDirectNode* CPathDirect::GetDirectNode(uint32 nIndex)
{
	return &m_VecNodeList[m_VecNodeList.size() - 1 - nIndex];
}

void CPathDirect::SetNearSize(float fNearSize)
{
	m_fNearSize = fNearSize;
}

float CPathDirect::GetNearSize()
{
	return m_fNearSize;
}

void CPathDirect::ResetEndPos(const CPos& pos)
{
	m_VecNodeList[0].SetPos(pos);
}

const CPos& CPathDirect::GetEndPos()
{
	return m_VecNodeList[0].GetPos();
}


