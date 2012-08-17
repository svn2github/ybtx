#include "stdafx.h"
#include "CharacterBrowserContext.h"
#include "GTRenderMgr.h"
#include "CCameraManager.h"

void CCharacterBrowserContext::_Update( uint32 uTime )
{
}

void CCharacterBrowserContext::_Render( uint32 uTime )
{
	CGTRenderMgr::GetInst()->OnRender();
}

bool  CCharacterBrowserContext::_ProcMsg( const CMsg& msg )
{
	CCameraManager::GetInst()->EventProcess(msg);
	return false;
}