#include "stdafx.h"
#include "CLinkRegionMgr.h"
#include "CLinkRegionNode.h"
#include "CBufFile.h"
#include "CAStarDataMgr.h"
#include "CNode.inl"
#include "CAStarData.inl"
#include "TSqrAllocator.inl"
#include "TBinaryHeap.inl"


#define	MIN_SAMPLE_NODE_NUM			16		//允许的最低精度 至少要达到的样点数, 实际路点是为(SAMPLE_RATE * MIN_SAMPLE_NODE_NUM)


CLinkRegionMgr::CLinkRegionMgr(void)
	:m_nAllNodeNum(0)
	,m_nBaseNodeNum(0)
	,m_pAStarData(NULL)
{
}

CLinkRegionMgr::~CLinkRegionMgr(void)
{
}

void CLinkRegionMgr::LoadData(CBufFile& File)
{
	Ast(m_pRegionNodeArray == NULL);
	if (File.GetBufSize() == 0)
	{
		return;
	}
	File.read( &(m_nWidth) , sizeof(uint32));
	File.read( &(m_nHeight) , sizeof(uint32));

	File.read(&m_nBaseNodeNum, sizeof(uint32));
	m_pBaseInfoArray =  BaseNodeArray_t(new CBaseRegionNode[m_nBaseNodeNum]);
	Ast(sizeof(CBaseRegionNode) == 2 * sizeof(uint32));
	File.read(m_pBaseInfoArray.get(), sizeof(CBaseRegionNode) * m_nBaseNodeNum);

	File.read(&m_nAllNodeNum, sizeof(uint32));
	File.read(&m_nUniteSize, sizeof(uint32));
	File.read(&m_nBeginLayer, sizeof(uint32));


	m_pRegionNodeArray = RegionNodeArray_t(new CLinkRegionNode[m_nAllNodeNum]);
	m_pRegionNodeArray[0].m_pLinkList = NULL;//第一个不用
	CLinkRegionNode* pTemp;
	CLinkInfo* pLinkInfo;
	for (uint32 i = 1; i < m_nAllNodeNum; ++i) //从1开始,  0编号为空
	{
		pTemp = m_pRegionNodeArray.get() + i;
		File.read(&pTemp->m_nKeyPosX, sizeof(uint16));
		File.read(&pTemp->m_nKeyPosY, sizeof(uint16));
		File.read(&pTemp->m_nFatherRegionId, sizeof(uint32));
		File.read(&pTemp->m_nChildNum, sizeof(uint8));
		File.read(&pTemp->m_nChildStartId, sizeof(uint32));

		File.read(&pTemp->m_nLinkNum, sizeof(uint8));
		if (pTemp->m_nLinkNum > 0)
		{
			pTemp->m_pLinkList = new CLinkInfo[pTemp->m_nLinkNum];
			for (int j = 0; j < pTemp->m_nLinkNum; ++j)
			{
				pLinkInfo = pTemp->m_pLinkList + j;
				File.read(&pLinkInfo->m_nDestRegionId, sizeof(uint32));
				File.read(&pLinkInfo->m_fFare, sizeof(float));
			}
		}
		else
		{
			pTemp->m_pLinkList = NULL;
		}
	}
}

void CLinkRegionMgr::SetSharedData(CLinkRegionMgr* pOthter)
{
	Ast(m_pRegionNodeArray == NULL);
	m_pBaseInfoArray = pOthter->m_pBaseInfoArray;
	m_pRegionNodeArray = pOthter->m_pRegionNodeArray;
	m_nAllNodeNum = pOthter->m_nAllNodeNum;
	m_nBaseNodeNum = pOthter->m_nBaseNodeNum;
	m_nUniteSize = pOthter->m_nUniteSize;
	m_nBeginLayer = pOthter->m_nBeginLayer;
	m_nWidth = pOthter->m_nWidth;
	m_nHeight = pOthter->m_nHeight;
}

uint32 CLinkRegionMgr::GetAllNodeNum()
{
	return m_nAllNodeNum;
}

