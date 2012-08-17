#include "stdafx.h"
#include "CDynamicBarrierInfo.h"
#include "ExpHelper.h"
#include "TSqrAllocator.inl"

CDynamicBarrierInfo::CDynamicBarrierInfo(uint32 uWidth, uint32 uHeight)
:m_uWidthInGrid(uWidth)
,m_uHeightInGrid(uHeight)
#ifndef DynamicBarrierUsingHashMap
,m_BarrierInfo(uWidth*uHeight);
,m_pBarrierMatrix(uWidth*2*uHeight*2, eBT_NoBarrier);
#endif
{
}

CDynamicBarrierInfo::~CDynamicBarrierInfo()
{
}

void CDynamicBarrierInfo::SetBarrier(uint32 uOwnerID, const vector<CPos>& vecDBArea, EBarrierType eBarrierType)
{
	ClearBarrier(uOwnerID, vecDBArea);
	if(eBarrierType == eBT_NoBarrier)
	{
		return;
	}

	vector<CPos>::const_iterator it_pos = vecDBArea.begin();
	for(; it_pos != vecDBArea.end(); ++it_pos)
	{
		//cout << "DB " << it_pos->x << " " << it_pos->y << " " << eBarrierType << endl;
		uint32 uXInGrid = it_pos->x;
		uint32 uYInGrid = it_pos->y;
		//Ast(uXInGrid < m_uWidthInGrid && uYInGrid < m_uHeightInGrid);
		if(uXInGrid >= m_uWidthInGrid || uYInGrid >= m_uHeightInGrid)
		{
			continue;
		}

		uint32 uIndex = m_uWidthInGrid*uYInGrid+uXInGrid;

#ifdef DynamicBarrierUsingHashMap
		DynamicBarrierInfo::iterator it = m_BarrierInfo.find(uIndex);
		if(it == m_BarrierInfo.end())
		{
			MapGridDBInfo gridDBInfo;
			gridDBInfo.insert(make_pair(uOwnerID, uint8(eBarrierType)));
			m_BarrierInfo.insert(make_pair(uIndex, gridDBInfo));
		}
		else
		{
			MapGridDBInfo& mapGridDBInfo = it->second;
			if(!mapGridDBInfo.insert(make_pair(uOwnerID, static_cast<uint8>(eBarrierType))).second)
			{
				MapGridDBInfo::iterator it = mapGridDBInfo.find(uOwnerID);
				Ast(it != mapGridDBInfo.end());
				it->second = eBarrierType;
			}
		}
#else
		MapGridDBInfo& mapGridDBInfo = m_BarrierInfo[uIndex];
		if(!mapGridDBInfo.insert(make_pair(uOwnerID, static_cast<uint8>(eBarrierType))).second)
		{
			MapGridDBInfo::iterator it = mapGridDBInfo.find(uOwnerID);
			Ast(it != mapGridDBInfo.end());
			it->second = eBarrierType;
		}
#endif

		SetBarrierMatrix(uXInGrid, uYInGrid, eBarrierType);
	}
}

void CDynamicBarrierInfo::ClearBarrier(uint32 uOwnerID, const vector<CPos>& vecDBArea)
{
	if(vecDBArea.empty())
	{
		return;
	}

	vector<CPos>::const_iterator it = vecDBArea.begin();
	for(; it != vecDBArea.end(); ++it)
	{
		uint32 uXInGrid = it->x;
		uint32 uYInGrid = it->y;
		if(uXInGrid >= m_uWidthInGrid || uYInGrid >= m_uHeightInGrid)
		{
			continue;
		}

		uint32 uIndex = m_uWidthInGrid*uYInGrid+uXInGrid;

#ifdef DynamicBarrierUsingHashMap
		DynamicBarrierInfo::iterator it = m_BarrierInfo.find(uIndex);
		if(it == m_BarrierInfo.end())
		{
			SetBarrierMatrix(uXInGrid, uYInGrid, eBT_NoBarrier);
		}
		else
		{
			MapGridDBInfo& mapGridDBInfo = it->second;
			mapGridDBInfo.erase(uOwnerID);

			if(mapGridDBInfo.empty())
			{
				SetBarrierMatrix(uXInGrid, uYInGrid, eBT_NoBarrier);
			}
		}
#else
		MapGridDBInfo& mapGridDBInfo = m_BarrierInfo[uIndex];
		mapGridDBInfo.erase(uOwnerID);

		if(mapGridDBInfo.empty())
		{
			SetBarrierMatrix(uXInGrid, uYInGrid, eBT_NoBarrier);
		}
#endif
	}
}

void CDynamicBarrierInfo::SetBarrierMatrix(uint32 uIndex, uint8 bVal)
{
#ifdef DynamicBarrierUsingHashMap
	DynamicBarrierMatrix::iterator it = m_pBarrierMatrix.find(uIndex);
	if(it == m_pBarrierMatrix.end())
	{
		m_pBarrierMatrix.insert(make_pair(uIndex, bVal));
	}
	else
	{
		it->second = bVal;
	}
#else
	m_pBarrierMatrix[uIndex]= bVal;
#endif
}

void CDynamicBarrierInfo::SetBarrierMatrix(uint32 x, uint32 y, uint8 bVal)
{
	if(x < m_uWidthInGrid && y < m_uHeightInGrid)
	{
		uint32 DoubleX = x * 2;
		uint32 DoubleY = y * 2;
		SetBarrierMatrix(GetIndex(DoubleX, DoubleY), bVal);
		SetBarrierMatrix(GetIndex(DoubleX+1, DoubleY), bVal);
		SetBarrierMatrix(GetIndex(DoubleX, DoubleY+1), bVal);
		SetBarrierMatrix(GetIndex(DoubleX+1, DoubleY+1), bVal);
	}
}

uint32 CDynamicBarrierInfo::GetIndex(uint32 x, uint32 y)const
{
	return (y * m_uWidthInGrid * 2 + x);
}

EBarrierType CDynamicBarrierInfo::GetBarrier(const CPos& pos)const
{
	if(uint32(pos.x) >= m_uWidthInGrid || uint32(pos.y) >= m_uHeightInGrid)
		return eBT_OutRange;

	uint32 uIndex = GetIndex(pos.x * 2, pos.y * 2);
#ifdef DynamicBarrierUsingHashMap
	DynamicBarrierMatrix::const_iterator it = m_pBarrierMatrix.find(uIndex);
	if(it == m_pBarrierMatrix.end())
		return eBT_NoBarrier;
	return static_cast<EBarrierType>(it->second);
#else
	return static_cast<EBarrierType>(m_pBarrierMatrix[uIndex]);
#endif
}

EBarrierType CDynamicBarrierInfo::GetBarrier(uint32 uIndex)const
{
#ifdef DynamicBarrierUsingHashMap
		DynamicBarrierMatrix::const_iterator it = m_pBarrierMatrix.find(uIndex);
		if(it == m_pBarrierMatrix.end())
			return eBT_NoBarrier;
		return static_cast<EBarrierType>(it->second);
#else
		return static_cast<EBarrierType>(m_pBarrierMatrix[uIndex]&0x03);
#endif
}
