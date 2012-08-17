#include "stdafx.h"
#include "CLinkRegionBuild.h"
#include "CLinkRegionBuildNode.h"
#include "TBinaryHeap.inl"
#include "IBarrierInfo.h"
#include "Console.h"
#include "AutoSearchFile.h"
#include "CBarrierData.h"

#define UNITE_POWER			1
uint32 UNITE_SIZE = 1 << UNITE_POWER;

#define Fill_SIDE_BLOCK_PROGRESS	0.03f
#define SORT_REGION_PROGRESS		0.05f
#define BUILD_BASE_LAYER_PROGRESS	0.08f
#define BUILD_END_PROGRESS			0.75f

#define	VALID_BEGIN_LAYER			2


const float BARRIER_WEIGHT[4] = {1.5f, 0.7f, 99999999999.f, 99999999999.f};


bool CompareRegion(CLinkRegionBuildNode* pRegion1, CLinkRegionBuildNode* pRegion2)
{
	if ((pRegion1->GetKeyPos().y >> 1) == (pRegion2->GetKeyPos().y >> 1))
	{
		return pRegion1->GetKeyPos().x < pRegion2->GetKeyPos().x;
	}
	return (pRegion1->GetKeyPos().y >> 1) < (pRegion2->GetKeyPos().y >> 1);
}

CLinkRegionBuild::CLinkRegionBuild()
	:m_ppBaseNodeArray(NULL)
	,progress(NULL)
	,m_pBarrierInfo(NULL)
{

}

CLinkRegionBuild::~CLinkRegionBuild()
{
	Clear();
}

void CLinkRegionBuild::Clear()
{
	uint32 i, j;
	for (i = 0; i < m_vecLayer.size() ; ++i)
	{
		for (j = 0; j < m_vecLayer[i]->size(); ++j)
		{
			delete (*m_vecLayer[i])[j];
		}
		m_vecLayer[i]->clear();
		delete m_vecLayer[i];
	}
	m_vecLayer.clear();
	if (m_ppBaseNodeArray)
	{
		delete [] m_ppBaseNodeArray;
		m_ppBaseNodeArray = NULL;
	}
}

void CLinkRegionBuild::SetProcessCallback( IProgressCallback* p )
{
	Ast(NULL != p);
	this->progress = p;
}

void CLinkRegionBuild::FillSideBlock()
{
	//if (IDYES != MessageBox(NULL, "是否将边界相连的区域填充为障碍(仅影响 .lrg文件)","提示", MB_YESNO) )
	//{
	//	return;
	//}
	uint32 x, y;

	for (x = 0; x < m_nWidth; ++x)
	{
		if (m_ppBaseNodeArray[x])
		{
			FillArea(x, 0);
		}
		if (m_ppBaseNodeArray[(m_nHeight-1) * m_nWidth + x])
		{
			FillArea(x, m_nHeight-1);
		}
	}

	for (y = 1; y < m_nHeight-1; ++y)
	{
		if (m_ppBaseNodeArray[y * m_nWidth])
		{
			FillArea(0, y);
		}
		if (m_ppBaseNodeArray[y * m_nWidth + m_nWidth - 1])
		{
			FillArea(m_nWidth - 1, y);
		}
	}
}

void CLinkRegionBuild::FillArea(uint32 x, uint32 y)
{
	static int OFFSET_4DIR[4][2] =   { { 0,-1},{ 1, 0},{ 0, 1},{-1, 0} };				//4向偏移数组

	int INDEX_OFFSET[4];
	for (int i = 0; i < 4; ++ i)
	{
		INDEX_OFFSET[i] = OFFSET_4DIR[i][1] * m_nWidth + OFFSET_4DIR[i][0];
	}

	m_vecStep.clear();
	int nIndex = y * m_nWidth + x;
	m_vecStep.push_back(nIndex);
	
	int iAllNum = m_nWidth * m_nHeight;
	int iCurIndex;
	int iIndexTemp;
	int nFillCount = 0;
	while(!m_vecStep.empty())
	{
		iCurIndex = m_vecStep.back();
		m_vecStep.pop_back();
		m_ppBaseNodeArray[iCurIndex] = NULL;
		++nFillCount;
		for (int i = 0; i < 4; ++i)
		{
			iIndexTemp = iCurIndex + INDEX_OFFSET[i];
			if (iIndexTemp >= 0 && iIndexTemp < iAllNum && m_ppBaseNodeArray[iIndexTemp]) //【注】: 这里没考虑左右边界,目的是填充边界,故可吧左右看成相通的, 地图似个圆筒状(错位了一行)
			{
				m_vecStep.push_back(iIndexTemp);
			}
		}
	}
	//LogConsole("填充了 " << nFillCount << "个与边界相连的格子 [" << x << ", " << y << "] \n");
}

