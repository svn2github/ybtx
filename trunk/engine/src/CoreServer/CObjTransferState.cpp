#include "stdafx.h"
#include "CObjTransferState.h"

CObjTransferState::CObjTransferState(CSyncCoreSceneServer*	pTargetScene, CFPos PixelPos)
:m_pTargetScene(pTargetScene)
,m_PixelPos(PixelPos)
,m_uRefNum(2)
{
}

CObjTransferState::~CObjTransferState(void)
{
}

bool CObjTransferState::DelRef()
{
	if (--m_uRefNum == 0)
		return true;
	return false;
}


