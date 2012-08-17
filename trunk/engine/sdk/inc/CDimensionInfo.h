#pragma once 
#include "CDynamicObject.h"
#include "TCoreSceneAllocator.h"

namespace sqr
{
	class CDimensionInfo 
		:public CDynamicObject
		,public CCoreSceneMallocObject
	{
	public:
		typedef map<uint32, uint32, less<uint32>, TCoreSceneAllocator<uint32> > MapDimensionInfo;
		CDimensionInfo();
		CDimensionInfo(const CDimensionInfo& info);
		~CDimensionInfo();

		void SetDimensionInfo(uint32 uDimensionID, uint32 uPrecision);
		const MapDimensionInfo& GetDimensionInfo();	

	private:
		MapDimensionInfo	m_DimToPrecision;
	};
}
