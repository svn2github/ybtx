#include "stdafx.h"
#include "CDebugHelper.h"
#include "BaseHelper.h"
#include "CComputerInfo.h"
#include "MemoryHelper.h"

namespace sqr
{
	CDebugHelper::CDebugHelper()
	{
	}

	CDebugHelper::~CDebugHelper()
	{
		for ( MemEatenIter it = m_mapMemEaten.begin(); it != m_mapMemEaten.end(); )
		{
			free(it->second);
			m_mapMemEaten.erase(it++);
		}
	}

	void CDebugHelper::EatMemory( uint32 nMB )
	{
		if ( 0 == nMB )
			return;

		if ( m_mapMemEaten.find(nMB) != m_mapMemEaten.end() )
		{
			EatMemory(++nMB);
			return;
		}

		uint32 uEatSize = nMB*1024*1024;
		uint8* pMemToEat = (uint8*)malloc(uEatSize);
		if ( 0 == pMemToEat )
		{
			EatMemory(nMB/2);
			EatMemory(nMB/2);
		}
		else
		{
			memset(pMemToEat, 0x91, uEatSize);
			if ( pMemToEat != 0 )
				m_mapMemEaten.insert( make_pair(nMB, pMemToEat) );
		}
	}

	void CDebugHelper::ReleaseMemory( uint32 nMB )
	{
		MemEatenIter it = m_mapMemEaten.lower_bound(nMB);
		if ( it != m_mapMemEaten.end() )
		{
			free(it->second);
			m_mapMemEaten.erase(it);
		}
	}

	void CDebugHelper::EatMemoryUntil( uint32 nMB )
	{
		stringstream ss;
		uint32 uAvailMem = 0;

		ProcessMemInfo MemInfo;
		GetProcessMemInfo(&MemInfo);
		
		ss << MemInfo.stAvailPhysMem;
		ss >> uAvailMem;

		if ( uAvailMem > nMB)
		{
			EatMemory(uAvailMem - nMB);
		}
	}
}
