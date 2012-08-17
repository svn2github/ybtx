#pragma once
#include "CTick.h"
#include "CActorMallocObject.h"

class CCharacterDirector;
class CCharacterFollower;

class CNPCGazedAtMgr 
	: public CTick
	, public CActorMallocObject
{
public:
	CNPCGazedAtMgr(CCharacterDirector* pCharacter);
	~CNPCGazedAtMgr();

	void AddSyncNPC(CCharacterFollower* pCharacter);
	void DelSyncNPC(CCharacterFollower* pCharacter);

private:
	void OnTick();
	bool InGazedAtZone(CCharacterFollower* pCharacter);

	typedef vector<CCharacterFollower*> VectorGazedAtNPC;
	VectorGazedAtNPC m_vectorGazedAtNPC;

	CCharacterDirector* m_pCharacter;
	bool m_bStartUp;
};
