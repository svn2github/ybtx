#include "stdafx.h"
#include "CLock.h"
#include "ThreadHelper.h"

CLock::CLock()
{
	CreateLock(&m_Lock);
}

CLock::~CLock()
{
	DestroyLock(&m_Lock);
}