CLinkRegionNode* CLinkRegionMgr::GetNearestRegion(const CPos& pos, CLinkRegionNode* pTopRegion, uint32 nLayerLv)
{
	uint32 nRealLayerLv = nLayerLv + m_nBeginLayer;
	m_heapNearRegion.clear();
	m_heapNearRegion.push(0.0f, make_pair(pTopRegion, nRealLayerLv));

	CLinkRegionNode* pRegionTemp, * pChildRegion;

	int dx, dy, nDiagonal,nStraight, radius;
	float fDis;
	uint32 i;
	uint32 nChildRealLayerLv;
	while (true)
	{
		pRegionTemp = m_heapNearRegion.top().first;
		nRealLayerLv = m_heapNearRegion.top().second;
		m_heapNearRegion.pop();
		if (nRealLayerLv == m_nBeginLayer)
		{
			return pRegionTemp;
		}
		nChildRealLayerLv = nRealLayerLv -1;
		radius = (1 << nChildRealLayerLv) >> 1;  // 边长 /2
		pChildRegion = m_pRegionNodeArray.get() + pRegionTemp->m_nChildStartId;
		for (i = 0; i < pRegionTemp->m_nChildNum; ++i)
		{
			dx = ((pChildRegion->m_nKeyPosX >> nChildRealLayerLv) << nChildRealLayerLv) + radius - pos.x;  //取矩形中点
			dy = ((pChildRegion->m_nKeyPosY >> nChildRealLayerLv) << nChildRealLayerLv) + radius - pos.y; 
			nDiagonal = min(abs(dx), abs(dy));
			nStraight = (abs(dx) + abs(dy));
			fDis = 1.414f * nDiagonal + nStraight - (nDiagonal << 1) - radius;
			m_heapNearRegion.push(fDis, make_pair(pChildRegion, nChildRealLayerLv));
			++pChildRegion;
		}
	}
	return NULL;
}

CLinkRegionNode* CLinkRegionMgr::GetTopRegionNode(CLinkRegionNode* pBaseNode, uint32& nLayerLv)
{
	nLayerLv = 0;
	while (pBaseNode->m_nFatherRegionId)
	{
		pBaseNode = m_pRegionNodeArray.get() + pBaseNode->m_nFatherRegionId;
		++ nLayerLv;
	}
	return pBaseNode;
}

CLinkRegionNode* CLinkRegionMgr::GetBaseLinkRegionNode(const CPos& pos)
{
	//二分查找
	uint32 nPosIndex = (pos.y >> 1) *(m_nWidth >> 1) + (pos.x >> 1);
	int nStart = 0;
	int nEnd = m_nBaseNodeNum -1;
	int nMid;
	CBaseRegionNode* pNode = NULL;
	while (nStart <= nEnd)
	{
		nMid = nStart + ((nEnd - nStart) >>  1);
		pNode = m_pBaseInfoArray.get() + nMid;
		if (pNode->m_nIndex > nPosIndex)
		{
			nEnd = nMid - 1;
		}
		else if(pNode->m_nIndex < nPosIndex)
		{
			nStart = nMid + 1;
		}
		else
		{
			return m_pRegionNodeArray.get() + pNode->m_nFatherRegionId;
		}
	}
	return NULL;
}

void CLinkRegionMgr::GetEveryLayerLinkRegion(VecLinkRegionNode_t& vecRegionList, CLinkRegionNode* pBaseNode, uint32 nLayerLv)
{
	vecRegionList.clear();
	for (uint32 i = 0; i <= nLayerLv; ++i)
	{
		vecRegionList.push_back(pBaseNode);
		if(!pBaseNode->m_nFatherRegionId)
		{
			return;
		}
		pBaseNode = m_pRegionNodeArray.get() + pBaseNode->m_nFatherRegionId;
	}
}

uint32 CLinkRegionMgr::GetUniteSize()
{
	return m_nUniteSize;
}

bool CLinkRegionMgr::IsConnected(CLinkRegionNode* pNode1, CLinkRegionNode* pNode2, uint32& nLayerLv)
{
	if (pNode1 == NULL || pNode2 == NULL)
	{
		return false;
	}
	nLayerLv = 0;
	while (pNode1 != m_pRegionNodeArray.get() && pNode2 != m_pRegionNodeArray.get())
	{
		if (pNode1 == pNode2)
		{
			return true;
		}

		pNode1 = m_pRegionNodeArray.get() + pNode1->m_nFatherRegionId;
		pNode2 = m_pRegionNodeArray.get() + pNode2->m_nFatherRegionId;
		++nLayerLv;
	}
	return false;
}

