
#include "stdafx.h"

#include "IPathFinding.h"
#include "CPathFinding.h"
#include "CPkgFile.h"
#include "CoreCommon.h"
#include "CBufFile.h"
#include "CPath.inl"
#include "CNode.h"
#include "CNode.inl"
#include "CPath.h"
#include "ErrLogHelper.h"
#include "MapInfoManager.inl"
#include "TSqrAllocator.inl"
#include "CDumpEndDrawLine.h"
#include "CAStarDataMgr.h"
#include "CAStarData.inl"
#include "ErrLogHelper.h"
#include "TSqrAllocator.inl"
#include "TBinaryHeap.inl"



namespace sqr
{
	IMap* CreateMap(uint32 uWidthInRegion, uint32 uHeightInRegion, const VecBarrierMatrixSharedPtr* pVecBarrierMatrixSharedPtr)
	{
		return new CMap(uWidthInRegion, uHeightInRegion, pVecBarrierMatrixSharedPtr);
	}
}


CMap::CMap(uint32 uWidthInRegion, uint32 uHeightInRegion, const CBarrierManager::VecBarrierMatrixSharedPtr* pVecBarrierManager)
	: m_uWidthInRegion(uWidthInRegion)
	, m_uHeightInRegion(uHeightInRegion)
	, m_nMaxStep(-1) // 初始步长限制值
	, m_MapInfo(uWidthInRegion,uHeightInRegion)
	, m_BarrierMgr(&m_MapInfo, pVecBarrierManager)
	, m_DrawLine(&m_BarrierMgr, &m_MapInfo)
{
	m_LoadRegin.resize(uHeightInRegion);

	for( uint32 i=0; i<uHeightInRegion; ++i )
	{
		m_LoadRegin[i].resize( uWidthInRegion );
		m_LoadRegin[i].assign( uWidthInRegion,false ); // 初始化障碍矩阵为false(无障碍)
	}

	m_uCountInLine = 0;

	CAStarDataMgr::Inst().UpdateSize(m_MapInfo.GetQuarWidth(),  m_MapInfo.GetQuarHeight());

	// 所有挂载的CNodeState对象
}

CMap::~CMap()
{
}


const CBarrierManager::VecBarrierMatrixSharedPtr* CMap::GetBarrierMatrix()const
{
	return m_BarrierMgr.GetBarrierMatrix();
}

CPath* CMap::DumpEnd(CNode* pEndNode, CNode* pBeginNode)
{
	CAStarData& AStarData = *(CAStarDataMgr::Inst().GetAStarData());
	CNode* pCurrentNode = pEndNode;
	CPath* pPath = new CPath(this);
	CPos pos;
	while ( pCurrentNode != NULL )
	{
		AStarData.GetPos(pos.x, pos.y, pCurrentNode);
		m_deqPathPos.push_front(pos);
		pCurrentNode  = AStarData.GetFront(pCurrentNode);
	}

	PathPosList::iterator it = m_deqPathPos.begin();
	PathPosList::iterator itPos = it++;
	PathPosList::iterator itCur;
	PathPosList::iterator itBefore = itPos;

	if(eFPT_HypoAStarOptiUseWeight == m_EFindType || eFPT_AStarUseWeight == m_EFindType)
	{
		CDumpEndDrawLine dumpEndDrawLine(&m_BarrierMgr);
		dumpEndDrawLine.SetCheckType(m_BarrierMgr.GetBarrierType(itPos->x >>1, itPos->y>>1));
		for (; it != m_deqPathPos.end();++it)
		{
			itCur = it;
			if (! Line(itPos->x, itPos->y, itCur->x, itCur->y, dumpEndDrawLine) )
			{
				pPath->PushBackPath(*itPos);
				if (itBefore != itPos)
				{
					pPath->PushBackPath(*itBefore);
				}
				itPos = itCur;
				dumpEndDrawLine.SetCheckType(m_BarrierMgr.GetBarrierType(itPos->x >> 1, itPos->y >> 1));
			}
			itBefore = itCur;
		}
	}
	else
	{
		for (; it != m_deqPathPos.end();)
		{
			itCur = it;
			if ( Line(itPos->x, itPos->y, itCur->x, itCur->y, m_DrawLine) )
			{
				itBefore = itCur;
				++it;
			}
			else
			{
				pPath->PushBackPath(*itPos);
				itPos = itBefore;
			}
		}
	}

	pPath->PushBackPath(*itPos);
	AStarData.GetPos(pos.x, pos.y, pEndNode);
	if(pos != *itPos)
		pPath->PushBackPath(pos);

	m_deqPathPos.clear();

	return pPath;
}


