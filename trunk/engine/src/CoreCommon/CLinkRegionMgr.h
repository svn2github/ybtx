#pragma once
#include "CPos.h"
#include "FindPathDefs.h"
#include "CLinkRegionNode.h"
#include "CAStarData.h"
#include "TBinaryHeap.h"
#include "CPathDirect.h"
#include <boost/shared_array.hpp>
#include "CFindPathAllocator.h"

namespace sqr
{

	class CBufFile;
	class CPath;

	class CLinkRegionMgr
		:public CFindPathMallocObject
	{
		typedef boost::shared_array<CLinkRegionNode>	RegionNodeArray_t;
		typedef boost::shared_array<CBaseRegionNode>	BaseNodeArray_t;
		typedef std::vector<CLinkRegionNode,CFindPathAllocator<CLinkRegionNode> >	VecLinkRegion_t;
		typedef	std::vector<CLinkRegionNode*,CFindPathAllocator<CLinkRegionNode*> >	VecLinkRegionNode_t;
		typedef CFindPathAllocator<float>	BHKeyAlloc;
		typedef CFindPathAllocator<pair<CLinkRegionNode*, uint32> >	BHValueAlloc;

		typedef TBinaryHeap<float, pair<CLinkRegionNode*, uint32>, less<float>, BHKeyAlloc, BHValueAlloc >	NearRegionHeap;
	public:
		CLinkRegionMgr(void);
		~CLinkRegionMgr(void);

		void LoadData(CBufFile& File); //仅第一个线程调用
		void SetSharedData(CLinkRegionMgr* pOthter);//其他线程调用
		uint32 GetAllNodeNum();
		
		CLinkRegionNode* GetNearestRegion(const CPos& pos, CLinkRegionNode* pTopRegion, uint32 nLayerLv);
		CLinkRegionNode* GetBaseLinkRegionNode(const CPos& pos);
		CLinkRegionNode* GetTopRegionNode(CLinkRegionNode* pBaseNode, uint32& nLayerLv);
		void GetEveryLayerLinkRegion(VecLinkRegionNode_t& vecRegionList, CLinkRegionNode* pBaseNode, uint32 nLayerLv);
		uint32 GetUniteSize();

		bool IsConnected(CLinkRegionNode* pNode1, CLinkRegionNode* pNode2, uint32&  nlayerLv);

		CPathDirect* GetPathDirect(const CPos& beginPos, const CPos& endPos, EFindPathType EFindType, EBarrierType EBarrier, uint32 nPathLength =0);
		void DumpPathDirect(CPathDirect& pathDirect,CLinkRegionNode* pBeginRegionNode, CNode* pEndNode);
		bool FindPathDirect(CPathDirect& pathDirect, CLinkRegionNode* pBeginNode,  CLinkRegionNode* pEndNode, bool bEndNodeIsFather, CPathDirect* pFatherDirect);
	private:
		RegionNodeArray_t m_pRegionNodeArray;
		BaseNodeArray_t m_pBaseInfoArray;
		uint32 m_nAllNodeNum;
		uint32 m_nBaseNodeNum;
		uint32 m_nUniteSize;
		uint32 m_nBeginLayer;

		uint32 m_nWidth;
		uint32 m_nHeight;
		NearRegionHeap	m_heapNearRegion;
		CAStarData* m_pAStarData;
		CPathDirect m_PathDirect1;
		CPathDirect m_PathDirect2;
	public:
		inline uint32 GetRegionIndex(CLinkRegionNode* pRegionNode);
		inline CNode* GetAStarDataNode(CLinkRegionNode* pRegionNode);
		inline CLinkRegionNode* GetRegionNode(CNode* pNode);
	};

}


