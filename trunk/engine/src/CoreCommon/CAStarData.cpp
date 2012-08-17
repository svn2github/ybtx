#include "stdafx.h"

#include "CAStarData.h"
#include "TBinaryHeap.inl"
#include "TSqrAllocator.inl"

int CAStarData::ms_nOffset[16] = { 1,1,  1,-1,  -1,-1, -1,1,  1,0,  -1,0,  0,1,  0,-1}; //Ë³Ðò²»ÒªÂÒ

CAStarData::CAStarData(uint32 nWidth, uint32 nHeight, uint32 nAllNodeNum)
	:m_pNodeArray(NULL)
	,m_nSearchId(0)
	,m_nWidth(0)
	,m_nHeight(0)
	,m_nAllNodeNum(0)
{
	UpdateSize(nWidth, nHeight, nAllNodeNum);
}

CAStarData::~CAStarData(void)
{
	if (m_pNodeArray)
	{
		delete [] m_pNodeArray;
		m_pNodeArray = NULL;
	}
}


void CAStarData::UpdateSize(uint32 nWidth, uint32 nHeight, uint32 nAllNodeNum)
{
	Ast(nAllNodeNum >= nWidth * nHeight && nWidth >= m_nWidth && nHeight >= m_nHeight);
	if (m_nAllNodeNum != nAllNodeNum)
	{
		m_nAllNodeNum = nAllNodeNum;
		if (m_pNodeArray)
		{
			delete [] m_pNodeArray;
			m_pNodeArray = NULL;
		}
		m_pNodeArray = new CNode[m_nAllNodeNum];
		
		ReSet();
	}
	if (m_nWidth != nWidth)
	{
		m_nWidth = nWidth;
		for (int i = 0; i < 8; ++i)
		{
			m_nNodeOffset[i] = m_nWidth * ms_nOffset[i * 2 + 1] + ms_nOffset[i * 2];
		}
	}
	m_nHeight = nHeight;
}

void CAStarData::ReSet()
{
	m_nSearchId = 0;
	memset(m_pNodeArray,  0, sizeof(CNode) * m_nAllNodeNum);
}

uint32 CAStarData::GetSearchId()
{
	if (m_nSearchId == static_cast<uint32>(-1))
	{
		ReSet();
	}
	++m_nSearchId;
	return m_nSearchId;
}

int* CAStarData::GetOffsetArray()
{
	return ms_nOffset;
}

CAStarData::MinHeap& CAStarData::GetHeapF()
{
	return m_heapF;
}

