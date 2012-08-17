#pragma once
#include "TEntityTraitsServer.h"
#include "TEntityAdapterServer.h"
#include "GameDef.h"
#include "CoreObjectDefs.h"
#include "FindPathDefs.h"

class CFighterDictator;
class CFighterMediator;
class IFighterServerHandler;
class CCharacterDictator;
class CCharacterMediator;
class ICharacterMediatorHandler;
class ICharacterDictatorHandler;
class CActorServer;
class IActorServerHandler;
class CIntObjServer;

template < class Traits >
	class TCharacterAdapterServer : public TEntityAdapterServer<Traits>
{
	public:

		/* ====================  LIFECYCLE     ======================================= */
		explicit TCharacterAdapterServer(typename Traits::EntityType *pEntity); /* constructor */
		virtual ~TCharacterAdapterServer();

		/* ====================  OPERATORS     ======================================= */

		/* ====================  OPERATIONS    ======================================= */

		/* ====================  INQUIRY       ======================================= */

		/* ====================  DATA MEMBERS  ======================================= */

		/* ====================  ACCESS        ======================================= */

	public:
		/* ====================  ACCESS        ======================================= */
		CCharacterDictator* GetCharacter()const {return this->GetEntity();}
		CIntObjServer* GetIntObj() const{return NULL;}
		EGameEntityType	GetEntityType()const {return this->GetEntity()->GetGameEntityType();}
		uint32 GetEntityID()const { return this->GetEntity()->GetEntityID(); }
		uint32 GetTeamID()const{return this->GetEntity()->GetTeamID();}
		uint32 GetTroopID()const{return this->GetEntity()->GetTroopID();}

		//Fighter
		void OnEnterBattleState() { this->GetEntity()->OnEnterBattleState(); }
		void OnLeaveBattleState() { this->GetEntity()->OnLeaveBattleState(); }
		void OnAgileValueChanged(uint32 uValueTypeID, float fValue)const { this->GetEntity()->OnAgileValueChanged(uValueTypeID, fValue); }
		void OnCtrlValueChanged(uint32 uValue, uint32 uValueTypeID, bool bSet)const { this->GetEntity()->OnCtrlValueChanged(uValue,uValueTypeID,bSet); }
		void OnDoSkillCtrlStateChanged(uint32 uState, bool bSet)const { this->GetEntity()->OnDoSkillCtrlStateChanged(uState,bSet); }

		void OnSkillBegin()const{ this->GetEntity()->OnSkillBegin();}
		void OnSkillEnd(bool bSucceed)const{ this->GetEntity()->OnSkillEnd(bSucceed);}

		//CoreObj Message
		void OnEvent(const void* pData){this->GetEntity()->OnCoreEvent(pData);};
		void OnMoveBegan(){this->GetEntity()->OnMoveBegan();}
		void OnMoveEnded(EMoveEndedType eMEType, uint32 uMoveID){this->GetEntity()->OnMoveEnded(eMEType,uMoveID);}
		void OnDestroy() { this->GetEntity()->Release();}

		void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension) { this->GetEntity()->OnCaughtViewSightOf(uObjGlobalId, uDimension); }
		void OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension) { this->GetEntity()->OnLostViewSightOf(uObjGlobalId, uDimension); }

		void OnTransfered(CCoreSceneServer* pOldScene,const CFPos& OldPos) {this->GetEntity()->OnTransfered(pOldScene,OldPos);}

		void OnTraceEnded(ETraceEndedType eType) { this->GetEntity()->OnTraceEnded(eType);}
};

typedef TEntityTraitsServer
	< CCharacterDictator
	, CCoreObjectDictator
	, ICharacterDictatorHandler
	, CActorServer
	, IActorServerHandler
	, CFighterDictator
	, IFighterServerHandler
	> CCharacterDictatorTraits;

typedef TCharacterAdapterServer<CCharacterDictatorTraits> CCharacterDictatorAdapter;

typedef TEntityTraitsServer
	< CCharacterMediator
	, CCoreObjectMediator
	, ICharacterMediatorHandler
	, CActorServer
	, IActorServerHandler
	, CFighterMediator
	, IFighterServerHandler
	> CCharacterMediatorTraits;

typedef TCharacterAdapterServer<CCharacterMediatorTraits> CCharacterMediatorAdapter;
