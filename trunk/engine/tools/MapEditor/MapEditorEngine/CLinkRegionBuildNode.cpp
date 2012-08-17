#include "stdafx.h"
#include "CLinkRegionBuildNode.h"

CLinkRegionBuildNode::CLinkRegionBuildNode()
	:m_nSearchId(0)
	,m_pFatherRegion(NULL)
	,m_nChildRegionNum(0)
	,m_ppChildRegion(NULL)
	,m_nLinkNum(0)
	,m_ppLinkInfo(NULL)
	,m_nGlobalIndex(0)
{

}

CLinkRegionBuildNode::~CLinkRegionBuildNode()
{
	//for (int i = m_nChildRegionNum -1; i >= 0; --i)
	//{
	//	delete m_ppChildRegion[i];
	//}
	delete [] m_ppChildRegion;
	m_ppChildRegion = NULL;

	for (int i = m_nLinkNum -1; i >= 0; --i)
	{
		delete m_ppLinkInfo[i];
	}
	delete [] m_ppLinkInfo;
	m_ppLinkInfo = NULL;
	
}

bool CLinkRegionBuildNode::IsInOtherLinkRegion(CLinkRegionBuildNode* pOhterNode)
{
	CLinkRegionBuildNode* pTemp = m_pFatherRegion;
	while (pTemp)
	{
		if (pTemp == pOhterNode)
		{
			return true;
		}
		pTemp = pTemp->m_pFatherRegion;
	}
	return false;
}

uint32 CLinkRegionBuildNode::GetLayerLv()
{
	uint32 nLayerLv = 0;
	CLinkRegionBuildNode* pNode = this;
	while (pNode->m_nChildRegionNum > 0)
	{
		pNode = pNode->m_ppChildRegion[0];
		++nLayerLv;
	}
	return nLayerLv;
}

void CLinkRegionBuildNode::SetChildLinkRegion(VecRegionList_t* pVecLinkRegionList)
{
	if (pVecLinkRegionList == NULL || pVecLinkRegionList->size() == 0)
	{
		m_nChildRegionNum = 0;
		m_ppChildRegion = NULL;
	}
	else
	{
		m_nChildRegionNum = pVecLinkRegionList->size();
		m_ppChildRegion = new CLinkRegionBuildNode*[m_nChildRegionNum];
		memcpy(m_ppChildRegion, &(*pVecLinkRegionList)[0], sizeof(CLinkRegionBuildNode*) * m_nChildRegionNum);
	}
}

void CLinkRegionBuildNode::SetFatherLinkRegion(CLinkRegionBuildNode* pFatherLinkRegion)
{
	m_pFatherRegion = pFatherLinkRegion;
}

void CLinkRegionBuildNode::SetLinkInfo(VecLinkList_t* pVecLinkInfoList)
{
	if (pVecLinkInfoList == NULL || pVecLinkInfoList->size() == 0)
	{
		m_nLinkNum = 0;
		m_ppLinkInfo = NULL;
	}
	else
	{
		m_nLinkNum = pVecLinkInfoList->size();
		m_ppLinkInfo = new CLinkInfoBuild*[m_nLinkNum];
		memcpy(m_ppLinkInfo, &(*pVecLinkInfoList)[0], sizeof(CLinkRegionBuildNode*) * m_nLinkNum);
	}
}

CLinkInfoBuild::CLinkInfoBuild(CLinkRegionBuildNode* pDestLinkRegion, float fFare)
	:m_pDestLinkRegion(pDestLinkRegion)
	,m_fFare(fFare)
{
	
}


