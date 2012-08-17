#pragma once
#include "TEntityAdapterClient.h"
#include "TEntityTraitsClient.h"
#include "CoreObjectDefs.h"
#include "CDir.h"
#include "CEntityClientCreator.h"
#include "CActorIntObjClient.h"

namespace sqr
{
	class CCoreObjectFollower;
	class CCoreSceneClient;
}

class CIntObjClient;
class IIntObjClientHandler;
class IActorIntObjHandler;
class CActorClient;
class CFighterFollower;
class IFighterFollowerHandler;
class CCharacterFollower;
class IRenderObjClientHandler;

template < class Traits >
class TIntObjClientAdapter : public TEntityAdapterClient<Traits>
{
public:
	TIntObjClientAdapter(typename Traits::EntityType* pEntity);
	virtual ~TIntObjClientAdapter();
	CIntObjClient* GetIntObj()const{return this->GetEntity();}
	virtual uint32 GetEntityID()const{return this->GetEntity()->GetEntityID();}
	virtual CCharacterFollower* GetCharacter()const{return NULL;}

	virtual void OnDestroy(){CEntityClientCreator::DestroyEntityClient(this->GetEntity());}
	virtual void OnEnteredSyncAoi(){this->GetEntity()->OnEnteredSyncAoi();} 
	virtual void OnLeftedSyncAoi(){this->GetEntity()->OnLeftedSyncAoi();} 
	virtual void OnPosChanged(){this->GetEntity()->OnPosChanged();}
	virtual void OnRefreshRenderObjPos(const CFPos& PixelPos){this->GetEntity()->OnRefreshRenderObjPos(PixelPos);}
	virtual void OnMoveBegan(){}
	virtual void OnMovePathChanged(){}
	virtual void OnMoveEnded(){}
	virtual void OnWayPointChanged(){}

	virtual void BeSelected(){this->GetEntity()->OnBeSelected();}
	void OnAnimationEnd(const TCHAR* szName){this->GetEntity()->GetActor()->OnAnimationEnd(szName);}
	// 转调引擎对象中的函数
	virtual const CFPos& GetPixelPos()const{return this->GetEntity()->GetPixelPos();}
	virtual void GetServerPixelPos(CFPos& PixelPos)const{this->GetEntity()->GetServerPixelPos(PixelPos);}
	virtual void GetDirection(CDir& dir){this->GetEntity()->GetDirection(dir);}
	virtual bool IsPassive(){return true;} 
	virtual void StopMoving(){}
	virtual bool IsMoving(){return this->GetEntity()->IsMoving();}
	virtual CCoreSceneClient* GetScene()const{return this->GetEntity()->GetScene();}

	virtual EMoveToResult MoveTo(const CFPos& PixelPosDes,float fSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType,float fReachDist){return eMTR_NotAllowed;} 

	virtual CFighterFollower* GetSelectedFighter()const{return NULL;}
};

typedef TEntityTraitsClient
< CIntObjClient
, CCoreObjectFollower
, IIntObjClientHandler
, CActorIntObjClient
, IActorIntObjHandler
, CFighterFollower
, IFighterFollowerHandler
, CRenderObject
, IRenderObjClientHandler
> CIntObjClientTraits; 

typedef TIntObjClientAdapter<CIntObjClientTraits> CIntObjClientAdapter;