IPath* CMap::CreatePath(const CPos& Begin, const CPos& End, EFindPathType EFindType, EBarrierType EBarrier,
						const IDynamicBarrierInfo* pBarrierInfo)
{
	// 设置动态障碍
	m_BarrierMgr.SetDynamicBarrier(pBarrierInfo);
	IPath* pPath = NULL;

	SQR_TRY
	{
		pPath = CreatePath( Begin, End, EFindType, EBarrier );
	}
	SQR_CATCH(exp)
	{
		// 清空动态障碍
		m_BarrierMgr.SetDynamicBarrier(NULL);
		exp.AppendMsg("没有限制步长");
		LogExp(exp);
		return NULL;
	}
	SQR_TRY_END;

	// 清空动态障碍
	m_BarrierMgr.SetDynamicBarrier(NULL);

	return pPath;
}

IPath* CMap::CreatePath(const CPos& Begin, const CPos& End, EFindPathType EFindType, EBarrierType EBarrier,
						int32 nMaxStep)
{
	uint32 nCacheMaxStep = m_nMaxStep;
	if ( nMaxStep > -1 )
		m_nMaxStep = nMaxStep << 1;

	IPath* pPath = CreatePath( Begin, End, EFindType, EBarrier );

	m_nMaxStep = nCacheMaxStep;

	return pPath;
}

IPath* CMap::CreatePath(const CPos& Begin, const CPos& End, EFindPathType EFindType, EBarrierType EBarrier,
						int32 nMaxStep, const IDynamicBarrierInfo* pBarrierInfo)
{
	int32 nCacheMaxStep = m_nMaxStep;
	if ( nMaxStep > -1 )
		m_nMaxStep = nMaxStep << 1;

	// 设置动态障碍
	m_BarrierMgr.SetDynamicBarrier(pBarrierInfo);

	IPath* pPath = NULL;

	SQR_TRY
	{
		pPath = CreatePath( Begin, End, EFindType, EBarrier );
		m_nMaxStep = nCacheMaxStep;
	}
	SQR_CATCH(exp)
	{
		// 清空动态障碍
		m_BarrierMgr.SetDynamicBarrier(NULL);
		ostringstream strm;
		strm << "步长限制 " << nMaxStep << endl;
		exp.AppendMsg(strm.str());
		LogExp(exp);
		return NULL;
	}
	SQR_TRY_END;

	// 清空动态障碍
	m_BarrierMgr.SetDynamicBarrier(NULL);

	return pPath;
}

