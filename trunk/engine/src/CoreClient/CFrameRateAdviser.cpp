#include "stdafx.h"
#include "CFrameRateAdviser.h"
#include "ExpHelper.h"

const uint16 CFrameRateAdviser::MaxRecordNum = 10;
CFrameRateAdviser* CFrameRateAdviser::m_pFrameRateAdviser = NULL;

CFrameRateAdviser::CFrameRateAdviser(uint32 uFrameNum)
:m_uFrameNum( uFrameNum ),m_uTotalTime(0)
{
	if( m_uFrameNum == 0 )
		GenErr("uFrameNum must greater than zero.");

	for( uint32 i=0 ; i< m_uFrameNum ; ++i )
		TellFrameTime(1);
}

CFrameRateAdviser::~CFrameRateAdviser(void)
{
}

CFrameRateAdviser* CFrameRateAdviser::Inst()
{
	if (!m_pFrameRateAdviser)
		m_pFrameRateAdviser = new CFrameRateAdviser(1);

	return m_pFrameRateAdviser;
}

void CFrameRateAdviser::TellFrameTime(uint32 uTime)
{
	//printf("获得的时间是%d\n", uTime);
	//这个时间队列里面放100个
	m_deqTime.push_back(uTime);
	if (m_deqTime.size() < MaxRecordNum)
		return;	
		
	m_deqTime.pop_front();
}


uint32 CFrameRateAdviser::GetFrameCyc()const
{
	uint32 uMaxValue = m_deqTime.front();
	uint32 uSumSqure = uMaxValue * uMaxValue;

	for(deque<uint32>::const_iterator it=++m_deqTime.begin() ; it != m_deqTime.end() ; ++it)
	{
		if( uMaxValue < *it ){
			uMaxValue = *it;
			uSumSqure += (uMaxValue * uMaxValue);
		}
	}
	//return 33;
	//return (uint32)sqrt((double)(uSumSqure/MaxRecordNum));
	return uMaxValue;
}