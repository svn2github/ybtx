#pragma once
#include "FindPathDefs.h"
#include "BarrierManager.h"

class CDumpEndDrawLine
{
	CBarrierManager*	m_pBarrierMgr;
	EBarrierType		m_CheckType;
public:
	CDumpEndDrawLine(CBarrierManager* pBarrierMg):m_pBarrierMgr(pBarrierMg),m_CheckType(eBT_NoBarrier){}
	inline bool Do( int32 x, int32 y )
	{
		return m_CheckType == m_pBarrierMgr->GetBarrierType((x >> 1), (y >> 1) );
	}
	inline void SetCheckType(EBarrierType eCheckType){m_CheckType = eCheckType;}
};