void CLinkRegionBuild::SortRegion(VecLayerList_t& vecLayerList)
{
	VecRegionList_t* pVecFatherList, *pVecChildList;
	CLinkRegionBuildNode* pRegionTemp;
	int i, j, k;
	int nIndex;
	for (i = vecLayerList.size() -1; i > VALID_BEGIN_LAYER; --i)
	{
		pVecFatherList = vecLayerList[i];
		pVecChildList = vecLayerList[i-1];
		nIndex = pVecChildList->size() - 1;
		for (j = 0; j <= nIndex;)  //将没有父region的移至最后
		{
			pRegionTemp = (*pVecChildList)[j];
			if (!pRegionTemp->m_pFatherRegion)
			{
				(*pVecChildList)[j] = (*pVecChildList)[nIndex];
				(*pVecChildList)[nIndex] = pRegionTemp;
				--nIndex;
				continue;
			}
			++j;
		}
		for (j = pVecFatherList->size() -1; j >=0 ; --j)
		{
			pRegionTemp = (*pVecFatherList)[j];
			for (k = pRegionTemp->m_nChildRegionNum -1; k >=0 ; --k)			//!! 倒着排
			{
				(*pVecChildList)[nIndex] = pRegionTemp->m_ppChildRegion[k];
				--nIndex;
			}
		}
		Ast(nIndex == -1);
	}

	//对第一层进行index排序
	if (vecLayerList.size() > 1)
	{
		sort(vecLayerList[1]->begin(), vecLayerList[1]->end(), CompareRegion);
	}
}

