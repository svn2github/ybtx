#pragma once
#include "CCharacterVariantHandler.h"
#include "CCharacterFollower.h"
#include "CEntityMallocObject.h"

class CCharacterFollowerVariantCallback : public CVariantCallback, public CEntityMallocObject
{
public:
	CCharacterFollowerVariantCallback(CCharacterFollower* pFollower):m_pFollower(pFollower){}
	virtual ~CCharacterFollowerVariantCallback() {m_pFollower = NULL;}
	CCharacterFollower* GetCharacter() {return m_pFollower;}
private:
	CCharacterFollower* m_pFollower;
};

class SetNpcTypeVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	SetNpcTypeVariantCallback(CCharacterFollower* pFollower):CCharacterFollowerVariantCallback(pFollower) {}
	int exec(CCypherVariant *pVariant);
};

class LevelChangeVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	LevelChangeVariantCallback(CCharacterFollower* pFollower):CCharacterFollowerVariantCallback(pFollower) {}
	int exec(CCypherVariant *pVariant);
};

class SetClassVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	SetClassVariantCallback(CCharacterFollower* pFollower):CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant* pVariant);
};

class SetCampVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	SetCampVariantCallback(CCharacterFollower* pFollower):CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant *pVariant);
};

class SetNpcAITypeVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	SetNpcAITypeVariantCallback(CCharacterFollower* pFollower):CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant *pVariant);
};

class SetNpcActiveTypeVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	SetNpcActiveTypeVariantCallback(CCharacterFollower* pFollower):CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant *pVariant);
};

class SetAttackTypeVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	SetAttackTypeVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant *pVariant);
};

class SetNpcGameCampVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	SetNpcGameCampVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant *pVariant);
};

class SetNpcRealNameVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	SetNpcRealNameVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant* pVariant);
};

class SetBeShowInClientVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	SetBeShowInClientVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant *pVariant);
};

class OnSpeedChangeVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	OnSpeedChangeVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant *pVariant);
};

class SetAlertTargetVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	SetAlertTargetVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant* pVariant);
};

class SetPKSwitchVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	SetPKSwitchVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower) {}
	int exec(CCypherVariant *pVariant);
};

//class SetZoneTypeVariantCallback : public CCharacterFollowerVariantCallback
//{
//public:
//	SetZoneTypeVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
//	int exec(CCypherVariant *pVariant);
//};

class SetFighteDirVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	SetFighteDirVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant *pVariant);
};

class AddMasterVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	AddMasterVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant* pVariant);
};

class BirthCampChangeVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	BirthCampChangeVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant* pVariant);
};

class ShadowVariantVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	ShadowVariantVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant* pVariant);
};

class StealthVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	StealthVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant* pVariant);
};

class MaterialNumVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	MaterialNumVariantCallback(CCharacterFollower* pFollower) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant* pVariant);
};

class ExpOwnerIDVariantCallback : public CCharacterFollowerVariantCallback
{
public:
	ExpOwnerIDVariantCallback(CCharacterFollower* pFollower ) : CCharacterFollowerVariantCallback(pFollower){}
	int exec(CCypherVariant* pVariant);
};