// 创建路径函数，传入的Begin和End为Grid
IPath* CMap::CreatePath(const CPos& Begin, const CPos& End, EFindPathType EFindType, EBarrierType EBarrier)
{
	// 加入容错是为了防止gm指令的错误输入。这个代码必须在GetIndex之前做检查，不然会被GetIndex的断言掉
	if ( m_MapInfo.IndexError(Begin.x, Begin.y) == 1 || m_MapInfo.IndexError(End.x,End.y) == 1 )
		return NULL;

	// 记录当前信息
	m_EFindType   = EFindType;
	m_EBarrier    = EBarrier;

	Ast(EBarrier <= eBT_HighBarrier);

	// 设置寻路所关注的障碍类型
	m_BarrierMgr.SetJumpBarrier(EBarrier, EFindType, m_MapInfo.GetIndex(Begin.x, Begin.y),  m_MapInfo.GetIndex(End.x, End.y));

	CPath* pPath = NULL; // 节点在算法内部创建，因为如果算法不成功节点要为NULL

	// 外框架分支
	switch (EFindType)
	{
		case eFPT_LineIgnoreEnd: // 忽略终点为障碍点
			if ( ! m_BarrierMgr.CheckRangeBarrier(End) ) // 检查邻近八格是否为障碍
				return NULL;

			pPath = LineAlgorithm(Begin, End);
			break;
		case eFPT_AStarIgnoreEnd:
			if ( !m_BarrierMgr.CheckRangeBarrier(End) )
				return NULL;

			if (!(pPath = LineAlgorithm(Begin, End))) // 直线寻路不通，试用A*
				pPath = AStarAlgorithm(Begin, End);   // A*寻路不通返回NULL
			break;
		case eFPT_Line:  // 如果需求是直线寻路，寻不通则跳出循环(eFPT_LineIgnoreEnd)
			pPath = LineAlgorithm(Begin, End);
			break;
		case eFPT_AStar: // 如果需求是A*寻路，先进行直线寻路，不通再用A*
			if (!(pPath = LineAlgorithm(Begin, End))) // 直线寻路不通，试用A*
				pPath = AStarAlgorithm(Begin, End);   // A*寻路不通返回NULL
			break;
		case eFPT_HypoAStar: // 直接使用HypoLine寻路
			if (!(pPath = LineAlgorithm(Begin, End)))      // 直线寻路不通，试用A*
				if (!(pPath = AStarAlgorithm(Begin, End))) // A*寻路不通使用HypoLine
					pPath = HypoLineAlgorithm(Begin, End); // HypoLine寻路是一定成功的
			break;
		case eFPT_HypoAStarOpti:
			if (!(pPath = LineAlgorithm(Begin, End))) // 直线寻路不通，试用A*
				pPath = AStarAlgorithm(Begin, End); // A*寻路不通返回NULL
			break;
		case eFPT_AStarUseWeight:
			pPath = AStarAlgorithm(Begin, End); //直接就A*了,不再预先使用直线寻路(直线寻路无法考虑权值, 当直线能通时不一定是预想的结果)
			break;
		case eFPT_HypoAStarOptiUseWeight:
			pPath = AStarAlgorithm(Begin, End); //直接就A*了,不再预先使用直线寻路(直线寻路无法考虑权值, 当直线能通时不一定是预想的结果)
			break;
		//case eFPT_TerrainAStar: // 直接使用TerrainAStar进行寻路，不通则使用HypoLine
		//	if (!(pPath = TerrianAStarAlgorithm(QuarBegin, QuarEnd)))
		//		pPath = HypoLineAlgorithm(QuarBegin, QuarEnd);

		//	break;
		case eFPT_HypoLine: // 直接使用HypoLine寻路
			if (!(pPath = LineAlgorithm(Begin, End)))  // 直线寻路不通
				pPath = HypoLineAlgorithm(Begin, End); // HypoLine寻路是一定成功的
			break;
		default:
			break;
	}

	// 探测起点和终点是否相同
	if ( pPath != NULL )
	{
		if ( const_cast<const CPath*>(pPath)->GetPath().size() < 2 )
		{
			delete pPath;
			pPath = NULL;
		}
	}

	return pPath;
}


IPath* CMap::CreatePath(const int16* pPathData, uint16 uPathDataLen, const CPos& Begin, const CPos& End, const IDynamicBarrierInfo* pBarrierInfo)
{
	// 设置动态障碍
	m_BarrierMgr.SetDynamicBarrier(pBarrierInfo);
	CPath* pPath = NULL;
	SQR_TRY
	{
		pPath = new CPath(pPathData, uPathDataLen, Begin, End, this);
	}
	SQR_CATCH(exp)
	{
		// 清空动态障碍
		m_BarrierMgr.SetDynamicBarrier(NULL);
		LogExp(exp);
		return NULL;
	}
	SQR_TRY_END;

	// 清空动态障碍
	m_BarrierMgr.SetDynamicBarrier(NULL);
	return pPath;
}