bool CLinkRegionBuild::CreateLinkRegionFile(const std::string &path, IBarrierInfo* pBarrierInfo, int ePassType)
{
	FILE* pFile = fopen(path.c_str(),"wb");
	if (pFile == NULL)
	{
		string msg = path + "打开或创建失败";
		MessageBox(NULL, msg.c_str(),"错误",MB_OK);
		return false;
	}
	fclose (pFile);
	pFile = NULL;

	m_pBarrierInfo = pBarrierInfo;
	float fProgress = 0.0f;
	progress->SetProgress(fProgress);
	m_nWidth = pBarrierInfo->GetExactWidth();
	m_nHeight = pBarrierInfo->GetExactHeight();
	m_ppBaseNodeArray = new CLinkRegionBuildNode*[m_nWidth * m_nHeight];
	memset(m_ppBaseNodeArray, 0, m_nWidth * m_nHeight * sizeof(CLinkRegionBuildNode*));
	int eBarrierType;
	uint32 x, y;
	CLinkRegionBuildNode temp;
	for (x = 0; x < m_nWidth; ++x)
	{
		for (y = 0; y < m_nHeight; ++y)
		{
			eBarrierType = pBarrierInfo->GetBarrierType(x, y);
			if (eBarrierType <= ePassType)
			{
				m_ppBaseNodeArray[y * m_nWidth + x] = &temp;
			}
		}
	}
	
	FillSideBlock();
	fProgress = Fill_SIDE_BLOCK_PROGRESS;
	progress->SetProgress(fProgress);

	CreateBaseLinkRegionArray(BARRIER_WEIGHT);
	
	fProgress = BUILD_BASE_LAYER_PROGRESS;
	progress->SetProgress(fProgress);

	VecRegionList_t* pVecBaseRegionList = new VecRegionList_t;
	uint32 nSize = m_nWidth * m_nHeight;
	uint32 i, j, k;
	for (i = 0; i < nSize; ++i)
	{
		if (m_ppBaseNodeArray[i])
		{
			pVecBaseRegionList->push_back(m_ppBaseNodeArray[i]);
		}
	}


	m_vecLayer.push_back(pVecBaseRegionList);
	VecRegionList_t* pVecChildRegionList = pVecBaseRegionList;
	VecRegionList_t* pVecFatherRegionList = NULL;
	uint32 nUniteSize = UNITE_SIZE;
	
	float fALayerProgress = (BUILD_END_PROGRESS - BUILD_BASE_LAYER_PROGRESS) / 10;

	while (true)
	{
		pVecFatherRegionList = CreateFatherLayer(*pVecChildRegionList, nUniteSize);
		if (pVecFatherRegionList == NULL)
		{
			break;
		}
		m_vecLayer.push_back(pVecFatherRegionList);
		pVecChildRegionList = pVecFatherRegionList;
		nUniteSize *= UNITE_SIZE;

		
		if (fProgress <= BUILD_END_PROGRESS)
		{
			fProgress = fProgress + fALayerProgress;
			progress->SetProgress(fProgress);
		}
		
	}

	if (m_vecLayer.size() < VALID_BEGIN_LAYER + 1)
	{
		Clear();
		return false;
	}

	
	SortRegion(m_vecLayer);		//排序 将子region放在一起
	fALayerProgress = SORT_REGION_PROGRESS;
	progress->SetProgress(fProgress);

	uint32 nAllNodeNum = 1; //索引0保留为空node, 所以总数 + 1
	

	//为了知道指针的节点 的索引,先为每个节点计算索引
	fALayerProgress = ((1.0f - fProgress) / 4) / (m_vecLayer.size() -1);
	for (i = VALID_BEGIN_LAYER; i < m_vecLayer.size() ; ++i)
	{
		//LogConsole("第 " << i << " 层有 " << m_vecLayer[i]->size() << " 个连通区域\n");
		for (j = 0; j < m_vecLayer[i]->size(); ++j)
		{
			(*m_vecLayer[i])[j]->m_nGlobalIndex = nAllNodeNum ++;
			if ((*m_vecLayer[i])[j]->m_nLinkNum > 63)
			{
				MessageBox(NULL, "障碍太复杂导致连通区域超过 63个,无法保存!!!","错误",MB_OK);

				Clear();
				return false;
			}
		}
		fProgress = fProgress + fALayerProgress;
		progress->SetProgress(fProgress);
	}
	
	fALayerProgress = (1.0f - fProgress)  / (m_vecLayer.size() - 1);

	LogConsole("共 "<< nAllNodeNum - 1  << "个连通区域\n");

	pFile = fopen(path.c_str(),"wb");
	if (pFile == NULL)
	{
		string msg = path + "打开或创建失败";
		MessageBox(NULL, msg.c_str(),"错误",MB_OK);
		return false;
	}

	fwrite(&m_nWidth, sizeof(uint32), 1, pFile);
	fwrite(&m_nHeight, sizeof(uint32), 1, pFile);
	


	//低层只保存父region信息
	CLinkRegionBuildNode* pTemp;
	uint32 nBaseNodeNum = 0;
	for (i = 0; i < m_vecLayer[1]->size(); ++i)
	{
		pTemp = (*m_vecLayer[1])[i];
		if (pTemp->m_pFatherRegion)
			++nBaseNodeNum;
	}
	fwrite(&nBaseNodeNum, sizeof(uint32), 1, pFile);

	uint32 nIndex;
	for (i = 0; i < m_vecLayer[1]->size(); ++i)
	{
		pTemp = (*m_vecLayer[1])[i];
		if (pTemp->m_pFatherRegion)
		{
			nIndex = (pTemp->m_KeyPos.y >> 1) * (m_nWidth >> 1) + (pTemp->m_KeyPos.x >> 1);
			fwrite(&nIndex, sizeof(uint32), 1, pFile);
			fwrite(&pTemp->m_pFatherRegion->m_nGlobalIndex, sizeof(uint32), 1, pFile);
		}
	}
	

	fProgress = fProgress + fALayerProgress;
	progress->SetProgress(fProgress);

	fwrite(&nAllNodeNum, sizeof(uint32), 1, pFile);
	
	nUniteSize = UNITE_SIZE;
	fwrite(&nUniteSize, sizeof(uint32), 1, pFile);
	uint32 nBegionLayer = VALID_BEGIN_LAYER;
	fwrite(&nBegionLayer, sizeof(uint32), 1, pFile);

	CLinkInfoBuild* pLinkInfo;
	uint32 nFatherIndex;
	uint16 nKeyPosX;
	uint16 nKeyPosY;
	uint8 nNil8 = 0; 
	uint32 nNil32 = 0;
	for (i = VALID_BEGIN_LAYER; i < m_vecLayer.size() ; ++i)
	{
		for (j = 0; j < m_vecLayer[i]->size(); ++j)
		{
			pTemp = (*m_vecLayer[i])[j];
			nKeyPosX = uint16(pTemp->m_KeyPos.x);
			nKeyPosY = uint16(pTemp->m_KeyPos.y);
			fwrite(&nKeyPosX, sizeof(uint16), 1, pFile);
			fwrite(&nKeyPosY, sizeof(uint16), 1, pFile);
			nFatherIndex = pTemp->m_pFatherRegion ? pTemp->m_pFatherRegion->m_nGlobalIndex : 0;
			fwrite(&nFatherIndex, sizeof(uint32), 1, pFile);
			if(i == VALID_BEGIN_LAYER)
			{
				fwrite(&nNil8, sizeof(uint8), 1, pFile);
				fwrite(&nNil32, sizeof(uint32), 1, pFile);
			}
			else
			{
				fwrite(&pTemp->m_nChildRegionNum, sizeof(uint8), 1, pFile);
				fwrite(&pTemp->m_ppChildRegion[0]->m_nGlobalIndex, sizeof(uint32), 1, pFile);
			}


			fwrite(&pTemp->m_nLinkNum, sizeof(uint8), 1, pFile);
			for (k = 0; k < pTemp->m_nLinkNum; ++k)
			{
				pLinkInfo = pTemp->m_ppLinkInfo[k];
				fwrite(&pLinkInfo->m_pDestLinkRegion->m_nGlobalIndex, sizeof(uint32), 1, pFile);
				fwrite(&pLinkInfo->m_fFare, sizeof(float), 1, pFile);
			}
		}
		fProgress = fProgress + fALayerProgress;
		if (fProgress > 1.0f)
		{
			fProgress = 1.0f;
		}
		progress->SetProgress(fProgress);
	}
	
	fclose(pFile);
	progress->SetProgress(1.0f);

	Clear();
	return true;
}


