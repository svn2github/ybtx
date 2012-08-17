#include "stdafx.h"
#include "CDimensionInfo.h"
#include "TSqrAllocator.inl"

CDimensionInfo::CDimensionInfo()
{

}

CDimensionInfo::CDimensionInfo(const CDimensionInfo& info)
{
	m_DimToPrecision.clear();

	MapDimensionInfo::const_iterator iter = info.m_DimToPrecision.begin();
	MapDimensionInfo::const_iterator iter_end = info.m_DimToPrecision.end();
	while (iter != iter_end)
	{
		m_DimToPrecision[iter->first] = iter->second;
		++iter;
	}
}

CDimensionInfo::~CDimensionInfo()
{

}

void CDimensionInfo::SetDimensionInfo(uint32 uDimensionID, uint32 uPrecision)
{
	m_DimToPrecision[uDimensionID] = uPrecision;
}

const CDimensionInfo::MapDimensionInfo& CDimensionInfo::GetDimensionInfo()
{
	return m_DimToPrecision;
}
