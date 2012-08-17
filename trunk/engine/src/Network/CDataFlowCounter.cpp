#include "stdafx.h"
#include "CDataFlowCounter.h"

CDataFlowCounter::CDataFlowCounter()
{
	memset( m_aryFlowUnit, 0 , sizeof( m_aryFlowUnit ) );
	m_stIndex=0;
}

void CDataFlowCounter::DataFlow( size_t stSize ,uint64 uNow )
{
	CDataFlowUnit* pUnit= &m_aryFlowUnit[m_stIndex];

	//结算总时间
	pUnit->m_uTime = uNow - pUnit->m_uBeginTime;

	m_stIndex = (m_stIndex + 1) % stArySize;

	pUnit= &m_aryFlowUnit[m_stIndex];
	pUnit->m_uBeginTime=uNow;
	pUnit->m_stDataSize=stSize;
}

float CDataFlowCounter::GetFlowSpeed( uint64 uNow )
{
	DataFlow( 0, uNow );

	size_t	stTotalDataSize=0;
	uint64	uTotalTime=0;

	//计算索引前面的
	for( size_t i=0;i<stArySize ;++i )
	{
		const CDataFlowUnit& Unit= m_aryFlowUnit[i];
		stTotalDataSize += Unit.m_stDataSize;
		uTotalTime += Unit.m_uTime;
	}
	
	return stTotalDataSize/(float)uTotalTime;
}
