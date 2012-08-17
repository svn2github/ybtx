#pragma once
#include "TEntityAdapterServer.h"
#include "TEntityTraitsServer.h"
#include "GameDef.h"
#include "CPos.h"
#include "CoreObjectDefs.h"
#include "FindPathDefs.h"

class CIntObjServer;
class IIntObjServerHandler;
class CActorIntObj;
class IActorIntObjHandler;
class IFighterServerHandler;
class CCharacterDictator;
class CFighterDictator;

template < class Traits >
class TIntObjAdapterServer : public TEntityAdapterServer<Traits>
{
	typedef TEntityAdapterServer<Traits> Parent_t;
public:

	explicit TIntObjAdapterServer(typename Traits::EntityType *pEntity);                             /* constructor */
	virtual ~TIntObjAdapterServer();
	CCharacterDictator* GetCharacter()const{return NULL;}
	CIntObjServer* GetIntObj() const{return Parent_t::GetEntity();}
	EGameEntityType	GetEntityType()const {return this->GetEntity()->GetGameEntityType();}
	uint32 GetEntityID()const { return this->GetEntity()->GetEntityID(); }

	void OnEnterBattleState() {}
	void OnLeaveBattleState() {}
	void OnAgileValueChanged(uint32 uValueTypeID, float fValue)const {}
	void OnCtrlValueChanged(uint32 uValue, uint32 uValueTypeID, bool bSet)const{}
	void OnDoSkillCtrlStateChanged(uint32 uState, bool bSet)const{}
	void OnSkillBegin()const{}
	void OnSkillEnd(bool bSucceed)const{}
	void OnAttackAniBegin()const{}
	void OnAttackAniEnd()const{}

	virtual void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension) {	this->GetEntity()->OnCaughtViewSightOf(uObjGlobalId, uDimension); }
	virtual void OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension) { this->GetEntity()->OnLostViewSightOf(uObjGlobalId, uDimension); }
	void OnDestroy() { this->GetEntity()->Release();}
};

typedef TEntityTraitsServer
< CIntObjServer
, CCoreObjectDictator
, IIntObjServerHandler
, CActorIntObj
, IActorIntObjHandler
, CFighterDictator
, IFighterServerHandler
> CCIntObjTraits; 

typedef TIntObjAdapterServer<CCIntObjTraits> CIntObjServerAdapter;
