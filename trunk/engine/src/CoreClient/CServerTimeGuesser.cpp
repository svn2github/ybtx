#include "stdafx.h"
#include "CServerTimeGuesser.h"
#include "CTimeSystemClient.h"


CServerTimeGuesser::CServerTimeGuesser()
:m_uBaseServerTime(0)
,m_uBaseFrameTime(0)
{
}

uint64 CServerTimeGuesser::Guess()const
{
	if(m_uBaseServerTime == 0)
		return 0;

	return m_uBaseServerTime + m_tcpElapsedTime.GetElapse();
}

uint64 CServerTimeGuesser::GetElapse()const
{
	return m_tcpElapsedTime.GetElapse();
}

uint64 CServerTimeGuesser::GetElapsedFrameTime()const
{
	if(m_uBaseServerTime == 0)
		return 0;
	return CTimeSystemClient::Inst()->GetFrameTime() - m_uBaseFrameTime + m_uBaseServerTime;
}


void CServerTimeGuesser::SetBaseServerTime(uint64 uBaseServerTime)
{
	m_uBaseServerTime = uBaseServerTime;
	m_uBaseFrameTime = CTimeSystemClient::Inst()->GetFrameTime();
	m_tcpElapsedTime.SetCheckPoint();
}
