#pragma once
#include "CPos.h"

namespace sqr
{
	class CCoreSceneServer;
}

class CCharacterServer;
class CCharacterDictator;
class CCharacterMediator;
class CIntObjServer;
class CEntityServer;


class CEntityServerCreator
{
public:
	static CCharacterDictator* CreateCharacterDictator(CCoreSceneServer* pScene, const CFPos* pPixelPos, const TCHAR* szVarCfgName, const char* szName, bool IsIgnoreClog);
	static CCharacterDictator* CreateCharacterDictatorWithMaster(CCharacterDictator* pMaster, const TCHAR* szVarCfgName, const char* szName);
	static CCharacterMediator* CreateCharacterMediator(CCoreSceneServer* pScene, const CFPos* pPixelPos, const TCHAR* szVarCfgName);
	static CIntObjServer* CreateServerIntObject(CCoreSceneServer* pScene, const CFPos* pPixelPos, const TCHAR* szVarCfgName, const char* szName);
	static void DestroyEntity(CEntityServer* pEntity);
private:
	static void FindNoBarrierPos(CFPos& resultPos, CCoreSceneServer *pScene, const CFPos* pPixelPos);
	static void InitPlayerVariant(CCharacterMediator* pPlayer);
	static void InitNpcVariant(CCharacterDictator* pNpc);
	static void InitObjVariant(CIntObjServer* pObj);
};

