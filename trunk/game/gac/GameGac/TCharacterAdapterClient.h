#pragma once
#include "TEntityAdapterClient.h"
#include "TEntityTraitsClient.h"
#include "CEntityClientCreator.h"

class CCharacterFollower;
class CCharacterDirector;
class ICharacterFollowerHandler;
class ICharacterDirectorHandler;
class CActorFollower;
class CActorDirector;
class IActorFollowerHandler;
class IActorDirectorHandler;
class CFighterFollower;
class CFighterDirector;
class IFighterFollowerHandler;
class IFighterDirectorHandler;
class CEntityClientCreator;
class IRenderObjClientHandler;

template<typename Traits>
class TCharacterAdapterClient
	:public TEntityAdapterClient<Traits>
{
public:
	TCharacterAdapterClient(typename Traits::EntityType* pEntity);
	virtual ~TCharacterAdapterClient();

	virtual uint32 GetEntityID()const{return this->GetEntity()->GetEntityID();}
	virtual CCharacterFollower* GetCharacter() const{return this->GetEntity();}

	virtual void OnDestroy(){CEntityClientCreator::DestroyEntityClient(this->GetEntity());} 
	virtual void OnEnteredSyncAoi(){this->GetEntity()->OnEnteredSyncAoi();} 
	virtual void OnLeftedSyncAoi(){this->GetEntity()->OnLeftedSyncAoi();} 
 	virtual void OnMoveBegan(){this->GetEntity()->OnMoveBegan();} 
 	virtual void OnMovePathChanged(){this->GetEntity()->OnMovePathChanged();} 
	virtual void OnMoveEnded(EMoveEndedType eMEType){this->GetEntity()->OnMoveEnded(eMEType);} 
	virtual void OnWayPointChanged(){this->GetEntity()->OnWayPointChanged();} 
	virtual void OnPosChanged(){this->GetEntity()->OnPosChanged();}
	virtual void OnRefreshRenderObjPos(const CFPos& PixelPos){this->GetEntity()->OnRefreshRenderObjPos(PixelPos);}
	virtual void OnSpeedChanged(){this->GetEntity()->OnSpeedChanged();}

	void OnDestoyed(){this->GetEntity()->OnRenderObjDestroyed();}
	void OnSepAnimationEnd(const TCHAR* szName){this->GetEntity()->GetActor()->OnSepAnimationEnd(szName);}
	void OnAnimationEnd(const TCHAR* szName){this->GetEntity()->GetActor()->OnAnimationEnd(szName);}
	void OnResLoaded(){this->GetEntity()->GetActor()->OnResLoaded();}
	void OnKeyFrame( const TCHAR * szName ){this->GetEntity()->GetActor()->OnKeyFrame(szName);}
	void OnSetVisable(bool isVisible){this->GetEntity()->GetActor()->OnSetVisable(isVisible);}
	virtual void BeSelected(){this->GetEntity()->OnBeSelected();}
	// 转调引擎对象中的函数
	virtual const CFPos& GetPixelPos()const{return this->GetEntity()->GetPixelPos();}
	
	virtual void GetServerPixelPos(CFPos& PixelPos)const{return this->GetEntity()->GetServerPixelPos(PixelPos);}
	virtual void GetDirection(CDir& dir){this->GetEntity()->GetDirection(dir);}
	virtual bool IsPassive(){return this->GetEntity()->IsPassive();} 
	virtual bool IsMoving(){return this->GetEntity()->IsMoving();} 
	virtual CCoreSceneClient* GetScene()const{return this->GetEntity()->GetScene();} 

	virtual CFighterFollower* GetSelectedFighter()const{return GetEntity()->GetSelectedFighter();}
};
 
typedef TEntityTraitsClient	
< CCharacterFollower
, CCoreObjectFollower
, ICharacterFollowerHandler
, CActorFollower
, IActorFollowerHandler
, CFighterFollower
, IFighterFollowerHandler
, CRenderObject
, IRenderObjClientHandler
> CCharacterFollowerTraits;

typedef TCharacterAdapterClient<CCharacterFollowerTraits> CCharacterFollowerAdapter;

typedef TEntityTraitsClient
< CCharacterDirector
, CCoreObjectDirector
, ICharacterDirectorHandler
, CActorDirector
, IActorDirectorHandler
, CFighterDirector
, IFighterDirectorHandler
, CRenderObject
, IRenderObjClientHandler
> CCharacterDirectorTraits;

typedef TCharacterAdapterClient<CCharacterDirectorTraits> CCharacterDirectorAdapter;
