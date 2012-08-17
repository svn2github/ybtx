#include "stdafx.h"
#include "CObjDestoryState.h"

CObjDestoryState::CObjDestoryState(void)
:m_uRefNum(2)
{
}

CObjDestoryState::~CObjDestoryState(void)
{
}

bool CObjDestoryState::DelRef()
{
	if (--m_uRefNum == 0)
		return true;
	return false;
}
