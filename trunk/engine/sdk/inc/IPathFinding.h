#pragma once
#include "CPos.h"
#include "FindPathDefs.h"
#include "CFindPathAllocator.h"
#include <boost/shared_ptr.hpp>


namespace sqr
{
	class CPkgFile;
	class CBufFile;
	class IDynamicBarrierInfo;

	typedef deque<CPos, CFindPathAllocator<CPos> > PathList;

	class IPath
	{
	public:
		virtual bool GetPos(CPos& Pos,uint32 uIndex)const=0;
		virtual uint32 GetPathSize()const=0;
		virtual const PathList& GetPath()const=0;
		virtual IPath* Clone()=0;
		virtual void Release()=0;
		virtual ~IPath(){}
	};

	typedef boost::shared_ptr<vector<uint8,CFindPathAllocator<uint8> > > VecBarrierMatrixSharedPtr;

	class IMap
	{
	public:
		virtual const VecBarrierMatrixSharedPtr* GetBarrierMatrix()const=0;
		//CreatePath 中的EBarrierType 表示能跳过的障碍信息。eBT_HighBarrier就表示能跳过全部的信息
		virtual IPath* CreatePath(const CPos& Begin, const CPos& End, EFindPathType EFindType, EBarrierType EBarrier)=0;
		virtual IPath* CreatePath(const CPos& Begin, const CPos& End, EFindPathType EFindType, EBarrierType EBarrier, const IDynamicBarrierInfo* pBarrierInfo)=0;
		virtual IPath* CreatePath(const CPos& Begin, const CPos& End, EFindPathType EFindType, EBarrierType EBarrier, int32 nMaxStep)=0;
		virtual IPath* CreatePath(const CPos& Begin, const CPos& End, EFindPathType EFindType, EBarrierType EBarrier, int32 nMaxStep, const IDynamicBarrierInfo* pBarrierInfo)=0;

		virtual IPath* CreatePath(const int16* pPathData, uint16 uPathDataLen, const CPos& Begin, const CPos& End, const IDynamicBarrierInfo* pBarrierInfo = NULL)=0;
		virtual EBarrierType GetBarrierType(int32 x, int32 y)=0;
		virtual void LoadRegionData(CPkgFile& File,uint32 uXInRegion,uint32 uYInRegion)=0;
		virtual void LoadRgnExactBlockData(CPkgFile& File,uint32 uXInRegion,uint32 uYInRegion)=0;
		virtual void LoadRegionWeight(CPkgFile& File,uint32 uXInRegion,uint32 uYInRegion)=0;
		virtual void LoadLinkRegionData(CBufFile& File)=0;
		virtual void SetSharedData(IMap* pOtherMap) = 0;

		virtual void Release()=0;
		virtual ~IMap(){}
	};

	IMap* CreateMap(uint32 uWidthInRegion, uint32 uHeightInRegion, const VecBarrierMatrixSharedPtr* pVecBarrierMatrixSharedPtr = NULL);
}

