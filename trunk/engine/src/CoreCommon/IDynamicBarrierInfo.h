#pragma once
#include "FindPathDefs.h"

namespace sqr
{
	template<class T> struct TPos;
	typedef TPos<int32> CPos;

	class IDynamicBarrierInfo
	{
	public:
		virtual EBarrierType GetBarrier(uint32 uIndex)const=0;
		virtual EBarrierType GetBarrier(const CPos& pos)const=0;
		virtual ~IDynamicBarrierInfo(){}
	};
}
