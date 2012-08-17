#pragma once
#include "GameDef.h"

namespace sqr
{
	class CCoreSceneServer;
}

class CCharacterDictator;
class CNpcServerCreator
{
public:
	static void CreateServerNpcAI( CCharacterDictator* pCharacter, const TCHAR* szNpcName,uint32 uLevel ,const TCHAR* AITypeName, const TCHAR* szNpcType, EClass eClass, ECamp eCamp, const TCHAR* szServantRealName, uint32 uMasterObjID=0);
private:
	static void CreateNpcAI(CCharacterDictator *pCharacter, const TCHAR* szNpcName, const TCHAR* AITypeName, const TCHAR* szNpcType, const TCHAR* szServantRealName, uint32 uMasterObjID);
};
