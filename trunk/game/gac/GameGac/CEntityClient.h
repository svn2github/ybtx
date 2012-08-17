#pragma once
#include "CDynamicObject.h"
#include "GameDef.h"
#include "CPos.h"
#include "CoreObjectDefs.h"
#include "FindPathDefs.h"
#include "CDir.h"
#include "CoreObjectClientDefs.h"
#include "CDistortedTick.h"
#include "CEntityMallocObject.h"
#include "CVariantHolderDef.h"

namespace sqr
{
	class CCoreObjectFollower;
	class CCoreSceneClient;
	class CRenderObject;
	class CSyncVariantClient;
	class CPixelPath;
	class CSyncVariantClientHolder;
}

class CEntityClientManager;
class CFighterFollower;
class CActorClient;
class CFacingMgr;
class CRenderLinkMgr;

class CEntityClient : public virtual CDynamicObject , public CEntityMallocObject
{
	friend class CEntityClientManager;
	friend class CCharacterDirector;
	friend class CRenderLinkMgr;
	friend class CIntObjClient;
protected:
	CEntityClient();
	virtual ~CEntityClient();
	template <class Traits, class AdapterType>
	void Init(CCoreObjectFollower *pCoreObj, uint32 uEntityID, AdapterType *pAdapter);

public:
	bool CoreObjisExist()const{return m_pCoreObj!=NULL;}
	static CEntityClient* GetSelectedEntity(CCoreSceneClient* pCoreScene);
	virtual EGameEntityType GetGameEntityType()const { return eGET_GameEntity; }		
	uint32 GetEntityID()const{return m_uEntityID;}
	uint32 GetLocalID()const;
	virtual EClientObjectType GetType()const;

	virtual void OnDestroy() {};
	virtual void OnRenderObjDestroyed() {};
	void GetDirection(CDir& dir)const;
	CActorClient* GetActor() const { return m_pActor; }
	CFighterFollower* GetFighter() const { return m_pFighter; }
	CCoreSceneClient* GetScene()const;
	CRenderObject* GetRenderObject() const{return m_pRenderObj;}
	void  GetPixelPos(CFPos& PixelPos)const;
	const CFPos& GetPixelPos()const;
	void  GetGridPos(CPos& GridPos)const;
	void SetCanBeSelected(bool IsCanBeSelected);
	bool CanBeSelected()const;
	void SetLieOnTheTerrain( bool bLieOn );
	bool CanBeOnTheTerrain()const;
	void SetUpdateRenderObjPos( bool bUpdate );
	bool NeedUpdateRenderObjPos()const;

	const CSyncVariantClient* GetSyncVariant(ECodeTableIndex eIndex) const;
	const CSyncVariantClient* GetViewVariant(ECodeTableIndex eIndex = eCTI_Static) const;
	const CSyncVariantClientHolder* GetSyncVariantHolder(ECodeTableIndex eIndex) const;
	const CSyncVariantClientHolder* GetViewVariantHolder(ECodeTableIndex eIndex = eCTI_Static) const;
	uint32 GetSyncVariantID(const TCHAR* szPath) const;
	uint32 GetViewVariantID(const TCHAR* szPath) const;
	bool IsPassive(); 
	bool IsMoving();
	CPixelPath* GetClonedMovePath()const;
	bool GetCurPixelWayPoint(CFPos& PixelPos)const;
	float GetCoreSpeed()const;

	void GetServerPixelPos(CFPos& PixelPos)const;
	float GetEntityDist(const CEntityClient* pEntity)const;
	float GetEntityDistInGrid(const CEntityClient* pEntity)const;
	void LogEntityMsg(const string& log);

	void RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc);
	void UnRegistDistortedTick(CDistortedTick* pTick);
	uint64 GetDistortedProcessTime();
	uint64 GetDistortedFrameTime();
	uint64 GetDistortedServerFrameTime();
	IDistortedTime* GetDistortedTime()const;

	void SetBottomSize(float fBottomSize);
	float GetBottomSize()const;

private:
	CCoreObjectFollower*	GetCoreObj()const {return m_pCoreObj;}
	uint32					m_uEntityID;
	uint32					m_uCoreObjID;
	CRenderObject*			m_pRenderObj;
	bool					m_bCanBeSelected;
	bool					m_bLieOnTheTerrain;
	bool					m_bUpdateRenderObjPos;
	float					m_fBottomSize;		//½Åµ×¹âÈ¦´óÐ¡
	mutable CSyncVariantClient*	m_pVariantBottomSize;
protected:
	CCoreObjectFollower*	m_pCoreObj;
	CActorClient*				m_pActor;
	CFighterFollower*		m_pFighter;
};
