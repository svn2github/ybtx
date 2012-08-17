#pragma once
#include <set>
#include <map>
#include "TServantMgr.h"
#include "GameDef.h"

class CCharacterFollower;

class CServantClientMgr : public TServantMgr<CCharacterFollower>
{
public:
	CServantClientMgr(CCharacterFollower* pOwner)
		:TServantMgr<CCharacterFollower>(pOwner){}
	~CServantClientMgr(){}
};
