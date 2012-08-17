#include "stdafx.h"

#include "CAStarDataMgr.h"
#include "ThreadHelper.h"
#include "CAStarData.h"

CAStarDataMgr::CAStarDataMgr()
	:m_nWidth(0)
	,m_nHeight(0)
	,m_nAllNodeNum(0)
{
	TLS_CreateKey(&ms_AStarDataKey);
}

CAStarDataMgr::~CAStarDataMgr()
{
	VecAStarData_t::iterator it = m_vecAStarData.begin();
	for (;it != m_vecAStarData.end(); ++it)
	{
		delete *it;
	}
	m_vecAStarData.clear();
	TLS_DestroyKey(ms_AStarDataKey);
}

CAStarDataMgr& CAStarDataMgr::Inst()
{
	static CAStarDataMgr instance;
	return instance;
}

void CAStarDataMgr::UpdateSize(uint32 nWidth, uint32 nHeight)
{
	m_nWidth = max( nWidth, m_nWidth);
	m_nHeight = max(nHeight, m_nHeight);
	m_nAllNodeNum = max(m_nAllNodeNum, m_nWidth * m_nHeight);

	VecAStarData_t::iterator it = m_vecAStarData.begin();
	for (;it != m_vecAStarData.end(); ++it)
	{
		(*it)->UpdateSize(m_nWidth, m_nHeight, m_nAllNodeNum);
	}
}

void CAStarDataMgr::UpdateAllNodeNum(uint32 nAllNodeNum)
{
	m_nAllNodeNum = max(m_nAllNodeNum, nAllNodeNum);
	VecAStarData_t::iterator it = m_vecAStarData.begin();
	for (;it != m_vecAStarData.end(); ++it)
	{
		(*it)->UpdateSize(m_nWidth, m_nHeight, m_nAllNodeNum);
	}
}

CAStarData* CAStarDataMgr::GetAStarData()
{
	CAStarData* pAStarData = (CAStarData*)TLS_GetValue(ms_AStarDataKey);
	if (!pAStarData)
	{
		pAStarData = new CAStarData(m_nWidth, m_nHeight, m_nAllNodeNum);
		TLS_SetValue(ms_AStarDataKey, pAStarData);
		m_vecAStarData.push_back(pAStarData);
	}
	return pAStarData;
}

uint32 CAStarDataMgr::GetCurWidth()
{
	return m_nWidth;
}

uint32 CAStarDataMgr::GetCurHeight()
{
	return m_nHeight;
}

uint32 CAStarDataMgr::GetAllNodeNum()
{
	return m_nAllNodeNum;
}