void CLinkRegionBuild::CreateBaseLinkRegionArray(const float* pBarrierWeight)
{
	static const int OFFSET_8DIR[8][2] =  { 1,0,  -1,0,  0,1,  0,-1, 1,1,  1,-1,  -1,-1, -1,1};   //8向偏移数组

	uint32 i, j, k;
	uint32 x,y;
	float fFare;
	CLinkRegionBuildNode* pCurNode;
	CLinkRegionBuildNode* pAroundNode;
	std::vector<CLinkInfoBuild*> vecLinkTemp;

	for (x = 0; x < m_nWidth; ++x)
	{
		for (y = 0; y < m_nHeight; ++y)
		{
			if (m_ppBaseNodeArray[y * m_nWidth + x])
			{
				pCurNode = new CLinkRegionBuildNode();
				m_ppBaseNodeArray[y * m_nWidth + x] = pCurNode;
				pCurNode->m_KeyPos.x = x;
				pCurNode->m_KeyPos.y = y;
				pCurNode->SetChildLinkRegion(NULL);
			}
		}
	}

	float fWeight1, fWeight2;
	for (i = 0; i < m_nWidth; ++i)
	{
		for (j = 0; j < m_nHeight; ++j)
		{
			pCurNode = m_ppBaseNodeArray[j * m_nWidth + i];
			if (pCurNode)
			{
				fWeight1 = pBarrierWeight[m_pBarrierInfo->GetBarrierType(i,j)];
				vecLinkTemp.clear();
				for (k = 0; k < 8; ++k)
				{
					x = i + OFFSET_8DIR[k][0];
					y = j + OFFSET_8DIR[k][1]; 
					if (x >= 0 && x < m_nWidth && y >= 0 && y < m_nHeight)
					{
						pAroundNode = m_ppBaseNodeArray[y * m_nWidth + x];
						if(pAroundNode)
						{
							fWeight2 = pBarrierWeight[m_pBarrierInfo->GetBarrierType(x,y)];
							fFare = (fWeight1 + fWeight2) * 0.5f * ( k < 4 ? 1.0f: 1.414f);
							vecLinkTemp.push_back(new CLinkInfoBuild(pAroundNode, fFare));
						}
					}
				}
				pCurNode->SetLinkInfo(&vecLinkTemp);
			}
		}
	}
}

