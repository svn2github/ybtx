#include "stdafx.h"
#include "CEventCollector.h"
#include "CEventState.h"

CEventCollector::CEventCollector( CEventOwner* pOwner )
:m_pOwner(pOwner)
{
}

CEventCollector::~CEventCollector()
{

}

void CEventCollector::Collect( const CMsg& msg )
{
	if(CEventState::GetInst()->UpdateState(msg) && _ProcMsg(msg))
		_collect(msg);
}

void CEventCollector::Collect( const MSG_SOURCE& fSource,const MSG_TYPE& fType,const MSG_INFO& fInfo )
{
	CMsg msg(fSource,fType,fInfo);
	Collect(msg);
}

bool CEventCollector::_ProcMsg(const CMsg& msg )
{
	return msg.msgSource < MGS_CUSTOM;
}

void CEventCollector::_collect( const CMsg& msg )
{
	if(m_pOwner)
		m_pOwner->EventCollect(msg);
}

void CEventCollector::SetOwner( CEventOwner* pOwner )
{
	m_pOwner = pOwner;
}
