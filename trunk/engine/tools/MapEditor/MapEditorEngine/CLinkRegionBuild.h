#pragma once

#include "CRectangle.h"
#include "TBinaryHeap.h"
#include "CLinkRegionBuildNode.h"
#include "Engine.h"


class IBarrierInfo;

class CLinkRegionBuild
{
	typedef CLinkRegionBuildNode::VecRegionList_t	VecRegionList_t;
	typedef CLinkRegionBuildNode::VecLinkList_t	VecLinkList_t;
	typedef vector<VecRegionList_t*>	VecLayerList_t;
	typedef vector<int>			VecIntList_t;
	typedef TBinaryHeap<float, CLinkRegionBuildNode*, less<float>>		MinHeapF_t;
	typedef vector<int>		VecStepList_t;
	typedef vector<string>	VecPathList_t;
public:
	~CLinkRegionBuild();

	void Clear();

	bool CreateLinkRegionFile(const std::string &path, IBarrierInfo* pBarrierInfo, int ePassType);
	
	void BatchCreateLinkRegionFile();

	static CLinkRegionBuild& Inst()
	{
		static CLinkRegionBuild instance;
		return instance;
	}

	void GetUniteRect(CIRect& uniteRect ,const CPos& pos, uint32 nSize);

	void SetProcessCallback( IProgressCallback* p);
private:
	CLinkRegionBuild();

	void FillSideBlock();
	void FillArea(uint32 x, uint32 y);

	void CreateBaseLinkRegionArray(const float* pBarrierWeight);
	VecRegionList_t* CreateFatherLayer(VecRegionList_t& vecChildLayer, uint32 nSize);
	void SortRegion(VecLayerList_t& vecLayerList);
	
	inline bool IsInUniteRect(const CIRect& uniteRect, const CPos& pos);
	inline CLinkRegionBuildNode* GetBaseLinkRegionNode(const CPos& pos);

	bool IsInLinkList(CLinkRegionBuildNode* pRegion, const VecLinkList_t& vecLinkList);
	void UniteLinkRegion(VecRegionList_t& vecRegionList, CLinkRegionBuildNode* pOriginLinkRegion, const CIRect& uniteRect, CLinkRegionBuildNode* pFatherLinkRegion);
	
	void GetBorderCenterNode(VecRegionList_t& vecRegionList, VecIntList_t& vecCoefList, CLinkRegionBuildNode* pRegion, const CIRect& uniteRect);
	void GetOneSideBorderCenterNode(VecRegionList_t& vecRegionList, VecIntList_t& vecCoefList, CLinkRegionBuildNode* pRegion, CLinkRegionBuildNode** ppStartLinkRegion,int iLength, int iOffset);

	float GetPathLength(CLinkRegionBuildNode* pStartLinkRegion, CLinkRegionBuildNode* pEndLinkRegion,const CIRect* pUniteRect);
	float TotalCostSqrToBorder(CLinkRegionBuildNode* pRegion, VecRegionList_t& vecRegionList, VecIntList_t& vecCoefList, const CIRect& uniteRect);
	CLinkRegionBuildNode* GetContainKeyPosLinkRegion(CLinkRegionBuildNode* pRegion, VecRegionList_t& vecRegionList, VecIntList_t& vecCoefList, const CIRect& uniteRect);

	void ComputeKeyPos(CLinkRegionBuildNode* pRegion, const CIRect& uniteRect);
	void ComputeLinkInfo(CLinkRegionBuildNode* pRegion, VecLinkList_t& vecLinkList);

private:
	CLinkRegionBuildNode**	m_ppBaseNodeArray;
	VecLayerList_t	m_vecLayer;
	uint32			m_nWidth;
	uint32			m_nHeight;

	MinHeapF_t		m_heapF;
	VecStepList_t	m_vecStep;
	uint32	s_nSearchId;

	IProgressCallback* progress;
	IBarrierInfo* m_pBarrierInfo;
};
