#pragma once

#include "GameDef.h"
#include "NpcDefs.h"

class CCharacterDictator;

class NpcInfoMgr 
{
public:
	static bool BeActiveNpc(ENpcAIType eNpcAIType);
	static const TCHAR* GetAINameByAIType(ENpcAIType eAIType);
	static ENpcAIType GetAITypeByAIName(const TCHAR* szAIName);
	static const TCHAR* GetTypeNameByType(ENpcType eNpcType);
	static ENpcType GetTypeByTypeName( const TCHAR* szTypeName);
	static bool CanTakeOnlyOne(ENpcType eNpcType);
	static bool BeServantType(ENpcType eNpcType);
	static bool BeFollowMaster(ENpcType eNpcType);
	static bool BeKillServantByMasterDestory(ENpcType eNpcType);
	static ENpcEnmityType GetEnmityMgrType(ENpcType eNpcType);
	static bool BeFightNpc(ENpcAIType eNpcAIType);
	static bool BeServantTypeNeedToSync(ENpcType eNpcType);
	static bool CanBeChangeAI(ENpcAIType eAIType, EClass eClass, ECamp eCamp);	//不能被召唤的职业很阵营
	static bool BeFightNpc(const TCHAR* szNpcName);
	static bool CanChangeCamp(ENpcType eType);
};