CPathDirect* CLinkRegionMgr::GetPathDirect(const CPos& beginPos, const CPos& endPos, EFindPathType EFindType, EBarrierType EBarrier, uint32 nPathLength /* =0 */)
{
	if (m_pRegionNodeArray.get() == NULL || EBarrier != eBT_LowBarrier) //目前仅有低障连通信息
	{
		m_PathDirect1.Clear();
		m_PathDirect1.AddPos(endPos.x, endPos.y, 0.0f);
		return &m_PathDirect1;
	}

	CLinkRegionNode* pBeginNode = GetBaseLinkRegionNode(beginPos);
	CLinkRegionNode* pEndNode = GetBaseLinkRegionNode(endPos);
	CPos realEndPos = endPos;
	uint32 nLayerLv;
	if (!IsConnected(pBeginNode, pEndNode, nLayerLv))
	{
		if ((EFindType == eFPT_HypoAStarOptiUseWeight || EFindType == eFPT_HypoAStarOpti) && pBeginNode)//就近点寻路
		{
			CLinkRegionNode* pTopRegion = GetTopRegionNode(pBeginNode, nLayerLv);
			pEndNode = GetNearestRegion(endPos, pTopRegion, nLayerLv);
			realEndPos.x = pEndNode->m_nKeyPosX;
			realEndPos.y = pEndNode->m_nKeyPosY;
			Ast(IsConnected(pBeginNode, pEndNode, nLayerLv));
		}
		else
		{
			m_PathDirect1.Clear();
			m_PathDirect1.AddPos(endPos.x, endPos.y, 0.0f);
			return &m_PathDirect1;
		}
	}

	if (EFindType != eFPT_AStarUseWeight && EFindType != eFPT_HypoAStarOptiUseWeight)
	{
		m_PathDirect1.Clear();
		m_PathDirect1.AddPos(realEndPos.x, realEndPos.y, 0.0f);
		return &m_PathDirect1;
	}

	if (nLayerLv > 4)
	{
		nLayerLv -= 4;
	}
	else
	{
		nLayerLv = 0;
	}


	VecLinkRegionNode_t vecBeginLinkRegionList, vecEndLinkRegionList;
	GetEveryLayerLinkRegion(vecBeginLinkRegionList, pBeginNode, nLayerLv);
	GetEveryLayerLinkRegion(vecEndLinkRegionList, pEndNode, nLayerLv);
	CPathDirect* pPathDirect = &m_PathDirect1;
	CPathDirect* pNextPathDirect = &m_PathDirect2;
	CPathDirect* pPathTemp;

	
	pPathDirect->Clear();
	pPathDirect->AddPos(realEndPos.x, realEndPos.y, 0.0f);
	for (int nCurLayer = nLayerLv; nCurLayer >= 0; --nCurLayer)
	{
		pEndNode = vecEndLinkRegionList[nCurLayer];
		Ast(FindPathDirect(*pNextPathDirect, vecBeginLinkRegionList[nCurLayer], pEndNode, pEndNode != vecEndLinkRegionList[nCurLayer], pPathDirect));
		pNextPathDirect->ResetEndPos(realEndPos);
		pNextPathDirect->SetNearSize(float(1 << (nCurLayer + m_nBeginLayer)));
		pPathTemp = pPathDirect;
		pPathDirect = pNextPathDirect;
		pNextPathDirect = pPathTemp;
	}

	return pPathDirect;
}


void CLinkRegionMgr::DumpPathDirect(CPathDirect& pathDirect,CLinkRegionNode* pBeginRegionNode, CNode* pEndNode)
{
	pathDirect.Clear();
	CLinkRegionNode* pRegionNode = GetRegionNode(pEndNode);
	float fAllLength = pEndNode->GetG();
	CNode* pNode = pEndNode;
	uint32 nNodeIndex;
	while (pRegionNode != pBeginRegionNode)
	{
		pathDirect.AddPos(pRegionNode->m_nKeyPosX, pRegionNode->m_nKeyPosY, fAllLength - pNode->GetG());
		nNodeIndex = pRegionNode->m_pLinkList[pNode->GetFrontIndex()].m_nDestRegionId;
		pNode = m_pAStarData->GetNode(nNodeIndex);
		pRegionNode = m_pRegionNodeArray.get() +  nNodeIndex;
	}
	if (pathDirect.GetPosNum() == 0)	//起点不要
	{
		pathDirect.AddPos(pRegionNode->m_nKeyPosX, pRegionNode->m_nKeyPosY, fAllLength - pNode->GetG());
	}
}