void CLinkRegionBuild::GetUniteRect(CIRect& uniteRect ,const CPos& pos, uint32 nSize)
{
	uniteRect.left = (pos.x / nSize) * nSize;
	uniteRect.bottom = (pos.y / nSize) * nSize;
	uniteRect.right = uniteRect.left + nSize;
	if (uniteRect.right > (int)m_nWidth)
	{
		uniteRect.right = m_nWidth;
	}
	uniteRect.top = uniteRect.bottom + nSize;
	if (uniteRect.top > (int)m_nHeight)
	{
		uniteRect.top = m_nHeight;
	}
}

inline bool CLinkRegionBuild::IsInUniteRect(const CIRect& uniteRect, const CPos& pos)
{
	return pos.x >= uniteRect.left && pos.x < uniteRect.right && pos.y >= uniteRect.bottom && pos.y < uniteRect.top;
}

inline CLinkRegionBuildNode* CLinkRegionBuild::GetBaseLinkRegionNode(const CPos& pos)
{
	return m_ppBaseNodeArray[pos.y * m_nWidth + pos.x];
}

CLinkRegionBuild::VecRegionList_t* CLinkRegionBuild::CreateFatherLayer(VecRegionList_t& vecChildLayer, uint32 nSize)
{
	int num = vecChildLayer.size();
	VecRegionList_t* pVecFatherLayer = new VecRegionList_t;
	VecRegionList_t& vecFatherLayer = *pVecFatherLayer;
	VecRegionList_t vecRegionList;
	CIRect uniteRect;
	CLinkRegionBuildNode* pRegion;
	CLinkRegionBuildNode* pFatherLinkRegion;

	//先生成上一层的连通区域
	for (int i = 0; i < num; ++i)
	{
		pRegion = vecChildLayer[i];
		if(pRegion->m_nLinkNum == 0)//独立连通区域 不再向上合并
		{	
			continue;
		}
		if (pRegion->m_pFatherRegion == NULL)
		{
			vecRegionList.clear();
			
			pFatherLinkRegion = new CLinkRegionBuildNode();
			pRegion->m_pFatherRegion =  pFatherLinkRegion;
			vecFatherLayer.push_back(pFatherLinkRegion);
			
			vecRegionList.push_back(pRegion);
			GetUniteRect(uniteRect, pRegion->m_KeyPos, nSize);
			UniteLinkRegion(vecRegionList, pRegion, uniteRect, pFatherLinkRegion);
			pFatherLinkRegion->SetChildLinkRegion(&vecRegionList);
			ComputeKeyPos(pFatherLinkRegion, uniteRect);
		}
	}
	
	num = vecFatherLayer.size();
	if (num == 0)//已经到最顶层了
	{
		delete pVecFatherLayer;
		pVecFatherLayer = NULL;
		return NULL;
	}
	//计算相连性
	VecLinkList_t	vecLinkList;
	for (int i = 0; i < num; ++i)
	{
		ComputeLinkInfo(vecFatherLayer[i], vecLinkList);
	}
	return pVecFatherLayer;
//	CreateFatherLayer(vecFatherLayer, nSize * UNITE_SIZE, LinkRegionMgr);
}

