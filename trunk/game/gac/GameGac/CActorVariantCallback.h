#pragma once
#include "CCharacterVariantHandler.h"

class CActorFollower;

class CActorVariantCallback : public CVariantCallback
{
public:
	CActorVariantCallback(CActorFollower* pActor) : m_pActor(pActor) {}
	virtual ~CActorVariantCallback() {m_pActor = NULL;}
	CActorFollower* m_pActor;
};

class CMoveStateCallback : public CActorVariantCallback
{
public:
	CMoveStateCallback(CActorFollower* pActor) : CActorVariantCallback(pActor) {} 
	int exec(CCypherVariant *pVariant);
};

class CMoveDirCallback : public CActorVariantCallback
{
public:
	CMoveDirCallback(CActorFollower* pActor) : CActorVariantCallback(pActor) {}
	int exec(CCypherVariant *pVariant);
};

class CActionStateCallback : public CActorVariantCallback
{
public:
	CActionStateCallback(CActorFollower* pActor) : CActorVariantCallback(pActor) {}
	int exec(CCypherVariant *pVariant);
};

class CActionDirCallback : public CActorVariantCallback
{
public:
	CActionDirCallback(CActorFollower* pActor) : CActorVariantCallback(pActor) {}
	int exec(CCypherVariant *pVariant);
};

class CWalkBackRadiusCallback : public CActorVariantCallback
{
public:
	CWalkBackRadiusCallback(CActorFollower* pActor) : CActorVariantCallback(pActor) {}
	int exec(CCypherVariant *pVariant);
};

class CNAActionRhythmCallback : public CActorVariantCallback
{
public:
	CNAActionRhythmCallback(CActorFollower* pActor) : CActorVariantCallback(pActor) {}
	int exec(CCypherVariant *pVariant);
};
