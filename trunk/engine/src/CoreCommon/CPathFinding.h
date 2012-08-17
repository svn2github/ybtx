#pragma once

#include "MathAlgorithm.h"
#include "IPathFinding.h"
#include "FindPathDefs.h"
#include "CPath.h"

#include "MapInfoManager.h"
#include "BarrierManager.h"
#include "CLinkRegionMgr.h"
#include "DrawLine.h"

#include <deque>
#include "CFindPathAllocator.h"


namespace sqr
{
	class CPkgFile;
	class CNode;
	class IDynamicBarrierInfo;

	class CMap
		:public IMap
		,public CFindPathMallocObject
	{
		friend class CTestSuiteMetaSceneServer;

	public:
		CMap(uint32 uWidthInRegion, uint32 uHeightInRegion, const CBarrierManager::VecBarrierMatrixSharedPtr* pVecBarrierMatrixSharedPtr = NULL);
		~CMap();

		bool IsIntegrate()const;
		void Release();

		const CBarrierManager::VecBarrierMatrixSharedPtr* GetBarrierMatrix()const;

		// 输出拐点信息，用于平滑问题
		//CPath* DumpEndOld(CNode* pEndNode, CNode* pBeginNode);
		CPath* DumpEnd(CNode* pEndNode, CNode* pBeginNode);
		void LoadRegionData(CPkgFile& File, uint32 uXInRegion, uint32 uYInRegion);
		void LoadRgnExactBlockData(CPkgFile& File, uint32 uXInRegion, uint32 uYInRegion);
		void LoadRegionWeight(CPkgFile& File, uint32 uXInRegion, uint32 uYInRegion){};
		void LoadLinkRegionData(CBufFile& File);

		void SetSharedData(IMap* pOtherMap);

		EBarrierType GetBarrierType(int32 x, int32 y);
		CPos GetBeginPos(const CPos& pos);

		// 鼠标寻路
		IPath* CreatePath(const CPos& Begin, const CPos& End, EFindPathType EFindType, EBarrierType EBarrier);
		IPath* CreatePath(const CPos& Begin, const CPos& End, EFindPathType EFindType, EBarrierType EBarrier, const IDynamicBarrierInfo* pBarrierInfo);
		IPath* CreatePath(const CPos& Begin, const CPos& End, EFindPathType EFindType, EBarrierType EBarrier, int32 nMaxStep);
		IPath* CreatePath(const CPos& Begin, const CPos& End, EFindPathType EFindType, EBarrierType EBarrier, int32 nMaxStep, const IDynamicBarrierInfo* pBarrierInfo);
		// 
		IPath* CreatePath(const int16* pPathData, uint16 uPathDataLen, const CPos& Begin, const CPos& End, const IDynamicBarrierInfo* pBarrierInfo);

		// 算法集
		CPath* LineAlgorithm(const CPos& Begin, const CPos& End);
		CPath* AStarAlgorithm(const CPos& Begin, const CPos& End);
		CPath* AStarLimiteAlgorithm(const CPos& Begin, const CPos& End);
		CPath* TerrianAStarAlgorithm(const CPos& Begin, const CPos& End);
		CPath* HypoLineAlgorithm(const CPos& Begin, const CPos& End);

	private:

		typedef deque<CPos, CFindPathAllocator<CPos> >	PathPosList;
		typedef deque<CNode*, CFindPathAllocator<CNode*> >	NodePathList;

		uint32      m_uWidthInRegion;
		uint32      m_uHeightInRegion;
		typedef vector<bool> VecBool;
		vector< VecBool, CFindPathAllocator<VecBool> >  m_LoadRegin;
	

		uint32 m_uCountInLine;				// 在A*寻路中直线查询多少次，用于测试速度用


		EFindPathType m_EFindType;
		EBarrierType  m_EBarrier;
		int32         m_nMaxStep;

		PathPosList  m_deqPathPos;
		CNode*        m_OptNode;

		// member of instead CNodeState
		CMapInfoManager  m_MapInfo;
		CBarrierManager  m_BarrierMgr;

		DrawLine         m_DrawLine;

		CLinkRegionMgr	m_LinkRegionMgr;
	};

}
