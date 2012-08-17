#pragma once

class CEntityClient;
class CCharacterFollower;
class CCharacterDirector;
class CIntObjClient;

namespace sqr
{
	class CCoreObjectFollower;
	class CCoreObjectDirector;
};


class CEntityClientCreator
{
public:
	static CCharacterFollower* CreateCharacterFollower(CCoreObjectFollower* pCoreObj, uint32 uEntityID, bool bIsBattleHorse);
	static CCharacterDirector* CreateCharacterDirector(CCoreObjectDirector* pCoreObj, uint32 uEntityID);
	static CIntObjClient* CreateIntObjClient(CCoreObjectFollower* pCoreObj, uint32 uEntityID);
	static void DestroyEntityClient(CEntityClient* pEntity);
};