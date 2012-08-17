#include "stdafx.h"
#include "CCoreMsgAppEvent.h"
#include "CAppEvent.h"

CCoreMsgAppEvent::CCoreMsgAppEvent(CAppEvent* pEvent)
: m_pEvent(pEvent)
{
}

CCoreMsgAppEvent::~CCoreMsgAppEvent()
{
	delete m_pEvent;
}