void CLinkRegionBuild::UniteLinkRegion(VecRegionList_t& vecUniteLinkRegion, CLinkRegionBuildNode* pOriginLinkRegion, const CIRect& uniteRect, CLinkRegionBuildNode* pFatherLinkRegion)
{	
	CLinkRegionBuildNode* pRegion;
	for (int i = pOriginLinkRegion->m_nLinkNum -1; i >= 0; --i)
	{
		pRegion = pOriginLinkRegion->m_ppLinkInfo[i]->m_pDestLinkRegion;
		if (pRegion->m_pFatherRegion == NULL && IsInUniteRect(uniteRect, pRegion->m_KeyPos))
		{
			pRegion->m_pFatherRegion = pFatherLinkRegion;
			vecUniteLinkRegion.push_back(pRegion);
			UniteLinkRegion(vecUniteLinkRegion, pRegion, uniteRect, pFatherLinkRegion);
		}
	}
}


bool CLinkRegionBuild::IsInLinkList(CLinkRegionBuildNode* pRegion, const VecLinkList_t& vecLinkList)
{
	for (int i = vecLinkList.size() -1; i >= 0; --i)
	{
		if (vecLinkList[i]->m_pDestLinkRegion == pRegion)
		{
			return true;
		}
	}
	return false;
}

void CLinkRegionBuild::GetBorderCenterNode(VecRegionList_t& vecRegionList, VecIntList_t& vecCoefList, CLinkRegionBuildNode* pRegion, const CIRect& uniteRect)
{
	int dx = uniteRect.right - uniteRect.left;
	int dy = uniteRect.top - uniteRect.bottom;
	CLinkRegionBuildNode** ppStartLinkRegion;
	ppStartLinkRegion = &m_ppBaseNodeArray[uniteRect.bottom * m_nWidth + uniteRect.left];
	GetOneSideBorderCenterNode(vecRegionList, vecCoefList, pRegion, ppStartLinkRegion, dx, 1);
	if (dy > 1)
	{
		ppStartLinkRegion = &m_ppBaseNodeArray[(uniteRect.top -1) * m_nWidth + uniteRect.left];
		GetOneSideBorderCenterNode(vecRegionList, vecCoefList, pRegion, ppStartLinkRegion, dx, 1);
		if (dy > 2)
		{
			ppStartLinkRegion = &m_ppBaseNodeArray[(uniteRect.bottom +1) * m_nWidth + uniteRect.left];
			GetOneSideBorderCenterNode(vecRegionList, vecCoefList, pRegion, ppStartLinkRegion, dy - 2, m_nWidth);
			if(dx > 1)
			{
				ppStartLinkRegion = &m_ppBaseNodeArray[(uniteRect.bottom +1) * m_nWidth + uniteRect.right -1];
				GetOneSideBorderCenterNode(vecRegionList, vecCoefList, pRegion, ppStartLinkRegion, dy - 2, m_nWidth);
			}
		}
	}
}

void CLinkRegionBuild::GetOneSideBorderCenterNode(VecRegionList_t& vecRegionList, VecIntList_t& vecCoefList, CLinkRegionBuildNode* pRegion, CLinkRegionBuildNode** ppStartLinkRegion, int iLength, int iOffset)
{
	CLinkRegionBuildNode* pCurLinkRegion;
	CLinkRegionBuildNode** ppCenter = NULL;

	uint32 num = 0;
	for (int i = 0; i < iLength; ++i, ppStartLinkRegion += iOffset)
	{
		pCurLinkRegion = *ppStartLinkRegion;
		if (pCurLinkRegion && pCurLinkRegion->IsInOtherLinkRegion(pRegion))
		{
			if (!ppCenter)
			{
				ppCenter = ppStartLinkRegion;
			}
			else if((num & 1) == 0)
			{
				ppCenter += iOffset;
			}
			++num;
		}
		else if(ppCenter)
		{
			vecRegionList.push_back(*ppCenter);
			//if ((num & 1) == 0)//偶数
			//{
			//	ppCenter += iOffset;
			//	vecRegionList.push_back(*ppCenter);
			//	vecCoefList.push_back(num >> 1);
			//	vecCoefList.push_back(num >> 1);
			//}
			//else
			//{
				vecCoefList.push_back(num);
			//}
			ppCenter = NULL;
			num = 0;
		}
	}
	if (ppCenter)
	{
		vecRegionList.push_back(*ppCenter);
		vecCoefList.push_back(num);
	}
}

