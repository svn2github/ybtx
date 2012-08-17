#pragma once

#include "FindPathDefs.h"
#include "CFindPathAllocator.h"
#include <boost/shared_ptr.hpp>

namespace sqr
{
	template<class T> struct TPos;
	typedef TPos<int32> CPos;

	class CMapInfoManager;
	class IDynamicBarrierInfo;


	class CBarrierManager
		:public CFindPathMallocObject
	{
		typedef vector<uint8,CFindPathAllocator<uint8> > VecBarrierMatrix;
	public:
		typedef boost::shared_ptr<VecBarrierMatrix> VecBarrierMatrixSharedPtr;
		CBarrierManager(CMapInfoManager* pMapInfo, const VecBarrierMatrixSharedPtr* pVecBarrierMaxtrixSharedPtr);
		~CBarrierManager();

		void SetJumpBarrier(EBarrierType eBarrier,EFindPathType EFindType,uint32 uBeginIndex,uint32 uEndIndex);
		bool JumpBarrier(uint32 uIndex)const;
		bool JumpBarrierAndGetWeight(uint32 uIndex, float& uBarrierWeight)const;
		bool IsBarrier(int32 x, int32 y)const;
		bool CheckRangeBarrier(const CPos& grid)const;
		void SetDynamicBarrier(const IDynamicBarrierInfo* pDynamicBarrier);
		EBarrierType GetQuarBarrierType(int32 x, int32 y)const;			//只获取静态障碍信息	
		void SetBarrier(uint32 x, uint32 y, uint8 bVal);
		EBarrierType GetBarrierType(int32 x, int32 y)const;
		const VecBarrierMatrixSharedPtr* GetBarrierMatrix()const;

	private:
		EBarrierType GetDynamicBarrierType(uint32 uIndex)const;
		CMapInfoManager*  m_MapInfo;
		EBarrierType m_eJumpBarrier;         // 当前寻路所能忽略的障碍等级，具体参见CNodeState::JumpBarrier
		uint32 m_uBeginIndex, m_uEndIndex;   // 保存寻路的开始和结束点
		EFindPathType m_eFindType;           // 保存寻路模式
		float*		  m_pBarrierWeight;			//保存但前寻路模式不同障碍类型对应的权值数组

		VecBarrierMatrixSharedPtr	m_pBarrier;	// 障碍矩阵
		const IDynamicBarrierInfo*	m_pDynamicBarrier;      // 动态障碍信息
	};

}
