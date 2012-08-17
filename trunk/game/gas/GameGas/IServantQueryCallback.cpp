#include "stdafx.h"
#include "IServantQueryCallback.h"
#include "CCharacterDictator.h"
#include "CServantServerMgr.h"

void IServantQueryCallback::QueryServantJob(CCharacterDictator* pCharacter)
{
	if (!pCharacter)
		return;
	CServantServerMgr* pServantMgr = pCharacter->GetServantMgr();
	if (!pServantMgr)
		return;
	CServantServerMgr::ServantIDMapType& ServantMap = pServantMgr->GetServantIDMapType();
	CServantServerMgr::ServantIDMapType::iterator iter = ServantMap.begin();
	for (; iter != ServantMap.end(); iter++)
	{
		CCharacterDictator* pServant = CCharacterDictator::GetCharacterByID(iter->first);
		if (pServant)
		{
			this->Exec(pServant->GetEntityID(), pServant);
		}
	}
}