float CLinkRegionBuild::GetPathLength(CLinkRegionBuildNode* pStartLinkRegion, CLinkRegionBuildNode* pEndLinkRegion, const CIRect* pUniteRect)
{
	Ast(pStartLinkRegion->m_nChildRegionNum == 0 && pEndLinkRegion->m_nChildRegionNum == 0);
	
	if (pStartLinkRegion == pEndLinkRegion)
	{
		return 0.0f;
	}

	++s_nSearchId;
	m_heapF.clear();
	CLinkRegionBuildNode* pCurLinkRegion, * pDestLinkRegion;
	int i;
	int dx, dy;
	int nDiagonal;
	int nStraight;
	int endX = pEndLinkRegion->m_KeyPos.x;
	int endY = pEndLinkRegion->m_KeyPos.y;
	float newG;
	float fH;
	pCurLinkRegion = pStartLinkRegion;
	pCurLinkRegion->m_fG = 0.0f;
	m_heapF.push(0.0f,pCurLinkRegion);
	pCurLinkRegion->Open(s_nSearchId);
	while(true)
	{
		pCurLinkRegion = m_heapF.top();
		m_heapF.pop();
		if (pEndLinkRegion->IsOpen(s_nSearchId))
		{
			return pEndLinkRegion->m_fG;
		}
		if (pCurLinkRegion->IsClose(s_nSearchId))
		{
			continue;
		}
		pCurLinkRegion->Close();
		for (i = pCurLinkRegion->m_nLinkNum - 1; i >=0; --i)
		{
			pDestLinkRegion = pCurLinkRegion->m_ppLinkInfo[i]->m_pDestLinkRegion;
			if (pDestLinkRegion->IsClose(s_nSearchId) || (pUniteRect && !IsInUniteRect(*pUniteRect, pDestLinkRegion->m_KeyPos) ))
			{
				continue;
			}
			newG = pCurLinkRegion->m_fG + pCurLinkRegion->m_ppLinkInfo[i]->m_fFare;
			if (pDestLinkRegion->IsOpen(s_nSearchId))
			{
				if (newG < pDestLinkRegion->m_fG)
				{
					pDestLinkRegion->m_fG = newG;

					dx = endX - pDestLinkRegion->m_KeyPos.x;
					dy = endY - pDestLinkRegion->m_KeyPos.y;

					nDiagonal = min(abs(dx), abs(dy));
					nStraight = (abs(dx) + abs(dy));

					fH = 1.414f * nDiagonal + nStraight - (nDiagonal << 1);
					m_heapF.push(fH + newG, pDestLinkRegion);
				}
			}
			else
			{
				dx = endX - pDestLinkRegion->m_KeyPos.x;
				dy = endY - pDestLinkRegion->m_KeyPos.y;

				nDiagonal = min(abs(dx), abs(dy));
				nStraight = (abs(dx) + abs(dy));

				fH = 1.414f * nDiagonal + nStraight - (nDiagonal << 1);

				pDestLinkRegion->m_fG = newG;
				m_heapF.push(fH + newG, pDestLinkRegion);
				pDestLinkRegion->Open(s_nSearchId);
			}
		}
	}
}

float CLinkRegionBuild::TotalCostSqrToBorder(CLinkRegionBuildNode* pRegion, VecRegionList_t& vecRegionList, VecIntList_t& vecCoefList, const CIRect& uniteRect)
{
	float fTotalCostSqr = 0.0f;
	float fCostSqr;
	for (int i = vecRegionList.size() -1; i >=0; --i)
	{
		fCostSqr = GetPathLength(vecRegionList[i], GetBaseLinkRegionNode(pRegion->m_KeyPos), &uniteRect);
		fCostSqr *= fCostSqr;
		fTotalCostSqr += fCostSqr * vecCoefList[i];
	}
	return fTotalCostSqr;
}