// 读取区域障碍信息(Grid消耗信息也可以考虑在这里读)
void CMap::LoadRegionData(CPkgFile& File, uint32 uXInRegion, uint32 uYInRegion)
{
	uint8 nVal = 0, roadweight = 0;
	Ast(uYInRegion < m_LoadRegin.size() && uXInRegion < m_LoadRegin[0].size());
	if ( m_LoadRegin[uYInRegion][uXInRegion] == true )
		return;
	else
		m_LoadRegin[uYInRegion][uXInRegion] = true;

	// 遍历一个区域
	for(uint32 i = 0; i < eRegionSpan; ++i)
	{
		for(uint32 j = 0; j < eRegionSpan; ++j)
		{
			File.read( &(nVal) , sizeof(uint8) ); // 通过内存块拷贝的方式，读取障碍信息到变量(0, 1, 2, 3)
			File.read( &roadweight, sizeof(uint8));
			m_BarrierMgr.SetBarrier((uXInRegion * eRegionSpan) + j, (uYInRegion * eRegionSpan) + i, nVal);
		}
	}
}

// 读取区域精确障碍信息(Grid消耗信息也可以考虑在这里读)
void CMap::LoadRgnExactBlockData(CPkgFile& File, uint32 uXInRegion, uint32 uYInRegion)
{
	uint8 bExactBlockType[4];

	// 遍历一个区域
	for(uint32 i = 0; i < eRegionSpan; i++)
	{
		for(uint32 j = 0; j < eRegionSpan; j++)
		{
			File.read( &bExactBlockType, sizeof(bExactBlockType));
		}
	}
}

void CMap::LoadLinkRegionData(CBufFile& File)
{
	m_LinkRegionMgr.LoadData(File);
	
	CAStarDataMgr::Inst().UpdateAllNodeNum(m_LinkRegionMgr.GetAllNodeNum());
}

void CMap::SetSharedData(IMap* pOtherMap)
{
	m_LinkRegionMgr.SetSharedData(&(static_cast<CMap*>(pOtherMap)->m_LinkRegionMgr));
}

EBarrierType CMap::GetBarrierType(int32 x, int32 y)
{
	return m_BarrierMgr.GetBarrierType(x,y);
}

CPos CMap::GetBeginPos(const CPos& pos)
{
	CPos temp = pos;
	if(m_BarrierMgr.IsBarrier(temp.x, temp.y))
	{
		if (temp.x % 2 == 0)
		{
			temp.x = temp.x -1;
		}
		else
		{
			temp.x = temp.x +1;
		}
		if (!m_BarrierMgr.IsBarrier(temp.x, temp.y))
		{
			return temp;
		}
	}
	return pos;
}

// 目前没有使用
bool CMap::IsIntegrate()const
{
	for(uint32 i = 0; i < m_uHeightInRegion; i++)
	{
		for( uint32 j = 0; j < m_uWidthInRegion; j++ )
		{
			if(m_LoadRegin[i][j] == false)
				return false;
		}
	}

	return true;
}

// 销毁当前节点
void CMap::Release()
{
	delete this;
}


// 算法集
// 直线寻路
CPath* CMap::LineAlgorithm(const CPos& Begin, const CPos& End)
{
	if( Line(Begin.x, Begin.y, End.x, End.y, m_DrawLine) ) // 直线寻路
	{
		m_uCountInLine++;
		CPath* pPath = new CPath(this);
		pPath->AddPath(End);
		pPath->AddPath(Begin);
		return pPath;
	}

	return NULL; // 寻路失败
}


