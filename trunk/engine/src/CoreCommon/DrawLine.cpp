
#include "stdafx.h"
#include "DrawLine.h"
#include "BarrierManager.h"
#include "MapInfoManager.inl"


DrawLine::DrawLine(CBarrierManager* pBarrierMgr, CMapInfoManager* pMapInfo)
	: m_nX(-1)
	, m_nY(-1)
	, m_BarrierMgr(pBarrierMgr)
	, m_MapInfo(pMapInfo)
{
}

DrawLine::~DrawLine()
{
}

bool DrawLine::Do( int32 x, int32 y )
{
	bool ret = m_BarrierMgr->JumpBarrier(m_MapInfo->GetIndex(x, y));
	// 当前Grid为可忽略障碍等级(具体能够忽略障碍等级由该次寻路传进的EBarrier决定)
	if(ret == true)
	{
		m_nX = x; // 将当前Grid记录为可通行点(HypoLine寻路用到的LastX, LastY)
		m_nY = y;
	}
	return ret;
}

int32 DrawLine::GetLastX()
{
	return m_nX;
}

int32 DrawLine::GetLastY()
{
	return m_nY;
}