CLinkRegionBuildNode* CLinkRegionBuild::GetContainKeyPosLinkRegion(CLinkRegionBuildNode* pRegion, VecRegionList_t& vecRegionList, VecIntList_t& vecCoefList, const CIRect& uniteRect)
{
	CLinkRegionBuildNode* pMinCostRegion = NULL;
	float fMinCost;
	float fCost;
	
	for (int i = pRegion->m_nChildRegionNum -1; i >= 0; --i)
	{
		fCost = TotalCostSqrToBorder(pRegion->m_ppChildRegion[i], vecRegionList, vecCoefList, uniteRect);
		if (pMinCostRegion == NULL || fCost < fMinCost)
		{
			fMinCost = fCost;
			pMinCostRegion = pRegion->m_ppChildRegion[i];
		}
	}
	if (pMinCostRegion->m_nChildRegionNum == 0)
	{
		return pMinCostRegion;
	}
	else
	{
		return GetContainKeyPosLinkRegion(pMinCostRegion, vecRegionList, vecCoefList, uniteRect);
	}
}

void CLinkRegionBuild::ComputeKeyPos(CLinkRegionBuildNode* pRegion, const CIRect& uniteRect)
{
	VecRegionList_t vecRegionList;
	VecIntList_t vecCoefList;
	GetBorderCenterNode(vecRegionList, vecCoefList, pRegion, uniteRect);
	if (vecRegionList.size() >= 0)
	{
		pRegion->m_KeyPos = GetContainKeyPosLinkRegion(pRegion, vecRegionList, vecCoefList, uniteRect)->m_KeyPos;
	}
	else//独立连通区域, keypos 无意义, 无计算的必要 就赋第一个子连通区域的关键点了
	{
		pRegion->m_KeyPos = pRegion->m_ppChildRegion[0]->m_KeyPos; 
	}
}


void CLinkRegionBuild::ComputeLinkInfo(CLinkRegionBuildNode* pRegion, VecLinkList_t& vecLinkList)
{
	CLinkRegionBuildNode* pCurKeyPosLinkRegion = GetBaseLinkRegionNode(pRegion->m_KeyPos);
	CLinkRegionBuildNode* pChildLinkRegion;
	CLinkRegionBuildNode* pLinkeLinkRegion;
	vecLinkList.clear();
	int i, j;
	float fLength;
	for (i = pRegion->m_nChildRegionNum -1; i >=0; --i)
	{
		pChildLinkRegion = pRegion->m_ppChildRegion[i];
		for (j = pChildLinkRegion->m_nLinkNum -1; j >=0; --j)
		{
			pLinkeLinkRegion = pChildLinkRegion->m_ppLinkInfo[j]->m_pDestLinkRegion->m_pFatherRegion;
			if (pLinkeLinkRegion != pRegion && !IsInLinkList(pLinkeLinkRegion, vecLinkList))
			{
				fLength = GetPathLength(pCurKeyPosLinkRegion, GetBaseLinkRegionNode(pLinkeLinkRegion->m_KeyPos), NULL);
				vecLinkList.push_back(new CLinkInfoBuild(pLinkeLinkRegion, fLength));
			}
		}
	}
	pRegion->SetLinkInfo(&vecLinkList);
}

void CLinkRegionBuild::BatchCreateLinkRegionFile()
{
	VecPathList_t& vecPathList = AutoSearchFile::GetInst()->GetAllFiles("common\\map\\", ".amp");
	string sPath;
	VecPathList_t errPathList;
	LogConsole("共 " << vecPathList.size() << " 张图需要转换\n");
	for (uint32 i =0; i < vecPathList.size(); ++i)
	{
		sPath = vecPathList[i].substr(0, vecPathList[i].size() - 4);
		LogConsole("正在处理 "<< sPath.c_str() << "   (" << i << " / " << vecPathList.size() << ")\n");
		CBarrierData barrierData;
		if (barrierData.LoadBarrierData(sPath))
		{
			CreateLinkRegionFile(sPath + ".lrg", &barrierData, 1);
		}
		else
		{
			errPathList.push_back(sPath);
		}
	}

	if (!errPathList.empty())
	{
		string errString = "以下地图处理失败:\n\n";
		for (uint i = 0; i < errString.size(); ++i)
		{
			errString += errString[i] + "\n";
		}
		 MessageBox(NULL, errString.c_str(),"提示", MB_OK);
	}
}

