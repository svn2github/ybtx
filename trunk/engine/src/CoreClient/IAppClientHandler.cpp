#include "stdafx.h"
#include "IAppClientHandler.h"
#include "CCoreSceneClient.h"
#include "CFileWatcherData.h"
#include "CRenderSystem.h"

IAppClientHandler::IAppClientHandler() //这个接口Set给App写在了TApp中，暂时先从这个地方设置进去，以后再想方法整理顺这个地方
{
	CRenderSystem::GetInst()->SetProcessCallBack(this);
}

void IAppClientHandler::OnMainSceneCreated(CCoreSceneClient* pCoreScene) 
{
	CALL_CLASS_CALLBACK_1(pCoreScene) 
}

void IAppClientHandler::OnFileChanged(CFileWatcherData* pFileDataVec)
{
	CALL_CLASS_CALLBACK_1(pFileDataVec); 
}
