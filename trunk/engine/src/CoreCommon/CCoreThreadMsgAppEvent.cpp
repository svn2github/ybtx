#include "stdafx.h"
#include "CCoreThreadMsgAppEvent.h"
#include "CAppThreadEvent.h"

CCoreThreadMsgAppEvent::CCoreThreadMsgAppEvent(CAppThreadEvent* pEvent)
: m_pEvent(pEvent)
{
}

CCoreThreadMsgAppEvent::~CCoreThreadMsgAppEvent()
{
	delete m_pEvent;
}