bool CLinkRegionMgr::FindPathDirect(CPathDirect& pathDirect, CLinkRegionNode* pBeginNode, CLinkRegionNode* pEndNode, bool bEndNodeIsFather, CPathDirect* pFatherDirect/*= NUll*/)
{
	m_pAStarData = CAStarDataMgr::Inst().GetAStarData();
	CAStarData::MinHeap& heapF =  m_pAStarData->GetHeapF();
	uint32 nSearchId = m_pAStarData->GetSearchId();
	heapF.clear();

	CNode* pCurNode, * pDestNode;
	CLinkRegionNode* pCurRegionNode, *pDestRegionNode, *pCheckRegionNode;
	CPathDirectNode* pDirectNode;
	uint32 nCurIndex;
	uint32 nTargetIndex;
	float fRemainderLength;
	int nLinkNum;
	int dx, dy, dx2, dy2, targetX, targetY;
	int nDiagonal;
	int nStraight;
	int cross;
	float newG;
	float fH, fTargetH;

	float fNearSize = pFatherDirect->GetNearSize();
	uint32 nMaxIndex = pFatherDirect->GetPosNum() -1;

	pCurNode = GetAStarDataNode(pBeginNode);
	pCurNode->SetG(0.0f);
	pCurNode->SetTargetId(0);
	heapF.push(0.0f,pCurNode);
	pCurNode->Open(nSearchId);
	//int num = 0;
	int i;
	while(true)
	{
		if(heapF.empty())
		{
			return false;
		}
		pCurNode = heapF.top();
		heapF.pop();
		//++num;
		if (pCurNode->IsClose(nSearchId))//重复弹出的节点, 因为修FrontNode的时候, 重复插入了更小F值的点, 这里弹出来的点已经处理过了
		{
			continue;
		}

		pCurRegionNode = GetRegionNode(pCurNode);

		pCheckRegionNode = bEndNodeIsFather ? m_pRegionNodeArray.get() + pCurRegionNode->m_nFatherRegionId : pCurRegionNode;

		if (pCheckRegionNode == pEndNode )//进入结束region  放在这里检出才能得到最小路径, 低精度寻路的时候误差可能比较大,所以尽量做到精准.
		{
			DumpPathDirect(pathDirect, pBeginNode, pCurNode);
			//printf("指导点寻路遍历节点数: %d,  路径节点数: %d, 指导点数: %d\n", num, pathDirect.GetPosNum(), pFatherDirect->GetPosNum());
			return true;
		}
		pCurNode->Close(nSearchId);
		nLinkNum = pCurRegionNode->m_nLinkNum;
		nCurIndex = GetRegionIndex(pCurRegionNode);
		nTargetIndex = pCurNode->GetTargetId();

		pDirectNode = pFatherDirect->GetDirectNode(nTargetIndex);
		targetX = pDirectNode->GetPos().x;
		targetY = pDirectNode->GetPos().y;
		fRemainderLength = pDirectNode->GetRemainderLength();
		
		dx2 =  pCurRegionNode->m_nKeyPosX - targetX;
		dy2 =  pCurRegionNode->m_nKeyPosY - targetY;
		for (i = 0; i < nLinkNum; ++i)
		{
			pDestRegionNode = m_pRegionNodeArray.get() + pCurRegionNode->m_pLinkList[i].m_nDestRegionId;
			pDestNode = GetAStarDataNode(pDestRegionNode);
			if (pDestNode->IsClose(nSearchId) )
			{
				continue;
			}
			newG = pCurNode->GetG() + pCurRegionNode->m_pLinkList[i].m_fFare;

			if (!(pDestNode->IsOpen(nSearchId) && newG > pDestNode->GetG()) )
			{

				dx = targetX - pDestRegionNode->m_nKeyPosX;
				dy = targetY - pDestRegionNode->m_nKeyPosY;
				
				nDiagonal = min(abs(dx), abs(dy));
				nStraight = (abs(dx) + abs(dy));
				cross = abs(dx * dy2 - dx2 * dy);

				fTargetH = 1.414f * nDiagonal + nStraight - (nDiagonal << 1);
				fH = fTargetH + fRemainderLength + cross * 0.001f;

				pDestNode->SetG(newG);
				pDestNode->SetTargetId((fTargetH < fNearSize && nTargetIndex < nMaxIndex) ? nTargetIndex + 1: nTargetIndex);
				pDestNode->SetFrontIndex(pDestRegionNode->GetLinkIndex(nCurIndex));
				heapF.push(fH + newG, pDestNode);//修改front节点, 重新插入堆, 原来的AStarAlgorithm中没有这一步,这里是为了得到最短路径. 低精度寻路的时候误差可能比较大,所以尽量做到精准.
				pDestNode->Open(nSearchId);
				//(本应删除原来的点, 但二叉堆不方便查询删除, 利用重复插入,在弹出的时候不处理重复点来实现的)
			}
		}
	}
}

inline uint32 CLinkRegionMgr::GetRegionIndex(CLinkRegionNode* pRegionNode)
{ 
	return static_cast<uint32>(pRegionNode - m_pRegionNodeArray.get());
}

inline CNode* CLinkRegionMgr::GetAStarDataNode(CLinkRegionNode* pRegionNode)
{
	return m_pAStarData->GetNode(GetRegionIndex(pRegionNode));
}

inline CLinkRegionNode* CLinkRegionMgr::GetRegionNode(CNode* pNode)
{
	return m_pRegionNodeArray.get() + m_pAStarData->GetIndex(pNode);
}

