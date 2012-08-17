
#include "CAStarData.h"
#include "TBinaryHeap.inl"
#include "CNode.h"


inline CNode* CAStarData::ReturnBestNode()
{
	CNode * pNode = m_heapF.top();
	m_heapF.pop();
	return pNode;
}

inline uint32 CAStarData::GetIndex(int x, int y)
{
	return m_nWidth * y + x;
}

inline CNode* CAStarData::GetNodeAddress(int x, int y)
{
	return m_pNodeArray + m_nWidth * y + x;
}

inline int CAStarData::GetNodeOffset(int iDir)
{
	return m_nNodeOffset[iDir];
}

inline void CAStarData::GetPos(int32& x, int32& y, CNode* pNode)
{
	int offset = static_cast<int>(pNode - m_pNodeArray);
	x = offset % m_nWidth;
	y = offset / m_nWidth;
}

inline CNode* CAStarData::GetFront(CNode* pNode)
{
	return pNode->GetFrontDir() == 8 ? NULL : pNode - m_nNodeOffset[pNode->GetFrontDir()];
}


inline CNode* CAStarData::GetNode(uint32 nIndex)
{
	return m_pNodeArray + nIndex;
}

inline uint32 CAStarData::GetIndex(CNode* pNode)
{
	return static_cast<uint32>(pNode - m_pNodeArray);
}


