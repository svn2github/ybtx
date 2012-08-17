#include "stdafx.h"
#include "IGameGridRadiusCallback.h"
#include "CCoreSceneServer.h"
#include "ICoreObjectServerHandler.h"
#include "CCharacterDictator.h"
#include "CCoreObjectServer.h"
#include "CIntObjServer.h"
#include "CoreCommon.h"

void IGameGridRadiusCallback::ForEachInRadius(const CCoreSceneServer* pScene, const CPos& GridPos, float fRadius)
{
	if (!pScene)
		return;
	vector<CCoreObjectServer*> vecCoreObj;
	CFPos pixelPos;
	GridPos2PixelPos(pixelPos, GridPos);
	pScene->QueryObject(vecCoreObj, pixelPos, fRadius);
	for (vector<CCoreObjectServer*>::iterator it = vecCoreObj.begin(); it != vecCoreObj.end(); ++it)
	{
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		size_t eType = (size_t)(pHandler->GetTag());
		switch(eType)
		{
		case eCOHT_Character:
			{
				this->Exec(CCharacterDictator::GetCharacterByCoreObj(*it));
			}
			break;
		case eCOHT_Obj:
			{
				this->Exec(CIntObjServer::GetIntObjServer(*it));
			}
			break;
		default:
			break;
		}
	}
}

void IGameGridRadiusCallback::ForEachMediatorInRadius(const CCoreSceneServer* pScene, const CPos& GridPos, float fRadius)
{
	if (!pScene)
		return;
	vector<CCoreObjectServer*> vecCoreObj;
	CFPos pixelPos;
	GridPos2PixelPos(pixelPos, GridPos);
	pScene->QueryObject(vecCoreObj, pixelPos, fRadius);
	for (vector<CCoreObjectServer*>::iterator it = vecCoreObj.begin(); it != vecCoreObj.end(); ++it)
	{
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		size_t eType = (size_t)(pHandler->GetTag());
		switch(eType)
		{
		case eCOHT_Character:
			{
				CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByCoreObj(*it);
				if (pCharacter && pCharacter->CastToCharacterMediator())
					this->Exec(pCharacter);
			}
			break;
		default:
			break;
		}
	}
}