// A*寻路;
CPath* CMap::AStarAlgorithm(const CPos& Begin, const CPos& End)
{
	//LARGE_INTEGER nFreq;
	//QueryPerformanceFrequency(&nFreq);
	//LARGE_INTEGER   nBeginTime;
	//LARGE_INTEGER   nEndTime;
	//double useTime = 0.f;
	//QueryPerformanceCounter(&nBeginTime);

	// 如果终点是障碍点，直接返回NULL
	
	CPathDirect* pPathDirect;
	pPathDirect = m_LinkRegionMgr.GetPathDirect(GetBeginPos(Begin), End, m_EFindType, m_EBarrier);
	if (! pPathDirect)
	{
		return NULL;
	}
	int32 nEndX = End.x, nEndY = End.y;
	if( pPathDirect->GetEndPos() != End)
	{
		nEndX = pPathDirect->GetEndPos().x;
		nEndY = pPathDirect->GetEndPos().y;
		m_BarrierMgr.SetJumpBarrier(m_EBarrier, m_EFindType, m_MapInfo.GetIndex(Begin.x, Begin.y),  m_MapInfo.GetIndex(nEndX, nEndY));
	}
	
	
	//if(!m_BarrierMgr.JumpBarrier(m_MapInfo.GetIndex(nEndX, nEndY)))
	//	return NULL;

	CAStarData& AStarData = *(CAStarDataMgr::Inst().GetAStarData()); 
	CAStarData::MinHeap& heapF =  AStarData.GetHeapF();
	uint32 nSearchId = AStarData.GetSearchId();
	int* nOffset = AStarData.GetOffsetArray();
	heapF.clear();

	
	CPath* pPath = NULL;
	CNode* pNode = NULL;
	CNode* pBestNode;
	CNode* pEndNode;
	CNode* pBeginNode;
	CPathDirectNode* pDirectNode;

	int32 x, y;
	int dx1, dy1, dx2, dy2, targetX, targetY;
	int32 cross;
	uint32 nTargetIndex;
	float fRemainderLength;
	float fNewG;// 起点到当前格的移动消耗 
	float fH; // 从当前格到终点的移动消耗
	float fTargetH; //从当前格到指导点的移动消耗
	float fF; // F = G + H
	float nDiagonal; // 对角线
	float nStraight; // 正垂线
	float fNearSize = pPathDirect->GetNearSize();
	uint32 nMaxIndex = pPathDirect->GetPosNum() -1;

	// eFPT_HypoLine 不能判断最后一个是否是障碍，直接返回障碍前的目标
	// 而 eFPT_Line 如果最后一个是障碍也是寻不通的

	pEndNode = AStarData.GetNodeAddress(nEndX, nEndY);
	pBeginNode = AStarData.GetNodeAddress(Begin.x, Begin.y);
	
	float fWeight = 1.f;

	nDiagonal = static_cast<float>(min(abs(nEndX - Begin.x), abs(nEndY - Begin.y)));
	nStraight = static_cast<float>((abs(nEndX - Begin.x) + abs(nEndY - Begin.y)));
	fH = 1.414f * nDiagonal + (nStraight - 2 * nDiagonal);
	float MinH = fH;// 保存当前最接近目标点的节, 初始化为评估最大值

	pBeginNode->SetG(0);
	pBeginNode->Open(nSearchId);
	pBeginNode->SetFrontDir(8); // 8代表起点
	pBeginNode->SetTargetId(0);
	heapF.push(0.0f,pBeginNode);

	m_OptNode = pBeginNode;
	int32 nBestX = 0;
	int32 nBestY = 0;
	const int AroundSize = 8;
	int nOffNum = 0;
	uint32 uExploreTime = 0;
	
	if ( m_nMaxStep != -1 )
		uExploreTime = static_cast<uint32>(m_nMaxStep * m_nMaxStep);
	else
		uExploreTime = static_cast<uint32>(m_nMaxStep);

	//int num = 0;
	for ( uint32 i=0; i<uExploreTime; ++i )
	{
		//++num;
		pBestNode = AStarData.ReturnBestNode();      // 弹出开启列表中F最低值
		pBestNode->Close(nSearchId);		// 关闭
		AStarData.GetPos(nBestX, nBestY, pBestNode);
		nTargetIndex = pBestNode->GetTargetId();
		pDirectNode = pPathDirect->GetDirectNode(nTargetIndex);
		targetX = pDirectNode->GetPos().x;
		targetY = pDirectNode->GetPos().y;
		fRemainderLength = pDirectNode->GetRemainderLength();
		dx2 =  nBestX - targetX;
		dy2 =  nBestY - targetY;
		for (int j = 0; j < AroundSize; ++j)
		{
			nOffNum = j << 1;
			x = nBestX + nOffset[nOffNum];
			y = nBestY + nOffset[nOffNum+1];
			// 节点 x, y 不在范围内
			if(!m_MapInfo.CheckXYInRegion(x, y))
				continue;
			pNode = pBestNode + AStarData.GetNodeOffset(j);

			// 测试节点是否在关闭列表
			if ( pNode->IsClose(nSearchId) )
				continue;

			// 是否为可逾越障碍等级
			if ( !m_BarrierMgr.JumpBarrierAndGetWeight(m_MapInfo.GetIndex(x, y), fWeight))
				continue;

			j< 4 ? fNewG = 1.414f * fWeight : fNewG = (float)fWeight;
			fNewG += pBestNode->GetG();
			if (!pNode->IsOpen(nSearchId) ) // 节点不在开启列表
			{
				dx1 = targetX - x;
				dy1 = targetY - y;

				nDiagonal = static_cast<float>(min(abs(dx1), abs(dy1)));
				nStraight = static_cast<float>((abs(dx1) + abs(dy1)));
				cross = abs(dx1 * dy2 - dx2 * dy1);
				
				fTargetH = 1.414f * nDiagonal + (nStraight - 2 * nDiagonal);
				fH = fTargetH + fRemainderLength + static_cast<float>(cross) * 0.001f;

				pNode->Open(nSearchId);
				pNode->SetG(fNewG); // 记录新G值
				pNode->SetTargetId((fTargetH < fNearSize && nTargetIndex < nMaxIndex) ? nTargetIndex + 1: nTargetIndex);
				pNode->SetFrontDir(j);       // 记录父节点 方向

				fF = fNewG + fH;
				heapF.push(fF,pNode); // 记录F值
				
				// 保留最优点
				if ( fH < MinH )
				{
					MinH = fH;
					m_OptNode = pNode;
				}
			}
			else // 已在开启列表中，判断G值是否比较小，是则修改父节点为当前节点
			{
				if(fNewG < pNode->GetG())
				{
					pNode->SetG(fNewG);
					pNode->SetFrontDir(j);
				}
			}
		}

		// 如果终点已经在开启列表或关闭列表中
		if( pEndNode->IsOpen(nSearchId) )
		{
			pPath = DumpEnd(pEndNode, pBeginNode);
			break;
		}
		else if(heapF.empty()) // 开启列表已空
		{
			break;
		}
	}

	if ( pPath == NULL && (m_EFindType == eFPT_HypoAStarOpti || m_EFindType == eFPT_HypoAStarOptiUseWeight) && m_OptNode != pBeginNode )
		pPath = DumpEnd(m_OptNode, pBeginNode);
	
	//QueryPerformanceCounter(&nEndTime);
	//useTime = static_cast<double>(nEndTime.QuadPart - nBeginTime.QuadPart) / nFreq.QuadPart;
	//printf("use time: %f\n", useTime);
	//printf("指导点A* 遍历 %d  指导点数 %d \n", num, pPathDirect->GetPosNum());
	return pPath;
}

// 考虑地形的寻路算法
CPath* CMap::TerrianAStarAlgorithm(const CPos& Begin, const CPos& End)
{
	return NULL;
}

// 返回第一个障碍点的算法
CPath* CMap::HypoLineAlgorithm(const CPos& Begin, const CPos& End)
{
	CPath* pPath = new CPath(this);
	pPath->AddPath(CPos(m_DrawLine.GetLastX(), m_DrawLine.GetLastY())); // LastX和LastY来自直线寻路
	pPath->AddPath(Begin);

	return pPath;
}

