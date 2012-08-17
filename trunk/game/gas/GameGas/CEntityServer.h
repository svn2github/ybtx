#pragma once
#include "CDynamicObject.h"
#include "GameDef.h"
#include "CPos.h"
#include "CoreObjectDefs.h"
#include "FindPathDefs.h"
#include "CAoiDimFilterType.h"
#include "PatternRef.h"
#include "CConnServer.h"
#include "CSyncVariantServerHolder.h"
#include "CEntityMallocObject.h"
#include "CVariantHolderDef.h"

namespace sqr
{
	struct CDir;
	class CCoreObjectDictator;
	class CCoreSceneServer;
	class ISend;
	class CSyncVariantServer;
	class CSynConnServer;
	class IWatchHandler;
	class CCoreObjTracer;
	class CCoreObjDistObserver;
	class CObjPosObservee;
	class IDistortedTime;
	class CDistortedTick;
	class CPixelPath;
}

template < class GameEntityType >
class TGameEntityManager;
class CActorServer;
class CFighterDictator;
class CMoveServerManager;
class CAoiDimMgr;
class IMoveManager;
class CBaseMoveMagicServer;
class CBulletMagicServer;

class CEntityServer 
	: public virtual CDynamicObject
	, public CEntityMallocObject
{
	friend class TGameEntityManager<CEntityServer>;
	friend class CEntityServerCreator;
	friend class IMoveManager;
	friend class CMoveServerManager;
	friend class CTransferableMagicServer;
	friend class CNpcAI;
	friend class CCharacterMediator;
	friend class CBulletMagicServer;
	friend class CAureMagicServer;
	friend class CMoveMagicServer;
	friend class CIntObjServer;
	friend class CFighterDictator;
protected:
	CEntityServer();
	virtual ~CEntityServer();
	template <class Traits, class AdapterType>
	void Init(CCoreObjectDictator *pCoreObj, uint32 uID, AdapterType *pAdapter);
	ESetSpeedResult SetCoreObjSpeed( float fSpeed);

	CFighterDictator*		m_pFighter;
public:
	virtual EGameEntityType GetGameEntityType()const { return eGET_GameEntity; }
	virtual void Release() = 0;
	void SetObjInValidByDestory();
	bool GetObjValidState();
	CActorServer* GetActor() const { return m_pActor; }
	inline CFighterDictator* GetFighter() const { return m_pFighter; }
	uint32 GetEntityID() const;
	uint32 GetGlobalID() const;
	void PostAoiMsgToSelf(const void* pContext);
	void SetEyeSight(float fEyeSight);
	void SetSecondEyeSight(float fEyeSight);
	void SetDoubleEyeSight(float fEyeSight, float fSecondEyeSight);
	void SetSize(float fSize);
	void SetKeenness(float fKeenness);
	virtual void SetStealth(float fStealth);
	void SetViewGroup(uint32 uGroup);
	void SetEyeSightType(EAoiItemEyeSightType eType);
	void SetSizeType(EAoiItemSizeType eType);
	void SetAoiType(EAoiItemEyeSightType eEyeSightType, EAoiItemSizeType eSizeType);
	void SetObjectAoiType(EObjectAoiType eObjectAoiType);
	void UniteObjectAoiType(EObjectAoiType eObjectAoiType);
	void InitAoiType(EAoiItemEyeSightType eEyeSightType, EAoiItemSizeType eSizeType);
	void StateSetSize();
	void StateSetBottomSize();
	virtual double GetStateZoomRate() = 0;
	//float GetBaseSize();
	//float GetBaseBarrierSize();
	virtual void OnTransfered(CCoreSceneServer* pOldScene,const CFPos& OldPos) {};
	float GetEyeSight()const;
	float GetSecondEyeSight()const;
	float GetSize()const;
	float GetKeenness()const;
	float GetStealth()const;
	CCoreSceneServer* GetScene() const;
	void GetGridPos(CPos& GridPos)const;
	ESetPosResult SetGridPos(const CPos& GridPos);
	void GetPixelPos(CFPos& PixelPos)const;
	const CFPos& GetPixelPos()const;
	ISend* GetIS(uint32 uRange)const;
	bool IsMoving()const;
	ESetPosResult SetPixelPos(const CFPos& PixelPos);

	EMoveToResult MoveToInLinePath(const CFPos& PixelPosDes,float fPixelSpeed,EFindPathType eFindPathType,EBarrierType eBarrierType);
	EMoveToResult MoveToInAStarPath(const CFPos& PixelPosDes, float fPixelSpeed,EFindPathType eFindPathType, EBarrierType eBarrierType, float fReachDist, int32 iMaxStep);
	EMoveToResult MoveToInCustomPath(const vector<CFPos>& vecPos, float fPixelSpeed, float fReachDist);

	EStopMovingResult	StopMoving();
	

	void PostCoreEvent(const void* pCoreEvent);

	CSyncVariantServer* GetSyncVariant(ECodeTableIndex eIndex = eCTI_Static) const;
	CSyncVariantServer* GetViewVariant(ECodeTableIndex eIndex = eCTI_Static) const;
	CSyncVariantServerHolder* GetSyncVariantHolder(ECodeTableIndex eIndex = eCTI_Static) const;
	CSyncVariantServerHolder* GetViewVariantHolder(ECodeTableIndex eIndex = eCTI_Static) const;

	bool	IsInViewAoi(uint32 uGlobalID);
	uint32  GetAoiMediatorNum();

	void SetDynamicBarrier(EBarrierType eBarrierType);

	//Mediator
	CConnServer* GetConnection() const;
	void SetConnection( CConnServer* pConn );
	bool DirectorMovingIsDisabled()const;
	void DisableDirectorMoving(bool bActive);
	void SetDirectorMaxSpeed(float fDirectorMaxSpeed);

	CMoveServerManager* GetMoveManager();
	CBaseMoveMagicServer* GetMoveMagic();
	bool IsValid(const CMoveServerManager* pMoveMgrKey);
	bool IsExistMoveMgr();

	float GetEntityDist(const CEntityServer* pEntity)const;
	float GetEntityDistInGrid(const CEntityServer* pEntity)const;
	void SetBarrierSize(float fSize);
	void LogEntityMsg(const string& log);

	void EnableFollowerDelay(bool bEnable);	
	int32 GetFollowerDelay()const;
	void InitPlaySizeForNpcSleepDim();	//用于Npc休眠层被Npc看到的Size，在主角的构造函数里面初始化
	void InitNpcEyeSizeForNpcSleepDim(bool bSleep); //用于Npc休眠层Npc看到主角的EyeSize设置，在NpcAI的构造函数中初始化

	void Watch(IWatchHandler* pHandler, CEntityServer* pTarget, float fPixelDistThreshold);
	void StopWatching(IWatchHandler* pHandler);
	void Trace(CEntityServer* pTarget, float fSpeed
		, EFindPathType eFindPathType, EBarrierType eBarrierType, float fReachDist);
	void StopTracing();

	uint32 GetLastMoveID();
	bool IsTracing();

	void RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc);
	void UnRegistDistortedTick(CDistortedTick* pTick);
	uint64 GetDistortedProcessTime();
	uint64 GetDistortedFrameTime();

	bool BindTo(CEntityServer* pEntity);
	void DisbindAllChildren();

	void SetBottomSize(float fBottomSize);
	float GetBottomSize()const;

	CSyncVariantServer* GetViewAttributeVariant()const;
	CSyncVariantServer* GetViewShowInClientVariant()const;

private:
	CCoreObjectDictator* GetCoreObj() const { return m_pCoreObj; }
	uint32 GetCoreObjID();
	void IntSetBottomSize(float fBottomSize);
	uint32					m_uEntityID;
	CCoreObjectDictator*	m_pCoreObj;
	CActorServer*			m_pActor;
	CAoiDimMgr*				m_pAoiDimMgr;
	TPtBiRef<CEntityServer, IMoveManager>	m_RefsByMoveMgr;
	uint32					m_uSetActiveCounter;
	bool					m_bMoveManagerHaveOwner;
	float					m_fBaseSize;
	float					m_fBaseBottomSize;
	bool					m_bValid;
	float					m_fBottomSize;
	CSyncVariantServer*		m_pVariantBottomSize;
	mutable CSyncVariantServer*		m_pVariantAttribute;
	mutable CSyncVariantServer*		m_pVariantShowInClient;

};
